<html>
<head>
<title>{title} | {targetcat.Name}</title>
{literal}
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />

<link type="text/css" href="css/flick/jquery-ui-1.8.14.custom.css" rel="stylesheet" />	
<link type="text/css" href="css/tables.css" rel="stylesheet" />	
<link rel="stylesheet" type="text/css" href="js/fancybox/jquery.fancybox-1.3.4.css" media="screen" />
<link rel="shortcut icon" href="res/icon.png" />
<script type="text/javascript" src="js/jquery.min.js"></script>
<script type="text/javascript" src="js/jquery-ui.min.js"></script>
<script type="text/javascript" src="js/jquery.address.min.js"></script>
<script type="text/javascript" src="js/ui.ariaTabs.js"></script>
<script type="text/javascript" src="js/fancybox/jquery.mousewheel-3.0.4.pack.js"></script>
<script type="text/javascript" src="js/fancybox/jquery.fancybox-1.3.4.pack.js"></script>
<script type="text/javascript" src="js/tiny_mce/jquery.tinymce.js"></script>
<script type="text/javascript" src="js/expander/expand.js"></script>
<script type="text/javascript">
//<![CDATA[
	$(function() {
{endliteral}
		$("#MasterContent").tabs({literal}
			{
				effect: 'ajax',
				jqAddress: {
					enable: true,
					title: {
						split: ' | '		
					}
				},
				load: function(event, ui) {
					event.preventDefault();
{endliteral}
{foreach targetcat.Elements}
					$("a[rel=img_group_{targetcat.Elements.PublicName}]").fancybox({literal}
					{
						'transitionIn'		: 'elastic',
						'transitionOut'		: 'elastic',
						'titlePosition' 	: 'over',
						'titleFormat'		: function(title, currentArray, currentIndex, currentOpts) {
							return '<span id="fancybox-title-over">Image ' + (currentIndex + 1) + ' / ' + currentArray.length + (title.length ? ' &nbsp; ' + title : '') + '</span>';
						}
					});
{endliteral}
					$("a[rel=movie_group_{targetcat.Elements.PublicName}]").fancybox({literal}
					{
						'transitionIn'		: 'elastic',
						'transitionOut'		: 'elastic'
					});

{endliteral}
					$('form#search_{targetcat.Elements.PublicName}_form').live('submit', function() {ldelim}
						$(ui.panel).load($(this).attr('action')+'?'+$(this).serialize(), function() {ldelim}
							$("a[rel=img_group_{targetcat.Elements.PublicName}]").fancybox({literal}
							{
								'transitionIn'		: 'elastic',
								'transitionOut'		: 'elastic',
								'titlePosition' 	: 'over',
								'titleFormat'		: function(title, currentArray, currentIndex, currentOpts) {
									return '<span id="fancybox-title-over">Image ' + (currentIndex + 1) + ' / ' + currentArray.length + (title.length ? ' &nbsp; ' + title : '') + '</span>';
								}
							});
{endliteral}
							$("a[rel=movie_group_{targetcat.Elements.PublicName}]").fancybox({literal}
							{
								'transitionIn'		: 'elastic',
								'transitionOut'		: 'elastic'
							});
{endliteral}
							$("input#search_{targetcat.Elements.PublicName}").autocomplete({ldelim}
								source: "_JS_{targetcat.Elements.PublicName}Suggestions.csp"
							{rdelim});
						{rdelim});
						return false;
					{rdelim});

					$("input#search_{targetcat.Elements.PublicName}").autocomplete({ldelim}
						source: "_JS_{targetcat.Elements.PublicName}Suggestions.csp"
					{rdelim});

{endfor}

{literal}
					$("a[rel=pagination]", ui.panel).live("click", function () {
						$(ui.panel).load(this.href, function() {
{endliteral}
{foreach targetcat.Elements}
							$("a[rel=img_group_{targetcat.Elements.PublicName}]").fancybox({literal}
							{
								'transitionIn'		: 'elastic',
								'transitionOut'		: 'elastic',
								'titlePosition' 	: 'over',
								'titleFormat'		: function(title, currentArray, currentIndex, currentOpts) {
									return '<span id="fancybox-title-over">Image ' + (currentIndex + 1) + ' / ' + currentArray.length + (title.length ? ' &nbsp; ' + title : '') + '</span>';
								}
							});
{endliteral}
							$("a[rel=movie_group_{targetcat.Elements.PublicName}]").fancybox({literal}
							{
								'transitionIn'		: 'elastic',
								'transitionOut'		: 'elastic'
							});

{endliteral}
							$("input#search_{targetcat.Elements.PublicName}").autocomplete({ldelim}
								source: "_JS_{targetcat.Elements.PublicName}Suggestions.csp"
							{rdelim});
{literal}
{endliteral}
{endfor}
{literal}

						});
						return false;
					});
				}
			});
	});
//]]>
</script>
{endliteral}
</head>
<body>
<div id="MasterContainer">
<div id="header">
  <a href="index.html">
    <img src="res/header.png" alt="" border="0" align="left"/>
  </a>
  <div id="headermenu">
	<div class="LoginInfo">
		Retrieving user information ...
	</div>
	<script type="text/javascript">
	{literal}
	//<![CDATA[
		$(function() {
			$.getJSON('IsLoggedIn.csp', function(data) {
				var datahtml;
				if ((data) && (data.logged_in) && (data.username)) {
					datahtml="You are logged in as ";
					if (data.userentity)
						datahtml+='<a href="'+data.userentity+'.csp?id='+data.UID+'">';
					datahtml+=data.username
					if (data.userentity)
						datahtml+="</a>";
					datahtml+=' | <a href="LogIn.csp?logout=1">Sign out</a>';
				} else {
					{endliteral}
					datahtml='Please <a href="LogIn.csp?__REDIRECT={targetcat.Parsed}.html">sign in</a> or <a href="SignUp.csp?__REDIRECT=index.html">sign up</a>';
					{literal}
				}
				$(".LoginInfo").html(datahtml);
			});
		});
	//]]>
	</script>
	{endliteral}
    {foreach cat}
	{if cat.Name}
		<a href="{cat.Parsed}.html">{cat.Name}</a> |
	{else}
		<a href="{cat.Parsed}.html">Home</a> |
	{endif}
    {endfor}
  </div>
  <div class="clear"></div>
</div>
<div id="MasterContent">
		<ul>
{foreach targetcat.Elements}
{if targetcat.Elements.IsSettings}
	{if targetcat.Elements.Icon}
		{if targetcat.Elements.IsImportant}
			<li><a href="{targetcat.Elements.Href}.csp?noheader&#x3D;1&amp;fullusers&#x3D;1" title="{targetcat.Elements.Title}"><span><img border="0" src="res/{targetcat.Elements.Icon}" alt="{targetcat.Elements.Title}" /> {targetcat.Elements.Title}</span></a></li>
		{else}
			<li><a href="{targetcat.Elements.Href}.csp?noheader&#x3D;1&amp;fullusers&#x3D;1" title="{targetcat.Elements.Title}"><span><img border="0" src="res/{targetcat.Elements.Icon}" alt="{targetcat.Elements.Title}" />&#160;</span></a></li>
		{endif}
	{else}
			<li><a href="{targetcat.Elements.Href}.csp?noheader&#x3D;1&amp;fullusers&#x3D;1" title="{targetcat.Elements.Title}"><span>{targetcat.Elements.Title}</span></a></li>
	{endif}
{else}
	{if targetcat.Elements.Icon}
		{if targetcat.Elements.IsImportant}
			<li><a href="{targetcat.Elements.Href}List.csp?noheader&#x3D;1&amp;fullusers&#x3D;1" title="{targetcat.Elements.Title}"><span><img border="0" src="res/{targetcat.Elements.Icon}" alt="{targetcat.Elements.Title}" /> {targetcat.Elements.Title}</span></a></li>
		{else}
			<li><a href="{targetcat.Elements.Href}List.csp?noheader&#x3D;1&amp;fullusers&#x3D;1" title="{targetcat.Elements.Title}"><span><img border="0" src="res/{targetcat.Elements.Icon}" alt="{targetcat.Elements.Title}" />&#160;</span></a></li>
		{endif}
	{else}
			<li><a href="{targetcat.Elements.Href}List.csp?noheader&#x3D;1&amp;fullusers&#x3D;1" title="{targetcat.Elements.Title}"><span>{targetcat.Elements.Title}</span></a></li>
	{endif}
{endif}
{endfor}
		</ul>
	</div>
</div>
<div id="footer">
  Do you like this application?<br/>
  It was created in only a few minutes using <a href="http://devronium.com/GyroGears.html">GyroGears</a> &#xA9; 2008-2016 <a href="http://devronium.com">Devronium Applications</a>.
</div>
</div>
</body>
</html>
