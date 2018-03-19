/*
 * Broadcom WPS module (for libupnp), xml_WFADevice.c
 *
 * Copyright 2017 Broadcom
 *
 * This program is the proprietary software of Broadcom and/or
 * its licensors, and may only be used, duplicated, modified or distributed
 * pursuant to the terms and conditions of a separate, written license
 * agreement executed between you and Broadcom (an "Authorized License").
 * Except as set forth in an Authorized License, Broadcom grants no license
 * (express or implied), right to use, or waiver of any kind with respect to
 * the Software, and Broadcom expressly reserves all rights in and to the
 * Software and all intellectual property rights therein.  IF YOU HAVE NO
 * AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 * WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF
 * THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof, and to
 * use this information only in connection with your use of Broadcom
 * integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR
 * OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL,
 * SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR
 * IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 * IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii)
 * ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
 * OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY
 * NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 *
 * $Id: xml_WFADevice.c 551899 2015-04-24 11:55:46Z $
 */

char xml_WFADevice[] =
	"<?xml version=\"1.0\"?>\r\n"
	"<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\r\n"
	"\t<specVersion>\r\n"
	"\t\t<major>1</major>\r\n"
	"\t\t<minor>0</minor>\r\n"
	"\t</specVersion>\r\n"
	"\t<device>\r\n"
	"\t\t<deviceType>urn:schemas-wifialliance-org:device:WFADevice:1</deviceType>\r\n"
	"\t\t<friendlyName>WFADevice</friendlyName>\r\n"
	"\t\t<manufacturer>Broadcom Corporation</manufacturer>\r\n"
	"\t\t<manufacturerURL>http://www.broadcom.com</manufacturerURL>\r\n"
	"\t\t<modelDescription>Wireless Device</modelDescription>\r\n"
	"\t\t<modelName>WPS</modelName>\r\n"
	"\t\t<modelNumber>X1</modelNumber>\r\n"
	"\t\t<serialNumber>0000001</serialNumber>\r\n"
	"\t\t<UDN>uuid:00010203-0405-0607-0809-0a0b0c0d0ebb</UDN>\r\n"
	"\t\t<serviceList>\r\n"
	"\t\t\t<service>\r\n"
	"\t\t\t\t<serviceType>urn:schemas-wifialliance-org:service:WFAWLANConfig:1</serviceType>\r\n"
	"\t\t\t\t<serviceId>urn:wifialliance-org:serviceId:WFAWLANConfig1</serviceId>\r\n"
	"\t\t\t\t<SCPDURL>x_wfawlanconfig.xml</SCPDURL>\r\n"
	"\t\t\t\t<controlURL>control?WFAWLANConfig</controlURL>\r\n"
	"\t\t\t\t<eventSubURL>event?WFAWLANConfig</eventSubURL>\r\n"
	"\t\t\t</service>\r\n"
	"\t\t</serviceList>\r\n"
	"\t</device>\r\n"
	"</root>\r\n"
	"\r\n";
