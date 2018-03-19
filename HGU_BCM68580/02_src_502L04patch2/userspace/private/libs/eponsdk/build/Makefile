#
# <:copyright-BRCM:2002:proprietary:epon
# 
#    Copyright (c) 2002 Broadcom 
#    All Rights Reserved
# 
#  This program is the proprietary software of Broadcom and/or its
#  licensors, and may only be used, duplicated, modified or distributed pursuant
#  to the terms and conditions of a separate, written license agreement executed
#  between you and Broadcom (an "Authorized License").  Except as set forth in
#  an Authorized License, Broadcom grants no license (express or implied), right
#  to use, or waiver of any kind with respect to the Software, and Broadcom
#  expressly reserves all rights in and to the Software and all intellectual
#  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
#  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
#  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
# 
#  Except as expressly set forth in the Authorized License,
# 
#  1. This program, including its structure, sequence and organization,
#     constitutes the valuable trade secrets of Broadcom, and you shall use
#     all reasonable efforts to protect the confidentiality thereof, and to
#     use this information only in connection with your use of Broadcom
#     integrated circuit products.
# 
#  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
#     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
#     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
#     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
#     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
#     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
#     PERFORMANCE OF THE SOFTWARE.
# 
#  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
#     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
#     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
#     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
#     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
#     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
#     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
#     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
#     LIMITED REMEDY.
# :>
#
#

# Will have problem with DESKTOP_LINUX, just copy the required parameters here.
# include Makefile.conf
PLATFORM_SRC_DIR = linux

#OBJ_DIR = $(SDK_BASE)/obj
OBJ_DIR = $(SDK_BASE)

-include depend
include $(BUILD_DIR)/make.common
ifneq ($(strip $(BRCM_VOICE_SUPPORT)),)
include $(BUILD_DIR)/make.voice
endif

CTC_OBJS = CtcAuthentication.o CtcEncryption.o CtcEthControl.o CtcMcast.o CtcMLlid.o\
		  CtcOamDiscovery.o CtcOptDiag.o CtcStats.o CtcAlarms.o CtcOamFileTransfer.o CtcOamGet.o CtcOamSet.o \
		  CtcOnuOam.o CtcOnuUtils.o CtcReport.o CtcVlan.o OamCtcPowerSave.o \
		  CtcClassification.o CtcOamIf.o

DPOE_OBJS = OamDpoeOnu.o OamDpoeGet.o OamDpoeSet.o DpoeOamIf.o OamDpoeMgmt.o OamDpoeOptProtect.o OamDpoeMgmtCommon.o OamDpoeBridge.o \
			OamDpoeBridgeCommon.o OamDpoeStats.o OamDpoeStatsCommon.o OamDpoeFileCommon.o OamDpoeFile.o OamDpoeEventCommon.o OamDpoeEvent.o \
			OamDpoeFrmProcCommon.o OamDpoeFrmProc.o DpoeOptDiag.o OamDpoeUni.o \
			OamDpoeSlaCommon.o OamDpoeSla.o EapTls.o Sha1.o Rsa.o OamDpoeDemarc.o OamDpoeSecCommon.o OamDpoeSec.o OamDpoeMcast.o \
			OamDpoeTekGet.o OamDpoeTekSet.o OamDpoeTeknovus.o

BCM_OBJS = OamFileTransfer.o OamGeneralActions.o OamPortOpsCom.o OamTeknovus.o OamPolicer.o OamShaper.o \
			OamTekSecCommon.o OamTekSec.o OamSys.o OamStats.o OamPortLrn.o OamTekEventCommon.o OamTekEvent.o \
			OamTekNegCom.o OamTekNeg.o TekOamIf.o SampleOam.o SampleOamIf.o OamTekSet.o OamTekUserRules.o	OamOptCtrl.o OamQueue.o

OBJS_LIB = TkOsAlSem.o TkOsAlThread.o TkOsAlMsg.o TkOsAlDataInOut.o TkOsAlCommon.o TkOamMem.o \
          TkOamApiCommon.o TkMsgProcess.o TkSdkInitApi.o  TkSdkDebug.o TkOs.o \
		  OnuOs.o OsAstEvt.o OsAstMsgQ.o OntConfigDb.o EponDevInfo.o Oui.o Pbi.o SysInfo.o Stream.o PowerSave.o Holdover.o\
		  Alarms.o PonManager.o PonMgrEncryptCommon.o PonMgrEncrypt.o UniConfigDb.o OamIeeeAutoNeg.o\
          EthFrameUtil.o OamPortUtil.o OamUtil.o OamEpon.o OamGet.o OamSet.o OamIeeeCommon.o OamIeeeAutoNegCommon.o \
          OamIeeeFecCommon.o OamIeeeMacCommon.o OamOnu.o MacLearning.o  \
		  OamIeeeEvent.o OamIeeePhyCommon.o OamIeee.o OamIeeePhy.o OntmTimer.o OamProcessInit.o \
		  Md5.o DataPathRules.o Stats.o OamCmdsPro.o Mcast.o
ifneq ($(findstring CLOCK_TRANSPORT, $(CMS_COMPILE_FLAGS)),)
	OBJS_LIB += ClockTransport.o
	DPOE_OBJS += OamDpoeClockTrans.o
endif

CTC_OBJS_DEP = CtcAuthentication.c CtcEncryption.c CtcEthControl.c CtcMcast.c CtcMLlid.c\
		  CtcOamDiscovery.c CtcOptDiag.c CtcStats.c CtcAlarms.c CtcOamFileTransfer.c CtcOamGet.c CtcOamSet.c \
		  CtcOnuOam.c CtcOnuUtils.c CtcReport.c CtcVlan.c OamCtcPowerSave.c \
		  CtcClassification.c CtcOamIf.c

DPOE_OBJS_DEP = OamDpoeOnu.c OamDpoeGet.c OamDpoeSet.c DpoeOamIf.c OamDpoeMgmt.c OamDpoeOptProtect.c OamDpoeMgmtCommon.c OamDpoeBridge.c \
			OamDpoeBridgeCommon.c OamDpoeStats.c OamDpoeStatsCommon.c OamDpoeFileCommon.c OamDpoeFile.c OamDpoeEventCommon.c OamDpoeEvent.c \
			OamDpoeFrmProcCommon.c OamDpoeFrmProc.c DpoeOptDiag.c OamDpoeUni.c \
			OamDpoeSlaCommon.c OamDpoeSla.c EapTls.c Sha1.c Rsa.c OamDpoeDemarc.c OamDpoeSecCommon.c OamDpoeSec.c OamDpoeMcast.c \
			OamDpoeTekGet.c OamDpoeTekSet.c OamDpoeTeknovus.c

BCM_OBJS_DEP = OamFileTransfer.c OamGeneralActions.c OamPortOpsCom.c OamTeknovus.c OamPolicer.c OamShaper.c \
			OamTekSecCommon.c OamTekSec.c OamSys.c OamStats.c OamPortLrn.c OamTekEventCommon.c OamTekEvent.c \
			OamTekNegCom.c OamTekNeg.c TekOamIf.c SampleOam.c SampleOamIf.c OamTekSet.c OamTekUserRules.c OamOptCtrl.c OamQueue.c
			
OBJS_DEP = TkOsAlSem.c TkOsAlThread.c TkOsAlMsg.c TkOsAlDataInOut.c TkOsAlCommon.c TkOamMem.c \
          TkOamApiCommon.c TkOamProcess.c TkSdkInitApi.c TkSdkDebug.c TkOs.c \
          OnuOs.c OsAstEvt.c OsAstMsgQ.c OntConfigDb.c EponDevInfo.c Oui.c Pbi.c SysInfo.c Stream.c PowerSave.c Holdover.c\
		  Alarms.c PonManager.c PonMgrEncryptCommon.c PonMgrEncrypt.c UniConfigDb.c OamIeeeAutoNeg.c\
          EthFrameUtil.c OamPortUtil.c OamUtil.c OamEpon.c OamGet.c OamSet.c OamIeeeCommon.c OamIeeeAutoNegCommon.c \
          OamIeeeFecCommon.c OamIeeeMacCommon.c OamOnu.c MacLearning.c  \
		  OamIeeeEvent.c OamIeeePhyCommon.c OamIeee.c OamIeeePhy.c OntmTimer.c OamProcessInit.c  \
		  Md5.c DataPathRules.c Stats.c Mcast.c
ifneq ($(findstring CLOCK_TRANSPORT, $(CMS_COMPILE_FLAGS)),)
	OBJS_DEP += ClockTransport.c
	DPOE_OBJS_DEP += OamDpoeClockTrans.c
endif
		  
ifneq ($(findstring CTC_OAM_BUILD, $(CMS_COMPILE_FLAGS)),)
	OBJS_LIB += $(CTC_OBJS)
	OBJS_DEP += $(CTC_OBJS_DEP)
endif

ifneq ($(findstring BCM_OAM_BUILD, $(CMS_COMPILE_FLAGS)),)
	OBJS_LIB += $(BCM_OBJS)
	OBJS_DEP += $(BCM_OBJS_DEP)
endif

ifneq ($(findstring DPOE_OAM_BUILD, $(CMS_COMPILE_FLAGS)),)
	OBJS_LIB += $(DPOE_OBJS)
	OBJS_DEP += $(DPOE_OBJS_DEP)
endif

#SDK_LIB=$(OBJ_DIR)/$(LIB)
SDK_LIB=$(LIB)

#SDK_APP=$(OBJ_DIR)/sdkApp

VPATH =   $(SDK_BASE)/api:\
          $(SDK_BASE)/common:\
				  $(SDK_BASE)/msgprocess:\
		      $(SDK_BASE)/platform/$(PLATFORM_SRC_DIR):\
				  $(SDK_BASE)/oamprocess/If/Oam:\
		      $(SDK_BASE)/oamprocess/If/Oam/CtcOam:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe/Bridge:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe/Event:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe/FrmProc:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe/File:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe/Mgmt:\
              $(SDK_BASE)/oamprocess/If/Oam/Dpoe/OptProtect:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe/Sec:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe/Sla:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe/Stats:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe/Clock:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe/TekOam:\
		      $(SDK_BASE)/oamprocess/If/Oam/Teknovus:\
		      $(SDK_BASE)/oamprocess/If/Oam/Teknovus/Event:\
		      $(SDK_BASE)/oamprocess/If/Oam/Teknovus/Neg:\
		      $(SDK_BASE)/oamprocess/If/Oam/Teknovus/PortOps:\
		      $(SDK_BASE)/oamprocess/If/Oam/Teknovus/Security:\
		      $(SDK_BASE)/oamprocess/If/Oam/Ieee/AutoNeg:\
		      $(SDK_BASE)/oamprocess/If/Oam/Ieee/AutoNeg/V2:\
				  $(SDK_BASE)/oamprocess/If/Oam/Ieee/Event:\
				  $(SDK_BASE)/oamprocess/If/Oam/Ieee/Fec:\
				  $(SDK_BASE)/oamprocess/If/Oam/Ieee/Mac:\
				  $(SDK_BASE)/oamprocess/If/Oam/Ieee:\
				  $(SDK_BASE)/oamprocess/If/Oam/Ieee/Phy:\
				  $(SDK_BASE)/oamprocess/If/Oam/Ieee/Phy/V2:\
				  $(SDK_BASE)/oamprocess/If/Oam/Ieee/V2:\
				  $(SDK_BASE)/oamprocess/If/Oam/SampleOam:\
				  $(SDK_BASE)/oamprocess/If/Oam/Utils:\
				  $(SDK_BASE)/oamprocess/App/CtcClassification:\
				  $(SDK_BASE)/oamprocess/App/CtcMLlid:\
				  $(SDK_BASE)/oamprocess/App/CtcApp:\
				  $(SDK_BASE)/oamprocess/App/DpoeApp:\
				  $(SDK_BASE)/oamprocess/App/Master:\
				  $(SDK_BASE)/oamprocess/App/PowerSave:\
				  $(SDK_BASE)/oamprocess/App/DpoeAuth:\
				  $(SDK_BASE)/oamprocess/App/DpoeDemarc:\
				  $(SDK_BASE)/oamprocess/App/Rules:\
          $(SDK_BASE)/oamprocess/App/Holdover:\
          $(SDK_BASE)/oamprocess/App/MacLearning:\
          $(SDK_BASE)/oamprocess/App/Mcast/Common:\
          $(SDK_BASE)/oamprocess/App/OsAssert:\
          $(SDK_BASE)/oamprocess/App/ClockTransport:\
          $(SDK_BASE)/oamprocess/Base:\
          $(SDK_BASE)/oamprocess/Base/Generic:\
		  $(SDK_BASE)/oamprocess/Base/Stats:\
          $(SDK_BASE)/oamprocess/Base/Alarms/V2:\
				  $(SDK_BASE)/oamprocess/Base/OntDirector:\
				  $(SDK_BASE)/oamprocess/Base/PonManager/V1/PonMgr:\
				  $(SDK_BASE)/oamprocess/Base/PonManager/Fec/V2:\
				  $(SDK_BASE)/oamprocess/Base/PonManager/Encrypt/V2:\
				  $(SDK_BASE)/oamprocess/Base/PonManager/Encrypt:\
				  $(SDK_BASE)/oamprocess/Base/UniManager/V1:\
  	      $(SDK_BASE)/oamprocess/Base/OnuOs/V1:\
  	      $(SDK_BASE)/oamprocess/Driver/TK4701/Timer:\
				  $(SDK_BASE)/oamprocess/Driver/TK4701/Pbi:\
          $(SDK_BASE)/oamprocess/BCM55030

#for DPoE, child files selection
ifneq ($(findstring DPOE_OAM_IMPL1_BUILD, $(CMS_COMPILE_FLAGS)),)
	VPATH += $(SDK_BASE)/oamprocess/If/Oam/Dpoe/Bridge/impl1:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe/Event/impl1:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe/FrmProc/impl1:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe/File/impl1:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe/Mgmt/impl1:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe/Sec/impl1:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe/Sla/impl1:\
	      	  $(SDK_BASE)/oamprocess/If/Oam/Dpoe/Stats/impl1:\
              $(SDK_BASE)/oamprocess/If/Oam/Dpoe/Uni/impl1:
endif
ifneq ($(findstring DPOE_OAM_STUB_BUILD, $(CMS_COMPILE_FLAGS)),)
	VPATH += $(SDK_BASE)/oamprocess/If/Oam/Dpoe/Bridge/Stub:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe/Event/Stub:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe/FrmProc/Stub:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe/File/Stub:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe/Mgmt/Stub:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe/Sec/Stub:\
		      $(SDK_BASE)/oamprocess/If/Oam/Dpoe/Sla/Stub:\
	      	$(SDK_BASE)/oamprocess/If/Oam/Dpoe/Stats/Stub:
endif

CFLAGS +=-Wall -Wstrict-prototypes -Wno-trigraphs -O2
CFLAGS +=-fomit-frame-pointer -fno-strict-aliasing -fno-common
CFLAGS +=-Wswitch -enum
CFLAGS +=-I$(SDK_BASE)/api/h
CFLAGS +=-I$(SDK_BASE)/config/
CFLAGS +=-I$(SDK_BASE)/common/h
CFLAGS +=-I$(SDK_BASE)/msgprocess/h
CFLAGS +=-I$(SDK_BASE)/oamprocess/h
CFLAGS +=-I$(SDK_BASE)/platform/h
CFLAGS +=-I$(SDK_BASE)/platform/h/$(PLATFORM_SRC_DIR)

#oamprocess If/Oam directory
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Utils
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Ieee/V2
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Ieee/AutoNeg
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Ieee/Fec
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Ieee/Mac
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Ieee/Event
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Ieee/Phy
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Ieee
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Teknovus
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Teknovus/Neg
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Teknovus/PortOps
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Teknovus/Security
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Teknovus/Event
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/CtcOam
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/Bridge
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/Event
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/File
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/FrmProc
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/Mgmt
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/OptProtect
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/Sec
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/Sla
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/Stats
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/Clock
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/TekOam
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/SampleOam

#for DPoE, child head file selection
ifneq ($(findstring DPOE_OAM_IMPL1_BUILD, $(CMS_COMPILE_FLAGS)),)
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/Bridge/impl1
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/Event/impl1
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/File/impl1
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/FrmProc/impl1
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/Mgmt/impl1
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/Sec/impl1
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/Sla/impl1
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/Stats/impl1
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/Uni/impl1
endif

ifneq ($(findstring DPOE_OAM_STUB_BUILD, $(CMS_COMPILE_FLAGS)),)
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/Bridge/Stub
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/Event/Stub
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/File/Stub
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/FrmProc/Stub
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/Mgmt/Stub
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/Sec/Stub
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/Sla/Stub
CFLAGS +=-I$(SDK_BASE)/oamprocess/If/Oam/Dpoe/Stats/Stub
endif

#oamprocess ApiResLayer directory
CFLAGS +=-I$(SDK_BASE)/oamprocess/ApiResLayer/V2

#oamprocess APP directory
CFLAGS +=-I$(SDK_BASE)/oamprocess/App/CtcApp
CFLAGS +=-I$(SDK_BASE)/oamprocess/App/CtcClassification
CFLAGS +=-I$(SDK_BASE)/oamprocess/App/CtcMLlid
CFLAGS +=-I$(SDK_BASE)/oamprocess/App/DpoeApp
CFLAGS +=-I$(SDK_BASE)/oamprocess/App/Holdover
CFLAGS +=-I$(SDK_BASE)/oamprocess/App/UserTraffic
CFLAGS +=-I$(SDK_BASE)/oamprocess/App/OsAssert
CFLAGS +=-I$(SDK_BASE)/oamprocess/App/OntmMpcp
CFLAGS +=-I$(SDK_BASE)/oamprocess/App/Mcast/2000-V1
CFLAGS +=-I$(SDK_BASE)/oamprocess/App/Mcast/Common
CFLAGS +=-I$(SDK_BASE)/oamprocess/App/PowerSave
CFLAGS +=-I$(SDK_BASE)/oamprocess/App/DpoeAuth
CFLAGS +=-I$(SDK_BASE)/oamprocess/App/DpoeDemarc
CFLAGS +=-I$(SDK_BASE)/oamprocess/App/Stats
CFLAGS +=-I$(SDK_BASE)/oamprocess/App/Master
CFLAGS +=-I$(SDK_BASE)/oamprocess/App/MacLearning
CFLAGS +=-I$(SDK_BASE)/oamprocess/App/ClockTransport
CFLAGS +=-I$(SDK_BASE)/oamprocess/App/Rules

#oamprocess Base directory
CFLAGS +=-I$(SDK_BASE)/oamprocess/Base
CFLAGS +=-I$(SDK_BASE)/oamprocess/Base/Stats
CFLAGS +=-I$(SDK_BASE)/oamprocess/Base/Generic/Fds
CFLAGS +=-I$(SDK_BASE)/oamprocess/Base/Vlan
CFLAGS +=-I$(SDK_BASE)/oamprocess/Base/Alarms/V2
CFLAGS +=-I$(SDK_BASE)/oamprocess/Base/Alarms
CFLAGS +=-I$(SDK_BASE)/oamprocess/Base/PonManager/Encrypt/
CFLAGS +=-I$(SDK_BASE)/oamprocess/Base/PonManager/V1/PonMgr
CFLAGS +=-I$(SDK_BASE)/oamprocess/Base/PonManager/V1/PonCfgDb/Stub
CFLAGS +=-I$(SDK_BASE)/oamprocess/Base/OnuOs/V1
CFLAGS +=-I$(SDK_BASE)/oamprocess/Base/UniManager/V1
CFLAGS +=-I$(SDK_BASE)/oamprocess/Base/LoadMgmt

#oamprocess Common directory
CFLAGS +=-I$(SDK_BASE)/oamprocess/Common

#oamprocess BCM55030 directory
CFLAGS +=-I$(SDK_BASE)/oamprocess/BCM55030/Products/2000


#oamprocess Capability directory
CFLAGS +=-I$(SDK_BASE)/oamprocess/Capability/BCM55030

#oamprocess Driver directory
CFLAGS +=-I$(SDK_BASE)/oamprocess/Driver

#oamprocedss Type directory
CFLAGS +=-I$(SDK_BASE)/oamprocess/Type/Generic

#bridge directory
CFLAGS +=-I$(BUILD_DIR)/kernel
                         
CFLAGS +=$(ALLOWED_INCLUDE_PATHS_2)

.PHONY : all 


BMD_TARGETS = shell api pkgsrc shared
BMD_LIBNAMES = $(addprefix -lbmd,$(BMD_TARGETS))
CDK_TARGETS = shell main pkgsrc shared sym libc
CDK_LIBNAMES = $(addprefix -lcdk,$(CDK_TARGETS))
PHY_TARGETS = pkgsrc generic util sym
PHY_LIBNAMES = $(addprefix -lphy,$(PHY_TARGETS))

LIBS += $(BMD_LIBNAMES)
LIBS += $(PHY_LIBNAMES)
LIBS += $(CDK_LIBNAMES)


$(SDK_LIB):$(OBJS_LIB) 
#	$(AR) vru $@ $(OBJS_LIB)
	$(CC) -shared $(BCM_LD_FLAGS) -Wl,--whole-archive -o $@ $(OBJS_LIB) -Wl,--no-whole-archive
	mv $(SDK_LIB) $(SDK_BASE)

#$(LIBDIR)/$(LIBNAME).$(LIBSUFFIX): $(BOBJS) $(BLDDIR)/.tree 
#	@$(ECHO) Building library $(LIBNAME) ...
#	$(CC) -shared $(BCM_LD_FLAGS) -Wl,--whole-archive -o $@ $(BOBJS)  -Wl,--no-whole-archive


lib:$(SDK_LIB)

#app:$(OBJS_LIB) $(OBJ_APP)
#	$(CC) -g $(OBJS_LIB) $(OBJ_APP) -o sdkApp -lpthread
	
#depend:$(OBJS_DEP)
#	$(CC) -M $(CFLAGS) $^ > $@


#all:lib app
all: lib

#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules


-include $(OBJS:.o=.d)

clean:
	rm -rf depend
	rm -rf *.o
	rm -rf *.d
	rm -rf *.d.*
	rm -f $(SDK_BASE)/$(SDK_LIB)
#	rm -f $(SDK_APP)	
	rm -f $(OBJ_DIR)/*.o

