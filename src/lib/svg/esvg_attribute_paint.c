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
 *                            Animated Descriptor                             *
 *----------------------------------------------------------------------------*/
static void * _egueb_svg_animate_paint_value_new(void)
{
	Egueb_Svg_Paint *v;

	v = calloc(1, sizeof(Egueb_Svg_Paint));
	return v;
}

static Eina_Bool _egueb_svg_animate_paint_value_get(const char *attr, void **value)
{
	Egueb_Svg_Paint *v = *value;

	egueb_svg_paint_string_from(v, attr);
	return EINA_TRUE;
}

static void _egueb_svg_animate_paint_value_free(void *value)
{
	Egueb_Svg_Paint *v = value;

	if (v->type == ESVG_PAINT_TYPE_SERVER)
		free(v->uri);
	free(v);
}

static void * _egueb_svg_animate_paint_destination_new(void)
{
	Egueb_Svg_Paint_Animated *v;
	v = calloc(1, sizeof(Egueb_Svg_Paint_Animated));
	return v;
}

static void _egueb_svg_animate_paint_destination_free(void *destination, Eina_Bool deep)
{
	Egueb_Svg_Paint_Animated *d = destination;
	free(d);
}

static void _egueb_svg_animate_paint_destination_value_to(void *destination, void **value)
{
	Egueb_Svg_Paint_Animated *d = destination;
	Egueb_Svg_Paint *v = *value;

	*v = d->base;
}

static void _egueb_svg_animate_paint_interpolate(void *a, void *b, double m,
		void *add, void *acc, int mul, void *res)
{
	Egueb_Svg_Paint *va = a;
	Egueb_Svg_Paint *vb = b;
	Egueb_Svg_Paint *vacc = acc;
	Egueb_Svg_Paint_Animated *vadd = add;
	Egueb_Svg_Paint_Animated *r = res;

	if (va->type != vb->type)
	{
		r->base.type = ESVG_PAINT_TYPE_NONE;
		return;
	}
	r->base.type = va->type;
	switch (va->type)
	{
		case ESVG_PAINT_TYPE_COLOR:{
		uint32_t acolor;
		uint32_t bcolor;
		uint32_t dcolor;

		enesim_argb_components_from(&acolor, 0xff, va->color.r, va->color.g, va->color.b);
		enesim_argb_components_from(&bcolor, 0xff, vb->color.r, vb->color.g, vb->color.b);
		etch_interpolate_argb(acolor, bcolor, m, &dcolor);
		enesim_argb_components_to(dcolor, NULL, &r->base.color.r, &r->base.color.g, &r->base.color.b);
#if 0
		if (vacc)
			r->base.value += vacc->value * mul;
		if (vadd)
			r->base.value += vadd->anim.value;
#endif
		}
		break;

		case ESVG_PAINT_TYPE_SERVER:
		case ESVG_PAINT_TYPE_NONE:
		case ESVG_PAINT_TYPE_CURRENT_COLOR:
		default:
		printf("NOT SUPPORTED\n");
		return;
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The different animated desriptors */
Egueb_Svg_Attribute_Animated_Descriptor egueb_svg_attribute_animated_paint_descriptor = {
	/* .value_new 			= */ _egueb_svg_animate_paint_value_new,
	/* .value_get 			= */ _egueb_svg_animate_paint_value_get,
	/* .value_free 			= */ _egueb_svg_animate_paint_value_free,
	/* .destination_new 		= */ _egueb_svg_animate_paint_destination_new,
	/* .destination_free 		= */ _egueb_svg_animate_paint_destination_free,
	/* .destination_keep 		= */ NULL,
	/* .destination_value_from 	= */ NULL,
	/* .destination_value_to 	= */ _egueb_svg_animate_paint_destination_value_to,
	/* .interpolate 		= */ _egueb_svg_animate_paint_interpolate,
};
/*----------------------------------------------------------------------------*
 *                                   Basic                                    *
 *----------------------------------------------------------------------------*/
void egueb_svg_attribute_paint_merge_rel(const Egueb_Svg_Attribute_Paint *rel,
		const Egueb_Svg_Attribute_Paint *v,
		Egueb_Svg_Attribute_Paint *d)
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

void egueb_svg_attribute_paint_unset(Egueb_Svg_Attribute_Paint *a, const Egueb_Svg_Paint *def)
{
	a->v = *def;
	a->is_set = EINA_FALSE;
}

void egueb_svg_attribute_paint_set(Egueb_Svg_Attribute_Paint *a, const Egueb_Svg_Paint *v,
		const Egueb_Svg_Paint *def)
{
	if (!v)
	{
		egueb_svg_attribute_paint_unset(a, def);
	}
	else
	{
		a->v = *v;
		if (v->type == ESVG_PAINT_TYPE_SERVER)
			a->v.uri = strdup(v->uri);
		a->is_set = EINA_TRUE;
	}
}
/*----------------------------------------------------------------------------*
 *                                  Animated                                  *
 *----------------------------------------------------------------------------*/
void egueb_svg_attribute_animated_paint_merge_rel(const Egueb_Svg_Attribute_Animated_Paint *rel,
		const Egueb_Svg_Attribute_Animated_Paint *v,
		Egueb_Svg_Attribute_Paint *d)
{
	const Egueb_Svg_Attribute_Paint *rr = NULL;
	const Egueb_Svg_Attribute_Paint *vv = NULL;

	if (v->animated && v->anim.is_set)
		vv = &v->anim;
	if (!vv)
		vv = &v->base;

	if (rel->animated && rel->anim.is_set)
		rr = &rel->anim;
	if (!rr)
		rr = &rel->base;

	egueb_svg_attribute_paint_merge_rel(rr, vv, d);
}

void egueb_svg_attribute_animated_paint_merge(const Egueb_Svg_Attribute_Animated_Paint *v,
		Egueb_Svg_Attribute_Paint *d)
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

void egueb_svg_attribute_animated_paint_set(Egueb_Svg_Attribute_Animated_Paint *aa,
	const Egueb_Svg_Paint_Animated *v,
	const Egueb_Svg_Paint *def,
	Eina_Bool animate)
{
	Egueb_Svg_Attribute_Paint *a;

	/* get the attribute to change */
	if (animate)
		a = &aa->anim;
	else
		a = &aa->base;
	/* get the value to set */
	if (v)
		egueb_svg_attribute_paint_set(a, &v->base, def);
	else
		egueb_svg_attribute_paint_unset(a, def);
}

void egueb_svg_attribute_animated_paint_extended_set(Egueb_Svg_Attribute_Animated_Paint *aa,
	const Egueb_Svg_Paint_Animated *v,
	const Egueb_Svg_Paint *def,
	Eina_Bool animate,
	int *set)
{
	Eina_Bool was_set;
	Eina_Bool is_set;

	was_set = aa->anim.is_set || aa->base.is_set;
	egueb_svg_attribute_animated_paint_set(aa, v, def, animate);
	is_set = aa->anim.is_set || aa->base.is_set;
	if (was_set && !is_set)
		(*set)--;
	else if (!was_set && is_set)
		(*set)++;
}

void egueb_svg_attribute_animated_paint_get(Egueb_Svg_Attribute_Animated_Paint *aa,
	Egueb_Svg_Paint_Animated *v)
{
	if (!v) return;

	v->base = aa->base.v;
	if (aa->animated && aa->anim.is_set)
		v->anim = aa->anim.v;
	else
		v->anim = v->base;
}
