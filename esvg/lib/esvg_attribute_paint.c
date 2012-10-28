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
#include "esvg_private_main.h"
#include "esvg_types.h"
#include "esvg_private_attribute.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                            Animated Descriptor                             *
 *----------------------------------------------------------------------------*/
static void * _esvg_animate_paint_value_new(void)
{
	Esvg_Paint *v;

	v = calloc(1, sizeof(Esvg_Paint));
	return v;
}

static Eina_Bool _esvg_animate_paint_value_get(const char *attr, void **value)
{
	Esvg_Paint *v = *value;

	esvg_paint_string_from(v, attr);
	return EINA_TRUE;
}

static void _esvg_animate_paint_value_free(void *value)
{
	Esvg_Paint *v = value;

	if (v->type == ESVG_PAINT_SERVER)
		free(v->value.paint_server);
	free(v);
}

static void * _esvg_animate_paint_destination_new(void)
{
	Esvg_Animated_Paint *v;
	v = calloc(1, sizeof(Esvg_Animated_Paint));
	return v;
}

static void _esvg_animate_paint_destination_free(void *destination, Eina_Bool deep)
{
	Esvg_Animated_Paint *d = destination;
	free(d);
}

static void _esvg_animate_paint_destination_value_to(void *destination, void **value)
{
	Esvg_Animated_Paint *d = destination;
	Esvg_Paint *v = *value;

	*v = d->base;
}

static void _esvg_animate_paint_interpolate(void *a, void *b, double m,
		void *add, void *acc, int mul, void *res)
{
	Esvg_Paint *va = a;
	Esvg_Paint *vb = b;
	Esvg_Paint *vacc = acc;
	Esvg_Animated_Paint *vadd = add;
	Esvg_Animated_Paint *r = res;

	if (va->type != vb->type)
	{
		r->base.type = ESVG_PAINT_NONE;
		return;
	}
	r->base.type = va->type;
	switch (va->type)
	{
		case ESVG_PAINT_COLOR:{
		uint32_t acolor;
		uint32_t bcolor;
		uint32_t dcolor;

		enesim_argb_components_from(&acolor, 0xff, va->value.color.r, va->value.color.g, va->value.color.b);
		enesim_argb_components_from(&bcolor, 0xff, vb->value.color.r, vb->value.color.g, vb->value.color.b);
		etch_interpolate_argb(acolor, bcolor, m, &dcolor);
		enesim_argb_components_to(dcolor, NULL, &r->base.value.color.r, &r->base.value.color.g, &r->base.value.color.b);
#if 0
		if (vacc)
			r->base.value += vacc->value * mul;
		if (vadd)
			r->base.value += vadd->anim.value;
#endif
		}
		break;

		case ESVG_PAINT_SERVER:
		case ESVG_PAINT_NONE:
		case ESVG_PAINT_CURRENT_COLOR:
		default:
		printf("NOT SUPPORTED\n");
		return;
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The different animated desriptors */
Esvg_Attribute_Animated_Descriptor esvg_attribute_animated_paint_descriptor = {
	/* .value_new 			= */ _esvg_animate_paint_value_new,
	/* .value_get 			= */ _esvg_animate_paint_value_get,
	/* .value_free 			= */ _esvg_animate_paint_value_free,
	/* .destination_new 		= */ _esvg_animate_paint_destination_new,
	/* .destination_free 		= */ _esvg_animate_paint_destination_free,
	/* .destination_keep 		= */ NULL,
	/* .destination_value_from 	= */ NULL,
	/* .destination_value_to 	= */ _esvg_animate_paint_destination_value_to,
	/* .interpolate 		= */ _esvg_animate_paint_interpolate,
};
/*----------------------------------------------------------------------------*
 *                                   Basic                                    *
 *----------------------------------------------------------------------------*/
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
		if (v->type == ESVG_PAINT_SERVER)
			a->v.value.paint_server = strdup(v->value.paint_server);
		a->is_set = EINA_TRUE;
	}
}
/*----------------------------------------------------------------------------*
 *                                  Animated                                  *
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

void esvg_attribute_animated_paint_extended_set(Esvg_Attribute_Animated_Paint *aa,
	const Esvg_Animated_Paint *v,
	const Esvg_Paint *def,
	Eina_Bool animate,
	int *set)
{
	Eina_Bool was_set;
	Eina_Bool is_set;

	was_set = aa->anim.is_set || aa->base.is_set;
	esvg_attribute_animated_paint_set(aa, v, def, animate);
	is_set = aa->anim.is_set || aa->base.is_set;
	if (was_set && !is_set)
		(*set)--;
	else if (!was_set && is_set)
		(*set)++;
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
