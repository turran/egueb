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

#ifndef _EGUEB_SMIL_SET_H_
#define _EGUEB_SMIL_SET_H_

EAPI Egueb_Dom_Node * egueb_smil_set_new(void);
EAPI Eina_Bool egueb_smil_is_set(Egueb_Dom_Node *n);
#if 0
EAPI void egueb_smil_element_set_to_set(Ender_Element *e, const char *v);
EAPI void egueb_smil_element_set_to_get(Ender_Element *e, const char **v);
#endif

#endif
