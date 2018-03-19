#ifndef	BRCM_CPE_MIB_H
#define	BRCM_CPE_MIB_H

/* required include files (IMPORTS) */
#include	"brcmDslSmi.h"
#include	"snmpv2Smi.h"


void init_brcm_cpe_mib(void);
void register_subtrees_of_brcm_cpe_mib(void);

#define BRCM_CPE_WIRELESS_TRANSCEIVER_ON      1
#define BRCM_CPE_WIRELESS_TRANSCEIVER_OFF     2

/* defined objects in this module */
typedef struct brcmCpeInfo {
  int wirelessTransceiver; /* on(1)/off(2) */
} BRCM_CPE_INFO, *pBRCM_CPE_INFO;

/* MIB object cpe = dsl, 1 */
#define	I_cpe	1
#define	O_cpe	1, 3, 6, 1, 4, 1, 4413, 4, 1

/* MIB object wireless = cpe, 1 */
#define	I_wireless	1
#define	O_wireless	1, 3, 6, 1, 4, 1, 4413, 4, 1, 1

/* MIB object wirelessTransceiver = wireless, 1 */
#define	I_wirelessTransceiver	1
#define	O_wirelessTransceiver	1, 3, 6, 1, 4, 1, 4413, 4, 1, 1, 1

/* Put here additional MIB specific include definitions */

#endif /* BRCM_CPE_MIB_H */
