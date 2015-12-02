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
/* TODO we should create a tree for easy matching if a key is or not printable
 * like:
 * input = Return
 * first check if there is a non-printable string that starts with R, then
 * if the next char can be e, then t, and so on ...
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Egueb_Dom_Key_Dict
{
	Eina_Inlist *roots;
} Egueb_Dom_Key_Dict;

typedef struct _Egueb_Dom_Key_Dict_Entry
{
	EINA_INLIST;
	Eina_Unicode k;
	Eina_Inlist *children;
} Egueb_Dom_Key_Dict_Entry;


static Eina_Bool _egueb_dom_key_dict_exists_recursive(Egueb_Dom_Key_Dict_Entry *entry,
		const char *buf, int **index)
{
	return EINA_FALSE;
}

static Egueb_Dom_Key_Dict * _egueb_dom_key_dict_new(void)
{
	return NULL;
}

static void _egueb_dom_key_dict_free(Egueb_Dom_Key_Dict *thiz)
{

}


static void _egueb_dom_key_dict_add(const char *str)
{

}

static Eina_Bool _egueb_dom_key_dict_exists(Egueb_Dom_Key_Dict *thiz,
		const char *str)
{
	/* iterate over the roots to find the first char */
	/* if not found, return false */
	/* if found, continue with the next entry */
	return EINA_FALSE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_dom_key_init(void)
{
	/* add every non-printable string */ 
}

void egueb_dom_key_shutdown(void)
{

}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_dom_key_is_printable(Egueb_Dom_String *s)
{
	/* check if the string exists on the non_printable tree */
	return EINA_FALSE;
}
