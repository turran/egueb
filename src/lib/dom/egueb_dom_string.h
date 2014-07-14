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

#ifndef _EGUEB_DOM_STRING_H_
#define _EGUEB_DOM_STRING_H_

/**
 * @file
 * @ender_group{Egueb_Dom_String}
 */

/**
 * @defgroup Egueb_Dom_String String
 * @{
 */

typedef struct _Egueb_Dom_String Egueb_Dom_String;

EAPI Egueb_Dom_String * egueb_dom_string_new(void);
EAPI Egueb_Dom_String * egueb_dom_string_new_with_string(const char *str);
EAPI Egueb_Dom_String * egueb_dom_string_new_with_length(const char *str, int len);
EAPI Egueb_Dom_String * egueb_dom_string_new_with_static_string(const char *str);
EAPI Egueb_Dom_String * egueb_dom_string_steal(char *str);
EAPI Egueb_Dom_String * egueb_dom_string_dup(Egueb_Dom_String *thiz);

EAPI Egueb_Dom_String * egueb_dom_string_ref(Egueb_Dom_String *thiz);
EAPI void egueb_dom_string_unref(Egueb_Dom_String *thiz);

EAPI const char * egueb_dom_string_string_get(const Egueb_Dom_String *thiz);
EAPI void egueb_dom_string_string_set(Egueb_Dom_String *thiz, const char *str);
EAPI void egueb_dom_string_static_string_set(Egueb_Dom_String *thiz,
		const char *str);
EAPI void egueb_dom_string_string_steal(Egueb_Dom_String *thiz, char *str);

EAPI Eina_Bool egueb_dom_string_is_equal(const Egueb_Dom_String *thiz,
		const Egueb_Dom_String *other);
EAPI Eina_Bool egueb_dom_string_is_valid(const Egueb_Dom_String *thiz);

/**
 * @}
 */

#endif
