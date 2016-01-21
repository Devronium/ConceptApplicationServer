<html>
<head>
<title>{title}</title>
{literal}
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link href="http://ajax.googleapis.com/ajax/libs/jqueryui/1.8/themes/ui-darkness/jquery-ui.css" rel="stylesheet" type="text/css"/>
<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/prototype/1.6.1.0/prototype.js"></script>
<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/scriptaculous/1.8.3/scriptaculous.js?load=effects,builder"></script>
<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.4.2/jquery.min.js"></script>
<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jqueryui/1.8.5/jquery-ui.min.js"></script>

<link rel="stylesheet" type="text/css" href="css/flexigrid/flexigrid.css">
<script type="text/javascript" src="js/flexigrid.js"></script>

<script type="text/javascript">
	$(function() {
		$("#tabs").tabs();
	});
</script>

<script>
	$('.flexme').flexigrid();
</script>


{endliteral}
</head>
<body>

<div id="tabs">
	<ul>
{foreach button}
		<li><a href="{button.Href}List.csp" title="{button.Title}"><span>{button.Title}</span></a></li>
{endfor}
	</ul>
</div>

</body>
</html>
