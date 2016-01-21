<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
<title>Please log in</title>
{literal}
<style type="text/css">
body,p,a,span,div,input,legend,h1,h2,h3,h4,h5,h6,li,dd,dt,th,td{
font-family:Arial, Helvetica, sans-serif;
}
body,p,a,span,div,input,legend,li,dd,dt,th,td{
font-size:10pt;
}
#loginform {
width:300px;
margin:auto;
}
#loginform fieldset{
padding:10px;
}
#loginform legend{
font-weight:bold;
font-size:9pt;
}
#loginform label{
display:block;
height:2em;
background-color:#E7E7E7;
padding:10px 10px 0;
}
#loginform input {
margin-right:20px;
border:1px solid #999999;
float:right;
clear:right;
background:#CCCCCC;
}
#loginform input:focus,#loginform input:hover {
border:1px solid #333333;
}
.error{
color:red;
font-weight:bold;
}
#loginform p {
color:#F00000;
}
</style>
<script type="text/javascript" src="js/md5.js"></script>
<script type="text/javascript" language="JavaScript">
// <!-- begin script
function encrypt() {
		var s = document.loginform.password.value;
		var s = hex_md5(s);
		document.loginform.password.value =s;
}
//	end script -->
</script>
{endliteral}
</head>
<body>

<form id="loginform" name="loginform" method="post" action="">
<fieldset>
<legend>Log in</legend>

<p>{RESPONSE}</p>
  <label for="username"><input type="text" name="username" tabindex="1" id="username" />username:
 </label>
    <label for="password"><input type="password" name="password" tabindex="2" id="password" />password:
    </label>
<label for="remember_me"><input type="checkbox" name="remember" value="1" tabindex="3" id="remember" />Remember me ?
    </label>
    <input name="__REDIRECT" type="hidden" value="{REDIRECT}" />
	<label for="submit">
    <input name="Submit" type="submit" id="submit" tabindex="4" value="Log in" onclick="encrypt();"/>
	</label>
 </fieldset>
</form>
</body>
</html>
