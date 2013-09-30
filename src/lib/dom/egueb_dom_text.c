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
#include "egueb_dom_text.h"
#include "egueb_dom_character_data_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_TEXT_DESCRIPTOR egueb_dom_text_descriptor_get()
#define EGUEB_DOM_TEXT_CLASS_GET(o) EGUEB_DOM_TEXT_CLASS(		\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_TEXT_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,		\
		Egueb_Dom_Text_Class, EGUEB_DOM_TEXT_DESCRIPTOR)
#define EGUEB_DOM_TEXT(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Dom_Text, EGUEB_DOM_TEXT_DESCRIPTOR)

typedef struct _Egueb_Dom_Text
{
	Egueb_Dom_Character_Data parent;
} Egueb_Dom_Text;

typedef struct _Egueb_Dom_Text_Class
{
	Egueb_Dom_Character_Data_Class parent;
} Egueb_Dom_Text_Class;

/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_CHARACTER_DATA_DESCRIPTOR,
		Egueb_Dom_Text, Egueb_Dom_Text_Class,
		egueb_dom_text);

static void _egueb_dom_text_class_init(void *k)
{
	Egueb_Dom_Node_Class *n_klass = EGUEB_DOM_NODE_CLASS(k);

	n_klass->type = EGUEB_DOM_NODE_TYPE_TEXT_NODE;
}

static void _egueb_dom_text_instance_init(void *o)
{
}

static void _egueb_dom_text_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_dom_text_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_text);
	return n;
}
