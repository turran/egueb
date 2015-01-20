/* Egueb_Css_Engine - CSS
 * Copyright (C) 2011 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "egueb_css_private.h"

#include "egueb_css_engine_selector_private.h"
#include "egueb_css_engine_rule_private.h"
#include "egueb_css_engine_style_private.h"
#include "egueb_css_engine_parser_private.h"
#include "egueb_css_engine_filter_private.h"
#include "egueb_css_engine_node_private.h"

void * egueb_css_engine__scan_string(const char *yy_str, void *scanner);
void egueb_css_engine__delete_buffer(void *state, void *scanner);
void egueb_css_engine__switch_to_buffer(void *state, void *scanner);
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/* Keep track of the selector state */
typedef struct _Egueb_Css_Engine_State_Selector
{
	Egueb_Css_Engine_Rule *r;
	Egueb_Css_Engine_Selector *s; /* the current selector in case we have a compound selector */
	int child;
	int sibling;
} Egueb_Css_Engine_State_Selector;

typedef struct _Egueb_Css_Engine_State
{
	Eina_List *active;
	Eina_List *inactive;
} Egueb_Css_Engine_State;

static void _element_rule_apply(Egueb_Css_Engine_Rule *r, Egueb_Dom_Node *n)
{
	Egueb_Css_Engine_Declaration *d;
	Eina_List *l;

	EINA_LIST_FOREACH(r->declarations, l, d)
	{
		egueb_css_engine_node_attribute_set_simple(n, d->attribute,
				d->value);
	}
}

static Eina_Bool _element_matches(Egueb_Css_Engine_State_Selector *ss, Egueb_Dom_Node *n)
{
	Egueb_Css_Engine_Rule *r = ss->r;
	Egueb_Css_Engine_Selector *s = ss->s;
	Eina_Bool ret = EINA_FALSE;

	if (!s->subject)
	{
		ret = EINA_TRUE;
	}
	else
	{
		Egueb_Dom_String *name;
		const char *str;

		name = egueb_dom_node_name_get(n);
		str = egueb_dom_string_string_get(name);
		if (!strcmp(str, s->subject))
			ret = EINA_TRUE;
		egueb_dom_string_unref(name);
		return ret;
	}

	if (ret)
	{
		Egueb_Css_Engine_Filter *f;
		Eina_List *l;

		EINA_LIST_FOREACH(s->filters, l, f)
		{
			if (!egueb_css_engine_filter_test(f, n))
				return EINA_FALSE;
		}
	}
	return ret;
}

static void _process_element(Egueb_Css_Engine_State *state, Egueb_Dom_Node *n)
{
	Egueb_Dom_Node *n1;
	Egueb_Css_Engine_State_Selector *ss;
	Eina_List *l;
	Eina_List *l_next;
	Eina_List *new_active = NULL;
	Eina_List *new_inactive = NULL;
	Eina_List *old_active;
	Eina_List *old_inactive;

	/* process this element */
	/* move from the inactive rule to the new active list or apply directly */
	old_inactive = state->inactive;
	EINA_LIST_FOREACH(state->inactive, l, ss)
	{
		Egueb_Css_Engine_Selector *s_next;
		Egueb_Css_Engine_Selector *s = ss->s;

		if (_element_matches(ss, n))
		{
			/* can we apply directly? */
			s_next = egueb_css_engine_selector_next_get(s);
			if (!s_next)
			{
				/* apply */
				_element_rule_apply(ss->r, n);
			}
			else
			{
				new_active = eina_list_append(new_active, ss);
			}
		}
		else
		{
			new_inactive = eina_list_append(new_inactive, ss);
		}
	}
	old_active = state->active;
	EINA_LIST_FOREACH(state->active, l, ss)
	{
		Egueb_Css_Engine_Selector *s_next;
		Egueb_Css_Engine_Selector *s = ss->s;
		Eina_Bool keep = EINA_FALSE; /* keep in the list of actives for later */
		Eina_Bool apply = EINA_FALSE; /* apply if it matches */

		switch (s->c)
		{
			case EGUEB_CSS_NONE:
			/* this case should not happen */
			ERR("Bad parsing");
			break;

			case EGUEB_CSS_DESCENDANT:
			if (ss->child > 0)
			{
				apply = EINA_TRUE;
				keep = EINA_TRUE;
			}
			break;

			case EGUEB_CSS_CHILD:
			if (ss->child == 1)
				apply = EINA_TRUE;
			break;

			case EGUEB_CSS_ADJACENT_SIBLING:
			if (ss->sibling == 1)
				apply = EINA_TRUE;
			break;

			case EGUEB_CSS_SIBLING:
			if (ss->sibling > 0)
			{
				apply = EINA_TRUE;
				keep = EINA_TRUE;
			}
			break;

		}

		if (apply)
		{
			if (_element_matches(ss, n))
			{
				s_next = egueb_css_engine_selector_next_get(s);
				if (!s_next)
				{
					/* apply */
					_element_rule_apply(ss->r, n);
				}
				else
				{
					ss->s = s_next;
					keep = EINA_TRUE;
				}
			}
		}
		if (keep)
		{
			new_active = eina_list_append(new_active, ss);
		}
		else
		{
			new_inactive = eina_list_append(new_inactive, ss);
		}
		/* given that we are going to test the first child we can safely
		 * increment the child here */
		ss->child++;
	}

	state->active = new_active;
	state->inactive = new_inactive;

	n1 = egueb_dom_element_child_first_get(n);
	while (n1)
	{
		Egueb_Dom_Node *tmp;

		_process_element(state, n1);

		tmp = egueb_dom_element_sibling_next_get(n1);
		egueb_dom_node_unref(n1);
		n1 = tmp;
		/* increment the active sibling count */
		EINA_LIST_FOREACH(new_active, l, ss)
		{
			ss->sibling++;
		}
	}
	/* TODO for every s_next we have advanced, set the prev one */
	/* decrement the child and reset the sibling */
	EINA_LIST_FOREACH(new_active, l, ss)
	{
		ss->child--;
		ss->sibling = 0;
	}

	state->active = old_active;
	state->inactive = old_inactive;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_css_engine_style_inline_apply(Egueb_Dom_Node *n, const char *style)
{
	Egueb_Dom_String *prop;
	Egueb_Dom_String *val;

	char *orig;
	char *v;
	char *sc;
	char *ch;

	orig = v = strdup(style);
	EGUEB_DOM_SPACE_SKIP(v);
	/* split the style by ';' */
	while ((sc = strchr(v, ';')))
	{
		*sc = '\0';
		/* split it by ':' */
		ch = strchr(v, ':');
		if (ch)
		{
			char *vv;

			*ch = '\0';
			vv = ch + 1;
			EGUEB_DOM_SPACE_SKIP(vv);
			/* and call the attr_cb */
			egueb_css_engine_node_attribute_set_simple(n, v, vv);
		}
		v = sc + 1;
		EGUEB_DOM_SPACE_SKIP(v);
	}
	/* do the last one */
	ch = strchr(v, ':');
	if (ch)
	{
		char *vv;

		*ch = '\0';
		vv = ch + 1;
		EGUEB_DOM_SPACE_SKIP(vv);
		/* and call the attr_cb */
		prop = egueb_dom_string_new_with_string(v);
		val = egueb_dom_string_new_with_string(vv);
		egueb_css_engine_node_attribute_set_simple(n, v, vv);
	}

	free(orig);
}

Egueb_Css_Engine_Style * egueb_css_engine_style_new(void)
{
	Egueb_Css_Engine_Style *thiz;

	thiz = calloc(1, sizeof(Egueb_Css_Engine_Style));
	return thiz;
}

Egueb_Css_Engine_Style * egueb_css_engine_style_load_from_file(const char *file)
{
	Egueb_Css_Engine_Style *thiz;
	Egueb_Css_Engine_Parser parser;
	void *scanner;
	FILE *f;
	int ret;

	if (!file) return NULL;

	f = fopen(file, "r");
	if (!f) return NULL;

	thiz = egueb_css_engine_style_new();
	parser.style = thiz;

	egueb_css_engine_lex_init(&scanner);
	egueb_css_engine_set_in(f, scanner);
	ret = egueb_css_engine_parse(scanner, &parser);

	egueb_css_engine_lex_destroy(scanner);
	fclose(f);

	if (ret)
	{
		ERR("bad parsing");
		free(thiz);
		return NULL;
	}
	return thiz;
}

Egueb_Css_Engine_Style * egueb_css_engine_style_load_from_content(const char *content, size_t len)
{
	Egueb_Css_Engine_Style *thiz;
	Egueb_Css_Engine_Parser parser;
	char *tmp;
	void *buffer_state;
	void *scanner;
	int ret;

	thiz = egueb_css_engine_style_new();
	parser.style = thiz;

	tmp = malloc(len + 2);
	strncpy(tmp, content, len);
	tmp[len] = tmp [len+1] = 0; // looks like flex/yacc need a double null terminated string

	egueb_css_engine_lex_init(&scanner);
	buffer_state = egueb_css_engine__scan_string(tmp, scanner);
	egueb_css_engine__switch_to_buffer(buffer_state, scanner); // switch flex to the buffer we just created
	ret = egueb_css_engine_parse(scanner, &parser);
	egueb_css_engine__delete_buffer(buffer_state, scanner);

	egueb_css_engine_lex_destroy(scanner);

	if (ret)
	{
		printf("bad parsing\n");
		free(thiz);
		return NULL;
	}

	return thiz;
}

void egueb_css_engine_style_rule_add(Egueb_Css_Engine_Style *thiz, Egueb_Css_Engine_Rule *r)
{
	if (!thiz) return;
	if (!r) return;
	thiz->rules = eina_list_append(thiz->rules, r);
}

void egueb_css_engine_style_apply(Egueb_Css_Engine_Style *thiz, Egueb_Dom_Node *n)
{
	Egueb_Css_Engine_State state;
	Egueb_Css_Engine_Rule *r;
	Eina_List *l;

	if (!thiz) return;
	if (!n) return;

	state.active = NULL;
	state.inactive = NULL;

	/* put every rule on the inactive list */
	EINA_LIST_FOREACH(thiz->rules, l, r)
	{
		Egueb_Css_Engine_State_Selector *ss;
		Egueb_Css_Engine_Selector *thiz = r->selector;

		ss = malloc(sizeof(Egueb_Css_Engine_State_Selector));
		ss->r = r;
		ss->s = thiz;
		ss->child = 0;
		ss->sibling = 0;
		state.inactive = eina_list_append(state.inactive, ss);
	}
	/* parse the tree top -> bottom */
	_process_element(&state, n);
	/* TODO destroy the state */
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

