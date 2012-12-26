/* Esvg - SVG
 * Copyright (C) 2012 Jorge Luis Zapata
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
#include "esvg_types.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _esvg_length_log

static int _esvg_length_log = -1;

static Ender_Property *ESVG_LENGTH_VALUE;
static Ender_Property *ESVG_LENGTH_UNIT;

#define _esvg_length_new NULL
#define _esvg_length_delete NULL
#define _esvg_length_value_set NULL
#define _esvg_length_value_get NULL
#define _esvg_length_value_is_set NULL
#define _esvg_length_unit_set NULL
#define _esvg_length_unit_get NULL
#define _esvg_length_unit_is_set NULL
#include "generated/esvg_generated_length.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_length_init(void)
{
	_esvg_length_log = eina_log_domain_register("esvg_length", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_length_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_esvg_length_init();
}

void esvg_length_shutdown(void)
{

}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
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


