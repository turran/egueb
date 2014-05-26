/* Egueb
 * Copyright (C) 2011 - 2013 Jorge Luis Zapata
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
#ifndef _EGUEB_CSS_SELECTOR_PRIVATE_H_
#define _EGUEB_CSS_SELECTOR_PRIVATE_H_

typedef struct _Egueb_Css_Engine_Filter Egueb_Css_Engine_Filter;

typedef enum _Egueb_Css_Engine_Combinator
{
	EGUEB_CSS_NONE,
	EGUEB_CSS_DESCENDANT,
	EGUEB_CSS_CHILD,
	EGUEB_CSS_ADJACENT_SIBLING,
	EGUEB_CSS_SIBLING,
} Egueb_Css_Engine_Combinator;

typedef struct _Egueb_Css_Engine_Selector Egueb_Css_Engine_Selector;

Egueb_Css_Engine_Selector * egueb_css_selector_new(void);
void egueb_css_selector_subject_set(Egueb_Css_Engine_Selector *thiz, const char *subject);
void egueb_css_selector_filter_add(Egueb_Css_Engine_Selector *thiz, Egueb_Css_Engine_Filter *f);
void egueb_css_selector_combinator_set(Egueb_Css_Engine_Selector *thiz, Egueb_Css_Engine_Selector *ss, Egueb_Css_Engine_Combinator c);

Egueb_Css_Engine_Filter * egueb_css_filter_class_new(const char *name);
Egueb_Css_Engine_Filter * egueb_css_filter_id_new(const char *name);


typedef struct _Egueb_Css_Engine_Selector_Combinator
{
	Egueb_Css_Engine_Selector *s;
	Egueb_Css_Engine_Combinator c;
} Egueb_Css_Engine_Selector_Combinator;

struct _Egueb_Css_Engine_Selector
{
	EINA_INLIST;
	char *subject;
	Egueb_Css_Engine_Combinator c;
	Eina_List *filters;
};

Egueb_Css_Engine_Selector * egueb_css_engine_selector_next_get(Egueb_Css_Engine_Selector *thiz);
Egueb_Css_Engine_Selector * egueb_css_engine_selector_prev_get(Egueb_Css_Engine_Selector *thiz);
void egueb_css_engine_selector_dump(Egueb_Css_Engine_Selector *thiz);

#endif
