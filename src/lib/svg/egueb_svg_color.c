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
#include "egueb_svg_color.h"

#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static unsigned char _egueb_svg_c_to_h(char c)
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

static unsigned char _egueb_svg_c_to_hh(char c)
{
	unsigned char h = _egueb_svg_c_to_h(c);

	return h | ((h << 4) & 240);
}

static unsigned char _egueb_svg_cc_to_hh(const char *cc)
{
	unsigned char h1;
	unsigned char h2;

	h1 = _egueb_svg_c_to_h(cc[0]);
	h2 = _egueb_svg_c_to_h(cc[1]);
	return h2 | ((h1 << 4) & 240);
}


static Eina_Bool _egueb_svg_color_keyword_from(Egueb_Svg_Color *color, const char *attr_val)
{
	static Eina_Hash *_colors = NULL;
	Enesim_Argb *argb;
	typedef struct _Egueb_Svg_Color_Entry
	{
		const char *name;
		Enesim_Argb color;
	} Egueb_Svg_Color_Entry;

	if (!_colors)
	{
		/* Css color table */
		Egueb_Svg_Color_Entry colors[] = {
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
		for (i = 0; i < sizeof(colors) / sizeof(Egueb_Svg_Color_Entry); i++)
		{
			argb = malloc(sizeof(Enesim_Argb));
			*argb = colors[i].color;
			eina_hash_add(_colors, colors[i].name, argb);
		}
	}
	/* convert the attr to lowercase */
	argb = eina_hash_find(_colors, attr_val);

	if (!argb)
	{
		return EINA_FALSE;
	}

	color->r = (*argb >> 16) & 0xff;
	color->g = (*argb >> 8) & 0xff;
	color->b = (*argb & 0xff);

	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Value_Descriptor _descriptor;

static void _egueb_svg_color_data_from(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_primitive_data_from(v, data);
}

static void _egueb_svg_color_data_to(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_primitive_data_to(v, data);
}

static void _egueb_svg_color_free(Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	if (v->owned)
	{
		free(v->data.ptr);
		v->data.ptr = NULL;
	}
}

static void _egueb_svg_color_copy(const Egueb_Dom_Value *v, Egueb_Dom_Value *copy)
{
	Egueb_Svg_Color *vl;
	Egueb_Svg_Color *cl;

	if (!v->data.ptr)
		return;

	if (!copy->data.ptr)
	{
		copy->data.ptr = calloc(1, sizeof(Egueb_Svg_Color));
		copy->owned = EINA_TRUE;
	}
	cl = copy->data.ptr;
	vl = v->data.ptr;
	*cl = *vl;
}

static char * _egueb_svg_color_string_to(const Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, NULL);
	return egueb_svg_color_string_to(v->data.ptr);
}

static Eina_Bool _egueb_svg_color_string_from(Egueb_Dom_Value *v, const char *str)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, EINA_FALSE);
	if (!v->data.ptr)
	{
		v->data.ptr = calloc(1, sizeof(Egueb_Svg_Color));
		v->owned = EINA_TRUE;
	}
	return egueb_svg_color_string_from(v->data.ptr, str);
}

static void _egueb_svg_color_interpolate(Egueb_Dom_Value *v,
		Egueb_Dom_Value *a, Egueb_Dom_Value *b, double m,
		Egueb_Dom_Value *add, Egueb_Dom_Value *acc, int mul)
{
}

static Egueb_Dom_Value_Descriptor _descriptor = {
	/* .data_from 		= */ _egueb_svg_color_data_from,
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .data_to 		= */ _egueb_svg_color_data_to,
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .init 		= */ NULL,
	/* .free 		= */ _egueb_svg_color_free,
	/* .copy 		= */ _egueb_svg_color_copy,
	/* .string_to 		= */ _egueb_svg_color_string_to,
	/* .string_from 	= */ _egueb_svg_color_string_from,
	/* .interpolate 	= */ _egueb_svg_color_interpolate,
};

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
const Egueb_Svg_Color EGUEB_SVG_COLOR_BLACK = { 0, 0, 0};

EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_color_descriptor_get(void)
{
	return &_descriptor;
}

EAPI Eina_Bool egueb_svg_color_is_equal(const Egueb_Svg_Color *c1,
		const Egueb_Svg_Color *c2)
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
EAPI Eina_Bool egueb_svg_color_string_from(Egueb_Svg_Color *color, const char *attr_val)
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
			if (EGUEB_DOM_IS_HEXA(attr_val[1]) &&
			    EGUEB_DOM_IS_HEXA(attr_val[2]) &&
			    EGUEB_DOM_IS_HEXA(attr_val[3]))
			{
				color->r = _egueb_svg_c_to_hh(attr_val[1]);
				color->g = _egueb_svg_c_to_hh(attr_val[2]);
				color->b = _egueb_svg_c_to_hh(attr_val[3]);
				return EINA_TRUE;
			}
		}
		else if (sz == 7)
		{
			if (EGUEB_DOM_IS_HEXA(attr_val[1]) &&
			    EGUEB_DOM_IS_HEXA(attr_val[2]) &&
			    EGUEB_DOM_IS_HEXA(attr_val[3]) &&
			    EGUEB_DOM_IS_HEXA(attr_val[4]) &&
			    EGUEB_DOM_IS_HEXA(attr_val[5]) &&
			    EGUEB_DOM_IS_HEXA(attr_val[6]))
			{
				color->r = _egueb_svg_cc_to_hh(attr_val + 1);
				color->g = _egueb_svg_cc_to_hh(attr_val + 3);
				color->b = _egueb_svg_cc_to_hh(attr_val + 5);
				return EINA_TRUE;
			}
		}
		return EINA_FALSE;
	}
	/*
	 * check if it starts with the rbg(:
	 * rgb(c,c,c) (size at least 10)
	 */
	else if (sz >= 10 && (attr_val[0] == 'r') &&
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
			EGUEB_DOM_SPACE_SKIP(tmp);
			if (*tmp == ',')
			{
				tmp++;
				iter = tmp;
			}
			if (egueb_dom_long_get(iter, &tmp, &val))
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
					EGUEB_DOM_SPACE_SKIP(tmp);
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
		return EINA_FALSE;
	}
	/* is a keyword */
	else
	{
		return _egueb_svg_color_keyword_from(color, attr_val);
	}
}

EAPI char * egueb_svg_color_string_to(Egueb_Svg_Color *thiz)
{
	ERR("TODO");
	return NULL;
}

EAPI void egueb_svg_color_components_from(Egueb_Svg_Color *thiz, uint8_t r, uint8_t g, uint8_t b)
{
	thiz->r = r;
	thiz->g = g;
	thiz->b = b;
}
