/* Esvg - SVG
 * Copyright (C) 2011 Jorge Luis Zapata, Vincent Torri
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "esvg_private_main.h"
#include "esvg_types.h"
#include "esvg_private_attribute.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_attribute_animated_color_merge(const Esvg_Attribute_Animated_Color *rel,
		const Esvg_Attribute_Animated_Color *v, Esvg_Attribute_Color *d)
{

}

void esvg_attribute_color_merge(const Esvg_Attribute_Color *rel,
		const Esvg_Attribute_Color *v,
		Esvg_Attribute_Color *d)
{
	if (!v->is_set)
	{
		d->v = rel->v;
		d->is_set = rel->is_set;
	}
	else
	{
		d->v = v->v;
		d->is_set = EINA_TRUE;
	}
}

void esvg_attribute_string_merge(const Esvg_Attribute_String *rel,
		const Esvg_Attribute_String *v,
		Esvg_Attribute_String *d)
{
	if (!v->is_set)
	{
		d->v = rel->v;
		d->is_set = rel->is_set;
	}
	else
	{
		d->v = v->v;
		d->is_set = EINA_TRUE;
	}
}

void esvg_attribute_length_merge(const Esvg_Attribute_Length *rel,
		const Esvg_Attribute_Length *v,
		Esvg_Attribute_Length *d)
{
	if (!v->is_set)
	{
		d->v = rel->v;
		d->is_set = rel->is_set;
	}
	else
	{
		d->v = v->v;
		d->is_set = EINA_TRUE;
	}
}

void esvg_attribute_number_merge(const Esvg_Attribute_Number *rel,
		const Esvg_Attribute_Number *v,
		Esvg_Attribute_Number *d)
{
	if (!v->is_set)
	{
		d->v = rel->v;
		d->is_set = rel->is_set;
	}
	else
	{
		d->v = v->v;
		d->is_set = EINA_TRUE;
	}
}

void esvg_attribute_bool_merge(const Esvg_Attribute_Bool *rel,
		const Esvg_Attribute_Bool *v,
		Esvg_Attribute_Bool *d)
{
	if (!v->is_set)
	{
		d->v = rel->v;
		d->is_set = rel->is_set;
	}
	else
	{
		d->v = v->v;
		d->is_set = EINA_TRUE;
	}
}

void esvg_attribute_paint_merge(const Esvg_Attribute_Paint *rel,
		const Esvg_Attribute_Paint *v,
		Esvg_Attribute_Paint *d)
{
	if (!v->is_set)
	{
		d->v = rel->v;
		d->is_set = rel->is_set;
	}
	else
	{
		d->v = v->v;
		d->is_set = EINA_TRUE;
	}
}

void esvg_attribute_enum_merge(const Esvg_Attribute_Enum *rel,
		const Esvg_Attribute_Enum *v,
		Esvg_Attribute_Enum *d)
{
	if (!v->is_set)
	{
		d->v = rel->v;
		d->is_set = rel->is_set;
	}
	else
	{
		d->v = v->v;
		d->is_set = EINA_TRUE;
	}
}

void esvg_attribute_string_unset(Esvg_Attribute_String *a)
{
	if (a->v)
	{
		free(a->v);
		a->v = NULL;
		a->is_set = EINA_FALSE;
	}
}


void esvg_attribute_string_set(Esvg_Attribute_String *a, const char *v)
{
	if (a->v == v)
		return;

	esvg_attribute_string_unset(a);
	if (v)
	{
		a->v = strdup(v);
		a->is_set = EINA_TRUE;
	}
}

void esvg_attribute_color_unset(Esvg_Attribute_Color *a, const Esvg_Color *def)
{
	a->v = *def;
	a->is_set = EINA_FALSE;
}

void esvg_attribute_color_set(Esvg_Attribute_Color *a, const Esvg_Color *v,
		const Esvg_Color *def)
{
	if (!v)
	{
		esvg_attribute_color_unset(a, def);
	}
	else
	{
		a->v = *v;
		a->is_set = EINA_TRUE;
	}
}

void esvg_attribute_paint_unset(Esvg_Attribute_Paint *a, const Esvg_Paint *def)
{
	a->v = *def;
	a->is_set = EINA_FALSE;
}

void esvg_attribute_paint_set(Esvg_Attribute_Paint *a, const Esvg_Paint *v,
		const Esvg_Paint *def)
{
	if (!v)
	{
		esvg_attribute_paint_unset(a, def);
	}
	else
	{
		a->v = *v;
		a->is_set = EINA_TRUE;
	}
}

void esvg_attribute_number_unset(Esvg_Attribute_Number *a, double def)
{
	a->v = def;
	a->is_set = EINA_FALSE;
}

void esvg_attribute_number_set(Esvg_Attribute_Number *a, double v)
{
	a->v = v;
	a->is_set = EINA_TRUE;
}

void esvg_attribute_length_unset(Esvg_Attribute_Length *a, const Esvg_Length *def)
{
	a->v = *def;
	a->is_set = EINA_FALSE;
}

void esvg_attribute_length_set(Esvg_Attribute_Length *a, const Esvg_Length *v,
		const Esvg_Length *def)
{
	if (!v)
	{
		esvg_attribute_length_unset(a, def);
	}
	else
	{
		a->v = *v;
		a->is_set = EINA_TRUE;
	}
}

void esvg_attribute_bool_unset(Esvg_Attribute_Bool *a, Eina_Bool def)
{
	a->v = def;
	a->is_set = EINA_FALSE;
}

void esvg_attribute_bool_set(Esvg_Attribute_Bool *a, Eina_Bool v)
{
	a->v = v;
	a->is_set = EINA_TRUE;
}

void esvg_attribute_enum_unset(Esvg_Attribute_Bool *a, int def)
{
	a->v = def;
	a->is_set = EINA_FALSE;
}

void esvg_attribute_enum_set(Esvg_Attribute_Bool *a, int v)
{
	a->v = v;
	a->is_set = EINA_TRUE;
}

#if 0
/* color default = black
 * opacity default = 1.0
 * fill default = black
 * stroke default = none
 * fill opacity > 1 && < 0
 * stroke opacity > 1 && < 0
 * stop color = black
 * stop opacity = 1.0
 */
 */
void esvg_attribute_fill_rule_set(Esvg_Attribute_Presentation *thiz, Esvg_Fill_Rule rule)
{
	thiz->fill_rule = rule;
	thiz->fill_rule_set = EINA_TRUE;
}

void esvg_attribute_fill_rule_unset(Esvg_Attribute_Presentation *thiz)
{
	thiz->fill_rule_set = EINA_FALSE;
}

void esvg_attribute_stroke_line_cap_set(Esvg_Attribute_Presentation *thiz, Esvg_Stroke_Line_Cap cap)
{
	thiz->stroke_line_cap = cap;
	thiz->stroke_line_cap_set = EINA_TRUE;
}

void esvg_attribute_stroke_line_cap_unset(Esvg_Attribute_Presentation *thiz)
{
	thiz->stroke_line_cap_set = EINA_FALSE;
}

void esvg_attribute_stroke_line_join_set(Esvg_Attribute_Presentation *thiz, Esvg_Stroke_Line_Join join)
{
	thiz->stroke_line_join = join;
	thiz->stroke_line_join_set = EINA_TRUE;
}

void esvg_attribute_stroke_line_join_unset(Esvg_Attribute_Presentation *thiz)
{
	thiz->stroke_line_join_set = EINA_FALSE;
}

#endif


