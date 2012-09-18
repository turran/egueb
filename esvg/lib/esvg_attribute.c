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
/*----------------------------------------------------------------------------*
 *                                Transform                                    *
 *----------------------------------------------------------------------------*/
void esvg_attribute_transform_set(Esvg_Attribute_Transform *a,
		const Enesim_Matrix *v, const Enesim_Matrix *def)
{
	if (!v)
	{
		esvg_attribute_transform_unset(a, def);
	}
	else
	{
		a->v = *v;
		a->is_set = EINA_TRUE;
	}
}

void esvg_attribute_transform_unset(Esvg_Attribute_Transform *a,
		const Enesim_Matrix *def)
{
	a->is_set = EINA_FALSE;
	a->v = *def;
}

void esvg_attribute_animated_transform_set(Esvg_Attribute_Animated_Transform *aa,
	const Esvg_Animated_Transform *v,
	const Enesim_Matrix *def,
	Eina_Bool animate)
{
	Esvg_Attribute_Transform *a;
	/* get the attribute to change */
	if (animate)
		a = &aa->anim;
	else
		a = &aa->base;
	/* get the value to set */
	if (v)
		esvg_attribute_transform_set(a, &v->base, def);
	else
		esvg_attribute_transform_unset(a, def);
}

void esvg_attribute_animated_transform_get(Esvg_Attribute_Animated_Transform *aa,
	Esvg_Animated_Transform *v)
{
	if (!v) return;

	v->base = aa->base.v;
	if (aa->animated && aa->anim.is_set)
		v->anim = aa->anim.v;
	else
		v->anim = v->base;
}

void esvg_attribute_animated_transform_final_get(Esvg_Attribute_Animated_Transform *aa, Enesim_Matrix *m)
{
	if (aa->animated)
		*m = aa->anim.v;
	else
		*m = aa->base.v;
}

Eina_Bool esvg_attribute_animated_transform_is_set(Esvg_Attribute_Animated_Transform *aa)
{
	return aa->animated ? EINA_TRUE : aa->base.is_set;
}
/*----------------------------------------------------------------------------*
 *                                  Color                                     *
 *----------------------------------------------------------------------------*/
void esvg_attribute_animated_color_merge_rel(const Esvg_Attribute_Animated_Color *rel,
		const Esvg_Attribute_Animated_Color *v,
		Esvg_Attribute_Color *d)
{
	const Esvg_Attribute_Color *rr = NULL;
	const Esvg_Attribute_Color *vv = NULL;

	if (v->animated && v->anim.is_set)
		vv = &v->anim;
	if (!vv)
		vv = &v->base;

	if (rel->animated && rel->anim.is_set)
		rr = &rel->anim;
	if (!rr)
		rr = &rel->base;

	esvg_attribute_color_merge_rel(rr, vv, d);
}

void esvg_attribute_animated_color_merge(const Esvg_Attribute_Animated_Color *v,
		Esvg_Attribute_Color *d)
{
	if (v->animated && v->anim.is_set)
	{
		d->v = v->anim.v;
		d->is_set = v->anim.is_set;
	}
	else
	{
		d->v = v->base.v;
		d->is_set = v->base.is_set;
	}
}

void esvg_attribute_color_merge_rel(const Esvg_Attribute_Color *rel,
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

void esvg_attribute_animated_color_set(Esvg_Attribute_Animated_Color *aa,
	const Esvg_Animated_Color *v,
	const Esvg_Color *def,
	Eina_Bool animate)
{
	Esvg_Attribute_Color *a;

	/* get the attribute to change */
	if (animate)
		a = &aa->anim;
	else
		a = &aa->base;
	/* get the value to set */
	if (v)
		esvg_attribute_color_set(a, &v->base, def);
	else
		esvg_attribute_color_unset(a, def);
}

void esvg_attribute_animated_color_get(Esvg_Attribute_Animated_Color *aa,
	Esvg_Animated_Color *v)
{
	if (!v) return;

	v->base = aa->base.v;
	if (aa->animated && aa->anim.is_set)
		v->anim = aa->anim.v;
	else
		v->anim = v->base;
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
/*----------------------------------------------------------------------------*
 *                                 String                                     *
 *----------------------------------------------------------------------------*/
void esvg_attribute_animated_string_merge_rel(const Esvg_Attribute_Animated_String *rel,
		const Esvg_Attribute_Animated_String *v,
		Esvg_Attribute_String *d)
{
	const Esvg_Attribute_String *rr = NULL;
	const Esvg_Attribute_String *vv = NULL;

	if (v->animated && v->anim.is_set)
		vv = &v->anim;
	if (!vv)
		vv = &v->base;

	if (rel->animated && rel->anim.is_set)
		rr = &rel->anim;
	if (!rr)
		rr = &rel->base;

	esvg_attribute_string_merge_rel(rr, vv, d);
}

void esvg_attribute_animated_string_merge(const Esvg_Attribute_Animated_String *v,
		Esvg_Attribute_String *d)
{
	if (v->animated && v->anim.is_set)
	{
		d->v = v->anim.v;
		d->is_set = v->anim.is_set;
	}
	else
	{
		d->v = v->base.v;
		d->is_set = v->base.is_set;
	}
}

void esvg_attribute_string_merge_rel(const Esvg_Attribute_String *rel,
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

void esvg_attribute_animated_string_set(Esvg_Attribute_Animated_String *aa,
	const Esvg_Animated_String *v,
	Eina_Bool animate)
{
	Esvg_Attribute_String *a;
	const char *s;

	/* get the attribute to change */
	if (animate)
		a = &aa->anim;
	else
		a = &aa->base;
	/* get the value to set */
	if (v)
		esvg_attribute_string_set(a, v->base);
	else
		esvg_attribute_string_unset(a);
}

void esvg_attribute_animated_string_get(Esvg_Attribute_Animated_String *aa,
	Esvg_Animated_String *v)
{
	if (!v) return;

	v->base = aa->base.v;
	if (aa->animated && aa->anim.is_set)
		v->anim = aa->anim.v;
	else
		v->anim = v->base;
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

void esvg_attribute_animated_string_final_get(Esvg_Attribute_Animated_String *aa, char **v)
{
	if (aa->animated)
		*v = aa->anim.v;
	else
		*v = aa->base.v;
}
/*----------------------------------------------------------------------------*
 *                                 Length                                     *
 *----------------------------------------------------------------------------*/
void esvg_attribute_animated_length_merge_rel(const Esvg_Attribute_Animated_Length *rel,
		const Esvg_Attribute_Animated_Length *v,
		Esvg_Attribute_Length *d)
{
	const Esvg_Attribute_Length *rr = NULL;
	const Esvg_Attribute_Length *vv = NULL;

	if (v->animated && v->anim.is_set)
		vv = &v->anim;
	if (!vv)
		vv = &v->base;

	if (rel->animated && rel->anim.is_set)
		rr = &rel->anim;
	if (!rr)
		rr = &rel->base;

	esvg_attribute_length_merge_rel(rr, vv, d);
}

void esvg_attribute_animated_length_merge(const Esvg_Attribute_Animated_Length *v,
		Esvg_Attribute_Length *d)
{
	if (v->animated && v->anim.is_set)
	{
		d->v = v->anim.v;
		d->is_set = v->anim.is_set;
	}
	else
	{
		d->v = v->base.v;
		d->is_set = v->base.is_set;
	}
}

void esvg_attribute_length_merge_rel(const Esvg_Attribute_Length *rel,
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

void esvg_attribute_animated_length_set(Esvg_Attribute_Animated_Length *aa,
	const Esvg_Animated_Length *v,
	const Esvg_Length *def,
	Eina_Bool animate)
{
	Esvg_Attribute_Length *a;
	/* get the attribute to change */
	if (animate)
		a = &aa->anim;
	else
		a = &aa->base;
	/* get the value to set */
	if (v)
		esvg_attribute_length_set(a, &v->base, def);
	else
		esvg_attribute_length_unset(a, def);
}

void esvg_attribute_animated_length_get(Esvg_Attribute_Animated_Length *aa,
	Esvg_Animated_Length *v)
{
	if (!v) return;

	v->base = aa->base.v;
	if (aa->animated && aa->anim.is_set)
		v->anim = aa->anim.v;
	else
		v->anim = v->base;
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

/*----------------------------------------------------------------------------*
 *                                  Bool                                      *
 *----------------------------------------------------------------------------*/
void esvg_attribute_animated_bool_merge_rel(const Esvg_Attribute_Animated_Bool *rel,
		const Esvg_Attribute_Animated_Bool *v,
		Esvg_Attribute_Bool *d)
{
	const Esvg_Attribute_Bool *rr = NULL;
	const Esvg_Attribute_Bool *vv = NULL;

	if (v->animated && v->anim.is_set)
		vv = &v->anim;
	if (!vv)
		vv = &v->base;

	if (rel->animated && rel->anim.is_set)
		rr = &rel->anim;
	if (!rr)
		rr = &rel->base;

	esvg_attribute_bool_merge_rel(rr, vv, d);
}

void esvg_attribute_animated_bool_merge(const Esvg_Attribute_Animated_Bool *v,
		Esvg_Attribute_Bool *d)
{
	if (v->animated && v->anim.is_set)
	{
		d->v = v->anim.v;
		d->is_set = v->anim.is_set;
	}
	else
	{
		d->v = v->base.v;
		d->is_set = v->base.is_set;
	}
}

void esvg_attribute_bool_merge_rel(const Esvg_Attribute_Bool *rel,
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

void esvg_attribute_animated_bool_set(Esvg_Attribute_Animated_Bool *aa,
	const Esvg_Animated_Bool *v,
	Eina_Bool def,
	Eina_Bool animate)
{
	Esvg_Attribute_Bool *a;
	/* get the attribute to change */
	if (animate)
		a = &aa->anim;
	else
		a = &aa->base;
	/* get the value to set */
	if (v)
		esvg_attribute_bool_set(a, v->base);
	else
		esvg_attribute_bool_unset(a, def);
}

void esvg_attribute_animated_bool_get(Esvg_Attribute_Animated_Bool *aa,
	Esvg_Animated_Bool *v)
{
	if (!v) return;

	v->base = aa->base.v;
	if (aa->animated && aa->anim.is_set)
		v->anim = aa->anim.v;
	else
		v->anim = v->base;
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

/*----------------------------------------------------------------------------*
 *                                 Number                                     *
 *----------------------------------------------------------------------------*/
void esvg_attribute_animated_number_merge_rel(const Esvg_Attribute_Animated_Number *rel,
		const Esvg_Attribute_Animated_Number *v,
		Esvg_Attribute_Number *d)
{
	const Esvg_Attribute_Number *rr = NULL;
	const Esvg_Attribute_Number *vv = NULL;

	if (v->animated && v->anim.is_set)
		vv = &v->anim;
	if (!vv)
		vv = &v->base;

	if (rel->animated && rel->anim.is_set)
		rr = &rel->anim;
	if (!rr)
		rr = &rel->base;

	esvg_attribute_number_merge_rel(rr, vv, d);
}

void esvg_attribute_animated_number_merge(const Esvg_Attribute_Animated_Number *v,
		Esvg_Attribute_Number *d)
{
	if (v->animated && v->anim.is_set)
	{
		d->v = v->anim.v;
		d->is_set = v->anim.is_set;
	}
	else
	{
		d->v = v->base.v;
		d->is_set = v->base.is_set;
	}
}

void esvg_attribute_number_merge_rel(const Esvg_Attribute_Number *rel,
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

/* TODO pass the possible range values */
void esvg_attribute_animated_number_set(Esvg_Attribute_Animated_Number *aa,
	const Esvg_Animated_Number *v,
	double def,
	Eina_Bool animate)
{
	Esvg_Attribute_Number *a;
	/* get the attribute to change */
	if (animate)
		a = &aa->anim;
	else
		a = &aa->base;
	/* get the value to set */
	if (v)
		esvg_attribute_number_set(a, v->base);
	else
		esvg_attribute_number_unset(a, def);
}

void esvg_attribute_animated_number_get(Esvg_Attribute_Animated_Number *aa,
	Esvg_Animated_Number *v)
{
	if (!v) return;

	v->base = aa->base.v;
	if (aa->animated && aa->anim.is_set)
		v->anim = aa->anim.v;
	else
		v->anim = v->base;
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


/*----------------------------------------------------------------------------*
 *                                  Paint                                     *
 *----------------------------------------------------------------------------*/
void esvg_attribute_animated_paint_merge_rel(const Esvg_Attribute_Animated_Paint *rel,
		const Esvg_Attribute_Animated_Paint *v,
		Esvg_Attribute_Paint *d)
{
	const Esvg_Attribute_Paint *rr = NULL;
	const Esvg_Attribute_Paint *vv = NULL;

	if (v->animated && v->anim.is_set)
		vv = &v->anim;
	if (!vv)
		vv = &v->base;

	if (rel->animated && rel->anim.is_set)
		rr = &rel->anim;
	if (!rr)
		rr = &rel->base;

	esvg_attribute_paint_merge_rel(rr, vv, d);
}

void esvg_attribute_animated_paint_merge(const Esvg_Attribute_Animated_Paint *v,
		Esvg_Attribute_Paint *d)
{
	if (v->animated && v->anim.is_set)
	{
		d->v = v->anim.v;
		d->is_set = v->anim.is_set;
	}
	else
	{
		d->v = v->base.v;
		d->is_set = v->base.is_set;
	}
}

void esvg_attribute_paint_merge_rel(const Esvg_Attribute_Paint *rel,
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

void esvg_attribute_animated_paint_set(Esvg_Attribute_Animated_Paint *aa,
	const Esvg_Animated_Paint *v,
	const Esvg_Paint *def,
	Eina_Bool animate)
{
	Esvg_Attribute_Paint *a;

	/* get the attribute to change */
	if (animate)
		a = &aa->anim;
	else
		a = &aa->base;
	/* get the value to set */
	if (v)
		esvg_attribute_paint_set(a, &v->base, def);
	else
		esvg_attribute_paint_unset(a, def);
}

void esvg_attribute_animated_paint_get(Esvg_Attribute_Animated_Paint *aa,
	Esvg_Animated_Paint *v)
{
	if (!v) return;

	v->base = aa->base.v;
	if (aa->animated && aa->anim.is_set)
		v->anim = aa->anim.v;
	else
		v->anim = v->base;
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

/*----------------------------------------------------------------------------*
 *                                  Enum                                      *
 *----------------------------------------------------------------------------*/
void esvg_attribute_animated_enum_merge_rel(const Esvg_Attribute_Animated_Enum *rel,
		const Esvg_Attribute_Animated_Enum *v,
		Esvg_Attribute_Enum *d)
{
	const Esvg_Attribute_Enum *rr = NULL;
	const Esvg_Attribute_Enum *vv = NULL;

	if (v->animated && v->anim.is_set)
		vv = &v->anim;
	if (!vv)
		vv = &v->base;

	if (rel->animated && rel->anim.is_set)
		rr = &rel->anim;
	if (!rr)
		rr = &rel->base;

	esvg_attribute_enum_merge_rel(rr, vv, d);
}

void esvg_attribute_animated_enum_merge(const Esvg_Attribute_Animated_Enum *v,
		Esvg_Attribute_Enum *d)
{
	if (v->animated && v->anim.is_set)
	{
		d->v = v->anim.v;
		d->is_set = v->anim.is_set;
	}
	else
	{
		d->v = v->base.v;
		d->is_set = v->base.is_set;
	}
}

void esvg_attribute_enum_merge_rel(const Esvg_Attribute_Enum *rel,
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

void esvg_attribute_enum_unset(Esvg_Attribute_Enum *a, int def)
{
	a->v = def;
	a->is_set = EINA_FALSE;
}

void esvg_attribute_enum_set(Esvg_Attribute_Enum *a, int v)
{
	a->v = v;
	a->is_set = EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                              Normal attributes                             *
 *----------------------------------------------------------------------------*/
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


