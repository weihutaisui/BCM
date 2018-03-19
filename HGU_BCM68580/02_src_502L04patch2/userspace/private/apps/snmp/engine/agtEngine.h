/* ====================================================================
 * <:label-BRCM:1997:NONE:standard
 * 
 * :>
 * Copyright (c) 1997, 1998,
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
 * be found on http://www-musiq.jrc.it/~harrie/SMASH/.
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
 *    Also acknowledged are:
 *    - The Simple group of the University of Twente,
 *          http://wwwsnmp.cs.utwente.nl/
 *    - The MUSIQ workpackage of the DESIRE project,
 *          http://www-musiq.jrc.it/
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

#ifndef _AGT_ENGINE_H_
#define _AGT_ENGINE_H_

#include "agtMib.h"
#include "snmp.h"
#include <netinet/in.h>

typedef struct snmp_info_s {
    long        version;
    unsigned char      pdutype;

    /* Access identification info */
    union {
        /* SNMPv1 and SNMPv2c community based info */
        struct {
            char                        string[ 64 ];
            int                         length;
            int                         number;
        } community_based;
    } access_info;

    /* message variables */
    long        request_id; /* Identifying Request number */
} snmp_info_t;

#define community           access_info.community_based.string
#define community_length    access_info.community_based.length
#define community_id        access_info.community_based.number

#define ADSL_EOC_HDR            {0xff, 0x03, 0x81, 0x4c}
#define ADSL_EOC_HDR_LEN        4
#define ADSL_EOC_ENABLE         {0xff, 0x03, 0x00, 0x57}

#define IP_ADDR_LEN             4

typedef void (*SNMP_SET_HANDLER) (unsigned long param);

typedef struct {
    snmp_info_t mesg;

    /* originator address */
    struct sockaddr_in  originatorAddress;
    int                 originatorAddressLength;

    Oid             trap_enterprise_oid;
    unsigned char   trap_agent_ip_addr[IP_ADDR_LEN];
    unsigned long   trap_type;
    unsigned long   trap_specific_type;
    unsigned long   trap_timestamp;

    /* The SNMP request (BER-encoded data) */
    int             request_length;
    unsigned char   request[ SNMP_MAX_MSG_LENGTH ];
    unsigned char   *request_varbind_sequence;
    int             request_varbind_sequence_length;

    /* The SNMP response (BER-encoded data) */
    unsigned char   *response_version;
    unsigned char   *response_pdu;
    unsigned char   *response_request_id;
    unsigned char   *response_error_index;
    unsigned char   *response_error_status;
    unsigned char   *response_varbind_sequence;
    unsigned char   *response_first_varbind;
    unsigned char   *response_packet_end;
    int             response_length;
    unsigned char   response_adsl_eoc_hdr[ADSL_EOC_HDR_LEN];
    unsigned char   response[ SNMP_MAX_MSG_LENGTH /* * 2 */ ];

    SNMP_SET_HANDLER set_handler_func;
    unsigned long set_handler_param;
} raw_snmp_info_t;

typedef struct snmp_delay_op
{
  SNMP_SET_HANDLER funcPtr;
  unsigned long param;
} SNMP_DELAY_OP, *pSNMP_DELAY_OP;

int     snmp_open_connection(int snmp_socket);

#include "agtTransport.h"
int snmp_process_message(pTRANSPORT_INFO transport);

/*
 * Error codes which must not clash with SNMP error codes (all positive).
 */
#define CREATE_IDENTICAL    -1
#define PARSE_ERROR -2
#define BUILD_ERROR -3
#define GEN_ERROR   -4

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif


//#define DEBUG


#ifdef BUILD_SNMP_DEBUG
#include "snmpDebug.h"
#else
#define PRINT_ERROR(string)
#endif

#define INCR_PARSE_ERRORS       snmpInASNParseErrs_instance++

#define RETURN_ON_PARSE_ERROR(value, s, rettype)    if (value == NULL) { \
                        INCR_PARSE_ERRORS; \
                        PRINT_ERROR(s); \
                        return(rettype PARSE_ERROR); \
                    }

#define RETURN_ON_BUILD_ERROR(value, s) if (value == NULL) { \
                        PRINT_ERROR(s); \
                        return (int)NULL; \
                    }
#define RETURN_ON_ERROR(value, s)   if (value == NULL) { \
                        PRINT_ERROR(s); \
                        return (int)NULL; \
                    }
#define RETURN_VOID_ON_ERROR(value, s)   if (value == NULL) { \
                        PRINT_ERROR(s); \
                        return; \
                    }


unsigned char   *
snmp_parse_varbind(register unsigned char *data, Oid *var_oid,
                unsigned char *var_val_type, int *var_val_len, unsigned char **var_val,
                int *listlength);

unsigned char *
snmp_build_varbind(register unsigned char *data, Oid *var_oid,
                unsigned char var_val_type, int var_val_len, unsigned char *var_val,
                register int *listlength);

unsigned char *build_snmp_trapv1_without_list_of_varbind(raw_snmp_info_t *pi);
unsigned char *build_snmp_response_without_list_of_varbind(raw_snmp_info_t *pi);
int snmp_send_trap(raw_snmp_info_t *message);
void snmp_set_trap_addr(unsigned char *trap_addr);
int correct_snmp_response_with_lengths(raw_snmp_info_t *pi, long error_status,
                                       long error_index);
#endif /* _AGT_ENGINE_H_ */

