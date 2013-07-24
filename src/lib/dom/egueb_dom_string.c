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
#include "egueb_dom_private.h"

#include "egueb_dom_string.h"
#include "egueb_dom_main.h"

#include "egueb_dom_string_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _egueb_dom_string_cleanup(Egueb_Dom_String *thiz)
{
	if (thiz->str && thiz->owned)
		free(thiz->str);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_String * egueb_dom_string_new(void)
{
	Egueb_Dom_String *thiz;

	thiz = calloc(1, sizeof(Egueb_Dom_String));
	thiz->ref = 1;
	return thiz;
}

EAPI Egueb_Dom_String * egueb_dom_string_steal(char *str)
{
	Egueb_Dom_String *thiz;

	thiz = egueb_dom_string_new();
	egueb_dom_string_string_steal(thiz, str);
	return thiz;
}

EAPI Egueb_Dom_String * egueb_dom_string_new_with_static_string(const char *str)
{
	Egueb_Dom_String *thiz;

	thiz = egueb_dom_string_new();
	egueb_dom_string_static_string_set(thiz, str);
	return thiz;
}

EAPI Egueb_Dom_String * egueb_dom_string_new_with_string(const char *str)
{
	Egueb_Dom_String *thiz;

	thiz = egueb_dom_string_new();
	egueb_dom_string_string_set(thiz, str);
	return thiz;
}

EAPI Egueb_Dom_String * egueb_dom_string_new_with_length(const char *str, int len)
{
	Egueb_Dom_String *thiz;

	thiz = egueb_dom_string_new();
	thiz->str = eina_extra_strndup(str, len);
	thiz->owned = EINA_TRUE;
	return thiz;

}

EAPI Egueb_Dom_String * egueb_dom_string_ref(Egueb_Dom_String *thiz)
{
	thiz->ref++;
	return thiz;
}

EAPI void egueb_dom_string_unref(Egueb_Dom_String *thiz)
{
	if (!thiz) return;
	thiz->ref--;
	if (!thiz->ref)
	{
		_egueb_dom_string_cleanup(thiz);
		free(thiz);
	}
}

EAPI const char * egueb_dom_string_string_get(const Egueb_Dom_String *thiz)
{
	if (!thiz) return NULL;
	return thiz->str;
}

EAPI void egueb_dom_string_string_set(Egueb_Dom_String *thiz, const char *str)
{
	_egueb_dom_string_cleanup(thiz);
	thiz->str = strdup(str);
	thiz->owned = EINA_TRUE;
}

EAPI void egueb_dom_string_static_string_set(Egueb_Dom_String *thiz,
		const char *str)
{
	_egueb_dom_string_cleanup(thiz);
	thiz->str = (char *)str;
	thiz->owned = EINA_FALSE;
}

EAPI void egueb_dom_string_string_steal(Egueb_Dom_String *thiz, char *str)
{
	_egueb_dom_string_cleanup(thiz);
	thiz->str = str;
	thiz->owned = EINA_TRUE;
}

EAPI Eina_Bool egueb_dom_string_is_equal(const Egueb_Dom_String *thiz,
		const Egueb_Dom_String *other)
{
	const char *str_thiz;
	const char *str_other;

	if (thiz == other) return EINA_TRUE;
	if (!thiz || !other) return EINA_FALSE;

	str_thiz = egueb_dom_string_string_get(thiz);
	str_other = egueb_dom_string_string_get(other);

	if (!strcmp(str_thiz, str_other))
		return EINA_TRUE;
	return EINA_FALSE;
}

EAPI Eina_Bool egueb_dom_string_is_valid(const Egueb_Dom_String *thiz)
{
	if (!thiz) return EINA_FALSE;
	if (!thiz->str) return EINA_FALSE;
	return EINA_TRUE;
}
