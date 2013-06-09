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

#ifndef _EGUEB_SVG_PRIVATE_REFERENCEABLE_H_
#define _EGUEB_SVG_PRIVATE_REFERENCEABLE_H_

#include "egueb_svg_reference.h"
#include "egueb_svg_referenceable.h"

#include "egueb_svg_element_private.h"
#include "egueb_svg_reference_private.h"

typedef struct _Egueb_Svg_Referenceable
{
	Egueb_Svg_Element base;
	/* private */
	Eina_List *references;
} Egueb_Svg_Referenceable;

typedef Eina_Bool (*Egueb_Svg_Referenceable_Process)(Egueb_Svg_Referenceable *e);
typedef Egueb_Svg_Reference * (*Egueb_Svg_Referenceable_Reference_New)(Egueb_Svg_Referenceable *thiz,
		Egueb_Dom_Node *referencer);

typedef struct _Egueb_Svg_Referenceable_Class
{
	Egueb_Svg_Element_Class base;
	Egueb_Svg_Referenceable_Process process;
	Egueb_Svg_Referenceable_Reference_New reference_new;
} Egueb_Svg_Referenceable_Class;

#define EGUEB_SVG_REFERENCEABLE_DESCRIPTOR egueb_svg_referenceable_descriptor_get()
#define EGUEB_SVG_REFERENCEABLE_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Referenceable_Class, 					\
		EGUEB_SVG_REFERENCEABLE_DESCRIPTOR)
#define EGUEB_SVG_REFERENCEABLE_CLASS_GET(o) EGUEB_SVG_REFERENCEABLE_CLASS(	\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SVG_REFERENCEABLE(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Referenceable, EGUEB_SVG_REFERENCEABLE_DESCRIPTOR)

Enesim_Object_Descriptor *  egueb_svg_referenceable_descriptor_get(void);
void egueb_svg_referenceable_reference_remove(Egueb_Dom_Node *n,
		Egueb_Svg_Reference *r);

#if 0
typedef Eina_Bool (*Egueb_Svg_Referenceable_Reference_Add)(Egueb_Dom_Tag *t, Egueb_Svg_Referenceable_Reference *rr);
typedef void (*Egueb_Svg_Referenceable_Reference_Remove)(Egueb_Dom_Tag *t, Egueb_Svg_Referenceable_Reference *rr);

typedef Egueb_Svg_Element_Setup_Return (*Egueb_Svg_Referenceable_Setup)(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		Egueb_Svg_Element_Context *ctx,
		Egueb_Svg_Attribute_Presentation *attr,
		Enesim_Log **error);

typedef Eina_Bool (*Egueb_Svg_Referenceable_Propagate)(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		const Egueb_Svg_Element_Context *parent_ctx,
		const Egueb_Svg_Attribute_Presentation *attr,
		void *data,
		Enesim_Log **error);

typedef void (*Egueb_Svg_Referenceable_Cleanup)(Egueb_Dom_Tag *t);

typedef struct _Egueb_Svg_Referenceable_Descriptor {
	/* the tag interface */
	Egueb_Dom_Tag_Child_Add child_add;
	Egueb_Dom_Tag_Child_Remove child_remove;
	Egueb_Dom_Tag_Attribute_Get attribute_get;
	Egueb_Dom_Tag_Cdata_Set cdata_set;
	Egueb_Dom_Tag_Text_Set text_set;
	Egueb_Dom_Tag_Text_Get text_get;
	Egueb_Dom_Tag_Free free;
	/* the element interface */
	Egueb_Svg_Element_Initialize initialize;
	Egueb_Svg_Element_Attribute_Set attribute_set;
	Egueb_Svg_Element_Attribute_Animated_Fetch attribute_animated_fetch;
	/* the referenceable interface */
	Egueb_Svg_Referenceable_Setup setup;
	Egueb_Svg_Referenceable_Cleanup cleanup;
	Egueb_Svg_Referenceable_Propagate propagate;
	Egueb_Svg_Referenceable_Reference_Add reference_add;
	Egueb_Svg_Referenceable_Reference_Remove reference_remove;
} Egueb_Svg_Referenceable_Descriptor;

typedef Eina_Bool (*Egueb_Svg_Referenceable_Cb)(Egueb_Dom_Tag *t, Egueb_Svg_Referenceable_Reference *rr, void *data);

void * egueb_svg_referenceable_data_get(Egueb_Dom_Tag *t);
Egueb_Dom_Tag * egueb_svg_referenceable_new(Egueb_Svg_Referenceable_Descriptor *descriptor, Egueb_Svg_Type type, void *data);
Egueb_Svg_Referenceable_Reference * egueb_svg_referenceable_reference_add(Egueb_Dom_Tag *t,
		Egueb_Dom_Tag *referencer);
void egueb_svg_referenceable_reference_remove(Egueb_Dom_Tag *t, Egueb_Svg_Referenceable_Reference *rr);
void egueb_svg_referenceable_reference_foreach(Egueb_Dom_Tag *t, Egueb_Svg_Referenceable_Cb cb, void *data);
void egueb_svg_referenceable_reference_propagate(Egueb_Svg_Referenceable_Reference *rr,
		Egueb_Svg_Context *c,
		Enesim_Log **error);
#endif

#endif
