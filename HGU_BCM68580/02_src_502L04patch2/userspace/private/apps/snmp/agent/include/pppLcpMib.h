#ifndef	_PPP_LCP_MIB_
#define	_PPP_LCP_MIB_

/* required include files (IMPORTS) */
#include	"rfc1212Mib.h"
#include	"rfc1155Smi.h"


void init_PPP_LCP_MIB(void);
void register_subtrees_of_PPP_LCP_MIB(void);

/* defined objects in this module */
/* MIB object transmission = mib_2, 10 */
#define	I_transmission	10
#define	O_transmission	1, 3, 6, 1, 2, 1, 10

/* MIB object ppp = transmission, 23 */
#define	I_ppp	23
#define	O_ppp	1, 3, 6, 1, 2, 1, 10, 23

/* MIB object pppLcp = ppp, 1 */
#define	I_pppLcp	1
#define	O_pppLcp	1, 3, 6, 1, 2, 1, 10, 23, 1

/* MIB object pppLink = pppLcp, 1 */
#define	I_pppLink	1
#define	O_pppLink	1, 3, 6, 1, 2, 1, 10, 23, 1, 1

/* MIB object pppLqr = pppLcp, 2 */
#define	I_pppLqr	2
#define	O_pppLqr	1, 3, 6, 1, 2, 1, 10, 23, 1, 2

/* MIB object pppTests = pppLcp, 3 */
#define	I_pppTests	3
#define	O_pppTests	1, 3, 6, 1, 2, 1, 10, 23, 1, 3

/* MIB object pppLinkStatusTable = pppLink, 1 */
#define	I_pppLinkStatusTable	1
#define	O_pppLinkStatusTable	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 1

/* MIB object pppLinkStatusEntry = pppLinkStatusTable, 1 */
#define	I_pppLinkStatusEntry	1
#define	O_pppLinkStatusEntry	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 1, 1

/* MIB object pppLinkStatusPhysicalIndex = pppLinkStatusEntry, 1 */
#define	I_pppLinkStatusPhysicalIndex	1
#define	O_pppLinkStatusPhysicalIndex	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 1, 1, 1

/* MIB object pppLinkStatusBadAddresses = pppLinkStatusEntry, 2 */
#define	I_pppLinkStatusBadAddresses	2
#define	O_pppLinkStatusBadAddresses	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 1, 1, 2

/* MIB object pppLinkStatusBadControls = pppLinkStatusEntry, 3 */
#define	I_pppLinkStatusBadControls	3
#define	O_pppLinkStatusBadControls	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 1, 1, 3

/* MIB object pppLinkStatusPacketTooLongs = pppLinkStatusEntry, 4 */
#define	I_pppLinkStatusPacketTooLongs	4
#define	O_pppLinkStatusPacketTooLongs	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 1, 1, 4

/* MIB object pppLinkStatusBadFCSs = pppLinkStatusEntry, 5 */
#define	I_pppLinkStatusBadFCSs	5
#define	O_pppLinkStatusBadFCSs	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 1, 1, 5

/* MIB object pppLinkStatusLocalMRU = pppLinkStatusEntry, 6 */
#define	I_pppLinkStatusLocalMRU	6
#define	O_pppLinkStatusLocalMRU	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 1, 1, 6

/* MIB object pppLinkStatusRemoteMRU = pppLinkStatusEntry, 7 */
#define	I_pppLinkStatusRemoteMRU	7
#define	O_pppLinkStatusRemoteMRU	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 1, 1, 7

/* MIB object pppLinkStatusLocalToPeerACCMap = pppLinkStatusEntry, 8 */
#define	I_pppLinkStatusLocalToPeerACCMap	8
#define	O_pppLinkStatusLocalToPeerACCMap	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 1, 1, 8

/* MIB object pppLinkStatusPeerToLocalACCMap = pppLinkStatusEntry, 9 */
#define	I_pppLinkStatusPeerToLocalACCMap	9
#define	O_pppLinkStatusPeerToLocalACCMap	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 1, 1, 9

/* MIB object pppLinkStatusLocalToRemoteProtocolCompression = pppLinkStatusEntry, 10 */
#define	I_pppLinkStatusLocalToRemoteProtocolCompression	10
#define	O_pppLinkStatusLocalToRemoteProtocolCompression	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 1, 1, 10

/* MIB object pppLinkStatusRemoteToLocalProtocolCompression = pppLinkStatusEntry, 11 */
#define	I_pppLinkStatusRemoteToLocalProtocolCompression	11
#define	O_pppLinkStatusRemoteToLocalProtocolCompression	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 1, 1, 11

/* MIB object pppLinkStatusLocalToRemoteACCompression = pppLinkStatusEntry, 12 */
#define	I_pppLinkStatusLocalToRemoteACCompression	12
#define	O_pppLinkStatusLocalToRemoteACCompression	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 1, 1, 12

/* MIB object pppLinkStatusRemoteToLocalACCompression = pppLinkStatusEntry, 13 */
#define	I_pppLinkStatusRemoteToLocalACCompression	13
#define	O_pppLinkStatusRemoteToLocalACCompression	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 1, 1, 13

/* MIB object pppLinkStatusTransmitFcsSize = pppLinkStatusEntry, 14 */
#define	I_pppLinkStatusTransmitFcsSize	14
#define	O_pppLinkStatusTransmitFcsSize	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 1, 1, 14

/* MIB object pppLinkStatusReceiveFcsSize = pppLinkStatusEntry, 15 */
#define	I_pppLinkStatusReceiveFcsSize	15
#define	O_pppLinkStatusReceiveFcsSize	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 1, 1, 15

/* MIB object pppLinkConfigTable = pppLink, 2 */
#define	I_pppLinkConfigTable	2
#define	O_pppLinkConfigTable	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 2

/* MIB object pppLinkConfigEntry = pppLinkConfigTable, 1 */
#define	I_pppLinkConfigEntry	1
#define	O_pppLinkConfigEntry	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 2, 1

/* MIB object pppLinkConfigInitialMRU = pppLinkConfigEntry, 1 */
#define	I_pppLinkConfigInitialMRU	1
#define	O_pppLinkConfigInitialMRU	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 2, 1, 1

/* MIB object pppLinkConfigReceiveACCMap = pppLinkConfigEntry, 2 */
#define	I_pppLinkConfigReceiveACCMap	2
#define	O_pppLinkConfigReceiveACCMap	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 2, 1, 2

/* MIB object pppLinkConfigTransmitACCMap = pppLinkConfigEntry, 3 */
#define	I_pppLinkConfigTransmitACCMap	3
#define	O_pppLinkConfigTransmitACCMap	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 2, 1, 3

/* MIB object pppLinkConfigMagicNumber = pppLinkConfigEntry, 4 */
#define	I_pppLinkConfigMagicNumber	4
#define	O_pppLinkConfigMagicNumber	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 2, 1, 4

/* MIB object pppLinkConfigFcsSize = pppLinkConfigEntry, 5 */
#define	I_pppLinkConfigFcsSize	5
#define	O_pppLinkConfigFcsSize	1, 3, 6, 1, 2, 1, 10, 23, 1, 1, 2, 1, 5

/* MIB object pppLqrTable = pppLqr, 1 */
#define	I_pppLqrTable	1
#define	O_pppLqrTable	1, 3, 6, 1, 2, 1, 10, 23, 1, 2, 1

/* MIB object pppLqrEntry = pppLqrTable, 1 */
#define	I_pppLqrEntry	1
#define	O_pppLqrEntry	1, 3, 6, 1, 2, 1, 10, 23, 1, 2, 1, 1

/* MIB object pppLqrQuality = pppLqrEntry, 1 */
#define	I_pppLqrQuality	1
#define	O_pppLqrQuality	1, 3, 6, 1, 2, 1, 10, 23, 1, 2, 1, 1, 1

/* MIB object pppLqrInGoodOctets = pppLqrEntry, 2 */
#define	I_pppLqrInGoodOctets	2
#define	O_pppLqrInGoodOctets	1, 3, 6, 1, 2, 1, 10, 23, 1, 2, 1, 1, 2

/* MIB object pppLqrLocalPeriod = pppLqrEntry, 3 */
#define	I_pppLqrLocalPeriod	3
#define	O_pppLqrLocalPeriod	1, 3, 6, 1, 2, 1, 10, 23, 1, 2, 1, 1, 3

/* MIB object pppLqrRemotePeriod = pppLqrEntry, 4 */
#define	I_pppLqrRemotePeriod	4
#define	O_pppLqrRemotePeriod	1, 3, 6, 1, 2, 1, 10, 23, 1, 2, 1, 1, 4

/* MIB object pppLqrOutLQRs = pppLqrEntry, 5 */
#define	I_pppLqrOutLQRs	5
#define	O_pppLqrOutLQRs	1, 3, 6, 1, 2, 1, 10, 23, 1, 2, 1, 1, 5

/* MIB object pppLqrInLQRs = pppLqrEntry, 6 */
#define	I_pppLqrInLQRs	6
#define	O_pppLqrInLQRs	1, 3, 6, 1, 2, 1, 10, 23, 1, 2, 1, 1, 6

/* MIB object pppLqrConfigTable = pppLqr, 2 */
#define	I_pppLqrConfigTable	2
#define	O_pppLqrConfigTable	1, 3, 6, 1, 2, 1, 10, 23, 1, 2, 2

/* MIB object pppLqrConfigEntry = pppLqrConfigTable, 1 */
#define	I_pppLqrConfigEntry	1
#define	O_pppLqrConfigEntry	1, 3, 6, 1, 2, 1, 10, 23, 1, 2, 2, 1

/* MIB object pppLqrConfigPeriod = pppLqrConfigEntry, 1 */
#define	I_pppLqrConfigPeriod	1
#define	O_pppLqrConfigPeriod	1, 3, 6, 1, 2, 1, 10, 23, 1, 2, 2, 1, 1

/* MIB object pppLqrConfigStatus = pppLqrConfigEntry, 2 */
#define	I_pppLqrConfigStatus	2
#define	O_pppLqrConfigStatus	1, 3, 6, 1, 2, 1, 10, 23, 1, 2, 2, 1, 2

/* MIB object pppLqrExtnsTable = pppLqr, 3 */
#define	I_pppLqrExtnsTable	3
#define	O_pppLqrExtnsTable	1, 3, 6, 1, 2, 1, 10, 23, 1, 2, 3

/* MIB object pppLqrExtnsEntry = pppLqrExtnsTable, 1 */
#define	I_pppLqrExtnsEntry	1
#define	O_pppLqrExtnsEntry	1, 3, 6, 1, 2, 1, 10, 23, 1, 2, 3, 1

/* MIB object pppLqrExtnsLastReceivedLqrPacket = pppLqrExtnsEntry, 1 */
#define	I_pppLqrExtnsLastReceivedLqrPacket	1
#define	O_pppLqrExtnsLastReceivedLqrPacket	1, 3, 6, 1, 2, 1, 10, 23, 1, 2, 3, 1, 1

/* MIB object pppEchoTest = pppTests, 1 */
#define	I_pppEchoTest	1
#define	O_pppEchoTest	1, 3, 6, 1, 2, 1, 10, 23, 1, 3, 1

/* MIB object pppDiscardTest = pppTests, 2 */
#define	I_pppDiscardTest	2
#define	O_pppDiscardTest	1, 3, 6, 1, 2, 1, 10, 23, 1, 3, 2

/* Put here additional MIB specific include definitions */

#endif	/* _PPP_LCP_MIB_ */
