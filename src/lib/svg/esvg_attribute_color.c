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
#include "esvg_main_private.h"
#include "esvg_types.h"
#include "esvg_attribute_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
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
	const Esvg_Color_Animated *v,
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

void esvg_attribute_animated_color_extended_set(Esvg_Attribute_Animated_Color *aa,
	const Esvg_Color_Animated *v,
	const Esvg_Color *def,
	Eina_Bool animate,
	int *set)
{
	Eina_Bool was_set;
	Eina_Bool is_set;

	was_set = aa->anim.is_set || aa->base.is_set;
	esvg_attribute_animated_color_set(aa, v, def, animate);
	is_set = aa->anim.is_set || aa->base.is_set;
	if (was_set && !is_set)
		(*set)--;
	else if (!was_set && is_set)
		(*set)++;
}

void esvg_attribute_animated_color_get(Esvg_Attribute_Animated_Color *aa,
	Esvg_Color_Animated *v)
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

