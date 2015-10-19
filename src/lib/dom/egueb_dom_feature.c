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
#include "egueb_dom_feature.h"

#include "egueb_dom_feature_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(ENESIM_OBJECT_DESCRIPTOR, Egueb_Dom_Feature,
		Egueb_Dom_Feature_Class, egueb_dom_feature);

static void _egueb_dom_feature_class_init(void *k)
{
}

static void _egueb_dom_feature_instance_init(void *o)
{
	Egueb_Dom_Feature *thiz;

	thiz = EGUEB_DOM_FEATURE(o);
	thiz->ref = 1;
}

static void _egueb_dom_feature_instance_deinit(void *o)
{
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Feature * egueb_dom_feature_ref(Egueb_Dom_Feature *thiz)
{
	if (!thiz) return NULL;

	thiz->ref++;
	return thiz;
}

EAPI void egueb_dom_feature_unref(Egueb_Dom_Feature *thiz)
{
	if (!thiz) return;
	thiz->ref--;
	if (!thiz->ref)
	{
		enesim_object_instance_free(ENESIM_OBJECT_INSTANCE(thiz));
	}
}

/**
 * @brief Get the type of a feature 
 * @ender_downcast
 * @param[in] thiz The feature to get the type from
 * @param[out] lib The ender library associated with this feature
 * @param[out] name The ender item name of the feature
 * @return EINA_TRUE if the function succeeds, EINA_FALSE otherwise
 *
 * This function is needed for ender in order to downcast a feature
 */
EAPI Eina_Bool egueb_dom_feature_type_get(Egueb_Dom_Feature *thiz,
		const char **lib, const char **name)
{
	if (!thiz)
		return EINA_FALSE;

	if (lib)
		*lib = "egueb-dom";
	if (name)
	{
		Egueb_Dom_Feature_Class *klass;
		klass = EGUEB_DOM_FEATURE_CLASS_GET(thiz);

		if (klass->type_get)
			*name = klass->type_get(thiz);
		else
			*name = "egueb.dom.feature";
	}
	return EINA_TRUE;
}
