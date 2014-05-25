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

#include "egueb_css_engine_context_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/* FIXME share this */
#define ESVG_SPACE_SKIP(t) \
	while (*t) { if ((*t == ' ') || (*t == '\n') || (*t == '\t')) t++; else break; }

#if 0
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

static void _element_rule_apply(Egueb_Css_Engine_Context *c, void *e, Egueb_Css_Engine_Rule *r)
{
	Egueb_Css_Engine_Declaration *d;
	Eina_List *l;

	EINA_LIST_FOREACH(r->declarations, l, d)
	{
		c->attribute_set(e, d->attribute, d->value);
	}
}

static Eina_Bool _element_matches(Egueb_Css_Engine_Context *c, void *e, Egueb_Css_Engine_State_Selector *ss)
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
		if (!strcmp(c->get_name(e), s->subject))
			ret = EINA_TRUE;
	}

	if (ret)
	{
		Egueb_Css_Engine_Filter *f;
		Eina_List *l;

		EINA_LIST_FOREACH(s->filters, l, f)
		{
			if (!egueb_css_engine_filter_test(f, c, e))
				return EINA_FALSE;
		}
	}
	return ret;
}

static void _process_element(Egueb_Css_Engine_Context *c, void *e, Egueb_Css_Engine_State *state)
{
	void *e1;
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

		if (_element_matches(c, e, ss))
		{
			/* can we apply directly? */
			s_next = egueb_css_engine_selector_next_get(s);
			if (!s_next)
			{
				/* apply */
				_element_rule_apply(c, e, ss->r);
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
			printf(">>> error <<<\n");
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
			if (_element_matches(c, e, ss))
			{
				s_next = egueb_css_engine_selector_next_get(s);
				if (!s_next)
				{
					/* apply */
					_element_rule_apply(c, e, ss->r);
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

	e1 = c->get_child(e);
	while (e1)
	{
		_process_element(c, e1, state);
		e1 = c->get_next_sibling(e1);
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
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void egueb_css_engine_context_style_apply(Egueb_Css_Engine_Context *c, Egueb_Css_Engine_Style *s, void *e)
{
	Egueb_Css_Engine_State state;
	Egueb_Css_Engine_Rule *r;
	Eina_List *l;

	if (!c) return;
	if (!s) return;
	if (!e) return;

	state.active = NULL;
	state.inactive = NULL;

	/* put every rule on the inactive list */
	EINA_LIST_FOREACH(s->rules, l, r)
	{
		Egueb_Css_Engine_State_Selector *ss;
		Egueb_Css_Engine_Selector *s = r->selector;

		ss = malloc(sizeof(Egueb_Css_Engine_State_Selector));
		ss->r = r;
		ss->s = s;
		ss->child = 0;
		ss->sibling = 0;
		state.inactive = eina_list_append(state.inactive, ss);
	}
	/* parse the tree top -> bottom */
	_process_element(c, e, &state);
	/* TODO destroy the state */
}

#endif
