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
#include "egueb_svg_string.h"
#include "egueb_svg_string_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void egueb_svg_string_animated_cleanup(Egueb_Svg_String_Animated *thiz)
{
	if (thiz->base)
	{
		egueb_dom_string_unref(thiz->base);
		thiz->base = NULL;
	}
	if (thiz->anim)
	{
		egueb_dom_string_unref(thiz->anim);
		thiz->anim = NULL;
	}
}

/* FIXME remove this */
EAPI Eina_Bool egueb_svg_string_is_equal(const char *s1, const char *s2)
{
	/* sanity checks */
	if (s1 == s2) return EINA_TRUE;
	if (!s1) return EINA_FALSE;
	if (!s2) return EINA_FALSE;

	if (strcmp(s1, s2)) return EINA_FALSE;
	return EINA_TRUE;
}


