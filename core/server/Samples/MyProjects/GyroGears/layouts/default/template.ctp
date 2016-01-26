<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>{title}</title>
{literal}
<style type="text/css">
	<!--
	a {
		color:#FFA0A0;
	}

	body {
		font-family:Arial, Helvetica, sans-serif;
		font-size:14px;
		color:#F2F2F2;
		background-color:#FFFFFF;
		background-repeat: no-repeat;
		padding:0;
	}

	.description {
		font-size:12px;
	}

	.status {
		font-size:12px;
		border:0px;
		color:#F0F0F0;
		border-style:solid;
		background-color:#020;
		box-shadow:5px 5px 10px #777; -webkit-box-shadow:5px 5px 10px #777; -moz-box-shadow:5px 5px 10px #000000; border-radius:25px; -moz-border-radius:25px; -webkit-border-radius:25px;
		padding:10px;
		width:720px;
	}

	.count {
		-webkit-border-radius: 999px;
		-moz-border-radius: 999px;
		border-radius: 999px;

		height: 18px;
		min-width:110px;
		padding: 8px;
    
		background: -webkit-gradient(linear, 0% 0%, 0% 100%, from(#202), to(#001));
		color: #DFD;
		text-align: right;
    
		font: 16px Arial, sans-serif
	}

	.entity {
		font-size:15px;
		font-weight:bold;
		border:0px;
		padding: 0px;
		color:#F0F0F0;
		border-style:solid;
		background: -webkit-gradient(linear, 0% 0%, 0% 100%, from(#222), to(#011));
		-webkit-border-radius:25px;
	}

	.entity:hover {
		background-color:#500;
		background: -webkit-gradient(linear, 0% 0%, 0% 100%, from(#333), to(#022));
	}

	.tabel {
		width:100%;
		border:0px;
	}

	.tabel_title {
		width:100%;
		opacity: 0.7;
		background-color:#003;
		border:0px;
		padding:10px;
		box-shadow:5px 5px 10px #777; -webkit-box-shadow:5px 5px 10px #777; -moz-box-shadow:5px 5px 10px #000000; border-radius:25px; -moz-border-radius:25px; -webkit-border-radius:25px;
	}

	.tabel2 {
		border: 1px solid #444244;
		opacity: 0.7;
		background-color:#003;
		width:720px;
		border:0px;
		padding:10px;
		box-shadow:5px 5px 10px #777; -webkit-box-shadow:5px 5px 10px #777; -moz-box-shadow:5px 5px 10px #000000; border-radius:25px; -moz-border-radius:25px; -webkit-border-radius:25px;
	}

	.tabel_chart {
		opacity: 0.7;
		background-color:#003;
		width:720px;
		border:0px;
		padding:10px;
		box-shadow:5px 5px 10px #777; -webkit-box-shadow:5px 5px 10px #777; -moz-box-shadow:5px 5px 10px #000000; border-radius:25px; -moz-border-radius:25px; -webkit-border-radius:25px;
	}

	img.back {
		position: fixed;
		left:0px;
		top:0px;
		width:0px;
		height:0px;
		display: none;
	}

	th {
		text-align:left;
		background: -webkit-gradient(linear, 0% 0%, 0% 100%, from(#222), to(#011));
	}

	a:hover {
		text-decoration:underline;
	}
	a:link {
		text-decoration:none;
	}
	a:active {
		text-decoration:none;
	}
	.courier {
		color:#999;
		width:100%;
	}
	.date {
		width:80%;
		color:#FFFFFF;
		padding-top:1px;
		padding-bottom:1px;
	}
</style>
{endliteral}
</head>

<body background="res/_home_background.jpg">
	<img src="res/_home_background.jpg" class="back"/>

<div class="shadow_container">
	<table class="tabel_title" border="0" cellpadding="0" cellspacing="0">
		<tr><th style="border-radius:25px;">
   	<h1><b>&nbsp;{title}</b></h1>
</th></tr>
<tr><td>
Application date: {application_time}, Client version: {client_version}
{if description}
<br /><br />&nbsp;&raquo; {description}<br/>
{endif}
        {if username}
	    <p class="courier">You are logged in as <b>{username}</b> with level {level}. Last login on {last_login}. <a href="internal://gopassword">Click here to change your password</a>.</p>
        {endif}
</td></tr>
	</table>
</div>
	<br/>
	<br/>
{foreach custom_statistics}
<table class="tabel_chart" border="0" cellpadding="2" cellspacing="0">
<tr>
<td width="1px;" valign="top">
<div id="shadow-container"><div class="shadow1"><div class="shadow2"><div class="shadow3"><div class="container">
{if custom_statistics.svg}
	{custom_statistics.svg}
{else}
	<img src="{custom_statistics.chartfilename}" alt="" />
{endif}
</div></div></div></div></div>
</td>
<td valign="top">
<div id="shadow-container"><div class="shadow1"><div class="shadow2"><div class="shadow3"><div class="container">
	<table class="tabel" border="0" cellpadding="2" cellspacing="0">
		<tr><th>{custom_statistics.entity}</th><th align="right" width="30px">Count</th></tr>
		{foreach custom_statistics.stats}
			<tr>
				<td style="border-bottom: 1px solid #777;">{custom_statistics.stats.description}</td>
				<td style="border-bottom: 1px solid #777;"width="30px" align="right">{custom_statistics.stats.total}</td>
			</tr>
		{endfor}
	</table>
</div></div></div></div></div>
</td>
</tr>
</table>
	<br/>
	<br/>
{endfor}

<table class="tabel2" cellpadding="2" cellspacing="0" width="100%">
        {foreach statistics}
        <tr>
            	<td class="entity">
	        <div class="count" style="float:left;">{statistics.count}</div>
		<div style="float:left; padding-left:10px; display: table-cell; vertical-align: middle; min-height: 32px;">
                {if statistics.has_view}
       	    	<a href="internal2://{statistics.has_view}"><b>{statistics.entity}</b></a>
		{else}
            	<b>{statistics.entity}</b>&nbsp;
                {endif}
		{if statistics.description}
			<br/>
			<span class="description">{statistics.description}</span>
		{else}
			<br/>
			<span class="description" style="color: #787">{statistics.count} record(s)</span>
		{endif}
		</div>
                </td>
          </tr>
	  <tr><td><div style="height: 1px;"></div></td></tr>
        {endfor}
</table>
<br/>
<br/>
		<table border="0" class="status">
			<tr><td>
			        <p class="courier">Statistics taken on {statistics_time} (<a href="internal://gorefresh">click here to refresh statistics</a>)</p>
			</td></tr>
            		<tr>
                		<td>This application was created using GyroGears 2.0 &copy;2008-2016 Devronium Applications. <br />For more details please visit <a href="http://www.devronium.com">http://www.devronium.com</a>. </td>
        		</tr>
		</table>
</body>
</html>
