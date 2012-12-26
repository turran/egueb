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
#define ESVG_LOG_DEFAULT _esvg_paint_log

static int _esvg_paint_log = -1;

static Ender_Property *ESVG_PAINT_TYPE;
static Ender_Property *ESVG_PAINT_URI;

static Ender_Property *ESVG_PAINT_ANIMATED_BASE;
static Ender_Property *ESVG_PAINT_ANIMATED_ANIM;

#define _esvg_paint_new NULL
#define _esvg_paint_delete NULL
#define _esvg_paint_type_set NULL
#define _esvg_paint_type_get NULL
#define _esvg_paint_type_is_set NULL
#define _esvg_paint_uri_set NULL
#define _esvg_paint_uri_get NULL
#define _esvg_paint_uri_is_set NULL
#include "generated/esvg_generated_paint.c"

#define _esvg_paint_animated_new NULL
#define _esvg_paint_animated_delete NULL
#define _esvg_paint_animated_base_set NULL
#define _esvg_paint_animated_base_get NULL
#define _esvg_paint_animated_base_is_set NULL
#define _esvg_paint_animated_anim_set NULL
#define _esvg_paint_animated_anim_get NULL
#define _esvg_paint_animated_anim_is_set NULL
#include "generated/esvg_generated_paint_animated.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_paint_init(void)
{
	_esvg_paint_log = eina_log_domain_register("esvg_paint", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_paint_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_esvg_paint_init();
	_esvg_paint_animated_init();
}

void esvg_paint_shutdown(void)
{
	_esvg_paint_shutdown();
	_esvg_paint_animated_shutdown();
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
		{
			dst->value.paint_server = strdup(src->value.paint_server);
		}
	}
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
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


