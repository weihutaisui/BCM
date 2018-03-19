/*****************************************************************************
//
// Copyright (c) 2005-2012 Broadcom Corporation
// All Rights Reserved
//
// <:label-BRCM:2012:proprietary:standard
// 
//  This program is the proprietary software of Broadcom and/or its
//  licensors, and may only be used, duplicated, modified or distributed pursuant
//  to the terms and conditions of a separate, written license agreement executed
//  between you and Broadcom (an "Authorized License").  Except as set forth in
//  an Authorized License, Broadcom grants no license (express or implied), right
//  to use, or waiver of any kind with respect to the Software, and Broadcom
//  expressly reserves all rights in and to the Software and all intellectual
//  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
//  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
//  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
// 
//  Except as expressly set forth in the Authorized License,
// 
//  1. This program, including its structure, sequence and organization,
//     constitutes the valuable trade secrets of Broadcom, and you shall use
//     all reasonable efforts to protect the confidentiality thereof, and to
//     use this information only in connection with your use of Broadcom
//     integrated circuit products.
// 
//  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
//     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
//     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
//     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
//     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
//     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
//     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
//     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
//     PERFORMANCE OF THE SOFTWARE.
// 
//  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
//     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
//     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
//     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
//     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
//     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
//     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
//     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
//     LIMITED REMEDY.
// :>
//
******************************************************************************
//
//  Filename:       queuemgtparams.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "queuemgtparams.h"


static VarTemplate StateVariables[] = { 
    { "Enable", "", VAR_BOOL }, 
    { "MaxQueues", "" ,VAR_USHORT},
    { "MaxClassificationEntries", "", VAR_USHORT},
    { "MaxAppEntries", "", VAR_USHORT},    
    { "MaxFlowEntries", "", VAR_USHORT},        
    { "MaxPolicerEntries", "", VAR_USHORT},
    { "MaxQueueEntries", "", VAR_USHORT},
    { "DefaultForwardingPolicy", "", VAR_USHORT},
    { "DefaultPolicer", "", VAR_SHORT},
    { "DefaultQueue", "", VAR_SHORT},
    { "DefaultDSCPMark", "", VAR_LONG},
    { "DefaultEthernetPriorityMark", "", VAR_SHORT},
    { "AvailableAppList", "", VAR_STRING},
    { "ClassificationNumberOfEntries", "", VAR_USHORT},
    { "ClassificationKey", "", VAR_USHORT},
    { "ClassificationEnable", "", VAR_BOOL},
    { "ClassificationStatus", "", VAR_STRING},
    { "ClassificationOrder", "", VAR_USHORT},
    { "ClassInterface", "", VAR_STRING},
    { "DestIP", "", VAR_STRING},
    { "DestMask", "", VAR_STRING},
    { "DestIPExclude", "", VAR_BOOL},
    { "SourceIP", "", VAR_STRING},
    { "SourceMask", "", VAR_STRING},
    { "SourceIPExclude", "", VAR_BOOL},
    { "Protocol", "", VAR_SHORT},
    { "ProtocolExclude", "", VAR_BOOL},
    { "DestPort", "", VAR_SHORT},
    { "DestPortRangeMax", "", VAR_SHORT},
    { "DestPortExclude", "", VAR_BOOL},
    { "SourcePort", "", VAR_SHORT},
    { "SourcePortRangeMax", "", VAR_SHORT},
    { "SourcePortExclude", "", VAR_BOOL},
    { "SourceMACAddress", "", VAR_STRING},
    { "SourceMACMask", "", VAR_STRING},
    { "SourceMACExclude", "", VAR_BOOL},
    { "DestMACAddress", "", VAR_STRING},
    { "DestMACMask", "", VAR_STRING},
    { "DestMACExclude", "", VAR_BOOL},
    { "Ethertype", "", VAR_SHORT},
    { "EthertypeExclude", "", VAR_BOOL},
    { "SSAP", "", VAR_LONG},
    { "SSAPExclude", "", VAR_BOOL},
    { "DSAP", "", VAR_LONG},
    { "DSAPExclude", "", VAR_BOOL},
    { "LLCControl", "", VAR_SHORT},
    { "LLCControlExclude", "", VAR_BOOL},
    { "SNAPOUI", "", VAR_LONG},
    { "SNAPOUIExclude", "", VAR_BOOL},
    { "SourceVendorClassID", "", VAR_STRING},
    { "SourceVendorClassIDExclude", "", VAR_BOOL},
    { "DestVendorClassID", "", VAR_STRING},
    { "DestVendorClassIDExclude", "", VAR_BOOL},
    { "SourceClientID", "", VAR_STRING},
    { "SourceClientIDExclude", "", VAR_BOOL},
    { "DestClientID", "", VAR_STRING},
    { "DestClientIDExclude", "", VAR_BOOL},
    { "SourceUserClassID", "", VAR_STRING},
    { "SourceUserClassIDExclude", "", VAR_BOOL},
    { "DestUserClassID", "", VAR_STRING},
    { "DestUserClassIDExclude", "", VAR_BOOL},
    { "TCPACK", "", VAR_BOOL},
    { "TCPACKExclude", "", VAR_BOOL},
    { "IPLengthMin", "", VAR_USHORT},
    { "IPLengthMax", "", VAR_USHORT},
    { "IPLengthExclude", "", VAR_BOOL},
    { "DSCPCheck", "", VAR_SHORT},
    { "DSCPExclude", "", VAR_BOOL},
    { "DSCPMark", "", VAR_SHORT},    
    { "EthernetPriorityCheck", "", VAR_SHORT},
    { "EthernetPriorityExclude", "", VAR_BOOL},
    { "EthernetPriorityMark", "", VAR_SHORT},	
    { "VLANIDCheck", "", VAR_SHORT},
    { "VLANIDExclude", "", VAR_BOOL},
    { "ForwardingPolicy", "", VAR_USHORT},    
    { "ClassPolicer", "", VAR_USHORT},
    { "ClassQueue", "", VAR_SHORT},
    { "ClassApp", "", VAR_SHORT},
    { "AppNumberOfEntries", "", VAR_USHORT},
    { "AppKey", "", VAR_USHORT},
	{ "AppEnable", "", VAR_BOOL},
    { "AppStatus", "", VAR_STRING},
    { "ProtocolIdentifier", "", VAR_STRING},
    { "AppName", "", VAR_STRING},
	{ "AppDefaultForwardingPolicy", "", VAR_USHORT},
	{ "AppDefaultPolicer", "", VAR_SHORT},
	{ "AppDefaultQueue", "", VAR_SHORT},
    { "AppDefaultDSCPMark", "", VAR_SHORT},
    { "AppDefaultEthernetPriorityMark", "", VAR_SHORT},
    { "FlowNumberOfEntries", "", VAR_USHORT},
    { "FlowKey", "", VAR_USHORT}, 
    { "FlowEnable", "", VAR_BOOL},
    { "FlowStatus", "", VAR_STRING},
	{ "FlowType", "", VAR_STRING},
	{ "FlowTypeParameters", "", VAR_STRING},
    { "FlowName", "", VAR_STRING},
    { "AppIdentifier", "", VAR_SHORT},
	{ "FlowForwardingPolicy", "", VAR_USHORT},
	{ "FlowPolicer", "", VAR_USHORT},
    { "FlowQueue", "", VAR_SHORT},
    { "FlowDSCPMark", "", VAR_SHORT},
	{ "FlowEthernetPriorityMark", "", VAR_SHORT},
	{ "PolicerNumberOfEntries", "", VAR_USHORT},
    { "PolicerKey", "", VAR_USHORT},
	{ "PolicerEnable", "", VAR_USHORT},
    { "PolicerStatus", "", VAR_STRING},
	{ "CommittedRate", "", VAR_ULONG},
	{ "CommittedBurstSize", "", VAR_ULONG},
	{ "ExcessBurstSize", "", VAR_ULONG},
    { "PeakRate", "", VAR_ULONG},
    { "PeakBurstSize", "", VAR_ULONG},
    { "MeterType", "", VAR_USHORT},
    { "PossibleMeterTypes", "", VAR_STRING},
	{ "ConformingAction", "", VAR_STRING},
    { "PartialConformingAction", "", VAR_STRING},
    { "NonConformingAction", "", VAR_STRING},
	{ "CountedPackets", "", VAR_ULONG},
	{ "CountedBytes", "", VAR_ULONG},
	{ "QueueNumberOfEntries", "", VAR_USHORT},
    { "QueueKey", "", VAR_USHORT},
    { "QueueEnable", "", VAR_BOOL},
    { "QueueStatus", "", VAR_STRING},
    { "QueueInterface", "", VAR_STRING},
	{ "QueueBufferLength", "", VAR_ULONG},
    { "QueueWeight", "", VAR_USHORT},
	{ "QueuePrecedence", "", VAR_USHORT},
    { "REDThreshold", "", VAR_USHORT},
	{ "REDPercentage", "", VAR_USHORT},
	{ "DropAlgorithm", "", VAR_STRING},
	{ "SchedulerAlgorithm", "", VAR_STRING},
	{ "ShapingRate", "", VAR_ULONG},
	{ "ShapingBurstSize", "", VAR_USHORT},
    { NULL }
};

static Action _GetInfo = { 
   "GetInfo", GetQueueMgt_Info,
   (Param []) {
      {"NewEnable", VAR_Enable, VAR_OUT},
      {"NewMaxQueues", VAR_MaxQueues, VAR_OUT},	  	
      {"NewMaxClassificationEntries", VAR_MaxClassificationEntries, VAR_OUT},      
      {"NewMaxAppEntries", VAR_MaxAppEntries, VAR_OUT},      
      {"NewMaxFlowEntries", VAR_MaxFlowEntries, VAR_OUT},      
      {"NewMaxPolicerEntries", VAR_MaxPolicerEntries, VAR_OUT},      
      {"NewMaxQueueEntries", VAR_MaxQueueEntries, VAR_OUT},      
      {"NewDefaultForwardingPolicy", VAR_DefaultForwardingPolicy, VAR_OUT},      
      {"NewDefaultPolicer", VAR_DefaultPolicer, VAR_OUT},      
      {"NewDefaultQueue", VAR_DefaultQueue, VAR_OUT},      
      {"NewDefaultDSCPMark", VAR_DefaultDSCPMark, VAR_OUT},      
      {"NewDefaultEthernetPriorityMark", VAR_DefaultEthernetPriorityMark, VAR_OUT},      
	  {"NewAvailableAppList", VAR_AvailableAppList, VAR_OUT},      
      { 0 }
   }
};

static Action _SetEnable = { 
   "SetEnable", SetQueueMGTEnable,
   (Param []) {
      {"NewEnable", VAR_Enable, VAR_IN},
      { 0 }
   }
};

static Action _SetDefaultBehavior = { 
   "SetDefaultBehavior", SetDefaultBehavior,
   (Param []) {
      {"NewDefaultForwardingPolicy", VAR_DefaultForwardingPolicy, VAR_IN},
      {"NewDefaultPolicer", VAR_DefaultPolicer, VAR_IN},
      {"NewDefaultQueue", VAR_DefaultQueue, VAR_IN},
      {"NewDefaultDSCPMark", VAR_DefaultDSCPMark, VAR_IN},
      {"NewDefaultEthernetPriorityMark", VAR_DefaultEthernetPriorityMark, VAR_IN},      
      { 0 }
   }
};

static Action _AddClassificationEntry = { 
   "AddClassificationEntry", AddClassificationEntry,
   (Param []) {
      {"NewClassificationEnable", VAR_ClassificationEnable, VAR_IN},
      {"NewClassificationOrder", VAR_ClassificationOrder, VAR_IN},
      {"NewClassInterface", VAR_ClassInterface, VAR_IN},      
      {"NewDestIP", VAR_DestIP, VAR_IN},      
      {"NewDestMask", VAR_DestMask, VAR_IN},      
      {"NewDestIPExclude", VAR_DestIPExclude, VAR_IN},      
      {"NewSourceIP", VAR_SourceIP, VAR_IN},      
      {"NewSourceMask", VAR_SourceMask, VAR_IN},      
      {"NewSourceIPExclude", VAR_SourceIPExclude, VAR_IN},      
      {"NewProtocol", VAR_Protocol, VAR_IN},      
      {"NewProtocolExclude", VAR_ProtocolExclude, VAR_IN},      
      {"NewDestPort", VAR_DestPort, VAR_IN},      
      {"NewDestPortRangeMax", VAR_DestPortRangeMax, VAR_IN},      
      {"NewDestPortExclude", VAR_DestPortExclude, VAR_IN},      
      {"NewSourcePort", VAR_SourcePort, VAR_IN},      
      {"NewSourcePortRangeMax", VAR_SourcePortRangeMax, VAR_IN},      
      {"NewSourcePortExclude", VAR_SourcePortExclude, VAR_IN},      
      {"NewSourceMACAddress", VAR_SourceMACAddress, VAR_IN},      
      {"NewSourceMACMask", VAR_SourceMACMask, VAR_IN},      
      {"NewSourceMACExclude", VAR_SourceMACExclude, VAR_IN},            
      {"NewDestMACAddress", VAR_DestMACAddress, VAR_IN},      
      {"NewDestMACMask", VAR_DestMACMask, VAR_IN},      
      {"NewDestMACExclude", VAR_DestMACExclude, VAR_IN},            
      {"NewEthertype", VAR_Ethertype, VAR_IN},            
      {"NewEthertypeExclude", VAR_EthertypeExclude, VAR_IN},            
      {"NewSSAP", VAR_SSAP, VAR_IN},            
      {"NewSSAPExclude", VAR_SSAPExclude, VAR_IN},            
      {"NewDSAP", VAR_DSAP, VAR_IN},                  
      {"NewDSAPExclude", VAR_DSAPExclude, VAR_IN},                  
      {"NewLLCControl", VAR_LLCControl, VAR_IN},            
      {"NewLLCControlExclude", VAR_LLCControlExclude, VAR_IN},                  
      {"NewSNAPOUI", VAR_SNAPOUI, VAR_IN},                  
      {"NewSNAPOUIExclude", VAR_SNAPOUIExclude, VAR_IN},                  
      {"NewSourceVendorClassID", VAR_SourceVendorClassID, VAR_IN},                  
      {"NewSourceVendorClassIDExclude", VAR_SourceVendorClassIDExclude, VAR_IN},                  
      {"NewDestVendorClassID", VAR_DestVendorClassID, VAR_IN},                  
      {"NewDestVendorClassIDExclude", VAR_DestVendorClassIDExclude, VAR_IN},                  
      {"NewSourceClientID", VAR_SourceClientID, VAR_IN},         
      {"NewSourceClientIDExclude", VAR_SourceClientIDExclude, VAR_IN},
      {"NewDestClientID", VAR_DestClientID, VAR_IN},
      {"NewDestClientIDExclude", VAR_DestClientIDExclude, VAR_IN},
      {"NewSourceUserClassID", VAR_SourceUserClassID, VAR_IN}, 
      {"NewSourceUserClassIDExclude", VAR_SourceUserClassIDExclude, VAR_IN},
      {"NewDestUserClassID", VAR_DestUserClassID, VAR_IN},
      {"NewDestUserClassIDExclude", VAR_DestUserClassIDExclude, VAR_IN},
      {"NewTCPACK", VAR_TCPACK, VAR_IN},
      {"NewTCPACKExclude", VAR_TCPACKExclude, VAR_IN},
      {"NewIPLengthMin", VAR_IPLengthMin, VAR_IN},
      {"NewIPLengthMax", VAR_IPLengthMax, VAR_IN},
      {"NewIPLengthExclude", VAR_IPLengthExclude, VAR_IN},
      {"NewDSCPCheck", VAR_DSCPCheck, VAR_IN},      
      {"NewDSCPExclude", VAR_DSCPExclude, VAR_IN},      
      {"NewDSCPMark", VAR_DSCPMark, VAR_IN},      
      {"NewEthernetPriorityCheck", VAR_EthernetPriorityCheck, VAR_IN},
      {"NewEthernetPriorityExclude", VAR_EthernetPriorityExclude, VAR_IN},
      {"NewEthernetPriorityMark", VAR_EthernetPriorityMark, VAR_IN},
      {"NewVLANIDCheck", VAR_VLANIDCheck, VAR_IN},
      {"NewVLANIDExclude", VAR_VLANIDExclude, VAR_IN},
      {"NewForwardingPolicy", VAR_ForwardingPolicy, VAR_IN},
      {"NewClassPolicer", VAR_ClassPolicer, VAR_IN},
      {"NewClassQueue", VAR_ClassQueue, VAR_IN},
      {"NewClassApp", VAR_ClassApp, VAR_IN},
      {"NewClassificationKey", VAR_ClassificationKey, VAR_IN},
      { 0 }
   }
};

static Action _DeleteClassificationEntry = { 
   "DeleteClassificationEntry", DeleteClassificationEntry,
   (Param []) {
      {"NewClassificationKey", VAR_ClassificationKey, VAR_IN},   
      { 0 }
   }
};

static Action _GetSpecificClassificationEntry = { 
   "GetSpecificClassificationEntry", GetSpecificClassificationEntry,
   (Param []) {
      {"NewClassificationKey", VAR_ClassificationKey, VAR_IN},      	  
      {"NewClassificationEnable", VAR_ClassificationEnable, VAR_OUT},
      {"NewClassificationOrder", VAR_ClassificationOrder, VAR_OUT},	  	
      {"NewClassInterface", VAR_ClassInterface, VAR_OUT},      
      {"NewDestIP", VAR_DestIP, VAR_OUT},      
      {"NewDestMask", VAR_DestMask, VAR_OUT},      
      {"NewDestIPExclude", VAR_DestIPExclude, VAR_OUT},      
      {"NewSourceIP", VAR_SourceIP, VAR_OUT},      
      {"NewSourceMask", VAR_SourceMask, VAR_OUT},      
      {"NewSourceIPExclude", VAR_SourceIPExclude, VAR_OUT},      
      {"NewProtocol", VAR_Protocol, VAR_OUT},      
      {"NewProtocolExclude", VAR_ProtocolExclude, VAR_OUT},      
      {"NewDestPort", VAR_DestPort, VAR_OUT},      
      {"NewDestPortRangeMax", VAR_DestPortRangeMax, VAR_OUT},      
      {"NewDestPortExclude", VAR_DestPortExclude, VAR_OUT},      
      {"NewSourcePort", VAR_SourcePort, VAR_OUT},      
      {"NewSourcePortRangeMax", VAR_SourcePortRangeMax, VAR_OUT},      
      {"NewSourcePortExclude", VAR_SourcePortExclude, VAR_OUT},      
      {"NewSourceMACAddress", VAR_SourceMACAddress, VAR_OUT},      
      {"NewSourceMACMask", VAR_SourceMACMask, VAR_OUT},      
      {"NewSourceMACExclude", VAR_SourceMACExclude, VAR_OUT},            
      {"NewDestMACAddress", VAR_DestMACAddress, VAR_OUT},      
      {"NewDestMACMask", VAR_DestMACMask, VAR_OUT},      
      {"NewDestMACExclude", VAR_DestMACExclude, VAR_OUT},            
      {"NewEthertype", VAR_Ethertype, VAR_OUT},            
      {"NewEthertypeExclude", VAR_EthertypeExclude, VAR_OUT},            
#if 0           
      {"NewSSAP", VAR_SSAP, VAR_OUT},            
      {"NewSSAPExclude", VAR_SSAPExclude, VAR_OUT},            
      {"NewDSAP", VAR_DSAP, VAR_OUT},                  
      {"NewDSAPExclude", VAR_DSAPExclude, VAR_OUT},                  
      {"NewLLCControl", VAR_LLCControl, VAR_OUT},
      {"NewLLCControlExclude", VAR_LLCControlExclude, VAR_OUT},                  
      {"NewSNAPOUI", VAR_SNAPOUI, VAR_OUT},                  
      {"NewSNAPOUIExclude", VAR_SNAPOUIExclude, VAR_OUT},                  
#endif
      {"NewSourceVendorClassID", VAR_SourceVendorClassID, VAR_OUT},                  
      {"NewSourceVendorClassIDExclude", VAR_SourceVendorClassIDExclude, VAR_OUT},                  
#if 0
      {"NewDestVendorClassID", VAR_DestVendorClassID, VAR_OUT},                  
      {"NewDestVendorClassIDExclude", VAR_DestVendorClassIDExclude, VAR_OUT},                  
      {"NewSourceClientID", VAR_SourceClientID, VAR_OUT},
      {"NewSourceClientIDExclude", VAR_SourceClientIDExclude, VAR_OUT},
      {"NewDestClientID", VAR_DestClientID, VAR_OUT}, 
      {"NewDestClientIDExclude", VAR_DestClientIDExclude, VAR_OUT},
#endif
      {"NewSourceUserClassID", VAR_SourceUserClassID, VAR_OUT},
      {"NewSourceUserClassIDExclude", VAR_SourceUserClassIDExclude, VAR_OUT},
#if 0
      {"NewDestUserClassID", VAR_DestUserClassID, VAR_OUT},
      {"NewDestUserClassIDExclude", VAR_DestUserClassIDExclude, VAR_OUT},
      {"NewTCPACK", VAR_TCPACK, VAR_OUT},
      {"NewTCPACKExclude", VAR_TCPACKExclude, VAR_OUT},
      {"NewIPLengthMin", VAR_IPLengthMin, VAR_OUT},
      {"NewIPLengthMax", VAR_IPLengthMax, VAR_OUT},      
      {"NewIPLengthExclude", VAR_IPLengthExclude, VAR_OUT},      
#endif
      {"NewDSCPCheck", VAR_DSCPCheck, VAR_OUT},      
      {"NewDSCPExclude", VAR_DSCPExclude, VAR_OUT},      
      {"NewDSCPMark", VAR_DSCPMark, VAR_OUT},      
      {"NewEthernetPriorityCheck", VAR_EthernetPriorityCheck, VAR_OUT},
      {"NewEthernetPriorityExclude", VAR_EthernetPriorityExclude, VAR_OUT},
      {"NewEthernetPriorityMark", VAR_EthernetPriorityMark, VAR_OUT},
      {"NewVLANIDCheck", VAR_VLANIDCheck, VAR_OUT},
      {"NewVLANIDExclude", VAR_VLANIDExclude, VAR_OUT},
      {"NewForwardingPolicy", VAR_ForwardingPolicy, VAR_OUT},
      {"NewClassPolicer", VAR_ClassPolicer, VAR_OUT},
      {"NewClassQueue", VAR_ClassQueue, VAR_OUT},
      {"NewClassApp", VAR_ClassApp, VAR_OUT},
      { 0 }
   }
};

static Action _GetGenericClassificationEntry = { 
   "GetGenericClassificationEntry", GetGenericClassificationEntry,
   (Param []) {
      {"NewClassificationIndex", VAR_ClassificationNumberOfEntries, VAR_IN},
      {"NewClassificationKey", VAR_ClassificationKey, VAR_OUT},
      {"NewClassificationEnable", VAR_ClassificationEnable, VAR_OUT},
      {"NewClassificationOrder", VAR_ClassificationOrder, VAR_OUT},
      {"NewClassInterface", VAR_ClassInterface, VAR_OUT},      
      {"NewDestIP", VAR_DestIP, VAR_OUT},      
      {"NewDestMask", VAR_DestMask, VAR_OUT},      
      {"NewDestIPExclude", VAR_DestIPExclude, VAR_OUT},      
      {"NewSourceIP", VAR_SourceIP, VAR_OUT},      
      {"NewSourceMask", VAR_SourceMask, VAR_OUT},      
      {"NewSourceIPExclude", VAR_SourceIPExclude, VAR_OUT},      
      {"NewProtocol", VAR_Protocol, VAR_OUT},      
      {"NewProtocolExclude", VAR_ProtocolExclude, VAR_OUT},      
      {"NewDestPort", VAR_DestPort, VAR_OUT},      
      {"NewDestPortRangeMax", VAR_DestPortRangeMax, VAR_OUT},      
      {"NewDestPortExclude", VAR_DestPortExclude, VAR_OUT},      
      {"NewSourcePort", VAR_SourcePort, VAR_OUT},      
      {"NewSourcePortRangeMax", VAR_SourcePortRangeMax, VAR_OUT},      
      {"NewSourcePortExclude", VAR_SourcePortExclude, VAR_OUT},      
      {"NewSourceMACAddress", VAR_SourceMACAddress, VAR_OUT},      
      {"NewSourceMACMask", VAR_SourceMACMask, VAR_OUT},      
      {"NewSourceMACExclude", VAR_SourceMACExclude, VAR_OUT},            
      {"NewDestMACAddress", VAR_DestMACAddress, VAR_OUT},      
      {"NewDestMACMask", VAR_DestMACMask, VAR_OUT},      
      {"NewDestMACExclude", VAR_DestMACExclude, VAR_OUT},            
      {"NewEthertype", VAR_Ethertype, VAR_OUT},            
      {"NewEthertypeExclude", VAR_EthertypeExclude, VAR_OUT},            
#if 0           
      {"NewSSAP", VAR_SSAP, VAR_OUT},            
      {"NewSSAPExclude", VAR_SSAPExclude, VAR_OUT},            
      {"NewDSAP", VAR_DSAP, VAR_OUT},                  
      {"NewDSAPExclude", VAR_DSAPExclude, VAR_OUT},                  
      {"NewLLCControl", VAR_LLCControl, VAR_OUT},
      {"NewLLCControlExclude", VAR_LLCControlExclude, VAR_OUT},                  
      {"NewSNAPOUI", VAR_SNAPOUI, VAR_OUT},                  
      {"NewSNAPOUIExclude", VAR_SNAPOUIExclude, VAR_OUT},                  
#endif
      {"NewSourceVendorClassID", VAR_SourceVendorClassID, VAR_OUT},                  
      {"NewSourceVendorClassIDExclude", VAR_SourceVendorClassIDExclude, VAR_OUT},                  
#if 0
      {"NewDestVendorClassID", VAR_DestVendorClassID, VAR_OUT},                  
      {"NewDestVendorClassIDExclude", VAR_DestVendorClassIDExclude, VAR_OUT},                  
      {"NewSourceClientID", VAR_SourceClientID, VAR_OUT},
      {"NewSourceClientIDExclude", VAR_SourceClientIDExclude, VAR_OUT},
      {"NewDestClientID", VAR_DestClientID, VAR_OUT},
      {"NewDestClientIDExclude", VAR_DestClientIDExclude, VAR_OUT},
#endif
      {"NewSourceUserClassID", VAR_SourceUserClassID, VAR_OUT},
      {"NewSourceUserClassIDExclude", VAR_SourceUserClassIDExclude, VAR_OUT},
#if 0
      {"NewDestUserClassID", VAR_DestUserClassID, VAR_OUT},
      {"NewDestUserClassIDExclude", VAR_DestUserClassIDExclude, VAR_OUT},
      {"NewTCPACK", VAR_TCPACK, VAR_OUT},
      {"NewTCPACKExclude", VAR_TCPACKExclude, VAR_OUT},
      {"NewIPLengthMin", VAR_IPLengthMin, VAR_OUT},
      {"NewIPLengthMax", VAR_IPLengthMax, VAR_OUT},      
      {"NewIPLengthExclude", VAR_IPLengthExclude, VAR_OUT},      
#endif
      {"NewDSCPCheck", VAR_DSCPCheck, VAR_OUT},      
      {"NewDSCPExclude", VAR_DSCPExclude, VAR_OUT},      
      {"NewDSCPMark", VAR_DSCPMark, VAR_OUT},      
      {"NewEthernetPriorityCheck", VAR_EthernetPriorityCheck, VAR_OUT},
      {"NewEthernetPriorityExclude", VAR_EthernetPriorityExclude, VAR_OUT},
      {"NewEthernetPriorityMark", VAR_EthernetPriorityMark, VAR_OUT},
      {"NewVLANIDCheck", VAR_VLANIDCheck, VAR_OUT},
      {"NewVLANIDExclude", VAR_VLANIDExclude, VAR_OUT},
      {"NewForwardingPolicy", VAR_ForwardingPolicy, VAR_OUT},
      {"NewClassPolicer", VAR_ClassPolicer, VAR_OUT},
      {"NewClassQueue", VAR_ClassQueue, VAR_OUT},
      {"NewClassApp", VAR_ClassApp, VAR_OUT},
      { 0 }
   }
};


static Action _SetClassificationEntryEnable = { 
   "SetClassificationEntryEnable", SetClassificationEntryEnable,
   (Param []) {
      {"NewClassificationKey", VAR_ClassificationKey, VAR_IN},
      {"NewClassificationEnable", VAR_ClassificationEnable, VAR_IN},
      { 0 }
   }
};


static Action _SetClassificationEntryOrder = { 
   "SetClassificationEntryOrder", SetClassificationEntryOrder,
   (Param []) {
      {"NewClassificationKey", VAR_ClassificationKey, VAR_IN},
      {"NewClassificationOrder", VAR_ClassificationOrder, VAR_IN},
      { 0 }
   }
};

static Action _AddQueueEntry = { 
   "AddQueueEntry", AddQueueEntry,
   (Param []) {
      {"NewQueueEnable", VAR_QueueEnable, VAR_IN},
      {"NewQueueInterface", VAR_QueueInterface, VAR_IN},
#if 0
      {"NewQueueWeight", VAR_QueueWeight, VAR_IN},
#endif
      {"NewQueuePrecedence", VAR_QueuePrecedence, VAR_IN},
#if 0
      {"NewREDThreshold", VAR_REDThreshold, VAR_IN},
      {"NewREDPercentage", VAR_REDPercentage, VAR_IN},
      {"NewDropAlgorithm", VAR_DropAlgorithm, VAR_IN},
      {"NewSchedulerAlgorithm", VAR_SchedulerAlgorithm, VAR_IN},
      {"NewShapingRate", VAR_ShapingRate, VAR_IN},
      {"NewShapingBurstSize", VAR_ShapingBurstSize, VAR_IN},
#endif
      {"NewQueueKey", VAR_QueueKey, VAR_IN},
      { 0 }
   }
};

static Action _DeleteQueueEntry = { 
   "DeleteQueueEntry", DeleteQueueEntry,
   (Param []) {
      {"NewQueueKey", VAR_QueueKey, VAR_IN},
      { 0 }
   }
};

static Action _GetSpecificQueueEntry = { 
   "GetSpecificQueueEntry", GetSpecificQueueEntry,
   (Param []) {
      {"NewQueueKey", VAR_QueueKey, VAR_IN},
      {"NewQueueEnable", VAR_QueueEnable, VAR_OUT},
      {"NewQueueStatus", VAR_QueueStatus, VAR_OUT},
      {"NewQueueInterface", VAR_QueueInterface, VAR_OUT},
#if 0
      {"NewQueueBufferLength", VAR_QueueBufferLength, VAR_OUT},
      {"NewQueueWeight", VAR_QueueWeight, VAR_OUT},
#endif
      {"NewQueuePrecedence", VAR_QueuePrecedence, VAR_OUT},
#if 0
      {"NewREDThreshold", VAR_REDThreshold, VAR_OUT},
      {"NewREDPercentage", VAR_REDPercentage, VAR_OUT},
      {"NewDropAlgorithm", VAR_DropAlgorithm, VAR_OUT},      
      {"NewSchedulerAlgorithm", VAR_SchedulerAlgorithm, VAR_OUT},
      {"NewShapingRate", VAR_ShapingRate, VAR_OUT},
      {"NewShapingBurstSize", VAR_ShapingBurstSize, VAR_OUT},      
#endif
      { 0 }
   }
};

static Action _GetGenericQueueEntry = { 
   "GetGenericQueueEntry", GetGenericQueueEntry,
   (Param []) {
      {"NewQueueIndex", VAR_QueueNumberOfEntries, VAR_IN}, 
      {"NewQueueKey", VAR_QueueKey, VAR_OUT},
      {"NewQueueEnable", VAR_QueueEnable, VAR_OUT},
      {"NewQueueStatus", VAR_QueueStatus, VAR_OUT},
      {"NewQueueInterface", VAR_QueueInterface, VAR_OUT},
#if 0
      {"NewQueueBufferLength", VAR_QueueBufferLength, VAR_OUT},
      {"NewQueueWeight", VAR_QueueWeight, VAR_OUT},
#endif
      {"NewQueuePrecedence", VAR_QueuePrecedence, VAR_OUT},
#if 0
      {"NewREDThreshold", VAR_REDThreshold, VAR_OUT},
      {"NewREDPercentage", VAR_REDPercentage, VAR_OUT},
      {"NewDropAlgorithm", VAR_DropAlgorithm, VAR_OUT},      
      {"NewSchedulerAlgorithm", VAR_SchedulerAlgorithm, VAR_OUT},
      {"NewShapingRate", VAR_ShapingRate, VAR_OUT},
      {"NewShapingBurstSize", VAR_ShapingBurstSize, VAR_OUT},      
#endif
      { 0 }
   }
};

static Action _SetQueueEntryEnable = { 
   "SetQueueEntryEnable", SetQueueEntryEnable,
   (Param []) {
      {"NewQueueKey", VAR_QueueKey, VAR_IN},
      {"NewQueueEnable", VAR_QueueEnable, VAR_IN},
      { 0 }
   }
};

static PAction Actions[] = {
    &_GetInfo,
    &_SetEnable,
    &_SetDefaultBehavior,
    &_AddClassificationEntry,    
    &_DeleteClassificationEntry,
    &_GetSpecificClassificationEntry,
    &_GetGenericClassificationEntry,
    &_SetClassificationEntryEnable,
    &_SetClassificationEntryOrder,
    &_AddQueueEntry,
    &_DeleteQueueEntry,
    &_GetSpecificQueueEntry,
    &_GetGenericQueueEntry,
    &_SetQueueEntryEnable,
    NULL
};

ServiceTemplate Template_QueueManagement = {
    "QueueManagement:1",
    NULL, 
    NULL,
    NULL,   /* SVCXML */
    ARRAYSIZE(StateVariables)-1, 
    StateVariables,
    Actions,
    0, 
    NULL,
    NULL
};
