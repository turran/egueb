%name-prefix="ecss_"
%defines
%error-verbose
%locations
%pure-parser
%parse-param { void * scanner }
%parse-param { Ecss_Parser *parser }
%lex-param { void * scanner }

%{

#include <stdio.h>
#include <string.h>

#include <Eina.h>

#include "Ecss.h"
#include "ecss_private.h"

%}

%union {
	Ecss_Rule *r;
	Ecss_Declaration *d;
	Ecss_Selector_Combinator sc;
	Ecss_Combinator c;
	Ecss_Selector *s;
	Ecss_Filter *f;
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
		Ecss_Rule *r;
		Ecss_Declaration *d;
		Eina_List *l;

		r = ecss_rule_new($1);
		/* add every declaration */
		EINA_LIST_FOREACH($3, l, d)
		{
			ecss_rule_declaration_insert(r, d);
		}
		ecss_style_rule_add(parser->style, r);
	}
	;

declaration
	: T_IDENT ':' value
	{
		Ecss_Declaration *d;

		d = malloc(sizeof(Ecss_Declaration));
		d->property = $1;
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
		Ecss_Selector_Combinator *sc;
		Ecss_Selector *s;

		s = ecss_selector_new();
		ecss_selector_subject_set(s, $2);

		if ($4.s)
		{
			ecss_selector_combinator_set(s, $4.s, $4.c);
		}

		if ($3)
		{
			ecss_selector_filter_add(s, $3);
		}

		$$.s = s;
		$$.c = $1;
	}
	| { $$.s = NULL; }
	;

selector
	: ident filter selectors
	{
		Ecss_Selector *s;

		s = ecss_selector_new();
		ecss_selector_subject_set(s, $1);

		if ($3.s)
		{
			ecss_selector_combinator_set(s, $3.s, $3.c);
		}
		if ($2)
		{
			ecss_selector_filter_add(s, $2);
		}
		$$ = s;
	}
	;

combinator
	: '+' { $$ = ECSS_ADJACENT_SIBLING; }
	| '>' { $$ = ECSS_CHILD; }
	| '~' { $$ = ECSS_SIBLING; }
	| { $$ = ECSS_DESCENDANT; }
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
	: '.' T_IDENT { $$ = ecss_filter_class_new($2); }
	;

id	: '#' T_IDENT { $$ = ecss_filter_id_new($2); }
	;
%%

void ecss_error(void *data, void *scanner, Ecss_Parser *parser, const char *str)
{
	YYLTYPE *lloc = (YYLTYPE *)data;
	printf("Parsing error at %d: %d.%d %s", lloc->last_line, lloc->first_column, lloc->last_column, str);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

