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

typedef struct _Egueb_Css_Selector_Combinator
{
	Egueb_Css_Selector *s;
	Egueb_Css_Combinator c;
} Egueb_Css_Selector_Combinator;

struct _Egueb_Css_Selector
{
	EINA_INLIST;
	char *subject;
	Egueb_Css_Combinator c;
	Eina_List *filters;
};

Egueb_Css_Selector * egueb_css_selector_next_get(Egueb_Css_Selector *thiz);
Egueb_Css_Selector * egueb_css_selector_prev_get(Egueb_Css_Selector *thiz);
void egueb_css_selector_dump(Egueb_Css_Selector *thiz);

#endif
