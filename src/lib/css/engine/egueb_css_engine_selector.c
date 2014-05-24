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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Css_Engine_Selector * egueb_css_engine_selector_next_get(Egueb_Css_Engine_Selector *thiz)
{
	Eina_Inlist *l;

	l = EINA_INLIST_GET(thiz)->next;
	if (!l) return NULL;
	return EINA_INLIST_CONTAINER_GET(l, Egueb_Css_Engine_Selector);
}

Egueb_Css_Engine_Selector * egueb_css_engine_selector_prev_get(Egueb_Css_Engine_Selector *thiz)
{
	Eina_Inlist *l;

	l = EINA_INLIST_GET(thiz)->prev;
	if (!l) return NULL;
	return EINA_INLIST_CONTAINER_GET(l, Egueb_Css_Engine_Selector);
}

void egueb_css_engine_selector_dump(Egueb_Css_Engine_Selector *thiz)
{
	Egueb_Css_Engine_Selector *s;
	Eina_Inlist *l;

	printf("%s %p", thiz->subject ? thiz->subject : "*", thiz->filters);
	l = EINA_INLIST_GET(thiz)->next;
	EINA_INLIST_FOREACH(l, s)
	{
		printf(" %c %s %p", s->c, s->subject ? s->subject : "*", thiz->filters);
	}
	printf("\n");
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Css_Engine_Selector * egueb_css_engine_selector_new(void)
{
	Egueb_Css_Engine_Selector *thiz;

	thiz = calloc(1, sizeof(Egueb_Css_Engine_Selector));
	return thiz;
}

EAPI void egueb_css_engine_selector_subject_set(Egueb_Css_Engine_Selector *thiz, const char *subject)
{
	if (!thiz) return;

	if (subject)
		thiz->subject = strdup(subject);
}

EAPI void egueb_css_engine_selector_filter_add(Egueb_Css_Engine_Selector *thiz, Egueb_Css_Engine_Filter *f)
{
	thiz->filters = eina_list_append(thiz->filters, f);
}

EAPI void egueb_css_engine_selector_combinator_set(Egueb_Css_Engine_Selector *thiz, Egueb_Css_Engine_Selector *ss, Egueb_Css_Engine_Combinator c)
{
	if (!thiz) return;

	ss->c = c;
	/* TODO remove the previous on ss in case it is already set */
	/* what to do with the return value */
	eina_inlist_append(EINA_INLIST_GET(thiz), EINA_INLIST_GET(ss));
}
