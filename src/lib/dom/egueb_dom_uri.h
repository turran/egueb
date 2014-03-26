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

#ifndef _EGUEB_DOM_URI_H_
#define _EGUEB_DOM_URI_H_

typedef enum _Egueb_Dom_Uri_Type
{
	EGUEB_DOM_URI_TYPE_RELATIVE,
	EGUEB_DOM_URI_TYPE_ABSOLUTE,
} Egueb_Dom_Uri_Type;

typedef struct _Egueb_Dom_Uri
{
	Egueb_Dom_Uri_Type type;
	Egueb_Dom_String *location;
	Egueb_Dom_String *fragment;
} Egueb_Dom_Uri;

EAPI void egueb_dom_uri_cleanup(Egueb_Dom_Uri *thiz);
EAPI Eina_Bool egueb_dom_uri_string_from(Egueb_Dom_Uri *thiz, Egueb_Dom_String *s);
EAPI Eina_Bool egueb_dom_uri_iri_from(Egueb_Dom_Uri *thiz, Egueb_Dom_String *iri);

#endif
