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
#include "egueb_svg_renderable_private.h"
#include "egueb_svg_document_private.h"

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
	/* after the document is processed, we might need to check again
	 * the mouse position for new events
	 */
	int mouse_check;
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

		case EGUEB_SVG_TYPE_STYLE:
		ret = egueb_css_element_style_new();
		break;

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
 *                            Document interface                              *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Node * _egueb_svg_document_element_create(
		Egueb_Dom_Document *d, const char *ns, const char *name)
{
	Egueb_Dom_Node *ret;
	size_t sz;
	int id;

	sz = strlen(name);
	INFO("Creating element '%s' with namespace '%s'", name, ns);
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

static void _egueb_svg_document_post_process(Egueb_Dom_Document *d)
{
	Egueb_Svg_Document *thiz;
	Egueb_Dom_Node *topmost;
	Egueb_Dom_Node *n;
	Egueb_Dom_Feature *feature;
	Egueb_Dom_Input *input;
	int mx, my;

	n = EGUEB_DOM_NODE(d);
	topmost = egueb_dom_document_document_element_get(n);
	if (!topmost)
		return;
	thiz = EGUEB_SVG_DOCUMENT(n);
	if (!thiz->mouse_check)
		return;

	feature = egueb_dom_node_feature_get(topmost, EGUEB_DOM_FEATURE_UI_NAME, NULL);
	input = egueb_dom_feature_ui_input_get(feature);
	egueb_dom_input_mouse_position_get(input, &mx, &my);
	egueb_dom_input_feed_mouse_move(input, mx, my);
	egueb_dom_input_unref(input);
	egueb_dom_feature_unref(feature);
	thiz->mouse_check = 0;
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
	klass->post_process = _egueb_svg_document_post_process;
}

static void _egueb_svg_document_instance_init(void *o)
{
	Egueb_Svg_Document *thiz;

	thiz = EGUEB_SVG_DOCUMENT(o);
	thiz->font_size = 16;
	thiz->mouse_check = 0;
}

static void _egueb_svg_document_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* Function to send again a mouse move after the current processing */
void egueb_svg_document_mouse_check(Egueb_Dom_Node *n)
{
	Egueb_Svg_Document *thiz;

	thiz = EGUEB_SVG_DOCUMENT(n);
	thiz->mouse_check++;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_svg_is_document(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_DOCUMENT_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

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
		return egueb_dom_string_new_with_chars(filename);
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
