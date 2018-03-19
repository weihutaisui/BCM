/* ====================================================================
 * <:label-BRCM:1997:NONE:standard
 * 
 * :>
 * Copyright (c) 1997 - 2000
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
 *    Also acknowledged are:
 *    - The Simple group of the University of Twente,
 *          http://wwwsnmp.cs.utwente.nl/
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

#include	<stdio.h>
#include        <sys/socket.h>
#include	<sys/types.h>
#include        <netinet/in.h>
#include	<strings.h>
#include "asn1.h"
#include "snmp.h"
#include "agtEngine.h"
#include "snmpCommunity.h"
#include "cms_util.h"

Community_t communities[ MAX_COMMUNITIES ] = {
        {"", 0, 0}, {"", 0, 0}, {"", 0, 0}} ;

char    *set_community(char *arg, int type)
{
  if ((arg == NULL) || (type < READ_COMMUNITY) || (type > TRAP_COMMUNITY))
    return NULL;

  cmsLog_debug("arg(%s), type(%d)", arg, type);

  strcpy(communities[type].string, arg);
  communities[type].length = strlen(arg);

  return communities[type].string;
}

/* set default community strings */
void ensure_communities(void)
{
  cmsLog_debug("Setting read(%s), write(%s), and trap(%s) communities",
               DEFAULT_COMM, DEFAULT_WRITE_COMM, DEFAULT_TRAP_COMM);

  strcpy(communities[READ_COMMUNITY].string, DEFAULT_COMM);
  communities[READ_COMMUNITY].length = strlen(DEFAULT_COMM);
  communities[READ_COMMUNITY].number = READ_COMMUNITY;
  strcpy(communities[WRITE_COMMUNITY].string, DEFAULT_WRITE_COMM);
  communities[WRITE_COMMUNITY].length = strlen(DEFAULT_WRITE_COMM);
  communities[WRITE_COMMUNITY].number = WRITE_COMMUNITY;
  strcpy(communities[TRAP_COMMUNITY].string, DEFAULT_TRAP_COMM);
  communities[TRAP_COMMUNITY].length = strlen(DEFAULT_TRAP_COMM);
  communities[TRAP_COMMUNITY].number = TRAP_COMMUNITY;
}


unsigned char   *snmp_parse_community(unsigned char *data, int *length,
                                      snmp_info_t *mesg)
{
    unsigned char       type;
    int     i;
    int communityType;
    char *in_data = NULL, *ptr;
    u_char mesgPduType=0;
    int len;
    
    data = asn_parse_string(data, length, &type, 
                            mesg->community, &(mesg->community_length));

    if (data == NULL) {
      return NULL;
    }

    cmsLog_debug("mesg->community(%s)", mesg->community);

    /* need to find out the pdu type, so peeking into it now */
    len = *length;
    in_data = (char*)malloc(len);
    memcpy((char*)in_data,(char*)data,len);
    ptr = asn_parse_header(in_data, &len, (unsigned char*)&mesgPduType);
    free(in_data);

    if (mesgPduType == SNMP_SET_REQ_PDU) 
      communityType = WRITE_COMMUNITY;
    else if (mesg->pdutype == SNMP_TRP2_REQ_PDU)
      communityType = TRAP_COMMUNITY;
    else
      communityType = READ_COMMUNITY;

    if ((mesg->community_length == communities[communityType].length)
        && !(memcmp((char *)mesg->community, communities[communityType].string,
                    mesg->community_length))) {
      mesg->community_id = communities[communityType].number;
      return (unsigned char *)data;
    }
    return NULL;
}

