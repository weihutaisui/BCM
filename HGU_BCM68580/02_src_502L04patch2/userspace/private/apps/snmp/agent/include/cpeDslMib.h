#ifndef	_CPE_DSL_MIB_
#define	_CPE_DSL_MIB_

/* required include files (IMPORTS) */
#include	"snmpv2Tc.h"
#include	"snmpv2Smi.h"
#include	"ifMib.h"
#include	"atmMib.h"

/* defined objects in this module */

/* MIB object cpeDSL = enterprises, 16972 */
#define	I_cpeDSL	16972
#define	O_cpeDSL	1, 3, 6, 1, 4, 1, 16972

/* MIB object cpeSysInfo = cpeDSL, 1 */
#define	I_cpeSysInfo	1
#define	O_cpeSysInfo	1, 3, 6, 1, 4, 1, 16972, 1

/* MIB object cpeSysInfoUptime = cpeSysInfo, 1 */
#define	I_cpeSysInfoUptime	1
#define	O_cpeSysInfoUptime	1, 3, 6, 1, 4, 1, 16972, 1, 1

/* MIB object cpeSysInfoConfigId = cpeSysInfo, 2 */
#define	I_cpeSysInfoConfigId	2
#define	O_cpeSysInfoConfigId	1, 3, 6, 1, 4, 1, 16972, 1, 2

/* MIB object cpeSysInfoFirmwareVersion = cpeSysInfo, 3 */
#define	I_cpeSysInfoFirmwareVersion	3
#define	O_cpeSysInfoFirmwareVersion	1, 3, 6, 1, 4, 1, 16972, 1, 3

/* MIB object cpeSysInfoProductId = cpeSysInfo, 4 */
#define	I_cpeSysInfoProductId	4
#define	O_cpeSysInfoProductId	1, 3, 6, 1, 4, 1, 16972, 1, 4

/* MIB object cpeSysInfoVendorId = cpeSysInfo, 5 */
#define	I_cpeSysInfoVendorId	5
#define	O_cpeSysInfoVendorId	1, 3, 6, 1, 4, 1, 16972, 1, 5

/* MIB object cpeSysInfoSystemVersion = cpeSysInfo, 6 */
#define	I_cpeSysInfoSystemVersion	6
#define	O_cpeSysInfoSystemVersion	1, 3, 6, 1, 4, 1, 16972, 1, 6

/* MIB object cpeSysInfoWorkMode = cpeSysInfo, 7 */
#define	I_cpeSysInfoWorkMode	7
#define	O_cpeSysInfoWorkMode	1, 3, 6, 1, 4, 1, 16972, 1, 7

/* MIB object cpeSysInfoMibVersion = cpeSysInfo, 8 */
#define	I_cpeSysInfoMibVersion	8
#define	O_cpeSysInfoMibVersion	1, 3, 6, 1, 4, 1, 16972, 1, 8

/* MIB object cpeSysInfoSerialNumber = cpeSysInfo, 9 */
#define	I_cpeSysInfoSerialNumber	9
#define	O_cpeSysInfoSerialNumber	1, 3, 6, 1, 4, 1, 16972, 1, 9

/* MIB object cpeSysInfoManufacturerOui = cpeSysInfo, 10 */
#define	I_cpeSysInfoManufacturerOui	10
#define	O_cpeSysInfoManufacturerOui	1, 3, 6, 1, 4, 1, 16972, 1, 10

#ifdef CT_SNMP_MIB_VERSION_2
/* MIB object cpeSysInfoType = cpeSysInfo, 11 */
#define I_cpeSysInfoType 11
#define O_cpeSysInfoType 1, 3, 6, 1, 4, 1, 16972, 1, 11
#endif

/* MIB object cpeConfig = cpeDSL, 2 */
#define	I_cpeConfig	2
#define	O_cpeConfig	1, 3, 6, 1, 4, 1, 16972, 2

/* MIB object cpeSystem = cpeConfig, 1 */
#define	I_cpeSystem	1
#define	O_cpeSystem	1, 3, 6, 1, 4, 1, 16972, 2, 1

/* MIB object cpeSystemReboot = cpeSystem, 1 */
#define	I_cpeSystemReboot	1
#define	O_cpeSystemReboot	1, 3, 6, 1, 4, 1, 16972, 2, 1, 1

/* MIB object cpeSystemSave = cpeSystem, 2 */
#define	I_cpeSystemSave	2
#define	O_cpeSystemSave	1, 3, 6, 1, 4, 1, 16972, 2, 1, 2

/* MIB object cpeSystemRestore = cpeSystem, 3 */
#define	I_cpeSystemRestore	3
#define	O_cpeSystemRestore	1, 3, 6, 1, 4, 1, 16972, 2, 1, 3

/* MIB object cpeSystemTrapEnable = cpeSystem, 4 */
#define	I_cpeSystemTrapEnable	4
#define	O_cpeSystemTrapEnable	1, 3, 6, 1, 4, 1, 16972, 2, 1, 4

/* MIB object cpeBootTrapResponse = cpeSystem, 5 */
#define	I_cpeBootTrapResponse	5
#define	O_cpeBootTrapResponse	1, 3, 6, 1, 4, 1, 16972, 2, 1, 5

/* MIB object cpeReservedString = cpeSystem, 6 */
#define	I_cpeReservedString	6
#define	O_cpeReservedString	1, 3, 6, 1, 4, 1, 16972, 2, 1, 6

/* MIB object cpeConfigAdslLine = cpeConfig, 2 */
#define	I_cpeConfigAdslLine	2
#define	O_cpeConfigAdslLine	1, 3, 6, 1, 4, 1, 16972, 2, 2

/* MIB object cpeConfigAdslLineTrellis = cpeConfigAdslLine, 1 */
#define	I_cpeConfigAdslLineTrellis	1
#define	O_cpeConfigAdslLineTrellis	1, 3, 6, 1, 4, 1, 16972, 2, 2, 1

/* MIB object cpeConfigAdslLineHandshake = cpeConfigAdslLine, 2 */
#define	I_cpeConfigAdslLineHandshake	2
#define	O_cpeConfigAdslLineHandshake	1, 3, 6, 1, 4, 1, 16972, 2, 2, 2

/* MIB object cpeConfigAdslLineBitSwapping = cpeConfigAdslLine, 3 */
#define	I_cpeConfigAdslLineBitSwapping	3
#define	O_cpeConfigAdslLineBitSwapping	1, 3, 6, 1, 4, 1, 16972, 2, 2, 3

/* MIB object cpePvcTable = cpeConfig, 3 */
#define	I_cpePvcTable	3
#define	O_cpePvcTable	1, 3, 6, 1, 4, 1, 16972, 2, 3

/* MIB object cpePvcEntry = cpePvcTable, 1 */
#define	I_cpePvcEntry	1
#define	O_cpePvcEntry	1, 3, 6, 1, 4, 1, 16972, 2, 3, 1

/* MIB object cpePvcId = cpePvcEntry, 1 */
#define	I_cpePvcId	1
#define	O_cpePvcId	1, 3, 6, 1, 4, 1, 16972, 2, 3, 1, 1

/* MIB object cpePvcIfIndex = cpePvcEntry, 3 */
#define	I_cpePvcIfIndex	3
#define	O_cpePvcIfIndex	1, 3, 6, 1, 4, 1, 16972, 2, 3, 1, 3

/* MIB object cpePvcUpperEncapsulation = cpePvcEntry, 4 */
#define	I_cpePvcUpperEncapsulation	4
#define	O_cpePvcUpperEncapsulation	1, 3, 6, 1, 4, 1, 16972, 2, 3, 1, 4

/* MIB object cpePvcAdminStatus = cpePvcEntry, 5 */
#define	I_cpePvcAdminStatus	5
#define	O_cpePvcAdminStatus	1, 3, 6, 1, 4, 1, 16972, 2, 3, 1, 5

/* MIB object cpePvcOperStatus = cpePvcEntry, 6 */
#define	I_cpePvcOperStatus	6
#define	O_cpePvcOperStatus	1, 3, 6, 1, 4, 1, 16972, 2, 3, 1, 6

/* MIB object cpePvcBridgeMode = cpePvcEntry, 7 */
#define	I_cpePvcBridgeMode	7
#define	O_cpePvcBridgeMode	1, 3, 6, 1, 4, 1, 16972, 2, 3, 1, 7

/* MIB object cpePvcMacLearnMode = cpePvcEntry, 8 */
#define	I_cpePvcMacLearnMode	8
#define	O_cpePvcMacLearnMode	1, 3, 6, 1, 4, 1, 16972, 2, 3, 1, 8

/* MIB object cpePvcIgmpMode = cpePvcEntry, 9 */
#define	I_cpePvcIgmpMode	9
#define	O_cpePvcIgmpMode	1, 3, 6, 1, 4, 1, 16972, 2, 3, 1, 9

/* MIB object cpePvcDhcpClientMode = cpePvcEntry, 10 */
#define	I_cpePvcDhcpClientMode	10
#define	O_cpePvcDhcpClientMode	1, 3, 6, 1, 4, 1, 16972, 2, 3, 1, 10

/* MIB object cpePvcNatMode = cpePvcEntry, 11 */
#define	I_cpePvcNatMode	11
#define	O_cpePvcNatMode	1, 3, 6, 1, 4, 1, 16972, 2, 3, 1, 11

#ifdef CT_SNMP_MIB_VERSION_2
/* MIB object cpePvcBindingMode = cpePvcEntry, 12 */
#define I_cpePvcBindingMode 12
#define O_cpePvcBindingMode 1, 3, 6, 1, 4, 1, 16972, 2, 3, 1, 12

/* MIB object cpePvcEthernetPortMap = cpePvcEntry, 13 */
#define I_cpePvcEthernetPortMap 13
#define O_cpePvcEthernetPortMap 1, 3, 6, 1, 4, 1, 16972, 2, 3, 1, 13

/* MIB object cpePvcServiceTypeIndex = cpePvcEntry, 14 */
#define I_cpePvcServiceTypeIndex 14
#define O_cpePvcServiceTypeIndex 1, 3, 6, 1, 4, 1, 16972, 2, 3, 1, 14
#endif

/* MIB object cpePppTable = cpeConfig, 4 */
#define	I_cpePppTable	4
#define	O_cpePppTable	1, 3, 6, 1, 4, 1, 16972, 2, 4

/* MIB object cpePppEntry = cpePppTable, 1 */
#define	I_cpePppEntry	1
#define	O_cpePppEntry	1, 3, 6, 1, 4, 1, 16972, 2, 4, 1

/* MIB object cpePppIfIndex = cpePppEntry, 1 */
#define	I_cpePppIfIndex	1
#define	O_cpePppIfIndex	1, 3, 6, 1, 4, 1, 16972, 2, 4, 1, 1

/* MIB object cpePppPvcIfIndex = cpePppEntry, 2 */
#define	I_cpePppPvcIfIndex	2
#define	O_cpePppPvcIfIndex	1, 3, 6, 1, 4, 1, 16972, 2, 4, 1, 2

/* MIB object cpePppServiceName = cpePppEntry, 3 */
#define	I_cpePppServiceName	3
#define	O_cpePppServiceName	1, 3, 6, 1, 4, 1, 16972, 2, 4, 1, 3

/* MIB object cpePppDisconnectTimeout = cpePppEntry, 4 */
#define	I_cpePppDisconnectTimeout	4
#define	O_cpePppDisconnectTimeout	1, 3, 6, 1, 4, 1, 16972, 2, 4, 1, 4

/* MIB object cpePppMSS = cpePppEntry, 5 */
#define	I_cpePppMSS	5
#define	O_cpePppMSS	1, 3, 6, 1, 4, 1, 16972, 2, 4, 1, 5

/* MIB object cpePppRowStatus = cpePppEntry, 6 */
#define	I_cpePppRowStatus	6
#define	O_cpePppRowStatus	1, 3, 6, 1, 4, 1, 16972, 2, 4, 1, 6

/* MIB object cpePppAdminStatus = cpePppEntry, 7 */
#define	I_cpePppAdminStatus	7
#define	O_cpePppAdminStatus	1, 3, 6, 1, 4, 1, 16972, 2, 4, 1, 7

/* MIB object cpePppOperStatus = cpePppEntry, 8 */
#define	I_cpePppOperStatus	8
#define	O_cpePppOperStatus	1, 3, 6, 1, 4, 1, 16972, 2, 4, 1, 8

/* MIB object cpePppTestResult = cpePppEntry, 9 */
#define	I_cpePppTestResult	9
#define	O_cpePppTestResult	1, 3, 6, 1, 4, 1, 16972, 2, 4, 1, 9

/* MIB object cpeIpTable = cpeConfig, 5 */
#define	I_cpeIpTable	5
#define	O_cpeIpTable	1, 3, 6, 1, 4, 1, 16972, 2, 5

/* MIB object cpeIpEntry = cpeIpTable, 1 */
#define	I_cpeIpEntry	1
#define	O_cpeIpEntry	1, 3, 6, 1, 4, 1, 16972, 2, 5, 1

/* MIB object cpeIpIndex = cpeIpEntry, 1 */
#define	I_cpeIpIndex	1
#define	O_cpeIpIndex	1, 3, 6, 1, 4, 1, 16972, 2, 5, 1, 1

/* MIB object cpeIpLowerIfIndex = cpeIpEntry, 2 */
#define	I_cpeIpLowerIfIndex	2
#define	O_cpeIpLowerIfIndex	1, 3, 6, 1, 4, 1, 16972, 2, 5, 1, 2

/* MIB object cpeIpAddress = cpeIpEntry, 3 */
#define	I_cpeIpAddress	3
#define	O_cpeIpAddress	1, 3, 6, 1, 4, 1, 16972, 2, 5, 1, 3

/* MIB object cpeIpNetmask = cpeIpEntry, 4 */
#define	I_cpeIpNetmask	4
#define	O_cpeIpNetmask	1, 3, 6, 1, 4, 1, 16972, 2, 5, 1, 4

/* MIB object cpeIpGateway = cpeIpEntry, 5 */
#define	I_cpeIpGateway	5
#define	O_cpeIpGateway	1, 3, 6, 1, 4, 1, 16972, 2, 5, 1, 5

/* MIB object cpeIpRowStatus = cpeIpEntry, 6 */
#define	I_cpeIpRowStatus	6
#define	O_cpeIpRowStatus	1, 3, 6, 1, 4, 1, 16972, 2, 5, 1, 6

/* MIB object cpeIpRowStatus = cpeIpEntry, 7 */
#define	I_cpeIpDefaultRowOnOff	7
#define	O_cpeIpDefaultRowOnOff	1, 3, 6, 1, 4, 1, 16972, 2, 5, 1, 7

/* MIB object cpeLanDhcp = cpeConfig, 6 */
#define	I_cpeLanDhcp	6
#define	O_cpeLanDhcp	1, 3, 6, 1, 4, 1, 16972, 2, 6

/* MIB object cpeLanDhcpAdminStatus = cpeLanDhcp, 1 */
#define	I_cpeLanDhcpAdminStatus	1
#define	O_cpeLanDhcpAdminStatus	1, 3, 6, 1, 4, 1, 16972, 2, 6, 1

#ifdef CT_SNMP_MIB_VERSION_2
/* MIB object cpeDhcpRelayIpAddress = cpeLanDhcp, 2*/
#define I_cpeDhcpRelayIpAddress 2
#define O_cpeDhcpRelayIpAddress 1, 3, 6, 1, 4, 1, 16972, 2, 6, 2
#endif
/* MIB object cpeWanDns = cpeConfig, 7 */
#define	I_cpeWanDns	7
#define	O_cpeWanDns	1, 3, 6, 1, 4, 1, 16972, 2, 7

/* MIB object cpeDnsPrimaryAddress = cpeWanDns, 1 */
#define	I_cpeDnsPrimaryAddress	1
#define	O_cpeDnsPrimaryAddress	1, 3, 6, 1, 4, 1, 16972, 2, 7, 1

/* MIB object cpeDnsSecondAddress = cpeWanDns, 2 */
#define	I_cpeDnsSecondAddress	2
#define	O_cpeDnsSecondAddress	1, 3, 6, 1, 4, 1, 16972, 2, 7, 2

#ifdef CT_SNMP_MIB_VERSION_2
/* MIB object cpeQos = cpeConfig, 8 */
#define I_cpeQos 		8
#define O_cpeQos        1, 3, 6, 1, 4, 1, 16972, 2, 8

/* MIB object cpeQosTagMode = cpeQos, 1*/
#define I_cpeQosTagMode		1
#define O_cpeQosTagMode		1, 3, 6, 1, 4, 1, 16972, 2, 8 ,1

/* MIB object cpeEthernetPortTable = cpeConfig, 9 */
#define I_cpeEthernetPortTable		9
#define O_cpeEthernetPortTable		1, 3, 6, 1, 4, 1, 16972, 2, 9

/* MIB object cpeEthernetPortEntry = cpeEthernetPortTable, 1 */
#define I_cpeEthernetPortEntry		1
#define O_cpeEthernetPortEntry		1, 3, 6, 1, 4, 1, 16972, 2, 9, 1

/* MIB object cpeEthernetPortIfIndex = cpeEthernetPortEntry, 1 */
#define I_cpeEthernetPortIfIndex		1
#define O_cpeEthernetPortIfIndex		1, 3, 6, 1, 4, 1, 16972, 2, 9, 1 ,1

/* MIB object cpeEthernetPortSerialNumber = cpeEthernetPortEntry, 2 */
#define I_cpeEthernetPortSerialNumber		2
#define O_cpeEthernetPortSerialNumber		1, 3, 6, 1, 4, 1, 16972, 2, 9, 1 ,2

/* MIB object cpeEthernetPortBindingMode= cpeEthernetPortEntry, 3 */
#define I_cpeEthernetPortBindingMode		3
#define O_cpeEthernetPortBindingMode		1, 3, 6, 1, 4, 1, 16972, 2, 9, 1 ,3

/* MIB object cpeEthernetPortIgmpOnOff = cpeEthernetPortEntry, 4 */
#define I_cpeEthernetPortIgmpOnOff		4
#define O_cpeEthernetPortIgmpOnOff		1, 3, 6, 1, 4, 1, 16972, 2, 9, 1 ,4

/* MIB object cpeEthernetPortI8021dParameter = cpeEthernetPortEntry, 5 */
#define I_cpeEthernetPortI8021dParameter		5
#define O_cpeEthernetPortI8021dParameter		1, 3, 6, 1, 4, 1, 16972, 2, 9, 1 ,5

/* MIB object cpeServiceTypeTable = cpeConfig, 10 */
#define I_cpeServiceTypeTable		10
#define O_cpeServiceTypeTable		1, 3, 6, 1, 4, 1, 16972, 2, 10

/* MIB object cpeServiceTypeEntry = cpeServiceTypeTable, 1 */
#define I_cpeServiceTypeEntry		1
#define O_cpeServiceTypeEntry		1, 3, 6, 1, 4, 1, 16972, 2, 10, 1

/* MIB object cpeServiceTypeIndex = cpeServiceTypeEntry, 1 */
#define I_cpeServiceTypeIndex		1
#define O_cpeServiceTypeIndex		1, 3, 6, 1, 4, 1, 16972, 2, 10, 1 ,1

/* MIB object cpePvcServiceType = cpeServiceTypeEntry, 2 */
#define I_cpePvcServiceType		2
#define O_cpePvcServiceType		1, 3, 6, 1, 4, 1, 16972, 2, 10, 1 ,2

/* MIB object cpePvcPcr = cpeServiceTypeEntry, 3 */
#define I_cpePvcPcr			 	3
#define O_cpePvcPcr			 	1, 3, 6, 1, 4, 1, 16972, 2, 10, 1 ,3

/* MIB object cpePvcMcr = cpeServiceTypeEntry, 4 */
#define I_cpePvcMcr				4
#define O_cpePvcMcr			 	1, 3, 6, 1, 4, 1, 16972, 2, 10, 1 ,4

/* MIB object cpePvcScr = cpeServiceTypeEntry, 5 */
#define I_cpePvcScr 				5
#define O_cpePvcScr 				1, 3, 6, 1, 4, 1, 16972, 2, 10, 1 ,5

/* MIB object cpeServiceTypeRowStatus = cpeServiceTypeEntry, 6*/
#define I_cpeServiceTypeRowStatus	6
#define O_cpeServcieTypeRowStatus 1, 3, 6, 1, 4, 1, 16972, 2, 10, 1 ,6

/* MIB object cpeAdmin = cpeConfig, 10 */
#define I_cpeAdmin				11	
#define O_cpeAdmin				1, 3, 6, 1, 4, 1, 16972, 2, 11

/* MIB object cpeAdminIdentity = cpeAdmin, 11*/
#define I_cpeAdminIdentity				1	
#define O_cpeAdminIdentity			1, 3, 6, 1, 4, 1, 16972, 2, 11, 1

/* MIB object cpeAdminPassword = cpeAdmin, 11*/
#define I_cpeAdminPassword			2	
#define O_cpeAdminPassword			1, 3, 6, 1, 4, 1, 16972, 2, 11, 2

#endif

/* MIB object cpeSysLog = cpeDSL, 3 */
#define	I_cpeSysLog	3
#define	O_cpeSysLog	1, 3, 6, 1, 4, 1, 16972, 3

/* MIB object cpeSysLogBeginTime = cpeSysLog, 1 */
#define	I_cpeSysLogBeginTime	1
#define	O_cpeSysLogBeginTime	1, 3, 6, 1, 4, 1, 16972, 3, 1

/* MIB object cpeSysLogEndTime = cpeSysLog, 2 */
#define	I_cpeSysLogEndTime	2
#define	O_cpeSysLogEndTime	1, 3, 6, 1, 4, 1, 16972, 3, 2

/* MIB object cpeSysLogAdminStatus = cpeSysLog, 3 */
#define	I_cpeSysLogAdminStatus	3
#define	O_cpeSysLogAdminStatus	1, 3, 6, 1, 4, 1, 16972, 3, 3

/* MIB object cpeSysLogTable = cpeSysLog, 4 */
#define	I_cpeSysLogTable	4
#define	O_cpeSysLogTable	1, 3, 6, 1, 4, 1, 16972, 3, 4

/* MIB object cpeSysLogEntry = cpeSysLogTable, 1 */
#define	I_cpeSysLogEntry	1
#define	O_cpeSysLogEntry	1, 3, 6, 1, 4, 1, 16972, 3, 4, 1

/* MIB object cpeSysLogIndex = cpeSysLogEntry, 1 */
#define	I_cpeSysLogIndex	1
#define	O_cpeSysLogIndex	1, 3, 6, 1, 4, 1, 16972, 3, 4, 1, 1

/* MIB object cpeSysLogContent = cpeSysLogEntry, 2 */
#define	I_cpeSysLogContent	2
#define	O_cpeSysLogContent	1, 3, 6, 1, 4, 1, 16972, 3, 4, 1, 2

/* MIB object cpeFtp = cpeDSL, 4 */
#define	I_cpeFtp	4
#define	O_cpeFtp	1, 3, 6, 1, 4, 1, 16972, 4

/* MIB object cpeFtpIp = cpeFtp, 1 */
#define	I_cpeFtpIp	1
#define	O_cpeFtpIp	1, 3, 6, 1, 4, 1, 16972, 4, 1

/* MIB object cpeFtpUser = cpeFtp, 2 */
#define	I_cpeFtpUser	2
#define	O_cpeFtpUser	1, 3, 6, 1, 4, 1, 16972, 4, 2

/* MIB object cpeFtpPasswd = cpeFtp, 3 */
#define	I_cpeFtpPasswd	3
#define	O_cpeFtpPasswd	1, 3, 6, 1, 4, 1, 16972, 4, 3

/* MIB object cpeFtpFileName = cpeFtp, 4 */
#define	I_cpeFtpFileName	4
#define	O_cpeFtpFileName	1, 3, 6, 1, 4, 1, 16972, 4, 4

/* MIB object cpeFtpAdminStatus = cpeFtp, 5 */
#define	I_cpeFtpAdminStatus	5
#define	O_cpeFtpAdminStatus	1, 3, 6, 1, 4, 1, 16972, 4, 5

/* MIB object cpeFtpOperStatus = cpeFtp, 6 */
#define	I_cpeFtpOperStatus	6
#define	O_cpeFtpOperStatus	1, 3, 6, 1, 4, 1, 16972, 4, 6

/* MIB object cpeFtpTotalSize = cpeFtp, 8 */
#define	I_cpeFtpTotalSize	8
#define	O_cpeFtpTotalSize	1, 3, 6, 1, 4, 1, 16972, 4, 8

/* MIB object cpeFtpDoneSize = cpeFtp, 9 */
#define	I_cpeFtpDoneSize	9
#define	O_cpeFtpDoneSize	1, 3, 6, 1, 4, 1, 16972, 4, 9

/* MIB object cpeFtpElapseTime = cpeFtp, 10 */
#define	I_cpeFtpElapseTime	10
#define	O_cpeFtpElapseTime	1, 3, 6, 1, 4, 1, 16972, 4, 10

/* MIB object cpeDiagnose = cpeDSL, 5 */
#define	I_cpeDiagnose	5
#define	O_cpeDiagnose	1, 3, 6, 1, 4, 1, 16972, 5

/* MIB object cpePing = cpeDiagnose, 1 */
#define	I_cpePing	1
#define	O_cpePing	1, 3, 6, 1, 4, 1, 16972, 5, 1

/* MIB object cpePingAddr = cpePing, 1 */
#define	I_cpePingAddr	1
#define	O_cpePingAddr	1, 3, 6, 1, 4, 1, 16972, 5, 1, 1

/* MIB object cpePingPkgSize = cpePing, 2 */
#define	I_cpePingPkgSize	2
#define	O_cpePingPkgSize	1, 3, 6, 1, 4, 1, 16972, 5, 1, 2

/* MIB object cpePingCount = cpePing, 3 */
#define	I_cpePingCount	3
#define	O_cpePingCount	1, 3, 6, 1, 4, 1, 16972, 5, 1, 3

/* MIB object cpePingAdminStatus = cpePing, 4 */
#define	I_cpePingAdminStatus	4
#define	O_cpePingAdminStatus	1, 3, 6, 1, 4, 1, 16972, 5, 1, 4

/* MIB object cpePingOperStatus = cpePing, 5 */
#define	I_cpePingOperStatus	5
#define	O_cpePingOperStatus	1, 3, 6, 1, 4, 1, 16972, 5, 1, 5

/* MIB object cpePingResult = cpePing, 6 */
#define	I_cpePingResult	6
#define	O_cpePingResult	1, 3, 6, 1, 4, 1, 16972, 5, 1, 6

/* MIB object cpeTrap, bootTrap */
#define	I_cpeBootTrap	1
#define	O_cpeBootTrap	1, 3, 6, 1, 4, 1, 16972, 6, 0, 1

/* MIB object cpeTrap, cpeFtpDone Trap */
#define	I_cpeFtpDone	2
#define	O_cpeFtpDone	1, 3, 6, 1, 4, 1, 16972, 6, 0, 2

/* MIB object cpeTrap, cpePingDone Trap */
#define	I_cpePingDone	3
#define	O_cpePingDone	1, 3, 6, 1, 4, 1, 16972, 6, 0, 3

/* MIB object cpeTrap, cpePppTestDone Trap */
#define	I_cpePppTestDone	4
#define	O_cpePppTestDone	1, 3, 6, 1, 4, 1, 16972, 6, 0, 4

/* Put here additional MIB specific include definitions */

#define CPE_CONFIG_ID_MAX_LEN   4
#define CPE_LEN_3     3
#define CPE_LEN_8     8
#define CPE_LEN_16    16
#define CPE_LEN_32    32
#define CPE_LEN_64    64

#define CPE_BRIDGE_MODE    1
#define CPE_ROUTER_MODE    2

#ifndef CT_SNMP_MIB_VERSION_2
#define CPE_MIB_VERSION    "V1.00"
#else
#define CPE_MIB_VERSION    "V2.00"
#endif
#define CPE_ADMIN_ENABLE   1
#define CPE_ADMIN_DISABLE  2
#define CPE_ADMIN_UP       1
#define CPE_ADMIN_DOWN     2
#define CPE_BOOTTRAP_NO_RESPONSE 1
#define CPE_BOOTTRAP_CFG_READY   2
#define CPE_BOOTTRAP_RECFG       3
#define CPE_ADSL_GDMT_FIRST      1
#define CPE_ADSL_T1413_FIRST     2
#define CPE_ADSL_GDMT_OR_LITE    3
#define CPE_ADSL_T1413           4
#define CPE_ADSL_GDMT            5
#define CPE_ADSL_GLITE           6
#define CPE_ADSL_AUTOSENSE       7
#define CPE_ADSL_AUTOSENSE_ADSL2PLUS 8

#define CPE_ROW_STATUS_ACTIVE            1
#define CPE_ROW_STATUS_NOT_IN_SERVICE    2
#define CPE_ROW_STATUS_NOT_READY         3
#define CPE_ROW_STATUS_CREATE_AND_GO     4
#define CPE_ROW_STATUS_CREATE_AND_WAIT   5
#define CPE_ROW_STATUS_DESTROY           6

#define CPE_DHCP_SERVER_NO_SUPPORT       1
#define CPE_DHCP_SERVER_ENABLE           2
#define CPE_DHCP_CLIENT_ENABLE           3
#define CPE_DHCP_RELAY_ENABLE            4

#define CPE_DNS_PRIMARY                  1
#define CPE_DNS_SECONDARY                2

#define CPE_PING_ADDR_DEFAULT            "http://www.chinatelecom.com.cn"
#define CPE_PING_PKT_SIZE_DEFAULT        32
#define CPE_PING_PKT_SIZE_MAX            4096
#define CPE_PING_COUNT_DEFAULT           4
#define CPE_PING_COUNT_MIN               1
#define CPE_PING_COUNT_MAX               32767
#define CPE_PING_ADDR_LEN_MIN            4
#define CPE_PING_ADDR_LEN_MAX            256
#define CPE_PING_RESULT_LEN_MAX          400
#define CPE_PING_ADMIN_STOP              1
#define CPE_PING_ADMIN_START             2
#define CPE_PING_OPER_NORMAL             1
#define CPE_PING_OPER_PINGING            2
#define CPE_PING_OPER_SUCCESS            3
#define CPE_PING_OPER_FAILURE            4

#define CPE_FTP_NAME_MAX_LEN                 128
#define CPE_FTP_PASSWORD_MAX_LEN             128
#define CPE_FTP_FILE_NAME_MAX_LEN            255
#define CPE_FTP_FILE_NAME_MIN_LEN            4
#define CPE_FTP_DEFAULT_FILE                 "ftpFile"
#define CPE_FTP_ADMIN_STATUS_STOP            1
#define CPE_FTP_ADMIN_STATUS_UPGRADE         2
#define CPE_FTP_ADMIN_STATUS_FTPTEST         3
#define CPE_FTP_OPER_STATUS_NORMAL           1
#define CPE_FTP_OPER_STATUS_CONNECT_SUCCESS  2
#define CPE_FTP_OPER_STATUS_CONECT_FAILURE   3
#define CPE_FTP_OPER_STATUS_DOWNLOADING      4
#define CPE_FTP_OPER_STATUS_DOWNLOAD_SUCCESS 5
#define CPE_FTP_OPER_STATUS_DOWNLOAD_FAILURE 6
#define CPE_FTP_OPER_STATUS_SAVING           7
#define CPE_FTP_OPER_STATUS_SAVE_FAILURE     8
#define CPE_FTP_OPER_STATUS_UPGRADE_SUCCESS  9
#define CPE_FTP_OPER_STATUS_UPGRADE_FAIL     10

#define CPE_SYSTEM_RESTORE_TO_NULL    1
#define CPE_SYSTEM_RESTORE_TO_DEFAULT   2

#define CPE_POLL_INTERVAL                1  /* 1 second */
#define CPE_MAX_TRAP_TIME_INTERVAL       10 /* random sending boottrap time is 1-10 sec */
#define CPE_TRAP_RETRY_INTERVAL          30 /* await for response and retry every 30 sec */
#define CPE_TRAP_RETRY_COUNT             3  /* retry time */
#define CPE_DELAY_OPERATION              5  

#define CPE_PPP_TEST_POLL_INTERVAL       10  /* in second */
#define CPE_PPP_TEST_POLL_TIMEOUT_TIME   180 /* 3 minutes,3*60 */
//#define CPE_PPP_TEST_POLL_TIMEOUT_TIME   60 /* for testing only, 1 minutes,3*60 */
#define CPE_PPP_TEST_TIMEOUT_PERIOD      (CPE_PPP_TEST_POLL_TIMEOUT_TIME/CPE_PPP_TEST_POLL_INTERVAL)

typedef struct cpeSysInfo {
  unsigned char configId[CPE_CONFIG_ID_MAX_LEN];
  char version[CPE_LEN_64];
  char prodId[CPE_LEN_16];
  char vendorId[CPE_LEN_32];
  char sysVersion[CPE_LEN_64];
  char mibVersion[CPE_LEN_8];
  char serialNum[CPE_LEN_32];
  unsigned char oui[CPE_LEN_3];
  #ifdef CT_SNMP_MIB_VERSION_2
  int sysInfoType;
  #endif
  int trap; 
  int trapResponseState;
} CPE_SYS_INFO, *pCPE_SYS_INFO;

typedef struct pvcInfo 
{
  int id;       
  int ifIndex;
  int upperEncap; 
  int adminStatus; 
  int operStatus;
  int bridgeMode;
  int igmpMode;
  int dhcpcMode;
  int natMode;
  struct pvcInfo *next;
} PVC_INFO, *pPVC_INFO;

typedef struct pvcInfoList 
{
  int pvcCount;
  pPVC_INFO pPvc;
  pPVC_INFO pPvcTail;
} PVC_INFO_LIST, *pPVC_INFO_LIST;

typedef struct pppInfo 
{
  int indexId;
  int link;
  int pppIfIndex;
  int pvcIfIndex;
  char serviceName[CPE_LEN_32]; 
  int timeout;
  int maxSegmentSize;
  int rowStatus;
  int adminStatus;
  int operStatus;
  int testResult;
  int vpi;              /* internal use only */
  int vci;              /* internal use only */
  int conId;            /* internal use only */
  struct pppInfo *next;
  char secretId[255];
  char secret[40];
} CPE_PPP_INFO, *pCPE_PPP_INFO;

typedef struct pppInfoList 
{
  int count;
  pCPE_PPP_INFO head;
  pCPE_PPP_INFO tail;
} PPP_INFO_LIST, *pPPP_INFO_LIST;

/* save this for username/password; this is a solution for not being
   able to get ppp_%vpi%vci when PPP is not up; router 's limitation */
typedef struct pppReference
{
  int pppIfIndex;
  int vpi;              /* internal use only */
  int vci;              /* internal use only */
  int conId;            /* internal use only */
  struct pppReference *next;
} CPE_PPP_REFERENCE, *pCPE_PPP_REFERENCE;

typedef struct pppReferenceList 
{
  int count;
  pCPE_PPP_REFERENCE head;
  pCPE_PPP_REFERENCE tail;
} PPP_REFERENCE_LIST, *pPPP_REFERENCE_LIST;

#define CPE_ROUTE_LENGTH    256
typedef struct ipTableInfo 
{
  unsigned int index;
  unsigned int lowerIfIndex;
  struct in_addr addr;   
  struct in_addr mask;   
  struct in_addr gw;   
  int status;
  int gwOnOff;
  struct ipTableInfo *next;
} CPE_IP_TABLE_INFO, *pCPE_IP_TABLE_INFO;

#ifdef CT_SNMP_MIB_VERSION_2

#define CPE_ETHERNETPORT_BINDING_MODE 2
#define CPE_ETHERNETPORT_NO_BINDING_MODE 1

typedef struct ethernetPortTableInfo 
{
  int ethernetPortIfIndex;
  int ethernetPortSerialNumber;
  int ethernetPortBindingMode;
  int ethernetPortIgmpOnOff;
  int ethernetPortI8021dParameter;
  struct ethernetPortTableInfo *next;
} CPE_ETHERNETPORT_TABLE_INFO, *pCPE_ETHERNETPORT_TABLE_INFO;

typedef struct ethernetPortTableList 
{
  int count;
  pCPE_ETHERNETPORT_TABLE_INFO head;
  pCPE_ETHERNETPORT_TABLE_INFO tail;
} CPE_ETHERNETPORT_TABLE_LIST, *pCPE_ETHERNETPORT_TABLE_LIST;

typedef struct serviceTypeTableInfo 
{
  int serviceTypeIndex;
  int pvcServiceType;
  int pvcPcr;
  int pvcMcr;
  int pvcScr;
  int serviceTypeRowStatus;
  struct serviceTypeTableInfo *next;
} CPE_SERVICETYPE_TABLE_INFO, *pCPE_SERVICETYPE_TABLE_INFO;

typedef struct serviceTypeTableList 
{
  int count;
  pCPE_SERVICETYPE_TABLE_INFO head;
  pCPE_SERVICETYPE_TABLE_INFO tail;
} CPE_SERVICETYPE_TABLE_LIST, *pCPE_SERVICETYPE_TABLE_LIST;


#endif

typedef struct ipTableList 
{
  int count;
  pCPE_IP_TABLE_INFO head;
  pCPE_IP_TABLE_INFO tail;
} CPE_IP_TABLE_LIST, *pCPE_IP_TABLE_LIST;

typedef struct cpePingInfo {
  char addr[CPE_PING_ADDR_LEN_MAX];
  int addrLen;
  int size;
  int count;
  int adminStatus;
  int operStatus;
  char result[CPE_PING_RESULT_LEN_MAX];
  int pollRetry;
} CPE_PING_INFO, *pCPE_PING_INFO;

typedef struct cpeFtpInfo {
  // input from server
  struct in_addr addr; 
  char user[128];
  char password[128];
  char filename[256];
  int adminStatus;
  // output from ftp operation
  int totalSize;  /* in Kbyte */
  int operStatus;
  int doneSize;   /* in Kbyte */
  int elapseTime; /* in second */
} CPE_FTP_INFO, *pCPE_FTP_INFO;

typedef struct cpePppTestInfo {
  int vpi;
  int vci;
  int conId;
  int pollCount;
  int stop;
  int testResult;
} CPE_PPPTEST_INFO, *pCPE_PPPTEST_INFO;

void cpeDslMibFree(void);
void init_cpe_dsl_mib(void);
void register_subtrees_of_cpe_dsl_mib(void);
void cpeInitPingData(void);
void cpeInitFtpData(void);
int cpeGenerateRandomNum(unsigned int max);
void cpeSendBootTrap(int junk);
void cpeSendPingDoneTrap(int junk);
void cpeSendFtpDoneTrap(int junk);
void cpeSendPppTestDoneTrap(int junk);
void cpeAddPppRefernceToList(pCPE_PPP_REFERENCE p);
void cpeFreeReferenceList(void);

#endif	/* _CPE_DSL_MIB_ */
