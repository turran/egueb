%option prefix="egueb_css_engine_"
%option yylineno
%option bison-bridge
%option reentrant
%option noyywrap
%option bison-locations
%option case-insensitive

%{
#include <stdio.h>

#include <Eina.h>

#include "egueb_css_private.h"

#include "egueb_css_engine_selector_private.h"
#include "egueb_css_engine_rule_private.h"
#include "egueb_css_engine_style_private.h"
#include "egueb_css_engine_parser_private.h"
#include "src_lib_css_libegueb_css_la-egueb_css_engine_grammar.h"
#define YY_NO_UNISTD_H
%}

operator  [{}.,;=\:\*\#]
ident     [-]?{nmstart}{nmchar}*
name      {nmchar}+
nmstart   [_a-z]
nmchar    [_a-z0-9-]
num       [0-9]+|[0-9]*\.[0-9]+
string1   \"([^\n\r\f\\"]|\\{nl})*\"
string2   \'([^\n\r\f\\']|\\{nl})*\'
string    {string1}|{string2}
nl        \n|\r\n|\r|\f
w         [ \t\r\n\f]*

%%

[a-zA-Z0-9_]*           {
				yylval_param->str = strdup(yytext);
				return T_IDENT;
			}
{ident}			{
				yylval_param->str = strdup(yytext);
				return T_IDENT;
			}
{string}		{
				yylval_param->str = strdup(yytext);
				return T_STRING;
			}
{operator}		{ return *yytext; }

\/\*[^*]*\*+([^/*][^*]*\*+)*\/ 	/* ignore comments */
\n                      /* ignore end of line */
[ \t]+                  /* ignore whitespace */

%%

