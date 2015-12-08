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
#include "egueb_dom_attr.h"
#include "egueb_dom_value.h"
#include "egueb_dom_character_data.h"
#include "egueb_dom_event.h"
#include "egueb_dom_event_mutation.h"

#include "egueb_dom_event_mutation_private.h"
#include "egueb_dom_character_data_private.h"
#include "egueb_dom_string_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                               Node interface                               *
 *----------------------------------------------------------------------------*/
static void _egueb_dom_character_data_clone(Egueb_Dom_Node *n, Eina_Bool live,
		Eina_Bool deep, Egueb_Dom_Node *clone)
{
	Egueb_Dom_Character_Data *thiz;
	Egueb_Dom_Character_Data *other;
	const char *str;

	thiz = EGUEB_DOM_CHARACTER_DATA(n);
	other = EGUEB_DOM_CHARACTER_DATA(clone);
	/* copy the buffer data */
	str = enesim_text_buffer_string_get(thiz->buffer);
	enesim_text_buffer_string_set(other->buffer, str, -1);
	/* TODO in case of live, for every character data modified event
	 * make sure to copy again on the clone
	 */
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_DOM_NODE_DESCRIPTOR,
		Egueb_Dom_Character_Data, Egueb_Dom_Character_Data_Class,
		egueb_dom_character_data);

static void _egueb_dom_character_data_class_init(void *k)
{
	Egueb_Dom_Node_Class *n_klass = EGUEB_DOM_NODE_CLASS(k);
	n_klass->clone = _egueb_dom_character_data_clone;
}

static void _egueb_dom_character_data_instance_init(void *o)
{
	Egueb_Dom_Character_Data *thiz;

	thiz = EGUEB_DOM_CHARACTER_DATA(o);
	thiz->buffer = enesim_text_buffer_utf8_new(0);
}

static void _egueb_dom_character_data_instance_deinit(void *o)
{
	Egueb_Dom_Character_Data *thiz;
	thiz = EGUEB_DOM_CHARACTER_DATA(o);
	if (thiz->buffer)
		enesim_text_buffer_unref(thiz->buffer);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Text_Buffer * egueb_dom_character_data_buffer_get(Egueb_Dom_Node *n)
{
	Egueb_Dom_Character_Data *thiz;

	thiz = EGUEB_DOM_CHARACTER_DATA(n);
	return enesim_text_buffer_ref(thiz->buffer);
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
EAPI Egueb_Dom_String * egueb_dom_character_data_data_get(Egueb_Dom_Node *n)
{
	Egueb_Dom_Character_Data *thiz;
	const char *content;

	thiz = EGUEB_DOM_CHARACTER_DATA(n);
	content = enesim_text_buffer_string_get(thiz->buffer);
	return egueb_dom_string_new_with_static_string(content);
}

/* readonly attribute unsigned long    length; */
EAPI int egueb_dom_character_data_length_get(Egueb_Dom_Node *n)
{
	Egueb_Dom_Character_Data *thiz;

	thiz = EGUEB_DOM_CHARACTER_DATA(n);
	return enesim_text_buffer_length_get(thiz->buffer);
}

/* void               appendData(in DOMString arg)
   raises(DOMException);
*/
EAPI Eina_Bool egueb_dom_character_data_data_append(Egueb_Dom_Node *n,
		Egueb_Dom_String *data, Eina_Error *err)
{
	Egueb_Dom_Character_Data *thiz;
	Egueb_Dom_Event *event;
	const char *str;
	int len;

	if (!data)
	{
		if (err) *err = EGUEB_DOM_ERROR_NOT_FOUND;
		return EINA_FALSE;
	}

	str = egueb_dom_string_string_get(data);
	if (!str)
	{
		if (err) *err = EGUEB_DOM_ERROR_NOT_FOUND;
		egueb_dom_string_unref(data);
		return EINA_FALSE;
	}

	thiz = EGUEB_DOM_CHARACTER_DATA(n);
	len = enesim_text_buffer_length_get(thiz->buffer);
	enesim_text_buffer_string_insert(thiz->buffer, str, -1, len);
	egueb_dom_string_unref(data);

	event = egueb_dom_event_mutation_character_data_modified_new();
	egueb_dom_event_target_event_dispatch(
			EGUEB_DOM_EVENT_TARGET(thiz), event, NULL, NULL);

	return EINA_TRUE;
}

/* void               deleteData(in unsigned long offset,
                                in unsigned long count)
                                        raises(DOMException);
 */
EAPI Eina_Bool egueb_dom_character_data_data_delete(Egueb_Dom_Node *n,
		int offset, int count, Eina_Error *err)
{
	Egueb_Dom_Character_Data *thiz;
	Egueb_Dom_Event *event;

	thiz = EGUEB_DOM_CHARACTER_DATA(n);
	enesim_text_buffer_string_delete(thiz->buffer, count, offset);
	event = egueb_dom_event_mutation_character_data_modified_new();
	egueb_dom_event_target_event_dispatch(
			EGUEB_DOM_EVENT_TARGET(thiz), event, NULL, NULL);
	return EINA_TRUE;
}

#if 0
  DOMString          substringData(in unsigned long offset,
                                   in unsigned long count)
                                        raises(DOMException);
  void               insertData(in unsigned long offset,
                                in DOMString arg)
                                        raises(DOMException);
  void               replaceData(in unsigned long offset,
                                 in unsigned long count,
                                 in DOMString arg)
                                        raises(DOMException);
#endif
