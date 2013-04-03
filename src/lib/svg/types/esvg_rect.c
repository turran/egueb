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

#include "esvg_main_private.h"

#include "esvg_types.h"
#include "esvg_types_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _esvg_rect_log

static int _esvg_rect_log = -1;

static Ender_Property *ESVG_RECT_X;
static Ender_Property *ESVG_RECT_Y;
static Ender_Property *ESVG_RECT_WIDTH;
static Ender_Property *ESVG_RECT_HEIGHT;

static Ender_Property *ESVG_RECT_ANIMATED_BASE;
static Ender_Property *ESVG_RECT_ANIMATED_ANIM;

static void _esvg_rect_x_get(Esvg_Rect *thiz, double *v)
{
	*v = thiz->y;
}

static void _esvg_rect_y_get(Esvg_Rect *thiz, double *v)
{
	*v = thiz->y;
}

static void _esvg_rect_width_get(Esvg_Rect *thiz, double *v)
{
	*v = thiz->w;
}

static void _esvg_rect_height_get(Esvg_Rect *thiz, double *v)
{
	*v = thiz->h;
}

#define _esvg_rect_new NULL
#define _esvg_rect_delete NULL
#define _esvg_rect_x_set NULL
#define _esvg_rect_x_is_set NULL
#define _esvg_rect_y_set NULL
#define _esvg_rect_y_is_set NULL
#define _esvg_rect_width_set NULL
#define _esvg_rect_width_is_set NULL
#define _esvg_rect_height_set NULL
#define _esvg_rect_height_is_set NULL
#include "esvg_generated_rect.c"

#define _esvg_rect_animated_new NULL
#define _esvg_rect_animated_delete NULL
#define _esvg_rect_animated_base_set NULL
#define _esvg_rect_animated_base_get NULL
#define _esvg_rect_animated_base_is_set NULL
#define _esvg_rect_animated_anim_set NULL
#define _esvg_rect_animated_anim_get NULL
#define _esvg_rect_animated_anim_is_set NULL
#include "esvg_generated_rect_animated.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_rect_init(void)
{
	_esvg_rect_log = eina_log_domain_register("esvg_rect", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_rect_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_esvg_rect_init();
	_esvg_rect_animated_init();
}

void esvg_rect_shutdown(void)
{
	_esvg_rect_shutdown();
	_esvg_rect_animated_shutdown();
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/* FIXME: fix parsing with ' ' and ',' (do like rgb(c,c,c)) */
Eina_Bool esvg_rect_string_from(Esvg_Rect *thiz, const char *attr_val)
{
	const char *iter;
	const char *tmp;
	char *endptr;
	double val;
	double *vbp;
	int nbr = 0;

        iter = tmp = attr_val;
	vbp = (double *)thiz;
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
				return EINA_TRUE;
			}
		}
	}
	return EINA_TRUE;
}
