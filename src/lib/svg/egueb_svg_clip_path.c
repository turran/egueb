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
#include "egueb_svg_clip_path.h"
#include "egueb_svg_string.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_svg_clip_path_string_from(Egueb_Svg_Clip_Path *thiz,
		const char *str)
{
	if (!strcmp(str, "none"))
	{
		thiz->type = EGUEB_SVG_CLIP_PATH_TYPE_NONE;
		thiz->value.iri = NULL;
	}
	else
	{
		thiz->type = EGUEB_SVG_CLIP_PATH_TYPE_IRI;
		thiz->value.iri = egueb_dom_string_new_with_string(str);
	}
	return EINA_TRUE;
}

char * egueb_svg_clip_path_string_to(Egueb_Svg_Clip_Path *thiz)
{
	ERR("Not implemented");
	return NULL;
}

void egueb_svg_clip_path_interpolate(Egueb_Svg_Clip_Path *v,
		Egueb_Svg_Clip_Path *a, Egueb_Svg_Clip_Path *b, double m,
		Egueb_Svg_Clip_Path *add, Egueb_Svg_Clip_Path *acc, int mul)
{
	ERR("Not implemented");
}

Eina_Bool egueb_svg_clip_path_is_equal(const Egueb_Svg_Clip_Path *p1,
		const Egueb_Svg_Clip_Path *p2)
{
	/* sanity checks */
	if (p1 == p2) return EINA_TRUE;
	if (!p1) return EINA_FALSE;
	if (!p2) return EINA_FALSE;

	if (p1->type != p2->type) return EINA_FALSE;
	/* ok, we have values and same types, now compare each type */
	switch (p1->type)
	{
		case EGUEB_SVG_CLIP_PATH_TYPE_NONE:
		return EINA_TRUE;

		case EGUEB_SVG_CLIP_PATH_TYPE_IRI:
		return egueb_dom_string_is_equal(p1->value.iri, p2->value.iri);

		/* FIXME what to do in this cases? add an assert? */
		default:
		return EINA_FALSE;
	}
}

void egueb_svg_clip_path_copy(const Egueb_Svg_Clip_Path *thiz,
		Egueb_Svg_Clip_Path *copy, Eina_Bool full)
{
	egueb_svg_clip_path_reset(copy);

	/* common */
	copy->type = thiz->type;
	if (thiz->type == EGUEB_SVG_CLIP_PATH_TYPE_IRI)
	{
		if (thiz->value.iri)
		{
			if (full)
				copy->value.iri = egueb_dom_string_dup(thiz->value.iri);
			else
				copy->value.iri = egueb_dom_string_ref(thiz->value.iri);
		}
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
const Egueb_Svg_Clip_Path EGUEB_SVG_CLIP_PATH_NONE = {
	EGUEB_SVG_CLIP_PATH_TYPE_NONE,
	{ NULL }
};

EAPI void egueb_svg_clip_path_reset(Egueb_Svg_Clip_Path *thiz)
{
	if (thiz->type == EGUEB_SVG_CLIP_PATH_TYPE_IRI)
	{
		if (thiz->value.iri)
		{
			egueb_dom_string_unref(thiz->value.iri);
			thiz->value.iri = NULL;
		}
	}
}
