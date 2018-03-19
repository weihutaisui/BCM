<!--
Copyright (C) 2017, Broadcom. All Rights Reserved.

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

$Id: iptv.asp 332154 2012-05-09 08:41:22Z $
-->

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html lang="en">
<head>
<title>Broadcom Home Gateway Reference Design: Media</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script language="JavaScript" type="text/javascript" src="overlib.js"></script>
<script language="JavaScript" type="text/javascript">
<!--
function acs_fcs_mode_change()
{
	set_fcs_items(document.forms[0].wl_acs_fcs_mode.value == 1);
	acs_dfs_change();
}

function acs_dfs_change()
{
	set_dfs_items((document.forms[0].wl_acs_fcs_mode.value == 1) &&
		(document.forms[0].wl_acs_dfs.value == 2));
}

function set_dfs_items(b)
{
	// b=1 means "enable". Need to reverse that to set ".disabled".
	b = !b;

	document.forms[0].wl_acs_dfsr_immediate_sec.disabled	= b;
	document.forms[0].wl_acs_dfsr_deferred_sec.disabled	= b;
	document.forms[0].wl_acs_dfsr_activity_sec.disabled	= b;
	document.forms[0].wl_acs_dfsr_immediate_thr.disabled	= b;
	document.forms[0].wl_acs_dfsr_deferred_thr.disabled	= b;
	document.forms[0].wl_acs_dfsr_activity_thr.disabled	= b;
}

function set_fcs_items(b)
{
	b = !b;

	document.forms[0].wl_acs_dfs.disabled = b;
	document.forms[0].wl_acs_cs_scan_timer.disabled = b;
	document.forms[0].wl_acs_ci_scan_timer.disabled = b;
	document.forms[0].wl_acs_ci_scan_timeout.disabled = b;
	document.forms[0].wl_acs_scan_entry_expire.disabled = b;
	document.forms[0].wl_acs_tx_idle_cnt.disabled = b;
	document.forms[0].wl_acs_chan_dwell_time.disabled = b;
	document.forms[0].wl_acs_chan_flop_period.disabled = b;
	document.forms[0].wl_intfer_period.disabled = b;
	document.forms[0].wl_intfer_cnt.disabled = b;
	document.forms[0].wl_intfer_txfail.disabled = b;
	document.forms[0].wl_intfer_tcptxfail.disabled = b;

}

function bsd_role_change()
{
	if((document.forms[0].bsd_role.value != "1") && (document.forms[0].bsd_role.value != "2")) {
		document.forms[0].bsd_primary.disabled = 1;
		document.forms[0].bsd_pport.disabled = 1;
		document.forms[0].bsd_helper.disabled = 1;
		document.forms[0].bsd_hport.disabled = 1;
	}
	else {
		document.forms[0].bsd_primary.disabled = 0;
		document.forms[0].bsd_pport.disabled = 0;
		document.forms[0].bsd_helper.disabled = 0;
		document.forms[0].bsd_hport.disabled = 0;
	}

}

function form_load()
{
	var acs_fcs	= ("<% nvram_get("wl_acs_fcs_mode"); %>" == "1");	// 1 = FCS
	var dfs_reentry	= ("<% nvram_get("wl_acs_dfs"); %>" == "2");		// 2 = DFS Reentry

	set_fcs_items(acs_fcs);
	set_dfs_items(acs_fcs && dfs_reentry);
	<!-- DSLCPE add showhide -->
	show_hide_header("<% nvram_get("hide_hnd_header");%>");
}
//-->
</script>
</head>

<body onload=form_load()>
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>
<!--DSLCPE added page_header -->
<div id="page_header">
<table border="0" cellpadding="0" cellspacing="0" width="100%" bgcolor="#cc0000">
  <% asp_list(); %>
</table>

<table border="0" cellpadding="0" cellspacing="0" width="100%">
  <tr>
    <td colspan="2" class="edge"><img border="0" src="blur_new.jpg" alt=""></td>
  </tr>
  <tr>
    <td><img border="0" src="logo_new.gif" alt=""></td>
    <td width="100%" valign="top">
	<br>
	<span class="title">Media</span><br>
	<span class="subtitle">This page allows you to configure the
	basic Media related parameters.</span>
    </td>
  </tr>
</table>
</div>
<!-- DSLCEP add page_title div -->
<div id="page_title" style="display:none">
	<br>
	<span class="title">Media</span><br>
	<span class="subtitle">This page allows you to configure the
	basic Media related parameters.</span>
	<br>
</div>
<form method="post" action="media.asp">
<input type="hidden" name="page" value="media.asp">

<p>
<!-- DSLCPE add bca_cpe_hide to table -->
<table class="bca_cpe_hide" border="0" cellpadding="0" cellspacing="0">
    <tr>
    <th width="310"
        onMouseOver="return overlib('Enable IGMP Proxy in AP mode: 0: disable; 1: enable', LEFT);"
        onMouseOut="return nd();">
        Enable IGMP Proxy: &nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><select name="igmp_enable" value="<% nvram_get("igmp_enable"); %>">
		<option value="0" <% nvram_match("igmp_enable", "0", "selected"); %>>Disable</option>
		<option value="1" <% nvram_match("igmp_enable", "1", "selected"); %>>Enable</option>
	</td>
  </tr>
</table>
<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
        onMouseOver="return overlib('Role of BandSteer Daemon 0:disable; 1:primary; 2:helper; 3:standalone', LEFT);"
        onMouseOut="return nd();">
        BandSteering Daemon : &nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><select name="bsd_role" value="<% nvram_get("bsd_role"); %>" onChange="bsd_role_change();">
		<option value="0" <% nvram_match("bsd_role", "0", "selected"); %>>Disable</option>
		<option value="1" <% nvram_match("bsd_role", "1", "selected"); %>>Primary</option>
		<option value="2" <% nvram_match("bsd_role", "2", "selected"); %>>Helper</option>
		<option value="3" <% nvram_match("bsd_role", "3", "selected"); %>>Standalone</option>
	</td>
  </tr>
</table>
<table border="0" cellpadding="0" cellspacing="0">
 <tr>
    <th width="310">
	BSD Role Config:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>

    <td><B>IPAddr</B></td>
    <td>&nbsp;&nbsp;</td>
    <td><B>Port Number</B></td>
  </tr>

  <tr>
    <th width="310"
        onMouseOver="return overlib('BSD(Helper) Addr and Port', LEFT);"
        onMouseOut="return nd();">
        Helper Addr&Port: &nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><input name="bsd_helper" value="<% nvram_get("bsd_helper"); %>" size="15" maxlength="15"</td>
    <td>&nbsp;&nbsp;</td>
    <td><input name="bsd_hport" value="<% nvram_get("bsd_hport"); %>" size="5" maxlength="5"</td>
	</td>
  </tr>
  <tr>
    <th width="310"
        onMouseOver="return overlib('BSD(Primary) Addr and Port', LEFT);"
        onMouseOut="return nd();">
        Primary Addr&Port: &nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><input name="bsd_primary" value="<% nvram_get("bsd_primary"); %>" size="15" maxlength="15"</td>
    <td>&nbsp;&nbsp;</td>
    <td><input name="bsd_pport" value="<% nvram_get("bsd_pport"); %>" size="5" maxlength="5"</td>
	</td>
  </tr>
</table>

<!--
#if defined(TRAFFIC_MGMT_RSSI_POLICY)
-->
<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Selects which wireless interface to configure.', LEFT);"
	onMouseOut="return nd();">
	Wireless Interface:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_unit" onChange="submit();">
	  <% wl_list(); %>
	</select>
    </td>
  </tr>
</table>
<!--
#endif // endif
-->

<p>
<table border="0" cellpadding="0" cellspacing="0">
    <tr>
    <th width="310"
	onMouseOver="return overlib('Provide airtime fairness between multiple links', LEFT);"
	onMouseOut="return nd();">
	Airtime Fairness:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_atf">
	  <option value="0" <% nvram_match("wl_atf", "0", "selected"); %>>Disable</option>
	  <option value="1" <% nvram_match("wl_atf", "1", "selected"); %>>Enable</option>
	</select>
    </td>
  </tr>
</table>

<p>
<table border="0" cellpadding="0" cellspacing="0">
    <tr>
    <th width="310"
        onMouseOver="return overlib('Stalled Link Detection Threshold, 0 to disable', LEFT);"
        onMouseOut="return nd();">
        Stalled Link Detection Threshold: &nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
      <input name="wl_pspretend_threshold" value="<% nvram_get("wl_pspretend_threshold"); %>" size="8" maxlength="8">
    </td>
  </tr>
    <tr>
    <th width="310"
        onMouseOver="return overlib('Set PsPretend retry limit, 0 to disable', LEFT);"
        onMouseOut="return nd();">
        Packet Saving Retry Limit: &nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
      <input name="wl_pspretend_retry_limit" value="<% nvram_get("wl_pspretend_retry_limit"); %>" size="8" maxlength="8">
    </td>
  </tr>
</table>

<!--
#ifdef __CONFIG_EMF__
-->
<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Convert multicast IGMP mac packet to unicast mac packet.', LEFT);"
	onMouseOut="return nd();">
	Unicast IGMP Query:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_wmf_ucigmp_query">
	  <option value="0" <% nvram_match("wl_wmf_ucigmp_query", "0", "selected"); %>>Disable</option>
	  <option value="1" <% nvram_match("wl_wmf_ucigmp_query", "1", "selected"); %>>Enable</option>
	</select>
    </td>
  </tr>
</table>
<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Filter (Drop) IGMP Query to host alway on AP', LEFT);"
	onMouseOut="return nd();">
	Filter IGMP Query:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_wmf_igmpq_filter">
	  <option value="0" <% nvram_match("wl_wmf_igmpq_filter", "0", "selected"); %>>Disable</option>
	  <option value="1" <% nvram_match("wl_wmf_igmpq_filter", "1", "selected"); %>>Enable</option>
	</select>
    </td>
  </tr>
</table>
<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Send (Sendup) multicast packets from EMF/WMF to host always', LEFT);"
	onMouseOut="return nd();">
	Multicast Data Sendup:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_wmf_mdata_sendup">
	  <option value="0" <% nvram_match("wl_wmf_mdata_sendup", "0", "selected"); %>>Disable</option>
	  <option value="1" <% nvram_match("wl_wmf_mdata_sendup", "1", "selected"); %>>Enable</option>
	</select>
    </td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Send multicast packets to PSTA', LEFT);"
	onMouseOut="return nd();">
	Send multicast packets to PSTA:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_wmf_psta_disable">
	  <option value="0" <% nvram_match("wl_wmf_psta_disable", "0", "selected"); %>>Enable</option>
	  <option value="1" <% nvram_match("wl_wmf_psta_disable", "1", "selected"); %>>Disable</option>
	</select>
    </td>
  </tr>
</table>
<!--
#endif // endif
-->

<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('ACS channel switch mode', LEFT);"
	onMouseOut="return nd();">
	ACS Mode:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_acs_fcs_mode" onChange="acs_fcs_mode_change();">
	  <option value="0" <% nvram_match("wl_acs_fcs_mode", "0", "selected"); %>>SCS</option>
	  <option value="1" <% nvram_match("wl_acs_fcs_mode", "1", "selected"); %>>FCS</option>
	</select>
    </td>
  </tr>

  <tr>
    <th width="310"
	onMouseOver="return overlib('Enable/Disable DFS channel selection as first channel', LEFT);"
	onMouseOut="return nd();">
	DFS Channel Selection:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_acs_dfs" onChange="acs_dfs_change();">
	  <option value="0" <% nvram_match("wl_acs_dfs", "0", "selected"); %>>Disable</option>
	  <option value="1" <% nvram_match("wl_acs_dfs", "1", "selected"); %>>Enable</option>
	  <option value="2" <% nvram_match("wl_acs_dfs", "2", "selected"); %>>DFS Reentry</option>
	</select>
    </td>
  </tr>

  <tr>
    <th width="310"
	onMouseOver="return overlib('Time interval in seconds between two consecutive CS scans', LEFT);"
	onMouseOut="return nd();">
	CS Scan Interval:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
      <input name="wl_acs_cs_scan_timer" value="<% nvram_get("wl_acs_cs_scan_timer"); %>" size="8" maxlength="8">
    </td>
  </tr>

  <tr>
    <th width="310"
	onMouseOver="return overlib('Time interval in seconds between two consecutive CI scans', LEFT);"
	onMouseOut="return nd();">
	CI Scan Interval:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
      <input name="wl_acs_ci_scan_timer" value="<% nvram_get("wl_acs_ci_scan_timer"); %>" size="8" maxlength="8">
    </td>
  </tr>

  <tr>
    <th width="310"
	onMouseOver="return overlib('How long (timeout) in seconds without CS/CI scan', LEFT);"
	onMouseOut="return nd();">
	CI Scan Timeout:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
      <input name="wl_acs_ci_scan_timeout" value="<% nvram_get("wl_acs_ci_scan_timeout"); %>" size="8" maxlength="8">
    </td>
  </tr>

  <tr>
    <th width="310"
	onMouseOver="return overlib('Scan results aging time (in seconds)', LEFT);"
	onMouseOut="return nd();">
	Scan Result Expiry:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
      <input name="wl_acs_scan_entry_expire" value="<% nvram_get("wl_acs_scan_entry_expire"); %>" size="8" maxlength="8">
    </td>
  </tr>

  <tr>
    <th width="310"
	onMouseOver="return overlib('TX frame rate threshold for IDLE state checking used to trigger CS scan', LEFT);"
	onMouseOut="return nd();">
	TX IDLE Frame Rate:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
      <input name="wl_acs_tx_idle_cnt" value="<% nvram_get("wl_acs_tx_idle_cnt"); %>" size="8" maxlength="8">
    </td>
  </tr>

  <tr>
    <th width="310"
	onMouseOver="return overlib('Minimum time (in seconds), it has to stay in current channel before switch-out', LEFT);"
	onMouseOut="return nd();">
	Chan Dwell Time:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
      <input name="wl_acs_chan_dwell_time" value="<% nvram_get("wl_acs_chan_dwell_time"); %>" size="8" maxlength="8">
    </td>
  </tr>

  <tr>
    <th width="310"
	onMouseOver="return overlib('Time interval (in seconds) to re-select same channel', LEFT);"
	onMouseOut="return nd();">
	Chan FLOP Period:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
      <input name="wl_acs_chan_flop_period" value="<% nvram_get("wl_acs_chan_flop_period"); %>" size="8" maxlength="8">
    </td>
  </tr>
</table>

<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Interference Sample Period(in sec)', LEFT);"
	onMouseOut="return nd();">
	Sample Period:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
      <input name="wl_intfer_period" value="<% nvram_get("wl_intfer_period"); %>" size="8" maxlength="8">
    </td>
  </tr>

  <tr>
    <th width="310"
	onMouseOver="return overlib('Sample Count(Number Of Samples)', LEFT);"
	onMouseOut="return nd();">
	Sample Count:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
      <input name="wl_intfer_cnt" value="<% nvram_get("wl_intfer_cnt"); %>" size="8" maxlength="8">
    </td>
  </tr>

  <tr>
    <th width="310"
	onMouseOver="return overlib('Non-TCP Stream TxFail Threshold', LEFT);"
	onMouseOut="return nd();">
	Non-TCP Stream TxFail Threshold:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
      <input name="wl_intfer_txfail" value="<% nvram_get("wl_intfer_txfail"); %>" size="8" maxlength="8">
    </td>
  </tr>

  <tr>
    <th width="310"
	onMouseOver="return overlib('TCP Stream TxFail Threshold', LEFT);"
	onMouseOut="return nd();">
	TCP Stream TxFail Threshold:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
      <input name="wl_intfer_tcptxfail" value="<% nvram_get("wl_intfer_tcptxfail"); %>" size="8" maxlength="8">
    </td>
  </tr>
</table>

<p>
<% dfs_reentry_display(); %>

<!-- DSLCPE hide starts -->
<div class="bca_cpe_hide">
<% wet_tunnel_display(); %>

<!--
#ifdef TRAFFIC_MGMT_RSSI_POLICY
-->
<% trf_mgmt_rssi_policy_display(); %>
<!--
#endif // endif
-->

<% trf_mgmt_dwm_display(); %>

<% trf_mgmt_display(); %>

<!-- DSLCPE hide ends-->
</div>
<p>
<table border="0" cellpadding="0" cellspacing="0">
    <tr>
      <td width="310"></td>
      <td>&nbsp;&nbsp;</td>
      <td>
	  <input type="submit" name="action" value="Apply">
	  <input type="reset" name="action" value="Cancel">
      </td>
    </tr>
</table>

</form>

<p class="label">&#169;2001-2016 Broadcom Limited. All rights reserved. 54g and XPress are trademarks of Broadcom Limited.</p>

</body>
</html>
