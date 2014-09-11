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

#ifndef _EGUEB_CSS_FONT_WEIGHT_PRIVATE_H_
#define _EGUEB_CSS_FONT_WEIGHT_PRIVATE_H_

Eina_Bool egueb_css_font_weight_string_from(Egueb_Css_Font_Weight *thiz, const char *attr_val);
char * egueb_css_font_weight_string_to(Egueb_Css_Font_Weight thiz);

#endif

