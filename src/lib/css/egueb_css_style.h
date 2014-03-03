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
#ifndef _EGUEB_CSS_STYLE_H_

typedef struct _Egueb_Css_Style Egueb_Css_Style;

EAPI Egueb_Css_Style * egueb_css_style_new(void);
EAPI Egueb_Css_Style * egueb_css_style_load_from_file(const char *file);
EAPI Egueb_Css_Style * egueb_css_style_load_from_content(const char *content, size_t len);
EAPI void egueb_css_style_rule_add(Egueb_Css_Style *thiz, Egueb_Css_Rule *r);

#endif
