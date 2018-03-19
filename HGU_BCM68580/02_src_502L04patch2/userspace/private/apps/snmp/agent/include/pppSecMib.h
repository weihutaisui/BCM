#ifndef	_PPP_SEC_MIB_
#define	_PPP_SEC_MIB_

/* required include files (IMPORTS) */
#include	"pppLcpMib.h"
#include	"rfc1212Mib.h"
#include	"rfc1155Smi.h"
#include        "asn1.h"

void init_pppSecMib(void);
void register_subtrees_of_ppp_sec_mib(void);

/* defined objects in this module */


/* MIB object pppSecurity = ppp, 2 */
#define	I_pppSecurity	2
#define	O_pppSecurity	1, 3, 6, 1, 2, 1, 10, 23, 2

/* MIB object pppSecurityProtocols = pppSecurity, 1 */
#define	I_pppSecurityProtocols	1
#define	O_pppSecurityProtocols	1, 3, 6, 1, 2, 1, 10, 23, 2, 1

/* MIB object pppSecurityPapProtocol = pppSecurityProtocols, 1 */
#define	I_pppSecurityPapProtocol	1
#define	O_pppSecurityPapProtocol	1, 3, 6, 1, 2, 1, 10, 23, 2, 1, 1

/* MIB object pppSecurityChapMD5Protocol = pppSecurityProtocols, 2 */
#define	I_pppSecurityChapMD5Protocol	2
#define	O_pppSecurityChapMD5Protocol	1, 3, 6, 1, 2, 1, 10, 23, 2, 1, 2

/* MIB object pppSecurityConfigTable = pppSecurity, 2 */
#define	I_pppSecurityConfigTable	2
#define	O_pppSecurityConfigTable	1, 3, 6, 1, 2, 1, 10, 23, 2, 2

/* MIB object pppSecurityConfigEntry = pppSecurityConfigTable, 1 */
#define	I_pppSecurityConfigEntry	1
#define	O_pppSecurityConfigEntry	1, 3, 6, 1, 2, 1, 10, 23, 2, 2, 1

/* MIB object pppSecurityConfigLink = pppSecurityConfigEntry, 1 */
#define	I_pppSecurityConfigLink	1
#define	O_pppSecurityConfigLink	1, 3, 6, 1, 2, 1, 10, 23, 2, 2, 1, 1

/* MIB object pppSecurityConfigPreference = pppSecurityConfigEntry, 2 */
#define	I_pppSecurityConfigPreference	2
#define	O_pppSecurityConfigPreference	1, 3, 6, 1, 2, 1, 10, 23, 2, 2, 1, 2

/* MIB object pppSecurityConfigProtocol = pppSecurityConfigEntry, 3 */
#define	I_pppSecurityConfigProtocol	3
#define	O_pppSecurityConfigProtocol	1, 3, 6, 1, 2, 1, 10, 23, 2, 2, 1, 3

/* MIB object pppSecurityConfigStatus = pppSecurityConfigEntry, 4 */
#define	I_pppSecurityConfigStatus	4
#define	O_pppSecurityConfigStatus	1, 3, 6, 1, 2, 1, 10, 23, 2, 2, 1, 4

/* MIB object pppSecuritySecretsTable = pppSecurity, 3 */
#define	I_pppSecuritySecretsTable	3
#define	O_pppSecuritySecretsTable	1, 3, 6, 1, 2, 1, 10, 23, 2, 3

/* MIB object pppSecuritySecretsEntry = pppSecuritySecretsTable, 1 */
#define	I_pppSecuritySecretsEntry	1
#define	O_pppSecuritySecretsEntry	1, 3, 6, 1, 2, 1, 10, 23, 2, 3, 1

/* MIB object pppSecuritySecretsLink = pppSecuritySecretsEntry, 1 */
#define	I_pppSecuritySecretsLink	1
#define	O_pppSecuritySecretsLink	1, 3, 6, 1, 2, 1, 10, 23, 2, 3, 1, 1

/* MIB object pppSecuritySecretsIdIndex = pppSecuritySecretsEntry, 2 */
#define	I_pppSecuritySecretsIdIndex	2
#define	O_pppSecuritySecretsIdIndex	1, 3, 6, 1, 2, 1, 10, 23, 2, 3, 1, 2

/* MIB object pppSecuritySecretsDirection = pppSecuritySecretsEntry, 3 */
#define	I_pppSecuritySecretsDirection	3
#define	O_pppSecuritySecretsDirection	1, 3, 6, 1, 2, 1, 10, 23, 2, 3, 1, 3

/* MIB object pppSecuritySecretsProtocol = pppSecuritySecretsEntry, 4 */
#define	I_pppSecuritySecretsProtocol	4
#define	O_pppSecuritySecretsProtocol	1, 3, 6, 1, 2, 1, 10, 23, 2, 3, 1, 4

/* MIB object pppSecuritySecretsIdentity = pppSecuritySecretsEntry, 5 */
#define	I_pppSecuritySecretsIdentity	5
#define	O_pppSecuritySecretsIdentity	1, 3, 6, 1, 2, 1, 10, 23, 2, 3, 1, 5

/* MIB object pppSecuritySecretsSecret = pppSecuritySecretsEntry, 6 */
#define	I_pppSecuritySecretsSecret	6
#define	O_pppSecuritySecretsSecret	1, 3, 6, 1, 2, 1, 10, 23, 2, 3, 1, 6

/* MIB object pppSecuritySecretsStatus = pppSecuritySecretsEntry, 7 */
#define	I_pppSecuritySecretsStatus	7
#define	O_pppSecuritySecretsStatus	1, 3, 6, 1, 2, 1, 10, 23, 2, 3, 1, 7

/* Put here additional MIB specific include definitions */
#define PPP_SEC_MAX_LENGTH                255
#define PPP_SEC_PASSWORD_LENGTH           40
#define PPP_SEC_PROTOCOL_LEN              12

typedef struct pppSecEntry
{
  int secretLink;        /* link ID/secret pair applies-- ifIndex of the ppp intf */
  int secretIdIndex;     
  int secretDirection;   /* local-to-remote/remote-to-local */
  oid secretProtocol[PPP_SEC_PROTOCOL_LEN];    /* CHAP or PAP */
  int protocolLen;        /* len of secretProtocol */
  char secretId[PPP_SEC_MAX_LENGTH];          /* user */
  int secretIdLen;
  char secret[PPP_SEC_PASSWORD_LENGTH];       /* password */
  int secretLen;
  int status;            /* invalid/valid */
  int vpi;               /* internal use */
  int vci;               /* internal use */
  int conId;             /* internal use */
  struct pppSecEntry *next;
} PPP_SEC_ENTRY, *pPPP_SEC_ENTRY;

typedef struct pppSecList {
  pPPP_SEC_ENTRY head;
  pPPP_SEC_ENTRY tail;
  int count;
} PPP_SEC_LIST, *pPPP_SEC_LIST;

#endif	/* _PPP_SEC_MIB_ */
