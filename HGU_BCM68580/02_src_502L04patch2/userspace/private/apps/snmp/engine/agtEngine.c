/* ====================================================================
 * <:label-BRCM:1997:NONE:standard
 * 
 * :>
 * Copyright (c) 1997-2000
 *                      SMASH, Harrie Hazewinkel.  All rights reserved.
 *
 * This product is developed by Harrie Hazewinkel and updates the
 * original SMUT compiler made as his graduation project at the
 * University of Twente.
 *
 * SMASH is a software package containing an SNMP MIB compiler and
 * an SNMP agent system. The package can be used for development
 * of monolithic SNMP agents and contains a compiler which compiles
 * MIB definitions into C-code to developed an SNMP agent.
 * More information about him and this software product can
 * be found on http://www.simpleweb.org/software/packages/smash/.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by Harrie Hazewinkel"
 *
 * 4. The name of the Copyright holder must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission.
 *
 * 5. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by Harrie Hazewinkel"
 *    Also thanked are:
 *    - The Simple group of the University of Twente,
 *          http://www.simpleweb.org/
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR, ITS DISTRIBUTORS
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>

#include "asn1.h"
#include "snmp.h"
#include "agtMib.h"
#include "agtEngine.h"
#include "agtTransport.h"
#include "snmpCommunity.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

#ifdef BUILD_SNMP_DEBUG
#include "snmpDebug.h"
#endif

long    snmpInPkts_instance = 0;
long    snmpInBadVersions_instance = 0;
long    snmpInBadCommunityNames_instance = 0;
long    snmpInBadCommunityUses_instance = 0;
long    snmpInASNParseErrs_instance = 0;
long    snmpEnableAuthenTraps_instance =0;
long    snmpSilentDrops_instance = 0;
long    snmpProxyDrops_instance = 0;
static SNMP_DELAY_OP delay;

extern void dumpaddr( unsigned char *, int );
extern void printOid(char *pPrefix, Oid *pOid);

void snmp_print_message(unsigned char *msg, int msglength)
{
    int     i = 0;
    char    buf[100] = "\0";
    char   *pBuf = buf;

    cmsLog_debug("msg(%p), msglength(%d)", msg, msglength);

    if (!(msglength))
    {
        msglength = 0x10;
    }
    for (i=0; i< msglength ; i++)
    {
        if (!(i % 0x10))
        {
            cmsLog_debug("%s", buf);
            pBuf = buf;
            pBuf += snprintf(pBuf, sizeof(buf), " %02X : ", i);
        }
        pBuf += snprintf(pBuf, sizeof(buf) - strlen(pBuf), "%02X ", *msg);
        msg++;
    }
    cmsLog_debug("%s", buf);
    cmsLog_debug("");
}

/*
 * snmp_parse_version(unsigned char *snmp_data, int *length, long *version)
 * Parses SEQUENCE hedaer and first integer (should be version)
 * On success it returns a pointer to the next ASN.1 field
 * If it fails it returns NULL.
 * The SNMP-version is returned in the 'version'-variable
 */
unsigned char  *
snmp_parse_version(unsigned char *snmp_data, int *length, long *version)
{
    unsigned char    type;
    snmp_data = asn_parse_header(snmp_data, length, &type);
    if (snmp_data == NULL)
    {
        PRINT_ERROR("bad header");
        return NULL;
    }
    if (type != (ASN_SEQUENCE | ASN_CONSTRUCTOR))
    {
        PRINT_ERROR("wrong auth header type");
        return NULL;
    }
    snmp_data = asn_parse_int(snmp_data, length, &type,
    version, sizeof(*version));
    return (unsigned char *)snmp_data;
}

void map_types_smiv2_to_smiv1(unsigned char *smi_type)
{

    /* This is empty, but need to be sorted out. */
}

void map_errorstatus_snmpv2_to_snmpv1(long *error_status)
{
    cmsLog_debug("error_status(%p)", error_status);

    switch (*error_status) 
    {

    case SNMP_ERROR_WRONGTYPE:
    case SNMP_ERROR_WRONGLENGTH:
    case SNMP_ERROR_WRONGENCODING:
    case SNMP_ERROR_WRONGVALUE:
    case SNMP_ERROR_INCONSISTENTVALUE:
        *error_status = SNMP_ERROR_BADVALUE;
        break;

    case SNMP_ERROR_NOACCESS:
    case SNMP_ERROR_NOTWRITABLE:
    case SNMP_ERROR_NOCREATION:
    case SNMP_ERROR_INCONSISTENTNAME:
    case SNMP_ERROR_AUTHORIZATIONERROR:
        *error_status = SNMP_ERROR_NOSUCHNAME;
        break;

    case SNMP_ERROR_RESOURCEUNAVAILABLE:
    case SNMP_ERROR_COMMITFAILED:
    case SNMP_ERROR_UNDOFAILED:
        *error_status = SNMP_ERROR_GENERR;
        break;

    default:
        break;
    }
}

int
goodValue( unsigned char inType, int inLen,
    unsigned char actualType, int actualLen)
{
    if (inLen > actualLen)
        return FALSE;
    return (inType == actualType);
}


/*
 * snmp_parse_list_of_varbindings(
 *    unsigned char *data, int length,
 *    unsigned char *out_data, int out_length,
 *   long non_repeaters, long *error_index,
 *   raw_snmp_info_t *raw_mesg, int action)
 *
 * NOTE: error_index CONTAINS in the beginning the max_repetitions!!!!!!!
 */
int
snmp_parse_list_of_varbindings(
    unsigned char *data, int length,
    unsigned char *out_data, int out_length,
    long non_repeaters, long *error_index,
    raw_snmp_info_t *raw_mesg, int action)
{
    long            max_repetitions = *error_index;
    unsigned char       *out_data_save;
    int         out_length_save;
    Oid         var_oid[32 /*10*/];
    int         var_val_len;
    unsigned char       var_val_type, *var_val, statType;
    register unsigned char  *statP;
    int         statLen;
    unsigned short      acl;
    int         searchType, err;
    WRITE_METHOD write_method;
    int         noSuchObject;
    int         msg_full = FALSE;
    int         repeaterIndex;
    int         number_of_repeaters;
    snmp_info_t     *mesg = &(raw_mesg->mesg);

    cmsLog_debug("data(%p), length(%d), non_repeaters(%d), action(%d), mesg->pdutype(0x%X)",
                 data, length, non_repeaters, action, mesg->pdutype);

    if ((mesg->pdutype == SNMP_GETNEXT_REQ_PDU) ||
        (mesg->pdutype == SNMP_GETBULK_REQ_PDU))
    {
        searchType = NEXT;
    }
    else
    {
        searchType = EXACT;
    }
    *error_index = 1;
    while(((int)length > 0) && (non_repeaters > 0))
    {
        var_oid[0].namelen = MAX_OID_LEN;
        data = snmp_parse_varbind(data, &(var_oid[0]), &var_val_type,
            &var_val_len, &var_val, (int *)&length);
        if (data == NULL)
            return PARSE_ERROR;
        write_method = NULL;    
        statP = getStatPtr(&(var_oid[0]), &statType, &statLen, &acl, 
            searchType, &write_method, mesg, &noSuchObject);

        cmsLog_debug("statP(%p), statType(%u), statLen(%d), acl(%u), write_method(%p), noSuchObject(%d)",
                     statP, statType, statLen, acl, write_method, noSuchObject);

        if ( mesg->pdutype == SNMP_SET_REQ_PDU )
        {
            if (!(acl & RWRITE))
            {
                return(SNMP_ERROR_NOTWRITABLE);
            }
            if (write_method == NULL)
            {
                if (statP == NULL)
                {
                    return(SNMP_ERROR_NOTWRITABLE);
                }
                else
                {
                    if (!goodValue(var_val_type, var_val_len, statType, statLen))
                    {
                        return(SNMP_ERROR_WRONGTYPE);
                    }
                    return(SNMP_ERROR_NOTWRITABLE);
                }
            }
            else
            {
                err = (*write_method)(action, var_val, var_val_type,
                    var_val_len, statP, &(var_oid[0]),
                    &raw_mesg->set_handler_func, &raw_mesg->set_handler_param);
                if (err != SNMP_ERROR_NOERROR)
                {
                    cmsLog_error("write_method returned %d", err);
                    return(err);
                }
            }
        }
        else
        {
            if (statP == NULL)
            {
                statLen = 0;
                if (searchType == EXACT)
                {
                    if (noSuchObject == TRUE)
                    {
                        statType = SNMP_NOSUCHOBJECT;
                    }
                    else
                    {
                        statType = SNMP_NOSUCHINSTANCE;
                    }
                }
                else
                {
                    statType = SNMP_ENDOFMIBVIEW;
                }
            }
            if (mesg->version == SNMP_VERSION_1)
            {
                map_types_smiv2_to_smiv1(&statType);
            }
            out_data = snmp_build_varbind(out_data, &(var_oid[0]),
                statType, statLen, statP, &out_length);
            if (out_data == NULL)
            {
                return SNMP_ERROR_TOOBIG;
            }
        }
        (*error_index)++;
        non_repeaters--;
    }
 
    /* Now we start working on the repeater list */
    for (number_of_repeaters = 0;
        ((int)length > 0) && !msg_full && number_of_repeaters < 10 ;
        number_of_repeaters++)
    {
        var_oid[number_of_repeaters].namelen = MAX_OID_LEN;
        data = snmp_parse_varbind(data, &(var_oid[number_of_repeaters]),
            &var_val_type, &var_val_len, &var_val, (int *)&length);
        RETURN_ON_PARSE_ERROR(data, "", (int));
        statP = getStatPtr(&(var_oid[number_of_repeaters]), &statType, &statLen,
            &acl, NEXT, &write_method, mesg, &noSuchObject);
        if (statP == NULL)
        {
            statType = SNMP_ENDOFMIBVIEW;
            statLen = 0;
        }
        out_data_save = out_data;
        out_length_save = out_length;
        if (mesg->version == SNMP_VERSION_1)
        {
            map_types_smiv2_to_smiv1(&statType);
        }
        out_data = snmp_build_varbind(out_data, &(var_oid[number_of_repeaters]),
            statType, statLen, statP, &out_length);
        if (statType == SNMP_ENDOFMIBVIEW)
        {
            var_oid[number_of_repeaters].namelen = 0;
        }
        if (out_data == NULL)
        {
            out_data = out_data_save;
            out_length = out_length_save;
            msg_full = TRUE;
        }
    }
    while (--max_repetitions > 0)
    {
        for (repeaterIndex = 0; repeaterIndex < number_of_repeaters && !msg_full;
            repeaterIndex++)
        {
            if (var_oid[repeaterIndex].namelen != 0)
            {
                statP = getStatPtr(&(var_oid[repeaterIndex]), &statType,
                    &statLen, &acl, NEXT, &write_method, mesg, &noSuchObject);
                if (statP == NULL)
                    statType = SNMP_ENDOFMIBVIEW;
                out_data_save = out_data;
                out_length_save = out_length;
                if (mesg->version == SNMP_VERSION_1)
                {
                    map_types_smiv2_to_smiv1(&statType);
                }
                out_data = snmp_build_varbind(out_data,&(var_oid[repeaterIndex]),
                    statType, statLen, statP, &out_length);
                if (statType == SNMP_ENDOFMIBVIEW)
                {
                    var_oid[repeaterIndex].namelen = 0;
                }
                if (out_data == NULL)
                {
                    out_data = out_data_save;
                    out_length = out_length_save;
                    msg_full = TRUE;
                }
            }
        }
    }
    raw_mesg->response_packet_end = out_data;
    *error_index = 0;
    return SNMP_ERROR_NOERROR;
}


unsigned char *build_snmp_response_without_list_of_varbind(raw_snmp_info_t *pi)
{
    unsigned char *out_data = pi->response;
    int out_length = SNMP_MAX_MSG_LENGTH;
    long zero = 0;

    cmsLog_debug("pi(%p)", pi);

    out_data = asn_build_sequence(out_data, &out_length,
        (unsigned char)(ASN_SEQUENCE | ASN_CONSTRUCTOR), 0);
    RETURN_ON_BUILD_ERROR (out_data, "build sequence header");
    pi->response_version = out_data;

    out_data = asn_build_int(out_data, &out_length,
        (unsigned char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER),
        &(pi->mesg.version), sizeof(pi->mesg.version));
    RETURN_ON_BUILD_ERROR (out_data, "build snmp-version");

    out_data = asn_build_string(out_data, &out_length,
        (unsigned char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_OCTET_STR),
        pi->mesg.community, pi->mesg.community_length);
    RETURN_ON_BUILD_ERROR (out_data, "build snmp-community");
    pi->response_pdu = out_data;

    out_data = asn_build_sequence(out_data, &out_length,
        (unsigned char)SNMP_GET_RSP_PDU, 0);
    pi->response_request_id = out_data;

    out_data = asn_build_int(out_data, &out_length, 
        (unsigned char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER),
        &(pi->mesg.request_id), sizeof(pi->mesg.request_id));
    RETURN_ON_BUILD_ERROR(out_data, "build request-id");
    pi->response_error_status = out_data;

    out_data = asn_build_int(out_data, &out_length, 
        (unsigned char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER),
        &zero, sizeof(zero));
    RETURN_ON_BUILD_ERROR(out_data, "build error_status");
    pi->response_error_index = out_data;

    out_data = asn_build_int(out_data, &out_length, 
        (unsigned char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER),
        &zero, sizeof(zero));
    RETURN_ON_BUILD_ERROR(out_data, "build error_index");
    pi->response_varbind_sequence = out_data;

    out_data = asn_build_sequence(out_data, &out_length,
        (unsigned char)(ASN_SEQUENCE | ASN_CONSTRUCTOR), 0);
    RETURN_ON_BUILD_ERROR (out_data, "build sequence header");

    return (unsigned char *)out_data;
}

unsigned char *build_snmp_trapv1_without_list_of_varbind(raw_snmp_info_t *pi)
{
    unsigned char *out_data = pi->response;
    int out_length = SNMP_MAX_MSG_LENGTH;

    out_data = asn_build_sequence(out_data, &out_length,
        (unsigned char)(ASN_SEQUENCE | ASN_CONSTRUCTOR), 0);
    RETURN_ON_BUILD_ERROR (out_data, "build sequence header");
    pi->response_version = out_data;

    out_data = asn_build_int(out_data, &out_length,
        (unsigned char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER),
        &(pi->mesg.version), sizeof(pi->mesg.version));
    RETURN_ON_BUILD_ERROR (out_data, "build snmp-version");

    out_data = asn_build_string(out_data, &out_length,
        (unsigned char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_OCTET_STR),
        pi->mesg.community, pi->mesg.community_length);
    RETURN_ON_BUILD_ERROR (out_data, "build snmp-community");
    pi->response_pdu = out_data;

    out_data = asn_build_sequence(out_data, &out_length,
        (unsigned char)SNMP_TRP_REQ_PDU, 0);
    pi->response_request_id = out_data;

    out_data = asn_build_objid(out_data, &out_length,
        (unsigned char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_OBJECT_ID),
        pi->trap_enterprise_oid.name, pi->trap_enterprise_oid.namelen);

    out_data = asn_build_string(out_data, &out_length, SNMP_IPADDRESS,
        pi->trap_agent_ip_addr, sizeof(pi->trap_agent_ip_addr));

    out_data = asn_build_int(out_data, &out_length, 
        (unsigned char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER),
        &pi->trap_type, sizeof(pi->trap_type));

    out_data = asn_build_int(out_data, &out_length, 
        (unsigned char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER),
        &pi->trap_specific_type, sizeof(pi->trap_specific_type));

    out_data = asn_build_int(out_data, &out_length, 
        (unsigned char)SNMP_TIMETICKS,
        &pi->trap_timestamp, sizeof(pi->trap_timestamp));
    pi->response_varbind_sequence = out_data;

    out_data = asn_build_sequence(out_data, &out_length,
        (unsigned char)(ASN_SEQUENCE | ASN_CONSTRUCTOR), 0);
    RETURN_ON_BUILD_ERROR (out_data, "build sequence header");

    return (unsigned char *)out_data;
}

int correct_snmp_response_with_lengths(raw_snmp_info_t *pi, long error_status,
    long error_index)
{
    unsigned char *out_data;
    int out_length;

    cmsLog_debug("pi(%p), error_status(%d)", pi, error_status);

    if (error_status == SNMP_ERROR_NOERROR)
    {
        out_length = 4;
        out_data = asn_build_sequence(pi->response_varbind_sequence, &out_length,
            (unsigned char)(ASN_SEQUENCE | ASN_CONSTRUCTOR),
            ((pi->response_packet_end - pi->response_varbind_sequence) - 4));
        RETURN_ON_BUILD_ERROR(out_data, "correction error varbind length");
    }
    else
        if (error_status == SNMP_ERROR_TOOBIG)
        {
            if (pi->mesg.version == SNMP_VERSION_1)
            {
                cmsLog_error("Cannot return SNMP_TOOBIG in V1");
                return(0);
            }
            error_index = 0; /* Just to make sure it is '0' */
            pi->response_packet_end = pi->response_varbind_sequence + 4;
            out_length = 3;
            out_data = asn_build_int(pi->response_error_status, &out_length, 
                (unsigned char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER),
                &error_status, sizeof(error_status));
            RETURN_ON_BUILD_ERROR(out_data, "build error_status");
            out_length = 3;
            out_data = asn_build_int(pi->response_error_index, &out_length, 
                (unsigned char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER),
                &error_index, sizeof(error_index));
            RETURN_ON_BUILD_ERROR(out_data, "build error_index");
        }
        else
            if (error_status == PARSE_ERROR)
            {
                snmpInASNParseErrs_instance++;
                return ((int)NULL);
            }
            else
            {
                /* Make varbindlist identical.
                 * This is done for the errors as well the SNMP SET
                 * (error_status==CREATE_IDENTICAL)
                 */
                if (error_status == CREATE_IDENTICAL)
                {
                    error_status = 0;
                }
                memcpy(pi->response_varbind_sequence,
                    pi->request_varbind_sequence,
                    pi->request_varbind_sequence_length);
                pi->response_packet_end = pi->response_varbind_sequence +
                    pi->request_varbind_sequence_length;
                out_length = 3;
                out_data = asn_build_int(pi->response_error_status, &out_length, 
                    (unsigned char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER),
                    &error_status, sizeof(error_status));
                RETURN_ON_BUILD_ERROR(out_data, "build error_status");
                out_length = 3;
                out_data = asn_build_int(pi->response_error_index, &out_length, 
                    (unsigned char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER),
                    &error_index, sizeof(error_index));
                RETURN_ON_BUILD_ERROR(out_data, "build error_index");
            }
    out_length = 4;
    out_data = asn_build_sequence(pi->response, &out_length,
        (unsigned char)(ASN_SEQUENCE | ASN_CONSTRUCTOR),
        (pi->response_packet_end - pi->response_version));
    RETURN_ON_BUILD_ERROR(out_data, "correction error message sequence");
    out_length = 4;
    out_data = asn_build_sequence(pi->response_pdu, &out_length,
        SNMP_GET_RSP_PDU, pi->response_packet_end - pi->response_request_id);
    RETURN_ON_BUILD_ERROR(out_data, "correction error request-id");
    pi->response_length = pi->response_packet_end - pi->response;
    return pi->response_length;
}

int snmp_parse_message(raw_snmp_info_t *pi)
{
    unsigned char *in_data = pi->request;
    int in_length = pi->request_length;
    unsigned char *out_data = pi->response;
    int out_length = SNMP_MAX_MSG_LENGTH;
    char type;
    long error_status, error_index, dummyindex = 0;

    cmsLog_debug("pi(%p)", pi);

    snmpInPkts_instance++;

    in_data = asn_parse_header(in_data, &in_length, &type);
    RETURN_ON_PARSE_ERROR(in_data, "bad header", (int));
    if (type != (ASN_SEQUENCE | ASN_CONSTRUCTOR))
    {
        PRINT_ERROR("wrong header type");
        return (int)NULL;
    }
    in_data = asn_parse_int(in_data, &in_length, &type,
        &(pi->mesg.version), sizeof(pi->mesg.version));
    RETURN_ON_PARSE_ERROR(in_data, "", (int));
    switch (pi->mesg.version) 
    {

    case SNMP_VERSION_1 :
    case SNMP_VERSION_2C    :
        pi->mesg.community_length = MAX_COMMUNITY_LEN;
        in_data = snmp_parse_community(in_data, &in_length, &(pi->mesg));
        if (in_data == NULL) {
          snmpInBadCommunityNames_instance++;
          RETURN_ON_PARSE_ERROR(in_data, "bad community name", (int));
        }
        break;  

    case SNMP_VERSION_2 :
    default         :
        PRINT_ERROR("Wrong version");
        return(0);
    }
    in_data = asn_parse_header(in_data, &in_length, &pi->mesg.pdutype);
    RETURN_ON_PARSE_ERROR(in_data, "asn_parse_header for pdutype error", (int));

    cmsLog_debug("version(%d), pdutype(0x%X), community(%s)",
                 pi->mesg.version, pi->mesg.pdutype, pi->mesg.access_info.community_based.string);

    switch (pi->mesg.pdutype) 
    {
    case SNMP_GETBULK_REQ_PDU   :
        if (pi->mesg.version == SNMP_VERSION_1)
        {
            PRINT_ERROR("Version 1 does not support a GETBULK");
            return(0);
        }

    case SNMP_GET_REQ_PDU   :
    case SNMP_GETNEXT_REQ_PDU   :
    case SNMP_SET_REQ_PDU   :
        in_data = asn_parse_int(in_data, &in_length, &type,
            &(pi->mesg.request_id), sizeof(pi->mesg.request_id));
        RETURN_ON_PARSE_ERROR(in_data, "parse request_id error", (int));
        in_data = asn_parse_int(in_data, &in_length, &type, &error_status,
            sizeof(error_status));
        RETURN_ON_PARSE_ERROR(in_data, "parse type error", (int));
        in_data = asn_parse_int(in_data, &in_length, &type, &error_index,
            sizeof(error_index));
        RETURN_ON_PARSE_ERROR(in_data, "parse error index error", (int));
        break;

    default         :
        PRINT_ERROR("Message type is not supported");
        return(0);
    }
    pi->request_varbind_sequence = in_data;
    pi->request_varbind_sequence_length = in_length;
    in_data = asn_parse_header(in_data, &in_length, &type);
    RETURN_ON_PARSE_ERROR(in_data, "not enough space for varlist", (int));
    if (type != (unsigned char)(ASN_SEQUENCE | ASN_CONSTRUCTOR))
    {
        PRINT_ERROR("wrong type");
        return PARSE_ERROR;
    }
    out_data = build_snmp_response_without_list_of_varbind(pi);
    RETURN_ON_BUILD_ERROR(out_data, "build error");
    switch (pi->mesg.pdutype) 
    {

    case SNMP_GETBULK_REQ_PDU:
        error_status = snmp_parse_list_of_varbindings(in_data, in_length,
            out_data, out_length, error_status, &error_index, pi, RESERVE1);
        break;

    case SNMP_GET_REQ_PDU   :
    case SNMP_GETNEXT_REQ_PDU   :
        error_status = snmp_parse_list_of_varbindings(in_data, in_length,
            out_data, out_length, 9999, &error_index, pi, RESERVE1);
        break;

    case SNMP_SET_REQ_PDU :
        error_status = snmp_parse_list_of_varbindings(in_data, in_length,
            out_data, out_length, 9999, &error_index, pi, RESERVE1);
        if (error_status == SNMP_ERROR_NOERROR)
        {
           error_status = snmp_parse_list_of_varbindings(in_data, in_length,
                out_data, out_length, 9999, &error_index, pi, RESERVE2);
        }
        if (error_status == SNMP_ERROR_NOERROR)
        {
           error_status = snmp_parse_list_of_varbindings(in_data, in_length,
                out_data, out_length, 9999, &error_index, pi, ACTION);
        }
        if (error_status == SNMP_ERROR_NOERROR)
        {
            error_status = snmp_parse_list_of_varbindings(in_data, in_length,
                out_data, out_length, 9999, &error_index, pi, COMMIT);
        }
        if (error_status == SNMP_ERROR_NOERROR)
        {
           return(correct_snmp_response_with_lengths(pi, CREATE_IDENTICAL, 0));
        }
        else
        {
            snmp_parse_list_of_varbindings(in_data, in_length,
                out_data, out_length, 9999, &dummyindex, pi, FREE);
        }
        break;

    default :
        /* This should be not neccessary, because this is earlier checked. */
        PRINT_ERROR("Message type is not supported");
        return(0);
    }
    if (pi->mesg.version == SNMP_VERSION_1)
    {
        map_errorstatus_snmpv2_to_snmpv1(&error_status);
    }
    return(correct_snmp_response_with_lengths(pi, error_status, error_index));
}

void snmpDoDelayOp(int junk) {
    (*delay.funcPtr)(delay.param);
}
/*
 * snmp_process_message(int socket)
 * Processes an SNMP message.
 */
int snmp_process_message(pTRANSPORT_INFO transport)
{
    CmsRet ret;
    int socket = transport->socket;
    raw_snmp_info_t message;
    
    cmsLog_debug("transport(%p)", transport);

    int byteRead=0;

    if ((byteRead = transport->receive_fn(transport,&message)) == 0) {
      return 0;
    }

    if ((ret = cmsLck_acquireLockWithTimeout(SNMP_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
    {
       cmsLog_error("failed to get lock, ret=%d", ret);
       return;
    }

#ifdef BUILD_SNMP_DEBUG
    if ((snmpDebug.level & SNMP_DEBUG_LEVEL_SNMP) == SNMP_DEBUG_LEVEL_SNMP) {
      cmsLog_debug("calling snmp_parse_message(transport socket %d,name %s, byteRead %d)",
             transport->socket,transport->name, byteRead);
    }
#endif

    /* process the SNMP request */
    if (snmp_parse_message(&message) > 0)
    {
#ifdef BUILD_SNMP_DEBUG                      
       if ((snmpDebug.level & SNMP_DEBUG_LEVEL_SNMP) == SNMP_DEBUG_LEVEL_SNMP) {
          cmsLog_debug("Return %d bytes to %s:", message.response_length,
                inet_ntoa(message.originatorAddress.sin_addr));
          snmp_print_message(message.response, message.response_length);
       }
#endif
       /* return SNMP response */

       /* should not call directly, call snmpTransportSend */
       snmpTransportSend(transport, &message);
       if( message.set_handler_func != NULL ) {
          /* wait a little bit before doing set handler */
          delay.funcPtr = (SNMP_SET_HANDLER)message.set_handler_func;
          delay.param = (unsigned long)message.set_handler_param;
          signal(SIGALRM,snmpDoDelayOp);
          alarm(1);	  
       }
    }
    cmsLck_releaseLock();
    return 1;
}

/*
 * snmp_open_connection
 * opens the transport connections used for SNMP.
 * It opens a UDP port to the network.
 */
int     snmp_open_connection(int snmp_port)
{
int                     socketDescriptor;
struct sockaddr_in      mySocketAddress;

#ifdef WIN32
WSADATA dontcare;
#endif

    cmsLog_debug("SNMP_PORT %d",snmp_port);

    if ( !snmp_port ) {
        cmsLog_error("The specified port is '0'\n");
        exit(1);
    }

#ifdef WIN32
    if (WSAStartup (0x0101,&dontcare)) {
        printf ("error calling stupid Microsoft \"WSAStartup()\" function.\n");
        exit (1);
    }
#endif

    snmpOpenTransport();
    return 1;
}

unsigned char *
snmp_parse_varbind(unsigned char *data, Oid *var, unsigned char *var_val_type,
    int *var_val_len, unsigned char **var_val, int *listlength)
{
    unsigned char   varbind_type;
    int             varbind_len = *listlength;
    unsigned char   *varbind_start = data;

    cmsLog_debug("data(%p), *listlength(%d)", data, *listlength);

    data = asn_parse_header(data, &varbind_len, &varbind_type);
    RETURN_ON_PARSE_ERROR(data, "", (unsigned char *));
    if (varbind_type != (unsigned char)(ASN_SEQUENCE | ASN_CONSTRUCTOR))
        return NULL;
    data = asn_parse_objid(data, &varbind_len, &varbind_type, var->name,
        &(var->namelen));
    RETURN_ON_PARSE_ERROR(data, "", (unsigned char *));
    if (varbind_type != (unsigned char)(ASN_UNIVERSAL|ASN_PRIMITIVE|ASN_OBJECT_ID))
        return NULL;

    /* find out what type of object this is */
    data = asn_parse_header(data, &varbind_len, var_val_type);
    RETURN_ON_PARSE_ERROR(data, "", (unsigned char *));
    *var_val_len = varbind_len;
    *var_val = data;    /* save pointer to this object */
    data += varbind_len;
    *listlength -= (int)(data - varbind_start);
    return data;
}

unsigned char *
snmp_build_varbind(data, var_oid, var_val_type, var_val_len, var_val, listlength)
    register unsigned char *data; /* IN - ptr to beginning of the output buffer*/
    Oid *var_oid;                 /* IN - object id of variable */
    unsigned char   var_val_type; /* IN - type of variable */
    int var_val_len;              /* IN - length of variable */
    unsigned char *var_val;       /* IN - value of variable */
    register int *listlength;     /* IN/OUT - valid bytes in output buffer */
{
    int         dummyLen, headerLen;
    unsigned char       *dataPtr;

    cmsLog_debug("data(%p), var_val_type(%u), var_val_len(%d), var_val(%p), *listlength(%d)",
                 data, var_val_type, var_val_len, var_val, *listlength);
    printOid("var_oid: ", var_oid);

    dummyLen = *listlength;
    dataPtr = data;
    data += 4;
    dummyLen -=4;
    if (dummyLen < 0)
        return NULL;
    headerLen = data - dataPtr;
    *listlength -= headerLen;
    data = asn_build_objid(data, listlength,
        (unsigned char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_OBJECT_ID),
        var_oid->name, var_oid->namelen);
    RETURN_ON_BUILD_ERROR (data, "cannot add objectId in varbind");
    switch(var_val_type)
    {
    case ASN_INTEGER:
        data = asn_build_int(data, listlength, var_val_type,
            (long *)var_val, var_val_len);
        break;

    case SNMP_GAUGE:
    case SNMP_COUNTER:
    case SNMP_TIMETICKS:
        /* case SNMP_UINTEGER: this is covered by SNMP_GAUGE */
        data = asn_build_unsigned_int(data, listlength, var_val_type,
            (unsigned long *)var_val, var_val_len);
        break;

    case SNMP_COUNTER64:
        data = asn_build_unsigned_int64(data, listlength, var_val_type,
            (Counter64 *)var_val, var_val_len);
        break;

    case ASN_OCTET_STR:
    case SNMP_IPADDRESS:
    case SNMP_OPAQUE:
    case SNMP_NSAP:
        data = asn_build_string(data, listlength, var_val_type,
            var_val, var_val_len);
        break;

    case ASN_OBJECT_ID:
        data = asn_build_objid(data, listlength, var_val_type,
            (oid *)var_val, var_val_len / sizeof(oid));
        break;

    case ASN_NULL:
        data = asn_build_null(data, listlength, var_val_type);
        break;

    case ASN_BIT_STR:
        data = asn_build_bitstring(data, listlength, var_val_type,
            var_val, var_val_len);
        break;

    case SNMP_NOSUCHOBJECT:
    case SNMP_NOSUCHINSTANCE:
    case SNMP_ENDOFMIBVIEW:
        data = asn_build_null(data, listlength, var_val_type);
        break;

    default:
        PRINT_ERROR("wrong type");
        return NULL;
    }
    RETURN_ON_BUILD_ERROR(data, "Wrong ASN.1 sequence built\n");
    dummyLen = (data - dataPtr) - headerLen;
    asn_build_sequence(dataPtr, &dummyLen,
        (unsigned char)(ASN_SEQUENCE | ASN_CONSTRUCTOR), dummyLen);
    return data;
}

/* IP address to send SNMP trap messages to. */
unsigned char g_trap_addr[4];

/* Sets the IP address to send the SNMP trap message to. */
void snmp_set_trap_addr( unsigned char *trap_addr )
{
   int i;
   UBOOL8 ret;

   cmsLog_debug("trap_addr string %s",trap_addr);

   ret = cmsUtl_ipStrToOctets(trap_addr,g_trap_addr);

   if (ret == TRUE)
   {
      for (i=0; i < 4; i++)
      {
         cmsLog_debug("trap_addr[%d] =  %d", i,g_trap_addr[i]);
      }
   }
   else
   {
      cmsLog_debug("trap_addr string conversion fail");
   }
}

int snmp_send_trap(raw_snmp_info_t *message)
{
  return snmpTransportSend(NULL,(void*)message);
}

//in_addr_t get_myaddr(void)
struct sockaddr_in get_myaddr(void)
{
  char            local_host[130];
  struct hostent *host;
  struct sockaddr_in in_addr;

  in_addr.sin_addr.s_addr = INADDR_ANY;

  if (gethostname(local_host, sizeof(local_host)) == 0) {
    host = gethostbyname(local_host);
    if (host) {
      in_addr.sin_addr.s_addr = *((ulong*)(host->h_addr));
      //      return ((in_addr_t) in_addr.sin_addr.s_addr);
      return (in_addr);
    }
  }

  return in_addr;
}

