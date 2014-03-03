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
#include "egueb_dom_node.h"
#include "egueb_dom_implementation.h"
#include "egueb_dom_implementation_source.h"

#include "egueb_dom_implementation_source_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Egueb_Dom_Implementation_Source
{
	const Egueb_Dom_Implementation_Source_Descriptor *d;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Dom_String *
egueb_dom_implementation_source_mime_get(Egueb_Dom_Implementation_Source *thiz)
{
	if (thiz->d->mime_get)
		return thiz->d->mime_get();
	return NULL;
}

Egueb_Dom_Implementation *
egueb_dom_implementation_source_implementation_get(Egueb_Dom_Implementation_Source *thiz)
{
	return thiz->d->implementation_get();
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Implementation_Source * egueb_dom_implementation_source_new(
		const Egueb_Dom_Implementation_Source_Descriptor *d)
{
	Egueb_Dom_Implementation_Source *thiz;

	if (!d) return NULL;
	if (!d->implementation_get) return NULL;

	thiz = calloc(1, sizeof(Egueb_Dom_Implementation_Source));
	thiz->d = d;

	return thiz;
}

EAPI void egueb_dom_implementation_source_free(
		Egueb_Dom_Implementation_Source *thiz)
{
	free(thiz);
}
