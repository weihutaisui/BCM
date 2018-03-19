//Note: The options array initialized in menu.html follows
//      the MENU_OPTION order defined here.	Both sides must
//      be in the same order.
var MENU_OPTION_USER              = 0;
var MENU_OPTION_STANDARD          = 1;
var MENU_OPTION_PROTOCOL          = 2;
var MENU_OPTION_FIREWALL          = 3;
var MENU_OPTION_NAT               = 4;
var MENU_OPTION_IP_EXTENSION      = 5;
var MENU_OPTION_WIRELESS          = 6;
var MENU_OPTION_VOICE_TR104       = 7;
var MENU_OPTION_SNMP              = 8;
var MENU_OPTION_UPNP              = 9;
var MENU_OPTION_DDNSD             = 10;
var MENU_OPTION_SNTP              = 11;
var MENU_OPTION_EBTABLES          = 12;
var MENU_OPTION_BRIDGE            = 13;
var MENU_OPTION_TOD               = 14;
var MENU_OPTION_SIPROXD           = 15;
var MENU_OPTION_DHCPEN            = 16;
var MENU_OPTION_QOS               = 17;
var MENU_OPTION_PORTMAP           = 18;
var MENU_OPTION_IPP               = 19;
var MENU_OPTION_WIRELESS_SES      = 20;
var MENU_OPTION_RIP               = 21;
var MENU_OPTION_IPSEC             = 22;
var MENU_OPTION_CERT              = 23;
var MENU_OPTION_WL_QOS            = 24;
var MENU_OPTION_TR69C             = 25;
var MENU_OPTION_VDSL              = 26;
var MENU_OPTION_URLFILTER         = 27;
var MENU_OPTION_IPV6_SUPPORT      = 28;
var MENU_OPTION_IPV6_ENABLE       = 29;
var MENU_OPTION_DNSPROXY          = 30;
var MENU_OPTION_POLICY_ROUTING    = 31;
var MENU_OPTION_OMCI              = 32;
var MENU_OPTION_CHIPID            = 33;
var MENU_OPTION_WIRELESS_NUM_ADAPTOR =34;
var MENU_OPTION_DIAG_P8021AG      =35;
var MENU_OPTION_ETHWAN            =36;
var MENU_OPTION_PTMWAN            =37;
var MENU_OPTION_EPTAPP            =38;
var MENU_OPTION_PWRMNGT           =39;
var MENU_OPTION_VOICE_NTR         =40;
var MENU_OPTION_ATMWAN            =41;
var MENU_OPTION_MOCAWAN           =42;
var MENU_OPTION_VOICE_DECT        =43;
var MENU_OPTION_DSL_BONDING       =44;
var MENU_OPTION_MULTICAST         =45;
var MENU_OPTION_VPN               =46;
var MENU_OPTION_STORAGESERVICE    =47;
var MENU_OPTION_SUPPORT_MOCA      =48;
var MENU_OPTION_STANDBY           =49;
var MENU_OPTION_DLNA              =50;
var MENU_OPTION_WIRELESS_WAPI_AS  =51;
var MENU_OPTION_AUTODETECTION	  =52;
var MENU_OPTION_GPONWAN           =53;
var MENU_OPTION_POLICE_ENABLE     =54;
var MENU_OPTION_MODSW_WEBUI       =55;
var MENU_OPTION_EPONWAN           =56;
var MENU_OPTION_SAMBA             =57;
var MENU_OPTION_BMU               =58;
var MENU_OPTION_BUILD_VDSL        =59;
var MENU_OPTION_SUPPORT_LAN_VLAN  =60;
var MENU_OPTION_OPTICAL           =61;
var MENU_OPTION_WIFIWAN           =62;
var MENU_OPTION_MODSW_WEBUI_ADMIN =63;
var MENU_OPTION_MODSW_WEBUI_SUPPORT =64;
var MENU_OPTION_MODSW_BASELINE    =65;
var MENU_OPTION_MODSW_OSGIEE      =66;
var MENU_OPTION_MODSW_LINUXEE     =67;
var MENU_OPTION_MODSW_LINUXPFP    =68;
var MENU_OPTION_SUPPORT_ETHPORTSHAPING =69;
var MENU_OPTION_BUILD_TMS         =70;
var MENU_OPTION_BUILD_SPDSVC      =71;
var MENU_OPTION_SUPPORT_HOMEPLUG  =72;
var MENU_OPTION_XMPP              =73;
var MENU_OPTION_JQPLOT            =74;
var MENU_OPTION_WEB_SOCKETS       =75;
var MENU_OPTION_DPI               =76;
var MENU_OPTION_SUPPORT_IEEE1905  =77;
var MENU_OPTION_SUPPORT_WEBSOCKETS=78;
var MENU_OPTION_SUPPORT_WLVISUALIZATION =79;
var MENU_OPTION_VOICE_SIP_CCTK    =80;
var MENU_OPTION_BUILD_USB_HOSTS   =81;
var MENU_OPTION_QRCODE_SAMPLE      =82;
var MENU_OPTION_SUPPORT_WLPASSPOINT =83;
var MENU_OPTION_SUPPORT_CELLULAR  =84;
var MENU_OPTION_NFC               =85;
var MENU_OPTION_EPON_LOID         =86;
var MENU_OPTION_INGRESS_FILTERS   =87;
var MENU_OPTION_STATS_QUEUE       =88;
var MENU_OPTION_SUPPORT_WLROUTER_PAGE   =89;
var MENU_OPTION_OPENVSWITCH       =90;
var MENU_OPTION_STUN              =91;
var MENU_OPTION_DEMO_ITEM         =92;
var MENU_OPTION_DBUS_REMOTE	  =93;
var MENU_OPTION_MAPT	          =94;
var MENU_OPTION_VOICE_SIPMODE	  =95;
var MENU_OPTION_PMD               =96;

var wlItemsCgiCmd = new Array(
 	                    'wlswitchinterface0.wl',
                           'wlswitchinterface1.wl',
                           'wlswitchinterface2.wl',
                           'wlswitchinterface3.wl'
                          );

 var wlmenuTitle = new Array(
 	                    'wl0',
                           'wl1',
                           'wl2',
                           'wl3'
                          );
function menuAdmin(options) {
   var std = options[MENU_OPTION_STANDARD];
   var proto = options[MENU_OPTION_PROTOCOL];
   var firewall = options[MENU_OPTION_FIREWALL];
   var ipExt = options[MENU_OPTION_IP_EXTENSION];
   var wireless = options[MENU_OPTION_WIRELESS];
   var voiceTr104Option = options[MENU_OPTION_VOICE_TR104];
   var snmp = options[MENU_OPTION_SNMP];
   var ddnsd = options[MENU_OPTION_DDNSD];
   var sntp = options[MENU_OPTION_SNTP];
   var ebtables = options[MENU_OPTION_EBTABLES];
   var bridge = options[MENU_OPTION_BRIDGE];
   var tod = options[MENU_OPTION_TOD];
   var QosEnabled = options[MENU_OPTION_QOS];
   var vlanconfig = options[MENU_OPTION_PORTMAP];
   var ipp = options[MENU_OPTION_IPP];
   var dlna = options[MENU_OPTION_DLNA];
   var wireless_ses = options[MENU_OPTION_WIRELESS_SES];
   var rip = options[MENU_OPTION_RIP];
   var ipsec = options[MENU_OPTION_IPSEC];
   var certificate = options[MENU_OPTION_CERT];
   var wlqos = options[MENU_OPTION_WL_QOS];
   var tr69c = options[MENU_OPTION_TR69C];
   var ipv6Support = options[MENU_OPTION_IPV6_SUPPORT];
   var ipv6Enable = options[MENU_OPTION_IPV6_ENABLE];
   var upnp = options[MENU_OPTION_UPNP];
   var urlfilter = options[MENU_OPTION_URLFILTER];
   var dnsproxy = options[MENU_OPTION_DNSPROXY];
   var pr = options[MENU_OPTION_POLICY_ROUTING];
   var omci = options[MENU_OPTION_OMCI];
   var numWl = options[MENU_OPTION_WIRELESS_NUM_ADAPTOR];
   var ethwan = options[MENU_OPTION_ETHWAN];
   var ptm = options[MENU_OPTION_PTMWAN];
   var eptapp = options[MENU_OPTION_EPTAPP];
   var pwrmngt = options[MENU_OPTION_PWRMNGT];
   var standby = options[MENU_OPTION_STANDBY];
   var voiceNtr = options[MENU_OPTION_VOICE_NTR];
   var atm = options[MENU_OPTION_ATMWAN];
   var mocawan = options[MENU_OPTION_MOCAWAN];
   var gponwan = options[MENU_OPTION_GPONWAN];
   var eponwan = options[MENU_OPTION_EPONWAN];
   var dect = options[MENU_OPTION_VOICE_DECT];
   var dslbonding = options[MENU_OPTION_DSL_BONDING];
   var multicast = options[MENU_OPTION_MULTICAST];
   var vpn = options[MENU_OPTION_VPN];
   var storageservice = options[MENU_OPTION_STORAGESERVICE];
   var sambaservice = options[MENU_OPTION_SAMBA];
   var mocaCfg = options[MENU_OPTION_SUPPORT_MOCA];
   var wireless_wapi = options[MENU_OPTION_WIRELESS_WAPI_AS];
   var autoDetection = options[MENU_OPTION_AUTODETECTION];
   var policeEnable = options[MENU_OPTION_POLICE_ENABLE];
   var isDsl = 0;
   var modsw_webui = options[MENU_OPTION_MODSW_WEBUI];
   var modsw_webui_admin = options[MENU_OPTION_MODSW_WEBUI_ADMIN];
   var modsw_baseline = options[MENU_OPTION_MODSW_BASELINE];
   var modsw_linuxpfp = options[MENU_OPTION_MODSW_LINUXPFP];
   var bmu = options[MENU_OPTION_BMU];
   var buildVdsl = options[MENU_OPTION_BUILD_VDSL];
   var lanvlanEnable = options[MENU_OPTION_SUPPORT_LAN_VLAN];
   var wifiwan = options[MENU_OPTION_WIFIWAN]; 	
   var supportEthPortShaping = options[MENU_OPTION_SUPPORT_ETHPORTSHAPING];
   var buildTms = options[MENU_OPTION_BUILD_TMS];
   var buildSpdsvc = options[MENU_OPTION_BUILD_SPDSVC];
   var homeplug = options[MENU_OPTION_SUPPORT_HOMEPLUG];
   var xmpp = options[MENU_OPTION_XMPP];
   var jqplot = options[MENU_OPTION_JQPLOT];
   var websockets = options[MENU_OPTION_WEB_SOCKETS];
   var dpi = options[MENU_OPTION_DPI];
   var ieee1905 = options[MENU_OPTION_SUPPORT_IEEE1905];
   var support1905TopologyWeb = options[MENU_OPTION_SUPPORT_WEBSOCKETS];
   var wlVisualization= options[MENU_OPTION_SUPPORT_WLVISUALIZATION];
   var sipCctk = options[MENU_OPTION_VOICE_SIP_CCTK];
   var wlPasspoint= options[MENU_OPTION_SUPPORT_WLPASSPOINT];
   var cellularwan = options[MENU_OPTION_SUPPORT_CELLULAR];
   var nfc = options[MENU_OPTION_NFC];
   var eponLoid = options[MENU_OPTION_EPON_LOID];
   var ingressFilters = options[MENU_OPTION_INGRESS_FILTERS];
   var wr_pages = options[MENU_OPTION_SUPPORT_WLROUTER_PAGE];
   var openvswitch = options[MENU_OPTION_OPENVSWITCH];
   var stun = options[MENU_OPTION_STUN];
   var dslcpe_demo_on = options[MENU_OPTION_DEMO_ITEM];
   var mapt = options[MENU_OPTION_MAPT];
   var voiceSipMode = options[MENU_OPTION_VOICE_SIPMODE];
   var pmd_option = options[MENU_OPTION_PMD];

   var anywan = (ptm == '1' || atm == '1' || mocawan == '1' || cellularwan == '1' ||
       ethwan == '1' || wifiwan == '1' || gponwan == '1' || eponwan == '1');

	// Configure advanced setup/layer 2 interface 
	if (atm == '1' ) {
		isDsl = 1;
		nodeAdvancedSetup = insFld(foldersTree, gFld(getMenuTitle(MENU_ADVANCED_SETUP), 'dslatm.cmd'));
		nodeLayer2Inteface = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_LAYER2_INTERFACE), 'dslatm.cmd'));
		insDoc(nodeLayer2Inteface, gLnk('R', getMenuTitle(MENU_DSL_ATM_INTERFACE), 'dslatm.cmd'));		
	} 
	if (ptm == '1') {
		isDsl = 1;
		if (atm != '1') {
			nodeAdvancedSetup = insFld(foldersTree, gFld(getMenuTitle(MENU_ADVANCED_SETUP), 'dslptm.cmd'));
			nodeLayer2Inteface = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_LAYER2_INTERFACE), 'dslptm.cmd'));
		}
		insDoc(nodeLayer2Inteface, gLnk('R', getMenuTitle(MENU_DSL_PTM_INTERFACE), 'dslptm.cmd'));		
	}	
	if (gponwan == '1' ) {
		if (!(atm == '1' || ptm == '1')) {
			nodeAdvancedSetup = insFld(foldersTree, gFld(getMenuTitle(MENU_ADVANCED_SETUP), 'gponwan.cmd'));
			nodeLayer2Inteface = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_LAYER2_INTERFACE), 'gponwan.cmd'));
		}
		insDoc(nodeLayer2Inteface, gLnk('R', getMenuTitle(MENU_GPONWAN_INTERFACE), 'gponwan.cmd'));
	}
	if (eponwan == '1' ) {
		if (!(atm == '1' || ptm == '1' || gponwan == '1')) {
			nodeAdvancedSetup = insFld(foldersTree, gFld(getMenuTitle(MENU_ADVANCED_SETUP), 'eponwan.cmd'));
			nodeLayer2Inteface = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_LAYER2_INTERFACE), 'eponwan.cmd'));
		}
		insDoc(nodeLayer2Inteface, gLnk('R', getMenuTitle(MENU_EPONWAN_INTERFACE), 'eponwan.cmd'));
	}	
	if (ethwan == '1' ) {
		if (!(atm == '1' || ptm == '1' || gponwan == '1' || eponwan == '1')) {
			nodeAdvancedSetup = insFld(foldersTree, gFld(getMenuTitle(MENU_ADVANCED_SETUP), 'ethwan.cmd'));
			nodeLayer2Inteface = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_LAYER2_INTERFACE), 'ethwan.cmd'));
		}
		insDoc(nodeLayer2Inteface, gLnk('R', getMenuTitle(MENU_ETH_INTERFACE), 'ethwan.cmd'));
	}
	if (wifiwan == '1' ) {
		if (!(atm == '1' || ptm == '1' || gponwan == '1' || eponwan == '1' || ethwan == '1')) {
			nodeAdvancedSetup = insFld(foldersTree, gFld(getMenuTitle(MENU_ADVANCED_SETUP), 'wifiwan.cmd'));
			nodeLayer2Inteface = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_LAYER2_INTERFACE), 'wifiwan.cmd'));
		}
		insDoc(nodeLayer2Inteface, gLnk('R', getMenuTitle(MENU_WIFI_INTERFACE), 'wifiwan.cmd'));
	}	
	if (mocawan == '1') {
		if (!(atm == '1' || ptm == '1' || ethwan == '1')) {
			nodeAdvancedSetup = insFld(foldersTree, gFld(getMenuTitle(MENU_ADVANCED_SETUP), 'mocawan.cmd'));
			nodeLayer2Inteface = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_LAYER2_INTERFACE), 'mocawan.cmd'));
		}
		insDoc(nodeLayer2Inteface, gLnk('R', getMenuTitle(MENU_MOCA_INTERFACE), 'mocawan.cmd'));
	}  

	if (cellularwan == '1') {
 		if (!(atm == '1' || ptm == '1' || gponwan == '1' || eponwan == '1' || ethwan == '1' || wifiwan == '1' || mocawan == '1')) {
			nodeAdvancedSetup = insFld(foldersTree, gFld(getMenuTitle(MENU_ADVANCED_SETUP), 'cellular.html'));
			nodeLayer2Inteface = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_LAYER2_INTERFACE), 'cellular.html'));
		}
        nodeCellular = insFld(nodeLayer2Inteface, gFld(getMenuTitle(MENU_CELLULAR_INTERFACE), 'cellular.html'));
        insDoc(nodeCellular, gLnk('R', getMenuTitle(MENU_CELLULAR_INTERFACE_INTF),'cellularinterface.html'));
        insDoc(nodeCellular, gLnk('R', getMenuTitle(MENU_CELLULAR_APN),'cellularapn.cmd?action=view'));
        insDoc(nodeCellular, gLnk('R', getMenuTitle(MENU_CELLULAR_SMS),'cellularsms.cmd?action=view'));
	}  

	if (anywan)
		insDoc(nodeAdvancedSetup, gLnk('R', getMenuTitle(MENU_WAN),'wancfg.cmd'));
	else
		nodeAdvancedSetup = insFld(foldersTree, gFld(getMenuTitle(MENU_ADVANCED_SETUP), 'lancfg2.html'));

	
	nodeLAN = insDoc(nodeAdvancedSetup, gFld(getMenuTitle(MENU_LAN),'lancfg2.html'));
	if ( lanvlanEnable == '1' ) 
		insDoc(nodeLAN, gLnk('R', getMenuTitle(MENU_LAN_VLAN),'lanvlancfg.html'));

	if (vpn == '1') {
		nodeVPN = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_VPN), 'l2tpacwan.cmd'));
		insDoc(nodeVPN, gLnk('R', getMenuTitle(MENU_VPN_L2TPAC), 'l2tpacwan.cmd'));		
	}

   if ( ipv6Enable == '1' ) {
      insDoc(nodeLAN, gLnk('R', getMenuTitle(MENU_LAN6),'ipv6lancfg.html'));
   }

   // Configure connection auto detection
   if (autoDetection == '1')
      insDoc(nodeAdvancedSetup, gFld(getMenuTitle(MENU_AUTODETECTION), 'autodetection.cmd?action=view'));

   if ( mocaCfg == '1' ) {
      insDoc(nodeAdvancedSetup, gLnk('R', getMenuTitle(MENU_MOCA_CONFIGURATION),'mocacfg.html'));
   }

   if (anywan) {
      // Configure security menu
      // If firewall is enabled and not in ipExt mode enable firewall menus
      // if (proto != 'Bridge' && ipExt != '1' ) {
      if ( proto != 'Not Applicable' && ipExt != '1' ) {
         // NAT menu is always displayed now
         nodeNat = insDoc(nodeAdvancedSetup, gFld(getMenuTitle(MENU_SC_NAT), 'scvrtsrv.cmd?action=view'));
         insDoc(nodeNat, gLnk('R', getMenuTitle(MENU_SC_VIRTUAL_SERVER), 'scvrtsrv.cmd?action=view'));
         insDoc(nodeNat, gLnk('R', getMenuTitle(MENU_SC_PORT_TRIGGER), 'scprttrg.cmd?action=view'));
         insDoc(nodeNat, gLnk('R', getMenuTitle(MENU_SC_DMZ_HOST), 'scdmz.html'));

         // Security menu is always displayed now                   	
         nodeFirewall = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_SC_SECURITY), 'scoutflt.cmd?action=view'));
         nodeIpFlt = insFld(nodeFirewall, gFld(getMenuTitle(MENU_SC_IP_FILTER), 'scoutflt.cmd?action=view'));
         insDoc(nodeIpFlt, gLnk('R', getMenuTitle(MENU_SC_OUTGOING), 'scoutflt.cmd?action=view'));
         insDoc(nodeIpFlt, gLnk('R', getMenuTitle(MENU_SC_INCOMING), 'scinflt.cmd?action=view'));
         insFld(nodeFirewall, gFld(getMenuTitle(MENU_MAC_FILTER),'scmacflt.cmd?action=view'));

         if ( tod == '1' ) 
         {
            nodeParentalControl = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_PARENTAL_CNTL),'todmngr.tod?action=view'));
            insDoc(nodeParentalControl, gFld(getMenuTitle(MENU_TOD),'todmngr.tod?action=view'));

            if ( urlfilter == '1' )
            {
               insDoc(nodeParentalControl, gFld(getMenuTitle(MENU_URLFILTER),'urlfilter.cmd?action=view'));
            }
         }
         else if ( urlfilter == '1' )
         {
            nodeParentalControl = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_PARENTAL_CNTL),'urlfilter.cmd?action=view'));
            insDoc(nodeParentalControl, gFld(getMenuTitle(MENU_URLFILTER),'urlfilter.cmd?action=view'));
         }
      }

      // Configure QoS class menu
      nodeQos = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_QOS),'qosqmgmt.html'));
      nodeQosQueue = insFld(nodeQos, gFld(getMenuTitle(MENU_QOS_QUEUE),'qosqueue.cmd?action=view'));
      insDoc(nodeQosQueue, gLnk('R', getMenuTitle(MENU_QUEUE_CFG), 'qosqueue.cmd?action=view'));
      if ( parseInt(numWl) != 0 )
         insDoc(nodeQosQueue, gLnk('R', getMenuTitle(MENU_WL_QUEUE), 'qosqueue.cmd?action=view_wlq'));
      if (policeEnable == '1')
         insDoc(nodeQos, gLnk('R', getMenuTitle(MENU_QOS_POLICER), 'qospolicer.cmd?action=view'));
      insDoc(nodeQos, gLnk('R', getMenuTitle(MENU_QOS_CLASS), 'qoscls.cmd?action=view'));
      if (supportEthPortShaping == '1')
      {
         insDoc(nodeQos, gLnk('R', getMenuTitle(MENU_QOS_PORT_SHAPING), 'qosportshaping.html'));
      }

      // Configure routing menu
      nodeRouting = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_ROUTING), 'rtdefaultcfg.html'));
      insDoc(nodeRouting, gLnk('R', getMenuTitle(MENU_RT_DEFAULT_ROUTE), 'rtdefaultcfg.html'));
      insDoc(nodeRouting, gLnk('R', getMenuTitle(MENU_RT_STATIC_ROUTE),'rtroutecfg.cmd?action=viewcfg'));
   }
   else {
      // Configure routing menu for bridging devices with no WAN interface
      nodeRouting = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_ROUTING), 'rtroutecfg.cmd?action=viewcfg'));
      insDoc(nodeRouting, gLnk('R', getMenuTitle(MENU_RT_STATIC_ROUTE),'rtroutecfg.cmd?action=viewcfg'));
   }

   if (pr == '1' )
      insDoc(nodeRouting, gLnk('R', getMenuTitle(MENU_POLICY_ROUTING),'prmngr.cmd?action=view'));

   if ( (proto == 'PPPoE' && ipExt == '0') ||
        (proto == 'PPPoA' && ipExt == '0') ||
        (proto == 'MER') ||
        (proto == 'IPoA') ) {
      // configure rip
      if ( rip == '1' )
         insDoc(nodeRouting, gLnk('R', getMenuTitle(MENU_RT_RIP),'ripcfg.cmd?action=view'));
      // configure dns server
      nodeDnsSetup = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DNS), 'dnscfg.html'));
      insDoc(nodeDnsSetup, gLnk('R', getMenuTitle(MENU_DNS_SETUP), 'dnscfg.html'));
      // configure ddns client
      if ( ddnsd == '1' )
         insDoc(nodeDnsSetup, gLnk('R', getMenuTitle(MENU_DDNS), 'ddnsmngr.cmd'));
   }

   if (isDsl == 1)
   {
      // Configure ADSL Setting Menu based on Annex
      if ( std == 'annex_c' )
         insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DSL), 'adslcfgc.html'));
      else if (buildVdsl == '1')
         insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DSL), 'xdslcfg.html'));
      else if (buildVdsl == '2')
         insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DSL), 'xdslcfg1.html'));
      else
         insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DSL), 'adslcfg.html'));

      if (dslbonding == '1')
         insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DSL_BONDING), 'dslbondingcfg.html'));
   }

	// Configure upnp
	if (upnp == '1')
	   insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_UPNP), 'upnpcfg.html'));

	
   // Configure dnsproxy
   if (dnsproxy == '1')
      insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DNSPROXY), 'dnsproxycfg.html'));

   // Configure print server
   if ( ipp == '1' )
      insDoc(nodeAdvancedSetup, gFld(getMenuTitle(MENU_IPP), 'ippcfg.html'));

   // Configure dlna
   if ( dlna == '1' )
      insDoc(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DLNA), 'dlnacfg.html'));

   // Configure wireless menu
   if ( parseInt(numWl) != 0 ) {
	   if(wr_pages == 'y' || wr_pages == '1' ) {
		   wl_wr_node = insFld(foldersTree,gFld("Wireless", "wlrouter/ssid.asp"));
		   insDoc(wl_wr_node, gLnk('R', getMenuTitle(MENU_WL_MENU_RADIO), 'wlrouter/radio.asp'));
		   insDoc(wl_wr_node, gLnk('R', getMenuTitle(MENU_WL_MENU_MEDIA), 'wlrouter/media.asp'));
		   insDoc(wl_wr_node, gLnk('R', getMenuTitle(MENU_WL_MENU_SSID), 'wlrouter/ssid.asp'));
		   insDoc(wl_wr_node, gLnk('R', getMenuTitle(MENU_WL_MENU_SECURITY), 'wlrouter/security.asp'));
		   insDoc(wl_wr_node, gLnk('R', getMenuTitle(MENU_WL_MENU_WPS), 'wlrouter/wps.asp'));
		   if(wlPasspoint == '1') {
			   insDoc(wl_wr_node, gLnk('R', getMenuTitle(MENU_WL_PASSPOINT), 'wlrouter/passpoint.asp'));
		   }
		   if(wlVisualization == '1') {

			   wlVisualNode = insFld(wl_wr_node, gFld(getMenuTitle(MENU_WL_VISUALIZATION), "wlrouter/configure.asp"));
			   insDoc(wlVisualNode, gLnk('R', getMenuTitle(MENU_WL_VISUALIZATION_SITESURVEY), 'wlrouter/visindex.asp'));
			   insDoc(wlVisualNode, gLnk('R', getMenuTitle(MENU_WL_VISUALIZATION_CHANNELSTAT), 'wlrouter/channelcapacity.asp'));
			   insDoc(wlVisualNode, gLnk('R', getMenuTitle(MENU_WL_VISUALIZATION_METRICS), 'wlrouter/metrics.asp'));
			   insDoc(wlVisualNode, gLnk('R', getMenuTitle(MENU_WL_VISUALIZATION_CONFIGURE), 'wlrouter/configure.asp'));
		   }
		   if(dslcpe_demo_on == '1' || dslcpe_demo_on=='y') {

			   insDoc(wl_wr_node, gLnk('R', getMenuTitle(MENU_WL_MENU_DEMO_ON_DFS), 'wlrouter/zerowaitdfs.asp'));
			   insDoc(wl_wr_node, gLnk('R', getMenuTitle(MENU_WL_MENU_DEMO_ON_WBD), 'wlrouter/wbd_demo.asp'));
           }

	   } else {
		   if(numWl != '1')
			   wlanMenu = insFld(foldersTree, gFld(getMenuTitle(MENU_WIRELESS_SETTINGS), wlItemsCgiCmd[0]));

		   for(i = 0; i < parseInt(numWl); i++)
		   {
			   // Configure wireless menu
			   if(numWl == '1')
				   nodeWireless = insFld(foldersTree, gFld(getMenuTitle(MENU_WIRELESS_SETTINGS), wlItemsCgiCmd[0]));
			   else
				   nodeWireless = insFld(wlanMenu, gFld(wlmenuTitle[i], wlItemsCgiCmd[i]));

			   insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_BASIC), 'wlcfg.html'));
			   insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_SECURITY), 'wlsecurity.html'));
			   insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_MAC_FILTERING), 'wlmacflt.cmd?action=view'));
			   insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_WDS), 'wlwds.cmd?action=view'));
			   insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_ADVANCED), 'wlcfgadv.html'));
			   //SUPPORT_SES
			   if ( wireless_ses == '1' ) { 
				   insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_SES), 'wlses.html'));      
			   }

			   insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_STATION_LIST), 'wlstationlist.cmd'));
		   }

		   if ( wireless_wapi == '1' ) {      
			   if (numWl == '1') {
				   insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_WAPI_AS), 'wlwapias.html'));
			   }
			   else {
				   insDoc(wlanMenu, gLnk('R', getMenuTitle(MENU_WL_WAPI_AS), 'wlwapias.html'));
			   }
		   }

		   if(wlVisualization == '1') {
			   if (numWl == '1') {
				   wlVisualNode = insFld(nodeWireless, gFld(getMenuTitle(MENU_WL_VISUALIZATION), "wlrouter/configure.asp"));
			   } else {
				   wlVisualNode = insFld(wlanMenu, gFld(getMenuTitle(MENU_WL_VISUALIZATION), "wlrouter/configure.asp"));
			   }
			   insDoc(wlVisualNode, gLnk('R', getMenuTitle(MENU_WL_VISUALIZATION_SITESURVEY), 'wlrouter/visindex.asp'));
			   insDoc(wlVisualNode, gLnk('R', getMenuTitle(MENU_WL_VISUALIZATION_CHANNELSTAT), 'wlrouter/channelcapacity.asp'));
			   insDoc(wlVisualNode, gLnk('R', getMenuTitle(MENU_WL_VISUALIZATION_METRICS), 'wlrouter/metrics.asp'));
			   insDoc(wlVisualNode, gLnk('R', getMenuTitle(MENU_WL_VISUALIZATION_CONFIGURE), 'wlrouter/configure.asp'));
		   }

		   if(wlPasspoint == '1') {
			   if (numWl == '1') {
				   wlPassPointNode = insFld(nodeWireless, gFld(getMenuTitle(MENU_WL_PASSPOINT), "wlrouter/passpoint.asp"));
			   } else {
				   wlPassPointNode = insFld(wlanMenu, gFld(getMenuTitle(MENU_WL_PASSPOINT), "wlrouter/passpoint.asp"));
			   }
		   }
	   } 

   }


     /*Storage Service menu */
   if(storageservice == '1')
   {
      nodeStorage = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_STORAGESERVICE), 'storageservicecfg.cmd?view'));
      insDoc(nodeStorage, gLnk('R', getMenuTitle(MENU_STORAGE_INFO), 'storageservicecfg.cmd?view'));
      if(sambaservice == '1'){
         insDoc(nodeStorage, gLnk('R', getMenuTitle(MENU_STORAGE_USERACCOUNT), 'storageuseraccountcfg.cmd?view'));
      }
   }

   // Configure voice 

   if ( eptapp == '1' ) {
         nodeVoice = insFld(foldersTree, gFld(getMenuTitle(MENU_VOICE_SETTINGS), 'voiceeptapp.html'));
         insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_EPTAPP), 'voiceeptapp.html'));
   } 
   else if ( voiceTr104Option == '1' ) {
      nodeVoice = insFld(foldersTree, gFld(getMenuTitle(MENU_VOICE_SETTINGS), 'voicesip_basic.html'));
      insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_SIP_BASIC), 'voicesip_basic.html'));
      insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_SIP_ADVANCED), 'voicesip_advanced.html'));
      insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_SIP_DEBUG), 'voicesip_debug.html'));
      if(sipCctk == '1'){
         insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_SIP_CCTK), 'voicesip_cctk.html'));
      }
      if( voiceNtr != '2' ) {
         insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_NTR), 'voicentr.html'));
      }
      if( dect == '1' ) {
         insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_DECT), 'voicedect.html'));
      }
   }
   else if ( voiceTr104Option == '2' ) {
      nodeVoice = insFld(foldersTree, gFld(getMenuTitle(MENU_VOICE2_SETTINGS), 'voice2_basic.html'));
      insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE2_GLOBAL), 'voice2_basic.html'));
      if (voiceSipMode == "RFC3261") {
         insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE2_SERVICE_PROVIDER), 'voice2_srvprov_rfc3261.html'));
      }
      else
	  {
         insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE2_SERVICE_PROVIDER), 'voice2_srvprov_ims.html'));
	  }
	  insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE2_CALLMGT), 'voice2_callmgt.html'));
      insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE2_STATS), 'voice2_stats.html'));
	  insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE2_DECT), 'voice2_dect.html'));
	  insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE2_DEBUG), 'voice2_debug.html'));
   }  

   // Configure VLAN port mapping menu
   if ( anywan && vlanconfig == '1' ) {
      insDoc(nodeAdvancedSetup, gLnk('R', getMenuTitle(MENU_INTF_GROUPING),'portmap.cmd'));
   }

   if ( ipv6Support == '1' ) {
      nodeIpTunnel = insDoc(nodeAdvancedSetup, gFld(getMenuTitle(MENU_IP_TUNNEL),'tunnelcfg.cmd?action=viewcfg'));
      insDoc(nodeIpTunnel, gLnk('R', getMenuTitle(MENU_6IN4_TUNNEL),'tunnelcfg.cmd?action=viewcfg'));
      insDoc(nodeIpTunnel, gLnk('R', getMenuTitle(MENU_4IN6_TUNNEL),'tunnelcfg.cmd?action=view'));
      if ( mapt == '1' ) {
         insDoc(nodeIpTunnel, gLnk('R', getMenuTitle(MENU_MAPT),'tunnelcfg.cmd?action=viewmapt'));
      }
   }

   if ( ipsec == '1' ) {
      insDoc(nodeAdvancedSetup, gLnk('R', getMenuTitle(MENU_SC_IPSEC), 'ipsec.cmd?action=view'));
   }
   if (certificate == '1')  {
      nodeCert = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_CERT), 'certlocal.cmd?action=view'));
      insDoc(nodeCert, gLnk('R', getMenuTitle(MENU_CERT_LOCAL), 'certlocal.cmd?action=view'));
      insDoc(nodeCert, gLnk('R', getMenuTitle(MENU_CERT_CA), 'certca.cmd?action=view'));
   }

   // Configure standby menu item 
   if ( standby == '1' ) 
      insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_STANDBY), 'standby.html'));

   // Configure power management 
   if ( pwrmngt == '1' ) 
      insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_PWRMNGT), 'pwrmngt.html'));
  
   if ( bmu == '1' ) 
      insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_BMU), 'bmu.html'));

   if ( multicast == '1' ) 
      insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_MULTICAST), 'multicast.html'));
    
   // Configure epon loid authentication  
   if ( eponLoid == '1' ) 
      insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_OAM_LOID), 'eponloid.html'));
   
   if ( ingressFilters == '1' ) 
      insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_INGRESS_FILTERS), 'ingressFilters.html'));

   // Configure nfc
   if ( nfc == '1' )
      insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_NFC), 'nfc.html'));

   // Configure homeplug menu
   if (homeplug == '1') {
      nodeHomePlug = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_HOMEPLUG_INFO), 'homeplug.html'));
      insDoc(nodeHomePlug, gLnk('R', getMenuTitle(MENU_HOMEPLUG_PASSWORD), 'homeplugpassword.html'));
   }

   if (ieee1905 == '1') {
      nodeIeee1905 = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_IEEE1905), 'ieee1905cfg.html'));
      insDoc(nodeIeee1905, gLnk('R', getMenuTitle(MENU_IEEE1905_CONFIG), 'ieee1905cfg.html'));
      insDoc(nodeIeee1905, gLnk('R', getMenuTitle(MENU_IEEE1905_STATUS), 'ieee1905sts.cmd'));
      if (support1905TopologyWeb == '1') {
          insDoc(nodeIeee1905, gLnk('R', getMenuTitle(MENU_IEEE1905_TOPOLOGY), 'networkTopology.html'));
      }
   }
  
   // Configure DPI 
   if ( dpi == '1' && websockets == '1' )
      insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DPI), 'dpicharts.html'));
   if ( openvswitch == '1')
      insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_OPENVS), 'openvswitchcfg.html'));
   // Configure diagnostics menu
   nodeDiagnostics = insFld(foldersTree, gFld(getMenuTitle(MENU_DIAGNOSTICS), 'diag.html'));
   insDoc(nodeDiagnostics, gLnk('R', getMenuTitle(MENU_DIAGNOSTICS),'diag.html'));
   if (buildTms == '1') {
      insDoc(nodeDiagnostics, gLnk('R', getMenuTitle(MENU_DIAGETHOAM),'diagethoam.html'));
   }

   if (buildSpdsvc == '1' ) {
      insDoc(nodeDiagnostics, gLnk('R', getMenuTitle(MENU_SPDSVC), 'speedsvc.html'));
   }

   // Configure management menu
   nodeMngr = insFld(foldersTree, gFld(getMenuTitle(MENU_MANAGEMENT), 'backupsettings.html'));

   nodeSettings = insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_SETTINGS), 'backupsettings.html'));
   insDoc(nodeSettings, gLnk('R', getMenuTitle(MENU_TL_SETTINGS_BACKUP),'backupsettings.html'));
   insDoc(nodeSettings, gLnk('R', getMenuTitle(MENU_TL_SETTINGS_UPDATE),'updatesettings.html'));
   insDoc(nodeSettings, gLnk('R', getMenuTitle(MENU_TL_SETTINGS_DEFAULT), 'defaultsettings.html'));

   if ( pmd_option == '1' )
      insDoc(nodeSettings, gLnk('R', getMenuTitle(MENU_TL_SETTINGS_PMD),'pmdsettings.cmd'));

   insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_SYSTEM_LOG), 'logintro.html'));
   if ( anywan )
      insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_SECURITY_LOG), 'seclogintro.html'));
   if ( snmp == '1' )
      insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_SNMP), 'snmpconfig.html'));
   if ( tr69c == '1' )
      insFld(nodeMngr, gFld(getMenuTitle(MENU_TR69C), 'tr69cfg.html'));
   if ( stun == '1' ) {
      insFld(nodeMngr, gFld(getMenuTitle(MENU_STUN), 'stuncfg.html'));}
   if ( xmpp == '1' )
      insFld(nodeMngr, gFld(getMenuTitle(MENU_XMPP_CONN), 'xmppconncfg.cmd?action=view'));

   if ( omci == '1' ) {
      nodeOmci = insFld(nodeMngr, gFld(getMenuTitle(MENU_OMCI_CFG), 'omcicfg.cmd?action=view'));
      insDoc(nodeOmci, gLnk('R', getMenuTitle(MENU_OMCI_GET_SET),'omcicfg.cmd?action=view'));
      insDoc(nodeOmci, gLnk('R', getMenuTitle(MENU_OMCI_CREATE),'omcicreate.cmd?action=view'));
      insDoc(nodeOmci, gLnk('R', getMenuTitle(MENU_OMCI_GET_NEXT),'omcigetnext.cmd?action=view'));
      insDoc(nodeOmci, gLnk('R', getMenuTitle(MENU_OMCI_MACRO),'omcimacro.cmd?action=view'));
      insDoc(nodeOmci, gLnk('R', getMenuTitle(MENU_OMCI_DOWNLOAD),'omcidownload.html'));
      insDoc(nodeOmci, gLnk('R', getMenuTitle(MENU_OMCI_SYSTEM),'omcisystem.html'));
   }	  
   if ( sntp == '1' && proto != 'Bridge' && !(proto=='PPPoE' && ipExt=='1') && !(proto=='PPPoA' && ipExt=='1') )
      insFld(nodeMngr, gFld(getMenuTitle(MENU_SNTP), 'sntpcfg.html'));

   nodeAccCntr = insFld(nodeMngr, gFld(getMenuTitle(MENU_ACC_CNTR), 'password.html'));
   insDoc(nodeAccCntr, gLnk('R', getMenuTitle(MENU_ACC_CNTR_PASSWORD), 'password.html'));

   if ( modsw_webui == '1' && modsw_webui_admin == '1' && ( modsw_baseline == '1' || modsw_linuxpfp == '1' ) ) {

      if ( modsw_baseline == '1' ) {
          nodeModSw = insFld(nodeMngr, gFld(getMenuTitle(MENU_MODSW), 'modSwEE.cmd'));
          insDoc(nodeModSw, gLnk('R', getMenuTitle(MENU_MODSW_EE),'modSwEE.cmd'));
          insDoc(nodeModSw, gLnk('R', getMenuTitle(MENU_MODSW_DU),'modSwDU.cmd'));
          insDoc(nodeModSw, gLnk('R', getMenuTitle(MENU_MODSW_EU), 'modSwEU.cmd'));

          if ( modsw_linuxpfp == '1' )
              insDoc(nodeModSw, gLnk('R', getMenuTitle(MENU_MODSW_PFP), 'modSwPFP.cmd'));
      }
      else {
           nodeModSw = insFld(nodeMngr, gFld(getMenuTitle(MENU_MODSW), 'modSwPFP.cmd'));
           insDoc(nodeModSw, gLnk('R', getMenuTitle(MENU_MODSW_PFP), 'modSwPFP.cmd'));
      }

   }

   insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_UPDATE_SOFTWARE), 'upload.html'));

   insFld(nodeMngr, gFld(getMenuTitle(MENU_RESET_ROUTER), 'resetrouter.html'));

}

function menuSupport(options) {
   var std = options[MENU_OPTION_STANDARD];
   var proto = options[MENU_OPTION_PROTOCOL];
   var ipExt = options[MENU_OPTION_IP_EXTENSION];
   var wireless = options[MENU_OPTION_WIRELESS];
   var voiceTr104Option = options[MENU_OPTION_VOICE_TR104];
   var snmp = options[MENU_OPTION_SNMP];
   var ddnsd = options[MENU_OPTION_DDNSD];
   var sntp = options[MENU_OPTION_SNTP];
   var QosEnabled = options[MENU_OPTION_QOS];
   var ipp = options[MENU_OPTION_IPP];
   var rip = options[MENU_OPTION_RIP];
   var tr69c = options[MENU_OPTION_TR69C];
   var ipv6Support = options[MENU_OPTION_IPV6_SUPPORT];
   var ipv6Enable = options[MENU_OPTION_IPV6_ENABLE];
   var upnp = options[MENU_OPTION_UPNP];
   var dnsproxy = options[MENU_OPTION_DNSPROXY];
   var omci = options[MENU_OPTION_OMCI];
   var numWl = options[MENU_OPTION_WIRELESS_NUM_ADAPTOR];
   var wireless_ses = options[MENU_OPTION_WIRELESS_SES];
   var ethwan = options[MENU_OPTION_ETHWAN];
   var ptm = options[MENU_OPTION_PTMWAN];
   var eptapp = options[MENU_OPTION_EPTAPP];
   var pwrmngt = options[MENU_OPTION_PWRMNGT];
   var standby = options[MENU_OPTION_STANDBY];
   var voiceNtr = options[MENU_OPTION_VOICE_NTR];
   var atm = options[MENU_OPTION_ATMWAN];
   var mocawan = options[MENU_OPTION_MOCAWAN];
   var gponwan = options[MENU_OPTION_GPONWAN];
   var eponwan = options[MENU_OPTION_EPONWAN];
   var dect = options[MENU_OPTION_VOICE_DECT];
   var dslbonding = options[MENU_OPTION_DSL_BONDING];
   var vpn = options[MENU_OPTION_VPN];
   var storageservice = options[MENU_OPTION_STORAGESERVICE];
   var sambaservice = options[MENU_OPTION_SAMBA];
   var mocaCfg = options[MENU_OPTION_SUPPORT_MOCA];
   var wireless_wapi = options[MENU_OPTION_WIRELESS_WAPI_AS];
   var policeEnable = options[MENU_OPTION_POLICE_ENABLE];
   var bmu = options[MENU_OPTION_BMU];
   var isDsl = 0;
   var modsw_webui = options[MENU_OPTION_MODSW_WEBUI];
   var modsw_webui_support = options[MENU_OPTION_MODSW_WEBUI_SUPPORT];
   var modsw_baseline = options[MENU_OPTION_MODSW_BASELINE];
   var modsw_linuxpfp = options[MENU_OPTION_MODSW_LINUXPFP];
   var buildVdsl = options[MENU_OPTION_BUILD_VDSL];
   var lanvlanEnable = options[MENU_OPTION_SUPPORT_LAN_VLAN];
   var wifiwan = options[MENU_OPTION_WIFIWAN];
   var supportEthPortShaping = options[MENU_OPTION_SUPPORT_ETHPORTSHAPING]; 
   var xmpp = options[MENU_OPTION_XMPP];
   var jqplot = options[MENU_OPTION_JQPLOT];
   var websockets = options[MENU_OPTION_WEB_SOCKETS];
   var wlVisualization= options[MENU_OPTION_SUPPORT_WLVISUALIZATION];
   var sipCctk = options[MENU_OPTION_VOICE_SIP_CCTK];
   var wlPasspoint= options[MENU_OPTION_SUPPORT_WLPASSPOINT];
   var wr_pages = options[MENU_OPTION_SUPPORT_WLROUTER_PAGE];
   var dpi = options[MENU_OPTION_DPI];
   var stun = options[MENU_OPTION_STUN];
   var dslcpe_demo_on = options[MENU_OPTION_DEMO_ITEM];
   var dbusRemote = options[MENU_OPTION_DBUS_REMOTE];
   var mapt = options[MENU_OPTION_MAPT];
   var sipMode = options[MENU_OPTION_VOICE_SIPMODE];
   var pmd_option = options[MENU_OPTION_PMD];

   var anywan = (ptm == '1' || atm == '1' || mocawan == '1' ||
       ethwan == '1' || wifiwan == '1' || gponwan == '1' || eponwan == '1');

	// Configure advanced setup/layer 2 interface 
	if (atm == '1' ) {
		isDsl = 1;
		nodeAdvancedSetup = insFld(foldersTree, gFld(getMenuTitle(MENU_ADVANCED_SETUP), 'dslatm.cmd'));
		nodeLayer2Inteface = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_LAYER2_INTERFACE), 'dslatm.cmd'));
		insDoc(nodeLayer2Inteface, gLnk('R', getMenuTitle(MENU_DSL_ATM_INTERFACE), 'dslatm.cmd'));		
	} 
	if (ptm == '1') {
		isDsl = 1;
		if (atm != '1') {
			nodeAdvancedSetup = insFld(foldersTree, gFld(getMenuTitle(MENU_ADVANCED_SETUP), 'dslptm.cmd'));
			nodeLayer2Inteface = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_LAYER2_INTERFACE), 'dslptm.cmd'));
		}
		insDoc(nodeLayer2Inteface, gLnk('R', getMenuTitle(MENU_DSL_PTM_INTERFACE), 'dslptm.cmd'));		
	}	   	
	if (gponwan == '1' ) {
		if (!(atm == '1' || ptm == '1')) {
			nodeAdvancedSetup = insFld(foldersTree, gFld(getMenuTitle(MENU_ADVANCED_SETUP), 'gponwan.cmd'));
			nodeLayer2Inteface = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_LAYER2_INTERFACE), 'gponwan.cmd'));
		}
		insDoc(nodeLayer2Inteface, gLnk('R', getMenuTitle(MENU_GPONWAN_INTERFACE), 'gponwan.cmd'));
	}
	if (eponwan == '1' ) {
		if (!(atm == '1' || ptm == '1') || gponwan == '1' ) {
			nodeAdvancedSetup = insFld(foldersTree, gFld(getMenuTitle(MENU_ADVANCED_SETUP), 'eponwan.cmd'));
			nodeLayer2Inteface = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_LAYER2_INTERFACE), 'eponwan.cmd'));
		}
		insDoc(nodeLayer2Inteface, gLnk('R', getMenuTitle(MENU_EPONWAN_INTERFACE), 'eponwan.cmd'));
	}	
	if (ethwan == '1' ) {
		if (!(atm == '1' || ptm == '1' || gponwan == '1'  || eponwan == '1')) {
			nodeAdvancedSetup = insFld(foldersTree, gFld(getMenuTitle(MENU_ADVANCED_SETUP), 'ethwan.cmd'));
			nodeLayer2Inteface = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_LAYER2_INTERFACE), 'ethwan.cmd'));
		}
		insDoc(nodeLayer2Inteface, gLnk('R', getMenuTitle(MENU_ETH_INTERFACE), 'ethwan.cmd'));
	}
	if (wifiwan == '1' ) {
		if (!(atm == '1' || ptm == '1' || gponwan == '1' || eponwan == '1' || ethwan == '1')) {
			nodeAdvancedSetup = insFld(foldersTree, gFld(getMenuTitle(MENU_ADVANCED_SETUP), 'wifiwan.cmd'));
			nodeLayer2Inteface = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_LAYER2_INTERFACE), 'wifiwan.cmd'));
		}
		insDoc(nodeLayer2Inteface, gLnk('R', getMenuTitle(MENU_WIFI_INTERFACE), 'wifiwan.cmd'));
	}	
	if (mocawan == '1') {
		if (!(atm == '1' || ptm == '1' || ethwan == '1')) {
			nodeAdvancedSetup = insFld(foldersTree, gFld(getMenuTitle(MENU_ADVANCED_SETUP), 'mocawan.cmd'));
			nodeLayer2Inteface = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_LAYER2_INTERFACE), 'mocawan.cmd'));
		}
		insDoc(nodeLayer2Inteface, gLnk('R', getMenuTitle(MENU_MOCA_INTERFACE), 'mocawan.cmd'));
	}  
	
	if (anywan)
		insDoc(nodeAdvancedSetup, gLnk('R', getMenuTitle(MENU_WAN),'wancfg.cmd'));
	else
		nodeAdvancedSetup = insFld(foldersTree, gFld(getMenuTitle(MENU_ADVANCED_SETUP), 'lancfg2.html'));

	nodeLAN = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_LAN),'lancfg2.html'));
	if ( lanvlanEnable == '1' ) 
		insFld(nodeLAN, gFld(getMenuTitle(MENU_LAN_VLAN),'lanvlancfg.html'));
   
	if (vpn == '1') {
		nodeVPN = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_VPN), 'l2tpacwan.cmd'));
		insDoc(nodeVPN, gLnk('R', getMenuTitle(MENU_VPN_L2TPAC), 'l2tpacwan.cmd'));		
	}

   if ( ipv6Enable == '1' ) {
      insDoc(nodeLAN, gLnk('R', getMenuTitle(MENU_LAN6),'ipv6lancfg.html'));
   }

   if ( mocaCfg == '1' ) {
      insDoc(nodeAdvancedSetup, gLnk('R', getMenuTitle(MENU_MOCA_CONFIGURATION),'mocacfg.html'));
   }

   if (anywan) {
      // Configure QoS class menu
      nodeQos = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_QOS),'qosqmgmt.html'));
      nodeQosQueue = insFld(nodeQos, gFld(getMenuTitle(MENU_QOS_QUEUE),'qosqueue.cmd?action=view'));
      insDoc(nodeQosQueue, gLnk('R', getMenuTitle(MENU_QUEUE_CFG), 'qosqueue.cmd?action=view'));
      if ( parseInt(numWl) != 0 )
         insDoc(nodeQosQueue, gLnk('R', getMenuTitle(MENU_WL_QUEUE), 'qosqueue.cmd?action=view_wlq'));
      if (policeEnable == '1')
         insDoc(nodeQos, gLnk('R', getMenuTitle(MENU_QOS_POLICER), 'qospolicer.cmd?action=view'));
      insDoc(nodeQos, gLnk('R', getMenuTitle(MENU_QOS_CLASS), 'qoscls.cmd?action=view'));
      if (supportEthPortShaping == '1')
      {
         insDoc(nodeQos, gLnk('R', getMenuTitle(MENU_QOS_PORT_SHAPING), 'qosportshaping.html'));
      }
   
      // Configure routing menu
      nodeRouting = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_ROUTING), 'rtdefaultcfg.html'));
      insDoc(nodeRouting, gLnk('R', getMenuTitle(MENU_RT_DEFAULT_ROUTE), 'rtdefaultcfg.html'));
      insDoc(nodeRouting, gLnk('R', getMenuTitle(MENU_RT_STATIC_ROUTE),'rtroutecfg.cmd?action=viewcfg'));

      if ( (proto == 'PPPoE' && ipExt == '0') ||
           (proto == 'PPPoA' && ipExt == '0') ||
           (proto == 'MER') ||
           (proto == 'IPoA') ) {
         // configure rip
         if ( rip == '1' )
            insDoc(nodeRouting, gLnk('R', getMenuTitle(MENU_RT_RIP),'ripcfg.cmd?action=view'));
         // configure dns server
         nodeDnsSetup = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DNS), 'dnscfg.html'));
         insDoc(nodeDnsSetup, gLnk('R', getMenuTitle(MENU_DNS_SETUP), 'dnscfg.html'));
         // configure ddns client
         if ( ddnsd == '1' )
            insDoc(nodeDnsSetup, gLnk('R', getMenuTitle(MENU_DDNS), 'ddnsmngr.cmd'));
      }
   }
   
   if (isDsl == 1)
   {
      // Configure ADSL Setting Menu based on Annex
      if ( std == 'annex_c' )
         insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DSL), 'adslcfgc.html'));
      else if (buildVdsl == '1')
         insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DSL), 'xdslcfg.html'));
      else
         insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DSL), 'adslcfg.html'));

      if (dslbonding == '1')
         insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DSL_BONDING), 'dslbondingcfg.html'));
   }

	
   // Configure print server
   if ( ipp == '1' )
      insDoc(nodeAdvancedSetup, gFld(getMenuTitle(MENU_IPP), 'ippcfg.html'));
   
   // Configure upnp
   if (upnp == '1')
      insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_UPNP), 'upnpcfg.html'));

   // Configure dnsproxy
   if (dnsproxy == '1')
      insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DNSPROXY), 'dnsproxycfg.html'));
   
   // Configure standby menu item 
   if ( standby == '1' ) 
      insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_STANDBY), 'standby.html'));

   // Configure power management 
   if ( pwrmngt == '1' ) 
      insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_PWRMNGT), 'pwrmngt.html'));
  
   if ( bmu == '1' ) 
      insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_BMU), 'bmu.html'));

   // Configure wireless menu
   if ( parseInt(numWl) != 0 ) {
	   if(wr_pages == 'y' || wr_pages == '1' ) {
		   wl_wr_node = insFld(foldersTree,gFld("Wireless", "wlrouter/ssid.asp"));
		   insDoc(wl_wr_node, gLnk('R', getMenuTitle(MENU_WL_MENU_RADIO), 'wlrouter/radio.asp'));
		   insDoc(wl_wr_node, gLnk('R', getMenuTitle(MENU_WL_MENU_MEDIA), 'wlrouter/media.asp'));
		   insDoc(wl_wr_node, gLnk('R', getMenuTitle(MENU_WL_MENU_SSID), 'wlrouter/ssid.asp'));
		   insDoc(wl_wr_node, gLnk('R', getMenuTitle(MENU_WL_MENU_SECURITY), 'wlrouter/security.asp'));
		   insDoc(wl_wr_node, gLnk('R', getMenuTitle(MENU_WL_MENU_WPS), 'wlrouter/wps.asp'));
		   if(wlPasspoint == '1') {
			   insDoc(wl_wr_node, gLnk('R', getMenuTitle(MENU_WL_PASSPOINT), 'wlrouter/passpoint.asp'));
		   }
		   if(wlVisualization == '1') {

			   wlVisualNode = insFld(wl_wr_node, gFld(getMenuTitle(MENU_WL_VISUALIZATION), "wlrouter/configure.asp"));
			   insDoc(wlVisualNode, gLnk('R', getMenuTitle(MENU_WL_VISUALIZATION_SITESURVEY), 'wlrouter/visindex.asp'));
			   insDoc(wlVisualNode, gLnk('R', getMenuTitle(MENU_WL_VISUALIZATION_CHANNELSTAT), 'wlrouter/channelcapacity.asp'));
			   insDoc(wlVisualNode, gLnk('R', getMenuTitle(MENU_WL_VISUALIZATION_METRICS), 'wlrouter/metrics.asp'));
			   insDoc(wlVisualNode, gLnk('R', getMenuTitle(MENU_WL_VISUALIZATION_CONFIGURE), 'wlrouter/configure.asp'));
		   }
		   if(dslcpe_demo_on== '1' || dslcpe_demo_on== 'y') {
			   insDoc(wlVisualNode, gLnk('R', getMenuTitle(MENU_WL_MENU_DEMO_ON_DFS), 'wlrouter/zerowaitdfs.asp'));
			   insDoc(wlVisualNode, gLnk('R', getMenuTitle(MENU_WL_MENU_DEMO_ON_WBD), 'wlrouter/wbd_demo.asp'));
           }
	   } else {
		   if(numWl != '1')
			   wlanMenu = insFld(foldersTree, gFld(getMenuTitle(MENU_WIRELESS_SETTINGS), wlItemsCgiCmd[0]));

		   for(i = 0; i < parseInt(numWl); i++)
		   {
			   // Configure wireless menu
			   if(numWl == '1')
				   nodeWireless = insFld(foldersTree, gFld(getMenuTitle(MENU_WIRELESS_SETTINGS), wlItemsCgiCmd[0]));
			   else
				   nodeWireless = insFld(wlanMenu, gFld(wlmenuTitle[i], wlItemsCgiCmd[i]));

			   insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_BASIC), 'wlcfg.html'));
			   insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_SECURITY), 'wlsecurity.html'));
			   insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_MAC_FILTERING), 'wlmacflt.cmd?action=view'));
			   insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_WDS), 'wlwds.cmd?action=view'));
			   insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_ADVANCED), 'wlcfgadv.html'));
			   //SUPPORT_SES
			   if ( wireless_ses == '1' ) { 
				   insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_SES), 'wlses.html'));      
			   }

			   insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_STATION_LIST), 'wlstationlist.cmd'));
		   }

		   if ( wireless_wapi == '1' ) {      
			   if (numWl == '1') {
				   insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_WAPI_AS), 'wlwapias.html'));
			   }
			   else {
				   insDoc(wlanMenu, gLnk('R', getMenuTitle(MENU_WL_WAPI_AS), 'wlwapias.html'));
			   }
		   }

		   if(wlVisualization == '1') {
			   if (numWl == '1') {
				   wlVisualNode = insFld(nodeWireless, gFld(getMenuTitle(MENU_WL_VISUALIZATION), "wlrouter/configure.asp"));
			   } else {
				   wlVisualNode = insFld(wlanMenu, gFld(getMenuTitle(MENU_WL_VISUALIZATION), "wlrouter/configure.asp"));
			   }
			   insDoc(wlVisualNode, gLnk('R', getMenuTitle(MENU_WL_VISUALIZATION_SITESURVEY), 'wlrouter/visindex.asp'));
			   insDoc(wlVisualNode, gLnk('R', getMenuTitle(MENU_WL_VISUALIZATION_CHANNELSTAT), 'wlrouter/channelcapacity.asp'));
			   insDoc(wlVisualNode, gLnk('R', getMenuTitle(MENU_WL_VISUALIZATION_METRICS), 'wlrouter/metrics.asp'));
			   insDoc(wlVisualNode, gLnk('R', getMenuTitle(MENU_WL_VISUALIZATION_CONFIGURE), 'wlrouter/configure.asp'));
		   }

		   if(wlPasspoint == '1') {
			   if (numWl == '1') {
				   wlPassPointNode = insFld(nodeWireless, gFld(getMenuTitle(MENU_WL_PASSPOINT), "wlrouter/passpoint.asp"));
			   } else {
				   wlPassPointNode = insFld(wlanMenu, gFld(getMenuTitle(MENU_WL_PASSPOINT), "wlrouter/passpoint.asp"));
			   }
		   }
	   } 

   }

     /*Storage Service menu */
   if(storageservice == '1')
   {
      nodeStorage = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_STORAGESERVICE), 'storageservicecfg.cmd?view'));
      insDoc(nodeStorage, gLnk('R', getMenuTitle(MENU_STORAGE_INFO), 'storageservicecfg.cmd?view'));
      if(sambaservice == '1'){
         insDoc(nodeStorage, gLnk('R', getMenuTitle(MENU_STORAGE_USERACCOUNT), 'storageuseraccountcfg.cmd?view'));
      }
   }

   // Configure voice menu 

   if ( eptapp == '1' ) {
         nodeVoice = insFld(foldersTree, gFld(getMenuTitle(MENU_VOICE_SETTINGS), 'voiceeptapp.html'));
         insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_EPTAPP), 'voiceeptapp.html'));
   } 
   else if ( voiceTr104Option == '1' ) {
      nodeVoice = insFld(foldersTree, gFld(getMenuTitle(MENU_VOICE_SETTINGS), 'voicesip_basic.html'));
      insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_SIP_BASIC), 'voicesip_basic.html'));
      insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_SIP_ADVANCED), 'voicesip_advanced.html'));
      insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_SIP_DEBUG), 'voicesip_debug.html'));
      if(sipCctk == '1'){
         insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_SIP_CCTK), 'voicesip_cctk.html'));
      }
      if( voiceNtr != '2' ) {
         insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_NTR), 'voicentr.html'));
      }
      if( dect == '1' ) {
         insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_DECT), 'voicedect.html'));
      }
   }
   else if ( voiceTr104Option == '2' ) {
      nodeVoice = insFld(foldersTree, gFld(getMenuTitle(MENU_VOICE2_SETTINGS), 'voice2_basic.html'));
      insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE2_GLOBAL), 'voice2_basic.html'));
   } 

   // Configure DPI 
   if ( dpi == '1' && websockets == '1' )
      insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DPI), 'dpicharts.html'));

    // Configure diagnostics menu
   nodeDiagnostics = insFld(foldersTree, gFld(getMenuTitle(MENU_DIAGNOSTICS), 'diag.html'));

   // Configure management menu
   nodeMngr = insFld(foldersTree, gFld(getMenuTitle(MENU_MANAGEMENT), 'backupsettings.html'));
   nodeSettings = insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_SETTINGS), 'backupsettings.html'));
   insDoc(nodeSettings, gLnk('R', getMenuTitle(MENU_TL_SETTINGS_BACKUP),'backupsettings.html'));
   insDoc(nodeSettings, gLnk('R', getMenuTitle(MENU_TL_SETTINGS_UPDATE),'updatesettings.html'));
   insDoc(nodeSettings, gLnk('R', getMenuTitle(MENU_TL_SETTINGS_DEFAULT), 'defaultsettings.html'));

   if ( pmd_option == '1' )
      insDoc(nodeSettings, gLnk('R', getMenuTitle(MENU_TL_SETTINGS_PMD),'pmdsettings.cmd'));

   insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_SYSTEM_LOG), 'logintro.html'));
   if ( anywan )
      insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_SECURITY_LOG), 'seclogintro.html'));
   if ( snmp == '1' )
      insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_SNMP), 'snmpconfig.html'));
   if ( tr69c == '1' )
      insFld(nodeMngr, gFld(getMenuTitle(MENU_TR69C), 'tr69cfg.html'));
   if ( stun == '1' )
      insFld(nodeMngr, gFld(getMenuTitle(MENU_STUN), 'stuncfg.html'));
   if ( xmpp == '1' )
      insFld(nodeMngr, gFld(getMenuTitle(MENU_XMPP_CONN), 'xmppconncfg.cmd?action=view'));

   if ( omci == '1' ) {
      nodeOmci = insFld(nodeMngr, gFld(getMenuTitle(MENU_OMCI_CFG), 'omcicfg.cmd?action=view'));
      insDoc(nodeOmci, gLnk('R', getMenuTitle(MENU_OMCI_GET_SET),'omcicfg.cmd?action=view'));
      insDoc(nodeOmci, gLnk('R', getMenuTitle(MENU_OMCI_CREATE),'omcicreate.cmd?action=view'));
      insDoc(nodeOmci, gLnk('R', getMenuTitle(MENU_OMCI_GET_NEXT),'omcigetnext.cmd?action=view'));
      insDoc(nodeOmci, gLnk('R', getMenuTitle(MENU_OMCI_MACRO),'omcimacro.cmd?action=view'));
      insDoc(nodeOmci, gLnk('R', getMenuTitle(MENU_OMCI_DOWNLOAD),'omcidownload.html'));
      insDoc(nodeOmci, gLnk('R', getMenuTitle(MENU_OMCI_SYSTEM),'omcisystem.html'));
   }
   if ( sntp == '1' && proto != 'Bridge' && !(proto=='PPPoE' && ipExt=='1') && !(proto=='PPPoA' && ipExt=='1') )
      insFld(nodeMngr, gFld(getMenuTitle(MENU_SNTP), 'sntpcfg.html'));


   if ( modsw_webui == '1' && modsw_webui_support == '1' && ( modsw_baseline == '1' || modsw_linuxpfp == '1' ) ) {

      if ( modsw_baseline == '1' ) {
          nodeModSw = insFld(nodeMngr, gFld(getMenuTitle(MENU_MODSW), 'modSwEE.cmd'));
          insDoc(nodeModSw, gLnk('R', getMenuTitle(MENU_MODSW_EE),'modSwEE.cmd'));
          insDoc(nodeModSw, gLnk('R', getMenuTitle(MENU_MODSW_DU),'modSwDU.cmd'));
          insDoc(nodeModSw, gLnk('R', getMenuTitle(MENU_MODSW_EU), 'modSwEU.cmd'));

          if ( modsw_linuxpfp == '1' )
              insDoc(nodeModSw, gLnk('R', getMenuTitle(MENU_MODSW_PFP), 'modSwPFP.cmd'));

          if ( dbusRemote == '1' )
             insFld(nodeModSw, gFld(getMenuTitle(MENU_MODSW_DBUS_REMOTE), 'dbusremotecfg.html'));
      }
      else {
           nodeModSw = insFld(nodeMngr, gFld(getMenuTitle(MENU_MODSW), 'modSwPFP.cmd'));
           insDoc(nodeModSw, gLnk('R', getMenuTitle(MENU_MODSW_PFP), 'modSwPFP.cmd'));
      }

   }

   insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_UPDATE_SOFTWARE), 'upload.html'));
   insFld(nodeMngr, gFld(getMenuTitle(MENU_RESET_ROUTER), 'resetrouter.html'));
}

function menuUser() {
   var snmp = options[MENU_OPTION_SNMP];
   var tr69c = options[MENU_OPTION_TR69C];
   var omci = options[MENU_OPTION_OMCI];
   var xmpp = options[MENU_OPTION_XMPP];
   var stun = options[MENU_OPTION_STUN];
   
   // Configure diagnostics menu
   nodeDiagnostics = insFld(foldersTree, gFld(getMenuTitle(MENU_DIAGNOSTICS), 'diag.html'));

   // Configure management menu
   nodeMngr = insFld(foldersTree, gFld(getMenuTitle(MENU_MANAGEMENT), 'logintro.html'));
   insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_SYSTEM_LOG), 'logintro.html'));
   if ( snmp == '1' )
   insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_SNMP), 'snmpconfig.html'));
   if ( tr69c == '1' )
      insFld(nodeMngr, gFld(getMenuTitle(MENU_TR69C), 'tr69cfg.html'));
   if ( stun == '1' )
      insFld(nodeMngr, gFld(getMenuTitle(MENU_STUN), 'stuncfg.html'));
   if ( xmpp == '1' )
      insFld(nodeMngr, gFld(getMenuTitle(MENU_XMPP_CONN), 'xmppconncfg.cmd?action=view'));
   if ( omci == '1' ) {
      nodeOmci = insFld(nodeMngr, gFld(getMenuTitle(MENU_OMCI_CFG), 'omcicfg.cmd?action=view'));
      insDoc(nodeOmci, gLnk('R', getMenuTitle(MENU_OMCI_GET_SET),'omcicfg.cmd?action=view'));
      insDoc(nodeOmci, gLnk('R', getMenuTitle(MENU_OMCI_CREATE),'omcicreate.cmd?action=view'));
      insDoc(nodeOmci, gLnk('R', getMenuTitle(MENU_OMCI_GET_NEXT),'omcigetnext.cmd?action=view'));
      insDoc(nodeOmci, gLnk('R', getMenuTitle(MENU_OMCI_MACRO),'omcimacro.cmd?action=view'));
      insDoc(nodeOmci, gLnk('R', getMenuTitle(MENU_OMCI_DOWNLOAD),'omcidownload.html'));
      insDoc(nodeOmci, gLnk('R', getMenuTitle(MENU_OMCI_SYSTEM),'omcisystem.html'));
   }
   insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_UPDATE_SOFTWARE), 'upload.html'));
}

function createBcmMenu(options) {
   var user = options[MENU_OPTION_USER];
   var proto = options[MENU_OPTION_PROTOCOL];
   var ipExt = options[MENU_OPTION_IP_EXTENSION];
   var dhcpen = options[MENU_OPTION_DHCPEN];
   var jqplot = options[MENU_OPTION_JQPLOT];
   var websockets = options[MENU_OPTION_WEB_SOCKETS];
   var modsw_webui = options[MENU_OPTION_MODSW_WEBUI];
   var modsw_webui_admin = options[MENU_OPTION_MODSW_WEBUI_ADMIN];
   var modsw_baseline = options[MENU_OPTION_MODSW_BASELINE];
   var modsw_linuxpfp = options[MENU_OPTION_MODSW_LINUXPFP];
   var mocaStats = options[MENU_OPTION_SUPPORT_MOCA];
   var ptm = options[MENU_OPTION_PTMWAN];
   var atm = options[MENU_OPTION_ATMWAN];
   var mocawan = options[MENU_OPTION_MOCAWAN];
   var ethwan = options[MENU_OPTION_ETHWAN];
   var gponwan = options[MENU_OPTION_GPONWAN];
   var eponwan = options[MENU_OPTION_EPONWAN];
   var optical = options[MENU_OPTION_OPTICAL];
   var wifiwan = options[MENU_OPTION_WIFIWAN]; 
   var cellularwan = options[MENU_OPTION_SUPPORT_CELLULAR]; 
   var buildUsbHosts = options[MENU_OPTION_BUILD_USB_HOSTS];
   var qrcode = options[MENU_OPTION_QRCODE_SAMPLE];
   var statsqueue = options[MENU_OPTION_STATS_QUEUE];
   var anywan = (ptm == '1' || atm == '1' || mocawan == '1' ||
       ethwan == '1' || wifiwan == '1' || gponwan == '1' || eponwan == '1');
   
   foldersTree = gFld('', 'info.html');
   // device info menu
   nodeDeviceInfo = insFld(foldersTree, gFld(getMenuTitle(MENU_DEVICE_INFO), 'info.html'));
   // device summary menu
   insFld(nodeDeviceInfo, gFld(getMenuTitle(MENU_DEVICE_SUMMARY), 'info.html'));
   // device wan menu
   if (anywan)
      insFld(nodeDeviceInfo, gFld(getMenuTitle(MENU_DEVICE_WAN), 'wancfg.cmd?action=view'));
   // device statistics menu
   nodeSts = insFld(nodeDeviceInfo, gFld(getMenuTitle(MENU_STATISTICS), 'statsifc.html'));
   insDoc(nodeSts, gLnk('R', getMenuTitle(MENU_ST_LAN), 'statsifc.html'));
   if (mocaStats == '1')
      insDoc(nodeSts, gLnk('R', getMenuTitle(MENU_ST_MOCA), 'statsmoca.cmd?choice=LAN'));
   if (anywan)
      insDoc(nodeSts, gLnk('R', getMenuTitle(MENU_WAN), 'statswan.cmd'));
   if (ptm == '1' || atm == '1') {
      insDoc(nodeSts, gLnk('R', getMenuTitle(MENU_ST_ATM), 'statsxtm.cmd'));
      insDoc(nodeSts, gLnk('R', getMenuTitle(MENU_ST_ADSL), 'statsadsl.html'));
   }
   if (optical == '1')
      insDoc(nodeSts, gLnk('R', getMenuTitle(MENU_OPTICAL), 'statsopticifc.html'));
   if (statsqueue == '1')
      insDoc(nodeSts, gLnk('R', getMenuTitle(MENU_ST_QUEUE), 'statsqueue.html'));
   // device route menu
   if (anywan)
      insFld(nodeDeviceInfo, gFld(getMenuTitle(MENU_DEVICE_ROUTE), 'rtroutecfg.cmd?action=view'));
   insFld(nodeDeviceInfo, gFld(getMenuTitle(MENU_RT_ARP),'arpview.cmd'));
   // dhcp info
   if (!(proto == 'Bridge' || ipExt == '1') && dhcpen == '1') {
      insFld(nodeDeviceInfo, gFld(getMenuTitle(MENU_DHCPINFO),'dhcpinfo.html'));
   }
   // cpu & memory info
   if (jqplot == '1' && websockets == '1')
      insDoc(nodeDeviceInfo, gLnk('R', getMenuTitle(MENU_CPU_MEM_GRAPH), 'cpumemcharts.html'));
   // usb hosts info
   if (buildUsbHosts  == '1') {
      insFld(nodeDeviceInfo, gFld(getMenuTitle(MENU_USB_HOSTS),'usbhosts.cmd?action=view'));
   }  
   // container menu
   if ( ( modsw_webui == '1' || modsw_webui_admin == '1' ) &&
        ( modsw_baseline == '1' || modsw_linuxpfp == '1' ) )
      insDoc(nodeDeviceInfo, gLnk('R', getMenuTitle(MENU_MODSW_CONTAINER), 'containertable.html'));
   if (qrcode=='1') {
       insDoc(nodeDeviceInfo, gFld(getMenuTitle(MENU_QRCODE_SAMPLE), 'qrcodesample.html'));
   }
   // cellular info menu
   if (cellularwan == '1')
      insDoc(nodeDeviceInfo, gLnk('R', getMenuTitle(MENU_CELLULAR_INFO), 'cellularinfo.html'));

   if ( user == 'admin' )
      menuAdmin(options);
   else if ( user == 'support' )
      menuSupport(options);
   else if ( user == 'user' )
      menuUser();
}
