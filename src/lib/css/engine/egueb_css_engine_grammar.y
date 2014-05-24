%name-prefix="egueb_css_engine_"
%defines
%error-verbose
%locations
%pure-parser
%parse-param { void * scanner }
%parse-param { Egueb_Css_Engine_Parser *parser }
%lex-param { void * scanner }

%{

#include <stdio.h>
#include <string.h>

#include <Eina.h>

#include "egueb_css_private.h"

#include "egueb_css_engine_context_private.h"
#include "egueb_css_engine_selector_private.h"
#include "egueb_css_engine_rule_private.h"
#include "egueb_css_engine_filter_private.h"
#include "egueb_css_engine_style_private.h"
#include "egueb_css_engine_selector_private.h"
#include "egueb_css_engine_parser_private.h"

void egueb_css_engine_error(void *data, void *scanner, Egueb_Css_Engine_Parser *parser, const char *str);

%}

%union {
	Egueb_Css_Engine_Rule *r;
	Egueb_Css_Engine_Declaration *d;
	Egueb_Css_Engine_Selector_Combinator sc;
	Egueb_Css_Engine_Combinator c;
	Egueb_Css_Engine_Selector *s;
	Egueb_Css_Engine_Filter *f;
	Eina_List *l; // use this for every _list nonterminal
	char *str;
}

%token <str> T_STRING
%token <str> T_IDENT
%type <str> ident
%type <r> rule
%type <c> combinator
%type <s> selector
%type <f> filter
%type <f> class
%type <f> id
%type <sc> selectors
%type <d> declaration
%type <l> declarations
%type <str> value
%%

rules
	: rule
	| rule rules
	|
	;

rule
	: selector '{' declarations '}'
	{
		Egueb_Css_Engine_Rule *r;
		Egueb_Css_Engine_Declaration *d;
		Eina_List *l;

		r = egueb_css_engine_rule_new($1);
		/* add every declaration */
		EINA_LIST_FOREACH($3, l, d)
		{
			egueb_css_engine_rule_declaration_insert(r, d);
		}
		egueb_css_engine_style_rule_add(parser->style, r);
	}
	;

declaration
	: T_IDENT ':' value
	{
		Egueb_Css_Engine_Declaration *d;

		d = malloc(sizeof(Egueb_Css_Engine_Declaration));
		d->attribute = $1;
		d->value = $3;

		$$ = d;
	}
	;

declarations
	: declaration
	{
		$$ = eina_list_append(NULL, $1);
	}
	| declaration ';' declarations
	{
		$$ = eina_list_append($3, $1);
	}
	|
	;

selectors
	: combinator ident filter selectors
	{
		Egueb_Css_Engine_Selector_Combinator *sc;
		Egueb_Css_Engine_Selector *s;

		s = egueb_css_engine_selector_new();
		egueb_css_engine_selector_subject_set(s, $2);

		if ($4.s)
		{
			egueb_css_engine_selector_combinator_set(s, $4.s, $4.c);
		}

		if ($3)
		{
			egueb_css_engine_selector_filter_add(s, $3);
		}

		$$.s = s;
		$$.c = $1;
	}
	| { $$.s = NULL; }
	;

selector
	: ident filter selectors
	{
		Egueb_Css_Engine_Selector *s;

		s = egueb_css_engine_selector_new();
		egueb_css_engine_selector_subject_set(s, $1);

		if ($3.s)
		{
			egueb_css_engine_selector_combinator_set(s, $3.s, $3.c);
		}
		if ($2)
		{
			egueb_css_engine_selector_filter_add(s, $2);
		}
		$$ = s;
	}
	;

combinator
	: '+' { $$ = EGUEB_CSS_ADJACENT_SIBLING; }
	| '>' { $$ = EGUEB_CSS_CHILD; }
	| '~' { $$ = EGUEB_CSS_SIBLING; }
	| { $$ = EGUEB_CSS_DESCENDANT; }
	;

ident
	: T_IDENT { $$ = $1; }
	| '*' { $$ = NULL; }
	| { $$ = NULL; }
	;

filter
	: { $$ = NULL; }
	| class { $$ = $1; }
	| id { $$ = $1; }
	;

value
	: T_IDENT { $$ = $1; }
	| T_STRING { $$ = $1; }
	| '#' T_IDENT
	{
		char *tmp;
		size_t len;

		len = strlen($2);
		tmp = malloc(len + 2);
		*tmp = '#';
		strcpy(tmp + 1, $2);
		tmp[len+1] = '\0';
		$$ = tmp;

		free($2);
	}
	;

class
	: '.' T_IDENT { $$ = egueb_css_engine_filter_class_new($2); }
	;

id	: '#' T_IDENT { $$ = egueb_css_engine_filter_id_new($2); }
	;
%%

void egueb_css_engine_error(void *data, void *scanner, Egueb_Css_Engine_Parser *parser, const char *str)
{
	YYLTYPE *lloc = (YYLTYPE *)data;
	printf("Parsing error at %d: %d.%d %s", lloc->last_line, lloc->first_column, lloc->last_column, str);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

