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
#include "egueb_smil_private.h"
#include "egueb_smil_main.h"
#include "egueb_smil_animate.h"
#include "egueb_smil_animation_private.h"
#include "egueb_smil_animate_base_private.h"
#include "egueb_smil_clock.h"

#include "egueb_smil_keyframe_private.h"
#include "egueb_smil_timeline_private.h"
#include "egueb_smil_event_timeline_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SMIL_ANIMATE_DESCRIPTOR egueb_smil_animate_descriptor_get()
#define EGUEB_SMIL_ANIMATE_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, Egueb_Smil_Animate_Class, EGUEB_SMIL_ANIMATE_DESCRIPTOR)
#define EGUEB_SMIL_ANIMATE(o) ENESIM_OBJECT_INSTANCE_CHECK(o, Egueb_Smil_Animate, EGUEB_SMIL_ANIMATE_DESCRIPTOR)

typedef struct _Egueb_Smil_Animate
{
	Egueb_Smil_Animate_Base base;
	/* properties */
	/* interface */
	/* private */
} Egueb_Smil_Animate;

typedef struct _Egueb_Smil_Animate_Class
{
	Egueb_Smil_Animate_Base_Class parent;
} Egueb_Smil_Animate_Class;
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_smil_animate_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SMIL_NAME_ANIMATE);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SMIL_ANIMATE_BASE_DESCRIPTOR,
		Egueb_Smil_Animate, Egueb_Smil_Animate_Class, egueb_smil_animate);

static void _egueb_smil_animate_class_init(void *k)
{
	Egueb_Dom_Element_Class *e_klass;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_smil_animate_tag_name_get;
}

static void _egueb_smil_animate_instance_init(void *o)
{
}

static void _egueb_smil_animate_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_smil_animate_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_smil_animate);
	return n;
}
