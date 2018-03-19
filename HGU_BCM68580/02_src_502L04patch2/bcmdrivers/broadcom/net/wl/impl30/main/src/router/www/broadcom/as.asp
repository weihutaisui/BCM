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

$Id: as.asp,v 1.5 2011-01-11 18:43:43 $
-->

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html lang="en">
<head>
<title>Broadcom Home Gateway Reference Design: As</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script language="JavaScript" type="text/javascript" src="overlib.js"></script>
<script language=javascript type='text/javascript'>

function showDiv() {
	var as_mode='<% nvram_get("as_mode"); %>';

	if (as_mode=="enabled") {
		document.getElementById('download_div').style.display="";
	}
	else {
		document.getElementById('download_div').style.display="none";
	}
}

function verify_user_info() {
	var user_id = document.forms[2].cer_owner.value;
	var cer_duration = document.forms[2].cer_period.value;
	var idx;
	var ch;

	if (user_id.length == 0) {
		alert("Owner Name Empty !!");
		return;
	}

	if (cer_duration.length == 0) {
		alert("Valid Period Empty !!\n");
		return;
	}
	else {
		for (idx = 0; idx < cer_duration.length; idx ++) {
			ch = cer_duration.charAt(idx);

			if (!(ch >= "0" && ch <= "9")) {
				alert("Valid Period Invalid !!\n");
				return;
			}
		}
	}

	document.forms[2].submit();
}

</script>
</head>

<body onLoad="showDiv();">
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
	<span class="title">AS</span><br>
	<span class="subtitle">This page allows you to configure the built-in
	WAPI Authentication Server(AS).</span>
    </td>
  </tr>
</table>
<form method="post" action="apply.cgi">
<input type="hidden" name="page" value="as.asp">
<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
    onMouseOver="return overlib('Enable/Disable X.509 Authentication Server', LEFT);"
    onMouseOut="return nd();">
    AS Configuration:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
      <select name="as_mode">
        <option value="disabled">Disabled</option>
        <option value="enabled" <% nvram_match("as_mode", "enabled", "selected"); %>>Enabled</option>
      </select>
    </td>
  </tr>
  <tr>
    <td width="310"></td>
    <td>&nbsp;&nbsp;</td>
    <td>
      <input type=submit name="action" value="Apply">
    </td>
  </tr>
</table>
</form>

<div id="download_div">
<form method="link" action="as_x509_cert_dl.cgi">
<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Push button to download AS X.509 certification to file', LEFT);"
	onMouseOut="return nd();">
	Export AS X.509 Certificate:&nbsp;&nbsp;
	</th>
	<td>&nbsp;&nbsp;</td>
	<td><input type="submit" value="Export AS Certificate"></td>
  </tr>
</table>
</form>
<form method="post" name="user_cert_form" action="user_x509_cert_dl.cgi">
<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Sets the owner name for this certificate.', LEFT);"
	onMouseOut="return nd();">
	Owner Name:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><input name="cer_owner" size="24" maxlength="32"></td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Sets the vaild period for this certificate.', LEFT);"
	onMouseOut="return nd();">
	Vaild Period:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><input name="cer_period" size="16" maxlength="5"></td>
    <td><select name="cer_period_unit">
	  <option value="1" "selected">DAY</option>
	  <option value="30" >MONTH</option>
  	  <option value="365" >YEAR</option>
	</select>
	</td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Below push button to download user X.509 certification to file', LEFT);"
	onMouseOut="return nd();">
	Issue User X.509 Certificate:&nbsp;&nbsp;
	</th>
	<td>&nbsp;&nbsp;</td>
	<td><input type="button" value="Issue User Certificate" onClick="verify_user_info()"></td>
  </tr>
</table>
</form>
</div>

<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310" valign="top"
	onMouseOver="return overlib('As published certificates list.', LEFT);"
	onMouseOut="return nd();">
	Certificate List:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<table border="1" cellpadding="2" cellspacing="0">
	  <tr align="center">
	    <td class="label">Certificate Owner</td>
	    <td class="label">Serial Number</td>
	    <td class="label">Duration</td>
	    <td class="label">Remain</td>
	    <td class="label">Type</td>
	    <td class="label">Status</td>
	    <td class="label">Action</td>
	  </tr>
	  <% as_cer_display(); %>
	</table>
    </td>
  </tr>
</table>

<p class="label">&#169;2001-2016 Broadcom Limited. All rights reserved. 54g and XPress are trademarks of Broadcom Limited.</p>

</body>
</html>
