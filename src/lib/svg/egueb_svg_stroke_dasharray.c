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
#include "egueb_svg_main_private.h"
#include "egueb_svg_length.h"
#include "egueb_svg_stroke_dasharray.h"

#include "egueb_svg_length_private.h"
#include "egueb_svg_stroke_dasharray_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Bool _egueb_svg_stroke_dasharray_string_to_cb(void *data, void *user_data)
{
	Egueb_Svg_Length *seg = data;
	Eina_Strbuf *strbuf = user_data;
	char *str;

	str = egueb_svg_length_string_to(seg);
	if (eina_strbuf_length_get(strbuf))
	{
		eina_strbuf_append_char(strbuf, ' ');
	}
	if (str)
	{
		eina_strbuf_append(strbuf, str);
		free(str);
	}
	return EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_svg_stroke_dasharray_string_from(Egueb_Svg_Stroke_Dasharray *thiz,
		const char *value)
{
	if (!strcmp(value, "none"))
	{
		thiz->type = EGUEB_SVG_STROKE_DASHARRAY_TYPE_NONE;
		return EINA_TRUE;
	}
	else
	{
		Egueb_Dom_List *l;
		char *found;
		char *attr = (char *)value;
		char sep;

		EGUEB_DOM_SPACE_SKIP(attr);

		l = egueb_dom_list_new(egueb_svg_length_descriptor_get());
 		sep = ',';
		found = strchr(attr, sep);
		if (!found)
		{
			sep = ' ';
			found = strchr(attr, sep);
		}

		while (found)
		{
			Egueb_Svg_Length *length;

			*found = '\0';
			EGUEB_DOM_SPACE_SKIP(attr);

			length = calloc(1, sizeof(Egueb_Svg_Length));
			egueb_svg_length_string_from(length, attr);
			egueb_dom_list_item_append(l, length);

			*found = sep;
			attr = found + 1;
			EGUEB_DOM_SPACE_SKIP(attr);

			sep = ',';
			found = strchr(attr, sep);
			if (!found)
			{
				sep = ' ';
				found = strchr(attr, sep);
			}
		}

		if (attr)
		{
			Egueb_Svg_Length *length;

			length = calloc(1, sizeof(Egueb_Svg_Length));
			egueb_svg_length_string_from(length, attr);
			egueb_dom_list_item_append(l, length);
		}

		thiz->type = EGUEB_SVG_STROKE_DASHARRAY_TYPE_LIST;
		thiz->list = l;
		return EINA_TRUE;
	}
}

char * egueb_svg_stroke_dasharray_string_to(Egueb_Svg_Stroke_Dasharray *thiz)
{
	char *ret = NULL;

	switch (thiz->type)
	{
		case EGUEB_SVG_STROKE_DASHARRAY_TYPE_NONE:
		return strdup("none");

		case EGUEB_SVG_STROKE_DASHARRAY_TYPE_LIST:
		{
			Eina_Strbuf *strbuf;
			strbuf = eina_strbuf_new();
			egueb_dom_list_foreach(thiz->list, _egueb_svg_stroke_dasharray_string_to_cb, strbuf);
			ret = eina_strbuf_string_steal(strbuf);
			eina_strbuf_free(strbuf);
		}
		break;

		default:
		break;
	}
	return ret;
}

void egueb_svg_stroke_dasharray_interpolate(Egueb_Svg_Stroke_Dasharray *v,
		Egueb_Svg_Stroke_Dasharray *a, Egueb_Svg_Stroke_Dasharray *b, double m,
		Egueb_Svg_Stroke_Dasharray *add, Egueb_Svg_Stroke_Dasharray *acc, int mul)
{
	if (a->type != b->type)
	{
		ERR("Not implemented");
	}
	else
	{
		switch (a->type)
		{
			case EGUEB_SVG_STROKE_DASHARRAY_TYPE_NONE:
			v->type = EGUEB_SVG_STROKE_DASHARRAY_TYPE_NONE;
			break;

			case EGUEB_SVG_STROKE_DASHARRAY_TYPE_LIST:
			{
				egueb_dom_list_interpolate(v->list,
						a->list, b->list, m,
						add ? add->list : NULL,
						acc ? acc->list : NULL,
						mul);
			}
			break;

			default:
			break;
		}
	}
}

void egueb_svg_stroke_dasharray_copy(const Egueb_Svg_Stroke_Dasharray *thiz,
		Egueb_Svg_Stroke_Dasharray *copy, Eina_Bool full)
{
	switch (thiz->type)
	{
		case EGUEB_SVG_STROKE_DASHARRAY_TYPE_NONE:
		copy->type = EGUEB_SVG_STROKE_DASHARRAY_TYPE_NONE;
		break;

		case EGUEB_SVG_STROKE_DASHARRAY_TYPE_LIST:
		if (full)
		{
			copy->list = egueb_dom_list_copy(thiz->list);
		}
		else
		{
			copy->list = egueb_dom_list_ref(thiz->list);
		}
		break;

		default:
		break;
	}
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void egueb_svg_stroke_dasharray_reset(Egueb_Svg_Stroke_Dasharray *thiz)
{
	switch (thiz->type)
	{
		case EGUEB_SVG_STROKE_DASHARRAY_TYPE_NONE:
		break;

		case EGUEB_SVG_STROKE_DASHARRAY_TYPE_LIST:
		if (thiz->list)
		{
			egueb_dom_list_unref(thiz->list);
			thiz->list = NULL;
		}
		break;

		default:
		break;
	}
}
