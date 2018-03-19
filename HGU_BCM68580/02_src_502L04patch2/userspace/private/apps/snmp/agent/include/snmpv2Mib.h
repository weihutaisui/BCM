/**********************************************************************************
** <:copyright-BRCM:2017:proprietary:standard
** 
**    Copyright (c) 2017 Broadcom 
**    All Rights Reserved
** 
**  This program is the proprietary software of Broadcom and/or its
**  licensors, and may only be used, duplicated, modified or distributed pursuant
**  to the terms and conditions of a separate, written license agreement executed
**  between you and Broadcom (an "Authorized License").  Except as set forth in
**  an Authorized License, Broadcom grants no license (express or implied), right
**  to use, or waiver of any kind with respect to the Software, and Broadcom
**  expressly reserves all rights in and to the Software and all intellectual
**  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
**  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
**  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
** 
**  Except as expressly set forth in the Authorized License,
** 
**  1. This program, including its structure, sequence and organization,
**     constitutes the valuable trade secrets of Broadcom, and you shall use
**     all reasonable efforts to protect the confidentiality thereof, and to
**     use this information only in connection with your use of Broadcom
**     integrated circuit products.
** 
**  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
**     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
**     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
**     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
**     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
**     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
**     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
**     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
**     PERFORMANCE OF THE SOFTWARE.
** 
**  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
**     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
**     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
**     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
**     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
**     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
**     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
**     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
**     LIMITED REMEDY.
** :>
***********************************************************************************/
#ifndef	_SNMPv2_MIB_
#define	_SNMPv2_MIB_

#define ENTERPRISE_OID  1, 3, 6, 1, 4, 1

#ifdef CPE_DSL_MIB
#define SYS_CONTACT     "China Communications Standards Association"
#define SYS_LOCATION    "UNKNOWN"
#define SYS_NAME        "UNKNOWN"
#define CPE_ENTERPRISE_OID 16972
#define SYS_OBJ         ENTERPRISE_OID, CPE_ENTERPRISE_OID
#else
#define SYS_CONTACT     "UNKNOWN"
#define SYS_LOCATION    "UNKNOWN"
#define SYS_NAME        "Broadcom"
#define BRCM_ENTERPRISE_OID 4413
#define SYS_OBJ         ENTERPRISE_OID, BRCM_ENTERPRISE_OID
#endif
#define SYS_SERVICES    72

/* required include files (IMPORTS) */
#include	"snmpv2Tc.h"
#include	"snmpv2Smi.h"

void snmpv2Free(void);
void init_SNMPv2_MIB(void);
void register_subtrees_of_SNMPv2_MIB(void);
void snmpSetSysInfo(char *sysName, char *sysLocation, char *sysContact);
void snmpStoreSysInfo(void);
unsigned char *var_sysDescr(int *var_len, Access_rec *access_id,
                            WRITE_METHOD *write_method);

/* defined objects in this module */


/* MIB object snmpMIB = snmpModules, 1 */
#define	I_snmpMIB	1
#define	O_snmpMIB	1, 3, 6, 1, 6, 3, 1

/* MIB object snmpMIBObjects = snmpMIB, 1 */
#define	I_snmpMIBObjects	1
#define	O_snmpMIBObjects	1, 3, 6, 1, 6, 3, 1, 1

/* MIB object system = mib_2, 1 */
#define	I_system	1
#define	O_system	1, 3, 6, 1, 2, 1, 1

/* MIB object sysDescr = system, 1 */
#define	I_sysDescr	1
#define	O_sysDescr	1, 3, 6, 1, 2, 1, 1, 1
int init_sysDescr(void);
char *set_sysDescription(FILE *f, char *line);

/* MIB object sysObjectID = system, 2 */
#define	I_sysObjectID	2
#define	O_sysObjectID	1, 3, 6, 1, 2, 1, 1, 2

/* MIB object sysUpTime = system, 3 */
#define	I_sysUpTime	3
#define	O_sysUpTime	1, 3, 6, 1, 2, 1, 1, 3
unsigned long current_sysUpTime(void);

/* MIB object sysContact = system, 4 */
#define	I_sysContact	4
#define	O_sysContact	1, 3, 6, 1, 2, 1, 1, 4
char *set_sysContact(FILE *f, char *line);

/* MIB object sysName = system, 5 */
#define	I_sysName	5
#define	O_sysName	1, 3, 6, 1, 2, 1, 1, 5

/* MIB object sysLocation = system, 6 */
#define	I_sysLocation	6
#define	O_sysLocation	1, 3, 6, 1, 2, 1, 1, 6
char *set_sysLocation(FILE *f, char *line);

/* MIB object sysServices = system, 7 */
#define	I_sysServices	7
#define	O_sysServices	1, 3, 6, 1, 2, 1, 1, 7

/* MIB object sysORLastChange = system, 8 */
#define	I_sysORLastChange	8
#define	O_sysORLastChange	1, 3, 6, 1, 2, 1, 1, 8

/* MIB object sysORTable = system, 9 */
#define	I_sysORTable	9
#define	O_sysORTable	1, 3, 6, 1, 2, 1, 1, 9

/* MIB object sysOREntry = sysORTable, 1 */
#define	I_sysOREntry	1
#define	O_sysOREntry	1, 3, 6, 1, 2, 1, 1, 9, 1
struct sysOREntry_struct {
    struct sysOREntry_struct *next;
    int                      index;
    Oid                      oid_value;
    char                     *descr;
    unsigned long            lastChange;
};
int insert_sysOREntry(struct sysOREntry_struct *value);

/* MIB object sysORIndex = sysOREntry, 1 */
#define	I_sysORIndex	1
#define	O_sysORIndex	1, 3, 6, 1, 2, 1, 1, 9, 1, 1

/* MIB object sysORID = sysOREntry, 2 */
#define	I_sysORID	2
#define	O_sysORID	1, 3, 6, 1, 2, 1, 1, 9, 1, 2

/* MIB object sysORDescr = sysOREntry, 3 */
#define	I_sysORDescr	3
#define	O_sysORDescr	1, 3, 6, 1, 2, 1, 1, 9, 1, 3

/* MIB object sysORUpTime = sysOREntry, 4 */
#define	I_sysORUpTime	4
#define	O_sysORUpTime	1, 3, 6, 1, 2, 1, 1, 9, 1, 4

/* MIB object snmp = mib_2, 11 */
#define	I_snmp	11
#define	O_snmp	1, 3, 6, 1, 2, 1, 11

/* MIB object snmpInPkts = snmp, 1 */
#define	I_snmpInPkts	1
#define	O_snmpInPkts	1, 3, 6, 1, 2, 1, 11, 1

/* MIB object snmpInBadVersions = snmp, 3 */
#define	I_snmpInBadVersions	3
#define	O_snmpInBadVersions	1, 3, 6, 1, 2, 1, 11, 3

/* MIB object snmpInBadCommunityNames = snmp, 4 */
#define	I_snmpInBadCommunityNames	4
#define	O_snmpInBadCommunityNames	1, 3, 6, 1, 2, 1, 11, 4

/* MIB object snmpInBadCommunityUses = snmp, 5 */
#define	I_snmpInBadCommunityUses	5
#define	O_snmpInBadCommunityUses	1, 3, 6, 1, 2, 1, 11, 5

/* MIB object snmpInASNParseErrs = snmp, 6 */
#define	I_snmpInASNParseErrs	6
#define	O_snmpInASNParseErrs	1, 3, 6, 1, 2, 1, 11, 6

/* MIB object snmpEnableAuthenTraps = snmp, 30 */
#define	I_snmpEnableAuthenTraps	30
#define	O_snmpEnableAuthenTraps	1, 3, 6, 1, 2, 1, 11, 30

/* MIB object snmpSilentDrops = snmp, 31 */
#define	I_snmpSilentDrops	31
#define	O_snmpSilentDrops	1, 3, 6, 1, 2, 1, 11, 31

/* MIB object snmpProxyDrops = snmp, 32 */
#define	I_snmpProxyDrops	32
#define	O_snmpProxyDrops	1, 3, 6, 1, 2, 1, 11, 32

/* MIB object snmpTrap = snmpMIBObjects, 4 */
#define	I_snmpTrap	4
#define	O_snmpTrap	1, 3, 6, 1, 6, 3, 1, 1, 4

/* MIB object snmpTrapOID = snmpTrap, 1 */
#define	I_snmpTrapOID	1
#define	O_snmpTrapOID	1, 3, 6, 1, 6, 3, 1, 1, 4, 1

/* MIB object snmpTrapEnterprise = snmpTrap, 3 */
#define	I_snmpTrapEnterprise	3
#define	O_snmpTrapEnterprise	1, 3, 6, 1, 6, 3, 1, 1, 4, 3

/* MIB object snmpTraps = snmpMIBObjects, 5 */
#define	I_snmpTraps	5
#define	O_snmpTraps	1, 3, 6, 1, 6, 3, 1, 1, 5

/* MIB object coldStart = snmpTraps, 1 */
#define	I_coldStart	1
#define	O_coldStart	1, 3, 6, 1, 6, 3, 1, 1, 5, 1

/* MIB object warmStart = snmpTraps, 2 */
#define	I_warmStart	2
#define	O_warmStart	1, 3, 6, 1, 6, 3, 1, 1, 5, 2

/* MIB object linkDown = snmpTraps, 3 */
#define	I_linkDown 3
#define	O_linkDown	1, 3, 6, 1, 6, 3, 1, 1, 5, 3

/* MIB object linkUp = snmpTraps, 4 */
#define	I_linkUp	4
#define	O_linkUp	1, 3, 6, 1, 6, 3, 1, 1, 5, 4

/* MIB object authenticationFailure = snmpTraps, 5 */
#define	I_authenticationFailure	5
#define	O_authenticationFailure	1, 3, 6, 1, 6, 3, 1, 1, 5, 5

/* MIB object snmpSet = snmpMIBObjects, 6 */
#define	I_snmpSet	6
#define	O_snmpSet	1, 3, 6, 1, 6, 3, 1, 1, 6

/* MIB object snmpSetSerialNo = snmpSet, 1 */
#define	I_snmpSetSerialNo	1
#define	O_snmpSetSerialNo	1, 3, 6, 1, 6, 3, 1, 1, 6, 1

/* MIB object snmpMIBConformance = snmpMIB, 2 */
#define	I_snmpMIBConformance	2
#define	O_snmpMIBConformance	1, 3, 6, 1, 6, 3, 1, 2

/* MIB object snmpMIBCompliances = snmpMIBConformance, 1 */
#define	I_snmpMIBCompliances	1
#define	O_snmpMIBCompliances	1, 3, 6, 1, 6, 3, 1, 2, 1

/* MIB object snmpMIBGroups = snmpMIBConformance, 2 */
#define	I_snmpMIBGroups	2
#define	O_snmpMIBGroups	1, 3, 6, 1, 6, 3, 1, 2, 2

/* MIB object snmpBasicCompliance = snmpMIBCompliances, 2 */
#define	I_snmpBasicCompliance	2
#define	O_snmpBasicCompliance	1, 3, 6, 1, 6, 3, 1, 2, 1, 2

/* MIB object snmpGroup = snmpMIBGroups, 8 */
#define	I_snmpGroup	8
#define	O_snmpGroup	1, 3, 6, 1, 6, 3, 1, 2, 2, 8

/* MIB object snmpCommunityGroup = snmpMIBGroups, 9 */
#define	I_snmpCommunityGroup	9
#define	O_snmpCommunityGroup	1, 3, 6, 1, 6, 3, 1, 2, 2, 9

/* MIB object snmpSetGroup = snmpMIBGroups, 5 */
#define	I_snmpSetGroup	5
#define	O_snmpSetGroup	1, 3, 6, 1, 6, 3, 1, 2, 2, 5

/* MIB object systemGroup = snmpMIBGroups, 6 */
#define	I_systemGroup	6
#define	O_systemGroup	1, 3, 6, 1, 6, 3, 1, 2, 2, 6

/* MIB object snmpBasicNotificationsGroup = snmpMIBGroups, 7 */
#define	I_snmpBasicNotificationsGroup	7
#define	O_snmpBasicNotificationsGroup	1, 3, 6, 1, 6, 3, 1, 2, 2, 7

/* MIB object snmpOutPkts = snmp, 2 */
#define	I_snmpOutPkts	2
#define	O_snmpOutPkts	1, 3, 6, 1, 2, 1, 11, 2

/* MIB object snmpInTooBigs = snmp, 8 */
#define	I_snmpInTooBigs	8
#define	O_snmpInTooBigs	1, 3, 6, 1, 2, 1, 11, 8

/* MIB object snmpInNoSuchNames = snmp, 9 */
#define	I_snmpInNoSuchNames	9
#define	O_snmpInNoSuchNames	1, 3, 6, 1, 2, 1, 11, 9

/* MIB object snmpInBadValues = snmp, 10 */
#define	I_snmpInBadValues	10
#define	O_snmpInBadValues	1, 3, 6, 1, 2, 1, 11, 10

/* MIB object snmpInReadOnlys = snmp, 11 */
#define	I_snmpInReadOnlys	11
#define	O_snmpInReadOnlys	1, 3, 6, 1, 2, 1, 11, 11

/* MIB object snmpInGenErrs = snmp, 12 */
#define	I_snmpInGenErrs	12
#define	O_snmpInGenErrs	1, 3, 6, 1, 2, 1, 11, 12

/* MIB object snmpInTotalReqVars = snmp, 13 */
#define	I_snmpInTotalReqVars	13
#define	O_snmpInTotalReqVars	1, 3, 6, 1, 2, 1, 11, 13

/* MIB object snmpInTotalSetVars = snmp, 14 */
#define	I_snmpInTotalSetVars	14
#define	O_snmpInTotalSetVars	1, 3, 6, 1, 2, 1, 11, 14

/* MIB object snmpInGetRequests = snmp, 15 */
#define	I_snmpInGetRequests	15
#define	O_snmpInGetRequests	1, 3, 6, 1, 2, 1, 11, 15

/* MIB object snmpInGetNexts = snmp, 16 */
#define	I_snmpInGetNexts	16
#define	O_snmpInGetNexts	1, 3, 6, 1, 2, 1, 11, 16

/* MIB object snmpInSetRequests = snmp, 17 */
#define	I_snmpInSetRequests	17
#define	O_snmpInSetRequests	1, 3, 6, 1, 2, 1, 11, 17

/* MIB object snmpInGetResponses = snmp, 18 */
#define	I_snmpInGetResponses	18
#define	O_snmpInGetResponses	1, 3, 6, 1, 2, 1, 11, 18

/* MIB object snmpInTraps = snmp, 19 */
#define	I_snmpInTraps	19
#define	O_snmpInTraps	1, 3, 6, 1, 2, 1, 11, 19

/* MIB object snmpOutTooBigs = snmp, 20 */
#define	I_snmpOutTooBigs	20
#define	O_snmpOutTooBigs	1, 3, 6, 1, 2, 1, 11, 20

/* MIB object snmpOutNoSuchNames = snmp, 21 */
#define	I_snmpOutNoSuchNames	21
#define	O_snmpOutNoSuchNames	1, 3, 6, 1, 2, 1, 11, 21

/* MIB object snmpOutBadValues = snmp, 22 */
#define	I_snmpOutBadValues	22
#define	O_snmpOutBadValues	1, 3, 6, 1, 2, 1, 11, 22

/* MIB object snmpOutGenErrs = snmp, 24 */
#define	I_snmpOutGenErrs	24
#define	O_snmpOutGenErrs	1, 3, 6, 1, 2, 1, 11, 24

/* MIB object snmpOutGetRequests = snmp, 25 */
#define	I_snmpOutGetRequests	25
#define	O_snmpOutGetRequests	1, 3, 6, 1, 2, 1, 11, 25

/* MIB object snmpOutGetNexts = snmp, 26 */
#define	I_snmpOutGetNexts	26
#define	O_snmpOutGetNexts	1, 3, 6, 1, 2, 1, 11, 26

/* MIB object snmpOutSetRequests = snmp, 27 */
#define	I_snmpOutSetRequests	27
#define	O_snmpOutSetRequests	1, 3, 6, 1, 2, 1, 11, 27

/* MIB object snmpOutGetResponses = snmp, 28 */
#define	I_snmpOutGetResponses	28
#define	O_snmpOutGetResponses	1, 3, 6, 1, 2, 1, 11, 28

/* MIB object snmpOutTraps = snmp, 29 */
#define	I_snmpOutTraps	29
#define	O_snmpOutTraps	1, 3, 6, 1, 2, 1, 11, 29

/* MIB object snmpObsoleteGroup = snmpMIBGroups, 10 */
#define	I_snmpObsoleteGroup	10
#define	O_snmpObsoleteGroup	1, 3, 6, 1, 6, 3, 1, 2, 2, 10

/* Put here additional MIB specific include definitions */

typedef struct snmpSystemMib {
  char sysDescr[MAX_OCTSTR_LEN];
  char sysContact[MAX_OCTSTR_LEN];
  char sysName[MAX_OCTSTR_LEN];
  char sysLocation[MAX_OCTSTR_LEN];
  unsigned long sysORLastChange_value;
  struct sysOREntry_struct *sysOREntry_root;
  int sysServices;
} SNMP_SYSTEM_MIB, *pSNMP_SYSTEM_MIB;

#endif	/* _SNMPv2_MIB_ */
