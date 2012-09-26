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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <errno.h>

#include "esvg_private_main.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_types.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

#if 1
# define eina_strtod strtod
#else
static double
eina_strtod(const char *nptr, char **endptr)
{
  char *str;
  int mantisse = 0;
  int dec = 1;
  int has_sign = 0;
  int has_exp_sign = 0;
  int dotted = 0;
  int has_exponent = 0;
  double res;
  int val = 1;

  str = (char *)nptr;

  if (!str || !*str)
    {
      if (endptr) *endptr = str;
      return 0.0;
    }

  if (*str == '-')
    {
      has_sign = 1;
      str++;
    }
  else if (*str == '+')
    str++;

  while (*str)
    {
      if ((*str >= '0') && (*str <= '9'))
	{
	  mantisse *= 10;
	  mantisse += (*str - '0');
	  if (dotted) dec *= 10;
	}
      else if (*str == '.')
	{
	  if (dotted)
	    {
	      if (endptr) *endptr = str;
	      return 0.0;
	    }
	  dotted = 1;
	}
      else if ((*str == 'e') || (*str == 'E'))
	{
	  str++;
	  has_exponent = 1;
	  break;
	}
      else
	break;

      str++;
    }

  if (*str && has_exponent)
    {
      int exponent = 0;
      int i;

      has_exponent = 0;
      if (*str == '+') str++;
      else if (*str == '-')
	{
	  has_exp_sign = 1;
	  str++;
	}
      while (*str)
	{
	  if ((*str >= '0') && (*str <= '9'))
	    {
	      has_exponent = 1;
	      exponent *= 10;
	      exponent += (*str - '0');
	    }
	  else
	    break;
	  str++;
	}

      if (has_exponent)
	{
	  for (i = 0; i < exponent; i++)
	    val *= 10;
	}
    }

  if (endptr) *endptr = str;

  if (has_sign)
    res = -(double)mantisse / (double)dec;
  else
    res = (double)mantisse / (double)dec;

  if (val != 1)
    {
      if (has_exp_sign)
	res /= val;
      else
	res *= val;
    }

  return res;
}
#endif

#define ESVG_LOG_DEFAULT esvg_log_type

#define ESVG_IS_HEXA(c) \
	(((c >= 48) && (c <= 57)) || \
	((c >= 65) && (c <= 70)) || \
	((c >= 97) && (c <= 102)))

#define ESVG_SPACE_SKIP(t) \
	while (*t) { if ((*t == ' ') || (*t == '\n') || (*t == '\t') || (*t == '\r')) t++; else break; }

#define ESVG_SPACE_COMMA_SKIP(t) \
	ESVG_SPACE_SKIP(t); \
	if (*t == ',') t++; \
	ESVG_SPACE_SKIP(t);

#define ESVG_CLOCK_SECONDS (1000000000LL)
#define ESVG_CLOCK_MSECONDS (1000000LL)

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

static Eina_Bool _esvg_long_get(const char *iter, const char **tmp, long *l)
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

static unsigned char _esvg_c_to_h(char c)
{
	unsigned char h;

	if ((c >= 48) && (c <= 57))
		h = c - 48;
	else if ((c >= 65) && (c <= 70))
		h = c - 55;
	else
		h = c - 87;
	return h;
}

static unsigned char _esvg_c_to_hh(char c)
{
	unsigned char h = _esvg_c_to_h(c);

	return h | ((h << 4) & 240);
}

static unsigned char _esvg_cc_to_hh(const char *cc)
{
	unsigned char h1;
	unsigned char h2;

	h1 = _esvg_c_to_h(cc[0]);
	h2 = _esvg_c_to_h(cc[1]);
	return h2 | ((h1 << 4) & 240);
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

static Eina_Bool _parse_name(const char *v, const char **start, const char **end)
{
	/* check the first letter */
	ESVG_SPACE_SKIP(v);
	if (!_is_name_first(*v))
		return NULL;
	v++;
	/* then for each, iterate until we find the last valid char */
}
/*----------------------------------------------------------------------------*
 *                           Color related functions                          *
 *----------------------------------------------------------------------------*/
Eina_Bool _esvg_color_keyword_from(Esvg_Color *color, const char *attr_val)
{
	static Eina_Hash *_colors = NULL;
	Enesim_Argb *argb;
	typedef struct _Esvg_Color_Entry
	{
		const char *name;
		Enesim_Argb color;
	} Esvg_Color_Entry;

	if (!_colors)
	{
		/* Css color table */
		Esvg_Color_Entry colors[] = {
			{ "aliceblue", 0xfff0f8ff },
			{ "antiquewhite", 0xfffaebd7 },
			{ "aqua", 0xff00ffff },
			{ "aquamarine", 0xff7fffd4 },
			{ "azure", 0xfff0ffff },
			{ "beige", 0xfff5f5dc },
			{ "bisque", 0xffffe4c4 },
			{ "black", 0xff000000 },
			{ "blanchedalmond", 0xffffebcd },
			{ "blue", 0xff0000ff },
			{ "blueviolet", 0xff8a2be2 },
			{ "brown", 0xffa52a2a },
			{ "burlywood", 0xffdeb887 },
			{ "cadetblue", 0xff5f9ea0 },
			{ "chartreuse", 0xff7fff00 },
			{ "chocolate", 0xffd2691e },
			{ "coral", 0xffff7f50 },
			{ "cornflowerblue", 0xff6495ed },
			{ "cornsilk", 0xfffff8dc },
			{ "crimson", 0xffdc143c },
			{ "cyan", 0xff00ffff },
			{ "darkblue", 0xff00008b },
			{ "darkcyan", 0xff008b8b },
			{ "darkgoldenrod", 0xffb8860b },
			{ "darkgray", 0xffa9a9a9 },
			{ "darkgrey", 0xffa9a9a9 },
			{ "darkgreen", 0xff006400 },
			{ "darkkhaki", 0xffbdb76b },
			{ "darkmagenta", 0xff8b008b },
			{ "darkolivegreen", 0xff556b2f },
			{ "darkorange", 0xffff8c00 },
			{ "darkorchid", 0xff9932cc },
			{ "darkred", 0xff8b0000 },
			{ "darksalmon", 0xffe9967a },
			{ "darkseagreen", 0xff8fbc8f },
			{ "darkslateblue", 0xff483d8b },
			{ "darkslategray", 0xff2f4f4f },
			{ "darkslategrey", 0xff2f4f4f },
			{ "darkturquoise", 0xff00ced1 },
			{ "darkviolet", 0xff9400d3 },
			{ "deeppink", 0xffff1493 },
			{ "deepskyblue", 0xff00bfff },
			{ "dimgray", 0xff696969 },
			{ "dimgrey", 0xff696969 },
			{ "dodgerblue", 0xff1e90ff },
			{ "firebrick", 0xffb22222 },
			{ "floralwhite", 0xfffffaf0 },
			{ "forestgreen", 0xff228b22 },
			{ "fuchsia", 0xffff00ff },
			{ "gainsboro", 0xffdcdcdc },
			{ "ghostwhite", 0xfff8f8ff },
			{ "gold", 0xffffd700 },
			{ "goldenrod", 0xffdaa520 },
			{ "gray", 0xff808080 },
			{ "grey", 0xff808080 },
			{ "green", 0xff008000 },
			{ "greenyellow", 0xffadff2f },
			{ "honeydew", 0xfff0fff0 },
			{ "hotpink", 0xffff69b4 },
			{ "indianred", 0xffcd5c5c },
			{ "indigo", 0xff4b0082 },
			{ "ivory", 0xfffffff0 },
			{ "khaki", 0xfff0e68c },
			{ "lavender", 0xffe6e6fa },
			{ "lavenderblush", 0xfffff0f5 },
			{ "lawngreen", 0xff7cfc00 },
			{ "lemonchiffon", 0xfffffacd },
			{ "lightblue", 0xffadd8e6 },
			{ "lightcoral", 0xfff08080 },
			{ "lightcyan", 0xffe0ffff },
			{ "lightgoldenrodyellow", 0xfffafad2 },
			{ "lightgray", 0xffd3d3d3 },
			{ "lightgrey", 0xffd3d3d3 },
			{ "lightgreen", 0xff90ee90 },
			{ "lightpink", 0xffffb6c1 },
			{ "lightsalmon", 0xffffa07a },
			{ "lightseagreen", 0xff20b2aa },
			{ "lightskyblue", 0xff87cefa },
			{ "lightslategray", 0xff778899 },
			{ "lightslategrey", 0xff778899 },
			{ "lightsteelblue", 0xffb0c4de },
			{ "lightyellow", 0xffffffe0 },
			{ "lime", 0xff00ff00 },
			{ "limegreen", 0xff32cd32 },
			{ "linen", 0xfffaf0e6 },
			{ "magenta", 0xffff00ff },
			{ "maroon", 0xff800000 },
			{ "mediumaquamarine", 0xff66cdaa },
			{ "mediumblue", 0xff0000cd },
			{ "mediumorchid", 0xffba55d3 },
			{ "mediumpurple", 0xff9370d8 },
			{ "mediumseagreen", 0xff3cb371 },
			{ "mediumslateblue", 0xff7b68ee },
			{ "mediumspringgreen", 0xff00fa9a },
			{ "mediumturquoise", 0xff48d1cc },
			{ "mediumvioletred", 0xffc71585 },
			{ "midnightblue", 0xff191970 },
			{ "mintcream", 0xfff5fffa },
			{ "mistyrose", 0xffffe4e1 },
			{ "moccasin", 0xffffe4b5 },
			{ "navajowhite", 0xffffdead },
			{ "navy", 0xff000080 },
			{ "oldlace", 0xfffdf5e6 },
			{ "olive", 0xff808000 },
			{ "olivedrab", 0xff6b8e23 },
			{ "orange", 0xffffa500 },
			{ "orangered", 0xffff4500 },
			{ "orchid", 0xffda70d6 },
			{ "palegoldenrod", 0xffeee8aa },
			{ "palegreen", 0xff98fb98 },
			{ "paleturquoise", 0xffafeeee },
			{ "palevioletred", 0xffd87093 },
			{ "papayawhip", 0xffffefd5 },
			{ "peachpuff", 0xffffdab9 },
			{ "peru", 0xffcd853f },
			{ "pink", 0xffffc0cb },
			{ "plum", 0xffdda0dd },
			{ "powderblue", 0xffb0e0e6 },
			{ "purple", 0xff800080 },
			{ "red", 0xffff0000 },
			{ "rosybrown", 0xffbc8f8f },
			{ "royalblue", 0xff4169e1 },
			{ "saddlebrown", 0xff8b4513 },
			{ "salmon", 0xfffa8072 },
			{ "sandybrown", 0xfff4a460 },
			{ "seagreen", 0xff2e8b57 },
			{ "seashell", 0xfffff5ee },
			{ "sienna", 0xffa0522d },
			{ "silver", 0xffc0c0c0 },
			{ "skyblue", 0xff87ceeb },
			{ "slateblue", 0xff6a5acd },
			{ "slategray", 0xff708090 },
			{ "slategrey", 0xff708090 },
			{ "snow", 0xfffffafa },
			{ "springgreen", 0xff00ff7f },
			{ "steelblue", 0xff4682b4 },
			{ "tan", 0xffd2b48c },
			{ "teal", 0xff008080 },
			{ "thistle", 0xffd8bfd8 },
			{ "tomato", 0xffff6347 },
			{ "turquoise", 0xff40e0d0 },
			{ "violet", 0xffee82ee },
			{ "wheat", 0xfff5deb3 },
			{ "white", 0xffffffff },
			{ "whitesmoke", 0xfff5f5f5 },
			{ "yellow", 0xffffff00 },
			{ "yellowgreen", 0xff9acd32 }
		};
		unsigned int i;

		_colors = eina_hash_string_superfast_new(NULL);
		for (i = 0; i < sizeof(colors) / sizeof(Esvg_Color_Entry); i++)
		{
			argb = malloc(sizeof(Enesim_Argb));
			*argb = colors[i].color;
			eina_hash_add(_colors, colors[i].name, argb);
		}
	}
	/* convert the attr to lowercase */
	argb = eina_hash_find(_colors, attr_val);

	if (!argb) return EINA_FALSE;

	color->r = (*argb >> 16) & 0xff;
	color->g = (*argb >> 8) & 0xff;
	color->b = (*argb & 0xff);

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
 *                           Path related functions                           *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_path_number_get(char **attr, double *x)
{
	char *iter;
	char *endptr;

	iter = *attr;
	ESVG_SPACE_COMMA_SKIP(iter);
	*x = eina_strtod(iter, &endptr);
	if (iter == endptr)
		return EINA_FALSE;

	*attr = endptr;

	return EINA_TRUE;
}

static Eina_Bool _esvg_path_flag_get(char **attr, Eina_Bool *b)
{
	char *iter;

	iter = *attr;
	ESVG_SPACE_COMMA_SKIP(iter);
	if (*iter == '0')
	{
		*b = EINA_FALSE;
	}
	else if (*iter == '1')
	{
		*b = EINA_TRUE;
	}
	else
	{
		ERR("can not convert flag");
		return EINA_FALSE;
	}
	*attr = iter + 1;
	return EINA_TRUE;
}

static Eina_Bool _esvg_path_point_get(char **attr, Esvg_Point *p)
{
	if (!_esvg_path_number_get(attr, &p->x))
	{
		ERR("can not convert number");
		return EINA_FALSE;
	}

	if (!_esvg_path_number_get(attr, &p->y))
	{
		ERR("can not convert number");
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static Eina_Bool esvg_parser_path_line_to(Eina_Bool relative,
		char **value, Esvg_Path_Command *cmd)
{
	Esvg_Point p;

	if (!_esvg_path_point_get(value, &p))
	{
		ERR("Can not get point");
		return EINA_FALSE;
	}

	cmd->type = ESVG_PATH_LINE_TO;
	cmd->relative = relative;
	cmd->data.line_to.x = p.x;
	cmd->data.line_to.y = p.y;

	return EINA_TRUE;
}

static Eina_Bool esvg_parser_path_move_to(Eina_Bool relative,
		char **value, Esvg_Path_Command *cmd)
{
	Esvg_Point p;

	if (!_esvg_path_point_get(value, &p))
	{
		ERR("Can not get point");
		return EINA_FALSE;
	}
	cmd->type = ESVG_PATH_MOVE_TO;
	cmd->relative = relative;
	cmd->data.move_to.x = p.x;
	cmd->data.move_to.y = p.y;

	return EINA_TRUE;
}

static Eina_Bool esvg_parser_path_hline_to(Eina_Bool relative,
		char **value, Esvg_Path_Command *cmd)
{
	double c;

	if (!_esvg_path_number_get(value, &c))
	{
		ERR("Can not get coord");
		return EINA_FALSE;
	}
	cmd->type = ESVG_PATH_HLINE_TO;
	cmd->relative = relative;
	cmd->data.hline_to.c = c;

	return EINA_TRUE;
}

static Eina_Bool esvg_parser_path_vline_to(Eina_Bool relative,
		char **value, Esvg_Path_Command *cmd)
{
	double c;

	if (!_esvg_path_number_get(value, &c))
	{
		ERR("Can not get coord");
		return EINA_FALSE;
	}
	cmd->type = ESVG_PATH_VLINE_TO;
	cmd->relative = relative;
	cmd->data.hline_to.c = c;

	return EINA_TRUE;
}

static Eina_Bool esvg_parser_path_cubic_to(Eina_Bool relative,
		char **value, Esvg_Path_Command *cmd)
{
	Esvg_Point ctrl0, ctrl1, p;

	if (!_esvg_path_point_get(value, &ctrl0))
	{
		ERR("Can not get control point %s", *value);
		return EINA_FALSE;
	}

	if (!_esvg_path_point_get(value, &ctrl1))
	{
		ERR("Can not get control point");
		return EINA_FALSE;
	}

	if (!_esvg_path_point_get(value, &p))
	{
		ERR("Can not get point");
		return EINA_FALSE;
	}

	cmd->type = ESVG_PATH_CUBIC_TO;
	cmd->relative = relative;
	cmd->data.cubic_to.ctrl_x0 = ctrl0.x;
	cmd->data.cubic_to.ctrl_y0 = ctrl0.y;
	cmd->data.cubic_to.ctrl_x1 = ctrl1.x;
	cmd->data.cubic_to.ctrl_y1 = ctrl1.y;
	cmd->data.cubic_to.x = p.x;
	cmd->data.cubic_to.y = p.y;

	return EINA_TRUE;
}

static Eina_Bool esvg_parser_path_scubic_to(Eina_Bool relative,
		char **value, Esvg_Path_Command *cmd)
{
	Esvg_Point ctrl, p;

	if (!_esvg_path_point_get(value, &ctrl))
	{
		ERR("Can not get control point");
		return EINA_FALSE;
	}

	if (!_esvg_path_point_get(value, &p))
	{
		ERR("Can not get point");
		return EINA_FALSE;
	}

	cmd->type = ESVG_PATH_SCUBIC_TO;
	cmd->relative = relative;
	cmd->data.scubic_to.ctrl_x = ctrl.x;
	cmd->data.scubic_to.ctrl_y = ctrl.y;
	cmd->data.scubic_to.x = p.x;
	cmd->data.scubic_to.y = p.y;

	return EINA_TRUE;
}

static Eina_Bool esvg_parser_path_quadratic_to(	Eina_Bool relative,
		char **value, Esvg_Path_Command *cmd)
{
	Esvg_Point ctrl, p;

	if (!_esvg_path_point_get(value, &ctrl))
	{
		ERR("Can not get control point");
		return EINA_FALSE;
	}

	if (!_esvg_path_point_get(value, &p))
	{
		ERR("Can not get point");
		return EINA_FALSE;
	}

	cmd->type = ESVG_PATH_QUADRATIC_TO;
	cmd->relative = relative;
	cmd->data.quadratic_to.ctrl_x = ctrl.x;
	cmd->data.quadratic_to.ctrl_y = ctrl.y;
	cmd->data.quadratic_to.x = p.x;
	cmd->data.quadratic_to.y = p.y;

	return EINA_TRUE;

}

static Eina_Bool esvg_parser_path_squadratic_to(Eina_Bool relative,
		char **value, Esvg_Path_Command *cmd)
{
	Esvg_Point p;

	if (!_esvg_path_point_get(value, &p))
	{
		ERR("Can not get point");
		return EINA_FALSE;
	}
	cmd->type = ESVG_PATH_SQUADRATIC_TO;
	cmd->relative = relative;
	cmd->data.squadratic_to.x = p.x;
	cmd->data.squadratic_to.y = p.y;

	return EINA_TRUE;
}

static Eina_Bool esvg_parser_path_arc_to(Eina_Bool relative,
		char **value, Esvg_Path_Command *cmd)
{
	Esvg_Point p, radii;
	Eina_Bool large, sweep;
	double angle;

	if (!_esvg_path_point_get(value, &radii))
	{
		ERR("can not get radii");
		return EINA_FALSE;
	}

	if (!_esvg_path_number_get(value, &angle))
	{
		ERR("can not convert number");
		return EINA_FALSE;
	}

	if (!_esvg_path_flag_get(value, &large))
	{
		ERR("can not convert the large flag");
		return EINA_FALSE;
	}

	if (!_esvg_path_flag_get(value, &sweep))
	{
		ERR("can not convert the sweep flag");
		return EINA_FALSE;
	}

	if (!_esvg_path_point_get(value, &p))
	{
		ERR("Can not get point");
		return EINA_FALSE;
	}

	cmd->type = ESVG_PATH_ARC_TO;
	cmd->relative = relative;
	cmd->data.arc_to.rx = radii.x;
	cmd->data.arc_to.ry = radii.y;
	cmd->data.arc_to.angle = angle;
	cmd->data.arc_to.large = large;
	cmd->data.arc_to.sweep = sweep;
	cmd->data.arc_to.x = p.x;
	cmd->data.arc_to.y = p.y;

	return EINA_TRUE;
}

static Eina_Bool esvg_parser_path_close(char **value,
		Esvg_Path_Command *cmd)
{
	cmd->type = ESVG_PATH_CLOSE;
	cmd->relative = EINA_FALSE;

	return EINA_TRUE;
}

static Eina_Bool esvg_parser_command(char command, char **value,
		Esvg_Path_Command *cmd)
{
	Eina_Bool ret = EINA_TRUE;

	switch (command)
	{
		case 'L':
		ret = esvg_parser_path_line_to(EINA_FALSE, value, cmd);
		break;

		case 'l':
		ret = esvg_parser_path_line_to(EINA_TRUE, value, cmd);
		break;

		case 'M':
		ret = esvg_parser_path_move_to(EINA_FALSE, value, cmd);
		break;

		case 'm':
		ret = esvg_parser_path_move_to(EINA_TRUE, value, cmd);
		break;

		case 'H':
		ret = esvg_parser_path_hline_to(EINA_FALSE, value, cmd);
		break;

		case 'h':
		ret = esvg_parser_path_hline_to(EINA_TRUE, value, cmd);
		break;

		case 'V':
		ret = esvg_parser_path_vline_to(EINA_FALSE, value, cmd);
		break;

		case 'v':
		ret = esvg_parser_path_vline_to(EINA_TRUE, value, cmd);
		break;

		case 'C':
		ret = esvg_parser_path_cubic_to(EINA_FALSE, value, cmd);
		break;

		case 'c':
		ret = esvg_parser_path_cubic_to(EINA_TRUE, value, cmd);
		break;

		case 'S':
		ret = esvg_parser_path_scubic_to(EINA_FALSE, value, cmd);
		break;

		case 's':
		ret = esvg_parser_path_scubic_to(EINA_TRUE, value, cmd);
		break;

		case 'Q':
		ret = esvg_parser_path_quadratic_to(EINA_FALSE, value, cmd);
		break;

		case 'q':
		ret = esvg_parser_path_quadratic_to(EINA_TRUE, value, cmd);
		break;

		case 'T':
		ret = esvg_parser_path_squadratic_to(EINA_FALSE, value, cmd);
		break;

		case 't':
		ret = esvg_parser_path_squadratic_to(EINA_TRUE, value, cmd);
		break;

		case 'A':
		ret = esvg_parser_path_arc_to(EINA_FALSE, value, cmd);
		break;

		case 'a':
		ret = esvg_parser_path_arc_to(EINA_TRUE, value, cmd);
		break;

		case 'z':
		case 'Z':
		ret = esvg_parser_path_close(value, cmd);
		break;

		default:
		ret = EINA_FALSE;
		break;
	}
	return ret;
}
/*----------------------------------------------------------------------------*
 *                         Timing related functions                           *
 *----------------------------------------------------------------------------*/


/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Esvg_Length ESVG_LENGTH_0 = { 0.0, ESVG_UNIT_LENGTH_PX };
Esvg_Length ESVG_LENGTH_1 = { 1.0, ESVG_UNIT_LENGTH_PX };
Esvg_Length ESVG_LENGTH_100_PERCENT = { 100.0, ESVG_UNIT_LENGTH_PERCENT };

/* FIXME: fix parsing with ' ' and ',' (do like rgb(c,c,c)) */
Esvg_View_Box esvg_view_box_get(const char *attr_val)
{
	Esvg_View_Box vb = { 0, 0, 0, 0 };
	const char *iter;
	const char *tmp;
	char *endptr;
	double val;
	double *vbp;
	int nbr = 0;

        iter = tmp = attr_val;
	vbp = (double *)&vb;
	while (*tmp)
	{
		while (*tmp)
		{
			if ((*tmp == ' ') || (*tmp == ','))
				tmp++;
			else
			{
				iter = tmp;
				break;
			}
		}
		val = eina_strtod(iter, &endptr);
		if ((errno != ERANGE) &&
		    !((val == 0) && (attr_val == endptr)))
		{
			*vbp = val;
			vbp++;
			tmp = endptr;
			nbr++;
			/* we store only the 4 first numbers */
			if (nbr >= 4)
			{
				return vb;
			}
		}
	}

	return vb;
}

void esvg_paint_copy(Esvg_Paint *dst, Esvg_Paint *src)
{
	if (dst->type == ESVG_PAINT_SERVER)
	{
		if (dst->value.paint_server)
			free(dst->value.paint_server);
	}
	*dst = *src;
	if (src->type == ESVG_PAINT_SERVER)
	{
		if (src->value.paint_server)
			dst->value.paint_server = strdup(src->value.paint_server);
	}
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI double esvg_number_string_from(const char *attr_val, double default_nbr)
{
	char *endptr;
	double val = default_nbr;

	if (!attr_val || !*attr_val)
		return val;

	val = eina_strtod(attr_val, &endptr);
	if (errno == ERANGE)
		return val;
	if ((val == 0) && (attr_val == endptr))
		return val;

	/* else, conversion has been done */
	if ((endptr == NULL) || (*endptr == '\0'))
	{
		return val;
	}

	ERR("Number %s is invalid", attr_val);
	return default_nbr;
}

EAPI Eina_Bool esvg_length_string_from(Esvg_Length *length, const char *attr_val)
{
	char *endptr;
	double val;

	if (!attr_val || !*attr_val)
		return EINA_FALSE;

	val = eina_strtod(attr_val, &endptr);
	if (errno == ERANGE)
		return EINA_FALSE;
	if ((val == 0) && (attr_val == endptr))
		return EINA_FALSE;

	/* else, conversion has been done */
	if ((endptr == NULL) || (*endptr == '\0'))
	{
		length->value = val;
		length->unit = ESVG_UNIT_LENGTH_PX;
		return EINA_TRUE;
	}

	/* strlen(endptr) >= 1 */
	if (endptr[1] == '\0')
	{
		if (endptr[0] == '%')
		{
			length->value = val;
			length->unit = ESVG_UNIT_LENGTH_PERCENT;
		}
	}
	else if (endptr[2] == '\0')
	{
		if (endptr[0] == 'e')
		{
			if (endptr[1] == 'm')
			{
				length->value = val;
				length->unit = ESVG_UNIT_LENGTH_EM;
			}
			else if (endptr[1] == 'x')
			{
				length->value = val;
				length->unit = ESVG_UNIT_LENGTH_EX;
			}
		}
		else if (endptr[0] == 'p')
		{
			if (endptr[1] == 'c')
			{
				length->value = val;
				length->unit = ESVG_UNIT_LENGTH_PC;
			}
			else if (endptr[1] == 't')
			{
				length->value = val;
				length->unit = ESVG_UNIT_LENGTH_PT;
			}
			else if (endptr[1] == 'x')
			{
				length->value = val;
				length->unit = ESVG_UNIT_LENGTH_PX;
			}
		}
		else if ((endptr[0] == 'c') && (endptr[1] == 'm'))
		{
			length->value = val;
			length->unit = ESVG_UNIT_LENGTH_CM;
		}
		else if ((endptr[0] == 'm') && (endptr[1] == 'm'))
		{
			length->value = val;
			length->unit = ESVG_UNIT_LENGTH_MM;
		}
		else if ((endptr[0] == 'i') && (endptr[1] == 'n'))
		{
			length->value = val;
			length->unit = ESVG_UNIT_LENGTH_IN;
		}
	}
	return EINA_TRUE;
}

EAPI Eina_Bool esvg_length_is_equal(Esvg_Length *length1, Esvg_Length *length2)
{
	if (!length1 || !length2)
		return EINA_FALSE;
	if ((length1->value == length2->value) &&(length1->unit == length2->unit))
		return EINA_TRUE;

	return EINA_FALSE;
}

/* FIXME rename this to esvg_coord_ */
EAPI double esvg_length_final_get(const Esvg_Length *l, double parent_length, double font_size)
{
	double ret;
	/* Here we should transform the length/coord to an absolute
	 * value. For some cases we'll need the dpi, should it be global of
	 * the lib? per svg instance?
	 */
	/* FIXME make this numbers preprocessor macros */
	switch (l->unit)
	{
		case ESVG_UNIT_LENGTH_PERCENT:
		ret = (parent_length * l->value) / 100.0;
		break;

		case ESVG_UNIT_LENGTH_PT:
		ret = 1.25 * l->value;
		break;

		case ESVG_UNIT_LENGTH_PC:
		ret = 15 * l->value;
		break;

		case ESVG_UNIT_LENGTH_CM:
		ret = 35.43307 * l->value;
		break;

		case ESVG_UNIT_LENGTH_MM:
		ret = 3.543307 * l->value;
		break;

		case ESVG_UNIT_LENGTH_IN:
		ret = 90 * l->value;
		break;

		case ESVG_UNIT_LENGTH_PX:
		ret = l->value;
		break;

		case ESVG_UNIT_LENGTH_EM:
		ret = font_size * l->value;
		break;

		/* FIXME this one depends on the x-height */
		case ESVG_UNIT_LENGTH_EX:
		default:
		ret = l->value;
		break;
	}

	return ret;
}

/* FIXME rename this to esvg_length_ */
EAPI double esvg_length_full_final_get(const Esvg_Length *l, double width, double height, double font_size)
{
	double length = 0;
	if (l->unit == ESVG_UNIT_LENGTH_PERCENT)
	{
		length = hypot(width, height) * M_SQRT1_2;
	}
	return esvg_length_final_get(l, length, font_size);
}

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

EAPI Eina_Bool esvg_paint_is_equal(const Esvg_Paint *p1,
		const Esvg_Paint *p2)
{
	/* sanity checks */
	if (p1 == p2) return EINA_TRUE;
	if (!p1) return EINA_FALSE;
	if (!p2) return EINA_FALSE;

	if (p1->type != p2->type) return EINA_FALSE;
	/* ok, we have values and same types, now compare each type */
	switch (p1->type)
	{
		case ESVG_PAINT_NONE:
		case ESVG_PAINT_CURRENT_COLOR:
		return EINA_TRUE;

		case ESVG_PAINT_COLOR:
		return esvg_color_is_equal(&p1->value.color, &p2->value.color);

		case ESVG_PAINT_SERVER:
		return esvg_string_is_equal(p1->value.paint_server, p2->value.paint_server);

		/* FIXME what to do in this cases? add an assert? */
		default:
		return EINA_FALSE;
	}
}

/*
 * none, currentColor, <color>, <uri>?
 */
EAPI Eina_Bool esvg_paint_string_from(Esvg_Paint *paint, const char *attr)
{
	/* none */
	if (strncmp(attr, "none", 4) == 0)
	{
		paint->type = ESVG_PAINT_NONE;
	}
	/* currentColor */
	else if (strncmp(attr, "currentColor", 12) == 0)
	{
		paint->type = ESVG_PAINT_CURRENT_COLOR;
	}
	/* color name */
	else if (esvg_color_string_from(&paint->value.color, attr))
	{
		paint->type = ESVG_PAINT_COLOR;
	}
	/* uri */
	else
	{
		paint->type = ESVG_PAINT_SERVER;
		paint->value.paint_server = strdup(attr);
	}

	return EINA_TRUE;
}


EAPI const char * esvg_type_string_to(Esvg_Type type)
{
	switch (type)
	{
		case ESVG_A: return "a";
		case ESVG_ALTGLYPH: return "";
		case ESVG_ALTGLYPHDEF: return "";
		case ESVG_ALTGLYPHITEM: return "";
		case ESVG_ANIMATE: return "animate";
		case ESVG_ANIMATECOLOR: return "animateColor";
		case ESVG_ANIMATEMOTION: return "animateMotion";
		case ESVG_ANIMATETRANSFORM: return "animateTransform";
		case ESVG_CIRCLE: return "circle";
		case ESVG_CLIPPATH: return "clipPath";
		case ESVG_COLOR_PROFILE: return "" /* 10 */;
		case ESVG_CURSOR: return "cursor";
		case ESVG_DEFINITION_SRC: return "";
		case ESVG_DEFS: return "defs";
		case ESVG_DESC: return "desc";
		case ESVG_ELLIPSE: return "ellipse";
		case ESVG_FEBLEND: return "feBlend";
		case ESVG_FECOLORMATRIX: return "";
		case ESVG_FECOMPONENTTRANSFER: return "";
		case ESVG_FECOMPOSITE: return "";
		case ESVG_FECONVOLVEMATRIX: return "" /* 20 */;
		case ESVG_FEDIFFUSELIGHTING: return "";
		case ESVG_FEDISPLACEMENTMAP: return "";
		case ESVG_FEDISTANTLIGHT: return "";
		case ESVG_FEFLOOD: return "";
		case ESVG_FEFUNCA: return "";
		case ESVG_FEFUNCB: return "";
		case ESVG_FEFUNCG: return "";
		case ESVG_FEFUNCR: return "";
		case ESVG_FEGAUSSIANBLUR: return "";
		case ESVG_FEIMAGE: return "" /* 30 */;
		case ESVG_FEMERGE: return "";
		case ESVG_FEMERGENODE: return "";
		case ESVG_FEMORPHOLOGY: return "";
		case ESVG_FEOFFSET: return "";
		case ESVG_FEPOINTLIGHT: return "";
		case ESVG_FESPECULARLIGHTING: return "";
		case ESVG_FESPOTLIGHT: return "";
		case ESVG_FETILE: return "";
		case ESVG_FETURBULENCE: return "";
		case ESVG_FILTER: return "" /* 40 */;
		case ESVG_FONT: return "";
		case ESVG_FONT_FACE: return "";
		case ESVG_FONT_FACE_FORMAT: return "";
		case ESVG_FONT_FACE_NAME: return "";
		case ESVG_FONT_FACE_SRC: return "";
		case ESVG_FONT_FACE_URI: return "";
		case ESVG_FOREIGNOBJECT: return "";
		case ESVG_G: return "g";
		case ESVG_GLYPH: return "";
		case ESVG_GLYPHREF: return "" /* 50 */;
		case ESVG_HKERN: return "";
		case ESVG_IMAGE: return "image";
		case ESVG_LINE: return "line";
		case ESVG_LINEARGRADIENT: return "linearGradient";
		case ESVG_MARKER: return "marker";
		case ESVG_MASK: return "mask";
		case ESVG_METADATA: return "";
		case ESVG_MISSING_GLYPH: return "";
		case ESVG_MPATH: return "";
		case ESVG_PATH: return "path" /* 60 */;
		case ESVG_PATTERN: return "pattern";
		case ESVG_POLYGON: return "polygon";
		case ESVG_POLYLINE: return "polyline";
		case ESVG_RADIALGRADIENT: return "radialGradient";
		case ESVG_RECT: return "rect";
		case ESVG_SCRIPT: return "";
		case ESVG_SET: return "";
		case ESVG_STOP: return "stop";
		case ESVG_STYLE: return "style";
		case ESVG_SVG: return "svg" /* 70 */;
		case ESVG_SWITCH: return "";
		case ESVG_SYMBOL: return "";
		case ESVG_TEXT: return "text";
		case ESVG_TEXTPATH: return "";
		case ESVG_TITLE: return "";
		case ESVG_TREF: return "";
		case ESVG_TSPAN: return "";
		case ESVG_USE: return "use";
		case ESVG_VIEW: return "";
		case ESVG_VKERN: return "";
		default: return "unknown";
	}
}

EAPI Eina_Bool esvg_type_is_paint_server(Esvg_Type type)
{
	switch (type)
	{
		case ESVG_LINEARGRADIENT:
		case ESVG_RADIALGRADIENT:
		case ESVG_PATTERN:
		return EINA_TRUE;

		default:
		return EINA_FALSE;
	}
}

EAPI Eina_Bool esvg_type_is_shape(Esvg_Type type)
{
	switch (type)
	{
		case ESVG_LINE:
		case ESVG_RECT:
		case ESVG_POLYLINE:
		case ESVG_POLYGON:
		case ESVG_ELLIPSE:
		case ESVG_CIRCLE:
		case ESVG_PATH:
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
		case ESVG_SVG:
		case ESVG_G:
		case ESVG_IMAGE:
		case ESVG_TEXT:
		case ESVG_USE:
		return EINA_TRUE;

		default:
		return EINA_FALSE;
	}
}


EAPI Eina_Bool esvg_parser_gradient_units_string_from(Esvg_Gradient_Units *gu, const char *attr)
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


EAPI Eina_Bool esvg_color_is_equal(const Esvg_Color *c1, const Esvg_Color *c2)
{
	/* sanity checks */
	if (c1 == c2) return EINA_TRUE;
	if (!c1) return EINA_FALSE;
	if (!c2) return EINA_FALSE;

	if ((c1->r == c2->r) && (c1->g == c2->g) && (c1->b == c2->b))
		return EINA_TRUE;
	return EINA_FALSE;
}

/*
 * #rgb
 * #rrggbb
 * rgb(c, c, c)
 */
EAPI Eina_Bool esvg_color_string_from(Esvg_Color *color, const char *attr_val)
{
	size_t sz;

	sz = strlen(attr_val);

	/*
	 * check if it starts with the #:
	 * #rgb
	 * #rrggbb
	 */
	if (attr_val[0] == '#')
	{
		if (sz == 4)
		{
			if (ESVG_IS_HEXA(attr_val[1]) &&
			    ESVG_IS_HEXA(attr_val[2]) &&
			    ESVG_IS_HEXA(attr_val[3]))
			{
				color->r = _esvg_c_to_hh(attr_val[1]);
				color->g = _esvg_c_to_hh(attr_val[2]);
				color->b = _esvg_c_to_hh(attr_val[3]);
				return EINA_TRUE;
			}
		}
		else if (sz == 7)
		{
			if (ESVG_IS_HEXA(attr_val[1]) &&
			    ESVG_IS_HEXA(attr_val[2]) &&
			    ESVG_IS_HEXA(attr_val[3]) &&
			    ESVG_IS_HEXA(attr_val[4]) &&
			    ESVG_IS_HEXA(attr_val[5]) &&
			    ESVG_IS_HEXA(attr_val[6]))
			{
				color->r = _esvg_cc_to_hh(attr_val + 1);
				color->g = _esvg_cc_to_hh(attr_val + 3);
				color->b = _esvg_cc_to_hh(attr_val + 5);
				return EINA_TRUE;
			}
		}
		return EINA_FALSE;
	}
	/*
	 * check if it starts with the rbg(:
	 * rgb(c,c,c) (size at least 10)
	 */
	else if (sz >= 10)
	{
		/* starts with rgb( and finish with ) ? */
		if ((attr_val[0] == 'r') &&
		    (attr_val[1] == 'g') &&
		    (attr_val[2] == 'b') &&
		    (attr_val[3] == '(') &&
		    (attr_val[sz - 1] == ')'))
		{
			unsigned char cl[3];
			const char *iter;
			const char *tmp;
			long val;
			int nbr = 0;

			/* n, n, n */
			/* n%, n%, n% */
			iter = tmp = attr_val + 4;
			while (*tmp)
			{
				ESVG_SPACE_SKIP(tmp);
				if (*tmp == ',')
				{
                                        tmp++;
					iter = tmp;
				}
				if (_esvg_long_get(iter, &tmp, &val))
				{
					if ((*tmp != ' ') && (*tmp != ',') && (*tmp != '%') && (*tmp != ')'))
						break;
					if ((val >= 0) && (val <= 255))
					{
						if (*tmp == '%')
						{
							tmp++;
							cl[nbr] = (unsigned char)((255L * val) / 100L);
						}
						else
						{
							cl[nbr] = (unsigned char)val;
						}
						nbr++;
                                        }
                                        if (nbr == 3)
                                        {
						/* check the last parameter */
						ESVG_SPACE_SKIP(tmp);
						if ((*tmp == ')') && (*(tmp + 1) == '\0'))
						{
							/* parsing is correct, we set the color and return */
							color->r = cl[0];
							color->g = cl[1];
							color->b = cl[2];
							return EINA_TRUE;
						}
                                        }
				}
                                else
					break;
			}
		}
		return EINA_FALSE;
	}
	/* is a keyword */
	else
	{
		return _esvg_color_keyword_from(color, attr_val);
	}
}


EAPI Esvg_Stroke_Line_Cap esvg_stroke_line_cap_string_from(const char *value)
{
	Esvg_Stroke_Line_Cap stroke_line_cap = ESVG_LINE_CAP_BUTT;

	if (!strcmp(value, "butt"))
		stroke_line_cap = ESVG_LINE_CAP_BUTT;
	else if (!strcmp(value, "round"))
		stroke_line_cap = ESVG_LINE_CAP_ROUND;
	else if (!strcmp(value, "square"))
		stroke_line_cap = ESVG_LINE_CAP_SQUARE;

	return stroke_line_cap;
}

EAPI Esvg_Stroke_Line_Join esvg_stroke_line_join_string_from(const char *value)
{
	Esvg_Stroke_Line_Join stroke_line_join = ESVG_LINE_JOIN_MITER;

	if (!strcmp(value, "miter"))
		stroke_line_join = ESVG_LINE_JOIN_MITER;
	else if (!strcmp(value, "round"))
		stroke_line_join = ESVG_LINE_JOIN_ROUND;
	else if (!strcmp(value, "bevel"))
		stroke_line_join = ESVG_LINE_JOIN_BEVEL;

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

EAPI Eina_Bool esvg_path_string_from(const char *value, Esvg_Command_Cb cb, void *data)
{
	Eina_Bool ret = EINA_TRUE;
	Eina_Bool first = EINA_TRUE;
	char last_command = 0;
	char *iter = (char *)value;

	if (!cb) return EINA_FALSE;

	ESVG_SPACE_SKIP(iter);
	/* empty path data */
	if (!*iter) return EINA_FALSE;
	/* First char must be 'M' or 'm' */
	if ((*iter != 'M') &&
	    (*iter != 'm'))
	{
		ERR("First char not 'M' or 'm' but '%c'", *iter);
		return EINA_FALSE;
	}
	while (*iter)
	{
		Esvg_Path_Command cmd;
		char command;

 		command = *iter;
		iter++;
		ret = esvg_parser_command(command, &iter, &cmd);
		if (!ret)
		{
			/* try with the last command */
			iter--;
			ret = esvg_parser_command(last_command, &iter, &cmd);
			if (ret)
			{
				cb(&cmd, data);
			}
		}
		else
		{
			/* everything went ok, update the last command */
			last_command = command;
			cb(&cmd, data);
		}

		if (!ret)
		{
			ERR("Unsupported path data instruction (%c) %s", command, iter);
			break;
		}
		/* for the 'move' case the next elements should be lines */
		if ((command == 'm' || command == 'M'))
		{
			/* the next commands should be lines */
			if (command == 'm')
				last_command = 'l';
			else
				last_command = 'L';
		}
		first = EINA_FALSE;
		ESVG_SPACE_COMMA_SKIP(iter);
	}
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
	if (!_esvg_long_get(attr, &tmp, &v))
		return EINA_FALSE;

	if (*tmp == ':')
	{
		long v2;

		tmp++;
		if (!_esvg_long_get(tmp, &tmp, &v2))
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
				if (!_esvg_long_get(tmp, &tmp, &f))
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
			if (!_esvg_long_get(tmp, &tmp, &f))
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
		else
			return EINA_FALSE;

		*clock = v * scale;
		if (f)
			*clock += (double)(f / 10.0) * scale;

		DBG("clock is %lld", *clock);
		printf("clock is %lld %s %ld\n", *clock, attr, v);
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

EAPI Eina_Bool esvg_duration_string_from(Esvg_Duration *d, const char *attr)
{
	Eina_Bool ret = EINA_TRUE;

	if (!strcmp(attr, "indefinite"))
		d->type = ESVG_DURATION_TYPE_INDEFINITE;
	else if (!strcmp(attr, "media"))
		d->type = ESVG_DURATION_TYPE_MEDIA;
	else
	{
		ret = esvg_clock_string_from(&d->data.clock, attr);
		d->type = ESVG_DURATION_TYPE_CLOCK;
		DBG("duration is %lld", d->data.clock);
	}
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

EAPI Eina_Bool esvg_repeat_count_string_from(int *rc, const char *attr)
{
	if (!strcmp(attr, "indefinite"))
		*rc = -1;
	*rc = atoi(attr);
	return EINA_TRUE;
}

EAPI Eina_Bool esvg_animate_key_spline_string_from(Esvg_Animate_Key_Spline *spline, const char *s)
{
	return EINA_TRUE;
}

EAPI Eina_Bool esvg_animation_event_string_from(Esvg_Animation_Event *e, const char *s)
{
	/* simplest cases */
	if (!strcmp("s", "indefinite"))
	{

	}
	else if (!strncmp(s, "accessKey", 9))
	{

	}
	else if (!strncmp(s, "wallclock", 9))
	{

	}
	else
	{
		/* check for the sign */
		/*
		 * TODO parse the other cases
		offset-value ::= ( S? "+" | "-" S? )? ( Clock-value )
		syncbase-value ::= ( Id-value "." ( "begin" | "end" ) ) ( S? ("+"|"-") S? Clock-value )?
		event-value ::= ( Id-value "." )? ( event-ref ) ( S? ("+"|"-") S? Clock-value )?
		repeat-value ::= ( Id-value "." )? "repeat(" integer ")" ( S? ("+"|"-") S? Clock-value )?
		*/
	}
}

EAPI void esvg_animation_event_offset_set(Esvg_Animation_Event *a, int64_t offset)
{

}
