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
/* elements */
#include "egueb_svg_element_circle.h"
#include "egueb_svg_element_clip_path.h"
#include "egueb_svg_element_defs.h"
#include "egueb_svg_element_ellipse.h"
#include "egueb_svg_element_g.h"
#include "egueb_svg_element_image.h"
#include "egueb_svg_element_line.h"
#include "egueb_svg_element_linear_gradient.h"
#include "egueb_svg_element_radial_gradient.h"
#include "egueb_svg_element_rect.h"
#include "egueb_svg_element_use.h"
#include "egueb_svg_element_stop.h"
#include "egueb_svg_element_svg.h"
#include "egueb_svg_element_text.h"
#include "egueb_dom_document.h"

#include "egueb_dom_document_private.h"

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

	double last_width;
	double last_height;
	Eina_Bool changed;
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

#if 0
		case EGUEB_SVG_TYPE_PATTERN:
		ret = egueb_svg_parser_pattern_new(parser);
		break;
#endif
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
		case EGUEB_SVG_TYPE_ELLIPSE:
		ret = egueb_svg_element_ellipse_new();
		break;

		case EGUEB_SVG_TYPE_RECT:
		ret = egueb_svg_element_rect_new();
		break;

		case EGUEB_SVG_TYPE_LINE:
		ret = egueb_svg_element_line_new();
		break;

#if 0
		case EGUEB_SVG_TYPE_PATH:
		ret = egueb_svg_element_path_new();
		break;

		case EGUEB_SVG_TYPE_POLYLINE:
		ret = egueb_svg_element_polyline_new();
		break;

		case EGUEB_SVG_TYPE_POLYGON:
		ret = egueb_svg_element_polygon_new();
		break;
#endif
		case EGUEB_SVG_TYPE_TEXT:
		ret = egueb_svg_element_text_new();
		break;

		case EGUEB_SVG_TYPE_G:
		ret = egueb_svg_element_g_new();
		break;
#if 0
		case EGUEB_SVG_TYPE_A:
		ret = egueb_svg_element_a_new();
		break;

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
#if 0
		case EGUEB_SVG_TYPE_ANIMATE:
		ret = egueb_svg_element_animate_new();
		break;

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

static void _egueb_svg_document_process(Egueb_Dom_Document *d)
{
	Egueb_Svg_Document *thiz;
	Eina_Bool changed;

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
		thiz->last_width = thiz->width;
		thiz->last_height = thiz->height;
	}
	else
	{
		DBG("Processing list of changed elements");
		egueb_dom_document_process_default(d);
	}
	thiz->changed = EINA_FALSE;
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
}

static void _egueb_svg_document_instance_init(void *o)
{
	Egueb_Svg_Document *thiz;

	thiz = EGUEB_SVG_DOCUMENT(o);
	thiz->font_size = 16;
}

static void _egueb_svg_document_instance_deinit(void *o)
{
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
	/* FIXME here we should use the application interface */
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

		thiz = EGUEB_SVG_DOCUMENT(n);
		egueb_svg_element_svg_width_get(topmost, &width);
		*actual_width = egueb_svg_coord_final_get(&width.anim, thiz->width, thiz->font_size);
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

		thiz = EGUEB_SVG_DOCUMENT(n);
		egueb_svg_element_svg_height_get(topmost, &height);
		*actual_height = egueb_svg_coord_final_get(&height.anim, thiz->height, thiz->font_size);
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
#if 0
	Egueb_Svg_Document_Svg *thiz;

	thiz = EGUEB_SVG_DOCUMENT(t);
	egueb_svg_input_feed_mouse_move(thiz->input, x, y);
#endif
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_document_feed_mouse_down(Egueb_Dom_Node *n, int button)
{
#if 0
	Egueb_Svg_Document_Svg *thiz;

	thiz = EGUEB_SVG_DOCUMENT(t);
	egueb_svg_input_feed_mouse_down(thiz->input, button);
#endif
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_document_feed_mouse_up(Egueb_Dom_Node *n, int button)
{
#if 0
	Egueb_Svg_Document_Svg *thiz;

	thiz = EGUEB_SVG_DOCUMENT(t);
	egueb_svg_input_feed_mouse_up(thiz->input, button);
#endif
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
