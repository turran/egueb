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

#include "egueb_svg_main_private.h"
#include "egueb_svg_reference.h"
#include "egueb_svg_referenceable.h"
#include "egueb_svg_reference_private.h"
#include "egueb_svg_referenceable_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(ENESIM_OBJECT_DESCRIPTOR, Egueb_Svg_Reference,
		Egueb_Svg_Reference_Class, egueb_svg_reference);

static void _egueb_svg_reference_class_init(void *k)
{
}

static void _egueb_svg_reference_instance_init(void *o)
{
}

static void _egueb_svg_reference_instance_deinit(void *o)
{
	Egueb_Svg_Reference *thiz = EGUEB_SVG_REFERENCE(o);
	/* decide what to do with the referencer and the referenceable
	 * refs, keep it, own it?
	 */
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_reference_init(Egueb_Svg_Reference *thiz,
		Egueb_Dom_Node *referenceable, Egueb_Dom_Node *referencer)
{
	Egueb_Svg_Reference_Class *klass;

	klass = EGUEB_SVG_REFERENCE_CLASS_GET(thiz);
	/* TODO add weak references */
	thiz->referenceable = egueb_dom_node_ref(referenceable);
	thiz->referencer = referencer;
	if (klass->setup)
		klass->setup(thiz);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_svg_reference_process(Egueb_Svg_Reference *thiz)
{
	Egueb_Svg_Reference_Class *klass;

	klass = EGUEB_SVG_REFERENCE_CLASS_GET(thiz);
	if (!thiz->referenceable)
	{
		WARN("No referenceable set");
		return EINA_FALSE;
	}
	if (!thiz->referencer)
	{
		WARN("No referencer set");
		return EINA_FALSE;
	}

	if (klass->process)
		return klass->process(thiz);
	return EINA_TRUE;
}

EAPI void egueb_svg_reference_referencer_get(Egueb_Svg_Reference *thiz,
		Egueb_Dom_Node **referencer)
{
	if (!referencer) return;
	if (thiz->referencer)
		*referencer = egueb_dom_node_ref(thiz->referencer);
	else
		*referencer = NULL;
}

EAPI void egueb_svg_reference_free(Egueb_Svg_Reference *thiz)
{
	/* unlink the referencer */
	if (thiz->referencer)
	{
		if (thiz->destroy_cb)
			thiz->destroy_cb(thiz, thiz->referencer);
		thiz->referencer = NULL;
	}
	/* unlink the referenceable */
	if (thiz->referenceable)
	{
		egueb_svg_referenceable_reference_remove(thiz->referenceable, thiz);
		egueb_dom_node_unref(thiz->referenceable);
		thiz->referenceable = NULL;
	}
	/* finally remove it */
	enesim_object_instance_free(ENESIM_OBJECT_INSTANCE(thiz));
}
