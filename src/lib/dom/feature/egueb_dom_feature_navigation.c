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
#include "egueb_dom_feature.h"
#include "egueb_dom_feature_navigation.h"

#include "egueb_dom_node_private.h"
#include "egueb_dom_string_private.h"
#include "egueb_dom_feature_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_FEATURE_NAVIGATION_DESCRIPTOR egueb_dom_feature_navigation_descriptor_get()
#define EGUEB_DOM_FEATURE_NAVIGATION(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Dom_Feature_Navigation, EGUEB_DOM_FEATURE_NAVIGATION_DESCRIPTOR)

typedef struct _Egueb_Dom_Feature_Navigation
{
	Egueb_Dom_Feature base;
	Egueb_Dom_Node *n;
} Egueb_Dom_Feature_Navigation;

typedef struct _Egueb_Dom_Feature_Navigation_Class
{
	Egueb_Dom_Feature_Class base;
} Egueb_Dom_Feature_Navigation_Class;

static Egueb_Dom_String _EGUEB_DOM_FEATURE_NAVIGATION_NAME = EGUEB_DOM_STRING_STATIC("EguebDomNavigation");

/*----------------------------------------------------------------------------*
 *                              Feature interface                             *
 *----------------------------------------------------------------------------*/
static const char * _egueb_dom_feature_navigation_type_get(Egueb_Dom_Feature *f)
{
	return "egueb.dom.feature.navigation";
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_FEATURE_DESCRIPTOR,
		Egueb_Dom_Feature_Navigation, Egueb_Dom_Feature_Navigation_Class,
		egueb_dom_feature_navigation);

static void _egueb_dom_feature_navigation_class_init(void *k)
{
	Egueb_Dom_Feature_Class *klass;

	klass = EGUEB_DOM_FEATURE_CLASS(k);
	klass->type_get = _egueb_dom_feature_navigation_type_get;
}

static void _egueb_dom_feature_navigation_instance_init(void *o)
{
}

static void _egueb_dom_feature_navigation_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_DOM_FEATURE_NAVIGATION_NAME = &_EGUEB_DOM_FEATURE_NAVIGATION_NAME;

EAPI Eina_Bool egueb_dom_feature_navigation_add(Egueb_Dom_Node *n)
{
	Egueb_Dom_Feature_Navigation *thiz;

	if (!n) return EINA_FALSE;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_feature_navigation);
	egueb_dom_node_weak_ref_add(n, &thiz->n);

	return egueb_dom_node_feature_add(n, EGUEB_DOM_FEATURE_NAVIGATION_NAME, NULL, EGUEB_DOM_FEATURE(thiz));
}

