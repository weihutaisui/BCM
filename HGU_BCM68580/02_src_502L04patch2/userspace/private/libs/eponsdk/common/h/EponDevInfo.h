/*
* <:copyright-BRCM:2012:proprietary:epon
* 
*    Copyright (c) 2012 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
* :> 
*/
#if !defined(EponDevInfo_h)
#define EponDevInfo_h
////////////////////////////////////////////////////////////////////////////////
/// \file Personality.h
/// \brief Contains configuration access for "personality" info
///
/// The "personality" information configures various options of the firmware
/// that allow the same firmware load to behave in a different manner in
/// different ONTs.  Most personality information is stored in flash.  Some
/// is provided by other input means such as DIP switched attached to GPIO
/// pins.  See the TK3701 Host Interface Spec for details of the personality
/// configuration required from the surrounding system.
///
////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
extern "C" {
#endif

#include "Teknovus.h"
#include "Ethernet.h"
#include "Polarity.h"
#include "Oam.h"
#include "CtcOam.h"    

#define MAX_NUM_UNI_PORTS       8
#define EponDevCtcPonIfNum()       1
#define MAX_NUM_DPOE_PORTS       8

#define DPOE_PORT_NUM_LEN          2
#define DPOE_PORT_TYPE_LEN         2

#define EponDevExtendedIdLen       80

#define EponDevCtcAuthIdLen        24
#define EponDevCtcAuthPassLen      12

// Vendor ID division in Epon Device Info
#define CtcOamE1PortCountIndex      30
#define CtcOamPotsPortCountIndex    31
#define CtcOamHardwareVerIndex      32
#define CtcOamHardwareVerLen        8
#define CtcOamSoftwareVerIndex      40
#define CtcOamVendorIdIndex         56
#define CtcOamOnuModelIndex         60
#define CtcOamOnuExtOnuModel        64

#define DateInfoLen                 4   // Date of Manufacture YYYYMMDD
#define DpoeVendorInfoLen           64  
#define DevModelNameLen             32
#define DevVendorNameLen            32
#define DpoeMfgTimeLen              26
#define DevHwVerLen                 32
#define DpoeFileNameLen             128



// Mirror OamPortCapability
typedef enum
    {
    EthPortHalfDuplex    = 0x0001UL,
    EthPortFullDuplex    = 0x0002UL,
    EthPort10Speed       = 0x0004UL,
    EthPort100Speed      = 0x0008UL,
    EthPort1GSpeed       = 0x0010UL,
    EthPort10GSpeed      = 0x0020UL,
    EthPortFlowControl   = 0x0040UL,
    EthPortAutoMDIMDIX   = 0x0080UL,
    EthPortAutoNegotiate = 0x0100UL,
    EthPortNoPhyPresent  = 0x0200UL,
    EthPortUndefined     = 0x0400UL,
    EthPortMaxCap        = 0x7FFFUL
    } EthPortCapability;

#define EthPortCapDuplexMask    EthPortHalfDuplex | EthPortFullDuplex
#define EthPortCapSpeedMask     EthPort10Speed | EthPort100Speed | \
                                EthPort1GSpeed | EthPort10GSpeed


typedef enum
{
	TkOnuEthPortDisabled		= 0x00,

	TkOnuEthPortForceHalf		= 0x00,
	TkOnuEthPortForceFull		= 0x01,

	TkOnuEthPortForce10Mbps   	= 0x00,
	TkOnuEthPortForce100Mbps  	= 0x02,
    TkOnuEthPortForce1Gbps      = 0x04,
	TkOnuEthPortSpeedMask		= 0x06,
	TkOnuEthPortSpeedShft		= 1,

    TkOnuEthPortSpeedDuplex    = TkOnuEthPortForce1Gbps |
                                 TkOnuEthPortForce100Mbps |
                                 TkOnuEthPortForceFull, 

    // if auto negotiation enabled, speed settings are ignored

    TkOnuEthPortFlowCtrlEn     = 0x08,

    TkOnuEthPortNegotiable     = TkOnuEthPortSpeedDuplex |
                                 TkOnuEthPortFlowCtrlEn,

    TkOnuEthPortMdi            = 0x10,
    TkOnuEthPortGpioCfg        = 0x20,

    TkOnuEthPortAutoEnable     = 0x40,

    TkOnuEthPortCfgEnable      = 0x80
}PACK TkOnuEthPortCfg;

typedef struct
{
    TkOnuEthPortCfg     portConfig;
}PACK EponPortCfg;

typedef struct 
{
    U16 vendorId;
    U16 chipModel;
    U32 revision;
}PACK EponChipId;

typedef enum
{
    DpoePortTypeUnspecified              = 0x00,
    DpoePortTypeMTA                      = 0x01,
    DpoePortTypeSTBIP                    = 0x02,
    DpoePortTypeSTBDSG                   = 0x03,
    DpoePortTypeTEA                      = 0x04,
    DpoePortTypeSG                       = 0x05,
    DpoePortTypeRouter                   = 0x06,
    DpoePortTypeDVA                      = 0x07,  
    DpoePortTypeSEBSTBIP                 = 0x08,
    DpoePortTypeReserved                 = 0x09
}DpoePortType;


typedef struct
{
    DpoePortType portType;
} DpoePort;

typedef struct
{
    U8                  extendedId[EponDevExtendedIdLen];
    U16                 firmwareVer;
    EponChipId          chipId;
    OamId               id802;

    MacAddr             resbaseEponMac;
    U8                  eponMacNum;
    
        
    U8                  fePortNum;
    U8                  gePortNum;
    U8                  uniPortNum;
    U8                  voipPortNum;
    U8                  uniToPhysicalPortMap[MAX_NUM_UNI_PORTS];
    U16                 portcapability[MAX_NUM_UNI_PORTS];
    EponPortCfg         portCfg[MAX_NUM_UNI_PORTS];

    U8                  dpoePortNum;
    DpoePort            dpoePort[MAX_NUM_DPOE_PORTS];

    U8                  numLinks;
    U16                 maxFrameSize;
    U32                 interop;
    
    OamCtcOnuType       onuType;
    U8                  ctcAuthId[EponDevCtcAuthIdLen+1];
    U8                  ctcAuthPass[EponDevCtcAuthPassLen+1];
    U8                  ctcProtectionType;
    U8                  powerFailOpts;
    U8                  fecFrameAbort;
    U8                  txFecEnable;
    U8                  rxFecEnable;
    U8                  holdoverTime;
    U8                  holdoverFlags;
    U8                  ctcLlidNumber;   
    U8 		   dnPonRate;
    U8 		   upPonRate;

    /* Get from obj XponObject */
    U8                  oamsel; 
    U8                  date[DateInfoLen];
    U8                  dpoeMfgTime[DpoeMfgTimeLen];
    U8                  dpoeFileName[DpoeFileNameLen+1];
    U32                 imageCrc;
    U32                 imageCrcBeforeCommit;
    U8                  schMode;
    U8                  idleTimeOffset;

    Bool                txLaserPowerOff;
    Bool                  failSafe;

    /* Get from obj IGDDeviceInfoObject */
    U8                  dpoeVendor[DpoeVendorInfoLen+1]; 
    U8                  modelName[DevModelNameLen];
    U8                  vendorName[DevVendorNameLen];
    U8                  devHwVer[DevHwVerLen];
}PACK EponDevInfo;


extern EponDevInfo eponDevInfo;


#define EponDevBaseMacAddr              (eponDevInfo.resbaseEponMac) 
#define EponDevExtendedId               (eponDevInfo.extendedId)
#define EponDevGePortNum                (eponDevInfo.gePortNum)
#define EponDevFePortNum                (eponDevInfo.fePortNum)
#define EponDevPortConfig(port)         (eponDevInfo.portCfg[port].portConfig)
#define EponDevPortCapability(port)     (eponDevInfo.portcapability[port])
#define EponDevUniPortNum               (eponDevInfo.uniPortNum)
#define EponDevDpoePortNum              (eponDevInfo.dpoePortNum)
#define EponDevDpoePortType(port)       (eponDevInfo.dpoePort[port].portType)

#define EponDevFirmwareVer              (eponDevInfo.firmwareVer)
#define EponDevChipId                   (eponDevInfo.chipId)
#define EponDevOnuType                  (eponDevInfo.onuType)
#define EponDevNumLinks                 (eponDevInfo.numLinks)
#define EponDevHwVer                    (eponDevInfo.devHwVer)
#define EponDevVendorInfo               (eponDevInfo.dpoeVendor)
#define EponDevModelName                (eponDevInfo.modelName)
#define EponDevVendorName               (eponDevInfo.vendorName)
#define EponDevDate                     (eponDevInfo.date)
#define EponDevDefMTU                   EponMaxFrameSize

////////////////////////////////////////////////////////////////////////////////
/// PersInit:       Initialize ramPers for OAM stack 
///
/// Parameters:
/// \param pers     pointer to the buffer where the personality locates 
///
/// \return 
/// TRUE on sucess
/// FALSE on failure
//
////////////////////////////////////////////////////////////////////////////////
extern
BOOL EponDevInit(void);


////////////////////////////////////////////////////////////////////////////////
/// PersExtendedId: get extended ID information
///
/// The extended ID is a 64-byte section of user-defined memory intended
/// to hold information such as a serial number, product code, date of
/// manufacture, etc.
///
 // Parameters:
/// \param paramName Description
///
/// \return
/// Description of return value
////////////////////////////////////////////////////////////////////////////////
extern
U8 const * EponDevInfoExtendedId(void);

extern 
BOOL EponDevSetMaxLinks(U16 maxLinks);

extern     
BOOL EponDevSetMfgTime(U8 *data, U16 len);

extern
U16 EponDevGetMfgTime(U8 *data, U16 maxLen);
    
extern
void EponDevSetFileName(U8 *data, U16 len);    

extern
U16 EponDevGetFileName(U8 *data, U16 maxLen);

extern
U8 EponDevGetOamSel(void);
extern
void EponDevSetImageCrc(U32 imageCrc);

extern    
U8 EponDevGetEponMacNum(void);

extern    
U32 EponDevGetImageCrc(void);

extern
void EponDevSetImageCrcBeforeCommit(U32 imageCrc);

extern
U32 EponDevGetImageCrcBeforeCommit(void);

extern
void EponDevSetSchMode(U8 mode);

extern
U8 EponDevGetSchMode(void);

extern
void EponDevSetIdleTimeOffset(U8 offset);

extern
U8 EponDevGetIdleTimeOffset(void);


extern
void EponDevSetTxLaserPowerOff(Bool powerOff);

extern
Bool EponDevGetTxLaserPowerOff(void);

extern 
void EponDevSetFailSafe(Bool enable);

extern 
Bool EponDevGetFailSafe(void);

extern 
U8 EponDevGetDnPonRate(void);

extern 
U8 EponDevGetUpPonRate(void);


#if defined(__cplusplus)
}
#endif

#endif // EponDevInfo.h
