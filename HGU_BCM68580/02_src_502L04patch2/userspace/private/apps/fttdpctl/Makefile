#***********************************************************************
#
# <:copyright-BRCM:2004:proprietary:standard
# 
#    Copyright (c) 2004 Broadcom 
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
#***********************************************************************/


PROGRAM_NAME:= fttdpctl
BOOTER_NAME:= booter.bin
FIRMWARE_NAME:= modem.bin
DSL_CMD_SCRIPT_NAME:= dsl_cmd_script.txt

SOURCE_FILES:= bcm_devObject.o\
bcm_hmiApi.o\
bcm_traceAndDebugApi.o\
bcm_utils.o\
bcm_traceAndDebugRfc.o\
bcm_lineObject.o\
bcm_globalObject.o\
bcm_modemUtils.o\
bcm_reportMessage.o\
bcm_api.o\
bcm_genericUtils.o\
bcm_vectoringGroup.o\
bcm_getShowtimeDropReason.o\
bcm_synchronizeDevice.o\
bcm_pollingTask.o\
bcm_board.o\
bcm_seltServices.o\
bcm_handleInterrupt.o\
bcm_seltObject.o\
bcm_counterMgr.o\
bcm_overheadServices.o\
bcm_lineObjectBase.o\
bcm_traceAndDebugBcmObject.o\
bcm_testServices.o\
bcm_recoverOrReport.o\
bcm_coreDump.o\
bcm_bondingObject.o\
bcm_getVdslTestParams.o\
bcm_dbgUtils.o\
bcm_traceAndDebugCommon.o\
bcm_traceAndDebugBcm.o\
bcm_traceAndDebugUtils.o\
bcm_traceAndDebugDbg.o\
bcm_traceAndDebug.o\
bcm_getDebugStringArgs.o\
bcm_fetchDataDump.o\
bcm_logAndDebugPrint.o\
bcm_logAndDebugSupport.o\
bcm_processResult.o\
bcm_getFirmwareInfo.o\
bcm_afeConstants.o\
bcm_boot.o\
bcm_getSelfTest.o\
bcm_configureLine.o\
bcm_configureDevice.o\
bcm_hmiQA.o\
bcm_proxyDev.o\
bcm_stpSocketServer.o\
bcm_socketTargetConnect.o\
bcm_socket.o\
bcm_psdToolbox.o\
bcm_stdAdsl2PlusShapes.o\
bcm_txFilters.o\
bcm_stdUpboRefPsd.o\
bcm_stdVdslShapes.o\
bcm_osWrap.o\
bcm_semaphore.o\
bcm_msgq.o\
bcm_hmiLcaMsg.o\
bcm_hmiBootMsg.o\
bcm_hmiMsgHdr.o\
bcm_hmiBondingMsg.o\
bcm_hmiDebugMsg.o\
bcm_hmiCoreMsg.o\
bcm_hmiLineMsg.o\
bcm_message.o\
bcm_types.o\
bcm_dbgPrint.o\
bcm_layout.o\
bcm_lz77.o\
bcm_errorHandler.o\
bcm_loadImages.o\
bcm_stp.o\
bcm_bigEndianByteSwap.o\
bcm_asciCodec.o\
bcm_genericOptions.o\
bcm_images.o\
bcm_timer.o\
bcm_modem.o


CFLAGS = -Wall -ggdb -O2 -W -Wpointer-arith -Wstrict-prototypes -DBCM_SNMP_PROXY_PROJECT -DBCM_MAX_NR_OF_EXTRA_SOCKET_SERVERS=4 -DBCM_LOG_VIEW_HMIQA=1 -DBCM_LOG_VIEW_PROXY=2 -DBCM_LOG_VIEW_COUNT=3 -DBCM_LOG_VIEW_OVH=4 -DBCM_LOG_VIEW_CALLB=5 -DBCM_LOG_NB_VIEWS=6 -DPLATFORM_IS_LINUX -fPIC -DNO_HOST_ACCESS -DETHERNET_PHY -DBRCM_FTTDP -lm -lpthread -lrt

CFLAGS += -I$(BRCMDRIVERS_DIR)/opensource/include/bcm963xx -I$(SHARED_DIR)/opensource/include/bcm963xx
CFLAGS += -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD) -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)
#CFLAGS += -I$(BRCMDRIVERS_DIR)/opensource/char/rdpa_gpl/bcm96838/include
#CFLAGS += -I$(BRCMDRIVERS_DIR)/opensource/char/bdmf/bcm96838/framework
#CFLAGS += -I$(BRCMDRIVERS_DIR)/opensource/char/bdmf/bcm96838/system/linux


%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

all: $(PROGRAM_NAME)

install: $(PROGRAM_NAME)
	install -m 755 $(PROGRAM_NAME) $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/$(PROGRAM_NAME)
	mkdir -p $(INSTALL_DIR)/sbin
	ln -sf ../bin/$(PROGRAM_NAME) $(INSTALL_DIR)/sbin/fttdpctl

dynamic: $(PROGRAM_NAME) install

	install $(BOOTER_NAME) $(INSTALL_DIR)/etc

	install $(FIRMWARE_NAME) $(INSTALL_DIR)/etc

	install $(DSL_CMD_SCRIPT_NAME) $(INSTALL_DIR)/etc

	install rgmiictl.sh $(INSTALL_DIR)/etc/init.d

#	install extphy.sh $(INSTALL_DIR)/etc/init.d

	install fttdp.sh $(INSTALL_DIR)/etc/init.d

	(cd $(INSTALL_DIR)/etc/rc3.d; rm -f S75rgmiictl; ln -s ../init.d/rgmiictl.sh S75rgmiictl)

#	(cd $(INSTALL_DIR)/etc/rc3.d; rm -f S80extphy; ln -s ../init.d/extphy.sh S80extphy)
	(cd $(INSTALL_DIR)/etc/rc3.d; rm -f S82fttdp; ln -s ../init.d/fttdp.sh S82fttdp)

static: $(PROGRAM_NAME).a

$(PROGRAM_NAME): $(SOURCE_FILES)
	$(CC) $(CFLAGS) -Wl,--allow-shlib-undefined -o $@ $^

$(PROGRAM_NAME).a: $(SOURCE_FILES)
	$(AR) rcs $(PROGRAM_NAME).a $^

clean:
	-rm -f $(PROGRAM_NAME) *.o *.a

