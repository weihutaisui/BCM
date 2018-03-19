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
$ID$-->

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html lang="en">
<head>
<title>Broadcom Home Gateway Reference Design: SSID</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script language="JavaScript" type="text/javascript" src="overlib.js"></script>
<script language="JavaScript" type="text/javascript">
<!--

var staListWl0 = "<% wl_invite_list(); %>".split('\t');

var interval = 30;
var nvinterval = "<% nvram_get("wfi_refresh_interval"); %>" ;
var wfi = "<% nvram_match("wl_wfi_enable", "1", "1"); %>";

if(nvinterval)
{
	interval = parseInt(nvinterval);
}

function printTable()
{
	if(wfi == 1){
		for(var i=0; i<staListWl0.length; i++){
			if(staListWl0[i].length < 17){
				continue;
			}

			document.writeln("<tr align=\"center\" id=\"trRow" + i + "\">");
	    		document.writeln("<td class=\"label\">");

	    		document.writeln("<input type=\"submit\" style=\"height: 25px; width: 100px\" name=\"action\" value=\"Invite\" OnClick=\"invite(" + i + ");\">");

	   		document.writeln("</td>");
	   		document.writeln("<td class=\"label\">");

	   		if(staListWl0[i].length > 17){
	   			document.writeln(staListWl0[i].substring(17,(staListWl0[i].length)));
	   		}
	   		else{
	   			document.writeln ("&nbsp");
	   		}
	   		document.writeln("</td>");
	   		document.writeln("<td class=\"label\">");
	   		document.writeln(staListWl0[i].substring(0,17));
	   		document.writeln("</td>");
	   		document.writeln("</tr>");
 		}
 	}
	return;
}

function invite(index)
{

	if(staListWl0[index].length > 17){
		document.forms[0].invite_name.value = staListWl0[index].substring(17,(staListWl0[index].length));
	}
	else{
		document.forms[0].invite_name.value = "";
	}

	document.forms[0].invite_mac.value = staListWl0[index].substring(0,17);

}

function wfi_enable_change()
{
	if(document.forms[0].wl_wfi_enable.value == 1){
		//document.forms[0].wl_unit.disabled = 0;
		document.forms[0].wl_wfi_pinmode.disabled = 0;
		document.forms[0].refresh_button.disabled = 0;
	}
	else{
		//gray out
		//document.forms[0].wl_unit.disabled = 1;
		document.forms[0].wl_wfi_pinmode.disabled = 1;
		document.forms[0].refresh_button.disabled = 1;
	}
}

function autoRefresh()
{
	if(wfi == 1){
		setTimeout("doRefresh()", interval * 1000 );
	}
}

function doRefresh()
{

	document.forms[0].action.value = "Select";
	document.forms[0].submit();
}
function writeStrRefres()
{
	if(wfi == 1){
		document.writeln("Auto refreshing every 30 seconds");
	}
}

//-->

</script>
</head>

<body OnLoad="wfi_enable_change();autoRefresh();">
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>

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
	<span class="title">Wifi-Invite</span><br>
	<span class="subtitle">Allow Wifi-Invite enabled STAs to easily join your secured network.</span>
    </td>
  </tr>
</table>

<form method="post" action="invite.asp">
<input type="hidden" name="page" value="invite.asp">
<input type="hidden" name="invite_name" value=0>
<input type="hidden" name="invite_mac" value=0>

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
	  <% wl_list("INCLUDE_SSID" , "INCLUDE_VIFS"); %>
	</select>
    </td>
    <td>
	<button type="submit" name="action" value="Select">Select</button>
    </td>
  </tr>
     <tr>
    <th width="310"> &nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
    	<% wl_wfi_mode(); %>
    </td>
  </tr>
</table>

<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Enables or disable Wifi-Invite feature.', LEFT);"
	onMouseOut="return nd();">
	Wifi-Invite:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_wfi_enable" onchange="wfi_enable_change();">
	  <option value="1" <% nvram_match("wl_wfi_enable", "1", "selected"); %>>Enabled</option>
	  <option value="0" <% nvram_invmatch("wl_wfi_enable", "1", "selected"); %>>Disabled</option>
	</select>
    </td>
  </tr>
    <tr>
    <th width="310"
	onMouseOver="return overlib('Set the Wifi-Invite PIN Mode to auto or manual.', LEFT);"
	onMouseOut="return nd();">
	Wifi-Invite PIN Mode:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_wfi_pinmode">
	  <option value="0" <% nvram_match("wl_wfi_pinmode", "0", "selected"); %>>Auto</option>
	  <option value="1" <% nvram_match("wl_wfi_pinmode", "1", "selected"); %>>Manual</option>
	</select>
    </td>
  </tr>
  <tr>
    <th width="310">&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
    	<br>
	<input type="submit" id="action" name="action" value="Apply" style="height: 25px; width: 100px">
	<input type="reset" name="action" value="Cancel" style="height: 25px; width: 100px">
    </td>
  </tr>
</table>

<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Scan to find Wifi-Invite enabled STAs.', LEFT);"
	onMouseOut="return nd();">
	List Wifi-Invite enabled STAs:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<input type="button" name="refresh_button" value="Refresh" style="height: 25px; width: 100px" onClick="doRefresh()">
    </td>
        <td>&nbsp;&nbsp;</td>
    <td>
	<SCRIPT LANGUAGE="JavaScript">
		writeStrRefres();
	</SCRIPT>
    </td>
  </tr>
</table>

<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310" valign="top"
	onMouseOver="return overlib('The list of Wifi-Invite enabled STAs.', LEFT);"
	onMouseOut="return nd();">
	Wifi-Invite enabled STAs:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<table border="1" cellpadding="2" cellspacing="0">
	  <tr align="center" HEIGHT=35>
	    <td class="label" WIDTH=100 HEIGHT=35 align="center">Action</td>
	    <td class="label" WIDTH=100 align="center">Friendly Name</td>
	    <td class="label" WIDTH=100 align="center">MAC Address</td>
	    <!-- <td class="label" WIDTH=100 align="center">Status</td> -->
	  </tr>
	  <script language="JavaScript" type="text/javascript">
		printTable();
	  </script>
	</table>
    </td>
  </tr>
</table>

<p>
</form>

<p class="label">&#169;2001-2016 Broadcom Limited. All rights reserved. 54g and XPress are trademarks of Broadcom Limited.</p>

</body>
</html>
