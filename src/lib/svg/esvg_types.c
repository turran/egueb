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
#include "esvg_private_main.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_types.h"

#include "esvg_types.h"

#include <stdlib.h>
#include <errno.h>
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

#define ESVG_LOG_DEFAULT _esvg_type_log

static int _esvg_type_log = -1;

/*----------------------------------------------------------------------------*
 *                               Generic helpers                              *
 *----------------------------------------------------------------------------*/
/* parse a string of type funcname(a, b, .... numelements)
 * in, out numelements
 * in attr_val
 * out endptr
 * out elements
 */
static Eina_Bool _esvg_function_get(const char *attr_val, const char **endptr,
		const char *funcname, int *numelements, double *elements)
{
	int nvalues = 0;
	const char *tmp = attr_val;
	char *end;
	size_t sz;

	sz = strlen(funcname);
	ESVG_SPACE_SKIP(tmp);
	if (strncmp(tmp, funcname, sz) != 0)
		return EINA_FALSE;
	tmp += sz;
	ESVG_SPACE_SKIP(tmp);
	if (tmp[0] != '(')
		return EINA_FALSE;
	tmp++;
	while (*tmp)
	{
		double val;

		ESVG_SPACE_SKIP(tmp);
		if (tmp[0] == ')')
			goto end;
		val = eina_strtod(tmp, &end);
		if (errno == ERANGE)
			val = 0;
		if (end == tmp)
			break;
		tmp = end;
		elements[nvalues] = val;
		nvalues++;
		/* if we passed the limit, break */
		if (nvalues >= *numelements)
			break;
		/* skip the comma and the blanks */
		ESVG_SPACE_COMMA_SKIP(tmp);
	}

	if (tmp[0] != ')')
		return EINA_FALSE;
end:
	tmp++;
	*numelements = nvalues;
	*endptr = tmp;

	return EINA_TRUE;
}

static Eina_Bool _esvg_double_get(const char *iter, const char **tmp, double *v)
{
	char *endptr;
	double val;

	val = eina_strtod(iter, &endptr);
	if ((errno != ERANGE) &&
	    !((val == 0) && (iter == endptr)))
	{
		*tmp = endptr;
		*v = val;
		return EINA_TRUE;
	}
	return EINA_FALSE;
}

static const char * _fragment_get(const char *uri)
{
	const char *tmp;
	const char *id;

	/* TODO split the uri, for local or relative */
	/* TODO get the id (#ElementId) */
	/* only check on the ids */
	for (tmp = uri; *tmp && *tmp != '#'; tmp++)

	if (*tmp != '#') return NULL;
	id = tmp + 1;

	return id;
}

static inline Eina_Bool _is_a_z(char v)
{
	if (v >= 'a' && v <= 'z')
		return EINA_TRUE;
	return EINA_FALSE;
}

static inline Eina_Bool _is_A_Z(char v)
{
	if (v >= 'A' && v <= 'Z')
		return EINA_TRUE;
	return EINA_FALSE;
}

static inline Eina_Bool _is_Aa_Zz(char v)
{
	if (_is_A_Z(v) || _is_a_z(v))
		return EINA_TRUE;
	return EINA_FALSE;
}

static inline Eina_Bool _is_0_9(char v)
{
	if (v >= '0' && v <= '9')
		return EINA_TRUE;
	return EINA_FALSE;
}

static Eina_Bool _is_name_first(char v)
{
	/* FIXME here we should use the correct one from the xml doc:
	 * http://www.w3.org/TR/REC-xml/#NT-NameStartChar
	 */
	if (_is_Aa_Zz(v) || v == ':' || v == '_')
		return EINA_TRUE;
	return EINA_FALSE;
}

static Eina_Bool _is_name(char v)
{
	if (_is_name_first(v) || v == '-' || v == '.' || _is_0_9(v))
		return EINA_TRUE;
	return EINA_FALSE;
}

static Eina_Bool _animation_name_parse(const char *v, const char **start, int *len)
{
	int l = 0;
	const char *s;

	/* check the first letter */
	ESVG_SPACE_SKIP(v);

	s = v;
	if (!_is_name_first(*v))
		return EINA_FALSE;
	v++;
	
	/* then for each, iterate until we find the last valid char */
	while (_is_name(*v) && *v != '-' && *v != '.')
	{
		v++;
	}
	*start = s;
	*len = v - *start;
	return EINA_TRUE;
}

static Eina_Bool _animation_clock_parse(const char *v, int64_t *clock)
{
	Eina_Bool n = EINA_FALSE;
	if (*v == '+')
		v++;
	else if (*v == '-')
	{
		v++;
		n = EINA_TRUE;
	}
	if (!esvg_clock_string_from(clock, v))
		return EINA_FALSE;

	if (n)
		*clock = -(*clock);
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                     Transformation related functions                       *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_transformation_matrix_get(Enesim_Matrix *matrix, const char *attr_val, const char **endptr)
{
	int numelements = 6;
	double mx[6];

	if (!_esvg_function_get(attr_val, endptr, "matrix", &numelements, mx))
		return EINA_FALSE;
	if (numelements < 6)
		return EINA_FALSE;
	enesim_matrix_values_set(matrix, mx[0], mx[2], mx[4], mx[1], mx[3], mx[5], 0, 0, 1);

	return EINA_TRUE;
}

static Eina_Bool _esvg_transformation_translate_get(Enesim_Matrix *matrix, const char *attr_val, const char **endptr)
{
	int numelements = 2;
	double tx[2];

	if (!_esvg_function_get(attr_val, endptr, "translate", &numelements, tx))
		return EINA_FALSE;
	if (numelements < 1)
		return EINA_FALSE;
	/* on translation the tx is mandatory but not ty */
	if (numelements == 1)
		tx[1] = 0;
	enesim_matrix_translate(matrix, tx[0], tx[1]);

	return EINA_TRUE;
}

static Eina_Bool _esvg_transformation_skewx_get(Enesim_Matrix *matrix, const char *attr_val, const char **endptr)
{
	double angle;
	int numelements = 1;

	if (!_esvg_function_get(attr_val, endptr, "skewX", &numelements, &angle))
		return EINA_FALSE;
	if (numelements < 1)
		return EINA_FALSE;

	enesim_matrix_values_set(matrix, 1, tan(angle * M_PI / 180.0), 0, 0, 1, 0, 0, 0, 1);
	return EINA_TRUE;
}

static Eina_Bool _esvg_transformation_skewy_get(Enesim_Matrix *matrix, const char *attr_val, const char **endptr)
{
	double angle;
	int numelements = 1;

	if (!_esvg_function_get(attr_val, endptr, "skewY", &numelements, &angle))
		return EINA_FALSE;
	if (numelements < 1)
		return EINA_FALSE;

	enesim_matrix_values_set(matrix, 1, 0, 0, tan(angle * M_PI / 180.0), 1, 0, 0, 0, 1);
	return EINA_TRUE;
}

static Eina_Bool _esvg_transformation_scale_get(Enesim_Matrix *matrix, const char *attr_val, const char **endptr)
{
	double sx[2];
	int numelements = 2;

	if (!_esvg_function_get(attr_val, endptr, "scale", &numelements, sx))
		return EINA_FALSE;
	if (numelements < 1)
		return EINA_FALSE;
	if (numelements == 1)
		sx[1] = sx[0];
	enesim_matrix_scale(matrix, sx[0], sx[1]);

	return EINA_TRUE;
}


static Eina_Bool _esvg_transformation_rotate_get(Enesim_Matrix *matrix, const char *attr_val, const char **endptr)
{
	double rx[3];
	int numelements = 3;

	if (!_esvg_function_get(attr_val, endptr, "rotate", &numelements, rx))
		return EINA_FALSE;
	if (numelements < 1)
		return EINA_FALSE;
	enesim_matrix_rotate(matrix, rx[0] * M_PI / 180.0);
	/* handle the origin */
	if (numelements > 1)
	{
		Enesim_Matrix tx;

		if (numelements < 3)
			return EINA_FALSE;
		enesim_matrix_translate(&tx, rx[1], rx[2]);
		enesim_matrix_compose(&tx, matrix, matrix);
		enesim_matrix_translate(&tx, -rx[1], -rx[2]);
		enesim_matrix_compose(matrix, &tx, matrix);
	}

	return EINA_TRUE;
}

/*----------------------------------------------------------------------------*
 *                         Timing related functions                           *
 *----------------------------------------------------------------------------*/
static void _esvg_animation_event_list_cb(const char *attr, void *data)
{
	Esvg_Animation_Event *ev;
	Eina_List **l = data;

	ev = calloc(1, sizeof(Esvg_Animation_Event));
	if (!esvg_animation_event_string_from(ev, attr))
	{
		free(ev);
		return;
	}
	*l = eina_list_append(*l, ev);

	return;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Esvg_Length ESVG_LENGTH_0 = { 0.0, ESVG_UNIT_LENGTH_PX };
Esvg_Length ESVG_LENGTH_1 = { 1.0, ESVG_UNIT_LENGTH_PX };
Esvg_Length ESVG_LENGTH_100_PERCENT = { 100.0, ESVG_UNIT_LENGTH_PERCENT };

Eina_Bool esvg_long_get(const char *iter, const char **tmp, long *l)
{
	char *endptr;
	long val;

	val = strtol(iter, &endptr, 10);
	if ((errno != ERANGE) &&
	    !((val == 0) && (iter == endptr)))
	{
		*tmp = endptr;
		*l = val;
		return EINA_TRUE;
	}

	return EINA_FALSE;
}


void esvg_type_init(void)
{
	_esvg_type_log = eina_log_domain_register("esvg_type", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_type_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
}

void esvg_type_shutdown(void)
{
	if (_esvg_type_log < 0)
		return;
	eina_log_domain_unregister(_esvg_type_log);
	_esvg_type_log = -1;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/*
 * [ <absoluteURI> | <relativeURI> ] [ "#" <elementID> ]
 */
EAPI Eina_Bool esvg_uri_string_from(const char *attr, Esvg_Uri_Descriptor *descriptor, void *data)
{
	const char *fragment;

	ESVG_SPACE_SKIP(attr);
	fragment = _fragment_get(attr);
	/* TODO check for the local/non-local case */
	if (*attr == '#')
	{
		if (!fragment) return EINA_FALSE;
		DBG("getting %s", fragment);
		descriptor->local_get(NULL, fragment, data);
	}
	else
	{
		DBG("getting %s %s", attr, fragment);
		/* TODO very naive way of handling relative/absolute, we need the scheme too, etc */
		/* check for the relative/absolute case */
		if (*attr == '/')
			descriptor->absolute_get(attr, fragment, data);
		else
			descriptor->relative_get(attr, fragment, data);
	}
	return EINA_TRUE;
}

EAPI Eina_Bool esvg_iri_string_from(const char *attr, Esvg_Uri_Descriptor *descriptor, void *data)
{
	/* funciri ? */
	ESVG_SPACE_SKIP(attr);
	if (!strncmp(attr, "url(", 4))
	{
		char url[PATH_MAX];
		int i = 0;

		attr += 4;
		/* also remove the last spaces */
		while (*attr != ')' && *attr != '\0')
			url[i++] = *attr++;
		url[i] = '\0';
		return esvg_uri_string_from(url, descriptor, data);

	}
	/* iri */
	else
	{
		return esvg_uri_string_from(attr, descriptor, data);
	}
}

EAPI Eina_Bool esvg_clip_path_is_equal(const Esvg_Clip_Path *p1,
		const Esvg_Clip_Path *p2)
{
	/* sanity checks */
	if (p1 == p2) return EINA_TRUE;
	if (!p1) return EINA_FALSE;
	if (!p2) return EINA_FALSE;

	if (p1->type != p2->type) return EINA_FALSE;
	/* ok, we have values and same types, now compare each type */
	switch (p1->type)
	{
		case ESVG_CLIP_PATH_NONE:
		return EINA_TRUE;

		case ESVG_CLIP_PATH_IRI:
		return esvg_string_is_equal(p1->value.iri, p2->value.iri);

		/* FIXME what to do in this cases? add an assert? */
		default:
		return EINA_FALSE;
	}
}

EAPI const char * esvg_type_string_to(Esvg_Type type)
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

EAPI Eina_Bool esvg_type_is_animation(Esvg_Type type)
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

EAPI Eina_Bool esvg_type_is_paint_server(Esvg_Type type)
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

EAPI Eina_Bool esvg_type_is_shape(Esvg_Type type)
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

EAPI Eina_Bool esvg_type_is_renderable(Esvg_Type type)
{
	if (esvg_type_is_shape(type))
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


EAPI Eina_Bool esvg_parser_gradient_units_string_from(Esvg_Element_Gradient_Units *gu, const char *attr)
{
	if (strncmp(attr, "userSpaceOnUse", 14) == 0)
	{
		*gu = ESVG_USER_SPACE_ON_USE;
	}
	else if (strncmp(attr, "objectBoundingBox", 17) == 0)
	{
		*gu = ESVG_OBJECT_BOUNDING_BOX;
	}
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

EAPI Eina_Bool esvg_parser_fill_rule_get(Esvg_Fill_Rule *rule, const char *attr)
{
	if (strncmp(attr, "nonzero", 7) == 0)
	{
		*rule = ESVG_NON_ZERO;
	}
	else if (strncmp(attr, "evenodd", 7) == 0)
	{
		*rule = ESVG_EVEN_ODD;
	}
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

EAPI Eina_Bool esvg_parser_spread_method_get(Esvg_Spread_Method *smethod, const char *attr)
{
	if (strncmp(attr, "pad", 3) == 0)
	{
		*smethod = ESVG_SPREAD_METHOD_PAD;
	}
	else if (strncmp(attr, "reflect", 7) == 0)
	{
		*smethod = ESVG_SPREAD_METHOD_REFLECT;
	}
	else if (strncmp(attr, "repeat", 6) == 0)
	{
		*smethod = ESVG_SPREAD_METHOD_REPEAT;
	}
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

EAPI Eina_Bool esvg_transformation_string_from(Enesim_Matrix *matrix, const char *attr)
{
	Eina_Bool ret;
	const char *endptr = NULL;
	typedef Eina_Bool (*Matrix_Get)(Enesim_Matrix *matrix, const char *attr_val, const char **endptr);

	Matrix_Get m[6] = {
		_esvg_transformation_matrix_get,
		_esvg_transformation_translate_get,
		_esvg_transformation_rotate_get,
		_esvg_transformation_scale_get,
		_esvg_transformation_skewx_get,
		_esvg_transformation_skewy_get,
	};
	enesim_matrix_identity(matrix);
	do
	{
		Enesim_Matrix parsed;
		int i;

		enesim_matrix_identity(&parsed);
		for (i = 0; i < 6; i++)
		{
			ret = m[i](&parsed, attr, &endptr);
			if (ret) break;
		}
		if (ret)
		{
			DBG("found, composing");
			enesim_matrix_compose(matrix, &parsed, matrix);
		}
		attr = endptr;
	}
	while (endptr && *endptr && ret);

	return ret;
}

EAPI Eina_Bool esvg_string_is_equal(const char *s1, const char *s2)
{
	/* sanity checks */
	if (s1 == s2) return EINA_TRUE;
	if (!s1) return EINA_FALSE;
	if (!s2) return EINA_FALSE;

	if (strcmp(s1, s2)) return EINA_FALSE;
	return EINA_TRUE;
}


EAPI Esvg_Stroke_Line_Cap esvg_stroke_line_cap_string_from(const char *value)
{
	Esvg_Stroke_Line_Cap stroke_line_cap = ESVG_STROKE_LINE_CAP_BUTT;

	if (!strcmp(value, "butt"))
		stroke_line_cap = ESVG_STROKE_LINE_CAP_BUTT;
	else if (!strcmp(value, "round"))
		stroke_line_cap = ESVG_STROKE_LINE_CAP_ROUND;
	else if (!strcmp(value, "square"))
		stroke_line_cap = ESVG_STROKE_LINE_CAP_SQUARE;

	return stroke_line_cap;
}

EAPI Esvg_Stroke_Line_Join esvg_stroke_line_join_string_from(const char *value)
{
	Esvg_Stroke_Line_Join stroke_line_join = ESVG_STROKE_LINE_JOIN_MITER;

	if (!strcmp(value, "miter"))
		stroke_line_join = ESVG_STROKE_LINE_JOIN_MITER;
	else if (!strcmp(value, "round"))
		stroke_line_join = ESVG_STROKE_LINE_JOIN_ROUND;
	else if (!strcmp(value, "bevel"))
		stroke_line_join = ESVG_STROKE_LINE_JOIN_BEVEL;

	return stroke_line_join;
}

EAPI Eina_Bool esvg_attribute_type_string_from(Esvg_Attribute_Type *type, const char *value)
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
EAPI void esvg_points_string_from(const char *value, Esvg_Points_Cb cb, void *data)
{
	const char *tmp;
	char *endptr;

	ESVG_SPACE_SKIP(value);
	tmp = value;
	while (*tmp)
	{
		Esvg_Point p;

		p.x = eina_strtod(tmp, &endptr);
		tmp = endptr;
		ESVG_SPACE_COMMA_SKIP(tmp);
		p.y = eina_strtod(tmp, &endptr);
		tmp = endptr;
		ESVG_SPACE_COMMA_SKIP(tmp);

		cb(&p, data);
	}
}


EAPI void esvg_timing_string_from(const char *attr, Esvg_Timing_Cb cb, void *data)
{

}

EAPI Eina_Bool esvg_animate_transform_type_string_from(Esvg_Animate_Transform_Type *type, const char *s)
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

EAPI Eina_Bool esvg_list_string_from(const char *attr, char sep, Esvg_List_Cb cb, void *data)
{
	char *found;

	if (!attr) return EINA_FALSE;
	if (!cb) return EINA_FALSE;

	ESVG_SPACE_SKIP(attr);
	while ((found = strchr(attr, sep)))
	{
		*found = '\0';
		ESVG_SPACE_SKIP(attr);
		cb(attr, data);
		*found = sep;
		attr = found + 1;
		ESVG_SPACE_SKIP(attr);
	}
	if (attr)
		cb(attr, data);

	return EINA_TRUE;
}

EAPI Eina_Bool esvg_number_list_string_from(const char *attr, Esvg_Number_List_Cb cb, void *data)
{
	const char *tmp = attr;
	char *end;

	if (!attr) return EINA_FALSE;
	if (!cb) return EINA_FALSE;

	ESVG_SPACE_SKIP(tmp);
	while (tmp)
	{
		double val;

		ESVG_SPACE_SKIP(tmp);
		if (!*tmp)
			break;

		val = eina_strtod(tmp, &end);
		if (errno == ERANGE)
			val = 0;
		if (end == tmp)
			break;
		tmp = end;
		if (!cb(val, data))
			break;
		/* skip the comma and the blanks */
		ESVG_SPACE_COMMA_SKIP(tmp);
	}
	return EINA_TRUE;
}

/* The clock is defined in miliseconds? nanoseconds? */
/* TODO maybe we should use doubles directly? */
/* Clock-val         ::= Full-clock-val | Partial-clock-val | Timecount-val */
EAPI Eina_Bool esvg_clock_string_from(int64_t *clock, const char *attr)
{
	Eina_Bool ret = EINA_FALSE;
	long v;
	const char *tmp;

	DBG("parsing the duration %s", attr);
	if (!esvg_long_get(attr, &tmp, &v))
		return EINA_FALSE;

	if (*tmp == ':')
	{
		long v2;

		tmp++;
		if (!esvg_long_get(tmp, &tmp, &v2))
			return EINA_FALSE;

		/* Full-clock-val::= Hours ":" Minutes ":" Seconds ("." Fraction)? */
		if (*tmp == ':')
		{

		}
		/* Partial-clock-val::= Minutes ":" Seconds ("." Fraction)? */
		else
		{
			long m = v;
			long s = v2;

			*clock = (m * 60) + (s);
			/* Fraction::= DIGIT+ */
			if (*tmp == '.')
			{
				long f;

				tmp++;
				if (!esvg_long_get(tmp, &tmp, &f))
					return EINA_FALSE;
			}
			ret = EINA_TRUE;

		}
	}
	/* Timecount-val::= Timecount ("." Fraction)? (Metric)? */
	else
	{
		int64_t scale;
		long f = 0;

		/* Fraction::= DIGIT+ */
		if (*tmp == '.')
		{
			tmp++;
			if (!esvg_long_get(tmp, &tmp, &f))
				return EINA_FALSE;
		}

		/* Metric::= "h" | "min" | "s" | "ms" */
		if (!strncmp(tmp, "ms", 2))
			scale = ESVG_CLOCK_MSECONDS;
		else if (*tmp == 's')
			scale = ESVG_CLOCK_SECONDS;
		else if (*tmp == 'm')
			scale = ESVG_CLOCK_SECONDS * 60;
		else if (*tmp == 'h')
			scale = ESVG_CLOCK_SECONDS * 60 * 60;
		else if (!*tmp)
			scale = ESVG_CLOCK_SECONDS;
		else
			return EINA_FALSE;

		*clock = v * scale;
		if (f)
			*clock += (double)(f / 10.0) * scale;

		DBG("clock %" ETCH_TIME_FORMAT " parsed from attr '%s'", ETCH_TIME_ARGS(*clock), attr);
		ret = EINA_TRUE;
	}

	return ret;
}

EAPI Eina_Bool esvg_calc_mode_string_from(Esvg_Calc_Mode *c, const char *attr)
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

EAPI Eina_Bool esvg_additive_string_from(Esvg_Additive *add, const char *attr)
{
	Eina_Bool ret = EINA_TRUE;

	if (!strcmp(attr, "sum"))
		*add = ESVG_ADDITIVE_SUM;
	else if (!strcmp(attr, "replace"))
		*add = ESVG_ADDITIVE_REPLACE;
	else
		ret = EINA_FALSE;
	return ret;
}

EAPI Eina_Bool esvg_accumulate_string_from(Esvg_Accumulate *acc, const char *attr)
{
	Eina_Bool ret = EINA_TRUE;

	if (!strcmp(attr, "sum"))
		*acc = ESVG_ACCUMULATE_SUM;
	else if (!strcmp(attr, "none"))
		*acc = ESVG_ACCUMULATE_NONE;
	else
		ret = EINA_FALSE;
	return ret;
}

EAPI Eina_Bool esvg_fill_string_from(Esvg_Fill *fill, const char *attr)
{
	Eina_Bool ret = EINA_TRUE;

	if (!strcmp(attr, "freeze"))
		*fill = ESVG_FILL_FREEZE;
	else if (!strcmp(attr, "remove"))
		*fill = ESVG_FILL_REMOVE;
	else
		ret = EINA_FALSE;
	return ret;
}


EAPI Eina_Bool esvg_repeat_count_string_from(int *rc, const char *attr)
{
	if (!strcmp(attr, "indefinite"))
		*rc = -1;
	else
		*rc = atoi(attr);
	return EINA_TRUE;
}

EAPI Eina_Bool esvg_animate_key_spline_string_from(Esvg_Animate_Key_Spline *spline, const char *s)
{
	return EINA_TRUE;
}

EAPI Eina_Bool esvg_animation_event_list_string_from(Eina_List **l, const char *s)
{
	return esvg_list_string_from(s, ';', _esvg_animation_event_list_cb, l);
}

EAPI Eina_Bool esvg_animation_event_string_from(Esvg_Animation_Event *e, const char *s)
{
	Eina_Bool ret;
	const char *start;
	const char *end;
	int len;

	/* just in case */
	e->id = NULL;
	e->event = NULL;

	ret = _animation_name_parse(s, &start, &len);
	end = start + len;
	/* no name, just numbers ... clock */
	if (!ret)
	{
		/* clock */
		if (!_animation_clock_parse(s, &e->offset))
			return EINA_FALSE;
	}
	else
	{
		/* simplest cases */
		if (!strcmp(s, "indefinite"))
		{
			/* TODO */
		}
		else if (!strncmp(s, "accessKey", 9))
		{
			/* TODO */
		}
		else if (!strncmp(s, "wallclock", 9))
		{
			/* TODO */
		}
		else
		{
			/* id */
			if (*end == '.')
			{
				e->id = strndup(start, len);
				/* advance after the point */
				ret = _animation_name_parse(end + 1, &start, &len);
				end = start + len;
				if (!ret) goto err;
			}
			/* repeat */
			if (!strncmp(s, "repeat", len))
			{
				long r;
				if (*end == '(')
					start++;
				if (!esvg_long_get(start, &end, &r))
					goto err;
				if (*end != ')')
					goto err;
				e->repeat = r;
			}
			/* event name */
			else
			{
				e->event = strndup(start, len);
			}
			/* offset */
			if (*end)
				_animation_clock_parse(end + 1, &e->offset);
		}
	}
	return EINA_TRUE;
err:
	if (e->id)
		free(e->id);
	if (e->event)
		free(e->event);
	return EINA_FALSE;
}

EAPI void esvg_animation_event_offset_set(Esvg_Animation_Event *a, int64_t offset)
{

}

EAPI Eina_Bool esvg_visibility_string_from(Esvg_Visibility *v, const char *attr)
{
	if (!strcmp(attr, "visible"))
		*v = ESVG_VISIBILITY_VISIBLE;
	else if (!strcmp(attr, "hidden"))
		*v = ESVG_VISIBILITY_HIDDEN;
	else if (!strcmp(attr, "collapse"))
		*v = ESVG_VISIBILITY_COLLAPSE;
	else
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI Eina_Bool esvg_display_string_from(Esvg_Display *d, const char *attr)
{
	if (!strcmp(attr, "inline"))
		*d = ESVG_DISPLAY_INLINE;
	else if (!strcmp(attr, "block"))
		*d = ESVG_DISPLAY_BLOCK;
	else if (!strcmp(attr, "list-item"))
		*d = ESVG_DISPLAY_LIST_ITEM;
	else if (!strcmp(attr, "run-in"))
		*d = ESVG_DISPLAY_RUN_IN;
	else if (!strcmp(attr, "compact"))
		*d = ESVG_DISPLAY_COMPACT;
	else if (!strcmp(attr, "marker"))
		*d = ESVG_DISPLAY_MARKER;
	else if (!strcmp(attr, "table"))
		*d = ESVG_DISPLAY_TABLE;
	else if (!strcmp(attr, "inline-table"))
		*d = ESVG_DISPLAY_INLINE_TABLE;
	else if (!strcmp(attr, "table-row-group"))
		*d = ESVG_DISPLAY_TABLE_ROW_GROUP;
	else if (!strcmp(attr, "table-header-group"))
		*d = ESVG_DISPLAY_TABLE_HEADER_GROUP;
	else if (!strcmp(attr, "table-footer-group"))
		*d = ESVG_DISPLAY_TABLE_FOOTER_GROUP;
	else if (!strcmp(attr, "table-row"))
		*d = ESVG_DISPLAY_TABLE_ROW;
	else if (!strcmp(attr, "table-column-group"))
		*d = ESVG_DISPLAY_TABLE_COLUMN_GROUP;
	else if (!strcmp(attr, "table-column"))
		*d = ESVG_DISPLAY_TABLE_COLUMN;
	else if (!strcmp(attr, "table-cell"))
		*d = ESVG_DISPLAY_TABLE_CELL;
	else if (!strcmp(attr, "table-caption"))
		*d = ESVG_DISPLAY_TABLE_CAPTION;
	else if (!strcmp(attr, "none"))
		*d = ESVG_DISPLAY_NONE;
	else
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI Eina_Bool esvg_clip_path_string_from(Esvg_Clip_Path *cp, const char *attr)
{
	if (!strcmp(attr, "none"))
	{
		cp->type = ESVG_CLIP_PATH_NONE;
		cp->value.iri = NULL;
	}
	else
	{
		cp->type = ESVG_CLIP_PATH_IRI;
		cp->value.iri = strdup(attr);
	}
	return EINA_TRUE;
}

