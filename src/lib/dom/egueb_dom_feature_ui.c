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
#include "egueb_dom_input.h"
#include "egueb_dom_feature.h"
#include "egueb_dom_feature_ui.h"

#include "egueb_dom_node_private.h"
#include "egueb_dom_string_private.h"
#include "egueb_dom_feature_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_FEATURE_UI_DESCRIPTOR egueb_dom_feature_ui_descriptor_get()
#define EGUEB_DOM_FEATURE_UI(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Dom_Feature_UI, EGUEB_DOM_FEATURE_UI_DESCRIPTOR)

typedef struct _Egueb_Dom_Feature_UI
{
	Egueb_Dom_Feature base;
	Egueb_Dom_Node *n;
	const Egueb_Dom_Feature_UI_Descriptor *d;
} Egueb_Dom_Feature_UI;

typedef struct _Egueb_Dom_Feature_UI_Class
{
	Egueb_Dom_Feature_Class base;
} Egueb_Dom_Feature_UI_Class;

static Egueb_Dom_String _EGUEB_DOM_FEATURE_UI_NAME = EGUEB_DOM_STRING_STATIC("EguebDomUI");

/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_FEATURE_DESCRIPTOR,
		Egueb_Dom_Feature_UI, Egueb_Dom_Feature_UI_Class,
		egueb_dom_feature_ui);

static void _egueb_dom_feature_ui_class_init(void *k)
{
}

static void _egueb_dom_feature_ui_instance_init(void *o)
{
}

static void _egueb_dom_feature_ui_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_DOM_FEATURE_UI_NAME = &_EGUEB_DOM_FEATURE_UI_NAME;

EAPI Eina_Bool egueb_dom_feature_ui_input_get(Egueb_Dom_Feature *f, Egueb_Dom_Input **i)
{
	Egueb_Dom_Feature_UI *thiz;

	thiz = EGUEB_DOM_FEATURE_UI(f);
	if (!thiz->n) return EINA_FALSE;

	*i = thiz->d->input_get(thiz->n);
	if (!*i) return EINA_FALSE;

	return EINA_TRUE;
}

EAPI Eina_Bool egueb_dom_feature_ui_add(Egueb_Dom_Node *n,
		const Egueb_Dom_Feature_UI_Descriptor *d)
{
	Egueb_Dom_Feature_UI *thiz;

	if (!n) return EINA_FALSE;
	if (!d) return EINA_FALSE;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_feature_ui);
	egueb_dom_node_weak_ref_add(n, &thiz->n);
	thiz->d = d;

	return egueb_dom_node_feature_add(n, EGUEB_DOM_FEATURE_UI_NAME, NULL, EGUEB_DOM_FEATURE(thiz));
}
