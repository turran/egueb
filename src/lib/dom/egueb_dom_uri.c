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
#include "egueb_dom_uri.h"
#include "egueb_dom_utils.h"
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
EAPI Eina_Bool egueb_dom_uri_string_from(Egueb_Dom_Uri *thiz, Egueb_Dom_String *s)
{
	const char *s_s;
	const char *fragment;

	s_s = egueb_dom_string_string_get(s);
	EGUEB_DOM_SPACE_SKIP(s_s);

	fragment = _fragment_get(s_s);
	/* TODO check for the local/non-local case */
	if (*s_s == '#')
	{
		if (!fragment) return EINA_FALSE;

		DBG("Getting URI fragment %s", fragment);
		thiz->type = EGUEB_DOM_URI_TYPE_RELATIVE;
		thiz->location = NULL;
		thiz->fragment = egueb_dom_string_new_with_string(fragment);
	}
	else
	{
		DBG("Getting URI from %s %s", s_s, fragment);

		/* check for the relative/absolute case */
		/* TODO very naive way of handling relative/absolute, we need the scheme too, etc */
		if (strstr(s_s, "://"))
		{
			thiz->type = EGUEB_DOM_URI_TYPE_ABSOLUTE;
			if (fragment)
			{
				thiz->location = egueb_dom_string_new_with_length(s_s, fragment - s_s - 1);
				thiz->fragment = egueb_dom_string_new_with_string(fragment);
			}
			else
			{
				thiz->location = egueb_dom_string_new_with_string(s_s);
				thiz->fragment = NULL;
			}
		}
		else
		{
			thiz->type = EGUEB_DOM_URI_TYPE_RELATIVE;
			if (fragment)
			{
				thiz->location = egueb_dom_string_new_with_length(s_s, fragment - s_s - 1);
				thiz->fragment = egueb_dom_string_new_with_string(fragment);
			}
			else
			{
				thiz->location = egueb_dom_string_new_with_string(s_s);
				thiz->fragment = NULL;
			}
		}
	}
	return EINA_TRUE;

}

EAPI Eina_Bool egueb_dom_uri_iri_from(Egueb_Dom_Uri *thiz, Egueb_Dom_String *iri)
{
	Eina_Bool ret;
	const char *s_iri;

	s_iri = egueb_dom_string_string_get(iri);
	EGUEB_DOM_SPACE_SKIP(s_iri);

	/* funciri ? */
	if (!strncmp(s_iri, "url(", 4))
	{
		Egueb_Dom_String *url;
		const char *end;

		s_iri += 4;
		end = s_iri;
		/* also remove the last spaces */
		while (*end != ')' && *end != '\0')
			end++;
		url = egueb_dom_string_new_with_length(s_iri, end - s_iri);
		ret = egueb_dom_uri_string_from(thiz, url);
		egueb_dom_string_unref(url);
		return ret;
	}
	/* iri */
	else
	{
		return egueb_dom_uri_string_from(thiz, iri);
	}

}

EAPI void egueb_dom_uri_cleanup(Egueb_Dom_Uri *thiz)
{
	if (thiz->location)
	{
		egueb_dom_string_unref(thiz->location);
		thiz->location = NULL;
	}
	if (thiz->fragment)
	{
		egueb_dom_string_unref(thiz->fragment);
		thiz->fragment = NULL;
	}
}
