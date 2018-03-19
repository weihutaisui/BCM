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

$Id: firmware.asp,v 1.28 2011-01-11 18:43:43 $
-->

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html lang="en">
<head>
<title>Broadcom Home Gateway Reference Design: Firmware</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script language="JavaScript" type="text/javascript" src="overlib.js"></script>
</head>

<body>
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
	<span class="title">FIRMWARE</span><br>
	<span class="subtitle">This page allows you to upgrade the
	firmware.</span>
    </td>
  </tr>
</table>
<form method="post" action="upgrade.cgi" enctype="multipart/form-data">
<input type="hidden" name="page" value="firmware.asp">

<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Displays the current version of Boot Loader.', LEFT);"
	onMouseOut="return nd();">
	Boot Loader Version:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><% nvram_get("pmon_ver"); %></td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Displays the current version of OS.', LEFT);"
	onMouseOut="return nd();">
	OS Version:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><% nvram_match("os_name", "linux", "Linux"); nvram_match("os_name", "vx", "VxWorks"); nvram_match("os_name", "eCos", "eCos"); %> <% kernel_version(); %> <% nvram_get("os_version"); %></td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Displays the current version of Wireless Driver.', LEFT);"
	onMouseOut="return nd();">
	WL Driver Version:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><% nvram_get("wl_version"); %></td>
  </tr>
</table>

<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Selects the new firmware to upload to the router.', LEFT);"
	onMouseOut="return nd();">
	New Firmware:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><input type="file" name="file"></td>
  </tr>
</table>

<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <td width="310"></td>
    <td>&nbsp;&nbsp;</td>
    <td><input type="submit" value="Upload new Firmware"></td>
  </tr>
</table>

 </form>
	<form method="link" action="nvramdl.cgi">
	<p>
		<table border="0" cellpadding="0" cellspacing="0">
  		 <tr>
    			<th width="310"
			onMouseOver="return overlib('Push button to save NVRAM variables to file', LEFT);"
			onMouseOut="return nd();">
			Download NVRAM file:&nbsp;&nbsp;
    			</th>
    		<td>&nbsp;&nbsp;</td>
    		<td><input type="submit" value="Save NVRAM to file"></td>
  		 </tr>
 		</table>
</form>

<form method="post" action="nvramul.cgi" enctype="multipart/form-data">
<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Enter filename of saved NVRAM file here.', LEFT);"
	onMouseOut="return nd();">
	Upload NVRAM file:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><input type="file" name="nvfile"></td>
  </tr>
</table>
<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <td width="310"></td>
    <td>&nbsp;&nbsp;</td>
    <td><input type="submit" value="Upload saved NVRAM file"></td>
  </tr>
</table>

</form>

<p class="label">&#169;2001-2016 Broadcom Limited. All rights reserved. 54g and XPress are trademarks of Broadcom Limited.</p>

</body>
</html>
