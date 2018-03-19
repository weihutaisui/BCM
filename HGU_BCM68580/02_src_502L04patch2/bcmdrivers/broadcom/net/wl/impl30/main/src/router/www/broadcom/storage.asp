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

$Id: storage.asp 667548 2016-10-27 14:50:33Z $
-->

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html lang="en">
<head>
<title>Broadcom Home Gateway Reference Design: Storage</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script language="JavaScript" type="text/javascript" src="overlib.js"></script>
<script language="JavaScript" type="text/javascript">

function dlna_dms_enable_change()
{
	if (document.forms[0].dlna_dms_enable.value == "0") {
		document.forms[0].dlna_dms_dir.disabled = 1;
	}
	else {
		document.forms[0].dlna_dms_dir.disabled = 0;
	}
}

function samba_mode_change()
{
	if (document.forms[0].samba_mode.value != "1") {
		document.forms[0].samba_passwd.disabled = 1;
	}
	else {
		document.forms[0].samba_passwd.disabled = 0;
	}
}

function storage_update()
{
	dlna_dms_enable_change();
	samba_mode_change();
}

</script>
</head>

<body onLoad="storage_update();">
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
	<span class="title">Storage</span><br>
	<span class="subtitle">This page displays Storage tools.</span>
    </td>
  </tr>
</table>

<form method="post" action="storage.asp">
<input type="hidden" name="page" value="storage.asp">

<!--
#ifdef __CONFIG_CIFS__
-->
<% disk_display(); %>
<!--
#endif // endif
-->

<!--
#ifdef __CONFIG_DLNA_DMS__
-->
<p>
<table border="0" cellpadding="0" cellspacing="0">
<tr>
<th width="310"
onMouseOver="return overlib('Sets whether DLNA Server is enabled.', LEFT);"
onMouseOut="return nd();">
DLNA Server:&nbsp;&nbsp;
</th>
<td>&nbsp;&nbsp;</td>
<td>
<select name="dlna_dms_enable" onChange=dlna_dms_enable_change();>
<option value="1" <% nvram_match("dlna_dms_enable", "1", "selected"); %>>Enabled</option>
<option value="0" <% nvram_match("dlna_dms_enable", "0", "selected"); %>>Disabled</option>
</select>
</td>
</tr>
<tr>
<th width="310"
onMouseOver="return overlib('Tell DLNA Server which directory is going to scan.', LEFT);"
onMouseOut="return nd();">
Content Directory:&nbsp;&nbsp;
</th>
<td>&nbsp;&nbsp;</td>
<td>
<input type="text" value="<% get_mnt_path(); %>" name="dlna_dms_dir">
</td>
</tr>
</table>
<!--
#endif // endif
-->

<!--
#ifdef __CONFIG_DLNA_DMR__
-->
<p>
<table border="0" cellpadding="0" cellspacing="0">
<tr>
<th width="310"
onMouseOver="return overlib('Sets whether DLNA Renderer is enabled.', LEFT);"
onMouseOut="return nd();">
DLNA Renderer:&nbsp;&nbsp;
</th>
<td>&nbsp;&nbsp;</td>
<td>
<select name="dlna_dmr_enable">
<option value="1" <% nvram_match("dlna_dmr_enable", "1", "selected"); %>>Enabled</option>
<option value="0" <% nvram_match("dlna_dmr_enable", "0", "selected"); %>>Disabled</option>
</select>
</td>
</tr>
</table>
<!--
#endif // endif
-->

<!--
#ifdef __CONFIG_SAMBA__
-->
<p>
<table border="0" cellpadding="0" cellspacing="0">
<tr>
<th width="310"
onMouseOver="return overlib('Sets Samba mode.', LEFT);"
onMouseOut="return nd();">
Samba Mode:&nbsp;&nbsp;
</th>
<td>&nbsp;&nbsp;</td>
<td>
<select name="samba_mode" onChange=samba_mode_change();>
<option value="0" <% nvram_match("samba_mode", "0", "selected"); %>>Disabled</option>
<option value="1" <% nvram_match("samba_mode", "1", "selected"); %>>User</option>
<option value="2" <% nvram_match("samba_mode", "2", "selected"); %>>Share</option>
</select>
</td>
</tr>

<tr>
<th width="310"
onMouseOver="return overlib('Samba user name.', LEFT);"
onMouseOut="return nd();">
User Name:&nbsp;&nbsp;
</th>
<td>&nbsp;&nbsp;</td>
<td>
<p>admin</p>
</td>
</tr>

<tr>
<th width="310"
onMouseOver="return overlib('Sets Samba password for remote connection.', LEFT);"
onMouseOut="return nd();">
Password:&nbsp;&nbsp;
</th>
<td>&nbsp;&nbsp;</td>
<td>
<input type="password" name="samba_passwd" value="<% nvram_get("samba_passwd"); %>">
</td>
</tr>
</table>
<!--
#endif // endif
-->

<p>
<table border="0" cellpadding="0" cellspacing="0">
    <tr>
      <td width="310"></td>
      <td>&nbsp;&nbsp;</td>
      <td>
	  <input type=submit name="action" value="Apply">
	  <input type=reset name="action" value="Cancel">
      </td>
    </tr>
</table>

</form>

<p class="label">&#169;2001-2016 Broadcom Limited. All rights reserved. 54g and XPress are trademarks of Broadcom Limited.</p>

</body>
</html>
