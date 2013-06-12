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
#include "egueb_dom_node.h"
#include "egueb_dom_character_data.h"
#include "egueb_dom_character_data_private.h"
#include "egueb_dom_string_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_DOM_NODE_DESCRIPTOR,
		Egueb_Dom_Character_Data, Egueb_Dom_Character_Data_Class,
		egueb_dom_character_data);

static void _egueb_dom_character_data_class_init(void *k)
{
}

static void _egueb_dom_character_data_class_deinit(void *k)
{

}

static void _egueb_dom_character_data_instance_init(void *o)
{
	Egueb_Dom_Character_Data *thiz;

	thiz = EGUEB_DOM_CHARACTER_DATA(o);
	thiz->buffer = enesim_text_buffer_new();
}

static void _egueb_dom_character_data_instance_deinit(void *o)
{
	Egueb_Dom_Character_Data *thiz;
	thiz = EGUEB_DOM_CHARACTER_DATA(o);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void egueb_dom_character_data_buffer_get(Egueb_Dom_Node *n,
		Enesim_Text_Buffer **buffer)
{
	Egueb_Dom_Character_Data *thiz;

	if (!buffer) return;
	thiz = EGUEB_DOM_CHARACTER_DATA(n);
	*buffer = enesim_text_buffer_ref(thiz->buffer);
}

EAPI void egueb_dom_character_data_buffer_set(Egueb_Dom_Node *n,
		Enesim_Text_Buffer *buffer)
{
	Egueb_Dom_Character_Data *thiz;

	if (!buffer) return;
	thiz = EGUEB_DOM_CHARACTER_DATA(n);
	if (thiz->buffer) enesim_text_buffer_unref(thiz->buffer);
	thiz->buffer = buffer;
}

/* attribute DOMString        data;
   raises(DOMException) on setting
   raises(DOMException) on retrieval
*/
EAPI Eina_Error egueb_dom_character_data_data_get(Egueb_Dom_Node *n,
		Egueb_Dom_String **data)
{
	return EINA_ERROR_NONE;
}

/* readonly attribute unsigned long    length; */
EAPI Eina_Error egueb_dom_character_data_length_get(Egueb_Dom_Node *n,
		int *length)
{
	return EINA_ERROR_NONE;
}

/* void               appendData(in DOMString arg)
   raises(DOMException);
*/
EAPI Eina_Error egueb_dom_character_data_append_data(Egueb_Dom_Node *n,
		Egueb_Dom_String *data)
{
	Egueb_Dom_Character_Data *thiz;
	const char *str;
	int len;

	if (!data) return EGUEB_DOM_ERROR_NOT_FOUND;
	str = egueb_dom_string_string_get(data);
	if (!str) return EGUEB_DOM_ERROR_NOT_FOUND;

	thiz = EGUEB_DOM_CHARACTER_DATA(n);
	len = enesim_text_buffer_string_length(thiz->buffer);
	enesim_text_buffer_string_insert(thiz->buffer, str, -1, len);
	egueb_dom_string_unref(data);
	return EINA_ERROR_NONE;
}

EAPI Eina_Error egueb_dom_character_data_append_data_inline(Egueb_Dom_Node *n,
		const char *data)
{
	Egueb_Dom_String *s;

	s = egueb_dom_string_new_with_static_string(data);
	egueb_dom_character_data_append_data(n, s);
	return EINA_ERROR_NONE;
}

#if 0
  DOMString          substringData(in unsigned long offset, 
                                   in unsigned long count)
                                        raises(DOMException);
  void               insertData(in unsigned long offset, 
                                in DOMString arg)
                                        raises(DOMException);
  void               deleteData(in unsigned long offset, 
                                in unsigned long count)
                                        raises(DOMException);
  void               replaceData(in unsigned long offset, 
                                 in unsigned long count, 
                                 in DOMString arg)
                                        raises(DOMException);
#endif
