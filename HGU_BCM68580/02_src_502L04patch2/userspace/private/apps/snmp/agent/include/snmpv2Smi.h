#ifndef	_SNMPv2_SMI_
#define	_SNMPv2_SMI_

/* required include files (IMPORTS) */


void init_SNMPv2_SMI(void);
void register_subtrees_of_SNMPv2_SMI(void);

/* defined objects in this module */


/* MIB object org = iso, 3 */
#define	I_org	3
#define	O_org	1, 3

/* MIB object dod = org, 6 */
#define	I_dod	6
#define	O_dod	1, 3, 6

/* MIB object internet = dod, 1 */
#define	I_internet	1
#define	O_internet	1, 3, 6, 1

/* MIB object directory = internet, 1 */
#define	I_directory	1
#define	O_directory	1, 3, 6, 1, 1

/* MIB object mgmt = internet, 2 */
#define	I_mgmt	2
#define	O_mgmt	1, 3, 6, 1, 2

/* MIB object mib_2 = mgmt, 1 */
#define	I_mib_2	1
#define	O_mib_2	1, 3, 6, 1, 2, 1

/* MIB object experimental = internet, 3 */
#define	I_experimental	3
#define	O_experimental	1, 3, 6, 1, 3

/* MIB object private = internet, 4 */
#define	I_private	4
#define	O_private	1, 3, 6, 1, 4

/* MIB object enterprises = private, 1 */
#define	I_enterprises	1
#define	O_enterprises	1, 3, 6, 1, 4, 1

/* MIB object security = internet, 5 */
#define	I_security	5
#define	O_security	1, 3, 6, 1, 5

/* MIB object snmpV2 = internet, 6 */
#define	I_snmpV2	6
#define	O_snmpV2	1, 3, 6, 1, 6

/* MIB object snmpDomains = snmpV2, 1 */
#define	I_snmpDomains	1
#define	O_snmpDomains	1, 3, 6, 1, 6, 1

/* MIB object snmpProxys = snmpV2, 2 */
#define	I_snmpProxys	2
#define	O_snmpProxys	1, 3, 6, 1, 6, 2

/* MIB object snmpModules = snmpV2, 3 */
#define	I_snmpModules	3
#define	O_snmpModules	1, 3, 6, 1, 6, 3

/* Put here additional MIB specific include definitions */

#endif	/* _SNMPv2_SMI_ */
