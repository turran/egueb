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
 *                            Animated Descriptor                             *
 *----------------------------------------------------------------------------*/
static void * _esvg_animate_length_value_new(void)
{
	Esvg_Length *v;

	v = calloc(1, sizeof(Esvg_Length));
	return v;
}

static Eina_Bool _esvg_animate_length_value_get(const char *attr, void **value)
{
	Esvg_Length *v = *value;

	esvg_length_string_from(v, attr);
	return EINA_TRUE;
}

static void * _esvg_animate_length_destination_new(void)
{
	Esvg_Length_Animated *v;
	v = calloc(1, sizeof(Esvg_Length_Animated));
	return v;
}

static void _esvg_animate_length_destination_free(void *destination, Eina_Bool deep)
{
	Esvg_Length_Animated *d = destination;
	free(d);
}

static void _esvg_animate_length_destination_value_to(void *destination, void **value)
{
	Esvg_Length_Animated *d = destination;
	Esvg_Length *v = *value;

	*v = d->base;
}

static void _esvg_animate_length_interpolate(void *a, void *b, double m,
		void *add, void *acc, int mul, void *res)
{
	Esvg_Length *va = a;
	Esvg_Length *vb = b;
	Esvg_Length *vacc = acc;
	Esvg_Length_Animated *vadd = add;
	Esvg_Length_Animated *r = res;

	r->base.unit = va->unit;
	etch_interpolate_double(va->value, vb->value, m, &r->base.value);
	if (vacc)
		r->base.value += vacc->value * mul;
	if (vadd)
		r->base.value += vadd->anim.value;
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The different animated desriptors */
Esvg_Attribute_Animated_Descriptor esvg_attribute_animated_length_descriptor = {
	/* .value_new 			= */ _esvg_animate_length_value_new,
	/* .value_get 			= */ _esvg_animate_length_value_get,
	/* .value_free 			= */ free,
	/* .destination_new 		= */ _esvg_animate_length_destination_new,
	/* .destination_free 		= */ _esvg_animate_length_destination_free,
	/* .destination_keep 		= */ NULL,
	/* .destination_value_from 	= */ NULL,
	/* .destination_value_to 	= */ _esvg_animate_length_destination_value_to,
	/* .interpolate 		= */ _esvg_animate_length_interpolate,
};
/*----------------------------------------------------------------------------*
 *                                   Basic                                    *
 *----------------------------------------------------------------------------*/
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
 *                                  Animated                                  *
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

/* FIXME rename the merge variants */
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

void esvg_attribute_animated_length_set(Esvg_Attribute_Animated_Length *aa,
	const Esvg_Length_Animated *v,
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

void esvg_attribute_animated_length_extended_set(Esvg_Attribute_Animated_Length *aa,
	const Esvg_Length_Animated *v,
	const Esvg_Length *def,
	Eina_Bool animate,
	int *set)
{
	Eina_Bool was_set;
	Eina_Bool is_set;

	was_set = aa->anim.is_set || aa->base.is_set;
	esvg_attribute_animated_length_set(aa, v, def, animate);
	is_set = aa->anim.is_set || aa->base.is_set;
	if (was_set && !is_set)
		(*set)--;
	else if (!was_set && is_set)
		(*set)++;
}

void esvg_attribute_animated_length_get(Esvg_Attribute_Animated_Length *aa,
	Esvg_Length_Animated *v)
{
	if (!v) return;

	v->base = aa->base.v;
	if (aa->animated && aa->anim.is_set)
		v->anim = aa->anim.v;
	else
		v->anim = v->base;
}

Eina_Bool esvg_attribute_animated_length_is_set(Esvg_Attribute_Animated_Length *aa)
{
	if ((aa->animated && aa->anim.is_set) || (aa->base.is_set))
		return EINA_TRUE;
	else
		return EINA_FALSE;
}

void esvg_attribute_animated_length_final_get(Esvg_Attribute_Animated_Length *aa,
	Esvg_Length *v)
{
	if (aa->animated && aa->anim.is_set)
		*v = aa->anim.v;
	else
		*v = aa->base.v;
}
