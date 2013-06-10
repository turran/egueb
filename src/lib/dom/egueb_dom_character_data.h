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

#ifndef _EGUEB_DOM_CHARACTER_DATA_H_
#define _EGUEB_DOM_CHARACTER_DATA_H_

EAPI Eina_Error egueb_dom_character_data_data_get(Egueb_Dom_Node *n,
		Egueb_Dom_String **data);
EAPI Eina_Error egueb_dom_character_data_length_get(Egueb_Dom_Node *n,
		int *length);
EAPI Eina_Error egueb_dom_character_data_append_data(Egueb_Dom_Node *n,
		Egueb_Dom_String *data);
EAPI Eina_Error egueb_dom_character_data_append_data_inline(Egueb_Dom_Node *n,
		const char *data);

#endif
