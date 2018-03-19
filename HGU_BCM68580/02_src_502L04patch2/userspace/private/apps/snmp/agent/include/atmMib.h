#ifndef	_ATM_MIB_
#define	_ATM_MIB_

/* required include files (IMPORTS) */
#include	"atmTcMib.h"
#include	"ifMib.h"
#include	"snmpv2Tc.h"
#include	"snmpv2Smi.h"

void init_atmMib(void);
void register_subtrees_of_ATM_MIB(void);
void atmMibFree(void);

/* defined objects in this module */


/* MIB object atmMIB = mib_2, 37 */
#define	I_atmMIB	37
#define	O_atmMIB	1, 3, 6, 1, 2, 1, 37

/* MIB object atmMIBObjects = atmMIB, 1 */
#define	I_atmMIBObjects	1
#define	O_atmMIBObjects	1, 3, 6, 1, 2, 1, 37, 1

/* MIB object atmInterfaceConfTable = atmMIBObjects, 2 */
#define	I_atmInterfaceConfTable	2
#define	O_atmInterfaceConfTable	1, 3, 6, 1, 2, 1, 37, 1, 2

/* MIB object atmInterfaceConfEntry = atmInterfaceConfTable, 1 */
#define	I_atmInterfaceConfEntry	1
#define	O_atmInterfaceConfEntry	1, 3, 6, 1, 2, 1, 37, 1, 2, 1

/* MIB object atmInterfaceMaxVpcs = atmInterfaceConfEntry, 1 */
#define	I_atmInterfaceMaxVpcs	1
#define	O_atmInterfaceMaxVpcs	1, 3, 6, 1, 2, 1, 37, 1, 2, 1, 1

/* MIB object atmInterfaceMaxVccs = atmInterfaceConfEntry, 2 */
#define	I_atmInterfaceMaxVccs	2
#define	O_atmInterfaceMaxVccs	1, 3, 6, 1, 2, 1, 37, 1, 2, 1, 2

/* MIB object atmInterfaceConfVpcs = atmInterfaceConfEntry, 3 */
#define	I_atmInterfaceConfVpcs	3
#define	O_atmInterfaceConfVpcs	1, 3, 6, 1, 2, 1, 37, 1, 2, 1, 3

/* MIB object atmInterfaceConfVccs = atmInterfaceConfEntry, 4 */
#define	I_atmInterfaceConfVccs	4
#define	O_atmInterfaceConfVccs	1, 3, 6, 1, 2, 1, 37, 1, 2, 1, 4

/* MIB object atmInterfaceMaxActiveVpiBits = atmInterfaceConfEntry, 5 */
#define	I_atmInterfaceMaxActiveVpiBits	5
#define	O_atmInterfaceMaxActiveVpiBits	1, 3, 6, 1, 2, 1, 37, 1, 2, 1, 5

/* MIB object atmInterfaceMaxActiveVciBits = atmInterfaceConfEntry, 6 */
#define	I_atmInterfaceMaxActiveVciBits	6
#define	O_atmInterfaceMaxActiveVciBits	1, 3, 6, 1, 2, 1, 37, 1, 2, 1, 6

/* MIB object atmInterfaceIlmiVpi = atmInterfaceConfEntry, 7 */
#define	I_atmInterfaceIlmiVpi	7
#define	O_atmInterfaceIlmiVpi	1, 3, 6, 1, 2, 1, 37, 1, 2, 1, 7

/* MIB object atmInterfaceIlmiVci = atmInterfaceConfEntry, 8 */
#define	I_atmInterfaceIlmiVci	8
#define	O_atmInterfaceIlmiVci	1, 3, 6, 1, 2, 1, 37, 1, 2, 1, 8

/* MIB object atmInterfaceAddressType = atmInterfaceConfEntry, 9 */
#define	I_atmInterfaceAddressType	9
#define	O_atmInterfaceAddressType	1, 3, 6, 1, 2, 1, 37, 1, 2, 1, 9

/* MIB object atmInterfaceAdminAddress = atmInterfaceConfEntry, 10 */
#define	I_atmInterfaceAdminAddress	10
#define	O_atmInterfaceAdminAddress	1, 3, 6, 1, 2, 1, 37, 1, 2, 1, 10

/* MIB object atmInterfaceMyNeighborIpAddress = atmInterfaceConfEntry, 11 */
#define	I_atmInterfaceMyNeighborIpAddress	11
#define	O_atmInterfaceMyNeighborIpAddress	1, 3, 6, 1, 2, 1, 37, 1, 2, 1, 11

/* MIB object atmInterfaceMyNeighborIfName = atmInterfaceConfEntry, 12 */
#define	I_atmInterfaceMyNeighborIfName	12
#define	O_atmInterfaceMyNeighborIfName	1, 3, 6, 1, 2, 1, 37, 1, 2, 1, 12

/* MIB object atmInterfaceCurrentMaxVpiBits = atmInterfaceConfEntry, 13 */
#define	I_atmInterfaceCurrentMaxVpiBits	13
#define	O_atmInterfaceCurrentMaxVpiBits	1, 3, 6, 1, 2, 1, 37, 1, 2, 1, 13

/* MIB object atmInterfaceCurrentMaxVciBits = atmInterfaceConfEntry, 14 */
#define	I_atmInterfaceCurrentMaxVciBits	14
#define	O_atmInterfaceCurrentMaxVciBits	1, 3, 6, 1, 2, 1, 37, 1, 2, 1, 14

/* MIB object atmInterfaceSubscrAddress = atmInterfaceConfEntry, 15 */
#define	I_atmInterfaceSubscrAddress	15
#define	O_atmInterfaceSubscrAddress	1, 3, 6, 1, 2, 1, 37, 1, 2, 1, 15

/* MIB object atmInterfaceDs3PlcpTable = atmMIBObjects, 3 */
#define	I_atmInterfaceDs3PlcpTable	3
#define	O_atmInterfaceDs3PlcpTable	1, 3, 6, 1, 2, 1, 37, 1, 3

/* MIB object atmInterfaceDs3PlcpEntry = atmInterfaceDs3PlcpTable, 1 */
#define	I_atmInterfaceDs3PlcpEntry	1
#define	O_atmInterfaceDs3PlcpEntry	1, 3, 6, 1, 2, 1, 37, 1, 3, 1

/* MIB object atmInterfaceDs3PlcpSEFSs = atmInterfaceDs3PlcpEntry, 1 */
#define	I_atmInterfaceDs3PlcpSEFSs	1
#define	O_atmInterfaceDs3PlcpSEFSs	1, 3, 6, 1, 2, 1, 37, 1, 3, 1, 1

/* MIB object atmInterfaceDs3PlcpAlarmState = atmInterfaceDs3PlcpEntry, 2 */
#define	I_atmInterfaceDs3PlcpAlarmState	2
#define	O_atmInterfaceDs3PlcpAlarmState	1, 3, 6, 1, 2, 1, 37, 1, 3, 1, 2

/* MIB object atmInterfaceDs3PlcpUASs = atmInterfaceDs3PlcpEntry, 3 */
#define	I_atmInterfaceDs3PlcpUASs	3
#define	O_atmInterfaceDs3PlcpUASs	1, 3, 6, 1, 2, 1, 37, 1, 3, 1, 3

/* MIB object atmInterfaceTCTable = atmMIBObjects, 4 */
#define	I_atmInterfaceTCTable	4
#define	O_atmInterfaceTCTable	1, 3, 6, 1, 2, 1, 37, 1, 4

/* MIB object atmInterfaceTCEntry = atmInterfaceTCTable, 1 */
#define	I_atmInterfaceTCEntry	1
#define	O_atmInterfaceTCEntry	1, 3, 6, 1, 2, 1, 37, 1, 4, 1

/* MIB object atmInterfaceOCDEvents = atmInterfaceTCEntry, 1 */
#define	I_atmInterfaceOCDEvents	1
#define	O_atmInterfaceOCDEvents	1, 3, 6, 1, 2, 1, 37, 1, 4, 1, 1

/* MIB object atmInterfaceTCAlarmState = atmInterfaceTCEntry, 2 */
#define	I_atmInterfaceTCAlarmState	2
#define	O_atmInterfaceTCAlarmState	1, 3, 6, 1, 2, 1, 37, 1, 4, 1, 2

/* MIB object atmTrafficDescrParamTable = atmMIBObjects, 5 */
#define	I_atmTrafficDescrParamTable	5
#define	O_atmTrafficDescrParamTable	1, 3, 6, 1, 2, 1, 37, 1, 5

/* MIB object atmTrafficDescrParamEntry = atmTrafficDescrParamTable, 1 */
#define	I_atmTrafficDescrParamEntry	1
#define	O_atmTrafficDescrParamEntry	1, 3, 6, 1, 2, 1, 37, 1, 5, 1

/* MIB object atmTrafficDescrParamIndex = atmTrafficDescrParamEntry, 1 */
#define	I_atmTrafficDescrParamIndex	1
#define	O_atmTrafficDescrParamIndex	1, 3, 6, 1, 2, 1, 37, 1, 5, 1, 1

/* MIB object atmTrafficDescrType = atmTrafficDescrParamEntry, 2 */
#define	I_atmTrafficDescrType	2
#define	O_atmTrafficDescrType	1, 3, 6, 1, 2, 1, 37, 1, 5, 1, 2

/* MIB object atmTrafficDescrParam1 = atmTrafficDescrParamEntry, 3 */
#define	I_atmTrafficDescrParam1	3
#define	O_atmTrafficDescrParam1	1, 3, 6, 1, 2, 1, 37, 1, 5, 1, 3

/* MIB object atmTrafficDescrParam2 = atmTrafficDescrParamEntry, 4 */
#define	I_atmTrafficDescrParam2	4
#define	O_atmTrafficDescrParam2	1, 3, 6, 1, 2, 1, 37, 1, 5, 1, 4

/* MIB object atmTrafficDescrParam3 = atmTrafficDescrParamEntry, 5 */
#define	I_atmTrafficDescrParam3	5
#define	O_atmTrafficDescrParam3	1, 3, 6, 1, 2, 1, 37, 1, 5, 1, 5

/* MIB object atmTrafficDescrParam4 = atmTrafficDescrParamEntry, 6 */
#define	I_atmTrafficDescrParam4	6
#define	O_atmTrafficDescrParam4	1, 3, 6, 1, 2, 1, 37, 1, 5, 1, 6

/* MIB object atmTrafficDescrParam5 = atmTrafficDescrParamEntry, 7 */
#define	I_atmTrafficDescrParam5	7
#define	O_atmTrafficDescrParam5	1, 3, 6, 1, 2, 1, 37, 1, 5, 1, 7

/* MIB object atmTrafficQoSClass = atmTrafficDescrParamEntry, 8 */
#define	I_atmTrafficQoSClass	8
#define	O_atmTrafficQoSClass	1, 3, 6, 1, 2, 1, 37, 1, 5, 1, 8

/* MIB object atmTrafficDescrRowStatus = atmTrafficDescrParamEntry, 9 */
#define	I_atmTrafficDescrRowStatus	9
#define	O_atmTrafficDescrRowStatus	1, 3, 6, 1, 2, 1, 37, 1, 5, 1, 9

/* MIB object atmServiceCategory = atmTrafficDescrParamEntry, 10 */
#define	I_atmServiceCategory	10
#define	O_atmServiceCategory	1, 3, 6, 1, 2, 1, 37, 1, 5, 1, 10

/* MIB object atmTrafficFrameDiscard = atmTrafficDescrParamEntry, 11 */
#define	I_atmTrafficFrameDiscard	11
#define	O_atmTrafficFrameDiscard	1, 3, 6, 1, 2, 1, 37, 1, 5, 1, 11

/* MIB object atmVplTable = atmMIBObjects, 6 */
#define	I_atmVplTable	6
#define	O_atmVplTable	1, 3, 6, 1, 2, 1, 37, 1, 6

/* MIB object atmVplEntry = atmVplTable, 1 */
#define	I_atmVplEntry	1
#define	O_atmVplEntry	1, 3, 6, 1, 2, 1, 37, 1, 6, 1

/* MIB object atmVplVpi = atmVplEntry, 1 */
#define	I_atmVplVpi	1
#define	O_atmVplVpi	1, 3, 6, 1, 2, 1, 37, 1, 6, 1, 1

/* MIB object atmVplAdminStatus = atmVplEntry, 2 */
#define	I_atmVplAdminStatus	2
#define	O_atmVplAdminStatus	1, 3, 6, 1, 2, 1, 37, 1, 6, 1, 2

/* MIB object atmVplOperStatus = atmVplEntry, 3 */
#define	I_atmVplOperStatus	3
#define	O_atmVplOperStatus	1, 3, 6, 1, 2, 1, 37, 1, 6, 1, 3

/* MIB object atmVplLastChange = atmVplEntry, 4 */
#define	I_atmVplLastChange	4
#define	O_atmVplLastChange	1, 3, 6, 1, 2, 1, 37, 1, 6, 1, 4

/* MIB object atmVplReceiveTrafficDescrIndex = atmVplEntry, 5 */
#define	I_atmVplReceiveTrafficDescrIndex	5
#define	O_atmVplReceiveTrafficDescrIndex	1, 3, 6, 1, 2, 1, 37, 1, 6, 1, 5

/* MIB object atmVplTransmitTrafficDescrIndex = atmVplEntry, 6 */
#define	I_atmVplTransmitTrafficDescrIndex	6
#define	O_atmVplTransmitTrafficDescrIndex	1, 3, 6, 1, 2, 1, 37, 1, 6, 1, 6

/* MIB object atmVplCrossConnectIdentifier = atmVplEntry, 7 */
#define	I_atmVplCrossConnectIdentifier	7
#define	O_atmVplCrossConnectIdentifier	1, 3, 6, 1, 2, 1, 37, 1, 6, 1, 7

/* MIB object atmVplRowStatus = atmVplEntry, 8 */
#define	I_atmVplRowStatus	8
#define	O_atmVplRowStatus	1, 3, 6, 1, 2, 1, 37, 1, 6, 1, 8

/* MIB object atmVplCastType = atmVplEntry, 9 */
#define	I_atmVplCastType	9
#define	O_atmVplCastType	1, 3, 6, 1, 2, 1, 37, 1, 6, 1, 9

/* MIB object atmVplConnKind = atmVplEntry, 10 */
#define	I_atmVplConnKind	10
#define	O_atmVplConnKind	1, 3, 6, 1, 2, 1, 37, 1, 6, 1, 10

/* MIB object atmVclTable = atmMIBObjects, 7 */
#define	I_atmVclTable	7
#define	O_atmVclTable	1, 3, 6, 1, 2, 1, 37, 1, 7

/* MIB object atmVclEntry = atmVclTable, 1 */
#define	I_atmVclEntry	1
#define	O_atmVclEntry	1, 3, 6, 1, 2, 1, 37, 1, 7, 1

/* MIB object atmVclVpi = atmVclEntry, 1 */
#define	I_atmVclVpi	1
#define	O_atmVclVpi	1, 3, 6, 1, 2, 1, 37, 1, 7, 1, 1

/* MIB object atmVclVci = atmVclEntry, 2 */
#define	I_atmVclVci	2
#define	O_atmVclVci	1, 3, 6, 1, 2, 1, 37, 1, 7, 1, 2

/* MIB object atmVclAdminStatus = atmVclEntry, 3 */
#define	I_atmVclAdminStatus	3
#define	O_atmVclAdminStatus	1, 3, 6, 1, 2, 1, 37, 1, 7, 1, 3

/* MIB object atmVclOperStatus = atmVclEntry, 4 */
#define	I_atmVclOperStatus	4
#define	O_atmVclOperStatus	1, 3, 6, 1, 2, 1, 37, 1, 7, 1, 4

/* MIB object atmVclLastChange = atmVclEntry, 5 */
#define	I_atmVclLastChange	5
#define	O_atmVclLastChange	1, 3, 6, 1, 2, 1, 37, 1, 7, 1, 5

/* MIB object atmVclReceiveTrafficDescrIndex = atmVclEntry, 6 */
#define	I_atmVclReceiveTrafficDescrIndex	6
#define	O_atmVclReceiveTrafficDescrIndex	1, 3, 6, 1, 2, 1, 37, 1, 7, 1, 6

/* MIB object atmVclTransmitTrafficDescrIndex = atmVclEntry, 7 */
#define	I_atmVclTransmitTrafficDescrIndex	7
#define	O_atmVclTransmitTrafficDescrIndex	1, 3, 6, 1, 2, 1, 37, 1, 7, 1, 7

/* MIB object atmVccAalType = atmVclEntry, 8 */
#define	I_atmVccAalType	8
#define	O_atmVccAalType	1, 3, 6, 1, 2, 1, 37, 1, 7, 1, 8

/* MIB object atmVccAal5CpcsTransmitSduSize = atmVclEntry, 9 */
#define	I_atmVccAal5CpcsTransmitSduSize	9
#define	O_atmVccAal5CpcsTransmitSduSize	1, 3, 6, 1, 2, 1, 37, 1, 7, 1, 9

/* MIB object atmVccAal5CpcsReceiveSduSize = atmVclEntry, 10 */
#define	I_atmVccAal5CpcsReceiveSduSize	10
#define	O_atmVccAal5CpcsReceiveSduSize	1, 3, 6, 1, 2, 1, 37, 1, 7, 1, 10

/* MIB object atmVccAal5EncapsType = atmVclEntry, 11 */
#define	I_atmVccAal5EncapsType	11
#define	O_atmVccAal5EncapsType	1, 3, 6, 1, 2, 1, 37, 1, 7, 1, 11

/* MIB object atmVclCrossConnectIdentifier = atmVclEntry, 12 */
#define	I_atmVclCrossConnectIdentifier	12
#define	O_atmVclCrossConnectIdentifier	1, 3, 6, 1, 2, 1, 37, 1, 7, 1, 12

/* MIB object atmVclRowStatus = atmVclEntry, 13 */
#define	I_atmVclRowStatus	13
#define	O_atmVclRowStatus	1, 3, 6, 1, 2, 1, 37, 1, 7, 1, 13

/* MIB object atmVclCastType = atmVclEntry, 14 */
#define	I_atmVclCastType	14
#define	O_atmVclCastType	1, 3, 6, 1, 2, 1, 37, 1, 7, 1, 14

/* MIB object atmVclConnKind = atmVclEntry, 15 */
#define	I_atmVclConnKind	15
#define	O_atmVclConnKind	1, 3, 6, 1, 2, 1, 37, 1, 7, 1, 15

/* MIB object atmVpCrossConnectIndexNext = atmMIBObjects, 8 */
#define	I_atmVpCrossConnectIndexNext	8
#define	O_atmVpCrossConnectIndexNext	1, 3, 6, 1, 2, 1, 37, 1, 8

/* MIB object atmVpCrossConnectTable = atmMIBObjects, 9 */
#define	I_atmVpCrossConnectTable	9
#define	O_atmVpCrossConnectTable	1, 3, 6, 1, 2, 1, 37, 1, 9

/* MIB object atmVpCrossConnectEntry = atmVpCrossConnectTable, 1 */
#define	I_atmVpCrossConnectEntry	1
#define	O_atmVpCrossConnectEntry	1, 3, 6, 1, 2, 1, 37, 1, 9, 1

/* MIB object atmVpCrossConnectIndex = atmVpCrossConnectEntry, 1 */
#define	I_atmVpCrossConnectIndex	1
#define	O_atmVpCrossConnectIndex	1, 3, 6, 1, 2, 1, 37, 1, 9, 1, 1

/* MIB object atmVpCrossConnectLowIfIndex = atmVpCrossConnectEntry, 2 */
#define	I_atmVpCrossConnectLowIfIndex	2
#define	O_atmVpCrossConnectLowIfIndex	1, 3, 6, 1, 2, 1, 37, 1, 9, 1, 2

/* MIB object atmVpCrossConnectLowVpi = atmVpCrossConnectEntry, 3 */
#define	I_atmVpCrossConnectLowVpi	3
#define	O_atmVpCrossConnectLowVpi	1, 3, 6, 1, 2, 1, 37, 1, 9, 1, 3

/* MIB object atmVpCrossConnectHighIfIndex = atmVpCrossConnectEntry, 4 */
#define	I_atmVpCrossConnectHighIfIndex	4
#define	O_atmVpCrossConnectHighIfIndex	1, 3, 6, 1, 2, 1, 37, 1, 9, 1, 4

/* MIB object atmVpCrossConnectHighVpi = atmVpCrossConnectEntry, 5 */
#define	I_atmVpCrossConnectHighVpi	5
#define	O_atmVpCrossConnectHighVpi	1, 3, 6, 1, 2, 1, 37, 1, 9, 1, 5

/* MIB object atmVpCrossConnectAdminStatus = atmVpCrossConnectEntry, 6 */
#define	I_atmVpCrossConnectAdminStatus	6
#define	O_atmVpCrossConnectAdminStatus	1, 3, 6, 1, 2, 1, 37, 1, 9, 1, 6

/* MIB object atmVpCrossConnectL2HOperStatus = atmVpCrossConnectEntry, 7 */
#define	I_atmVpCrossConnectL2HOperStatus	7
#define	O_atmVpCrossConnectL2HOperStatus	1, 3, 6, 1, 2, 1, 37, 1, 9, 1, 7

/* MIB object atmVpCrossConnectH2LOperStatus = atmVpCrossConnectEntry, 8 */
#define	I_atmVpCrossConnectH2LOperStatus	8
#define	O_atmVpCrossConnectH2LOperStatus	1, 3, 6, 1, 2, 1, 37, 1, 9, 1, 8

/* MIB object atmVpCrossConnectL2HLastChange = atmVpCrossConnectEntry, 9 */
#define	I_atmVpCrossConnectL2HLastChange	9
#define	O_atmVpCrossConnectL2HLastChange	1, 3, 6, 1, 2, 1, 37, 1, 9, 1, 9

/* MIB object atmVpCrossConnectH2LLastChange = atmVpCrossConnectEntry, 10 */
#define	I_atmVpCrossConnectH2LLastChange	10
#define	O_atmVpCrossConnectH2LLastChange	1, 3, 6, 1, 2, 1, 37, 1, 9, 1, 10

/* MIB object atmVpCrossConnectRowStatus = atmVpCrossConnectEntry, 11 */
#define	I_atmVpCrossConnectRowStatus	11
#define	O_atmVpCrossConnectRowStatus	1, 3, 6, 1, 2, 1, 37, 1, 9, 1, 11

/* MIB object atmVcCrossConnectIndexNext = atmMIBObjects, 10 */
#define	I_atmVcCrossConnectIndexNext	10
#define	O_atmVcCrossConnectIndexNext	1, 3, 6, 1, 2, 1, 37, 1, 10

/* MIB object atmVcCrossConnectTable = atmMIBObjects, 11 */
#define	I_atmVcCrossConnectTable	11
#define	O_atmVcCrossConnectTable	1, 3, 6, 1, 2, 1, 37, 1, 11

/* MIB object atmVcCrossConnectEntry = atmVcCrossConnectTable, 1 */
#define	I_atmVcCrossConnectEntry	1
#define	O_atmVcCrossConnectEntry	1, 3, 6, 1, 2, 1, 37, 1, 11, 1

/* MIB object atmVcCrossConnectIndex = atmVcCrossConnectEntry, 1 */
#define	I_atmVcCrossConnectIndex	1
#define	O_atmVcCrossConnectIndex	1, 3, 6, 1, 2, 1, 37, 1, 11, 1, 1

/* MIB object atmVcCrossConnectLowIfIndex = atmVcCrossConnectEntry, 2 */
#define	I_atmVcCrossConnectLowIfIndex	2
#define	O_atmVcCrossConnectLowIfIndex	1, 3, 6, 1, 2, 1, 37, 1, 11, 1, 2

/* MIB object atmVcCrossConnectLowVpi = atmVcCrossConnectEntry, 3 */
#define	I_atmVcCrossConnectLowVpi	3
#define	O_atmVcCrossConnectLowVpi	1, 3, 6, 1, 2, 1, 37, 1, 11, 1, 3

/* MIB object atmVcCrossConnectLowVci = atmVcCrossConnectEntry, 4 */
#define	I_atmVcCrossConnectLowVci	4
#define	O_atmVcCrossConnectLowVci	1, 3, 6, 1, 2, 1, 37, 1, 11, 1, 4

/* MIB object atmVcCrossConnectHighIfIndex = atmVcCrossConnectEntry, 5 */
#define	I_atmVcCrossConnectHighIfIndex	5
#define	O_atmVcCrossConnectHighIfIndex	1, 3, 6, 1, 2, 1, 37, 1, 11, 1, 5

/* MIB object atmVcCrossConnectHighVpi = atmVcCrossConnectEntry, 6 */
#define	I_atmVcCrossConnectHighVpi	6
#define	O_atmVcCrossConnectHighVpi	1, 3, 6, 1, 2, 1, 37, 1, 11, 1, 6

/* MIB object atmVcCrossConnectHighVci = atmVcCrossConnectEntry, 7 */
#define	I_atmVcCrossConnectHighVci	7
#define	O_atmVcCrossConnectHighVci	1, 3, 6, 1, 2, 1, 37, 1, 11, 1, 7

/* MIB object atmVcCrossConnectAdminStatus = atmVcCrossConnectEntry, 8 */
#define	I_atmVcCrossConnectAdminStatus	8
#define	O_atmVcCrossConnectAdminStatus	1, 3, 6, 1, 2, 1, 37, 1, 11, 1, 8

/* MIB object atmVcCrossConnectL2HOperStatus = atmVcCrossConnectEntry, 9 */
#define	I_atmVcCrossConnectL2HOperStatus	9
#define	O_atmVcCrossConnectL2HOperStatus	1, 3, 6, 1, 2, 1, 37, 1, 11, 1, 9

/* MIB object atmVcCrossConnectH2LOperStatus = atmVcCrossConnectEntry, 10 */
#define	I_atmVcCrossConnectH2LOperStatus	10
#define	O_atmVcCrossConnectH2LOperStatus	1, 3, 6, 1, 2, 1, 37, 1, 11, 1, 10

/* MIB object atmVcCrossConnectL2HLastChange = atmVcCrossConnectEntry, 11 */
#define	I_atmVcCrossConnectL2HLastChange	11
#define	O_atmVcCrossConnectL2HLastChange	1, 3, 6, 1, 2, 1, 37, 1, 11, 1, 11

/* MIB object atmVcCrossConnectH2LLastChange = atmVcCrossConnectEntry, 12 */
#define	I_atmVcCrossConnectH2LLastChange	12
#define	O_atmVcCrossConnectH2LLastChange	1, 3, 6, 1, 2, 1, 37, 1, 11, 1, 12

/* MIB object atmVcCrossConnectRowStatus = atmVcCrossConnectEntry, 13 */
#define	I_atmVcCrossConnectRowStatus	13
#define	O_atmVcCrossConnectRowStatus	1, 3, 6, 1, 2, 1, 37, 1, 11, 1, 13

/* MIB object aal5VccTable = atmMIBObjects, 12 */
#define	I_aal5VccTable	12
#define	O_aal5VccTable	1, 3, 6, 1, 2, 1, 37, 1, 12

/* MIB object aal5VccEntry = aal5VccTable, 1 */
#define	I_aal5VccEntry	1
#define	O_aal5VccEntry	1, 3, 6, 1, 2, 1, 37, 1, 12, 1

/* MIB object aal5VccVpi = aal5VccEntry, 1 */
#define	I_aal5VccVpi	1
#define	O_aal5VccVpi	1, 3, 6, 1, 2, 1, 37, 1, 12, 1, 1

/* MIB object aal5VccVci = aal5VccEntry, 2 */
#define	I_aal5VccVci	2
#define	O_aal5VccVci	1, 3, 6, 1, 2, 1, 37, 1, 12, 1, 2

/* MIB object aal5VccCrcErrors = aal5VccEntry, 3 */
#define	I_aal5VccCrcErrors	3
#define	O_aal5VccCrcErrors	1, 3, 6, 1, 2, 1, 37, 1, 12, 1, 3

/* MIB object aal5VccSarTimeOuts = aal5VccEntry, 4 */
#define	I_aal5VccSarTimeOuts	4
#define	O_aal5VccSarTimeOuts	1, 3, 6, 1, 2, 1, 37, 1, 12, 1, 4

/* MIB object aal5VccOverSizedSDUs = aal5VccEntry, 5 */
#define	I_aal5VccOverSizedSDUs	5
#define	O_aal5VccOverSizedSDUs	1, 3, 6, 1, 2, 1, 37, 1, 12, 1, 5

/* MIB object atmTrafficDescrParamIndexNext = atmMIBObjects, 13 */
#define	I_atmTrafficDescrParamIndexNext	13
#define	O_atmTrafficDescrParamIndexNext	1, 3, 6, 1, 2, 1, 37, 1, 13

/* MIB object atmMIBConformance = atmMIB, 2 */
#define	I_atmMIBConformance	2
#define	O_atmMIBConformance	1, 3, 6, 1, 2, 1, 37, 2

/* MIB object atmMIBGroups = atmMIBConformance, 1 */
#define	I_atmMIBGroups	1
#define	O_atmMIBGroups	1, 3, 6, 1, 2, 1, 37, 2, 1

/* MIB object atmMIBCompliances = atmMIBConformance, 2 */
#define	I_atmMIBCompliances	2
#define	O_atmMIBCompliances	1, 3, 6, 1, 2, 1, 37, 2, 2

/* MIB object atmMIBCompliance2 = atmMIBCompliances, 2 */
#define	I_atmMIBCompliance2	2
#define	O_atmMIBCompliance2	1, 3, 6, 1, 2, 1, 37, 2, 2, 2

/* MIB object atmInterfaceDs3PlcpGroup = atmMIBGroups, 3 */
#define	I_atmInterfaceDs3PlcpGroup	3
#define	O_atmInterfaceDs3PlcpGroup	1, 3, 6, 1, 2, 1, 37, 2, 1, 3

/* MIB object atmInterfaceTCGroup = atmMIBGroups, 4 */
#define	I_atmInterfaceTCGroup	4
#define	O_atmInterfaceTCGroup	1, 3, 6, 1, 2, 1, 37, 2, 1, 4

/* MIB object aal5VccGroup = atmMIBGroups, 9 */
#define	I_aal5VccGroup	9
#define	O_aal5VccGroup	1, 3, 6, 1, 2, 1, 37, 2, 1, 9

/* MIB object atmInterfaceConfGroup2 = atmMIBGroups, 10 */
#define	I_atmInterfaceConfGroup2	10
#define	O_atmInterfaceConfGroup2	1, 3, 6, 1, 2, 1, 37, 2, 1, 10

/* MIB object atmTrafficDescrGroup2 = atmMIBGroups, 11 */
#define	I_atmTrafficDescrGroup2	11
#define	O_atmTrafficDescrGroup2	1, 3, 6, 1, 2, 1, 37, 2, 1, 11

/* MIB object atmVpcTerminationGroup2 = atmMIBGroups, 12 */
#define	I_atmVpcTerminationGroup2	12
#define	O_atmVpcTerminationGroup2	1, 3, 6, 1, 2, 1, 37, 2, 1, 12

/* MIB object atmVccTerminationGroup2 = atmMIBGroups, 13 */
#define	I_atmVccTerminationGroup2	13
#define	O_atmVccTerminationGroup2	1, 3, 6, 1, 2, 1, 37, 2, 1, 13

/* MIB object atmVplCrossConnectGroup = atmMIBGroups, 14 */
#define	I_atmVplCrossConnectGroup	14
#define	O_atmVplCrossConnectGroup	1, 3, 6, 1, 2, 1, 37, 2, 1, 14

/* MIB object atmVpPvcCrossConnectGroup = atmMIBGroups, 15 */
#define	I_atmVpPvcCrossConnectGroup	15
#define	O_atmVpPvcCrossConnectGroup	1, 3, 6, 1, 2, 1, 37, 2, 1, 15

/* MIB object atmVclCrossConnectGroup = atmMIBGroups, 16 */
#define	I_atmVclCrossConnectGroup	16
#define	O_atmVclCrossConnectGroup	1, 3, 6, 1, 2, 1, 37, 2, 1, 16

/* MIB object atmVcPvcCrossConnectGroup = atmMIBGroups, 17 */
#define	I_atmVcPvcCrossConnectGroup	17
#define	O_atmVcPvcCrossConnectGroup	1, 3, 6, 1, 2, 1, 37, 2, 1, 17

/* MIB object atmMIBCompliance = atmMIBCompliances, 1 */
#define	I_atmMIBCompliance	1
#define	O_atmMIBCompliance	1, 3, 6, 1, 2, 1, 37, 2, 2, 1

/* MIB object atmInterfaceConfGroup = atmMIBGroups, 1 */
#define	I_atmInterfaceConfGroup	1
#define	O_atmInterfaceConfGroup	1, 3, 6, 1, 2, 1, 37, 2, 1, 1

/* MIB object atmTrafficDescrGroup = atmMIBGroups, 2 */
#define	I_atmTrafficDescrGroup	2
#define	O_atmTrafficDescrGroup	1, 3, 6, 1, 2, 1, 37, 2, 1, 2

/* MIB object atmVpcTerminationGroup = atmMIBGroups, 5 */
#define	I_atmVpcTerminationGroup	5
#define	O_atmVpcTerminationGroup	1, 3, 6, 1, 2, 1, 37, 2, 1, 5

/* MIB object atmVccTerminationGroup = atmMIBGroups, 6 */
#define	I_atmVccTerminationGroup	6
#define	O_atmVccTerminationGroup	1, 3, 6, 1, 2, 1, 37, 2, 1, 6

/* MIB object atmVpCrossConnectGroup = atmMIBGroups, 7 */
#define	I_atmVpCrossConnectGroup	7
#define	O_atmVpCrossConnectGroup	1, 3, 6, 1, 2, 1, 37, 2, 1, 7

/* MIB object atmVcCrossConnectGroup = atmMIBGroups, 8 */
#define	I_atmVcCrossConnectGroup	8
#define	O_atmVcCrossConnectGroup	1, 3, 6, 1, 2, 1, 37, 2, 1, 8

#endif	/* _ATM_MIB_ */
