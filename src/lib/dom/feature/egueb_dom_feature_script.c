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
#include "egueb_dom_uri.h"
#include "egueb_dom_feature.h"
#include "egueb_dom_feature_script.h"
#include "egueb_dom_event_io.h"
#include "egueb_dom_document.h"

#include "egueb_dom_node_private.h"
#include "egueb_dom_string_private.h"
#include "egueb_dom_feature_private.h"

#include <libgen.h>
#include <stdio.h>
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_FEATURE_SCRIPT_DESCRIPTOR egueb_dom_feature_script_descriptor_get()
#define EGUEB_DOM_FEATURE_SCRIPT(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Dom_Feature_Script, EGUEB_DOM_FEATURE_SCRIPT_DESCRIPTOR)

typedef struct _Egueb_Dom_Feature_Script
{
	Egueb_Dom_Feature base;
	Egueb_Dom_Node *n;
} Egueb_Dom_Feature_Script;

typedef struct _Egueb_Dom_Feature_Script_Class
{
	Egueb_Dom_Feature_Class base;
} Egueb_Dom_Feature_Script_Class;

static Egueb_Dom_String _EGUEB_DOM_FEATURE_SCRIPT_NAME = EGUEB_DOM_STRING_STATIC("EguebDomScript");

static void _egueb_dom_feature_script_node_destroyed_cb(Egueb_Dom_Event *ev,
		void *data)
{
	Egueb_Dom_Feature_Script *thiz = data;
	thiz->n = NULL;
}

/*----------------------------------------------------------------------------*
 *                              Feature interface                             *
 *----------------------------------------------------------------------------*/
static const char * _egueb_dom_feature_script_type_get(Egueb_Dom_Feature *f)
{
	return "egueb.dom.feature.script";
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_FEATURE_DESCRIPTOR,
		Egueb_Dom_Feature_Script, Egueb_Dom_Feature_Script_Class,
		egueb_dom_feature_script);

static void _egueb_dom_feature_script_class_init(void *k)
{
	Egueb_Dom_Feature_Class *klass;

	klass = EGUEB_DOM_FEATURE_CLASS(k);
	klass->type_get = _egueb_dom_feature_script_type_get;
}

static void _egueb_dom_feature_script_instance_init(void *o)
{
}

static void _egueb_dom_feature_script_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_DOM_FEATURE_SCRIPT_NAME = &_EGUEB_DOM_FEATURE_SCRIPT_NAME;

EAPI Eina_Bool egueb_dom_feature_script_add(Egueb_Dom_Node *n)
{
	Egueb_Dom_Feature_Script *thiz;

	if (!n) return EINA_FALSE;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_feature_script);
	thiz->n = n;
	egueb_dom_node_weak_ref(n, _egueb_dom_feature_script_node_destroyed_cb, thiz);

	return egueb_dom_node_feature_add(n, EGUEB_DOM_FEATURE_SCRIPT_NAME, NULL,
		EGUEB_DOM_FEATURE(thiz));
}
