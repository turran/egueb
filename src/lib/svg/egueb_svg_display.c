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
#include "egueb_svg_display.h"

#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Value_Descriptor _descriptor;

static void _egueb_svg_display_data_from(Egueb_Dom_Value *v,
		Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	v->data.d = data->d;
}

static void _egueb_svg_display_data_to(Egueb_Dom_Value *v,
		Egueb_Dom_Value_Data *data)
{
	Egueb_Svg_Display *ptr;
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	ptr = data->ptr;
	if (!ptr) return;
	*ptr = v->data.i32;
}

static char * _egueb_svg_display_string_to(const Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, NULL);
	return egueb_svg_display_string_to(v->data.i32);
}

static Eina_Bool _egueb_svg_display_string_from(Egueb_Dom_Value *v, const char *str)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, EINA_FALSE);
	return egueb_svg_display_string_from((Egueb_Svg_Display *)&v->data.i32, str);
}

static void _egueb_svg_display_interpolate(Egueb_Dom_Value *v,
		Egueb_Dom_Value *a, Egueb_Dom_Value *b, double m,
		Egueb_Dom_Value *add, Egueb_Dom_Value *acc, int mul)
{
}

static Egueb_Dom_Value_Descriptor _descriptor = {
	/* .data_from 		= */ _egueb_svg_display_data_from,
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_INT32,
	/* .data_to 		= */ _egueb_svg_display_data_to,
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .init 		= */ NULL,
	/* .free 		= */ NULL,
	/* .copy 		= */ NULL,
	/* .string_to 		= */ _egueb_svg_display_string_to,
	/* .string_from 	= */ _egueb_svg_display_string_from,
	/* .interpolate 	= */ _egueb_svg_display_interpolate,
};
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_display_descriptor_get(void)
{
	return &_descriptor;
}

EAPI Eina_Bool egueb_svg_display_string_from(Egueb_Svg_Display *d, const char *attr)
{
	if (!strcmp(attr, "inline"))
		*d = EGUEB_SVG_DISPLAY_INLINE;
	else if (!strcmp(attr, "block"))
		*d = EGUEB_SVG_DISPLAY_BLOCK;
	else if (!strcmp(attr, "list-item"))
		*d = EGUEB_SVG_DISPLAY_LIST_ITEM;
	else if (!strcmp(attr, "run-in"))
		*d = EGUEB_SVG_DISPLAY_RUN_IN;
	else if (!strcmp(attr, "compact"))
		*d = EGUEB_SVG_DISPLAY_COMPACT;
	else if (!strcmp(attr, "marker"))
		*d = EGUEB_SVG_DISPLAY_MARKER;
	else if (!strcmp(attr, "table"))
		*d = EGUEB_SVG_DISPLAY_TABLE;
	else if (!strcmp(attr, "inline-table"))
		*d = EGUEB_SVG_DISPLAY_INLINE_TABLE;
	else if (!strcmp(attr, "table-row-group"))
		*d = EGUEB_SVG_DISPLAY_TABLE_ROW_GROUP;
	else if (!strcmp(attr, "table-header-group"))
		*d = EGUEB_SVG_DISPLAY_TABLE_HEADER_GROUP;
	else if (!strcmp(attr, "table-footer-group"))
		*d = EGUEB_SVG_DISPLAY_TABLE_FOOTER_GROUP;
	else if (!strcmp(attr, "table-row"))
		*d = EGUEB_SVG_DISPLAY_TABLE_ROW;
	else if (!strcmp(attr, "table-column-group"))
		*d = EGUEB_SVG_DISPLAY_TABLE_COLUMN_GROUP;
	else if (!strcmp(attr, "table-column"))
		*d = EGUEB_SVG_DISPLAY_TABLE_COLUMN;
	else if (!strcmp(attr, "table-cell"))
		*d = EGUEB_SVG_DISPLAY_TABLE_CELL;
	else if (!strcmp(attr, "table-caption"))
		*d = EGUEB_SVG_DISPLAY_TABLE_CAPTION;
	else if (!strcmp(attr, "none"))
		*d = EGUEB_SVG_DISPLAY_NONE;
	else
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI char * egueb_svg_display_string_to(Egueb_Svg_Display thiz)
{
	switch (thiz)
	{
		case EGUEB_SVG_DISPLAY_INLINE:
			return strdup("inline");
		case EGUEB_SVG_DISPLAY_BLOCK:
			return strdup("block");
		case EGUEB_SVG_DISPLAY_LIST_ITEM:
			return strdup("list-item");
		case EGUEB_SVG_DISPLAY_RUN_IN:
			return strdup("run-in");
		case EGUEB_SVG_DISPLAY_COMPACT:
			return strdup("compact");
		case EGUEB_SVG_DISPLAY_MARKER:
			return strdup("marker");
		case EGUEB_SVG_DISPLAY_TABLE:
			return strdup("table");
		case EGUEB_SVG_DISPLAY_INLINE_TABLE:
			return strdup("inline-table");
		case EGUEB_SVG_DISPLAY_TABLE_ROW_GROUP:
			return strdup("table-row-group");
		case EGUEB_SVG_DISPLAY_TABLE_HEADER_GROUP:
			return strdup("table-header-group");
		case EGUEB_SVG_DISPLAY_TABLE_FOOTER_GROUP:
			return strdup("table-footer-group");
		case EGUEB_SVG_DISPLAY_TABLE_ROW:
			return strdup("table-row");
		case EGUEB_SVG_DISPLAY_TABLE_COLUMN_GROUP:
			return strdup("table-column-group");
		case EGUEB_SVG_DISPLAY_TABLE_COLUMN:
			return strdup("table-column");
		case EGUEB_SVG_DISPLAY_TABLE_CELL:
			return strdup("table-cell");
		case EGUEB_SVG_DISPLAY_TABLE_CAPTION:
			return strdup("table-caption");
		case EGUEB_SVG_DISPLAY_NONE:
			return strdup("none");
		default:
		return NULL;
	}
}
