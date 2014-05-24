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
#ifndef _EGUEB_CSS_CONTEXT_H_

typedef void (*Egueb_Css_Engine_Context_Attribute_Set)(void *e, const char *attribute, const char *value);
typedef const char * (*Egueb_Css_Engine_Context_Attribute_Get)(void *e, const char *attribute);
typedef void * (*Egueb_Css_Engine_Context_Get_Child)(void *e);
typedef void * (*Egueb_Css_Engine_Context_Get_Next_Sibling)(void *e);
typedef const char * (*Egueb_Css_Engine_Context_Get_Name)(void *e);

typedef struct _Egueb_Css_Engine_Context
{
	Egueb_Css_Engine_Context_Attribute_Set attribute_set;
	Egueb_Css_Engine_Context_Attribute_Get attribute_get;
	Egueb_Css_Engine_Context_Get_Name get_name;
	Egueb_Css_Engine_Context_Get_Child get_child;
	Egueb_Css_Engine_Context_Get_Next_Sibling get_next_sibling;
} Egueb_Css_Engine_Context;

#if 0
EAPI void egueb_css_engine_context_style_apply(Egueb_Css_Engine_Context *c, Egueb_Css_Engine_Style *s, void *element);
EAPI void egueb_css_engine_context_inline_style_apply(Egueb_Css_Engine_Context *c, const char *style, void *element);
#endif

#endif
