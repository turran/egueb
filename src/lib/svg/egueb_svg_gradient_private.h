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

#ifndef _EGUEB_SVG_PRIVATE_GRADIENT_H_
#define _EGUEB_SVG_PRIVATE_GRADIENT_H_

#include "egueb_svg_paint_server_private.h" 
#include "egueb_svg_reference_gradient_private.h"

typedef struct _Egueb_Svg_Gradient
{
	Egueb_Svg_Paint_Server base;
	Egueb_Dom_Node *units;
	Egueb_Dom_Node *transform;
	Egueb_Dom_Node *spread_method;
	Egueb_Dom_Node *xlink_href;
} Egueb_Svg_Gradient;

typedef struct _Egueb_Svg_Gradient_Class
{
	Egueb_Svg_Paint_Server_Class base;
} Egueb_Svg_Gradient_Class;

#define EGUEB_SVG_GRADIENT_DESCRIPTOR egueb_svg_gradient_descriptor_get()
#define EGUEB_SVG_GRADIENT_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Gradient_Class, 					\
		EGUEB_SVG_GRADIENT_DESCRIPTOR)
#define EGUEB_SVG_GRADIENT_CLASS_GET(o) EGUEB_SVG_GRADIENT_CLASS(	\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SVG_GRADIENT(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Gradient, EGUEB_SVG_GRADIENT_DESCRIPTOR)
Enesim_Object_Descriptor *  egueb_svg_gradient_descriptor_get(void);

void egueb_svg_gradient_href_node_get(Egueb_Dom_Node *n, Egueb_Dom_Node **href);

#if 0
typedef struct _Egueb_Svg_Element_Gradient_Descriptor_Internal
{
	Egueb_Dom_Tag_Free free;
	Egueb_Dom_Tag_Child_Add child_add;
	Egueb_Dom_Tag_Child_Remove child_remove;
	Egueb_Svg_Element_Attribute_Set attribute_set;
	Egueb_Svg_Element_Attribute_Animated_Fetch attribute_animated_fetch;
	Egueb_Svg_Referenceable_Setup setup;
	Egueb_Svg_Element_Gradient_Propagate propagate;
} Egueb_Svg_Element_Gradient_Descriptor_Internal;

typedef struct _Egueb_Svg_Element_Gradient_State
{
	char *href;
} Egueb_Svg_Element_Gradient_State;

typedef struct _Egueb_Svg_Element_Gradient
{
	EINA_MAGIC
	/* properties */
	Egueb_Svg_Attribute_Enum units;
	Egueb_Svg_Attribute_Enum spread_method;
	Egueb_Svg_Attribute_Animated_Transform transform;
	Egueb_Svg_Element_Gradient_State current;
	Egueb_Svg_Element_Gradient_State past;
	/* private */
	Egueb_Svg_Element_Gradient_Descriptor_Internal descriptor;
	Eina_Bool stops_changed : 1;
	Eina_Bool state_changed : 1;
	/* whenever the href is set, this points to the real objects */
	Egueb_Dom_Tag *href_t;
	Ender_Element *href_e;
	void *data;
} Egueb_Svg_Element_Gradient;


typedef struct _Egueb_Svg_Element_Gradient_Context {
	Egueb_Svg_Element_Gradient_Units units;
	Egueb_Svg_Spread_Method spread_method;
	Enesim_Matrix transform;
} Egueb_Svg_Element_Gradient_Context;

typedef Eina_Bool (*Egueb_Svg_Element_Gradient_Propagate)(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		const Egueb_Svg_Element_Context *ctx,
		const Egueb_Svg_Attribute_Presentation *attr,
		Egueb_Svg_Element_Gradient_Context *gctx,
		Enesim_Renderer *r,
		Enesim_Log **error);

typedef struct _Egueb_Svg_Element_Gradient_Descriptor {
	/* the tag interface */
	Egueb_Dom_Tag_Child_Add child_add;
	Egueb_Dom_Tag_Child_Remove child_remove;
	Egueb_Dom_Tag_Attribute_Get attribute_get;
	Egueb_Dom_Tag_Cdata_Set cdata_set;
	Egueb_Dom_Tag_Free free;
	/* the element interface */
	Egueb_Svg_Element_Initialize initialize;
	Egueb_Svg_Element_Attribute_Set attribute_set;
	Egueb_Svg_Element_Attribute_Animated_Fetch attribute_animated_fetch;
	/* the referenceable interface */
	Egueb_Svg_Referenceable_Setup setup;
	/* the paint server interface */
	Egueb_Svg_Paint_Server_Renderer_New renderer_new;
	/* our own interface */
	Egueb_Svg_Element_Gradient_Propagate propagate;
} Egueb_Svg_Element_Gradient_Descriptor;

Egueb_Dom_Tag * egueb_svg_element_gradient_new(Egueb_Svg_Element_Gradient_Descriptor *descriptor, Egueb_Svg_Type type, void *data);
void * egueb_svg_element_gradient_data_get(Egueb_Dom_Tag *t);
Egueb_Dom_Tag * egueb_svg_element_gradient_href_tag_get(Egueb_Dom_Tag *t);
#endif

#endif
