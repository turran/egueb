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

#ifndef _EGUEB_SMIL_ANIMATION_PRIVATE_H_
#define _EGUEB_SMIL_ANIMATION_PRIVATE_H_

#include "Egueb_Xlink.h"

#include "egueb_dom_element_private.h"
#include "egueb_dom_attr_private.h"
#include "egueb_smil_clock.h"
#include "egueb_smil_duration.h"
#include "egueb_smil_fill.h"
#include "egueb_smil_repeat_count.h"
#include "egueb_smil_timing.h"

#include "egueb_smil_keyframe_private.h"
#include "egueb_smil_timeline_private.h"
#include "egueb_smil_attr_duration_private.h"
#include "egueb_smil_attr_fill_private.h"
#include "egueb_smil_attr_repeat_count_private.h"
#include "egueb_smil_attr_timing_list_private.h"

typedef struct _Egueb_Smil_Animation
{
	Egueb_Dom_Element parent;
	/* properties */
	Egueb_Dom_Node *attribute_name;
	Egueb_Dom_Node *fill;
	Egueb_Dom_Node *dur;
	Egueb_Dom_Node *begin;
	Egueb_Dom_Node *end;
	Egueb_Dom_Node *xlink_href;
	Egueb_Dom_Node *repeat_count;
	Egueb_Dom_Node *repeat_dur;

	/* private */
	Eina_List *begin_events;
	Eina_List *end_events;

	/* a weak reference to the target */
	Egueb_Dom_Node *target;
	/* the timeline system */
	Egueb_Smil_Timeline *timeline;
	Egueb_Smil_Signal *signal;

	/* the attribute this animation animates */
	Egueb_Dom_Node *attr;
	/* the descriptor used for parsing the values */
	const Egueb_Dom_Value_Descriptor *d;
	Eina_Bool started;

	/* the animation siblings related to the target */
	EINA_INLIST;
} Egueb_Smil_Animation;

typedef Egueb_Smil_Signal * (*Egueb_Smil_Animation_Class_Setup)(Egueb_Smil_Animation *thiz, Egueb_Dom_Node *parent);
typedef const Egueb_Dom_Value_Descriptor * (*Egueb_Smil_Animation_Class_Value_Descriptor_Get)(Egueb_Smil_Animation *thiz);
typedef void (*Egueb_Smil_Animation_Class_Cleanup)(Egueb_Smil_Animation *thiz, Egueb_Dom_Node *parent);

typedef struct _Egueb_Smil_Animation_Class
{
	Egueb_Dom_Element_Class parent;
	Egueb_Smil_Animation_Class_Value_Descriptor_Get value_descriptor_get;
	Egueb_Smil_Animation_Class_Setup setup;
	Egueb_Smil_Animation_Class_Cleanup cleanup;
} Egueb_Smil_Animation_Class;

extern Egueb_Dom_String *EGUEB_SMIL_ANIMATION_KEY;;

Enesim_Object_Descriptor * egueb_smil_animation_descriptor_get();

#define EGUEB_SMIL_ANIMATION_DESCRIPTOR egueb_smil_animation_descriptor_get()
#define EGUEB_SMIL_ANIMATION_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, Egueb_Smil_Animation_Class, EGUEB_SMIL_ANIMATION_DESCRIPTOR)
#define EGUEB_SMIL_ANIMATION_CLASS_GET(o) EGUEB_SMIL_ANIMATION_CLASS((ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SMIL_ANIMATION(o) ENESIM_OBJECT_INSTANCE_CHECK(o, Egueb_Smil_Animation, EGUEB_SMIL_ANIMATION_DESCRIPTOR)

Eina_Bool egueb_smil_animation_active_duration_get(Egueb_Dom_Node *n, Egueb_Smil_Clock *duration);

#endif
