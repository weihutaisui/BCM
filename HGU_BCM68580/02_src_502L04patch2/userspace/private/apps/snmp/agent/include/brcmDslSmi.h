#ifndef	BRCM_DSL_SMI_H
#define	BRCM_DSL_SMI_H

/* required include files (IMPORTS) */
#include	"snmpv2Smi.h"

/* defined objects in this module */

/* MIB object broadcom = enterprises, 4413 */
#define	I_broadcom	4413
#define	O_broadcom	1, 3, 6, 1, 4, 1, 4413

/* MIB object dsl = broadcom, 4 */
#define	I_dsl	4
#define	O_dsl	1, 3, 6, 1, 4, 1, 4413, 4

/* Put here additional MIB specific include definitions */

#endif	/* BRCM_DSL_SMI_H */
