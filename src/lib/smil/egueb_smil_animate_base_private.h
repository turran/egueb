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

#ifndef _EGUEB_SMIL_PRIVATE_ANIMATE_BASE_H_
#define _EGUEB_SMIL_PRIVATE_ANIMATE_BASE_H_

typedef struct _Egueb_Smil_Animate_Base
{
	Egueb_Smil_Animation base;

	/* properties */
	Egueb_Dom_Node *additive;
	Egueb_Dom_Node *calc_mode;
	Egueb_Dom_Node *by;
	Egueb_Dom_Node *to;
	Egueb_Dom_Node *from;
	Egueb_Dom_Node *values;
	Egueb_Dom_Node *key_times;
	Egueb_Dom_Node *key_splines;

	/* etch related data */
	Etch_Animation *etch_a;

	/* private */
	Egueb_Dom_Value dst_value;
	Egueb_Dom_Value prv_value;
	/* In case the animation is 'from by' we need
	 * an addition value
	 */
	Egueb_Dom_Value *add_value;
	Eina_List *generated_values;
	Eina_List *generated_times;
#if 0
	/* properties */
	Egueb_Smil_Animate_Base_Context current;
	/* interface */
	Egueb_Smil_Animate_Base_Descriptor_Internal descriptor;
	/* private */
	Ender_Element *thiz_e;
	Egueb_Smil_Attribute_Type attribute_type;
	Egueb_Smil_Attribute_Animated_Descriptor *d;
	Eina_List *values;
	Eina_List *times;
	void *destination_data;
	void *destination_add;
	void *destination_acc;
	int repeat_count;
	/* parent relation */
	Ender_Element *parent_e;
	Egueb_Dom_Tag *parent_t;
	Ender_Property *p;
	/* for the inheritance */
	void *data;
#endif
} Egueb_Smil_Animate_Base;

typedef struct _Egueb_Smil_Animate_Base_Class
{
	Egueb_Smil_Animation_Class base;
} Egueb_Smil_Animate_Base_Class;

Enesim_Object_Descriptor * egueb_smil_animate_base_descriptor_get();

#define EGUEB_SMIL_ANIMATE_BASE_DESCRIPTOR egueb_smil_animate_base_descriptor_get()
#define EGUEB_SMIL_ANIMATE_BASE_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, Egueb_Smil_Animate_Base_Class, EGUEB_SMIL_ANIMATE_BASE_DESCRIPTOR)
#define EGUEB_SMIL_ANIMATE_BASE_CLASS_GET(o) EGUEB_SMIL_ANIMATE_BASE_CLASS((ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SMIL_ANIMATE_BASE(o) ENESIM_OBJECT_INSTANCE_CHECK(o, Egueb_Smil_Animate_Base, EGUEB_SMIL_ANIMATE_BASE_DESCRIPTOR)

#if 0
#include "egueb_smil_private_attribute_animation.h"

typedef struct _Egueb_Smil_Animate_Base_Context {
	Egueb_Smil_Attribute_Animation_Value value;
	Eina_Bool changed : 1;
} Egueb_Smil_Animate_Base_Context;

typedef Eina_Bool (*Egueb_Smil_Animate_Base_Attribute_Descriptor_Get)(Egueb_Dom_Tag *t,
		const char *name,
		Egueb_Smil_Attribute_Animated_Descriptor **d);

typedef struct _Egueb_Smil_Animate_Base_Descriptor {
	/* the tag interface */
	Egueb_Dom_Tag_Attribute_Get attribute_get;
	Egueb_Dom_Tag_Free free;
	/* the element interface */
	Egueb_Smil_Element_Initialize initialize;
	Egueb_Smil_Element_Attribute_Set attribute_set;
	/* the animate_base interface */
	Egueb_Smil_Animate_Base_Attribute_Descriptor_Get type_descriptor_get;
} Egueb_Smil_Animate_Base_Descriptor;

Eina_Bool egueb_smil_is_animate_base_internal(Egueb_Dom_Tag *t);
void * egueb_smil_animate_base_data_get(Egueb_Dom_Tag *t);
Egueb_Dom_Tag * egueb_smil_animate_base_new(Egueb_Smil_Animate_Base_Descriptor *descriptor, Egueb_Smil_Type type, void *data);

Etch_Interpolator_Type egueb_smil_animate_base_calc_mode_etch_to(Egueb_Smil_Calc_Mode c);
/* generated function */
void egueb_smil_animate_base_init(void);
void egueb_smil_animate_base_shutdown(void);
#endif

#endif

