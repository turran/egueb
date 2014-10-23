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
#include "egueb_svg_color_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_color_interpolate(Egueb_Svg_Color *v,
		Egueb_Svg_Color *a, Egueb_Svg_Color *b, double m,
		Egueb_Svg_Color *add, Egueb_Svg_Color *acc, int mul)
{
	uint32_t aargb;
	uint32_t bargb;
	uint32_t rargb;

	aargb = 0xff | (a->r >> 16) | (a->g >> 8) | a->b;
	bargb = 0xff | (b->r >> 16) | (b->g >> 8) | b->b;
	egueb_dom_value_interpolate_argb(aargb, bargb, m, &rargb);
	v->r = (rargb >> 16) & 0xff;
	v->g = (rargb >> 8) & 0xff;
	v->b = (rargb & 0xff);
}

Eina_Bool egueb_svg_color_string_from(Egueb_Svg_Color *thiz, const char *attr_val)
{
	Egueb_Css_Color argb;

	if (!egueb_css_color_string_from(&argb, attr_val))
		return EINA_FALSE;
	thiz->r = (argb >> 16) & 0xff;
	thiz->g = (argb >> 8) & 0xff;
	thiz->b = (argb & 0xff);
	return EINA_TRUE;
}

char * egueb_svg_color_string_to(Egueb_Svg_Color *thiz)
{
	Egueb_Css_Color argb;

	argb = 0xff000000 | thiz->r << 16 | thiz->g << 8 | thiz->b;
	return egueb_css_color_string_to(argb);
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
const Egueb_Svg_Color EGUEB_SVG_COLOR_BLACK = { 0, 0, 0};

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

EAPI void egueb_svg_color_components_from(Egueb_Svg_Color *thiz, uint8_t r, uint8_t g, uint8_t b)
{
	thiz->r = r;
	thiz->g = g;
	thiz->b = b;
}
