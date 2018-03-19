/* ====================================================================
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
#ifndef _SNMP_COMMUNITIES_H_
#define _SNMP_COMMUNITIES_H_

#define	MAX_COMMUNITIES		3
#define MAX_COMMUNITY_LEN       64

#ifdef NEED_CHANGE
#ifdef CPE_DSL_MIB
#define DEFAULT_COMM		"ADSL"
#define DEFAULT_WRITE_COMM      "ADSL"
#define DEFAULT_TRAP_COMM       "ADSL"
#else
#define DEFAULT_COMM		"public"
#define DEFAULT_WRITE_COMM      "private"
#define DEFAULT_TRAP_COMM       "trapComm"
#endif
#else
#define DEFAULT_COMM            "public"
#define DEFAULT_WRITE_COMM      "private"
#define DEFAULT_TRAP_COMM       "trapComm"
#endif

#define READ_COMMUNITY          0
#define WRITE_COMMUNITY         1
#define TRAP_COMMUNITY          2

typedef struct {
    char                        string[MAX_COMMUNITY_LEN];
    int                         length;
    int                         number;
} Community_t;


char	*set_community(char *arg, int type);
unsigned char *snmp_parse_community(unsigned char *data, int *length,
                                    snmp_info_t *record);
void ensure_communities(void);

#endif /* _SNMP_COMMUNITIES_H_ */

