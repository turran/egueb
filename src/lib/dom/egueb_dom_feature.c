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
Egueb_Dom_Feature * egueb_dom_feature_ref(Egueb_Dom_Feature *thiz)
{
	if (!thiz) return NULL;

	thiz->ref++;
	return thiz;
}

void egueb_dom_feature_unref(Egueb_Dom_Feature *thiz)
{
	if (!thiz) return;
	thiz->ref--;
	if (!thiz->ref)
	{
		enesim_object_instance_free(ENESIM_OBJECT_INSTANCE(thiz));
	}
}
