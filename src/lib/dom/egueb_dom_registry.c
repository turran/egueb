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
#include "egueb_dom_list.h"
#include "egueb_dom_value_dom_string.h"
#include "egueb_dom_node.h"
#include "egueb_dom_implementation.h"
#include "egueb_dom_registry.h"

#include "egueb_dom_registry_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_List *_implementations = NULL;
static Eina_Array *_modules = NULL;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_dom_registry_init(void)
{
	/* load the modules */
	_modules = eina_module_list_get(_modules,
			PACKAGE_LIB_DIR"/egueb/implementation/", 1, NULL, NULL);
	eina_module_list_load(_modules);
}

void egueb_dom_registry_shutdown(void)
{
	Egueb_Dom_Implementation *i;

	/* unload them */
	eina_module_list_free(_modules);
	eina_array_free(_modules);
	EINA_LIST_FREE(_implementations, i)
		egueb_dom_implementation_unref(i);
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void egueb_dom_registry_implementation_add(
		Egueb_Dom_Implementation *i)
{
	_implementations = eina_list_append(_implementations, i);
}

EAPI Egueb_Dom_List * egueb_dom_registry_mime_get (void)
{
	Egueb_Dom_Implementation *i = NULL;
	Egueb_Dom_List *ret;
	Eina_List *l;

	ret = egueb_dom_list_new(egueb_dom_value_dom_string_descriptor_get());
	EINA_LIST_FOREACH(_implementations, l, i)
	{
		Egueb_Dom_List *mimes;
		Egueb_Dom_String *i_mime;
		Eina_Iterator *it;

		mimes = egueb_dom_implementation_mime_get(i);
		it = egueb_dom_list_iterator_new(mimes);
		EINA_ITERATOR_FOREACH(it, i_mime)
		{
			egueb_dom_list_item_append(ret,
					egueb_dom_string_ref(i_mime));
		}
		eina_iterator_free(it);
		egueb_dom_list_unref(mimes);
	}

	return ret;
}

EAPI Egueb_Dom_Implementation * egueb_dom_registry_implementation_get_by_mime(
		Egueb_Dom_String *mime)
{
	Egueb_Dom_Implementation *ret = NULL;
	Egueb_Dom_Implementation *i = NULL;
	Eina_List *l;

	EINA_LIST_FOREACH(_implementations, l, i)
	{
		Egueb_Dom_List *mimes;
		Egueb_Dom_String *i_mime;
		Eina_Iterator *it;
		Eina_Bool equal = EINA_FALSE;

		mimes = egueb_dom_implementation_mime_get(i);
		it = egueb_dom_list_iterator_new (mimes);
		EINA_ITERATOR_FOREACH(it, i_mime)
		{
			equal = egueb_dom_string_is_equal(mime, i_mime);
			if (equal)
				break;
		}
		eina_iterator_free(it);
		egueb_dom_list_unref(mimes);
		if (equal)
		{
			ret = i;
			break;
		}
	}
	return ret;
}
