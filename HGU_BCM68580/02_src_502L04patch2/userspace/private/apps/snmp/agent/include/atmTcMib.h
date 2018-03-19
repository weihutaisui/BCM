#ifndef	_ATM_TC_MIB_
#define	_ATM_TC_MIB_

/* required include files (IMPORTS) */
#include	"snmpv2Tc.h"
#include	"snmpv2Smi.h"


void init_ATM_TC_MIB(void);
void register_subtrees_of_ATM_TC_MIB(void);

/* defined objects in this module */


/* MIB object atmTCMIB = atmMIB, 3 */
#define	I_atmTCMIB	3
#define	O_atmTCMIB	1, 3, 6, 1, 2, 1, 37, 3

/* MIB object atmTrafficDescriptorTypes = atmMIBObjects, 1 */
#define	I_atmTrafficDescriptorTypes	1
#define	O_atmTrafficDescriptorTypes	1, 3, 6, 1, 2, 1, 37, 1, 1

/* MIB object atmObjectIdentities = atmTCMIB, 1 */
#define	I_atmObjectIdentities	1
#define	O_atmObjectIdentities	1, 3, 6, 1, 2, 1, 37, 3, 1

/* MIB object atmNoTrafficDescriptor = atmTrafficDescriptorTypes, 1 */
#define	I_atmNoTrafficDescriptor	1
#define	O_atmNoTrafficDescriptor	1, 3, 6, 1, 2, 1, 37, 1, 1, 1

/* MIB object atmNoClpNoScr = atmTrafficDescriptorTypes, 2 */
#define	I_atmNoClpNoScr	2
#define	O_atmNoClpNoScr	1, 3, 6, 1, 2, 1, 37, 1, 1, 2

/* MIB object atmClpNoTaggingNoScr = atmTrafficDescriptorTypes, 3 */
#define	I_atmClpNoTaggingNoScr	3
#define	O_atmClpNoTaggingNoScr	1, 3, 6, 1, 2, 1, 37, 1, 1, 3

/* MIB object atmClpTaggingNoScr = atmTrafficDescriptorTypes, 4 */
#define	I_atmClpTaggingNoScr	4
#define	O_atmClpTaggingNoScr	1, 3, 6, 1, 2, 1, 37, 1, 1, 4

/* MIB object atmNoClpScr = atmTrafficDescriptorTypes, 5 */
#define	I_atmNoClpScr	5
#define	O_atmNoClpScr	1, 3, 6, 1, 2, 1, 37, 1, 1, 5

/* MIB object atmClpNoTaggingScr = atmTrafficDescriptorTypes, 6 */
#define	I_atmClpNoTaggingScr	6
#define	O_atmClpNoTaggingScr	1, 3, 6, 1, 2, 1, 37, 1, 1, 6

/* MIB object atmClpTaggingScr = atmTrafficDescriptorTypes, 7 */
#define	I_atmClpTaggingScr	7
#define	O_atmClpTaggingScr	1, 3, 6, 1, 2, 1, 37, 1, 1, 7

/* MIB object atmClpNoTaggingMcr = atmTrafficDescriptorTypes, 8 */
#define	I_atmClpNoTaggingMcr	8
#define	O_atmClpNoTaggingMcr	1, 3, 6, 1, 2, 1, 37, 1, 1, 8

/* MIB object atmClpTransparentNoScr = atmTrafficDescriptorTypes, 9 */
#define	I_atmClpTransparentNoScr	9
#define	O_atmClpTransparentNoScr	1, 3, 6, 1, 2, 1, 37, 1, 1, 9

/* MIB object atmClpTransparentScr = atmTrafficDescriptorTypes, 10 */
#define	I_atmClpTransparentScr	10
#define	O_atmClpTransparentScr	1, 3, 6, 1, 2, 1, 37, 1, 1, 10

/* MIB object atmNoClpTaggingNoScr = atmTrafficDescriptorTypes, 11 */
#define	I_atmNoClpTaggingNoScr	11
#define	O_atmNoClpTaggingNoScr	1, 3, 6, 1, 2, 1, 37, 1, 1, 11

/* MIB object atmNoClpNoScrCdvt = atmTrafficDescriptorTypes, 12 */
#define	I_atmNoClpNoScrCdvt	12
#define	O_atmNoClpNoScrCdvt	1, 3, 6, 1, 2, 1, 37, 1, 1, 12

/* MIB object atmNoClpScrCdvt = atmTrafficDescriptorTypes, 13 */
#define	I_atmNoClpScrCdvt	13
#define	O_atmNoClpScrCdvt	1, 3, 6, 1, 2, 1, 37, 1, 1, 13

/* MIB object atmClpNoTaggingScrCdvt = atmTrafficDescriptorTypes, 14 */
#define	I_atmClpNoTaggingScrCdvt	14
#define	O_atmClpNoTaggingScrCdvt	1, 3, 6, 1, 2, 1, 37, 1, 1, 14

/* MIB object atmClpTaggingScrCdvt = atmTrafficDescriptorTypes, 15 */
#define	I_atmClpTaggingScrCdvt	15
#define	O_atmClpTaggingScrCdvt	1, 3, 6, 1, 2, 1, 37, 1, 1, 15

/* Put here additional MIB specific include definitions */

#endif /* _ATM_TC_MIB_ */
