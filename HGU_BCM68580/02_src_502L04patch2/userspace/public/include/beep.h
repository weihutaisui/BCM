/***********************************************************************
 *
 * Copyright (c) 2017  Broadcom Corporation
 * All Rights Reserved
 *
 * <:label-BRCM:2017:DUAL/GPL:standard
 *
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed
 * to you under the terms of the GNU General Public License version 2
 * (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
 * with the following added to such license:
 *
 *    As a special exception, the copyright holders of this software give
 *    you permission to link this software with independent modules, and
 *    to copy and distribute the resulting executable under terms of your
 *    choice, provided that you also meet, for each linked independent
 *    module, the terms and conditions of the license of that module.
 *    An independent module is a module which is not derived from this
 *    software.  The special exception does not apply to any modifications
 *    of the software.
 *
 * Not withstanding the above, under no circumstances may you combine
 * this software in any way with any other Broadcom software provided
 * under a license other than the GPL, without Broadcom's express prior
 * written consent.
 *
 * :>
 *
 ************************************************************************/


/*****************************************************************************
*    Description:
*
*      BEEP header file.
*
*****************************************************************************/
#ifndef BEEP_H
#define BEEP_H

/* ---- Include Files ----------------------------------------------------- */


/* ---- Constants and Types ----------------------------------------------- */

#define BEEP_USERNAME_LEN_MAX    32
#define BEEP_APPNAME_LEN_MAX     32
#define BEEP_BUSNAME_LEN_MAX     255
#define BEEP_OBJPATH_LEN_MAX     255
#define BEEP_INTERFACE_LEN_MAX   255
#define BEEP_MEMBER_LEN_MAX      255

/* ---- DBUS defines ---- */

#define DBUS_BUS_NAME            "org.freedesktop.DBus"
#define DBUS_OBJECT_PATH         "/org/freedesktop/DBus"
#define DBUS_INTERFACE           "org.freedesktop.DBus"

/* ---- SPD defines ---- */

#define SPD_BUS_NAME             "com.broadcom.spd"
#define SPD_OBJECT_PATH          "/com/broadcom/spd"
#define SPD_INTERFACE            "com.broadcom.spd"
#define SPD_BUSGATE_OBJECT_PATH  "/com/broadcom/busgate"
#define SPD_BUSGATE_INTERFACE    "com.broadcom.busgate"

/* ---- PMD defines ---- */

#define PMD_BUS_NAME             "com.broadcom.pmd"
#define PMD_OBJECT_PATH          "/com/broadcom/pmd"
#define PMD_INTERFACE            "com.broadcom.pmd"

/* ---- CWMPD defines ---- */

#define CWMPD_BUS_NAME           "com.broadcom.cwmp"
#define CWMPD_OBJECT_PATH        "/com/broadcom/cwmp"
#define CWMPD_INTERFACE          "com.broadcom.cwmp"
#define CWMPUTL_OBJECT_PATH      "/com/broadcom/cwmputl"
#define CWMPUTL_INTERFACE        "com.broadcom.cwmputl"

/* ---- DAD defines ---- */

#define DAD_BUS_NAME             "com.broadcom.DataAdaptation"
#define DAD_OBJECT_PATH          "/com/broadcom/DataAdaptation"
#define DAD_LAYER2_INTERFACE     "com.broadcom.DataAdaptation.Layer2"
#define DAD_CWMPCLIENT_INTERFACE "com.broadcom.DataAdaptation.CwmpClient"
#define DAD_WIFI_INTERFACE       "com.broadcom.DataAdaptation.Wifi"

/* ---- Data Model Access Daemon defines ---- */

#define DMAD_BUS_NAME            "com.broadcom.dmad"
#define DMAD_OBJECT_PATH         "/com/broadcom/dmad"
#define DMAD_INTERFACE           "com.broadcom.dmad"

/* ---- BEEP User Services Platform Daemon defines ---- */

#define USPD_BUS_NAME            "com.broadcom.uspd"
#define USPD_OBJECT_PATH         "/com/broadcom/uspd"
#define USPD_INTERFACE           "com.broadcom.uspd"


/* -------------------------------- */
/* ---- Signals emitted by PMD ---- */
/* -------------------------------- */

/* Signal Name: DuStateChangeComplete
 * Description:
 *    DU operation has been completed.
 * Parameters:
 *    char *operation  - Install, Update, Uninstall, install_at_bootup
 *    char *URL  - Location of DU to be installed/update
 *    char *UUID  - Unique ID to describe DU,36 bytes
 *    char *version  - Version of DU
 *    char *EUlist  - list of comma seperated EU bundle ID
 *    char *DUlist  - list of comma seperated DU bundle ID
 *    gint32   faultCode  -
 *    guint16  reqId  -
 */
#define SIGNAL_DU_STATE_CHANGE_COMPLETE   "DuStateChangeComplete"

/* Signal Name: EthLinkUp
 * Description:
 *    Eth link is up.
 * Parameters:
 *    char *ifname  - interface name
 */
#define SIGNAL_ETH_LINK_UP    "EthLinkUp"

/* Signal Name: EthLinkDown
 * Description:
 *    Eth link is down.
 * Parameters:
 *    char *ifname  - interface name
 */
#define SIGNAL_ETH_LINK_DOWN  "EthLinkDown"

/* Signal Name: UsbLinkUp
 * Description:
 *    Usb link is up.
 * Parameters:
 *    char *ifname  - interface name
 */
#define SIGNAL_USB_LINK_UP    "UsbLinkUp"

/* Signal Name: UsbLinkDown
 * Description:
 *    Usb link is down.
 * Parameters:
 *    char *ifname  - interface name
 */
#define SIGNAL_USB_LINK_DOWN  "UsbLinkDown"

/* Signal Name: WifiLinkUp
 * Description:
 *    Wifi link is up.
 * Parameters:
 *    char *ifname  - interface name
 */
#define SIGNAL_WIFI_LINK_UP    "WifiLinkUp"

/* Signal Name: WifiLinkDown
 * Description:
 *    Wifi link is down.
 * Parameters:
 *    char *ifname  - interface name
 */
#define SIGNAL_WIFI_LINK_DOWN  "WifiLinkDown"

/* Signal Name: MocaLanLinkUp
 * Description:
 *    Moca LAN link is up.
 * Parameters:
 *    char *ifname  - interface name
 */
#define SIGNAL_MOCA_LAN_LINK_UP     "MocaLanLinkUp"

/* Signal Name: MocaLanLinkDown
 * Description:
 *    Moca LAN link is down.
 * Parameters:
 *    char *ifname  - interface name
 */
#define SIGNAL_MOCA_LAN_LINK_DOWN   "MocaLanLinkDown"

/* Signal Name: HomePlugLinkUp
 * Description:
 *    Homeplug link is up.
 * Parameters:
 *    char *ifname  - interface name
 */
#define SIGNAL_HOMEPLUG_LINK_UP     "HomePlugLinkUp"

/* Signal Name: HomePlugLinkDown
 * Description:
 *    Homeplug link is down.
 * Parameters:
 *    char *ifname  - interface name
 */
#define SIGNAL_HOMEPLUG_LINK_DOWN   "HomePlugLinkDown"

/* Signal Name: WanLinkUp
 * Description:
 *    Wan link is up.
 * Parameters:
 *    char *ifname  - interface name
 */
#define SIGNAL_WAN_LINK_UP    "WanLinkUp"

/* Signal Name: WanLinkDown
 * Description:
 *    Wan link is down.
 * Parameters:
 *    char *ifname  - interface name
 */
#define SIGNAL_WAN_LINK_DOWN  "WanLinkDown"

/* Signal Name: WanConnectionUp
 * Description:
 *    Layer 3 Wan connection is up.
 * Parameters:
 *    char *ifname  - interface name
 */
#define SIGNAL_WAN_CONNECTION_UP    "WanConnectionUp"

/* Signal Name: WanConnectionDown
 * Description:
 *    Layer 3 Wan connection is down.
 * Parameters:
 *    char *ifname  - interface name
 */
#define SIGNAL_WAN_CONNECTION_DOWN  "WanConnectionDown"

/* Signal Name: AcsConfigChanged
 * Description:
 *    ACS configuration has changed.
 * Parameters:
 *    none
 */
#define SIGNAL_ACS_CONFIG_CHANGED   "AcsConfigChanged"

/* Signal Name: TR69cConfigChanged
 * Description:
 *    TR69c configuration has changed.
 * Parameters:
 *    none
 */
#define SIGNAL_TR69C_CONFIG_CHANGED   "TR69cConfigChanged"


/* Signal Name: DeviceNotificationLimitChanged
 * Description:
 *    TR69c manageable device notification limit has changed
 * Parameters:
 *    unsigned int limit
 */
#define SIGNAL_DEVICE_NOTIFICATION_LIMIT_CHANGED "DeviceNotificationLimitChanged"

/* Signal Name: ActiveParameterValueChanged
 * Description:
 *    One or more parameters with active notification attribute set
 *    had their value changed.
 * Parameters:
 *    none
 */
#define SIGNAL_ACTIVE_PARAMETER_VALUE_CHANGED   "ActiveParameterValueChanged"

/* Signal Name: Tr69GetRpcMethodsDiag
 * Description:
 *    request tr69c send out a GetRpcMethods.
 * Parameters:
 *    none
 */
#define SIGNAL_TR69_GETRPCMETHODS_DIAG    "Tr69GetRpcMethodsDiag"

/* Signal Name: ConfigUploadComplete
 * Description:
 *    A remote configuration cycle has ended.
 * Parameters:
 *    none
 */
#define SIGNAL_CONFIG_UPLOAD_COMPLETE  "ConfigUploadComplete"

/* Signal Name: ConfigWritten
 * Description:
 *    A config file has been written.
 * Parameters:
 *    guint32 eid  - eid who wrote out the config file.
 */
#define SIGNAL_CONFIG_WRITTEN    "ConfigWritten"

/* Signal Name: TimeStateChanged
 * Description:
 *    TIME state changed.
 * Parameters:
 *    guint32 state  - time state.
 */
#define SIGNAL_TIME_STATE_CHANGED   "TimeStateChanged"

/* Signal Name: LogicalVolumeAdded
 * Description:
 *    USB storage logical volume added.
 * Parameters:
  *    int disk int: partition
 */
#define SIGNAL_LOGICAL_VOLUME_ADDED   "LogicalVolumeAdded"

/* Signal Name: LogicalVolumeRemoved
 * Description:
 *    USB storage logical volume removed.
 * Parameters:
 *    int disk int: partition
 */
#define SIGNAL_LOGICAL_VOLUME_REMOVED   "LogicalVolumeRemoved"


/* Signal Name: PmdTermination
 * Description:
 *    Pmd is terminating.
 * Parameters:
 *    none
 * Note: This is a public signal that reqires no permission to receive.
 *       PMD related applications should subscribe and gracefully shutdown
 *       upon receiving this signal.
 */
#define SIGNAL_PMD_TERMINATION  "PmdTermination"

/* Signal Name: SpdTermination
 * Description:
 *    Spd is terminating.
 * Parameters:
 *    none
 * Note: This is a public signal that reqires no permission to receive.
 *       SPD related applications should subscribe and gracefully shutdown
 *       upon receiving this signal.
 */
#define SIGNAL_SPD_TERMINATION  "SpdTermination"


/* ---------------------------------- */
/* ---- Signals emitted by CWMPD ---- */
/* ---------------------------------- */

/* Signal Name: AcsConfigChanged
 * Description:
 *    ACS configuration has changed.
 * Parameters:
 *    none
 */
#define SIGNAL_ACS_CONFIG_CHANGED   "AcsConfigChanged"

/* Signal Name: PingStateChanged 
 * Description:
 *    Forward ipping diag state change event
 * Parameters:
 *    none
 */
#define SIGNAL_PING_STATE_CHANGED "PingStateChanged"

/* Signal Name: Tr69ActiveNotification
 * Description:
 *    One or more parameters with active notification attribute set
 *    had their value changed.
 * Parameters:
 *    none
 */
#define SIGNAL_TR69_ACTIVE_NOTIFICATION   "Tr69ActiveNotification"

/* Signal Name: StunConfigChanged
 * Description:
 *    STUN configuration has changed.
 * Parameters:
 *    none
 */
#define SIGNAL_STUN_CONFIG_CHANGED  "StunConfigChanged"


#endif /* BEEP_H */
