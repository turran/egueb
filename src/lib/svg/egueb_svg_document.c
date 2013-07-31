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

#include "egueb_svg_document.h"
/* needed types */
#include "egueb_svg_referenceable_units.h"
#include "egueb_svg_string.h"
#include "egueb_svg_length.h"
#include "egueb_svg_overflow.h"
#include "egueb_svg_point.h"
/* elements */
#include "egueb_svg_element.h"
#include "egueb_svg_element_a.h"
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
#include "egueb_dom_document.h"
/* abstracts */
#include "egueb_svg_renderable.h"
#include "egueb_svg_renderable_container.h"

#include "egueb_dom_document_private.h"

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

	/* damages */
	Eina_Tiler *tiler;
	int tw;
	int th;

	/* input */
	Egueb_Dom_Input *input;

	/* animation */
	Etch *etch;

	/* our own state */
	double last_width;
	double last_height;
	Eina_Bool changed;

	/* application interface */
	Egueb_Svg_Document_String_Get_Cb location_get;
	void *location_get_data;
	Egueb_Svg_Document_String_Get_Cb filename_get;
	void *filename_get_data;

} Egueb_Svg_Document;

typedef struct _Egueb_Svg_Document_Class
{
	Egueb_Dom_Document_Class parent;
} Egueb_Svg_Document_Class;

typedef struct _Egueb_Svg_Document_Uri_Data
{
	Egueb_Dom_Node *node;
	Egueb_Dom_Node *ret;
	Eina_Error error;
} Egueb_Svg_Document_Uri_Data;

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

static Egueb_Dom_Node * _egueb_svg_document_element_create(int id)
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
#if 0
		case EGUEB_SVG_TYPE_SCRIPT:
		ret = egueb_svg_element_script_new();
		break;

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
#if 0
		case EGUEB_SVG_TYPE_ANIMATETRANSFORM:
		ret = egueb_svg_element_animate_transform_new();
		break;

		case EGUEB_SVG_TYPE_VIDEO:
		ret = egueb_svg_element_video_new();
		break;
#endif

		default:
		break;
	}
	return ret;
}

static Eina_Bool _egueb_svg_document_has_changed(Egueb_Svg_Document *thiz)
{
	if (!thiz->changed)
		return EINA_FALSE;
	/* TODO check that the topmost element has a relative coordinates */
	if (thiz->width != thiz->last_width)
		return EINA_TRUE;
	else if (thiz->height != thiz->last_height)
		return EINA_TRUE;
	return EINA_FALSE;
}

static char * _egueb_svg_document_uri_get_absolute(Egueb_Svg_Document *thiz,
		const char *uri)
{
	Eina_Bool free_location = EINA_FALSE;
	const char *location = NULL;
	char *tmp = NULL;
	char *ret;

	if (!strncmp(uri, "http://", 7) || !strncmp(uri, "/", 1))
	{
		ERR("absolute uri '%s'", uri);
		return strdup(uri);
	}

	if (thiz->location_get)
	{
		location = thiz->location_get(thiz->location_get_data);
	}
	if (!location)
	{
		const char *filename = NULL;
		if (thiz->filename_get)
		{
			filename = thiz->filename_get(thiz->filename_get_data);
		}

		if (!filename)
		{
			location = getcwd(NULL, 0);
			free_location = EINA_TRUE;
		}
		else
		{
			tmp = strdup(filename);
			location = dirname(tmp);
		}
	}
	/* ok, we got the location, not concat it with the uri */
	INFO("location is '%s' and uri '%s'", location, uri);
	ret = eina_extra_str_dup_printf("%s/%s", location, uri);
	if (free_location)
		free((char *)location);
	if (tmp)
		free(tmp);

	return ret;
}

static Eina_Bool _egueb_svg_document_damage_cb(Enesim_Renderer *r,
		const Eina_Rectangle *area, Eina_Bool past,
		void *data)
{
	Eina_Tiler *tiler = data;
	const char *name;

	eina_tiler_rect_add(tiler, area);
	enesim_renderer_name_get(r, &name);
	DBG("Renderer %s has changed at area %d %d %d %d", name, area->x,
			area->y, area->w, area->h);
	return EINA_TRUE;
}

/*----------------------------------------------------------------------------*
 *                     The URI interface for elements                         *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_document_element_uri_local_get(const char *uri,
		const char *fragment, void *user_data)
{
	Egueb_Svg_Document_Uri_Data *data = user_data;
	Egueb_Dom_String *id;

	DBG("Getting local element with fragment '%s'", fragment);
	/* FIXME avoid this copy, for that we need to pass directly
	 * the dom string to the uri descriptor, instead of the char *
	 */
	id = egueb_dom_string_new_with_static_string(fragment);
	data->error = egueb_dom_document_element_get_by_id(data->node, id,
			&data->ret);
	egueb_dom_string_unref(id);
}

/* TODO For absolute IRI we should acquire the element using the application
 * descriptor to fetch it
 */
static void _egueb_svg_document_element_uri_absolute_get(const char *uri,
		const char *fragment, void *user_data)
{
	ERR("Absolute elements '%s:%s' are not supported yet", uri, fragment);
}

/* TODO For relative IRI should acquire the element concating the location
 * with the uri
 */
static void _egueb_svg_document_element_uri_relative_get(const char *uri,
		const char *fragment, void *user_data)
{
	ERR("Relative elements '%s:%s' are not supported yet", uri, fragment);
}

static Egueb_Dom_Uri_Descriptor _element_uri_descriptor = {
	/* .local_get 		= */ _egueb_svg_document_element_uri_local_get,
	/* .absolute_get 	= */ _egueb_svg_document_element_uri_absolute_get,
	/* .relative_get 	= */ _egueb_svg_document_element_uri_relative_get,
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
		egueb_dom_node_child_last_get(n, &child);
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

			egueb_dom_node_sibling_previous_get(child, &tmp);
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

			egueb_dom_node_name_get(n, &name);
			DBG("Element '%s' found with bounds %"
					EINA_EXTRA_RECTANGLE_FORMAT,
					egueb_dom_string_string_get(name),
					EINA_EXTRA_RECTANGLE_ARGS(&bounds));
			egueb_dom_string_unref(name);
			ret = egueb_dom_node_ref(n);
		}
	}
done:
	egueb_dom_node_unref(n);
	return ret;
}

static Egueb_Dom_Node * _egueb_svg_document_input_element_at(void *data,
		int x, int y)
{
	Egueb_Svg_Document *thiz = data;
	Egueb_Dom_Node *n = NULL;
	Egueb_Dom_Node *ret;
	Eina_Rectangle ptr;

	egueb_dom_document_element_get(EGUEB_DOM_NODE(thiz), &n);
	if (!n) return NULL;

	/* iterate over the whole tree */
	eina_rectangle_coords_from(&ptr, x, y, 1, 1);
	ret = _egueb_svg_document_input_element_at_recursive(n, &ptr);
	return ret;
}

static Egueb_Dom_Input_Descriptor _document_svg_input_descriptor = {
	/* .element_ad 		= */ _egueb_svg_document_input_element_at,
};
/*----------------------------------------------------------------------------*
 *                            Document interface                              *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Node * _egueb_svg_document_class_element_create(
		Egueb_Dom_Document *d, const char *name)
{
	Egueb_Dom_Node *ret;
	size_t sz;
	int id;

	sz = strlen(name);
	if (!_egueb_svg_document_element_id_get(name, sz, &id))
		return NULL;
	ret = _egueb_svg_document_element_create(id);
	if (!ret)
	{
		/* FIXME here we should use the application interface to let
		 * the application create new elements
		 */
		WARN("can't create the tag %s", name);
	}
	return ret;
}

static void _egueb_svg_document_process(Egueb_Dom_Node *n)
{
	Egueb_Svg_Document *thiz;
	Egueb_Dom_Document *d;
	Eina_Bool changed;

	d = EGUEB_DOM_DOCUMENT(n);
	thiz = EGUEB_SVG_DOCUMENT(d);
	/* be sure we have actually changed */
	changed = _egueb_svg_document_has_changed(thiz);
	/* if so, force a complete process */
	if (changed)
	{
		if (!d->element)
		{
			DBG("Nothing to process. No topmost element found");
			return;
		}
		DBG("Processing topmost element only");
		egueb_dom_element_process(d->element);
		/* remove every enqueued element */
		egueb_dom_document_process_queue_clear(n);

		thiz->last_width = thiz->width;
		thiz->last_height = thiz->height;
	}
	else
	{
		DBG("Processing list of changed elements");
		egueb_dom_document_process_default(n);
	}
	thiz->changed = EINA_FALSE;
}

static Eina_Bool _egueb_svg_document_needs_process(Egueb_Dom_Node *n)
{
	Egueb_Svg_Document *thiz;

	thiz = EGUEB_SVG_DOCUMENT(n);
	return _egueb_svg_document_has_changed(thiz);
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

	klass->element_create = _egueb_svg_document_class_element_create;
	klass->process = _egueb_svg_document_process;
	klass->needs_process = _egueb_svg_document_needs_process;
}

static void _egueb_svg_document_instance_init(void *o)
{
	Egueb_Svg_Document *thiz;

	thiz = EGUEB_SVG_DOCUMENT(o);
	thiz->font_size = 16;
	thiz->input = egueb_dom_input_new(&_document_svg_input_descriptor, thiz);
	thiz->etch = etch_new();
}

static void _egueb_svg_document_instance_deinit(void *o)
{
	Egueb_Svg_Document *thiz;

	thiz = EGUEB_SVG_DOCUMENT(o);
	egueb_dom_input_free(thiz->input);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_svg_document_new(void *app)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_document);
	return n;
}

/* readonly attribute DOMString title; */
EAPI Eina_Error egueb_svg_document_title_get(Egueb_Dom_Node *n,
		Egueb_Dom_String **title)
{
	/* FIXME here we should use the application interface */
	return EINA_ERROR_NONE;
}

/* readonly attribute DOMString referrer; */
EAPI Eina_Error egueb_svg_document_referrer_get(Egueb_Dom_Node *n,
		Egueb_Dom_String **referrer)
{
	/* FIXME here we should use the application interface */
	return EINA_ERROR_NONE;
}

/* readonly attribute DOMString domain; */
EAPI Eina_Error egueb_svg_document_domain_get(Egueb_Dom_Node *n,
		Egueb_Dom_String **domain)
{
	/* FIXME here we should use the application interface */
	return EINA_ERROR_NONE;
}

/* readonly attribute DOMString URL */
EAPI Eina_Error egueb_svg_document_url_get(Egueb_Dom_Node *n,
		Egueb_Dom_String **url)
{
	Egueb_Svg_Document *thiz;
	const char *location = NULL;
	const char *filename = NULL;
	char *ret;

	thiz = EGUEB_SVG_DOCUMENT(n);
	if (!thiz->filename_get)
		return EGUEB_DOM_ERROR_NOT_FOUND;
	filename = thiz->filename_get(thiz->filename_get_data);
	if (!filename)
		return EGUEB_DOM_ERROR_NOT_FOUND;
	/* check if it is relative or absolute */
	if (!strncmp(filename, "http://", 7) || strncmp(filename, "/", 1))
	{
		*url = egueb_dom_string_new_with_string(filename);
		return EINA_ERROR_NONE;
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
		ret = eina_extra_str_dup_printf("%s/%s", location, filename);
		free(tmp);
	}
	else
	{
		ret = eina_extra_str_dup_printf("%s/%s", location, filename);
	}
	*url = egueb_dom_string_steal(ret);
	return EINA_ERROR_NONE;
}

/* readonly attribute SVGSVGElement rootElement; */
EAPI Eina_Error egueb_svg_document_element_root_get(Egueb_Dom_Node *n,
		Egueb_Dom_Node **root)
{
	egueb_dom_document_element_get(n, root);
	return EINA_ERROR_NONE;
}

EAPI Eina_Error egueb_svg_document_element_root_set(Egueb_Dom_Node *n,
		Egueb_Dom_Node *root)
{
	egueb_dom_document_element_set(n, root);
	return EINA_ERROR_NONE;
}

EAPI Eina_Error egueb_svg_document_width_set(Egueb_Dom_Node *n,
		double width)
{
	Egueb_Svg_Document *thiz;

	thiz = EGUEB_SVG_DOCUMENT(n);
	thiz->width = width;
	thiz->changed = EINA_TRUE;
	return EINA_ERROR_NONE;
}

EAPI Eina_Error egueb_svg_document_width_get(Egueb_Dom_Node *n,
		double *width)
{
	Egueb_Svg_Document *thiz;

	if (!width) return EGUEB_DOM_ERROR_INVALID_ACCESS;
	thiz = EGUEB_SVG_DOCUMENT(n);
	*width = thiz->width;
	return EINA_ERROR_NONE;
}

EAPI Eina_Error egueb_svg_document_height_set(Egueb_Dom_Node *n,
		double height)
{
	Egueb_Svg_Document *thiz;

	thiz = EGUEB_SVG_DOCUMENT(n);
	thiz->height = height;
	thiz->changed = EINA_TRUE;
	return EINA_ERROR_NONE;
}

EAPI Eina_Error egueb_svg_document_height_get(Egueb_Dom_Node *n,
		double *height)
{
	Egueb_Svg_Document *thiz;

	if (!height) return EGUEB_DOM_ERROR_INVALID_ACCESS;
	thiz = EGUEB_SVG_DOCUMENT(n);
	*height = thiz->height;
	return EINA_ERROR_NONE;
}

EAPI void egueb_svg_document_actual_width_get(Egueb_Dom_Node *n, double *actual_width)
{
	Egueb_Dom_Node *topmost = NULL;

	egueb_dom_document_element_get(n, &topmost);
	if (!topmost)
	{
		*actual_width = 0;
	}
	else
	{
		Egueb_Svg_Document *thiz;
		Egueb_Svg_Length_Animated width;
		Egueb_Svg_Overflow overflow;

		thiz = EGUEB_SVG_DOCUMENT(n);

		egueb_svg_element_svg_width_get(topmost, &width);
		*actual_width = egueb_svg_coord_final_get(&width.anim, thiz->width,
				thiz->font_size);
		egueb_svg_element_overflow_final_get(topmost, &overflow);
		/* check if we should overflow */
		if (overflow == EGUEB_SVG_OVERFLOW_VISIBLE)
		{
			Eina_Rectangle r;
			egueb_svg_renderable_user_bounds_get(topmost, &r);
			if (*actual_width < r.x + r.w)
				*actual_width = r.x + r.w;
		}
		egueb_dom_node_unref(topmost);
	}
}

EAPI void egueb_svg_document_actual_height_get(Egueb_Dom_Node *n, double *actual_height)
{
	Egueb_Dom_Node *topmost = NULL;

	egueb_dom_document_element_get(n, &topmost);
	if (!topmost)
	{
		*actual_height = 0;
	}
	else
	{
		Egueb_Svg_Document *thiz;
		Egueb_Svg_Length_Animated height;
		Egueb_Svg_Overflow overflow;

		thiz = EGUEB_SVG_DOCUMENT(n);

		egueb_svg_element_svg_height_get(topmost, &height);
		*actual_height = egueb_svg_coord_final_get(&height.anim, thiz->height,
				thiz->font_size);
		egueb_svg_element_overflow_final_get(topmost, &overflow);
		if (overflow == EGUEB_SVG_OVERFLOW_VISIBLE)
		{
			Eina_Rectangle r;
			egueb_svg_renderable_user_bounds_get(topmost, &r);
			if (*actual_height < r.y + r.h)
				*actual_height = r.y + r.h;
		}
		egueb_dom_node_unref(topmost);
	}
}

EAPI Eina_Error egueb_svg_document_font_size_set(Egueb_Dom_Node *n,
		double font_size)
{
	Egueb_Svg_Document *thiz;

	thiz = EGUEB_SVG_DOCUMENT(n);
	thiz->font_size = font_size;
	thiz->changed = EINA_TRUE;
	return EINA_ERROR_NONE;
}

EAPI Eina_Error egueb_svg_document_font_size_get(Egueb_Dom_Node *n,
		double *font_size)
{
	Egueb_Svg_Document *thiz;

	if (!font_size) return EGUEB_DOM_ERROR_INVALID_ACCESS;
	thiz = EGUEB_SVG_DOCUMENT(n);
	*font_size = thiz->font_size;
	return EINA_ERROR_NONE;
}

EAPI Eina_Error egueb_svg_document_element_get_by_iri(Egueb_Dom_Node *n,
		Egueb_Dom_String *iri, Egueb_Dom_Node **ref)
{
	Egueb_Svg_Document_Uri_Data data;
	const char *str;

	if (!egueb_dom_string_is_valid(iri) || !ref)
		return EGUEB_DOM_ERROR_INVALID_ACCESS;

	data.node = n;
	data.ret = NULL;
	data.error = EINA_ERROR_NONE;

	/* resolve the uri for relative/absolute */
	str = egueb_dom_string_string_get(iri);
	DBG("Looking for %s", str);
	egueb_dom_iri_string_from(&_element_uri_descriptor, str, &data);
	*ref = data.ret;
	return data.error;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_document_feed_mouse_move(Egueb_Dom_Node *n, int x, int y)
{
	Egueb_Svg_Document *thiz;

	thiz = EGUEB_SVG_DOCUMENT(n);
	egueb_dom_input_feed_mouse_move(thiz->input, x, y);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_document_feed_mouse_down(Egueb_Dom_Node *n, int button)
{
	Egueb_Svg_Document *thiz;

	thiz = EGUEB_SVG_DOCUMENT(n);
	egueb_dom_input_feed_mouse_down(thiz->input, button);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_document_feed_mouse_up(Egueb_Dom_Node *n, int button)
{
	Egueb_Svg_Document *thiz;

	thiz = EGUEB_SVG_DOCUMENT(n);
	egueb_dom_input_feed_mouse_up(thiz->input, button);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_document_damages_get(Egueb_Dom_Node *n,
		Egueb_Svg_Document_Damage_Cb cb, void *data)
{
	Egueb_Svg_Document *thiz;
	Egueb_Dom_Node *topmost = NULL;
	Enesim_Renderer *r;
	Eina_Iterator *iter;
	Eina_Rectangle *rect;
	double aw, ah;
	int tw, th;

	thiz = EGUEB_SVG_DOCUMENT(n);
	egueb_svg_document_actual_width_get(n, &aw);
	egueb_svg_document_actual_height_get(n, &ah);
	tw = ceil(aw);
	th = ceil(ah);

	if (!thiz->tiler || thiz->tw != tw || thiz->th != th)
	{
		Eina_Rectangle full;

		if (thiz->tiler)
			eina_tiler_free(thiz->tiler);
		thiz->tiler = eina_tiler_new(tw, th);
		thiz->tw = tw;
		thiz->th = th;

		eina_rectangle_coords_from(&full, 0, 0, tw, th);
		cb(n, &full, data);
		return;
	}

	egueb_dom_document_element_get(n, &topmost);
	if (!topmost) return;

	r = egueb_svg_renderable_renderer_get(topmost);
	enesim_renderer_damages_get(r, _egueb_svg_document_damage_cb, thiz->tiler);

	iter = eina_tiler_iterator_new(thiz->tiler);
	EINA_ITERATOR_FOREACH(iter, rect)
	{
		cb(n, rect, data);
	}
	eina_iterator_free(iter);
	eina_tiler_clear(thiz->tiler);
	egueb_dom_node_unref(topmost);
}

EAPI void egueb_svg_document_image_load(Egueb_Dom_Node *n,
		Egueb_Dom_String *uri,
		Egueb_Svg_Document_Image_Load_Descriptor *d,
		void *user_data)
{
	Egueb_Svg_Document *thiz;
	char *absolute;

	thiz = EGUEB_SVG_DOCUMENT(n);
	absolute = _egueb_svg_document_uri_get_absolute(thiz,
		egueb_dom_string_string_get(uri));
	/* resolve the uri to make it absolute */
	ERR("URI is %s", absolute);
	free(absolute);
}

EAPI void egueb_svg_document_image_data_load(Egueb_Dom_Node *n,
		Enesim_Image_Data *data,
		Egueb_Svg_Document_Image_Load_Descriptor *d,
		void *user_data)
{
	Enesim_Buffer *b = NULL;

	if (!d || !d->loaded)
		return;

	/* TODO check if there is an application callback */
	if (!enesim_image_load(data, NULL, &b, NULL, NULL))
	{
		Eina_Error err;

		err = eina_error_get();
		ERR("Can not load image, error: %s", eina_error_msg_get(err));
		if (d->failed) d->failed(user_data);
	}
	else
	{
		Enesim_Surface *s;

		s = enesim_surface_new_buffer_from(b);
		d->loaded(s, user_data);
	}
}

EAPI void egueb_svg_document_uri_fetch(Egueb_Dom_Node *n,
		Egueb_Dom_String *uri,
		Egueb_Svg_Document_Uri_Fetch_Descriptor *d,
		void *user_data)
{
	Egueb_Svg_Document *thiz;
	char *absolute;

	if (!d || !d->fetched)
		goto done;

	thiz = EGUEB_SVG_DOCUMENT(n);
	/* resolve the uri to make it absolute */
	absolute = _egueb_svg_document_uri_get_absolute(thiz,
		egueb_dom_string_string_get(uri));
	DBG("URI is %s", absolute);
	/* in case of a http uri, delegate it to the application */
	if (!strncmp(absolute, "/", 1))
	{
		Enesim_Image_Data *data;

		data = enesim_image_data_file_new(absolute, "rb");
		if (data)
		{
			if (d->fetched) d->fetched(data, user_data);
			enesim_image_data_free(data);
		}
		else
		{
			if (d->failed) d->failed(user_data);
		}
	}
	else
	{
		/* TODO call the application to fetch the data */
		ERR("TODO");
	}
	/* call the descriptor functions */
	free(absolute);
done:
	egueb_dom_string_unref(uri);
}

EAPI void egueb_svg_document_location_get_cb_set(Egueb_Dom_Node *n,
		Egueb_Svg_Document_String_Get_Cb cb, void *user_data)
{
	Egueb_Svg_Document *thiz;

	thiz = EGUEB_SVG_DOCUMENT(n);
	thiz->location_get = cb;
	thiz->location_get_data = user_data;
}

EAPI void egueb_svg_document_filename_get_cb_set(Egueb_Dom_Node *n,
		Egueb_Svg_Document_String_Get_Cb cb, void *user_data)
{
	Egueb_Svg_Document *thiz;

	thiz = EGUEB_SVG_DOCUMENT(n);
	thiz->filename_get = cb;
	thiz->filename_get_data = user_data;
}

/* FIXME This might not be needed */
EAPI Eina_Error egueb_svg_document_iri_clone(Egueb_Dom_Node *n,
		Egueb_Dom_String *iri, Egueb_Dom_Node **cloned)
{
	Egueb_Dom_Node *ref = NULL;
	Eina_Error err;

	err = egueb_svg_document_element_get_by_iri(n, iri, &ref);
	if (err != EINA_ERROR_NONE) return err;
	if (ref)
	{
		DBG("'%s' found, cloning it", egueb_dom_string_string_get(iri));
		/* clone the returned element */
		egueb_dom_node_clone(ref, EINA_FALSE, EINA_TRUE, cloned);
		egueb_dom_node_unref(ref);
	}

	return EINA_ERROR_NONE;
}
