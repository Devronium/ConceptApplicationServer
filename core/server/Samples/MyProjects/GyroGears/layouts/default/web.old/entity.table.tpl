<html>
<head>
<title>{title}</title>
{literal}
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link rel="stylesheet" href="css/tables.css" type="text/css" title='main' media="screen" />
<link rel="stylesheet" href="css/mainwindow.css" type="text/css" title='main' media="screen" />

<link rel="stylesheet" href="js/lightbox/css/lightbox.css" type="text/css" media="screen" />
<script type="text/javascript" src="js/lightbox/src/prototype.js"></script>
<script type="text/javascript" src="js/lightbox/src/scriptaculous.js?load=effects,builder"></script>
<script type="text/javascript" src="js/lightbox/src/lightbox.js"></script>

{endliteral}
{extra_head}
</head>
<body>

<div id="modernbricksmenu">
	<ul>
		{foreach button}
			{if button.Selected}
			<li id="current"><a href="{button.Href}List.csp" title="{button.Title}">{button.Title}</a></li>
			{else}
			<li><a href="{button.Href}List.csp" title="{button.Title}">{button.Title}</a></li>
			{endif}
		{endfor}
	</ul>
	<form id="myform">
		<input type="text" name="q" value="{ldelim}criteria{rdelim}" class="textinput" /> <input class="submit" type="submit" value="Find" />
		<input type="hidden" name="sort" value="{ldelim}sort{rdelim}" />
		<input type="hidden" name="desc" value="{ldelim}desc{rdelim}" />
	</form>
</div>

<div id="modernbricksmenuline">&nbsp;</div>

{ldelim}if prev_link{rdelim}
	<a href="{public_name}List.csp?q={ldelim}criteria{rdelim}&page={ldelim}prev_link{rdelim}&sort={ldelim}sort{rdelim}&desc={ldelim}desc{rdelim}"> << </a>
{ldelim}endif{rdelim}

{ldelim}foreach page_link{rdelim}
	{ldelim}if page_link{rdelim}
		<a href="{public_name}List.csp?q={ldelim}criteria{rdelim}&page={ldelim}page_link{rdelim}&sort={ldelim}sort{rdelim}&desc={ldelim}desc{rdelim}">{ldelim}page_link{rdelim}</a>
	{ldelim}else{rdelim}
		&nbsp;...&nbsp;
	{ldelim}endif{rdelim}
{ldelim}endfor{rdelim}

{ldelim}if next_link{rdelim}
	<a href="{public_name}List.csp?q={ldelim}criteria{rdelim}&page={ldelim}next_link{rdelim}&sort={ldelim}sort{rdelim}&desc={ldelim}desc{rdelim}"> >> </a>
{ldelim}endif{rdelim}

	<table>
		<thead>
			<tr>
				<th scope="col" align="right">#</th>
{foreach column}
{if column.align_right}
				<th scope="col" align="right">{column.column_name}{if column.can_sort} <a href="?q={ldelim}criteria{rdelim}&sort={column.member_name}&desc=1">&uArr;</a> <a href="?q={ldelim}criteria{rdelim}&sort={column.member_name}&desc=0">&dArr;</a>{endif}</th>
{else}
				<th scope="col" align="left">{column.column_name}{if column.can_sort}  <a href="?q={ldelim}criteria{rdelim}&sort={column.member_name}&desc=1">&uArr;</a> <a href="?q={ldelim}criteria{rdelim}&sort={column.member_name}&desc=0">&dArr;</a>{endif}</th>
{endif}
{endfor}
			</tr>
		</thead>
		<tfoot>
			<tr>
				<th scope="row">Total</th>
				<td colspan="{len_column}">{ldelim}count{rdelim} {friendly_name}</td>
			</tr>
		</tfoot>
		<tbody>
{ldelim}foreach row{rdelim}

{ldelim}if row.__EVENINDEX{rdelim}
			<tr class="odd">
{ldelim}else{rdelim}
			<tr>
{ldelim}endif{rdelim}
				<th scope="row" align="right"><a href="view{public_name}.csp?id={ldelim}row.__DBID{rdelim}">{ldelim}row.__VIEWINDEX{rdelim}</a></th>
{foreach column}
{if column.align_right}
				<td scope="col" align="right"><a href="view{public_name}.csp?id={ldelim}row.__DBID{rdelim}">{ldelim}row.{column.member_name}{rdelim}</a></td>
{else}
{if column.is_picture}
				<td scope="col" align="left"><a rel="lightbox" title="{ldelim}row.{master_field}{rdelim}" href="get{public_name}{column.member_name}.csp?id={ldelim}row.__DBID{rdelim}"><img border="0" src="get{public_name}{column.member_name}.csp?id={ldelim}row.__DBID{rdelim}&thumb=1" alt=""/></a>&nbsp;</td>
{else}
{if column.is_video}
				<td scope="col" align="left"><a href="Show{public_name}{column.member_name}.csp?id={ldelim}row.__DBID{rdelim}"><img border="0" src="get{public_name}{column.member_name}.csp?id={ldelim}row.__DBID{rdelim}&thumb=1" alt=""/></a>&nbsp;</td>
{else}
				<td scope="col" align="left"><a href="view{public_name}.csp?id={ldelim}row.__DBID{rdelim}">{ldelim}row.{column.member_name}{rdelim}</a>&nbsp;</td>
{endif}
{endif}
{endif}
{endfor}
			</tr>
{ldelim}endfor{rdelim}
		</tbody>	
	</table>
</body>
</html>
