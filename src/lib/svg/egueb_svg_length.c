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
#include "egueb_svg_length.h"
#include "egueb_svg_length_private.h"

#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static const char * _egueb_svg_length_units_string_to(Egueb_Svg_Length *thiz)
{
	switch (thiz->unit)
	{
		case EGUEB_SVG_LENGTH_UNIT_EM:
		return "em";

		case EGUEB_SVG_LENGTH_UNIT_EX:
		return "ex";

		case EGUEB_SVG_LENGTH_UNIT_PX:
		return "px";

		case EGUEB_SVG_LENGTH_UNIT_PT:
		return "pt";

		case EGUEB_SVG_LENGTH_UNIT_PC:
		return "pc";

		case EGUEB_SVG_LENGTH_UNIT_CM:
		return "cm";

		case EGUEB_SVG_LENGTH_UNIT_MM:
		return "mm";

		case EGUEB_SVG_LENGTH_UNIT_IN:
		return "in";

		case EGUEB_SVG_LENGTH_UNIT_PERCENT:
		return "%";

		default:
		return NULL;
	}
}
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
static void egueb_svg_length_interpolate(Egueb_Svg_Length *thiz,
		Egueb_Svg_Length *a, Egueb_Svg_Length *b, double m,
		Egueb_Svg_Length *add, Egueb_Svg_Length *acc, int mul)
{
	thiz->unit = a->unit;
	egueb_dom_value_interpolate_double(a->value, b->value, m, &thiz->value);
	if (acc)
	{
		thiz->value += acc->value * mul;
	}
	if (add)
	{
		thiz->value += add->value;
	}
}

EGUEB_DOM_VALUE_PRIMITIVE_SIMPLE_BOILERPLATE(egueb_svg_length, Egueb_Svg_Length);
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_svg_length_string_from(Egueb_Svg_Length *thiz, const char *attr_val)
{
	char *endptr;
	double val;

	if (!attr_val || !*attr_val)
		return EINA_FALSE;

	if (!egueb_dom_double_get(attr_val, &endptr, &val))
	{
		ERR("Length %s is invalid", attr_val);
		return EINA_FALSE;
	}

	/* else, conversion has been done */
	if ((endptr == NULL) || (*endptr == '\0'))
	{
		thiz->value = val;
		thiz->unit = EGUEB_SVG_LENGTH_UNIT_UNKNOWN;
		return EINA_TRUE;
	}

	/* strlen(endptr) >= 1 */
	if (endptr[1] == '\0')
	{
		if (endptr[0] == '%')
		{
			thiz->value = val;
			thiz->unit = EGUEB_SVG_LENGTH_UNIT_PERCENT;
		}
	}
	else if (endptr[2] == '\0')
	{
		if (endptr[0] == 'e')
		{
			if (endptr[1] == 'm')
			{
				thiz->value = val;
				thiz->unit = EGUEB_SVG_LENGTH_UNIT_EM;
			}
			else if (endptr[1] == 'x')
			{
				thiz->value = val;
				thiz->unit = EGUEB_SVG_LENGTH_UNIT_EX;
			}
		}
		else if (endptr[0] == 'p')
		{
			if (endptr[1] == 'c')
			{
				thiz->value = val;
				thiz->unit = EGUEB_SVG_LENGTH_UNIT_PC;
			}
			else if (endptr[1] == 't')
			{
				thiz->value = val;
				thiz->unit = EGUEB_SVG_LENGTH_UNIT_PT;
			}
			else if (endptr[1] == 'x')
			{
				thiz->value = val;
				thiz->unit = EGUEB_SVG_LENGTH_UNIT_PX;
			}
		}
		else if ((endptr[0] == 'c') && (endptr[1] == 'm'))
		{
			thiz->value = val;
			thiz->unit = EGUEB_SVG_LENGTH_UNIT_CM;
		}
		else if ((endptr[0] == 'm') && (endptr[1] == 'm'))
		{
			thiz->value = val;
			thiz->unit = EGUEB_SVG_LENGTH_UNIT_MM;
		}
		else if ((endptr[0] == 'i') && (endptr[1] == 'n'))
		{
			thiz->value = val;
			thiz->unit = EGUEB_SVG_LENGTH_UNIT_IN;
		}
	}

	return EINA_TRUE;
}

char * egueb_svg_length_string_to(Egueb_Svg_Length *thiz)
{
	const char *units;
	char *ret;

	if (!thiz) return NULL;
	units = _egueb_svg_length_units_string_to(thiz);
	if (!units)
	{
		if (asprintf(&ret, "%g", thiz->value) < 0)
			return NULL;
	}
	else
	{
		if (asprintf(&ret, "%g%s", thiz->value, units) < 0)
			return NULL;
	}
	return ret;
}

void egueb_svg_length_css_length_from(Egueb_Svg_Length *thiz,
		Egueb_Css_Length *length)
{
	switch (length->unit)
	{
		case EGUEB_CSS_LENGTH_UNIT_EM:
		thiz->unit = EGUEB_SVG_LENGTH_UNIT_EM;
		break;
		case EGUEB_CSS_LENGTH_UNIT_EX:
		thiz->unit = EGUEB_SVG_LENGTH_UNIT_EX;
		break;
		case EGUEB_CSS_LENGTH_UNIT_PX:
		thiz->unit = EGUEB_SVG_LENGTH_UNIT_PX;
		break;
		case EGUEB_CSS_LENGTH_UNIT_PT:
		thiz->unit = EGUEB_SVG_LENGTH_UNIT_PT;
		break;
		case EGUEB_CSS_LENGTH_UNIT_PC:
		thiz->unit = EGUEB_SVG_LENGTH_UNIT_PC;
		break;
		case EGUEB_CSS_LENGTH_UNIT_CM:
		thiz->unit = EGUEB_SVG_LENGTH_UNIT_CM;
		break;
		case EGUEB_CSS_LENGTH_UNIT_MM:
		thiz->unit = EGUEB_SVG_LENGTH_UNIT_MM;
		break;
		case EGUEB_CSS_LENGTH_UNIT_IN:
		thiz->unit = EGUEB_SVG_LENGTH_UNIT_IN;
		break;
	}
	thiz->value = length->value;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
const Egueb_Svg_Length EGUEB_SVG_LENGTH_0 = { 0, EGUEB_SVG_LENGTH_UNIT_PX};
const Egueb_Svg_Length EGUEB_SVG_LENGTH_1 = { 1, EGUEB_SVG_LENGTH_UNIT_PX};
const Egueb_Svg_Length EGUEB_SVG_LENGTH_100_PERCENT = { 100, EGUEB_SVG_LENGTH_UNIT_PERCENT};
const Egueb_Svg_Length EGUEB_SVG_LENGTH_50_PERCENT = { 50, EGUEB_SVG_LENGTH_UNIT_PERCENT};

EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_length_descriptor_get(void)
{
	return &_egueb_svg_length_descriptor;
}

EAPI Eina_Bool egueb_svg_length_is_equal(Egueb_Svg_Length *length1, Egueb_Svg_Length *length2)
{
	if (!length1 || !length2)
		return EINA_FALSE;
	if ((length1->value == length2->value) &&(length1->unit == length2->unit))
		return EINA_TRUE;

	return EINA_FALSE;
}

EAPI double egueb_svg_length_final_get(const Egueb_Svg_Length *l, double width, double height, double font_size)
{
	double length = 0;
	if (l->unit == EGUEB_SVG_LENGTH_UNIT_PERCENT)
	{
		length = hypot(width, height) * M_SQRT1_2;
	}
	return egueb_svg_coord_final_get(l, length, font_size);
}

EAPI double egueb_svg_coord_final_get(const Egueb_Svg_Length *l, double parent_length, double font_size)
{
	double ret;
	/* Here we should transform the length/coord to an absolute
	 * value. For some cases we'll need the dpi, should it be global of
	 * the lib? per svg instance?
	 */
	/* FIXME make this numbers preprocessor macros */
	switch (l->unit)
	{
		case EGUEB_SVG_LENGTH_UNIT_PERCENT:
		ret = (parent_length * l->value) / 100.0;
		break;

		case EGUEB_SVG_LENGTH_UNIT_PT:
		ret = 1.25 * l->value;
		break;

		case EGUEB_SVG_LENGTH_UNIT_PC:
		ret = 15 * l->value;
		break;

		case EGUEB_SVG_LENGTH_UNIT_CM:
		ret = 35.43307 * l->value;
		break;

		case EGUEB_SVG_LENGTH_UNIT_MM:
		ret = 3.543307 * l->value;
		break;

		case EGUEB_SVG_LENGTH_UNIT_IN:
		ret = 90 * l->value;
		break;

		case EGUEB_SVG_LENGTH_UNIT_UNKNOWN:
		case EGUEB_SVG_LENGTH_UNIT_PX:
		ret = l->value;
		break;

		case EGUEB_SVG_LENGTH_UNIT_EM:
		ret = font_size * l->value;
		break;

		/* FIXME this one depends on the x-height */
		case EGUEB_SVG_LENGTH_UNIT_EX:
		default:
		ret = l->value;
		break;
	}

	return ret;
}
