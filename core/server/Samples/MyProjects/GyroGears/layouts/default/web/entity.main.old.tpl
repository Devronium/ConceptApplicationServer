<html>
<head>
<title>{title}</title>
{literal}
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />

<link type="text/css" href="css/flick/jquery-ui-1.8.14.custom.css" rel="stylesheet" />	
<link type="text/css" href="css/tables.css" rel="stylesheet" />	
<link rel="stylesheet" type="text/css" href="js/fancybox/jquery.fancybox-1.3.4.css" media="screen" />
<link rel="shortcut icon" href="res/icon.png" />
<script type="text/javascript" src="js/jquery.min.js"></script>
<script type="text/javascript" src="js/jquery-ui.min.js"></script>
<script type="text/javascript" src="js/fancybox/jquery.mousewheel-3.0.4.pack.js"></script>
<script type="text/javascript" src="js/fancybox/jquery.fancybox-1.3.4.pack.js"></script>
<script type="text/javascript" src="js/tiny_mce/jquery.tinymce.js"></script>
<script type="text/javascript" src="js/expander/expand.js"></script>
<script type="text/javascript">
//<![CDATA[
	$(function() {
		$("#MasterContent").tabs({history: true});
{endliteral}
{foreach cat}
		$("#tabs{cat.Reserved}").tabs({literal}
			{
				effect: 'ajax',
				history: true,
				load: function(event, ui) {
{endliteral}
{foreach cat.Elements}
					$("a[rel=img_group_{cat.Elements.PublicName}]").fancybox({literal}
					{
						'transitionIn'		: 'elastic',
						'transitionOut'		: 'elastic',
						'titlePosition' 	: 'over',
						'titleFormat'		: function(title, currentArray, currentIndex, currentOpts) {
							return '<span id="fancybox-title-over">Image ' + (currentIndex + 1) + ' / ' + currentArray.length + (title.length ? ' &nbsp; ' + title : '') + '</span>';
						}
					});
{endliteral}
					$("a[rel=movie_group_{cat.Elements.PublicName}]").fancybox({literal}
					{
						'transitionIn'		: 'elastic',
						'transitionOut'		: 'elastic'
					});

{endliteral}
					$('form#search_{cat.Elements.PublicName}_form').live('submit', function() {ldelim}
						$(ui.panel).load($(this).attr('action')+'?'+$(this).serialize(), function() {ldelim}
							$("a[rel=img_group_{cat.Elements.PublicName}]").fancybox({literal}
							{
								'transitionIn'		: 'elastic',
								'transitionOut'		: 'elastic',
								'titlePosition' 	: 'over',
								'titleFormat'		: function(title, currentArray, currentIndex, currentOpts) {
									return '<span id="fancybox-title-over">Image ' + (currentIndex + 1) + ' / ' + currentArray.length + (title.length ? ' &nbsp; ' + title : '') + '</span>';
								}
							});
{endliteral}
							$("a[rel=movie_group_{cat.Elements.PublicName}]").fancybox({literal}
							{
								'transitionIn'		: 'elastic',
								'transitionOut'		: 'elastic'
							});
{endliteral}
							$("input#search_{cat.Elements.PublicName}").autocomplete({ldelim}
								source: "_JS_{cat.Elements.PublicName}Suggestions.csp"
							{rdelim});
						{rdelim});
						return false;
					{rdelim});

					$("input#search_{cat.Elements.PublicName}").autocomplete({ldelim}
						source: "_JS_{cat.Elements.PublicName}Suggestions.csp"
					{rdelim});

{endfor}
{literal}
					$("a[rel=pagination]", ui.panel).live("click", function () {
						$(ui.panel).load(this.href, function() {
{endliteral}
{foreach cat.Elements}
							$("a[rel=img_group_{cat.Elements.PublicName}]").fancybox({literal}
							{
								'transitionIn'		: 'elastic',
								'transitionOut'		: 'elastic',
								'titlePosition' 	: 'over',
								'titleFormat'		: function(title, currentArray, currentIndex, currentOpts) {
									return '<span id="fancybox-title-over">Image ' + (currentIndex + 1) + ' / ' + currentArray.length + (title.length ? ' &nbsp; ' + title : '') + '</span>';
								}
							});
{endliteral}
							$("a[rel=movie_group_{cat.Elements.PublicName}]").fancybox({literal}
							{
								'transitionIn'		: 'elastic',
								'transitionOut'		: 'elastic'
							});

{endliteral}
							$("input#search_{cat.Elements.PublicName}").autocomplete({ldelim}
								source: "_JS_{cat.Elements.PublicName}Suggestions.csp"
							{rdelim});
{endfor}
{literal}

						});
						return false;
					});

				}
			}
		);
{endliteral}
{endfor}
{literal}
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
    <a href="index.html">Home</a> |
    {foreach cat}
		<a href="{cat.Parsed}.html">{cat.Name}</a> |
    {endfor}
  </div>
  <div class="clear"></div>
</div>
<div id="MasterContent">
	<ul>
{foreach cat}
		<li><a href="#tabs{cat.Reserved}">{cat.Name}</a></li>
{endfor}
	</ul>
{foreach cat}
	<div id="tabs{cat.Reserved}">
		<ul>
{foreach cat.Elements}
{if cat.Elements.IsSettings}
	{if cat.Elements.Icon}
		{if cat.Elements.IsImportant}
			<li><a href="{cat.Elements.Href}.csp?noheader&#x3D;1" title="{cat.Elements.Title}"><span><img border="0" src="res/{cat.Elements.Icon}" alt="{cat.Elements.Title}" /> {cat.Elements.Title}</span></a></li>
		{else}
			<li><a href="{cat.Elements.Href}.csp?noheader&#x3D;1" title="{cat.Elements.Title}"><span><img border="0" src="res/{cat.Elements.Icon}" alt="{cat.Elements.Title}" />&#160;</span></a></li>
		{endif}
	{else}
			<li><a href="{cat.Elements.Href}.csp?noheader&#x3D;1" title="{cat.Elements.Title}"><span>{cat.Elements.Title}</span></a></li>
	{endif}
{else}
	{if cat.Elements.Icon}
		{if cat.Elements.IsImportant}
			<li><a href="{cat.Elements.Href}List.csp?noheader&#x3D;1" title="{cat.Elements.Title}"><span><img border="0" src="res/{cat.Elements.Icon}" alt="{cat.Elements.Title}" /> {cat.Elements.Title}</span></a></li>
		{else}
			<li><a href="{cat.Elements.Href}List.csp?noheader&#x3D;1" title="{cat.Elements.Title}"><span><img border="0" src="res/{cat.Elements.Icon}" alt="{cat.Elements.Title}" />&#160;</span></a></li>
		{endif}
	{else}
			<li><a href="{cat.Elements.Href}List.csp?noheader&#x3D;1" title="{cat.Elements.Title}"><span>{cat.Elements.Title}</span></a></li>
	{endif}
{endif}
{endfor}
		</ul>
	</div>
{endfor}
</div>
<div id="footer">
  Do you like this application?<br/>
  It was created in only a few minutes using <a href="http://www.radgs.com/GyroGears.html">GyroGears</a> &#xA9; 2008-2012 <a href="http://www.radgs.com">RadGs Software</a>.
</div>
</div>
</body>
</html>
