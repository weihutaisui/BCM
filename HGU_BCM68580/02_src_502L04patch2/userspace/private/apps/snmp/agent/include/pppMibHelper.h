#ifndef	_PPP_MIB_HELPER_
#define	_PPP_MIB_HELPER_

#include "pppSecMib.h"

#define PPP_SEC_LOCAL_TO_REMOTE           1
#define PPP_SEC_REMOTE_TO_LOCAL           2
#define PPP_SEC_STATUS_INVALID            1
#define PPP_SEC_STATUS_VALID              2

#define PPP_SEC_SECRET_DIR_FIELD          1
#define PPP_SEC_SECRET_PROTOCOL_FIELD     2
#define PPP_SEC_SECRET_ID_FIELD           3
#define PPP_SEC_SECRET_PASSWORD_FIELD     4
#define PPP_SEC_SECRET_STATUS_FIELD       5

#define PPP_SEC_ID_SET                    1
#define PPP_SEC_PASSWD_SET                2

void pppFreeSecretInfo(void);
void pppAddSecretToList(pPPP_SEC_ENTRY p);
void pppScanPppInfo(void);
void pppFreeSecretInfo(void);
int pppConfigSecretInfo(pPPP_SEC_ENTRY pEntry, int index, int field);

#endif	/* _PPP_MIB_HELPER_ */
