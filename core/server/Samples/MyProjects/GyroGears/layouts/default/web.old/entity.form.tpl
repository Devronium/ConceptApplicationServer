<html>
<head>
<title>{title}</title>
{ldelim}literal{rdelim}
{literal}
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link rel="stylesheet" href="css/entity.css" type="text/css" title='main' media="screen" />
<link rel="stylesheet" href="css/tabs.css" type="text/css" title='main' media="screen" />
<link rel="stylesheet" href="js/jquery/themes/ui.datepicker.css" type="text/css" title='main' media="screen" />
<script type="text/javascript" src="js/jquery/jquery-1.3.1.min.js"></script>
<script type="text/javascript" src="js/jquery/jquery.idTabs.min.js"></script>
<script type="text/javascript" src="js/jquery/ui/ui.core.js"></script>
<script type="text/javascript" src="js/jquery/ui/ui.datepicker.js"></script>
<script>
		$(document).ready(function()
		{
			$('.content .left, .content input, .content textarea, .content select').focus(function(){
				$(this).parents('.content').addClass("over");
			}).blur(function(){
				$(this).parents('.content').removeClass("over");
			});
			$('.contentA .left, .contentA input, .contentA textarea, .contentA select').focus(function(){
				$(this).parents('.row').addClass("over");
			}).blur(function(){
				$(this).parents('.row').removeClass("over");
			});
		});
</script>
{endliteral}
{extra_head}
{ldelim}endliteral{rdelim}
</head>
<body>
{content}
{if has_tabs}
<script type="text/javascript"> 
  $("#usual1 ul").idTabs(); 
</script>
{endif}
</body>
</html>
