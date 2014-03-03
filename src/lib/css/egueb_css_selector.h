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
#ifndef _EGUEB_CSS_SELECTOR_H_

typedef struct _Egueb_Css_Filter Egueb_Css_Filter;

typedef enum _Egueb_Css_Combinator
{
	EGUEB_CSS_NONE,
	EGUEB_CSS_DESCENDANT,
	EGUEB_CSS_CHILD,
	EGUEB_CSS_ADJACENT_SIBLING,
	EGUEB_CSS_SIBLING,
} Egueb_Css_Combinator;

typedef struct _Egueb_Css_Selector Egueb_Css_Selector;

EAPI Egueb_Css_Selector * egueb_css_selector_new(void);
EAPI void egueb_css_selector_subject_set(Egueb_Css_Selector *thiz, const char *subject);
EAPI void egueb_css_selector_filter_add(Egueb_Css_Selector *thiz, Egueb_Css_Filter *f);
EAPI void egueb_css_selector_combinator_set(Egueb_Css_Selector *thiz, Egueb_Css_Selector *ss, Egueb_Css_Combinator c);

EAPI Egueb_Css_Filter * egueb_css_filter_class_new(const char *name);
EAPI Egueb_Css_Filter * egueb_css_filter_id_new(const char *name);

#endif
