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

#ifndef _EGUEB_SVG_ELEMENT_PRIVATE_H_
#define _EGUEB_SVG_ELEMENT_PRIVATE_H_

#include "egueb_dom_element.h"
#include "egueb_dom_element_private.h"
#include "egueb_dom_attr.h"

#include "egueb_svg_clip_path.h"
#include "egueb_svg_color.h"
#include "egueb_svg_display.h"
#include "egueb_svg_clip_path.h"
#include "egueb_svg_fill_rule.h"
#include "egueb_svg_length.h"
#include "egueb_svg_font_size.h"
#include "egueb_svg_number.h"
#include "egueb_svg_overflow.h"
#include "egueb_svg_paint.h"
#include "egueb_svg_rect.h"
#include "egueb_svg_stroke_line_join.h"
#include "egueb_svg_stroke_line_cap.h"
#include "egueb_svg_text_anchor.h"
#include "egueb_svg_visibility.h"

#include "egueb_svg_reference.h"

#include "egueb_svg_attr_clip_path_private.h"
#include "egueb_svg_attr_color_private.h"
#include "egueb_svg_attr_display_private.h"
#include "egueb_svg_attr_font_size_private.h"
#include "egueb_svg_attr_length_private.h"
#include "egueb_svg_attr_number_private.h"
#include "egueb_svg_attr_overflow_private.h"
#include "egueb_svg_attr_paint_private.h"
#include "egueb_svg_attr_text_anchor_private.h"
#include "egueb_svg_attr_visibility_private.h"
#include "egueb_svg_attr_string_private.h"
#include "egueb_svg_attr_stroke_dasharray_private.h"
#include "egueb_svg_attr_stroke_line_cap_private.h"
#include "egueb_svg_attr_stroke_line_join_private.h"


#if 0
/* FIXME given that we include every header
 * needed on every source, we better dont
 * do this
 */
#include "egueb_svg_types.h"
#include "egueb_svg_attribute_private.h"
#include "egueb_svg_private_attribute_presentation.h"
#include "egueb_svg_context_private.h"
#include "egueb_svg_private_renderable_behaviour.h"

/* FIXME use this instead of the add/remove */
typedef int * (*Egueb_Svg_Element_Attribute_Animated_Fetch)(Egueb_Dom_Node *n, const char *name);

typedef Egueb_Svg_Element_Setup_Return (*Egueb_Svg_Element_Setup)(Egueb_Dom_Node *n,
		Egueb_Svg_Context *c,
		const Egueb_Svg_Element_Context *parent_ctx,
		Egueb_Svg_Element_Context *ctx,
		Egueb_Svg_Attribute_Presentation *attr,
		Enesim_Log **error);
typedef Eina_Bool (*Egueb_Svg_Element_Setup_Interceptor)(Egueb_Dom_Node *n,
		Egueb_Dom_Node *child,
		Egueb_Svg_Context *c,
		Enesim_Log **error,
		void *data);
#endif

/* forward declaration to make the struct have a pointer to itself */
typedef struct _Egueb_Svg_Element
{
	Egueb_Dom_Element parent;
	/* properties */
	Egueb_Dom_Node *id;
	Egueb_Dom_Node *klass;
	Egueb_Dom_Node *style;
	/* presentation attributes */
	Egueb_Dom_Node *clip_path;
	Egueb_Dom_Node *color;
	Egueb_Dom_Node *display;
	Egueb_Dom_Node *fill;
	Egueb_Dom_Node *fill_opacity;
	Egueb_Dom_Node *font;
	Egueb_Dom_Node *font_family;
	Egueb_Dom_Node *font_size;
	Egueb_Dom_Node *font_style;
	Egueb_Dom_Node *font_weight;
	Egueb_Dom_Node *opacity;
	Egueb_Dom_Node *overflow;
	Egueb_Dom_Node *stop_color;
	Egueb_Dom_Node *stop_opacity;
	Egueb_Dom_Node *stroke;
	Egueb_Dom_Node *stroke_width;
	Egueb_Dom_Node *stroke_opacity;
	Egueb_Dom_Node *stroke_dasharray;
	Egueb_Dom_Node *stroke_line_join;
	Egueb_Dom_Node *stroke_line_cap;
	Egueb_Dom_Node *text_anchor;
	Egueb_Dom_Node *visibility;
	/* protected */
	/* the current viewbox ?? */
	Egueb_Svg_Rect viewbox;
	/* the current transformation */
	Enesim_Matrix transform;
	/* the current font size */
	double final_font_size;
	/* private */
	/* some nodes need to do the setup not based on its parent, but on
	 * another node, like the <g> created by a <use> for that we use this
	 *  relative member
	 */
	Egueb_Dom_Node *presentation_relative;
	Egueb_Dom_Node *geometry_relative;
	Egueb_Dom_String *last_style;
} Egueb_Svg_Element;

typedef Eina_Bool (*Egueb_Svg_Element_Process)(Egueb_Svg_Element *e);

typedef struct _Egueb_Svg_Element_Class
{
	Egueb_Dom_Element_Class parent;
	Egueb_Svg_Element_Process process;
} Egueb_Svg_Element_Class;

#define EGUEB_SVG_ELEMENT_DESCRIPTOR egueb_svg_element_descriptor_get()
#define EGUEB_SVG_ELEMENT_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Element_Class, EGUEB_SVG_ELEMENT_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_CLASS_GET(o) EGUEB_SVG_ELEMENT_CLASS(			\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SVG_ELEMENT(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 			\
		Egueb_Svg_Element, EGUEB_SVG_ELEMENT_DESCRIPTOR)

/* Helper macro to get the animated/base values of an attribute */
#define EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(attr, val)				\
	if (egueb_dom_attr_type_is_set(attr, EGUEB_DOM_ATTR_TYPE_BASE))		\
		egueb_dom_attr_get(attr, EGUEB_DOM_ATTR_TYPE_BASE, &val->base);	\
	else									\
		egueb_dom_attr_get(attr, EGUEB_DOM_ATTR_TYPE_DEFAULT, 		\
				&val->base);					\
	if (egueb_dom_attr_type_is_set(attr, EGUEB_DOM_ATTR_TYPE_ANIMATED))	\
		egueb_dom_attr_get(attr, EGUEB_DOM_ATTR_TYPE_ANIMATED, 		\
				&val->anim);					\
	else									\
		val->anim = val->base;

#define EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET_WITH_RETURN(attr, val)		\
	if (!val) return EGUEB_DOM_ERROR_INVALID_ACCESS;			\
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(attr, val);				\
	return EINA_ERROR_NONE;

Enesim_Object_Descriptor * egueb_svg_element_descriptor_get(void);
void egueb_svg_element_clip_path_resolve(Egueb_Dom_Node *n,
		Egueb_Svg_Clip_Path *clip_path_current,
		Egueb_Svg_Clip_Path *clip_path_last, Egueb_Svg_Reference **r);
void egueb_svg_element_children_clone(Egueb_Dom_Node *n, Egueb_Dom_Node *from);

Egueb_Dom_Node * egueb_svg_element_presentation_relative_get(Egueb_Dom_Node *n);
EAPI Eina_Bool egueb_svg_element_presentation_relative_set(Egueb_Dom_Node *n,
		Egueb_Dom_Node *presentation_relative, Eina_Error *err);

Egueb_Dom_Node * egueb_svg_element_geometry_relative_get(Egueb_Dom_Node *n);
EAPI Eina_Bool egueb_svg_element_geometry_relative_set(Egueb_Dom_Node *n,
		Egueb_Dom_Node *geometry_relative, Eina_Error *err);

void egueb_svg_element_overflow_final_get(Egueb_Dom_Node *n, Egueb_Svg_Overflow *o);
void egueb_svg_element_color_final_get(Egueb_Dom_Node *n,
   	Egueb_Svg_Color *color);
void egueb_svg_element_fill_final_get(Egueb_Dom_Node *n,
   	Egueb_Svg_Paint *fill);
void egueb_svg_element_stroke_final_get(Egueb_Dom_Node *n,
   	Egueb_Svg_Paint *stroke);
void egueb_svg_element_clip_path_final_get(Egueb_Dom_Node *n,
		Egueb_Svg_Clip_Path *clip_path);

#endif
