#ifndef	_IFTYPE_MIB_
#define	_IFTYPE_MIB_

/* required include files (IMPORTS) */
#include	"snmpv2Tc.h"
#include	"snmpv2Smi.h"


void init_IANAifType_MIB(void);
void register_subtrees_of_IANAifType_MIB(void);

/* defined objects in this module */


/* MIB object ianaifType = mib_2, 30 */
#define	I_ianaifType	30
#define	O_ianaifType	1, 3, 6, 1, 2, 1, 30

/* Put here additional MIB specific include definitions */

#endif	/* _IFTYPE_MIB_ */
