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

#include "egueb_dom_main.h"
#include "egueb_dom_utils.h"
#include "egueb_dom_uri.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static const char * _fragment_get(const char *uri)
{
	const char *tmp;
	const char *id;

	/* TODO split the uri, for local or relative */
	/* TODO get the id (#ElementId) */
	/* only check on the ids */
	for (tmp = uri; *tmp && *tmp != '#'; tmp++)

	if (*tmp != '#') return NULL;
	id = tmp + 1;

	return id;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

/*
 * [ <absoluteURI> | <relativeURI> ] [ "#" <elementID> ]
 */
EAPI Eina_Bool egueb_dom_uri_string_from(Egueb_Dom_Uri_Descriptor *descriptor,
		const char *attr, void *data)
{
	const char *fragment;

	EGUEB_DOM_SPACE_SKIP(attr);
	fragment = _fragment_get(attr);
	/* TODO check for the local/non-local case */
	if (*attr == '#')
	{
		if (!fragment) return EINA_FALSE;
		DBG("getting %s", fragment);
		descriptor->local_get(NULL, fragment, data);
	}
	else
	{
		DBG("getting %s %s", attr, fragment);
		/* TODO very naive way of handling relative/absolute, we need the scheme too, etc */
		/* check for the relative/absolute case */
		if (*attr == '/')
			descriptor->absolute_get(attr, fragment, data);
		else
			descriptor->relative_get(attr, fragment, data);
	}
	return EINA_TRUE;
}

EAPI Eina_Bool egueb_dom_iri_string_from(Egueb_Dom_Uri_Descriptor *descriptor,
		const char *attr, void *data)
{
	/* funciri ? */
	EGUEB_DOM_SPACE_SKIP(attr);
	if (!strncmp(attr, "url(", 4))
	{
		char url[PATH_MAX];
		int i = 0;

		attr += 4;
		/* also remove the last spaces */
		while (*attr != ')' && *attr != '\0')
			url[i++] = *attr++;
		url[i] = '\0';
		return egueb_dom_uri_string_from(descriptor, url, data);
	}
	/* iri */
	else
	{
		return egueb_dom_uri_string_from(descriptor, attr, data);
	}
}


