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

/**
 * @file
 * @ender_group{Egueb_Dom_Character_Data}
 */

/**
 * @defgroup Egueb_Dom_Character_Data Character Data
 * @brief Character Data @ender_inherits{Egueb_Dom_Node}
 * @ingroup Egueb_Dom_Group
 * @{
 */

EAPI Enesim_Text_Buffer * egueb_dom_character_data_buffer_get(Egueb_Dom_Node *n);
EAPI void egueb_dom_character_data_buffer_set(Egueb_Dom_Node *n,
		Enesim_Text_Buffer *buffer);

EAPI Egueb_Dom_String * egueb_dom_character_data_data_get(Egueb_Dom_Node *n);
EAPI int egueb_dom_character_data_length_get(Egueb_Dom_Node *n);
EAPI Eina_Bool egueb_dom_character_data_data_append(Egueb_Dom_Node *n,
		Egueb_Dom_String *data, Eina_Error *err);
EAPI Eina_Bool egueb_dom_character_data_data_delete(Egueb_Dom_Node *n,
		int offset, int count, Eina_Error *err);

/**
 * @}
 */

#endif
