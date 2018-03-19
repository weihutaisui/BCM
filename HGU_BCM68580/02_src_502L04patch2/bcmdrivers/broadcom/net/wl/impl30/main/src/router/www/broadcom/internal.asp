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

$Id: internal.asp,v 1.42 2011-01-11 18:43:43 $
-->

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html lang="en">
<head>
<title>Broadcom Home Gateway Reference Design: Internal</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script language="JavaScript" type="text/javascript" src="overlib.js"></script>
<script language="JavaScript" type="text/javascript">

<!--
function nfs_proto_change() {
/*
*/
}
//-->
</script>
</head>

<body onLoad="nfs_proto_change();">

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
	<span class="title">INTERNAL</span><br>
	<span class="subtitle">This screen is for Broadcom internal
	usage only.</span>
    </td>
  </tr>
</table>

<form method="post" action="internal.asp">
<input type="hidden" name="page" value="internal.asp">

<!--
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
  <tr>
    <th width="310">54g Only Mode:&nbsp;&nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_gmode">
	  <option value="1" <% nvram_invmatch("wl_gmode", "2", "selected"); %>>Disabled</option>
	  <option value="2" <% nvram_match("wl_gmode", "2", "selected"); %>>Enabled</option>
	</select>
    </td>
  </tr>
   <tr>
     <th width="310"
 	onMouseOver="return overlib('Sets whether system log messages will be saved in RAM for showing in web.', LEFT);"
 	onMouseOut="return nd();">
 	Syslog in RAM:&nbsp;&nbsp;
     </th>
     <td>&nbsp;&nbsp;</td>
     <td>
 	<select name="log_ram_enable">
 	  <option value="0" <% nvram_match("log_ram_enable", "0", "selected"); %>>Disabled</option>
 	  <option value="1" <% nvram_match("log_ram_enable", "1", "selected"); %>>Enabled</option>
 	</select>
     </td>
   </tr>
</table>

<!--
-->

<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <td width="310"></td>
    <td>&nbsp;&nbsp;</td>
    <td>
	<input type="submit" name="action" value="Apply">
	<input type="reset" name="action" value="Cancel">
	<input type="submit" name="action" value="Upgrade">
	<input type="submit" name="action" value="Stats">
	<% wl_radio_roam_option(); %>
    </td>
  </tr>
</table>

</form>

<p class="label">&#169;2001-2016 Broadcom Limited. All rights reserved. 54g and XPress are trademarks of Broadcom Limited.</p>

</body>
</html>
