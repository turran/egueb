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

#include "egueb_svg_main.h"
#include "egueb_svg_document.h"
/* needed types */
#include "egueb_svg_referenceable_units.h"
#include "egueb_svg_string.h"
#include "egueb_svg_length.h"
#include "egueb_svg_overflow.h"
#include "egueb_svg_point.h"
#include "egueb_svg_matrix.h"
/* elements */
#include "egueb_svg_element.h"
#include "egueb_svg_element_a.h"
#include "egueb_svg_element_animate_transform.h"
#include "egueb_svg_element_circle.h"
#include "egueb_svg_element_clip_path.h"
#include "egueb_svg_element_defs.h"
#include "egueb_svg_element_desc.h"
#include "egueb_svg_element_ellipse.h"
#include "egueb_svg_element_g.h"
#include "egueb_svg_element_image.h"
#include "egueb_svg_element_line.h"
#include "egueb_svg_element_linear_gradient.h"
#include "egueb_svg_element_path.h"
#include "egueb_svg_element_pattern.h"
#include "egueb_svg_element_polygon.h"
#include "egueb_svg_element_polyline.h"
#include "egueb_svg_element_radial_gradient.h"
#include "egueb_svg_element_rect.h"
#include "egueb_svg_element_use.h"
#include "egueb_svg_element_stop.h"
#include "egueb_svg_element_svg.h"
#include "egueb_svg_element_text.h"
#include "egueb_svg_element_tspan.h"
#include "egueb_svg_element_video.h"
#include "egueb_dom_document.h"
/* abstracts */
#include "egueb_svg_renderable.h"
#include "egueb_svg_renderable_container.h"

#include "egueb_dom_document_private.h"
#include "egueb_svg_element_svg_private.h"

#include <unistd.h>
#include <libgen.h>
#include <string.h>
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_DOCUMENT_DESCRIPTOR egueb_svg_document_descriptor_get()
#define EGUEB_SVG_DOCUMENT_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Document_Class, EGUEB_SVG_DOCUMENT_DESCRIPTOR)
#define EGUEB_SVG_DOCUMENT_CLASS_GET(o) EGUEB_SVG_DOCUMENT_CLASS(		\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SVG_DOCUMENT(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 			\
		Egueb_Svg_Document, EGUEB_SVG_DOCUMENT_DESCRIPTOR)

typedef struct _Egueb_Svg_Document
{
	Egueb_Dom_Document parent;
	double width;
	double height;
	double font_size;
	int fps;

	/* input */
	Egueb_Dom_Input *input;
} Egueb_Svg_Document;

typedef struct _Egueb_Svg_Document_Class
{
	Egueb_Dom_Document_Class parent;
} Egueb_Svg_Document_Class;

typedef enum _Egueb_Svg_Type {
	EGUEB_SVG_TYPE_A,
	EGUEB_SVG_TYPE_ALTGLYPH,
	EGUEB_SVG_TYPE_ALTGLYPHDEF,
	EGUEB_SVG_TYPE_ALTGLYPHITEM,
	EGUEB_SVG_TYPE_ANIMATE,
	EGUEB_SVG_TYPE_ANIMATECOLOR,
	EGUEB_SVG_TYPE_ANIMATEMOTION,
	EGUEB_SVG_TYPE_ANIMATETRANSFORM,
	EGUEB_SVG_TYPE_CIRCLE,
	EGUEB_SVG_TYPE_CLIPPATH,
	EGUEB_SVG_TYPE_COLOR_PROFILE, /* 10 */
	EGUEB_SVG_TYPE_CURSOR,
	EGUEB_SVG_TYPE_DEFINITION_SRC,
	EGUEB_SVG_TYPE_DEFS,
	EGUEB_SVG_TYPE_DESC,
	EGUEB_SVG_TYPE_ELLIPSE,
	EGUEB_SVG_TYPE_FEBLEND,
	EGUEB_SVG_TYPE_FECOLORMATRIX,
	EGUEB_SVG_TYPE_FECOMPONENTTRANSFER,
	EGUEB_SVG_TYPE_FECOMPOSITE,
	EGUEB_SVG_TYPE_FECONVOLVEMATRIX, /* 20 */
	EGUEB_SVG_TYPE_FEDIFFUSELIGHTING,
	EGUEB_SVG_TYPE_FEDISPLACEMENTMAP,
	EGUEB_SVG_TYPE_FEDISTANTLIGHT,
	EGUEB_SVG_TYPE_FEFLOOD,
	EGUEB_SVG_TYPE_FEFUNCA,
	EGUEB_SVG_TYPE_FEFUNCB,
	EGUEB_SVG_TYPE_FEFUNCG,
	EGUEB_SVG_TYPE_FEFUNCR,
	EGUEB_SVG_TYPE_FEGAUSSIANBLUR,
	EGUEB_SVG_TYPE_FEIMAGE, /* 30 */
	EGUEB_SVG_TYPE_FEMERGE,
	EGUEB_SVG_TYPE_FEMERGENODE,
	EGUEB_SVG_TYPE_FEMORPHOLOGY,
	EGUEB_SVG_TYPE_FEOFFSET,
	EGUEB_SVG_TYPE_FEPOINTLIGHT,
	EGUEB_SVG_TYPE_FESPECULARLIGHTING,
	EGUEB_SVG_TYPE_FESPOTLIGHT,
	EGUEB_SVG_TYPE_FETILE,
	EGUEB_SVG_TYPE_FETURBULENCE,
	EGUEB_SVG_TYPE_FILTER, /* 40 */
	EGUEB_SVG_TYPE_FONT,
	EGUEB_SVG_TYPE_FONT_FACE,
	EGUEB_SVG_TYPE_FONT_FACE_FORMAT,
	EGUEB_SVG_TYPE_FONT_FACE_NAME,
	EGUEB_SVG_TYPE_FONT_FACE_SRC,
	EGUEB_SVG_TYPE_FONT_FACE_URI,
	EGUEB_SVG_TYPE_FOREIGNOBJECT,
	EGUEB_SVG_TYPE_G,
	EGUEB_SVG_TYPE_GLYPH,
	EGUEB_SVG_TYPE_GLYPHREF, /* 50 */
	EGUEB_SVG_TYPE_HKERN,
	EGUEB_SVG_TYPE_IMAGE,
	EGUEB_SVG_TYPE_LINE,
	EGUEB_SVG_TYPE_LINEARGRADIENT,
	EGUEB_SVG_TYPE_MARKER,
	EGUEB_SVG_TYPE_MASK,
	EGUEB_SVG_TYPE_METADATA,
	EGUEB_SVG_TYPE_MISSING_GLYPH,
	EGUEB_SVG_TYPE_MPATH,
	EGUEB_SVG_TYPE_PATH, /* 60 */
	EGUEB_SVG_TYPE_PATTERN,
	EGUEB_SVG_TYPE_POLYGON,
	EGUEB_SVG_TYPE_POLYLINE,
	EGUEB_SVG_TYPE_RADIALGRADIENT,
	EGUEB_SVG_TYPE_RECT,
	EGUEB_SVG_TYPE_SCRIPT,
	EGUEB_SVG_TYPE_SET,
	EGUEB_SVG_TYPE_STOP,
	EGUEB_SVG_TYPE_STYLE,
	EGUEB_SVG_TYPE_SVG, /* 70 */
	EGUEB_SVG_TYPE_SWITCH,
	EGUEB_SVG_TYPE_SYMBOL,
	EGUEB_SVG_TYPE_TEXT,
	EGUEB_SVG_TYPE_TEXTPATH,
	EGUEB_SVG_TYPE_TITLE,
	EGUEB_SVG_TYPE_TREF,
	EGUEB_SVG_TYPE_TSPAN,
	EGUEB_SVG_TYPE_USE,
	EGUEB_SVG_TYPE_VIDEO,
	EGUEB_SVG_TYPE_VIEW,
	EGUEB_SVG_TYPE_VKERN,
	EGUEB_SVG_TYPE_PARSE_TAGS
} Egueb_Svg_Type;

static Eina_Bool _egueb_svg_document_element_id_get(const char *name, size_t sz, int *id)
{
	/* sz ==1 : 2 cases : a and g */
	if (sz == 1)
	{
		if (name[0] == 'a')
		{
			*id = EGUEB_SVG_TYPE_A;
			return EINA_TRUE;
		}
		else if (name[0] == 'g')
		{
			*id = EGUEB_SVG_TYPE_G;
			return EINA_TRUE;
		}
	}
	else if (sz == 2)
	{
	}
	else if (sz == 3)
	{
		if (name[0] == 's')
		{
			if ((name[1] == 'e') && (name[2] == 't'))
			{
				*id = EGUEB_SVG_TYPE_SET;
				return EINA_TRUE;
			}
			else if ((name[1] == 'v') && (name[2] == 'g'))
			{
				*id = EGUEB_SVG_TYPE_SVG;
				return EINA_TRUE;
			}
		}
		else if (name[0] == 'u')
		{
			if ((name[1] == 's') && (name[2] == 'e'))
			{
				*id = EGUEB_SVG_TYPE_USE;
				return EINA_TRUE;
			}
		}
	}
	else
	{
		if (name[0] == 'a')
		{
			if (strncmp("animate", name, sz) == 0)
			{
				*id = EGUEB_SVG_TYPE_ANIMATE;
				return EINA_TRUE;
			}
			else if (strncmp("animateTransform", name, sz) == 0)
			{
				*id = EGUEB_SVG_TYPE_ANIMATETRANSFORM;
				return EINA_TRUE;
			}

		}
		else if (name[0] == 'c')
		{
			if (strncmp("circle", name, sz) == 0)
			{
				*id = EGUEB_SVG_TYPE_CIRCLE;
				return EINA_TRUE;
			}
			else if (strncmp("clipPath", name, sz) == 0)
			{
				*id = EGUEB_SVG_TYPE_CLIPPATH;
				return EINA_TRUE;
			}
		}
		else if (name[0] == 'd')
		{
			if (strncmp("defs", name, sz) == 0)
			{
				*id = EGUEB_SVG_TYPE_DEFS;
				return EINA_TRUE;
			}
			else if (strncmp("desc", name, sz) == 0)
			{
				*id = EGUEB_SVG_TYPE_DESC;
				return EINA_TRUE;
			}
		}
		else if (name[0] == 'e')
		{
			if (strncmp("ellipse", name, sz) == 0)
			{
				*id = EGUEB_SVG_TYPE_ELLIPSE;
				return EINA_TRUE;
			}
		}
		else if (name[0] == 'f')
		{
		}
		else if (name[0] == 'g')
		{
			/* <g> already managed */
			if (strncmp("glyph", name, sz) == 0)
			{
			}
			else if (strncmp("glyphRef", name, sz) == 0)
			{
			}
		}
		else if (name[0] == 'h')
		{
			if (strncmp("hkern", name, sz) == 0)
			{
			}
		}
		else if (name[0] == 'i')
		{
			if (strncmp("image", name, sz) == 0)
			{
				*id = EGUEB_SVG_TYPE_IMAGE;
				return EINA_TRUE;
			}
		}
		else if (name[0] == 'l')
		{
			if (strncmp("line", name, sz) == 0)
			{
				*id = EGUEB_SVG_TYPE_LINE;
				return EINA_TRUE;
			}
			else if (strncmp("linearGradient", name, sz) == 0)
			{
				*id = EGUEB_SVG_TYPE_LINEARGRADIENT;
				return EINA_TRUE;
			}
		}
		else if (name[0] == 'm')
		{
		}
		else if (name[0] == 'p')
		{
			if (strncmp("path", name, sz) == 0)
			{
				*id = EGUEB_SVG_TYPE_PATH;
				return EINA_TRUE;
			}
			else if (strncmp("pattern", name, sz) == 0)
			{
				*id = EGUEB_SVG_TYPE_PATTERN;
				return EINA_TRUE;
			}
			else if (strncmp("polyline", name, sz) == 0)
			{
				*id = EGUEB_SVG_TYPE_POLYLINE;
				return EINA_TRUE;
			}
			else if (strncmp("polygon", name, sz) == 0)
			{
				*id = EGUEB_SVG_TYPE_POLYGON;
				return EINA_TRUE;
			}
		}
		else if (name[0] == 'r')
		{
			if (strncmp("radialGradient", name, sz) == 0)
			{
				*id = EGUEB_SVG_TYPE_RADIALGRADIENT;
				return EINA_TRUE;
			}
			else if (strncmp("rect", name, sz) == 0)
			{
				*id = EGUEB_SVG_TYPE_RECT;
				return EINA_TRUE;
			}
		}
		else if (name[0] == 's')
		{
			if (strncmp("script", name, sz) == 0)
			{
				*id = EGUEB_SVG_TYPE_SCRIPT;
				return EINA_TRUE;
			}
			else if (strncmp("stop", name, sz) == 0)
			{
				*id = EGUEB_SVG_TYPE_STOP;
				return EINA_TRUE;
			}
			else if (strncmp("style", name, sz) == 0)
			{
				*id = EGUEB_SVG_TYPE_STYLE;
				return EINA_TRUE;
			}
		}
		else if (name[0] == 't')
		{
			if (strncmp("text", name, sz) == 0)
			{
				*id = EGUEB_SVG_TYPE_TEXT;
				return EINA_TRUE;
			}
			else if (strncmp("tspan", name, sz) == 0)
			{
				*id = EGUEB_SVG_TYPE_TSPAN;
				return EINA_TRUE;
			}
		}
		else if (name[0] == 'u')
		{
		}
		else if (name[0] == 'v')
		{
			if (strncmp("view", name, sz) == 0)
			{
			}
			else if (strncmp("vkern", name, sz) == 0)
			{
			}
			else if (strncmp("video", name, sz) == 0)
			{
				*id = EGUEB_SVG_TYPE_VIDEO;
				return EINA_TRUE;
			}
		}
	}
	return EINA_FALSE;
}

static Egueb_Dom_Node * _egueb_svg_document_element_create_by_id(int id)
{
	Egueb_Dom_Node *ret = NULL;

	switch (id)
	{
		case EGUEB_SVG_TYPE_LINEARGRADIENT:
                ret = egueb_svg_element_linear_gradient_new();
		break;

		case EGUEB_SVG_TYPE_RADIALGRADIENT:
		ret = egueb_svg_element_radial_gradient_new();
		break;

		case EGUEB_SVG_TYPE_PATTERN:
		ret = egueb_svg_element_pattern_new();
		break;

		case EGUEB_SVG_TYPE_DEFS:
		ret = egueb_svg_element_defs_new();
		break;

		case EGUEB_SVG_TYPE_USE:
		ret = egueb_svg_element_use_new();
		break;

		case EGUEB_SVG_TYPE_SVG:
		ret = egueb_svg_element_svg_new();
		break;

		case EGUEB_SVG_TYPE_CIRCLE:
		ret = egueb_svg_element_circle_new();
		break;

		case EGUEB_SVG_TYPE_DESC:
		ret = egueb_svg_element_desc_new();
		break;

		case EGUEB_SVG_TYPE_ELLIPSE:
		ret = egueb_svg_element_ellipse_new();
		break;

		case EGUEB_SVG_TYPE_RECT:
		ret = egueb_svg_element_rect_new();
		break;

		case EGUEB_SVG_TYPE_LINE:
		ret = egueb_svg_element_line_new();
		break;

		case EGUEB_SVG_TYPE_PATH:
		ret = egueb_svg_element_path_new();
		break;

		case EGUEB_SVG_TYPE_POLYLINE:
		ret = egueb_svg_element_polyline_new();
		break;

		case EGUEB_SVG_TYPE_POLYGON:
		ret = egueb_svg_element_polygon_new();
		break;

		case EGUEB_SVG_TYPE_TEXT:
		ret = egueb_svg_element_text_new();
		break;

		case EGUEB_SVG_TYPE_TSPAN:
		ret = egueb_svg_element_tspan_new();
		break;

		case EGUEB_SVG_TYPE_G:
		ret = egueb_svg_element_g_new();
		break;

		case EGUEB_SVG_TYPE_A:
		ret = egueb_svg_element_a_new();
		break;

		case EGUEB_SVG_TYPE_SCRIPT:
		ret = egueb_dom_element_script_new();
		break;

#if 0
		case EGUEB_SVG_TYPE_STYLE:
		ret = egueb_svg_element_style_new();
		break;

#endif
		case EGUEB_SVG_TYPE_IMAGE:
		ret = egueb_svg_element_image_new();
		break;

		case EGUEB_SVG_TYPE_CLIPPATH:
		ret = egueb_svg_element_clip_path_new();
		break;

		case EGUEB_SVG_TYPE_SET:
		ret = egueb_smil_set_new();
		break;

		case EGUEB_SVG_TYPE_STOP:
		ret = egueb_svg_element_stop_new();
		break;

		case EGUEB_SVG_TYPE_ANIMATE:
		ret = egueb_smil_animate_new();
		break;

		case EGUEB_SVG_TYPE_ANIMATETRANSFORM:
		ret = egueb_svg_element_animate_transform_new();
		break;

		case EGUEB_SVG_TYPE_VIDEO:
		ret = egueb_svg_element_video_new();
		break;

		default:
		break;
	}
	return ret;
}

/*----------------------------------------------------------------------------*
 *                          UI feature interface                              *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Input * _egueb_svg_document_ui_input_get(Egueb_Dom_Node *n)
{
	Egueb_Svg_Document *thiz;

	thiz = EGUEB_SVG_DOCUMENT(n);
	return egueb_dom_input_ref(thiz->input);
}

static Egueb_Dom_Feature_UI_Descriptor 
_egueb_svg_document_ui_descriptor = {
	/* .input_get 	= */ _egueb_svg_document_ui_input_get,
};
/*----------------------------------------------------------------------------*
 *                       Animation feature interface                          *
 *----------------------------------------------------------------------------*/
static Egueb_Smil_Timeline * _egueb_svg_document_animation_timeline_get(Egueb_Dom_Node *n)
{
	Egueb_Dom_Node *topmost;
	Egueb_Smil_Timeline *ret;

	topmost = egueb_dom_document_document_element_get(n);
	if (!topmost) return NULL;

	ret = egueb_svg_element_svg_timeline_get(topmost);
	egueb_dom_node_unref(topmost);
	return ret;
}

static Egueb_Smil_Feature_Animation_Descriptor 
_egueb_svg_document_animation_descriptor = {
	/* .timeline_get 	= */ _egueb_svg_document_animation_timeline_get,
};
/*----------------------------------------------------------------------------*
 *                        Window feature interface                            *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_document_window_type_get(
		Egueb_Dom_Node *n, Egueb_Dom_Feature_Window_Type *type)
{
	*type = EGUEB_DOM_FEATURE_WINDOW_TYPE_MASTER;
	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_document_window_content_size_set(
		Egueb_Dom_Node *n, int w, int h)
{
	Egueb_Svg_Document *thiz;
	Egueb_Dom_Node *topmost;

	thiz = EGUEB_SVG_DOCUMENT(n);
	thiz->height = h;
	thiz->width = w;

	topmost = egueb_dom_document_document_element_get(n);
	if (topmost)
	{
		egueb_dom_element_request_process(topmost);
		egueb_dom_node_unref(topmost);
	}
	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_document_window_content_size_get(
		Egueb_Dom_Node *n, int *w, int *h)
{
	Egueb_Svg_Document *thiz;
	Egueb_Dom_Node *topmost = NULL;

	thiz = EGUEB_SVG_DOCUMENT(n);
	topmost = egueb_dom_document_document_element_get(n);
	if (!topmost)
	{
		*h = thiz->height;
		*w = thiz->width;
	}
	else
	{
		Egueb_Svg_Length_Animated width;
		Egueb_Svg_Length_Animated height;
		Egueb_Svg_Overflow overflow;
		double dw, dh;

		egueb_svg_element_svg_width_get(topmost, &width);
		egueb_svg_element_svg_height_get(topmost, &height);

		dw = egueb_svg_coord_final_get(&width.anim, thiz->width,
				thiz->font_size);
		dh = egueb_svg_coord_final_get(&height.anim, thiz->height,
				thiz->font_size);
		*w = ceil(dw);
		*h = ceil(dh);

		egueb_svg_element_overflow_final_get(topmost, &overflow);
		/* check if we should overflow */
		if (overflow == EGUEB_SVG_OVERFLOW_VISIBLE)
		{
			Eina_Rectangle r;
			egueb_svg_renderable_user_bounds_get(topmost, &r);
			if (*w < r.x + r.w)
				*w = r.x + r.w;
			if (*h < r.y + r.h)
				*h = r.y + r.h;
		}
	}

	return EINA_TRUE;
}

static Egueb_Dom_Feature_Window_Descriptor 
_egueb_svg_document_window_descriptor = {
	/* .type_get 		= */ _egueb_svg_document_window_type_get,
	/* .content_size_set 	= */ _egueb_svg_document_window_content_size_set,
	/* .content_size_get 	= */ _egueb_svg_document_window_content_size_get,
};
/*----------------------------------------------------------------------------*
 *                        Render feature interface                            *
 *----------------------------------------------------------------------------*/
static Enesim_Renderer * _egueb_svg_document_render_renderer_get(
		Egueb_Dom_Node *n)
{
	Egueb_Dom_Node *topmost;
	Enesim_Renderer *r;

	/* get the topmost element, get the renderer and return it */
	topmost = egueb_dom_document_document_element_get(n);
	if (!topmost) return NULL;

	r = egueb_svg_renderable_renderer_get(topmost);
	egueb_dom_node_unref(topmost);
	return r;
}

static Egueb_Dom_Feature_Render_Descriptor
_egueb_svg_document_render_descriptor = {
	/* .renderer_get 	= */ _egueb_svg_document_render_renderer_get,
};
/*----------------------------------------------------------------------------*
 *                              Input interface                               *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Node * _egueb_svg_document_input_element_at_recursive(
		Egueb_Dom_Node *n, Eina_Rectangle *ptr)
{
	Egueb_Dom_Node *ret = NULL;
	Eina_Rectangle bounds;

	if (!egueb_svg_is_renderable(n))
		goto done;

	if (egueb_svg_is_renderable_container(n))
	{
		Egueb_Dom_Node *child = NULL;
		child = egueb_dom_node_child_last_get(n);
		while (child)
		{
			Egueb_Dom_Node *tmp;

			ret = _egueb_svg_document_input_element_at_recursive(
					egueb_dom_node_ref(child), ptr);
			if (ret)
			{
				egueb_dom_node_unref(child);
				break;
			}

			tmp = egueb_dom_node_sibling_previous_get(child);
			egueb_dom_node_unref(child);
			child = tmp;
		}
	}

	if (!ret)
	{
		/* check the bounds */
		egueb_svg_renderable_user_bounds_get(n, &bounds);
		if (eina_rectangles_intersect(&bounds, ptr))
		{
			Egueb_Dom_String *name;

			name = egueb_dom_node_name_get(n);
			DBG("Element '%s' found with bounds %"
					EINA_RECTANGLE_FORMAT,
					egueb_dom_string_string_get(name),
					EINA_RECTANGLE_ARGS(&bounds));
			egueb_dom_string_unref(name);
			ret = egueb_dom_node_ref(n);
		}
	}
done:
	egueb_dom_node_unref(n);
	return ret;
}

static Egueb_Dom_Node * _egueb_svg_document_input_element_at(
		Egueb_Dom_Node *current, int x, int y, void *data)
{
	Egueb_Svg_Document *thiz = data;
	Egueb_Dom_Node *n = NULL;
	Egueb_Dom_Node *ret;
	Eina_Rectangle ptr;

	eina_rectangle_coords_from(&ptr, x, y, 1, 1);
#if 0
	/* check that the new coords are still on current bounds */
	if (current)
	{
		Eina_Rectangle bounds;

		egueb_svg_renderable_user_bounds_get(current, &bounds);
		if (eina_rectangles_intersect(&bounds, &ptr))
		{
			return egueb_dom_node_ref(current);
		}
		else
		{
			Egueb_Dom_Node *parent;

			/* go up until the parent is inside the bounds */
			parent = egueb_dom_node_parent_get(current);
			while (!n)
			{
				Egueb_Dom_Node *tmp;
				if (!parent)
					break;
				if (egueb_svg_is_renderable(parent))
				{
					egueb_svg_renderable_user_bounds_get(parent, &bounds);
					if (eina_rectangles_intersect(&bounds, &ptr))
					{
						n = parent;
					}
				}
				tmp = egueb_dom_node_parent_get(parent);
				egueb_dom_node_unref(parent);
				parent = tmp;
			}
		}
	}
#endif

	if (!n)
	{
		n = egueb_dom_document_document_element_get(EGUEB_DOM_NODE(thiz));
		if (!n) return NULL;
	}

	/* iterate over the whole tree */
	ret = _egueb_svg_document_input_element_at_recursive(n, &ptr);
	return ret;
}

static Egueb_Dom_Node * _egueb_svg_document_input_focus_next(
		Egueb_Dom_Node *current, void *data)
{
	Egueb_Svg_Document *thiz = data;
	Egueb_Dom_Node *ret = NULL;

	/* if we start from the beginning, pick the first child */
	/* first on the first level */
	/* focusable elements are only <a> and <use> that points to a's */
	return ret;
}

static Egueb_Dom_Node * _egueb_svg_document_input_focus_prev(
		Egueb_Dom_Node *current, void *data)
{
	Egueb_Svg_Document *thiz = data;
	Egueb_Dom_Node *ret = NULL;

	/* focusable elements are only <a> and <use> that points to a's */
	return ret;
}

static Egueb_Dom_Input_Descriptor _document_svg_input_descriptor = {
	/* .version 		= */ EGUEB_DOM_INPUT_DESCRIPTOR_VERSION,
	/* .element_at	 	= */ _egueb_svg_document_input_element_at,
	/* .focus_next		= */ _egueb_svg_document_input_focus_next,
	/* .focus_prev		= */ _egueb_svg_document_input_focus_prev,
};
/*----------------------------------------------------------------------------*
 *                            Document interface                              *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Node * _egueb_svg_document_element_create(
		Egueb_Dom_Document *d, const char *name)
{
	Egueb_Dom_Node *ret;
	size_t sz;
	int id;

	sz = strlen(name);
	if (!_egueb_svg_document_element_id_get(name, sz, &id))
		return NULL;
	ret = _egueb_svg_document_element_create_by_id(id);
	if (!ret)
	{
		/* FIXME here we should use the application interface to let
		 * the application create new elements
		 */
		WARN("can't create the tag %s", name);
	}
	return ret;
}

static Eina_Bool _egueb_svg_document_child_appendable(
		Egueb_Dom_Document *thiz, Egueb_Dom_Node *child)
{
	Egueb_Dom_String *name;
	Eina_Bool ret;

	name = egueb_dom_node_name_get(child);
	ret = egueb_dom_string_is_equal(name, EGUEB_SVG_NAME_SVG);
	egueb_dom_string_unref(name);

	return ret;
}

static Ender_Item * _egueb_svg_document_item_get(Egueb_Dom_Document *d)
{
	const Ender_Lib *lib;

	lib = ender_lib_find("egueb_svg");
	return ender_lib_item_find(lib, "egueb.svg.document");
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_DOCUMENT_DESCRIPTOR,
		Egueb_Svg_Document, Egueb_Svg_Document_Class,
		egueb_svg_document)

static void _egueb_svg_document_class_init(void *k)
{
	Egueb_Dom_Document_Class *klass = EGUEB_DOM_DOCUMENT_CLASS(k);

	klass->element_create = _egueb_svg_document_element_create;
	klass->child_appendable = _egueb_svg_document_child_appendable;
	klass->item_get = _egueb_svg_document_item_get;
}

static void _egueb_svg_document_instance_init(void *o)
{
	Egueb_Svg_Document *thiz;

	thiz = EGUEB_SVG_DOCUMENT(o);
	/* register the features */
	egueb_dom_feature_window_add(EGUEB_DOM_NODE(thiz),
			&_egueb_svg_document_window_descriptor);
	egueb_dom_feature_render_add(EGUEB_DOM_NODE(thiz),
			&_egueb_svg_document_render_descriptor);
	egueb_dom_feature_ui_add(EGUEB_DOM_NODE(thiz),
			&_egueb_svg_document_ui_descriptor);
	egueb_smil_feature_animation_add(EGUEB_DOM_NODE(thiz),
			&_egueb_svg_document_animation_descriptor);
	egueb_dom_feature_io_add(EGUEB_DOM_NODE(thiz));
	egueb_dom_feature_script_add(EGUEB_DOM_NODE(thiz));
	egueb_dom_feature_multimedia_add(EGUEB_DOM_NODE(thiz));
	egueb_dom_feature_navigation_add(EGUEB_DOM_NODE(thiz));
	thiz->fps = 30;
	thiz->font_size = 16;
	thiz->input = egueb_dom_input_new(&_document_svg_input_descriptor, thiz);
}

static void _egueb_svg_document_instance_deinit(void *o)
{
	Egueb_Svg_Document *thiz;

	thiz = EGUEB_SVG_DOCUMENT(o);
	if (thiz->input)
	{
		egueb_dom_input_unref(thiz->input);
		thiz->input = NULL;
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_document_width_get(Egueb_Dom_Node *n,
		double *width)
{
	Egueb_Svg_Document *thiz;

	if (!width) return;
	thiz = EGUEB_SVG_DOCUMENT(n);
	*width = thiz->width;
}

void egueb_svg_document_height_get(Egueb_Dom_Node *n,
		double *height)
{
	Egueb_Svg_Document *thiz;

	if (!height) return;
	thiz = EGUEB_SVG_DOCUMENT(n);
	*height = thiz->height;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_svg_document_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_document);
	return n;
}

/* readonly attribute DOMString title; */
EAPI Egueb_Dom_String * egueb_svg_document_title_get(Egueb_Dom_Node *n)
{
	/* FIXME here we should use the application interface */
	return NULL;
}

/* readonly attribute DOMString referrer; */
EAPI Egueb_Dom_String * egueb_svg_document_referrer_get(Egueb_Dom_Node *n)
{
	/* FIXME here we should use the application interface */
	return NULL;
}

/* readonly attribute DOMString domain; */
EAPI Egueb_Dom_String * egueb_svg_document_domain_get(Egueb_Dom_Node *n)
{
	/* FIXME here we should use the application interface */
	return NULL;
}

#if 0
/* readonly attribute DOMString URL */
EAPI Egueb_Dom_String * egueb_svg_document_url_get(Egueb_Dom_Node *n)
{
	Egueb_Svg_Document *thiz;
	const char *location = NULL;
	const char *filename = NULL;
	char *ret;
	int len;

	thiz = EGUEB_SVG_DOCUMENT(n);
	if (!thiz->filename_get) return NULL;
	filename = thiz->filename_get(thiz->filename_get_data);
	if (!filename) return NULL;
	/* check if it is relative or absolute */
	if (!strncmp(filename, "http://", 7) || strncmp(filename, "/", 1))
	{
		return egueb_dom_string_new_with_string(filename);
	}
	/* in case of relative check if we have a location */	
	if (!thiz->location_get)
	{
		location = thiz->location_get(thiz->location_get_data);
	}

	if (!location)
	{
		char *tmp;
		tmp = getcwd(NULL, 0);
		len = asprintf(&ret, "%s/%s", location, filename);
		free(tmp);
	}
	else
	{
		len = asprintf(&ret, "%s/%s", location, filename);
	}
	if (len <= 0) return NULL;
	return egueb_dom_string_steal(ret);
}
#endif

/**
 * Get the root 'svg' element in the document hierarchy
 * (readonly attribute SVGSVGElement rootElement)
 * @ender_prop{root_element}
 * @param[in] n The document to get the root element from
 * @return The root element @ender_type{egueb.svg.element}
 */
EAPI Egueb_Dom_Node * egueb_svg_document_root_element_get(Egueb_Dom_Node *n)
{
	return egueb_dom_document_document_element_get(n);
}

EAPI void egueb_svg_document_element_root_set(Egueb_Dom_Node *n,
		Egueb_Dom_Node *root)
{
	egueb_dom_document_element_set(n, root);
}

EAPI void egueb_svg_document_font_size_set(Egueb_Dom_Node *n,
		double font_size)
{
	Egueb_Svg_Document *thiz;
	Egueb_Dom_Node *topmost;

	thiz = EGUEB_SVG_DOCUMENT(n);
	thiz->font_size = font_size;

	topmost = egueb_dom_document_document_element_get(n);
	if (topmost)
	{
		egueb_dom_element_request_process(topmost);
		egueb_dom_node_unref(topmost);
	}
}

EAPI double egueb_svg_document_font_size_get(Egueb_Dom_Node *n)
{
	Egueb_Svg_Document *thiz;

	thiz = EGUEB_SVG_DOCUMENT(n);
	return thiz->font_size;
}

EAPI Egueb_Dom_Node * egueb_svg_document_element_get_by_iri(Egueb_Dom_Node *n,
		Egueb_Dom_String *iri)
{
	Egueb_Dom_Uri uri = { 0 };

	if (!egueb_dom_string_is_valid(iri))
		return NULL;

	/* resolve the uri for relative/absolute */
	DBG("Looking for %s", egueb_dom_string_string_get(iri));
	if (!egueb_dom_uri_iri_from(&uri, iri))
		return NULL;
	/* get the element by iri, only local ones for now */
	if (uri.location || !uri.fragment)
	{
		ERR("Unsupported iri '%s'", egueb_dom_string_string_get(iri));
		egueb_dom_uri_cleanup(&uri);
		return NULL;
	}
	else
	{
		Egueb_Dom_Node *ret;
		ret = egueb_dom_document_element_get_by_id(n, uri.fragment, NULL);
		egueb_dom_uri_cleanup(&uri);
		return ret;
	}
}

/* FIXME This might not be needed */
EAPI Egueb_Dom_Node * egueb_svg_document_iri_clone(Egueb_Dom_Node *n,
		Egueb_Dom_String *iri, Eina_Error *err)
{
	Egueb_Dom_Node *ref = NULL;
	Egueb_Dom_Node *cloned;

	ref = egueb_svg_document_element_get_by_iri(n, iri);
	if (!ref) return NULL;

	DBG("'%s' found, cloning it", egueb_dom_string_string_get(iri));
	/* clone the returned element */
	cloned = egueb_dom_node_clone(ref, EINA_FALSE, EINA_TRUE, err);
	egueb_dom_node_unref(ref);

	return cloned;
}
