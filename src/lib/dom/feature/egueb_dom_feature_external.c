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
#include "egueb_dom_feature_external.h"
#include "egueb_dom_event_io.h"
#include "egueb_dom_document.h"

#include "egueb_dom_node_private.h"
#include "egueb_dom_string_private.h"
#include "egueb_dom_feature_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_FEATURE_EXTERNAL_DESCRIPTOR egueb_dom_feature_external_descriptor_get()
#define EGUEB_DOM_FEATURE_EXTERNAL(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Dom_Feature_External, EGUEB_DOM_FEATURE_EXTERNAL_DESCRIPTOR)

typedef struct _Egueb_Dom_Feature_External
{
	Egueb_Dom_Feature base;
	Egueb_Dom_Feature_External_Descriptor d;
	void *data;
} Egueb_Dom_Feature_External;

typedef struct _Egueb_Dom_Feature_External_Class
{
	Egueb_Dom_Feature_Class base;
} Egueb_Dom_Feature_External_Class;

static void _egueb_dom_feature_external_init(Egueb_Dom_Feature_External *thiz)
{
	if (!thiz->d.init)
		return;
	thiz->d.init(thiz->data);
}
/*----------------------------------------------------------------------------*
 *                              Feature interface                             *
 *----------------------------------------------------------------------------*/
static const char * _egueb_dom_feature_external_type_get(Egueb_Dom_Feature *f)
{
	Egueb_Dom_Feature_External *thiz;

	thiz = EGUEB_DOM_FEATURE_EXTERNAL(f);
	if (thiz->d.type_get)
		return thiz->d.type_get(thiz->data);
	return "egueb.dom.feature";
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_FEATURE_DESCRIPTOR,
		Egueb_Dom_Feature_External, Egueb_Dom_Feature_External_Class,
		egueb_dom_feature_external);

static void _egueb_dom_feature_external_class_init(void *k)
{
	Egueb_Dom_Feature_Class *klass;

	klass = EGUEB_DOM_FEATURE_CLASS(k);
	klass->type_get = _egueb_dom_feature_external_type_get;
}

static void _egueb_dom_feature_external_instance_init(void *o)
{
}

static void _egueb_dom_feature_external_instance_deinit(void *o)
{
	Egueb_Dom_Feature_External *thiz;

	thiz = EGUEB_DOM_FEATURE_EXTERNAL(o);
	/* invalidate the descriptor */
	if (thiz->d.deinit)
		thiz->d.deinit(thiz->data);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Feature * egueb_dom_feature_external_new(
		const Egueb_Dom_Feature_External_Descriptor *d, void *data)
{
	Egueb_Dom_Feature_External *thiz;
	Egueb_Dom_Feature *f;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_feature_external);
	thiz->d = *d;
	thiz->data = data;

	/* now initialize it */
	_egueb_dom_feature_external_init(thiz);

	f = EGUEB_DOM_FEATURE(thiz);
	return f;
}

EAPI void * egueb_dom_feature_external_data_get(Egueb_Dom_Feature *f)
{
	Egueb_Dom_Feature_External *thiz;

	thiz = EGUEB_DOM_FEATURE_EXTERNAL(f);
	return thiz->data;
}
