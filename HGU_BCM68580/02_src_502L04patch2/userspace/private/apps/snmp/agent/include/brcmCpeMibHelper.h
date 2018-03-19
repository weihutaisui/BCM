#ifndef	BRCM_CPE_MIB_HELPER_H
#define	BRCM_CPE_MIB_HELPER_H

/* required include files (IMPORTS) */
#include	"brcmDslSmi.h"
#include	"snmpv2Smi.h"


int brcmCpeGetWirelessTransceiver(void);
void brcmCpeSetWirelessTransceiver(int mode);

#endif /* BRCM_CPE_MIB_HELPER_H */
