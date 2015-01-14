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
#ifndef _EGUEB_CSS_FILTER_PRIVATE_H_
#define _EGUEB_CSS_FILTER_PRIVATE_H_

typedef Eina_Bool (*Egueb_Css_Engine_Filter_Test)(void *data, Egueb_Dom_Node *n);
typedef void (*Egueb_Css_Engine_Filter_Free)(void *data);

typedef struct _Egueb_Css_Engine_Filter_Descriptor
{
	Egueb_Css_Engine_Filter_Test test;
	Egueb_Css_Engine_Filter_Free free;
} Egueb_Css_Engine_Filter_Descriptor;

Egueb_Css_Engine_Filter * egueb_css_engine_filter_new(Egueb_Css_Engine_Filter_Descriptor *fd, void *data);
Eina_Bool egueb_css_engine_filter_test(Egueb_Css_Engine_Filter *f, Egueb_Dom_Node *n);
void egueb_css_engine_filter_delete(Egueb_Css_Engine_Filter *f);

#endif
