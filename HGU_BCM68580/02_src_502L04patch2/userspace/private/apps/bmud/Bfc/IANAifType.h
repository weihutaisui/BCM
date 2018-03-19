//****************************************************************************
//
// Copyright (c) 2002-2013 Broadcom Corporation
//
// This program is the proprietary software of Broadcom Corporation and/or
// its licensors, and may only be used, duplicated, modified or distributed
// pursuant to the terms and conditions of a separate, written license
// agreement executed between you and Broadcom (an "Authorized License").
// Except as set forth in an Authorized License, Broadcom grants no license
// (express or implied), right to use, or waiver of any kind with respect to
// the Software, and Broadcom expressly reserves all rights in and to the
// Software and all intellectual property rights therein.  IF YOU HAVE NO
// AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY,
// AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE
// SOFTWARE.  
//
// Except as expressly set forth in the Authorized License,
//
// 1.     This program, including its structure, sequence and organization,
// constitutes the valuable trade secrets of Broadcom, and you shall use all
// reasonable efforts to protect the confidentiality thereof, and to use this
// information only in connection with your use of Broadcom integrated circuit
// products.
//
// 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
// "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS
// OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
// RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL
// IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR
// A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET
// ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME
// THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
//
// 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM
// OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
// INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY
// RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM
// HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN
// EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
// WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY
// FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
//
//****************************************************************************
//    Description:                                                          
//                                                                          
//****************************************************************************
//    Revision History:                                                     
//                                                                          
//****************************************************************************
//    Filename: IANAifType.h
//    Creation Date: Wednesday, December 04, 2002 at 15:27:23
//    Created by Broadcom V2 Mib Compiler BCMIBC.EXE version 1.4
//    from input file IANAifType.bcmibc.mib
//
//****************************************************************************

#ifndef IANAIFTYPE_H
#define IANAIFTYPE_H


#define CONSTVAL_IANAIFTYPE_OTHER 1                               // none of the following                                                                     
#define CONSTVAL_IANAIFTYPE_REGULAR1822 2                                                                                                                      
#define CONSTVAL_IANAIFTYPE_HDH1822 3                                                                                                                          
#define CONSTVAL_IANAIFTYPE_DDNX25 4                                                                                                                           
#define CONSTVAL_IANAIFTYPE_RFC877X25 5                                                                                                                        
#define CONSTVAL_IANAIFTYPE_ETHERNETCSMACD 6                      // for all ethernet-like interfaces                                                          
#define CONSTVAL_IANAIFTYPE_                                      // regardless of speed as per RFC3635                                                        
#define CONSTVAL_IANAIFTYPE_ISO88023CSMACD 7                      // Deprecated via RFC3635                                                                    
#define CONSTVAL_IANAIFTYPE_                                      // ethernetCsmacd  6 should be used instead                                                  
#define CONSTVAL_IANAIFTYPE_ISO88024TOKENBUS 8                                                                                                                 
#define CONSTVAL_IANAIFTYPE_ISO88025TOKENRING 9                                                                                                                
#define CONSTVAL_IANAIFTYPE_ISO88026MAN 10                                                                                                                     
#define CONSTVAL_IANAIFTYPE_STARLAN 11                            // Deprecated via RFC3635                                                                    
#define CONSTVAL_IANAIFTYPE_                                      // ethernetCsmacd  6 should be used instead                                                  
#define CONSTVAL_IANAIFTYPE_PROTEON10MBIT 12                                                                                                                   
#define CONSTVAL_IANAIFTYPE_PROTEON80MBIT 13                                                                                                                   
#define CONSTVAL_IANAIFTYPE_HYPERCHANNEL 14                                                                                                                    
#define CONSTVAL_IANAIFTYPE_FDDI 15                                                                                                                            
#define CONSTVAL_IANAIFTYPE_LAPB 16                                                                                                                            
#define CONSTVAL_IANAIFTYPE_SDLC 17                                                                                                                            
#define CONSTVAL_IANAIFTYPE_DS1 18                                // DS1-MIB                                                                                   
#define CONSTVAL_IANAIFTYPE_E1 19                                 // Obsolete see DS1-MIB                                                                      
#define CONSTVAL_IANAIFTYPE_BASICISDN 20                          // no longer used                                                                            
#define CONSTVAL_IANAIFTYPE_                                      // see also RFC2127                                                                          
#define CONSTVAL_IANAIFTYPE_PRIMARYISDN 21                        // no longer used                                                                            
#define CONSTVAL_IANAIFTYPE_                                      // see also RFC2127                                                                          
#define CONSTVAL_IANAIFTYPE_PROPPOINTTOPOINTSERIAL 22             // proprietary serial                                                                        
#define CONSTVAL_IANAIFTYPE_PPP 23                                                                                                                             
#define CONSTVAL_IANAIFTYPE_SOFTWARELOOPBACK 24                                                                                                                
#define CONSTVAL_IANAIFTYPE_EON 25                                // CLNP over IP                                                                              
#define CONSTVAL_IANAIFTYPE_ETHERNET3MBIT 26                                                                                                                   
#define CONSTVAL_IANAIFTYPE_NSIP 27                               // XNS over IP                                                                               
#define CONSTVAL_IANAIFTYPE_SLIP 28                               // generic SLIP                                                                              
#define CONSTVAL_IANAIFTYPE_ULTRA 29                              // ULTRA technologies                                                                        
#define CONSTVAL_IANAIFTYPE_DS3 30                                // DS3-MIB                                                                                   
#define CONSTVAL_IANAIFTYPE_SIP 31                                // SMDS coffee                                                                               
#define CONSTVAL_IANAIFTYPE_FRAMERELAY 32                         // DTE only.                                                                                 
#define CONSTVAL_IANAIFTYPE_RS232 33                                                                                                                           
#define CONSTVAL_IANAIFTYPE_PARA 34                               // parallel-port                                                                             
#define CONSTVAL_IANAIFTYPE_ARCNET 35                             // arcnet                                                                                    
#define CONSTVAL_IANAIFTYPE_ARCNETPLUS 36                         // arcnet plus                                                                               
#define CONSTVAL_IANAIFTYPE_ATM 37                                // ATM cells                                                                                 
#define CONSTVAL_IANAIFTYPE_MIOX25 38                                                                                                                          
#define CONSTVAL_IANAIFTYPE_SONET 39                              // SONET or SDH                                                                              
#define CONSTVAL_IANAIFTYPE_X25PLE 40                                                                                                                          
#define CONSTVAL_IANAIFTYPE_ISO88022LLC 41                                                                                                                     
#define CONSTVAL_IANAIFTYPE_LOCALTALK 42                                                                                                                       
#define CONSTVAL_IANAIFTYPE_SMDSDXI 43                                                                                                                         
#define CONSTVAL_IANAIFTYPE_FRAMERELAYSERVICE 44                  // FRNETSERV-MIB                                                                             
#define CONSTVAL_IANAIFTYPE_V35 45                                                                                                                             
#define CONSTVAL_IANAIFTYPE_HSSI 46                                                                                                                            
#define CONSTVAL_IANAIFTYPE_HIPPI 47                                                                                                                           
#define CONSTVAL_IANAIFTYPE_MODEM 48                              // Generic modem                                                                             
#define CONSTVAL_IANAIFTYPE_AAL5 49                               // AAL5 over ATM                                                                             
#define CONSTVAL_IANAIFTYPE_SONETPATH 50                                                                                                                       
#define CONSTVAL_IANAIFTYPE_SONETVT 51                                                                                                                         
#define CONSTVAL_IANAIFTYPE_SMDSICIP 52                           // SMDS InterCarrier Interface                                                               
#define CONSTVAL_IANAIFTYPE_PROPVIRTUAL 53                        // proprietary virtual/internal                                                              
#define CONSTVAL_IANAIFTYPE_PROPMULTIPLEXOR 54                    // proprietary multiplexing                                                                  
#define CONSTVAL_IANAIFTYPE_IEEE80212 55                          // 100BaseVG                                                                                 
#define CONSTVAL_IANAIFTYPE_FIBRECHANNEL 56                       // Fibre Channel                                                                             
#define CONSTVAL_IANAIFTYPE_HIPPIINTERFACE 57                     // HIPPI interfaces                                                                          
#define CONSTVAL_IANAIFTYPE_FRAMERELAYINTERCONNECT 58             // Obsolete use either                                                                       
#define CONSTVAL_IANAIFTYPE_                                      // frameRelay 32 or                                                                          
#define CONSTVAL_IANAIFTYPE_                                      // frameRelayService 44.                                                                     
#define CONSTVAL_IANAIFTYPE_AFLANE8023 59                         // ATM Emulated LAN for 802.3                                                                
#define CONSTVAL_IANAIFTYPE_AFLANE8025 60                         // ATM Emulated LAN for 802.5                                                                
#define CONSTVAL_IANAIFTYPE_CCTEMUL 61                            // ATM Emulated circuit                                                                      
#define CONSTVAL_IANAIFTYPE_FASTETHER 62                          // Obsoleted via RFC3635                                                                     
#define CONSTVAL_IANAIFTYPE_                                      // ethernetCsmacd  6 should be used instead                                                  
#define CONSTVAL_IANAIFTYPE_ISDN 63                               // ISDN and X.25                                                                             
#define CONSTVAL_IANAIFTYPE_V11 64                                // CCITT V.11/X.21                                                                           
#define CONSTVAL_IANAIFTYPE_V36 65                                // CCITT V.36                                                                                
#define CONSTVAL_IANAIFTYPE_G703AT64K 66                          // CCITT G703 at 64Kbps                                                                      
#define CONSTVAL_IANAIFTYPE_G703AT2MB 67                          // Obsolete see DS1-MIB                                                                      
#define CONSTVAL_IANAIFTYPE_QLLC 68                               // SNA QLLC                                                                                  
#define CONSTVAL_IANAIFTYPE_FASTETHERFX 69                        // Obsoleted via RFC3635                                                                     
#define CONSTVAL_IANAIFTYPE_                                      // ethernetCsmacd  6 should be used instead                                                  
#define CONSTVAL_IANAIFTYPE_CHANNEL 70                            // channel                                                                                   
#define CONSTVAL_IANAIFTYPE_IEEE80211 71                          // radio spread spectrum                                                                     
#define CONSTVAL_IANAIFTYPE_IBM370PARCHAN 72                      // IBM System 360/370 OEMI Channel                                                           
#define CONSTVAL_IANAIFTYPE_ESCON 73                              // IBM Enterprise Systems Connection                                                         
#define CONSTVAL_IANAIFTYPE_DLSW 74                               // Data Link Switching                                                                       
#define CONSTVAL_IANAIFTYPE_ISDNS 75                              // ISDN S/T interface                                                                        
#define CONSTVAL_IANAIFTYPE_ISDNU 76                              // ISDN U interface                                                                          
#define CONSTVAL_IANAIFTYPE_LAPD 77                               // Link Access Protocol D                                                                    
#define CONSTVAL_IANAIFTYPE_IPSWITCH 78                           // IP Switching Objects                                                                      
#define CONSTVAL_IANAIFTYPE_RSRB 79                               // Remote Source Route Bridging                                                              
#define CONSTVAL_IANAIFTYPE_ATMLOGICAL 80                         // ATM Logical Port                                                                          
#define CONSTVAL_IANAIFTYPE_DS0 81                                // Digital Signal Level 0                                                                    
#define CONSTVAL_IANAIFTYPE_DS0BUNDLE 82                          // group of ds0s on the same ds1                                                             
#define CONSTVAL_IANAIFTYPE_BSC 83                                // Bisynchronous Protocol                                                                    
#define CONSTVAL_IANAIFTYPE_ASYNC 84                              // Asynchronous Protocol                                                                     
#define CONSTVAL_IANAIFTYPE_CNR 85                                // Combat Net Radio                                                                          
#define CONSTVAL_IANAIFTYPE_ISO88025DTR 86                        // ISO 802.5r DTR                                                                            
#define CONSTVAL_IANAIFTYPE_EPLRS 87                              // Ext Pos Loc Report Sys                                                                    
#define CONSTVAL_IANAIFTYPE_ARAP 88                               // Appletalk Remote Access Protocol                                                          
#define CONSTVAL_IANAIFTYPE_PROPCNLS 89                           // Proprietary Connectionless Protocol                                                       
#define CONSTVAL_IANAIFTYPE_HOSTPAD 90                            // CCITT-ITU X.29 PAD Protocol                                                               
#define CONSTVAL_IANAIFTYPE_TERMPAD 91                            // CCITT-ITU X.3 PAD Facility                                                                
#define CONSTVAL_IANAIFTYPE_FRAMERELAYMPI 92                      // Multiproto Interconnect over FR                                                           
#define CONSTVAL_IANAIFTYPE_X213 93                               // CCITT-ITU X213                                                                            
#define CONSTVAL_IANAIFTYPE_ADSL 94                               // Asymmetric Digital Subscriber Loop                                                        
#define CONSTVAL_IANAIFTYPE_RADSL 95                              // Rate-Adapt. Digital Subscriber Loop                                                       
#define CONSTVAL_IANAIFTYPE_SDSL 96                               // Symmetric Digital Subscriber Loop                                                         
#define CONSTVAL_IANAIFTYPE_VDSL 97                               // Very H-Speed Digital Subscrib. Loop                                                       
#define CONSTVAL_IANAIFTYPE_ISO88025CRFPINT 98                    // ISO 802.5 CRFP                                                                            
#define CONSTVAL_IANAIFTYPE_MYRINET 99                            // Myricom Myrinet                                                                           
#define CONSTVAL_IANAIFTYPE_VOICEEM 100                           // voice recEive and transMit                                                                
#define CONSTVAL_IANAIFTYPE_VOICEFXO 101                          // voice Foreign Exchange Office                                                             
#define CONSTVAL_IANAIFTYPE_VOICEFXS 102                          // voice Foreign Exchange Station                                                            
#define CONSTVAL_IANAIFTYPE_VOICEENCAP 103                        // voice encapsulation                                                                       
#define CONSTVAL_IANAIFTYPE_VOICEOVERIP 104                       // voice over IP encapsulation                                                               
#define CONSTVAL_IANAIFTYPE_ATMDXI 105                            // ATM DXI                                                                                   
#define CONSTVAL_IANAIFTYPE_ATMFUNI 106                           // ATM FUNI                                                                                  
#define CONSTVAL_IANAIFTYPE_ATMIMA  107                           // ATM IMA                                                                                   
#define CONSTVAL_IANAIFTYPE_PPPMULTILINKBUNDLE 108                // PPP Multilink Bundle                                                                      
#define CONSTVAL_IANAIFTYPE_IPOVERCDLC  109                       // IBM ipOverCdlc                                                                            
#define CONSTVAL_IANAIFTYPE_IPOVERCLAW  110                       // IBM Common Link Access to Workstn                                                         
#define CONSTVAL_IANAIFTYPE_STACKTOSTACK  111                     // IBM stackToStack                                                                          
#define CONSTVAL_IANAIFTYPE_VIRTUALIPADDRESS  112                 // IBM VIPA                                                                                  
#define CONSTVAL_IANAIFTYPE_MPC  113                              // IBM multi-protocol channel support                                                        
#define CONSTVAL_IANAIFTYPE_IPOVERATM  114                        // IBM ipOverAtm                                                                             
#define CONSTVAL_IANAIFTYPE_ISO88025FIBER  115                    // ISO 802.5j Fiber Token Ring                                                               
#define CONSTVAL_IANAIFTYPE_TDLC  116	                          // IBM twinaxial data link control                                                           
#define CONSTVAL_IANAIFTYPE_GIGABITETHERNET  117                  // Obsoleted via RFC3635                                                                     
#define CONSTVAL_IANAIFTYPE_                                      // ethernetCsmacd  6 should be used instead                                                  
#define CONSTVAL_IANAIFTYPE_HDLC  118                             // HDLC                                                                                      
#define CONSTVAL_IANAIFTYPE_LAPF  119	                          // LAP F                                                                                     
#define CONSTVAL_IANAIFTYPE_V37  120	                          // V.37                                                                                      
#define CONSTVAL_IANAIFTYPE_X25MLP  121                           // Multi-Link Protocol                                                                       
#define CONSTVAL_IANAIFTYPE_X25HUNTGROUP  122                     // X25 Hunt Group                                                                            
#define CONSTVAL_IANAIFTYPE_TRANSPHDLC  123                       // Transp HDLC                                                                               
#define CONSTVAL_IANAIFTYPE_INTERLEAVE  124                       // Interleave channel                                                                        
#define CONSTVAL_IANAIFTYPE_FAST  125                             // Fast channel                                                                              
#define CONSTVAL_IANAIFTYPE_IP  126	                              // IP  for APPN HPR in IP networks                                                           
#define CONSTVAL_IANAIFTYPE_DOCSCABLEMACLAYER  127                // CATV Mac Layer                                                                            
#define CONSTVAL_IANAIFTYPE_DOCSCABLEDOWNSTREAM  128              // CATV Downstream interface                                                                 
#define CONSTVAL_IANAIFTYPE_DOCSCABLEUPSTREAM  129                // CATV Upstream interface                                                                   
#define CONSTVAL_IANAIFTYPE_A12MPPSWITCH  130                     // Avalon Parallel Processor                                                                 
#define CONSTVAL_IANAIFTYPE_TUNNEL  131                           // Encapsulation interface                                                                   
#define CONSTVAL_IANAIFTYPE_COFFEE  132                           // coffee pot                                                                                
#define CONSTVAL_IANAIFTYPE_CES  133                              // Circuit Emulation Service                                                                 
#define CONSTVAL_IANAIFTYPE_ATMSUBINTERFACE  134                  // ATM Sub Interface                                                                         
#define CONSTVAL_IANAIFTYPE_L2VLAN  135                           // Layer 2 Virtual LAN using 802.1Q                                                          
#define CONSTVAL_IANAIFTYPE_L3IPVLAN  136                         // Layer 3 Virtual LAN using IP                                                              
#define CONSTVAL_IANAIFTYPE_L3IPXVLAN  137                        // Layer 3 Virtual LAN using IPX                                                             
#define CONSTVAL_IANAIFTYPE_DIGITALPOWERLINE  138                 // IP over Power Lines                                                                       
#define CONSTVAL_IANAIFTYPE_MEDIAMAILOVERIP  139                  // Multimedia Mail over IP                                                                   
#define CONSTVAL_IANAIFTYPE_DTM  140                              // Dynamic syncronous Transfer Mode                                                          
#define CONSTVAL_IANAIFTYPE_DCN  141                              // Data Communications Network                                                               
#define CONSTVAL_IANAIFTYPE_IPFORWARD  142                        // IP Forwarding Interface                                                                   
#define CONSTVAL_IANAIFTYPE_MSDSL  143                            // Multi-rate Symmetric DSL                                                                  
#define CONSTVAL_IANAIFTYPE_IEEE1394  144                         // IEEE1394 High Performance Serial Bus                                                      
#define CONSTVAL_IANAIFTYPE_IF_GSN  145                           // HIPPI-6400                                                                              
#define CONSTVAL_IANAIFTYPE_DVBRCCMACLAYER  146                   // DVB-RCC MAC Layer                                                                         
#define CONSTVAL_IANAIFTYPE_DVBRCCDOWNSTREAM  147                 // DVB-RCC Downstream Channel                                                                
#define CONSTVAL_IANAIFTYPE_DVBRCCUPSTREAM  148                   // DVB-RCC Upstream Channel                                                                  
#define CONSTVAL_IANAIFTYPE_ATMVIRTUAL  149                       // ATM Virtual Interface                                                                     
#define CONSTVAL_IANAIFTYPE_MPLSTUNNEL  150                       // MPLS Tunnel Virtual Interface                                                             
#define CONSTVAL_IANAIFTYPE_SRP  151	                          // Spatial Reuse Protocol                                                                    
#define CONSTVAL_IANAIFTYPE_VOICEOVERATM  152                     // Voice Over ATM                                                                            
#define CONSTVAL_IANAIFTYPE_VOICEOVERFRAMERELAY  153              // Voice Over Frame Relay                                                                    
#define CONSTVAL_IANAIFTYPE_IDSL  154		                      // Digital Subscriber Loop over ISDN                                                         
#define CONSTVAL_IANAIFTYPE_COMPOSITELINK  155                    // Avici Composite Link Interface                                                            
#define CONSTVAL_IANAIFTYPE_SS7SIGLINK  156                       // SS7 Signaling Link                                                                        
#define CONSTVAL_IANAIFTYPE_PROPWIRELESSP2P  157                  //  Prop. P2P wireless interface                                                             
#define CONSTVAL_IANAIFTYPE_FRFORWARD  158                        // Frame Forward Interface                                                                   
#define CONSTVAL_IANAIFTYPE_RFC1483  159	                      // Multiprotocol over ATM AAL5                                                               
#define CONSTVAL_IANAIFTYPE_USB  160		                      // USB Interface                                                                             
#define CONSTVAL_IANAIFTYPE_IEEE8023ADLAG  161                    // IEEE 802.3ad Link Aggregate                                                               
#define CONSTVAL_IANAIFTYPE_BGPPOLICYACCOUNTING  162              // BGP Policy Accounting                                                                     
#define CONSTVAL_IANAIFTYPE_FRF16MFRBUNDLE  163                   // FRF .16 Multilink Frame Relay                                                             
#define CONSTVAL_IANAIFTYPE_H323GATEKEEPER  164                   // H323 Gatekeeper                                                                           
#define CONSTVAL_IANAIFTYPE_H323PROXY  165                        // H323 Voice and Video Proxy                                                                
#define CONSTVAL_IANAIFTYPE_MPLS  166                             // MPLS                                                                                      
#define CONSTVAL_IANAIFTYPE_MFSIGLINK  167                        // Multi-frequency signaling link                                                            
#define CONSTVAL_IANAIFTYPE_HDSL2  168                            // High Bit-Rate DSL - 2nd generation                                                        
#define CONSTVAL_IANAIFTYPE_SHDSL  169                            // Multirate HDSL2                                                                           
#define CONSTVAL_IANAIFTYPE_DS1FDL  170                           // Facility Data Link 4Kbps on a DS1                                                         
#define CONSTVAL_IANAIFTYPE_POS  171                              // Packet over SONET/SDH Interface                                                           
#define CONSTVAL_IANAIFTYPE_DVBASIIN  172                         // DVB-ASI Input                                                                             
#define CONSTVAL_IANAIFTYPE_DVBASIOUT  173                        // DVB-ASI Output                                                                            
#define CONSTVAL_IANAIFTYPE_PLC  174                              // Power Line Communtications                                                                
#define CONSTVAL_IANAIFTYPE_NFAS  175                             // Non Facility Associated Signaling                                                         
#define CONSTVAL_IANAIFTYPE_TR008  176                            // TR008                                                                                     
#define CONSTVAL_IANAIFTYPE_GR303RDT  177                         // Remote Digital Terminal                                                                   
#define CONSTVAL_IANAIFTYPE_GR303IDT  178                         // Integrated Digital Terminal                                                               
#define CONSTVAL_IANAIFTYPE_ISUP  179                             // ISUP                                                                                      
#define CONSTVAL_IANAIFTYPE_PROPDOCSWIRELESSMACLAYER  180         // Cisco proprietary Maclayer                                                                
#define CONSTVAL_IANAIFTYPE_PROPDOCSWIRELESSDOWNSTREAM  181       // Cisco proprietary Downstream                                                              
#define CONSTVAL_IANAIFTYPE_PROPDOCSWIRELESSUPSTREAM  182         // Cisco proprietary Upstream                                                                
#define CONSTVAL_IANAIFTYPE_HIPERLAN2  183                        // HIPERLAN Type 2 Radio Interface                                                           
#define CONSTVAL_IANAIFTYPE_PROPBWAP2MP  184                      // PropBroadbandWirelessAccesspt2multipt                                                     
#define CONSTVAL_IANAIFTYPE_                                      // use of this iftype for IEEE 802.16 WMAN                                                   
#define CONSTVAL_IANAIFTYPE_                                      // interfaces as per IEEE Std 802.16f is                                                     
#define CONSTVAL_IANAIFTYPE_                                      // deprecated and ifType 237 should be used instead.                                         
#define CONSTVAL_IANAIFTYPE_SONETOVERHEADCHANNEL  185             // SONET Overhead Channel                                                                    
#define CONSTVAL_IANAIFTYPE_DIGITALWRAPPEROVERHEADCHANNEL  186    // Digital Wrapper                                                                           
#define CONSTVAL_IANAIFTYPE_AAL2  187                             // ATM adaptation layer 2                                                                    
#define CONSTVAL_IANAIFTYPE_RADIOMAC  188                         // MAC layer over radio links                                                                
#define CONSTVAL_IANAIFTYPE_ATMRADIO  189                         // ATM over radio links                                                                      
#define CONSTVAL_IANAIFTYPE_IMT  190                              // Inter Machine Trunks                                                                      
#define CONSTVAL_IANAIFTYPE_MVL  191                              // Multiple Virtual Lines DSL                                                                
#define CONSTVAL_IANAIFTYPE_REACHDSL  192                         // Long Reach DSL                                                                            
#define CONSTVAL_IANAIFTYPE_FRDLCIENDPT  193                      // Frame Relay DLCI End Point                                                                
#define CONSTVAL_IANAIFTYPE_ATMVCIENDPT  194                      // ATM VCI End Point                                                                         
#define CONSTVAL_IANAIFTYPE_OPTICALCHANNEL  195                   // Optical Channel                                                                           
#define CONSTVAL_IANAIFTYPE_OPTICALTRANSPORT  196                 // Optical Transport                                                                         
#define CONSTVAL_IANAIFTYPE_PROPATM  197                          //  Proprietary ATM                                                                          
#define CONSTVAL_IANAIFTYPE_VOICEOVERCABLE  198                   // Voice Over Cable Interface                                                                
#define CONSTVAL_IANAIFTYPE_INFINIBAND  199                       // Infiniband                                                                                
#define CONSTVAL_IANAIFTYPE_TELINK  200                           // TE Link                                                                                   
#define CONSTVAL_IANAIFTYPE_Q2931  201                            // Q.2931                                                                                    
#define CONSTVAL_IANAIFTYPE_VIRTUALTG  202                        // Virtual Trunk Group                                                                       
#define CONSTVAL_IANAIFTYPE_SIPTG  203                            // SIP Trunk Group                                                                           
#define CONSTVAL_IANAIFTYPE_SIPSIG  204                           // SIP Signaling                                                                             
#define CONSTVAL_IANAIFTYPE_DOCSCABLEUPSTREAMCHANNEL  205         // CATV Upstream Channel                                                                     
#define CONSTVAL_IANAIFTYPE_ECONET  206                           // Acorn Econet                                                                              
#define CONSTVAL_IANAIFTYPE_PON155  207                           // FSAN 155Mb Symetrical PON interface                                                       
#define CONSTVAL_IANAIFTYPE_PON622  208                           // FSAN622Mb Symetrical PON interface                                                        
#define CONSTVAL_IANAIFTYPE_BRIDGE  209                           // Transparent bridge interface                                                              
#define CONSTVAL_IANAIFTYPE_LINEGROUP  210                        // Interface common to multiple lines                                                        
#define CONSTVAL_IANAIFTYPE_VOICEEMFGD  211                       // voice E&M Feature Group D                                                                 
#define CONSTVAL_IANAIFTYPE_VOICEFGDEANA  212                     // voice FGD Exchange Access North American                                                  
#define CONSTVAL_IANAIFTYPE_VOICEDID  213                         // voice Direct Inward Dialing                                                               
#define CONSTVAL_IANAIFTYPE_MPEGTRANSPORT  214                    // MPEG transport interface                                                                  
#define CONSTVAL_IANAIFTYPE_SIXTOFOUR  215                        // 6to4 interface  DEPRECATED                                                                
#define CONSTVAL_IANAIFTYPE_GTP  216                              // GTP  GPRS Tunneling Protocol                                                              
#define CONSTVAL_IANAIFTYPE_PDNETHERLOOP1  217                    // Paradyne EtherLoop 1                                                                      
#define CONSTVAL_IANAIFTYPE_PDNETHERLOOP2  218                    // Paradyne EtherLoop 2                                                                      
#define CONSTVAL_IANAIFTYPE_OPTICALCHANNELGROUP  219              // Optical Channel Group                                                                     
#define CONSTVAL_IANAIFTYPE_HOMEPNA  220                          // HomePNA ITU-T G.989                                                                       
#define CONSTVAL_IANAIFTYPE_GFP  221                              // Generic Framing Procedure  GFP                                                            
#define CONSTVAL_IANAIFTYPE_CISCOISLVLAN  222                     // Layer 2 Virtual LAN using Cisco ISL                                                       
#define CONSTVAL_IANAIFTYPE_ACTELISMETALOOP  223                  // Acteleis proprietary MetaLOOP High Speed Link                                             
#define CONSTVAL_IANAIFTYPE_FCIPLINK  224                         // FCIP Link                                                                                 
#define CONSTVAL_IANAIFTYPE_RPR  225                              // Resilient Packet Ring Interface Type                                                      
#define CONSTVAL_IANAIFTYPE_QAM  226                              // RF Qam Interface                                                                          
#define CONSTVAL_IANAIFTYPE_LMP  227                              // Link Management Protocol                                                                  
#define CONSTVAL_IANAIFTYPE_CBLVECTASTAR  228                     // Cambridge Broadband Networks Limited VectaStar                                            
#define CONSTVAL_IANAIFTYPE_DOCSCABLEMCMTSDOWNSTREAM  229         // CATV Modular CMTS Downstream Interface                                                    
#define CONSTVAL_IANAIFTYPE_ADSL2  230                            // Asymmetric Digital Subscriber Loop Version 2                                              
#define CONSTVAL_IANAIFTYPE_                                      //  DEPRECATED/OBSOLETED - please use adsl2plus 238 instead                                  
#define CONSTVAL_IANAIFTYPE_MACSECCONTROLLEDIF  231               // MACSecControlled                                                                          
#define CONSTVAL_IANAIFTYPE_MACSECUNCONTROLLEDIF  232             // MACSecUncontrolled                                                                        
#define CONSTVAL_IANAIFTYPE_AVICIOPTICALETHER  233                // Avici Optical Ethernet Aggregate                                                          
#define CONSTVAL_IANAIFTYPE_ATMBOND  234                          // atmbond                                                                                   
#define CONSTVAL_IANAIFTYPE_VOICEFGDOS  235                       // voice FGD Operator Services                                                               
#define CONSTVAL_IANAIFTYPE_MOCAVERSION1  236                     // MultiMedia over Coax Alliance  MoCA Interface                                             
#define CONSTVAL_IANAIFTYPE_                                      // as documented in information provided privately to IANA                                   
#define CONSTVAL_IANAIFTYPE_IEEE80216WMAN  237                    // IEEE 802.16 WMAN interface                                                                
#define CONSTVAL_IANAIFTYPE_ADSL2PLUS  238                        // Asymmetric Digital Subscriber Loop Version 2                                              
#define CONSTVAL_IANAIFTYPE_                                      // Version 2 Plus and all variants                                                           
#define CONSTVAL_IANAIFTYPE_DVBRCSMACLAYER  239                   // DVB-RCS MAC Layer                                                                         
#define CONSTVAL_IANAIFTYPE_DVBTDM  240                           // DVB Satellite TDM                                                                         
#define CONSTVAL_IANAIFTYPE_DVBRCSTDMA  241                       // DVB-RCS TDMA                                                                              
#define CONSTVAL_IANAIFTYPE_X86LAPS  242                          // LAPS based on ITU-T X.86/Y.1323                                                           
#define CONSTVAL_IANAIFTYPE_WWANPP  243                           // 3GPP WWAN                                                                                 
#define CONSTVAL_IANAIFTYPE_WWANPP2  244                          // 3GPP2 WWAN                                                                                
#define CONSTVAL_IANAIFTYPE_VOICEEBS  245                         // voice P-phone EBS physical interface                                                      
#define CONSTVAL_IANAIFTYPE_IFPWTYPE  246                         // Pseudowire interface type                                                                 
#define CONSTVAL_IANAIFTYPE_ILAN  247                             // Internal LAN on a bridge per IEEE 802.1ap                                                 
#define CONSTVAL_IANAIFTYPE_PIP  248                              // Provider Instance Port on a bridge per IEEE 802.1ah PBB                                   
#define CONSTVAL_IANAIFTYPE_ALUELP  249                           // Alcatel-Lucent Ethernet Link Protection                                                   
#define CONSTVAL_IANAIFTYPE_GPON  250                             // Gigabit-capable passive optical networks  G-PON as per ITU-T G.948                        
#define CONSTVAL_IANAIFTYPE_VDSL2  251                            // Very high speed digital subscriber line Version 2  as per ITU-T Recommendation G.993.2    
#define CONSTVAL_IANAIFTYPE_CAPWAPDOT11PROFILE  252               // WLAN Profile Interface                                                                    
#define CONSTVAL_IANAIFTYPE_CAPWAPDOT11BSS  253                   // WLAN BSS Interface                                                                        
#define CONSTVAL_IANAIFTYPE_CAPWAPWTPVIRTUALRADIO  254            // WTP Virtual Radio Interface                                                               
#define CONSTVAL_IANAIFTYPE_BITS  255                             // bitsport                                                                                  
#define CONSTVAL_IANAIFTYPE_DOCSCABLEUPSTREAMRFPORT  256          // DOCSIS CATV Upstream RF Port                                                              
#define CONSTVAL_IANAIFTYPE_CABLEDOWNSTREAMRFPORT  257            // CATV downstream RF port                                                                   
#define CONSTVAL_IANAIFTYPE_VMWAREVIRTUALNIC  258                 // VMware Virtual Network Interface                                                          
#define CONSTVAL_IANAIFTYPE_IEEE802154  259                       // IEEE 802.15.4 WPAN interface                                                              
#define CONSTVAL_IANAIFTYPE_OTNODU  260                           // OTN Optical Data Unit                                                                     
#define CONSTVAL_IANAIFTYPE_OTNOTU  261                           // OTN Optical channel Transport Unit                                                        
#define CONSTVAL_IANAIFTYPE_IFVFITYPE  262                        // VPLS Forwarding Instance Interface Type                                                   
#define CONSTVAL_IANAIFTYPE_G9981  263                            // G.998.1 bonded interface                                                                  
#define CONSTVAL_IANAIFTYPE_G9982  264                            // G.998.2 bonded interface                                                                  
#define CONSTVAL_IANAIFTYPE_G9983  265                            // G.998.3 bonded interface                                                                  
#define CONSTVAL_IANAIFTYPE_ALUEPON  266                          // Ethernet Passive Optical Networks  E-PON                                                  
#define CONSTVAL_IANAIFTYPE_ALUEPONONU  267                       // EPON Optical Network Unit                                                                 
#define CONSTVAL_IANAIFTYPE_ALUEPONPHYSICALUNI  268               // EPON physical User to Network interface                                                   
#define CONSTVAL_IANAIFTYPE_ALUEPONLOGICALLINK  269               // The emulation of a point-to-point link over the EPON layer                                
#define CONSTVAL_IANAIFTYPE_ALUGPONONU  270                       // GPON Optical Network Unit                                                                 
#define CONSTVAL_IANAIFTYPE_ALUGPONPHYSICALUNI  271               // GPON physical User to Network interface                                                   
#define CONSTVAL_IANAIFTYPE_VMWARENICTEAM  272                    // VMware NIC Team                                                                           

// Types below are unassigned by the IANA, but 
// supported by the BFC code.  If assigning a new type, it is very important
// that it be > CONSTVAL_IANAIFTYPE_BRCMBASE
#define CONSTVAL_IANAIFTYPE_BRCMBASE  1000
#define CONSTVAL_IANAIFTYPE_BRCMIPSTACK  1001
#define CONSTVAL_IANAIFTYPE_BRCMPCI  1002
#define CONSTVAL_IANAIFTYPE_BRCMHPNA  1003
#define CONSTVAL_IANAIFTYPE_BRCMBLUETOOTH  1004
#define CONSTVAL_IANAIFTYPE_BRCMPKTGENERATOR  1005
#define CONSTVAL_IANAIFTYPE_BRCMHOMEPLUGINTERFACE 1006
#define CONSTVAL_IANAIFTYPE_BRCMITC 1007

// ifType >= brcmVirtualInterface means this interface is not
// a hardware interface, but rather a software interface which
// has a special purpose such as aggregating other interfaces
#define CONSTVAL_IANAIFTYPE_BRCMVIRTUALINTERFACE  1100
#define CONSTVAL_IANAIFTYPE_BRCMCABLEHOMEAGGREGATEDLANINTERFACE  1101
#define CONSTVAL_IANAIFTYPE_BRCMCABLEHOMEWANDATAINTERFACE  1102
#define CONSTVAL_IANAIFTYPE_BRCMCABLEHOMEAGGREGATEDWIRELESSLANINTERFACE  1103
#define CONSTVAL_IANAIFTYPE_BRCMOPENCABLEDSGCLIENTINTERFACE 1104
#define CONSTVAL_IANAIFTYPE_BRCMOPENCABLEINTERACTIVEIPINTERFACE 1105
#define CONSTVAL_IANAIFTYPE_BRCMMOCAINTERFACE 1106  // Deprecated now that IANA has assigned type 236
#define CONSTVAL_IANAIFTYPE_BRCMRADIOINTERFACE0 1107
#define CONSTVAL_IANAIFTYPE_BRCMRADIOINTERFACE1 1108
#define CONSTVAL_IANAIFTYPE_BRCMAPRPROXYINTERFACE 1109

#define CONSTVAL_IANAIFTYPE_BRCMOPENCABLESEBINTERACTIVEIPINTERFACE 1110

// ifType >= brcmPacketSwitchBridgingInterface means this
// interface is a packet switch bridging interface.  This threshold is used in
// SW to identify a PacketSwitchBridgingHalIf HalIf.
//
// Thus it is *VERY* important that any new types assigned to a 
// PacketSwitchBridgingHalIf *MUST* have a value > 
// CONSTVAL_IANATYPE_BRCMPACKETSWITCHBRIDGINGINTERFACE, and that *NO*
// types be assigned > CONSTVAL_IANATYPE_BRCMPACKETSWITCHBRIDGINGINTERFACE
// if it is not a PacketSwitchBridgingHalIf.  
#define CONSTVAL_IANATYPE_BRCMPACKETSWITCHBRIDGINGINTERFACE 2000
#define CONSTVAL_IANAIFTYPE_BRCMVIRTUALCABLEHOMEINTERFACE  2001
#define CONSTVAL_IANAIFTYPE_BRCMVIRTUALPACKETCABLEINTERFACE  2002

#endif
