/* Esvg - SVG
 * Copyright (C) 2011 Jorge Luis Zapata, Vincent Torri
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
#include "egueb_svg_private_attribute_presentation.h"
#include "egueb_svg_types_private.h"

#include "egueb_svg_types.h"

#include <stdlib.h>
#include <errno.h>
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

#define ESVG_LOG_DEFAULT _egueb_svg_type_log

static int _egueb_svg_type_log = -1;

/*----------------------------------------------------------------------------*
 *                               Generic helpers                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_double_get(const char *iter, const char **tmp, double *v)
{
	char *endptr;
	double val;

	val = egueb_base_double_get(iter, &endptr);
	if ((errno != ERANGE) &&
	    !((val == 0) && (iter == endptr)))
	{
		*tmp = endptr;
		*v = val;
		return EINA_TRUE;
	}
	return EINA_FALSE;
}

/*----------------------------------------------------------------------------*
 *                         Timing related functions                           *
 *----------------------------------------------------------------------------*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Svg_Length ESVG_LENGTH_0 = { 0.0, ESVG_UNIT_LENGTH_PX };
Egueb_Svg_Length ESVG_LENGTH_1 = { 1.0, ESVG_UNIT_LENGTH_PX };
Egueb_Svg_Length ESVG_LENGTH_100_PERCENT = { 100.0, ESVG_UNIT_LENGTH_PERCENT };

void egueb_svg_type_init(void)
{
	_egueb_svg_type_log = eina_log_domain_register("egueb_svg_type", ESVG_LOG_COLOR_DEFAULT);
	if (_egueb_svg_type_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
}

void egueb_svg_type_shutdown(void)
{
	if (_egueb_svg_type_log < 0)
		return;
	eina_log_domain_unregister(_egueb_svg_type_log);
	_egueb_svg_type_log = -1;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI const char * egueb_svg_type_string_to(Egueb_Svg_Type type)
{
	switch (type)
	{
		case ESVG_TYPE_A: return "a";
		case ESVG_TYPE_ALTGLYPH: return "";
		case ESVG_TYPE_ALTGLYPHDEF: return "";
		case ESVG_TYPE_ALTGLYPHITEM: return "";
		case ESVG_TYPE_ANIMATE: return "animate";
		case ESVG_TYPE_ANIMATECOLOR: return "animateColor";
		case ESVG_TYPE_ANIMATEMOTION: return "animateMotion";
		case ESVG_TYPE_ANIMATETRANSFORM: return "animateTransform";
		case ESVG_TYPE_CIRCLE: return "circle";
		case ESVG_TYPE_CLIPPATH: return "clipPath";
		case ESVG_TYPE_COLOR_PROFILE: return "" /* 10 */;
		case ESVG_TYPE_CURSOR: return "cursor";
		case ESVG_TYPE_DEFINITION_SRC: return "";
		case ESVG_TYPE_DEFS: return "defs";
		case ESVG_TYPE_DESC: return "desc";
		case ESVG_TYPE_ELLIPSE: return "ellipse";
		case ESVG_TYPE_FEBLEND: return "feBlend";
		case ESVG_TYPE_FECOLORMATRIX: return "";
		case ESVG_TYPE_FECOMPONENTTRANSFER: return "";
		case ESVG_TYPE_FECOMPOSITE: return "";
		case ESVG_TYPE_FECONVOLVEMATRIX: return "" /* 20 */;
		case ESVG_TYPE_FEDIFFUSELIGHTING: return "";
		case ESVG_TYPE_FEDISPLACEMENTMAP: return "";
		case ESVG_TYPE_FEDISTANTLIGHT: return "";
		case ESVG_TYPE_FEFLOOD: return "";
		case ESVG_TYPE_FEFUNCA: return "";
		case ESVG_TYPE_FEFUNCB: return "";
		case ESVG_TYPE_FEFUNCG: return "";
		case ESVG_TYPE_FEFUNCR: return "";
		case ESVG_TYPE_FEGAUSSIANBLUR: return "";
		case ESVG_TYPE_FEIMAGE: return "" /* 30 */;
		case ESVG_TYPE_FEMERGE: return "";
		case ESVG_TYPE_FEMERGENODE: return "";
		case ESVG_TYPE_FEMORPHOLOGY: return "";
		case ESVG_TYPE_FEOFFSET: return "";
		case ESVG_TYPE_FEPOINTLIGHT: return "";
		case ESVG_TYPE_FESPECULARLIGHTING: return "";
		case ESVG_TYPE_FESPOTLIGHT: return "";
		case ESVG_TYPE_FETILE: return "";
		case ESVG_TYPE_FETURBULENCE: return "";
		case ESVG_TYPE_FILTER: return "" /* 40 */;
		case ESVG_TYPE_FONT: return "";
		case ESVG_TYPE_FONT_FACE: return "";
		case ESVG_TYPE_FONT_FACE_FORMAT: return "";
		case ESVG_TYPE_FONT_FACE_NAME: return "";
		case ESVG_TYPE_FONT_FACE_SRC: return "";
		case ESVG_TYPE_FONT_FACE_URI: return "";
		case ESVG_TYPE_FOREIGNOBJECT: return "";
		case ESVG_TYPE_G: return "g";
		case ESVG_TYPE_GLYPH: return "";
		case ESVG_TYPE_GLYPHREF: return "" /* 50 */;
		case ESVG_TYPE_HKERN: return "";
		case ESVG_TYPE_IMAGE: return "image";
		case ESVG_TYPE_LINE: return "line";
		case ESVG_TYPE_LINEARGRADIENT: return "linearGradient";
		case ESVG_TYPE_MARKER: return "marker";
		case ESVG_TYPE_MASK: return "mask";
		case ESVG_TYPE_METADATA: return "";
		case ESVG_TYPE_MISSING_GLYPH: return "";
		case ESVG_TYPE_MPATH: return "";
		case ESVG_TYPE_PATH: return "path" /* 60 */;
		case ESVG_TYPE_PATTERN: return "pattern";
		case ESVG_TYPE_POLYGON: return "polygon";
		case ESVG_TYPE_POLYLINE: return "polyline";
		case ESVG_TYPE_RADIALGRADIENT: return "radialGradient";
		case ESVG_TYPE_RECT: return "rect";
		case ESVG_TYPE_SCRIPT: return "script";
		case ESVG_TYPE_SET: return "set";
		case ESVG_TYPE_STOP: return "stop";
		case ESVG_TYPE_STYLE: return "style";
		case ESVG_TYPE_SVG: return "svg" /* 70 */;
		case ESVG_TYPE_SWITCH: return "";
		case ESVG_TYPE_SYMBOL: return "";
		case ESVG_TYPE_TEXT: return "text";
		case ESVG_TYPE_TEXTPATH: return "";
		case ESVG_TYPE_TITLE: return "";
		case ESVG_TYPE_TREF: return "";
		case ESVG_TYPE_TSPAN: return "";
		case ESVG_TYPE_USE: return "use";
		case ESVG_TYPE_VIDEO: return "video";
		case ESVG_TYPE_VIEW: return "";
		case ESVG_TYPE_VKERN: return "";
		default: return "unknown";
	}
}

EAPI Eina_Bool egueb_svg_type_is_animation(Egueb_Svg_Type type)
{
	switch (type)
	{
		case ESVG_TYPE_ANIMATE:
		case ESVG_TYPE_ANIMATECOLOR:
		case ESVG_TYPE_ANIMATETRANSFORM:
		case ESVG_TYPE_SET:
		return EINA_TRUE;

		default:
		return EINA_FALSE;
	}
}

EAPI Eina_Bool egueb_svg_type_is_paint_server(Egueb_Svg_Type type)
{
	switch (type)
	{
		case ESVG_TYPE_LINEARGRADIENT:
		case ESVG_TYPE_RADIALGRADIENT:
		case ESVG_TYPE_PATTERN:
		return EINA_TRUE;

		default:
		return EINA_FALSE;
	}
}

EAPI Eina_Bool egueb_svg_type_is_shape(Egueb_Svg_Type type)
{
	switch (type)
	{
		case ESVG_TYPE_LINE:
		case ESVG_TYPE_RECT:
		case ESVG_TYPE_POLYLINE:
		case ESVG_TYPE_POLYGON:
		case ESVG_TYPE_ELLIPSE:
		case ESVG_TYPE_CIRCLE:
		case ESVG_TYPE_PATH:
		return EINA_TRUE;

		default:
		return EINA_FALSE;
	}
}

EAPI Eina_Bool egueb_svg_type_is_renderable(Egueb_Svg_Type type)
{
	if (egueb_svg_type_is_shape(type))
		return EINA_TRUE;
	switch (type)
	{
		case ESVG_TYPE_SVG:
		case ESVG_TYPE_G:
		case ESVG_TYPE_IMAGE:
		case ESVG_TYPE_TEXT:
		case ESVG_TYPE_USE:
		case ESVG_TYPE_VIDEO:
		return EINA_TRUE;

		default:
		return EINA_FALSE;
	}
}


EAPI Eina_Bool egueb_svg_attribute_type_string_from(Egueb_Svg_Attribute_Type *type, const char *value)
{
	Eina_Bool ret = EINA_TRUE;

	if (!strcmp(value, "CSS"))
		*type = ESVG_ATTR_CSS;
	else if (!strcmp(value, "XML"))
		*type = ESVG_ATTR_XML;
	else if (!strcmp(value, "auto"))
		*type = ESVG_ATTR_AUTO;
	else
		ret = EINA_FALSE;
	return ret;
}

/* x1,y1 x2,y2 ... */
EAPI void egueb_svg_points_string_from(const char *value, Egueb_Svg_Points_Cb cb, void *data)
{
	const char *tmp;
	char *endptr;

	EGUEB_BASE_SPACE_SKIP(value);
	tmp = value;
	while (*tmp)
	{
		Egueb_Svg_Point p;

		p.x = egueb_base_double_get(tmp, &endptr);
		tmp = endptr;
		EGUEB_BASE_SPACE_COMMA_SKIP(tmp);
		p.y = egueb_base_double_get(tmp, &endptr);
		tmp = endptr;
		EGUEB_BASE_SPACE_COMMA_SKIP(tmp);

		cb(&p, data);
	}
}


EAPI void egueb_svg_timing_string_from(const char *attr, Egueb_Svg_Timing_Cb cb, void *data)
{

}

EAPI Eina_Bool egueb_svg_animate_transform_type_string_from(Egueb_Svg_Animate_Transform_Type *type, const char *s)
{
	Eina_Bool ret = EINA_TRUE;

	if (!strcmp(s, "translate"))
		*type = ESVG_ANIMATE_TRANSFORM_TYPE_TRANSLATE;
	else if (!strcmp(s, "scale"))
		*type = ESVG_ANIMATE_TRANSFORM_TYPE_SCALE;
	else if (!strcmp(s, "rotate"))
		*type = ESVG_ANIMATE_TRANSFORM_TYPE_ROTATE;
	else if (!strcmp(s, "skewX"))
		*type = ESVG_ANIMATE_TRANSFORM_TYPE_SKEWX;
	else if (!strcmp(s, "skewY"))
		*type = ESVG_ANIMATE_TRANSFORM_TYPE_SKEWY;
	else
		ret = EINA_FALSE;

	return ret;
}

EAPI Eina_Bool egueb_svg_number_list_string_from(const char *attr, Egueb_Svg_Number_List_Cb cb, void *data)
{
	const char *tmp = attr;
	char *end;

	if (!attr) return EINA_FALSE;
	if (!cb) return EINA_FALSE;

	EGUEB_BASE_SPACE_SKIP(tmp);
	while (tmp)
	{
		double val;

		EGUEB_BASE_SPACE_SKIP(tmp);
		if (!*tmp)
			break;

		val = egueb_base_double_get(tmp, &end);
		if (errno == ERANGE)
			val = 0;
		if (end == tmp)
			break;
		tmp = end;
		if (!cb(val, data))
			break;
		/* skip the comma and the blanks */
		EGUEB_BASE_SPACE_COMMA_SKIP(tmp);
	}
	return EINA_TRUE;
}

EAPI Eina_Bool egueb_svg_calc_mode_string_from(Egueb_Svg_Calc_Mode *c, const char *attr)
{
	Eina_Bool ret = EINA_TRUE;

	if (!strcmp(attr, "discrete"))
		*c = ESVG_CALC_MODE_DISCRETE;
	else if (!strcmp(attr, "linear"))
		*c = ESVG_CALC_MODE_LINEAR;
	else if (!strcmp(attr, "paced"))
		*c = ESVG_CALC_MODE_PACED;
	else if (!strcmp(attr, "spline"))
		*c = ESVG_CALC_MODE_SPLINE;
	else
		ret = EINA_FALSE;
	return ret;
}

EAPI Eina_Bool egueb_svg_animate_key_spline_string_from(Egueb_Svg_Animate_Key_Spline *spline, const char *s)
{
	return EINA_TRUE;
}

EAPI void egueb_svg_animation_event_offset_set(Egueb_Svg_Animation_Event *a, int64_t offset)
{

}
