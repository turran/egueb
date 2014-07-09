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

#ifndef _EGUEB_DOM_UTILS_H_
#define _EGUEB_DOM_UTILS_H_

/* FIXME all this generic parsers should have a common prefix
 * and a common attribute name
 */

#define EGUEB_DOM_IS_HEXA(c) \
	(((c >= 48) && (c <= 57)) || \
	((c >= 65) && (c <= 70)) || \
	((c >= 97) && (c <= 102)))

#define EGUEB_DOM_SPACE_SKIP(t) \
	while (*t) { if ((*t == ' ') || (*t == '\n') || (*t == '\t') || (*t == '\r')) t++; else break; }

#define EGUEB_DOM_SPACE_COMMA_SKIP(t) \
	EGUEB_DOM_SPACE_SKIP(t); \
	if (*t == ',') t++; \
	EGUEB_DOM_SPACE_SKIP(t);

typedef void (*Egueb_Base_List_Cb)(const char *attr, void *data);

EAPI Eina_Bool egueb_dom_long_get(const char *iter, const char **tmp, long *l);
EAPI Eina_Bool egueb_dom_double_get(const char *nptr, char **endptr, double *r);
EAPI Eina_Bool egueb_dom_list_get(const char *attr, char sep, Egueb_Base_List_Cb cb, void *data);
EAPI Eina_Bool egueb_dom_function_get(const char *attr_val, const char **endptr,
		const char *funcname, int *numelements, double *elements);

#endif
