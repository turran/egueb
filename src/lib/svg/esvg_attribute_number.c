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
#include "egueb_svg_main_private.h"
#include "egueb_svg_types.h"
#include "egueb_svg_attribute_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                                   Basic                                    *
 *----------------------------------------------------------------------------*/
void egueb_svg_attribute_animated_number_merge_rel(const Egueb_Svg_Attribute_Animated_Number *rel,
		const Egueb_Svg_Attribute_Animated_Number *v,
		Egueb_Svg_Attribute_Number *d)
{
	const Egueb_Svg_Attribute_Number *rr = NULL;
	const Egueb_Svg_Attribute_Number *vv = NULL;

	if (v->animated && v->anim.is_set)
		vv = &v->anim;
	if (!vv)
		vv = &v->base;

	if (rel->animated && rel->anim.is_set)
		rr = &rel->anim;
	if (!rr)
		rr = &rel->base;

	egueb_svg_attribute_number_merge_rel(rr, vv, d);
}

void egueb_svg_attribute_animated_number_merge(const Egueb_Svg_Attribute_Animated_Number *v,
		Egueb_Svg_Attribute_Number *d)
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

void egueb_svg_attribute_number_merge_rel(const Egueb_Svg_Attribute_Number *rel,
		const Egueb_Svg_Attribute_Number *v,
		Egueb_Svg_Attribute_Number *d)
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
void egueb_svg_attribute_animated_number_set(Egueb_Svg_Attribute_Animated_Number *aa,
	const Egueb_Svg_Number_Animated *v,
	const Egueb_Svg_Number *def,
	Eina_Bool animate)
{
	Egueb_Svg_Attribute_Number *a;
	/* get the attribute to change */
	if (animate)
		a = &aa->anim;
	else
		a = &aa->base;
	/* get the value to set */
	if (v)
		egueb_svg_attribute_number_set(a, &v->base, def);
	else
		egueb_svg_attribute_number_unset(a, def);
}

void egueb_svg_attribute_animated_number_extended_set(Egueb_Svg_Attribute_Animated_Number *aa,
	const Egueb_Svg_Number_Animated *v,
	const Egueb_Svg_Number *def,
	Eina_Bool animate,
	int *set)
{
	Eina_Bool was_set;
	Eina_Bool is_set;

	was_set = aa->anim.is_set || aa->base.is_set;
	egueb_svg_attribute_animated_number_set(aa, v, def, animate);
	is_set = aa->anim.is_set || aa->base.is_set;
	if (was_set && !is_set)
		(*set)--;
	else if (!was_set && is_set)
		(*set)++;
}

void egueb_svg_attribute_animated_number_get(Egueb_Svg_Attribute_Animated_Number *aa,
	Egueb_Svg_Number_Animated *v)
{
	if (!v) return;

	v->base = aa->base.v;
	if (aa->animated && aa->anim.is_set)
		v->anim = aa->anim.v;
	else
		v->anim = v->base;
}

void egueb_svg_attribute_number_unset(Egueb_Svg_Attribute_Number *a, const Egueb_Svg_Number *def)
{
	a->v = *def;
	a->is_set = EINA_FALSE;
}

void egueb_svg_attribute_number_set(Egueb_Svg_Attribute_Number *a, const Egueb_Svg_Number *v,
		const Egueb_Svg_Number *def)
{
	if (!v)
	{
		egueb_svg_attribute_number_unset(a, def);
	}
	else
	{
		a->v = *v;
		a->is_set = EINA_TRUE;
	}
}
/*----------------------------------------------------------------------------*
 *                            Animated Descriptor                             *
 *----------------------------------------------------------------------------*/
static void * _egueb_svg_animate_number_new(void)
{
	Egueb_Svg_Number *v;

	v = calloc(1, sizeof(Egueb_Svg_Number));
	return v;
}

static Eina_Bool _egueb_svg_animate_number_get(const char *attr, void **value)
{
	Egueb_Svg_Number *v = *value;

	egueb_svg_number_string_from(v, attr, 1.0);
	return EINA_TRUE;
}

static void * _egueb_svg_animate_number_destination_new(void)
{
	Egueb_Svg_Number_Animated *d;
	d = calloc(1, sizeof(Egueb_Svg_Number_Animated));
	return d;
}

static void _egueb_svg_animate_number_destination_free(void *destination, Eina_Bool deep)
{
	Egueb_Svg_Number_Animated *d = destination;
	free(d);
}

static void _egueb_svg_animate_number_destination_value_to(void *destination, void **value)
{
	Egueb_Svg_Number_Animated *d = destination;
	Egueb_Svg_Number *v = *value;

	*v = d->base;
}

static void _egueb_svg_animate_number_interpolate(void *a, void *b, double m,
		void *add, void *acc, int mul, void *res)
{
	Egueb_Svg_Number *va = a;
	Egueb_Svg_Number *vb = b;
	Egueb_Svg_Number_Animated *vadd = add;
	Egueb_Svg_Number_Animated *r = res;

	etch_interpolate_double(va->value, vb->value, m, &r->base.value);
	if (vadd)
		r->base.value += vadd->anim.value;
}

Egueb_Svg_Attribute_Animated_Descriptor egueb_svg_attribute_animated_number_descriptor = {
	/* .value_new 			= */ _egueb_svg_animate_number_new,
	/* .value_get 			= */ _egueb_svg_animate_number_get,
	/* .value_free 			= */ free,
	/* .destination_new 		= */ _egueb_svg_animate_number_destination_new,
	/* .destination_free 		= */ _egueb_svg_animate_number_destination_free,
	/* .destination_keep 		= */ NULL,
	/* .destination_value_from 	= */ NULL,
	/* .destination_value_to 	= */ _egueb_svg_animate_number_destination_value_to,
	/* .interpolate 		= */ _egueb_svg_animate_number_interpolate,
};


