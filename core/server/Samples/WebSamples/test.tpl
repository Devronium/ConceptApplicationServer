<html>
   <head>
      <title>{var1}</title>
   </head>
   <body>
      Array test : <b>{array[1]}</b><br />
      Multimple Objective test with modifier: <b>{object.m2.m3:year}</b><br />
      Simple Objective test : <b>{object.m1}</b><br />
      <br />
      {foreach var2}
         {foreach var2}
            {ldelim}Key '{var2:key}' for index {var2:index}/{var2:count} has value '{var2}'{rdelim}<br />
            {if var2}
                Not null !<br />
            {else}
                A null date (Y/M/D): {var2:year}/{var2:month}/{var2:day}<br />

            {endif}
         {endfor}
	 cod html bla bla bla<br />
      {endfor}
      Source code:<br />
      <pre>
{content}
      </pre>
      <br />
      Template: <br />
      <pre>
{template}
      </pre>
   </body>
{literal}
<br /> Here is some text that contains { and }. Is encapsulated by {litaral} element.
{endliteral}
	{var2["cheie"]}
</html>