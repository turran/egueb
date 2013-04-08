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
/*----------------------------------------------------------------------------*
 *                                   Basic                                    *
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
	const Esvg_Number_Animated *v,
	const Esvg_Number *def,
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
		esvg_attribute_number_set(a, &v->base, def);
	else
		esvg_attribute_number_unset(a, def);
}

void esvg_attribute_animated_number_extended_set(Esvg_Attribute_Animated_Number *aa,
	const Esvg_Number_Animated *v,
	const Esvg_Number *def,
	Eina_Bool animate,
	int *set)
{
	Eina_Bool was_set;
	Eina_Bool is_set;

	was_set = aa->anim.is_set || aa->base.is_set;
	esvg_attribute_animated_number_set(aa, v, def, animate);
	is_set = aa->anim.is_set || aa->base.is_set;
	if (was_set && !is_set)
		(*set)--;
	else if (!was_set && is_set)
		(*set)++;
}

void esvg_attribute_animated_number_get(Esvg_Attribute_Animated_Number *aa,
	Esvg_Number_Animated *v)
{
	if (!v) return;

	v->base = aa->base.v;
	if (aa->animated && aa->anim.is_set)
		v->anim = aa->anim.v;
	else
		v->anim = v->base;
}

void esvg_attribute_number_unset(Esvg_Attribute_Number *a, const Esvg_Number *def)
{
	a->v = *def;
	a->is_set = EINA_FALSE;
}

void esvg_attribute_number_set(Esvg_Attribute_Number *a, const Esvg_Number *v,
		const Esvg_Number *def)
{
	if (!v)
	{
		esvg_attribute_number_unset(a, def);
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
static void * _esvg_animate_number_new(void)
{
	Esvg_Number *v;

	v = calloc(1, sizeof(Esvg_Number));
	return v;
}

static Eina_Bool _esvg_animate_number_get(const char *attr, void **value)
{
	Esvg_Number *v = *value;

	esvg_number_string_from(v, attr, 1.0);
	return EINA_TRUE;
}

static void * _esvg_animate_number_destination_new(void)
{
	Esvg_Number_Animated *d;
	d = calloc(1, sizeof(Esvg_Number_Animated));
	return d;
}

static void _esvg_animate_number_destination_free(void *destination, Eina_Bool deep)
{
	Esvg_Number_Animated *d = destination;
	free(d);
}

static void _esvg_animate_number_destination_value_to(void *destination, void **value)
{
	Esvg_Number_Animated *d = destination;
	Esvg_Number *v = *value;

	*v = d->base;
}

static void _esvg_animate_number_interpolate(void *a, void *b, double m,
		void *add, void *acc, int mul, void *res)
{
	Esvg_Number *va = a;
	Esvg_Number *vb = b;
	Esvg_Number_Animated *vadd = add;
	Esvg_Number_Animated *r = res;

	etch_interpolate_double(va->value, vb->value, m, &r->base.value);
	if (vadd)
		r->base.value += vadd->anim.value;
}

Esvg_Attribute_Animated_Descriptor esvg_attribute_animated_number_descriptor = {
	/* .value_new 			= */ _esvg_animate_number_new,
	/* .value_get 			= */ _esvg_animate_number_get,
	/* .value_free 			= */ free,
	/* .destination_new 		= */ _esvg_animate_number_destination_new,
	/* .destination_free 		= */ _esvg_animate_number_destination_free,
	/* .destination_keep 		= */ NULL,
	/* .destination_value_from 	= */ NULL,
	/* .destination_value_to 	= */ _esvg_animate_number_destination_value_to,
	/* .interpolate 		= */ _esvg_animate_number_interpolate,
};


