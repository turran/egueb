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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Egueb_Dom_String
{
	char *str;
	int ref;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_String * egueb_dom_string_steal(char *str)
{
	Egueb_Dom_String *thiz;

	thiz = calloc(1, sizeof(Egueb_Dom_String));
	thiz->str = str;
	thiz->ref = 1;
	return thiz;
}

EAPI Egueb_Dom_String * egueb_dom_string_new(const char *str)
{
	Egueb_Dom_String *thiz;

	thiz = calloc(1, sizeof(Egueb_Dom_String));
	thiz->str = strdup(str);
	thiz->ref = 1;
	return thiz;
}

EAPI Egueb_Dom_String * egueb_dom_string_new_with_length(const char *str, int len)
{
	Egueb_Dom_String *thiz;

	thiz = calloc(1, sizeof(Egueb_Dom_String));
	thiz->str = strndup(str, len);
	thiz->ref = 1;
	return thiz;

}

EAPI Egueb_Dom_String * egueb_dom_string_ref(Egueb_Dom_String *thiz)
{
	thiz->ref++;
	return thiz;
}

EAPI void egueb_dom_string_unref(Egueb_Dom_String *thiz)
{
	thiz->ref--;
	if (!thiz->ref)
	{
		free(thiz->str);
		free(thiz);
	}
}

const char * egueb_dom_string_string_get(Egueb_Dom_String *thiz)
{
	return thiz->str;
}
