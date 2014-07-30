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

#include "egueb_smil_keyframe.h"
#include "egueb_smil_timeline.h"
#include "egueb_smil_signal.h"

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

	/* timeline related data */
	Egueb_Smil_Signal *signal;
	Egueb_Smil_Keyframe_Interpolator_Type itype;

	/* private */
	Egueb_Dom_Value dst_value;
	Egueb_Dom_Value prv_value;
	/* In case the animation is 'from by' we need
	 * an addition value
	 */
	Egueb_Dom_Value *add_value;
	Eina_List *generated_values;
	Eina_List *generated_times;
	Eina_List *generated_keysplines;
} Egueb_Smil_Animate_Base;

typedef Eina_Bool (*Egueb_Smil_Animate_Base_Interpolate)(Egueb_Smil_Animate_Base *thiz, Egueb_Dom_Value *va, Egueb_Dom_Value *vb, double m, Egueb_Dom_Value *add, Egueb_Dom_Value *mul, int accum);

typedef struct _Egueb_Smil_Animate_Base_Class
{
	Egueb_Smil_Animation_Class base;
	Egueb_Smil_Animate_Base_Interpolate interpolate;
} Egueb_Smil_Animate_Base_Class;

Enesim_Object_Descriptor * egueb_smil_animate_base_descriptor_get();

#define EGUEB_SMIL_ANIMATE_BASE_DESCRIPTOR egueb_smil_animate_base_descriptor_get()
#define EGUEB_SMIL_ANIMATE_BASE_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, Egueb_Smil_Animate_Base_Class, EGUEB_SMIL_ANIMATE_BASE_DESCRIPTOR)
#define EGUEB_SMIL_ANIMATE_BASE_CLASS_GET(o) EGUEB_SMIL_ANIMATE_BASE_CLASS((ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SMIL_ANIMATE_BASE(o) ENESIM_OBJECT_INSTANCE_CHECK(o, Egueb_Smil_Animate_Base, EGUEB_SMIL_ANIMATE_BASE_DESCRIPTOR)

#endif
