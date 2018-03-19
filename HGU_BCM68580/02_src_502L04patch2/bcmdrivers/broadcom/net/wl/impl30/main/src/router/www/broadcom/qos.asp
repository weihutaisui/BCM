<!--
Copyright 2012, Broadcom Corporation
All Rights Reserved.

THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
$ID$-->

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html lang="en">
<head>
<title>Broadcom Home Gateway Reference Design: Qos</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script language="JavaScript" type="text/javascript" src="overlib.js"></script>
<script language="JavaScript" type="text/javascript">
<!--

var orules = "<% nvram_get("qos_orules"); %>"

var len;
var rules = orules.split('&#62');;
if(orules.length >1)
{
 len = rules.length;
}
else
{
	len = 0;
}

var rulesTowrite = "";
var editIndex;

// addr_type < addr < proto < port_type < port < ipp2p < L7 < bcount < class < desc

function printTable()
{
	var ruleItem;
	var j = 1;

	if(len <= 0)
		return;

	document.writeln("<tr>");
	document.writeln("<th width=\"310\" valign=\"top\" onMouseOver=\"return overlib(\'Current Qos Rules (Outbound).\', LEFT);\" onMouseOut=\"return nd();\"> ");
	document.writeln("Current Qos Rules (Outbound):&nbsp;&nbsp;</th>");
	document.writeln("<td>&nbsp;&nbsp;</td>");
	document.writeln("<td>");
	document.writeln("<table border=\"1\" cellpadding=\"2\" cellspacing=\"1\">");
	document.writeln("<tr align=\"center\" HEIGHT=30>");
	document.writeln("<td class=\"label\" WIDTH=30 HEIGHT=30 align=\"center\"><input type=\"checkbox\" name=\"checkAll\" id=\"checkAll\" onclick=\"chkAll();\"></td>");
	document.writeln("<td class=\"label\" WIDTH=40 align=\"center\">Rule No.</td>");
	document.writeln("<td class=\"label\" WIDTH=90 align=\"center\">Address Type</td>");
	document.writeln("<td class=\"label\" WIDTH=90 align=\"center\">Address</td>");
	document.writeln("<td class=\"label\" WIDTH=80 align=\"center\">Protocol</td>");
	document.writeln("<td class=\"label\" WIDTH=100 align=\"center\">Port Filter</td>");
	document.writeln("<td class=\"label\" WIDTH=90 align=\"center\">Port No.</td>");
	document.writeln("<td class=\"label\" WIDTH=50 align=\"center\">Class</td>");
	document.writeln("<td class=\"label\" WIDTH=150 align=\"center\">Description</td>");
	document.writeln("</tr>");

	for(var i=0; i<rules.length; i++)
	{
		if(rules[i].length <= 0)
			continue;

		ruleItem = rules[i].split('&#60');

		document.writeln("<tr align=\"center\">");
		document.writeln("<td class=\"label\" align=\"center\">");
		document.writeln("<input type=\"checkbox\" name=\"rule_" + i + "\" id=\"rule_" + i + "\">");
		document.writeln("</td>");
		document.writeln("<td class=\"label\" align=\"center\">");
		document.writeln(j);
		document.writeln("</td>");
		document.writeln("<td class=\"label\" align=\"center\">");
		if(ruleItem[0] == 1)
			document.writeln("Destination IP");
		else if(ruleItem[0] == 2)
			document.writeln("Source IP");
		else if(ruleItem[0] == 3)
			document.writeln("Source MAC");
		else
			document.writeln("Any");
		document.writeln("</td>");
		document.writeln("<td class=\"label\" align=\"center\">");
		if(ruleItem[1])
			document.writeln(ruleItem[1]);
		else
			document.writeln("&nbsp;");
		document.writeln("</td>");
		document.writeln("<td class=\"label\" align=\"center\">");
		//[-2, 'Any Protocol'],[-1,'TCP/UDP'],[6,'TCP'],[17,'UDP']];
		if(ruleItem[2] == -2)
			document.writeln("Any");
		else if(ruleItem[2] == -1)
			document.writeln("TCP/UDP");
		else if(ruleItem[2] == 6)
			document.writeln("TCP");
		else if(ruleItem[2] == 17)
			document.writeln("UDP");
		else
			document.writeln("&nbsp;");
		document.writeln("</td>");
		document.writeln("<td class=\"label\" align=\"center\">");
		if(ruleItem[3] == "d")
			document.writeln("Destination");
		else if(ruleItem[3] == "s")
			document.writeln("Source");
		else if(ruleItem[3] == "x")
			document.writeln("Destination or Source");
		else
			document.writeln("Any");
		document.writeln("</td>");
		document.writeln("<td class=\"label\" align=\"center\">");
		if(ruleItem[4])
			document.writeln(ruleItem[4]);
		else
			document.writeln("&nbsp;");
		document.writeln("</td>");

		document.writeln("<td class=\"label\" align=\"center\">");
		if(ruleItem[8] == 0)
			document.writeln("Highest");
		else if(ruleItem[8] == "1")
			document.writeln("High");
		else if(ruleItem[8] == "2")
			document.writeln("Medium");
		else if(ruleItem[8] == "3")
			document.writeln("Low");
		else if(ruleItem[8] == "4")
			document.writeln("Lowest");
		else
			document.writeln("&nbsp;");
		document.writeln("</td>");
		document.writeln("<td class=\"label\" align=\"center\">");
		if(ruleItem[9])
			document.writeln(ruleItem[9]);
		else
			document.writeln("&nbsp;");
		document.writeln("</td>");
   		document.writeln("</tr>");
   		j++;
   	}
   	document.writeln("</table>");
   	document.writeln("</td>");
   	document.writeln("</tr>");
	document.writeln("<tr>");
	document.writeln("<th width=\"310\">");
	document.writeln("</th>");
	document.writeln("<td>&nbsp;&nbsp;</td>");
	document.writeln("<td align=\"left\">");
	document.writeln("<INPUT type=\"button\" name=\"delete\" value=\"Delete Rule\" onclick=\"delRule();\" &nbsp;&nbsp;>");
	document.writeln("<INPUT type=\"button\" name=\"edit\" value=\"Edit Rule\" onclick=\"toEdit();\" >");
	document.writeln("</td>");
	document.writeln("</tr>");
}

function toEdit(index)
{
	var r = 0;
	var index;
	var ruleItem;

	for(i = 0; i < rules.length; i++)
	{
		chkid = "rule_" + i ;
		if(document.getElementById(chkid) && (document.getElementById(chkid).checked == true))
		{
			r++;
			index = i;
		}
	}
	if(r != 1)
	{
		alert("Please selet ONE rule to edit at a time.");
		return false;
	}

	element = document.getElementById("tabEdit").style;
	element.display='block';
	element1 = document.getElementById("tabAdd").style;
	element1.display='none';
	document.forms[1].edit.disabled = 1 ;

	ruleItem = rules[index].split('&#60');

	document.forms[1].ruleNo.value = "Rule No. " + (index + 1);
	document.forms[1].qos_ip_e.focus();

	document.forms[1].qos_ip_e.selectedIndex = ruleItem[0];
	document.forms[1].qos_ipaddr_e.value = ruleItem[1];

	if(ruleItem[2] == -1)
		document.forms[1].qos_proto_e.selectedIndex = 1;
	else if(ruleItem[2] == 6)
		document.forms[1].qos_proto_e.selectedIndex = 2;
	else if(ruleItem[2] == 17)
		document.forms[1].qos_proto_e.selectedIndex = 3;
	else
		document.forms[1].qos_proto_e.selectedIndex = 0;

	if(ruleItem[3] == "d")
		document.forms[1].qos_port_e.selectedIndex = 1;
	else if(ruleItem[3] == "s")
		document.forms[1].qos_port_e.selectedIndex = 2;
	else if(ruleItem[3] == "x")
		document.forms[1].qos_port_e.selectedIndex = 3;
	else
		document.forms[1].qos_port_e.selectedIndex = 0;

	document.forms[1].qos_port_num_e.value = ruleItem[4]
	document.forms[1].qos_class_e.selectedIndex = ruleItem[8];
	document.forms[1].qos_desc_e.value = ruleItem[9];

	editIndex = index;
}

function submitEdit()
{
	var v;

 	if(validateRule(1) == false)
 	{
 		return false;
 	}

	v = document.forms[1].qos_ip_e.value + "<";
	v = v + document.forms[1].qos_ipaddr_e.value + "<";
	v = v + document.forms[1].qos_proto_e.value + "<";
	if(document.forms[1].qos_port_e.value == 1)
		v = v + "d" + "<";
	else if(document.forms[1].qos_port_e.value == 2)
		v = v + "s" + "<";
	else if(document.forms[1].qos_port_e.value == 3)
		v = v + "x" + "<";
	else
		v = v + "a" + "<";
	v = v + document.forms[1].qos_port_num_e.value + "<" + "0<";

		v = v+ "<" + "<";

	v = v + document.forms[1].qos_class_e.value + "<";
	v = v + document.forms[1].qos_desc_e.value;

	// addr_type < addr < proto < port_type < port < < < < class < desc
	rulesTowrite = "";
	if(editIndex > 0)
	{
		for(i = 0; i < editIndex; i++)
		{
			if(rules[i].length > 0)
				witeRules(i);
		}
	}

	if(rulesTowrite.length > 1)
	{

		rulesTowrite = rulesTowrite + ">" + v;
	}
	else
	{
		rulesTowrite = v;
	}

	if( editIndex < (rules.length - 1))
	{
		for(i = (editIndex + 1); i < rules.length; i++)
		{
			if(rules[i].length > 0)
				witeRules(i);
		}
	}

	document.forms[1].qos_orules.value = rulesTowrite;
	document.forms[1].submit();
	return true;
}

function resetEdit()
{

	element = document.getElementById("tabEdit").style;
	element.display='none';
	document.forms[1].edit.disabled = 0 ;
	element1 = document.getElementById("tabAdd").style;
	element1.display='block';
}

function setMax()
{
	var orates = "<% nvram_get("qos_orates"); %>";
	var irates = "<% nvram_get("qos_irates"); %>";

	if(!orates)
	{
		orates = "80-100,10-100,5-100,3-100,2-95,0-0,0-0,0-00,0-0,0-0";
	}

	if(!irates)
	{
		irates = "0,0,0,0,5,0,0,0,0,0";
	}

	var rates = orates.split(',');

	ratehi = rates[0].split('-');
	document.forms[0].obw_hi_lower.value = ratehi[0];
	document.forms[0].obw_hi_higher.value = ratehi[1];

	rateh = rates[1].split('-');
	document.forms[0].obw_h_lower.value = rateh[0];
	document.forms[0].obw_h_higher.value = rateh[1];

	ratemed = rates[2].split('-');
	document.forms[0].obw_med_lower.value = ratemed[0];
	document.forms[0].obw_med_higher.value = ratemed[1];

	ratel = rates[3].split('-');
	document.forms[0].obw_l_lower.value = ratel[0];
	document.forms[0].obw_l_higher.value = ratel[1];

	ratelo = rates[4].split('-');
	document.forms[0].obw_lo_lower.value = ratelo[0];
	document.forms[0].obw_lo_higher.value = ratelo[1];

	var ratein = irates.split(',');

	document.forms[0].ibw_hi.value = ratein[0];
	document.forms[0].ibw_h.value = ratein[1];
	document.forms[0].ibw_med.value = ratein[2];
	document.forms[0].ibw_l.value = ratein[3];
	document.forms[0].ibw_lo.value = ratein[4];

}

function qos_recalc(input)
{
	var qos_enable = "<% nvram_get("qos_enable"); %>";
	var qos_obw = "<% nvram_get("qos_obw"); %>";
	var qos_ibw = "<% nvram_get("qos_ibw"); %>";

	if(input == 1)
	{
		qos_enable = document.forms[0].qos_enable.value;
		setItems(qos_enable);
	}

	else if(input == 2)
	{
		qos_obw1 = document.forms[0].qos_obw.value;
		setBW(qos_obw1);
	}

	else if(input == 3)
	{
		qos_ibw1 = document.forms[0].qos_ibw.value;
		setiBW(qos_ibw1);
	}

	else
	{
		setItems(qos_enable);
		setBW(qos_obw);
		setiBW(qos_ibw);
	}
}

function setBW(qos_obw)
{
	a = document.forms[0].obw_hi_lower.value/100;
	b = a*qos_obw;
	document.forms[0].hi_lo.value = Math.floor(b);

	a = document.forms[0].obw_hi_higher.value/100;
	b = a*qos_obw;
	document.forms[0].hi_hi.value = Math.floor(b);

	a = document.forms[0].obw_h_lower.value/100;
	b = a*qos_obw;
	document.forms[0].h_lo.value = Math.floor(b);

	a = document.forms[0].obw_h_higher.value/100;
	b = a*qos_obw;
	document.forms[0].h_hi.value = Math.floor(b);

	a = document.forms[0].obw_med_lower.value/100;
	b = a*qos_obw;
	document.forms[0].med_lo.value = Math.floor(b);

	a = document.forms[0].obw_med_higher.value/100;
	b = a*qos_obw;
	document.forms[0].med_hi.value = Math.floor(b);

	a = document.forms[0].obw_l_lower.value/100;
	b = a*qos_obw;
	document.forms[0].l_lo.value = Math.floor(b);

	a = document.forms[0].obw_l_higher.value/100;
	b = a*qos_obw;
	document.forms[0].l_hi.value = Math.floor(b);

	a = document.forms[0].obw_lo_lower.value/100;
	b = a*qos_obw;
	document.forms[0].lo_lo.value = Math.floor(b);

	a = document.forms[0].obw_lo_higher.value/100;
	b = a*qos_obw;
	document.forms[0].lo_hi.value = Math.floor(b);
}

function setiBW(qos_ibw)
{
	x = document.forms[0].ibw_hi.value/100;
	y = x*qos_ibw;
	document.forms[0].in_hi.value = Math.floor(y);

	x = document.forms[0].ibw_h.value/100;
	y = x*qos_ibw;
	document.forms[0].in_h.value = Math.floor(y);

	x = document.forms[0].ibw_med.value/100;
	y = x*qos_ibw;
	document.forms[0].in_med.value = Math.floor(y);

	x = document.forms[0].ibw_l.value/100;
	y = x*qos_ibw;
	document.forms[0].in_l.value = Math.floor(y);

	x = document.forms[0].ibw_lo.value/100;
	y = x*qos_ibw;
	document.forms[0].in_lo.value = Math.floor(y);

}

function setItems(b)
{
	if(b == 0)
	{
   		document.forms[0].qos_ack.disabled = 1;
		document.forms[0].qos_icmp.disabled = 1;

		document.forms[0].obw_hi_lower.disabled = 1;
		document.forms[0].obw_hi_higher.disabled = 1;
		document.forms[0].obw_h_lower.disabled = 1;
		document.forms[0].obw_h_higher.disabled = 1;
		document.forms[0].obw_med_lower.disabled = 1;
		document.forms[0].obw_med_higher.disabled = 1;
		document.forms[0].obw_l_lower.disabled = 1;
		document.forms[0].obw_l_higher.disabled = 1;
		document.forms[0].obw_lo_lower.disabled = 1;
		document.forms[0].obw_lo_higher.disabled = 1;
		document.forms[0].qos_obw.disabled = 1;

		document.forms[0].ibw_hi.disabled = 1;
		document.forms[0].ibw_h.disabled = 1;
		document.forms[0].ibw_med.disabled = 1;
		document.forms[0].ibw_l.disabled = 1;
		document.forms[0].ibw_lo.disabled = 1;
		document.forms[0].qos_ibw.disabled = 1;

		document.forms[0].qos_default.disabled = 1;

	}
	else
	{
		document.forms[0].qos_ack.disabled = 0;
		document.forms[0].qos_icmp.disabled = 0;

		document.forms[0].obw_hi_lower.disabled = 0;
		document.forms[0].obw_hi_higher.disabled = 0;
		document.forms[0].obw_h_lower.disabled = 0;
		document.forms[0].obw_h_higher.disabled = 0;
		document.forms[0].obw_med_lower.disabled = 0;
		document.forms[0].obw_med_higher.disabled = 0;
		document.forms[0].obw_l_lower.disabled = 0;
		document.forms[0].obw_l_higher.disabled = 0;
		document.forms[0].obw_lo_lower.disabled = 0;
		document.forms[0].obw_lo_higher.disabled = 0;
		document.forms[0].qos_obw.disabled = 0;

		document.forms[0].ibw_hi.disabled = 0;
		document.forms[0].ibw_h.disabled = 0;
		document.forms[0].ibw_med.disabled = 0;
		document.forms[0].ibw_l.disabled = 0;
		document.forms[0].ibw_lo.disabled = 0;
		document.forms[0].qos_ibw.disabled = 0;

		document.forms[0].qos_default.disabled = 0;

	}
}

function validateData()
{
	if(!validateNumber(document.forms[0].obw_hi_lower))
		return false;
	if(!validateNumber(document.forms[0].obw_hi_higher))
		return false;
	if(!validateNumber(document.forms[0].obw_h_lower))
		return false;
	if(!validateNumber(document.forms[0].obw_h_higher))
		return false;
	if(!validateNumber(document.forms[0].obw_med_lower))
		return false;
	if(!validateNumber(document.forms[0].obw_med_higher))
		return false;
	if(!validateNumber(document.forms[0].obw_l_lower))
		return false;
	if(!validateNumber(document.forms[0].obw_l_higher))
		return false;
	if(!validateNumber(document.forms[0].obw_lo_lower))
		return false;
	if(!validateNumber(document.forms[0].obw_lo_higher))
		return false;
	if(!validateRange(document.forms[0].obw_hi_lower,document.forms[0].obw_hi_higher))
		return false;
	if(!validateRange(document.forms[0].obw_h_lower,document.forms[0].obw_h_higher))
		return false;
	if(!validateRange(document.forms[0].obw_med_lower,document.forms[0].obw_med_higher))
		return false;
	if(!validateRange(document.forms[0].obw_l_lower,document.forms[0].obw_l_higher))
		return false;
	if(!validateRange(document.forms[0].obw_lo_lower,document.forms[0].obw_lo_higher))
		return false;
	if(!validateSum(document.forms[0].obw_hi_lower, document.forms[0].obw_h_lower, document.forms[0].obw_med_lower, document.forms[0].obw_l_lower, document.forms[0].obw_lo_lower))
		return false;

	var vhigh = document.forms[0].obw_hi_lower.value + "-" + document.forms[0].obw_hi_higher.value;
	var vh = document.forms[0].obw_h_lower.value + "-" + document.forms[0].obw_h_higher.value;
	var vmed = document.forms[0].obw_med_lower.value + "-" + document.forms[0].obw_med_higher.value;
	var vl = document.forms[0].obw_l_lower.value + "-" + document.forms[0].obw_l_higher.value;
	var vlow = document.forms[0].obw_lo_lower.value + "-" + document.forms[0].obw_lo_higher.value;

	document.forms[0].qos_orates.value = vhigh + "," + vh + "," + vmed + "," + vl + "," + vlow + ",0-0,0-0,0-0,0-0,0-0";

	var vhigh = document.forms[0].ibw_hi.value;
	var vh = document.forms[0].ibw_h.value;
	var vmed = document.forms[0].ibw_med.value;
	var vl = document.forms[0].ibw_l.value;
	var vlow = document.forms[0].ibw_lo.value;

	document.forms[0].qos_irates.value = vhigh + "," + vh + "," + vmed + "," + vl + "," + vlow + ",0,0,0,0,0";

	return true;
}

function validateSum(v1, v2, v3, v4, v5)
{
	var sum;
	sum = parseInt(v1.value) + parseInt(v2.value) + parseInt(v3.value) + parseInt(v4.value)+ parseInt(v5.value);

	if(sum > 100)
	{
		alert("The sum of all %BWMin can not be over 100!");
		v1.focus();
		v1.select();
		return false;
	}
	return true;
}

function validateNumber(val)
{
	if(!val.value)
	{
		alert("Please enter a number!");
		val.focus();
		val.select();
		return false;
	}

	for(i = 0; i < val.value.length; i++)
	{
		if(val.value.charAt(i) < '0' || val.value.charAt(i) > '9')
		{
			alert('Invalid number '+val.value.charAt(i)+' at position '+ (i+1)+' ! Must be 0123456789');
			val.focus();
			val.select();
			return false;
		}
	}

	if(val.value < 0 || val.value > 100)
	{
		alert("Invalid number, the number must be great than 0 and less than 100!");
		val.focus();
		val.select();
		return false;
	}

	return true;
}

function validateRange(val1, val2)
{
	if(parseInt(val1.value) > parseInt(val2.value))
	{
		alert("Invalid number, the %BWMin must not be great than %BWMax!");
		val1.focus();
		val1.select();
		return false;
	}
	return true;
}

function chkAll()
{
	var chk = 1;
	var chkid;

	if(document.getElementById("checkAll") && document.getElementById("checkAll").checked ==0){
		chk = 0;
	}
	for ( var i = 0; i < rules.length; i++)
	{
		chkid = "rule_" + i ;
		if(document.getElementById(chkid) && document.getElementById(chkid).checked == (!chk))
		{
			document.getElementById(chkid).click();
		}
	}
}

function delRule()
{
	var flag = 0;
	var ruleLeft = "";
	var i;
	var chkid;

	for(i = 0; i < rules.length; i++)
	{
		chkid = "rule_" + i ;
		if(document.getElementById(chkid) && (document.getElementById(chkid).checked == true))
		{
			flag = 1;
			break;
		}
	}

	if(!flag)
	{
		alert("No rule is selected. Please check the rules you want to delete.");
		return false;
	}

	rulesTowrite = "";
	for(i = 0; i < rules.length; i++)
	{
		chkid = "rule_" + i ;

		if(document.getElementById(chkid) && (document.getElementById(chkid).checked == false))
		{
			//alert("add rule #" + i);
			if(rules[i].length > 0)
				witeRules(i);
		}
	}

	document.forms[1].qos_orules.value = rulesTowrite;
	document.forms[1].submit();
	return true;
}

function addRule()
{
	var v;

 	if(validateRule(0) == false)
 	{
 		return false;
 	}

	v = document.forms[1].qos_ip.value + "<";
	v = v + document.forms[1].qos_ipaddr.value + "<";
	v = v + document.forms[1].qos_proto.value + "<";
	if(document.forms[1].qos_port.value == 1)
		v = v + "d" + "<";
	else if(document.forms[1].qos_port.value == 2)
		v = v + "s" + "<";
	else if(document.forms[1].qos_port.value == 3)
		v = v + "x" + "<";
	else
		v = v + "a" + "<";
	v = v + document.forms[1].qos_port_num.value + "<" + "0<";

		v = v + "<" + "<";

	v = v + document.forms[1].qos_class.value + "<";
	v = v + document.forms[1].qos_desc.value;

	// addr_type < addr < proto < port_type < port < < < < class < desc

	rulesTowrite = "";
	if(len > 0)
	{
		for(i = 0; i < rules.length; i++)
		{
			//alert("add rule #" + i);
			if(rules[i].length > 0)
				witeRules(i);
		}
		if(rulesTowrite.length > 1)
		{
			v = rulesTowrite + ">" + v;
		}
	}

	document.forms[1].qos_orules.value = v;
	//alert("document.forms[1].qos_orules.value = " + document.forms[1].qos_orules.value);
	document.forms[1].submit();
	return true;
}

function validateRule(val)
{
	//validate IP address or MAC address
	if(val == 0)
	{
		ipE = document.forms[1].qos_ip;
		ipaddrE = document.forms[1].qos_ipaddr;
		portE = document.forms[1].qos_port;
		portnumE = document.forms[1].qos_port_num;
	}
	else if(val == 1)
	{
		ipE = document.forms[1].qos_ip_e;
		ipaddrE = document.forms[1].qos_ipaddr_e;
		portE = document.forms[1].qos_port_e;
		portnumE = document.forms[1].qos_port_num_e;
	}

	if(ipE.value == 1 || ipE.value == 2)
	{
		if(!validateIp(ipaddrE.value))
		{
			ipaddrE.focus();
			ipaddrE.select();
			return false;
		}
	}
	else if(ipE.value == 3)
	{
		if (!validateMac(ipaddrE.value))
		{
			qos_ipaddrE.focus();
			qos_ipaddrE.select();
			return false;
		}
	}
	//validate Ports
	if(portE.value != 0)
	{
		if(!validatePorts(portnumE.value))
		{
			portnumE.focus();
			portnumE.select();
			return false;
		}
	}
	return true;
}

function witeRules(index)
{
	var ruleItem = rules[index].split('&#60');
	var aRule = "";

	//alert("index =" + index);
	aRule = ruleItem[0];
	for(var i = 1; i < ruleItem.length; i++)
	{
		aRule = aRule + "<" + ruleItem[i];
	}
	if(rulesTowrite.length > 1)
	{
		rulesTowrite = rulesTowrite + ">" + aRule;
	}
	else
	{
		rulesTowrite = aRule;
	}
}

function validateMac(val)
{
	var i;
	var sval = String(val);
	var strAlert = "Invalid MAC address! Please enter a valid MAC address.";

	if (sval.length != 17)
 	{
		alert(strAlert);
		return false;
	}

	for (i = 0; i < sval.length; i++)
	{
		if ((i % 3) == 2)
		{
			if (sval.charAt(i) != ':')
	 		{
	 			alert(strAlert);
            			return false;
            		}
        	}
        	else if (!((sval.charAt(i) >= '0' && sval.charAt(i) <= '9') ||
        		(sval.charAt(i) >= 'A' && sval.charAt(i) <= 'F') ||
        		(sval.charAt(i) >= 'a' && sval.charAt(i) <= 'f')))
		{
			alert(strAlert);
			return false;
		}
 	}

 	var sub = val.split(':');

	if(sub[0].toLowerCase() == 'ff' && sub[1].toLowerCase() == 'ff' && sub[2].toLowerCase() == 'ff' &&
		sub[3].toLowerCase() == 'ff' && sub[4].toLowerCase() == 'ff' && sub[5].toLowerCase() == 'ff')
	{
		alert(strAlert);
		return false;
	}
	else if(sub[0] == '00' && sub[1] == '00' && sub[2] == '00' && sub[3] == '00' && sub[4] == '00' && sub[5] == '00')
	{
		alert(strAlert);
		return false;
	}

 	sub[0] = '0x'+sub[0];
 	if((sub[0] & 1) == 1)
 	{
		alert(strAlert);
		return false;
	}
	return true;
}

function validateIp(val)
{
	var i;
	sval = String(val);
	var strAlert = "Invalid IP address! Please enter a valid IP address.";

	if(val.length < 7 || val.length > 15)
	{
		alert(strAlert);
		return false;
	}

	for (i = 0; i < sval.length; i++)
	{
		if(sval.charAt(i) == '.')
		{
			continue;
		}
		else if(sval.charAt(i)<'0' || sval.charAt(i)>'9')
		{
			alert(strAlert);
			return false;
		}
	}

	var sub = val.split('.');
	if(sub.length != 4)
	{
		alert(strAlert);
		return false;
	}

	if(sub[0].length == 0 || sub[1].length == 0 || sub[2].length == 0 || sub[3].length == 0)
	{
		alert(strAlert);
		return false;
	}

	if((sub[0]-0) < 0 || (sub[0]-0) > 255 || (sub[1]-0) < 0 || (sub[1]-0) > 255 ||
		(sub[2]-0) < 0 || (sub[2]-0) > 255 || (sub[3]-0) < 0 || (sub[3]-0) > 255)
	{
		alert(strAlert);
		return false;
	}
	return true;
}

function validatePorts(val)
{
	var i;
	sval = String(val);
	var strAlert = "Invalid Ports! Please enter valid Ports, the correct format is \"Port[,Port]...\".";

	if(val.length == 0)
		return true;

	for (i = 0; i < sval.length; i++)
	{
		if(sval.charAt(i) == ',')
		{
			continue;
		}
		else if(sval.charAt(i)<'0' || sval.charAt(i)>'9')
		{
			alert(strAlert);
			return false;
		}
	}

	var sub = val.split(',');
	for(i = 0; i < sub.length; i++ )
	{
		if((sub[i]-0) < 0 || (sub[i]-0) > 65535)
		{
			alert(strAlert);
			return false;
		}
	}
	return true;
}

//-->
</script>
</head>

<body onLoad="setMax();qos_recalc(0);">
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
	<span class="title">Qos</span><br>
	<span class="subtitle">This page allows you to configure the new Broadcom IQos feature.</span>
    </td>
  </tr>
</table>

<form name="set" method="post" action="apply.cgi" onsubmit="return validateData();">
<input type="hidden" name="page" value="qos.asp">
<input type="hidden" name="qos_orates" value="<% nvram_get("qos_orates"); %>">
<input type="hidden" name="qos_irates" value="<% nvram_get("qos_irates"); %>">
<p>
<p>

<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Enables the Qos feature.', LEFT);"
	onMouseOut="return nd();">
	Enable Qos:&nbsp;&nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="qos_enable" onChange="qos_recalc(1);">
	  <option value="0" <% nvram_match("qos_enable", "0", "selected"); %>>Disabled</option>
	  <option value="1" <% nvram_match("qos_enable", "1", "selected"); %>>Enabled</option>
	</select>
	</td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Prioritizes the transmit ACK packets.', LEFT);"
	onMouseOut="return nd();">
	Prioritize ACK:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="qos_ack">
	  <option value="0" <% nvram_match("qos_ack", "0", "selected"); %>>Disabled</option>
	  <option value="1" <% nvram_match("qos_ack", "1", "selected"); %>>Enabled</option>
	</select>
    </td>
  </tr>

  <tr>
    <th width="310"
	onMouseOver="return overlib('Prioritizes the ICMP packets (PING replies, etc)..', LEFT);"
	onMouseOut="return nd();">
	Prioritize ICMP:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="qos_icmp">
	  <option value="0" <% nvram_match("qos_icmp", "0", "selected"); %>>Disabled</option>
	  <option value="1" <% nvram_match("qos_icmp", "1", "selected"); %>>Enabled</option>
	</select>
    </td>
  </tr>
  </table>

<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('IQos default traffic class for unclassified traffic.', LEFT);"
	onMouseOut="return nd();">
	Default Traffic Class:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="qos_default">
	  <option value="0" <% nvram_match("qos_default", "0", "selected"); %>>Highest</option>
	  <option value="1" <% nvram_match("qos_default", "1", "selected"); %>>High</option>
	  <option value="2" <% nvram_match("qos_default", "2", "selected"); %>>Medium</option>
	  <option value="3" <% nvram_match("qos_default", "3", "selected"); %>>Low</option>
	  <option value="4" <% nvram_match("qos_default", "4", "selected"); %>>Lowest</option>
	</select>
    </td>
  </tr>
</table>

<p>

<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Inbound traffic classes.', LEFT);"
	onMouseOut="return nd();" height="21">
	Inbound classes (% Max Input BW)&nbsp;&nbsp;
	</th>
    <td>&nbsp;&nbsp;</td>
    <td></td>
  </tr>

  <tr>
    <th width="310"
	onMouseOver="return overlib('Sets the maximum bw for inbound traffic.', LEFT);"
	onMouseOut="return nd();">
	BW Max inbound:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
    <input name="qos_ibw" value="<% nvram_get("qos_ibw"); %>" onChange="qos_recalc(3);" size="6" maxlength="6">
    Kbit/s
    </td>
  </tr>
  <tr>
    <th width="310">
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
    <font size="1" color="#0000FF">%BW</font></td>
  </tr>
    </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Highest Priority settings in percent of max BW', LEFT);"
	onMouseOut="return nd();">
	Highest:&nbsp;&nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<input name="ibw_hi" size="3" maxlength="3" onChange="qos_recalc(3);"> &nbsp;&nbsp;&nbsp;
        <INPUT maxLength=40 size=3 name=in_hi style="text-align: center; border: 1px solid #FFFFFF; padding: 0"> Kbit/s
    </td>
  </tr>
    <tr>
    <th width="310"
	onMouseOver="return overlib('High Priority settings in percent of max BW', LEFT);"
	onMouseOut="return nd();">
	High:&nbsp;&nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<input name="ibw_h" size="3" maxlength="3" onChange="qos_recalc(3);"> &nbsp;&nbsp;&nbsp;
        <INPUT maxLength=40 size=3 name=in_h style="text-align: center; border: 1px solid #FFFFFF; padding: 0"> Kbit/s
    </td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Medium Priority settings in percent of max BW', LEFT);"
	onMouseOut="return nd();">
	Medium:&nbsp;&nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<input name="ibw_med"  size="3" maxlength="3" onChange="qos_recalc(3);"> &nbsp;&nbsp;&nbsp;
        <INPUT maxLength=40 size=3 name=in_med style="text-align: center; border: 1px solid #FFFFFF; padding: 0"> Kbit/s
    </td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Low Priority settings in percent of max BW', LEFT);"
	onMouseOut="return nd();">
	Low:&nbsp;&nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<input name="ibw_l" size="3" maxlength="3" onChange="qos_recalc(3);"> &nbsp;&nbsp;&nbsp;
        <INPUT maxLength=40 size=3 name=in_l style="text-align: center; border: 1px solid #FFFFFF; padding: 0"> Kbit/s</td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Lowest Priority settings in percent of max BW', LEFT);"
	onMouseOut="return nd();">
	Lowest:&nbsp;&nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<input name="ibw_lo" size="3" maxlength="3" onChange="qos_recalc(3);"> &nbsp;&nbsp;&nbsp;
        <INPUT maxLength=40 size=3 name=in_lo style="text-align: center; border: 1px solid #FFFFFF; padding: 0"> Kbit/s</td>
  </tr>
</table>

<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Outbound traffic classes.', LEFT);"
	onMouseOut="return nd();" height="21">
	Outbound classes (% Max Output BW)&nbsp;&nbsp;
	</th>
    <td>&nbsp;&nbsp;</td>
    <td></td>
  </tr>

  <tr>
    <th width="310"
	onMouseOver="return overlib('Sets the maximum bw for outbound traffic.', LEFT);"
	onMouseOut="return nd();">
	BW Max outbound:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
    <input name="qos_obw" value="<% nvram_get("qos_obw"); %>" onChange="qos_recalc(2);" size="6" maxlength="6">
    Kbit/s
    </td>
  </tr>
  <tr>
    <th width="310">
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
    <font size="1" color="#0000FF">%BWMin&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; %BWMax</font>
    </td>
  </tr>
    </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Highest Priority settings in percent of max BW', LEFT);"
	onMouseOut="return nd();">
	Highest:&nbsp;&nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<input name="obw_hi_lower" size="3" maxlength="3" onChange="qos_recalc(2);">
	<input name="obw_hi_higher" size="3" maxlength="3" onChange="qos_recalc(2);">&nbsp;&nbsp;&nbsp;
        <INPUT maxLength=40 size=3 name=hi_lo style="text-align: center; border: 1px solid #FFFFFF; padding: 0">
        --
        <INPUT maxLength=40 size=3 name=hi_hi style="text-align: center; border: 1px solid #FFFFFF; padding: 0">Kbit/s
    </td>
  </tr>
    <tr>
    <th width="310"
	onMouseOver="return overlib('High Priority settings in percent of max BW', LEFT);"
	onMouseOut="return nd();">
	High:&nbsp;&nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<input name="obw_h_lower" size="3" maxlength="3" onChange="qos_recalc(2);">
	<input name="obw_h_higher" size="3" maxlength="3" onChange="qos_recalc(2);">&nbsp;&nbsp;&nbsp;
        <INPUT maxLength=40 size=3 name=h_lo style="text-align: center; border: 1px solid #FFFFFF; padding: 0">
        --
        <INPUT maxLength=40 size=3 name=h_hi style="text-align: center; border: 1px solid #FFFFFF; padding: 0">Kbit/s
    </td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Medium Priority settings in percent of max BW', LEFT);"
	onMouseOut="return nd();">
	Medium:&nbsp;&nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<input name="obw_med_lower"  size="3" maxlength="3" onChange="qos_recalc(2);">
	<input name="obw_med_higher" size="3" maxlength="3" onChange="qos_recalc(2);">&nbsp;&nbsp;&nbsp;
        <INPUT maxLength=40 size=3 name=med_lo style="text-align: center; border: 1px solid #FFFFFF; padding: 0">
        --
       <INPUT maxLength=40 size=3 name=med_hi style="text-align: center; border: 1px solid #FFFFFF; padding: 0">Kbit/s
    </td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Low Priority settings in percent of max BW', LEFT);"
	onMouseOut="return nd();">
	Low:&nbsp;&nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<input name="obw_l_lower" size="3" maxlength="3" onChange="qos_recalc(2);">
	<input name="obw_l_higher" size="3" maxlength="3" onChange="qos_recalc(2);">&nbsp;&nbsp;&nbsp;
        <INPUT maxLength=40 size=3 name=l_lo style="text-align: center; border: 1px solid #FFFFFF; padding: 0">
        --
        <INPUT maxLength=40 size=3 name=l_hi style="text-align: center; border: 1px solid #FFFFFF; padding: 0">Kbit/s</td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Lowest Priority settings in percent of max BW', LEFT);"
	onMouseOut="return nd();">
	Lowest:&nbsp;&nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<input name="obw_lo_lower" size="3" maxlength="3" onChange="qos_recalc(2);">
	<input name="obw_lo_higher" size="3" maxlength="3" onChange="qos_recalc(2);">&nbsp;&nbsp;&nbsp;
        <INPUT maxLength=40 size=3 name=lo_lo style="text-align: center; border: 1px solid #FFFFFF; padding: 0">
        --
        <INPUT maxLength=40 size=3 name=lo_hi style="text-align: center; border: 1px solid #FFFFFF; padding: 0">Kbit/s</td>
  </tr>
</table>

<table border="0" cellpadding="0" cellspacing="0">
    <tr>
      <td width="310"></td>
      <td></td>
      <td>
       <input type="submit" name="action" value="Apply">
       <input type="button" value="Cancel" onClick="window.location.reload();">
      </td>
    </tr>
</table>
</form>

<form name="adddel" method="post" action="apply.cgi">
<input type="hidden" name="page" value="qos.asp">
<input type="hidden" name="qos_orules" value="<% nvram_get("qos_orules"); %>">
<input type="hidden" name="action" value="Apply">

<!-- add table -->
<div style="display:block;" id="tabAdd">
<p>
<table border="0" cellpadding="0" cellspacing="0">
    <tr>
    <th width="310"
	onMouseOver="return overlib('Add Qos Rule (Outbound). The rules are evaluated top to bottom, as shown in the GUI. The *first* one that matches sets the class.', LEFT);"
	onMouseOut="return nd();" height="21">
	Add Qos Rule (Outbound)&nbsp;&nbsp;
	</th>
    <td>&nbsp;&nbsp;</td>
    <td></td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('IQos IP/MAC Address Filter Setting', LEFT);"
	onMouseOut="return nd();">
	IP/MAC Address Filter:&nbsp;&nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="qos_ip">
	<option value="0">Any</option>
	<option value="1">Destination IP</option>
	<option value="2">Source IP</option>
	<option value="3">Source MAC</option>
  	</select>
	&nbsp; Address: <input name="qos_ipaddr" size="17" maxlength="17"></td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Protocol Filter', LEFT);"
	onMouseOut="return nd();">
	Protocol Filter:</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="qos_proto">
	<option value="-2">Any</option>
	<option value="-1">TCP/UDP</option>
	<option value="6">TCP</option>
	<option value="17">UDP</option>
	</select>
   </td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('IQos Port Filter Setting', LEFT);"
	onMouseOut="return nd();" >
	Port Filter:&nbsp;&nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="qos_port">
	<option value="0">Any</option>
	<option value="1">Destination Port</option>
	<option value="2">Source Port</option>
	<option value="3">Source or Destination</option>
	</select>
	&nbsp; Port List: <input name="qos_port_num" size="16" maxlength="16">
    </td>
  </tr>

    <tr>
    <th width="310"
	onMouseOver="return overlib('Set the IQos Classes', LEFT);"
	onMouseOut="return nd();">
	Class Assigned:&nbsp;&nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="qos_class">
	  <option value="0">Highest</option>
	  <option value="1">High</option>
	  <option value="2">Medium</option>
	  <option value="3">Low</option>
	  <option value="4">Lowest</option>
	</select>
    </td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Description of the rule', LEFT);"
	onMouseOut="return nd();">
	Description:&nbsp;&nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<input name="qos_desc" size="32" maxlength="32">
    </td>
  </tr>
  <tr>
    <th width="310">
    </th>
    <td></td>
    <td>
	<INPUT type="button" value="Add Rule" onclick="addRule();">
    </td>
   </tr>
</table>
</div>

<!-- edit table -->
<div style="display:none;" id="tabEdit">
<p>
<table border="0" cellpadding="0" cellspacing="0" >
    <tr>
    <th width="310"
	onMouseOver="return overlib('Edit/Make changes to the IQos Rules (Outbound).', LEFT);"
	onMouseOut="return nd();" height="21">
	Edit Qos Rule (Outbound)&nbsp;&nbsp;
	</th>
    <td>&nbsp;&nbsp;</td>
    <td align=left>
    	<INPUT maxLength="20" size="12" name="ruleNo" style="text-align: center; border: 1px solid #FFFFFF; padding: 0">
    </td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('IQos IP/MAC Address Filter Setting', LEFT);"
	onMouseOut="return nd();">
	IP Address Filter:&nbsp;&nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="qos_ip_e">
	<option value="0">Any</option>
	<option value="1">Destination IP</option>
	<option value="2">Source IP</option>
	<option value="3">Source MAC</option>
  	</select>
	&nbsp; Address: <input name="qos_ipaddr_e" size="17" maxlength="17"></td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('IQos Protocol Filter - TCP, UDP, Any etc.', LEFT);"
	onMouseOut="return nd();">
	Protocol Filter:</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="qos_proto_e">
	<option value="-2">Any</option>
	<option value="-1">TCP/UDP</option>
	<option value="6">TCP</option>
	<option value="17">UDP</option>
	</select>
   </td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Port Filter Setting', LEFT);"
	onMouseOut="return nd();" >
	Port Filter:&nbsp;&nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="qos_port_e">
	<option value="0">Any</option>
	<option value="1">Destination Port</option>
	<option value="2">Source Port</option>
	<option value="3">Source or Destination</option>
	</select>
	&nbsp; Port List: <input name="qos_port_num_e" size="16" maxlength="16">
    </td>
  </tr>

  <tr>
    <th width="310"
	onMouseOver="return overlib('Qos Class Setting', LEFT);"
	onMouseOut="return nd();">
	Class Assigned:&nbsp;&nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="qos_class_e">
	  <option value="0">Highest</option>
	  <option value="1">High</option>
	  <option value="2">Medium</option>
	  <option value="3">Low</option>
	  <option value="4">Lowest</option>
	</select>
    </td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Description of the rule', LEFT);"
	onMouseOut="return nd();">
	Description:&nbsp;&nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<input name="qos_desc_e" size="32" maxlength="32">
    </td>
  </tr>
  <tr>
    <th width="310">
    </th>
    <td></td>
    <td>
     <INPUT type="button" name="subEdit" value="Submit Edit" onclick="submitEdit();">&nbsp;
     <INPUT type="button" name="resEdit" value="Reset Edit" onclick="resetEdit();">
    </td>
   </tr>
</table>
</div>

<p>
<table border="0" cellpadding="0" cellspacing="0">
<script language="JavaScript" type="text/javascript">
	printTable();
</script>
</table>
</form>

<p class="label">&#169;2001-2016 Broadcom Limited. All rights reserved. 54g and XPress are trademarks of Broadcom Limited.</p>

</body>
</html>
