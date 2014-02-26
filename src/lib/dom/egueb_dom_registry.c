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
#include "egueb_dom_node.h"
#include "egueb_dom_implementation.h"
#include "egueb_dom_implementation_source.h"
#include "egueb_dom_registry.h"

#include "egueb_dom_registry_private.h"
#include "egueb_dom_implementation_source_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_List *_sources = NULL;
static Eina_Array *_modules = NULL;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_dom_registry_init(void)
{
	/* load the modules */
	_modules = eina_module_list_get(_modules, PACKAGE_LIB_DIR"/egueb/source/", 1, NULL, NULL);
	eina_module_list_load(_modules);
}

void egueb_dom_registry_shutdown(void)
{
	Egueb_Dom_Implementation_Source *s;

	/* unload them */
	eina_module_list_free(_modules);
	eina_array_free(_modules);
	EINA_LIST_FREE(_sources, s)
		egueb_dom_implementation_source_free(s);
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void egueb_dom_registry_source_add(
		Egueb_Dom_Implementation_Source *s)
{
	_sources = eina_list_append(_sources, s);
}

EAPI Egueb_Dom_Implementation * egueb_dom_registry_implementation_get_by_mime(
		Egueb_Dom_String *mime)
{
	Egueb_Dom_Implementation_Source *s;
	Egueb_Dom_Implementation *i = NULL;
	Eina_List *l;

	EINA_LIST_FOREACH(_sources, l, s)
	{
		i = egueb_dom_implementation_source_implementation_get_by_mime(
				s, mime);
		if (i) break;
	}
	return i;
}
