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
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_context.h"
#include "esvg_private_element.h"
#include "esvg_private_referenceable.h"
#include "esvg_private_paint_server.h"
#include "esvg_private_gradient.h"
#include "esvg_private_stop.h"
#include "esvg_private_types.h"

#include "esvg_element_linear_gradient.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _esvg_element_linear_gradient_log

static int _esvg_element_linear_gradient_log = -1;

static Ender_Property *ESVG_ELEMENT_LINEAR_GRADIENT_X1;
static Ender_Property *ESVG_ELEMENT_LINEAR_GRADIENT_Y1;
static Ender_Property *ESVG_ELEMENT_LINEAR_GRADIENT_X2;
static Ender_Property *ESVG_ELEMENT_LINEAR_GRADIENT_Y2;

typedef struct _Esvg_Element_Linear_Gradient
{
	/* properties */
	Esvg_Attribute_Coord x1;
	Esvg_Attribute_Coord y1;
	Esvg_Attribute_Coord x2;
	Esvg_Attribute_Coord y2;
	/* private */
} Esvg_Element_Linear_Gradient;

static Esvg_Element_Linear_Gradient * _esvg_element_linear_gradient_get(Edom_Tag *t)
{
	Esvg_Element_Linear_Gradient *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_TYPE_LINEARGRADIENT)
		return NULL;
	thiz = esvg_element_gradient_data_get(t);

	return thiz;
}

static void _esvg_element_linear_gradient_deep_x1_get(Esvg_Element_Linear_Gradient *thiz,
		Edom_Tag *t,
		Esvg_Coord *x1)
{
	Edom_Tag *href;

	href = esvg_element_gradient_href_tag_get(t);
	if (!thiz->x1.is_set && href)
	{
		Esvg_Element_Linear_Gradient *other;

		other = _esvg_element_linear_gradient_get(href);
		_esvg_element_linear_gradient_deep_x1_get(other, href, x1);
	}
	else
		*x1 = thiz->x1.v;
}

static void _esvg_element_linear_gradient_deep_y1_get(Esvg_Element_Linear_Gradient *thiz,
		Edom_Tag *t,
		Esvg_Coord *y1)
{
	Edom_Tag *href;

	href = esvg_element_gradient_href_tag_get(t);
	if (!thiz->y1.is_set && href)
	{
		Esvg_Element_Linear_Gradient *other;

		other = _esvg_element_linear_gradient_get(href);
		_esvg_element_linear_gradient_deep_y1_get(other, href, y1);
	}
	else
		*y1 = thiz->y1.v;
}

static void _esvg_element_linear_gradient_deep_x2_get(Esvg_Element_Linear_Gradient *thiz,
		Edom_Tag *t,
		Esvg_Coord *x2)
{
	Edom_Tag *href;

	href = esvg_element_gradient_href_tag_get(t);
	if (!thiz->x2.is_set && href)
	{
		Esvg_Element_Linear_Gradient *other;

		other = _esvg_element_linear_gradient_get(href);
		_esvg_element_linear_gradient_deep_x2_get(other, href, x2);
	}
	else
		*x2 = thiz->x2.v;
}

static void _esvg_element_linear_gradient_deep_y2_get(Esvg_Element_Linear_Gradient *thiz,
		Edom_Tag *t,
		Esvg_Coord *y2)
{
	Edom_Tag *href;

	href = esvg_element_gradient_href_tag_get(t);
	if (!thiz->y2.is_set && href)
	{
		Esvg_Element_Linear_Gradient *other;

		other = _esvg_element_linear_gradient_get(href);
		_esvg_element_linear_gradient_deep_y2_get(other, href, y2);
	}
	else
		*y2 = thiz->y2.v;
}

static void _esvg_element_linear_gradient_merge(Esvg_Element_Linear_Gradient *thiz,
		Edom_Tag *t,
		Esvg_Coord *x1, Esvg_Coord *y1,
		Esvg_Coord *x2, Esvg_Coord *y2)
{
	_esvg_element_linear_gradient_deep_x1_get(thiz, t, x1);
	_esvg_element_linear_gradient_deep_y1_get(thiz, t, y1);
	_esvg_element_linear_gradient_deep_x2_get(thiz, t, x2);
	_esvg_element_linear_gradient_deep_y2_get(thiz, t, y2);
}
/*----------------------------------------------------------------------------*
 *                       Esvg Paint Server interface                          *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_element_linear_gradient_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "x1") == 0)
	{
		Esvg_Coord x;

		esvg_length_string_from(&x, value);
		esvg_element_linear_gradient_x1_set(e, &x);
	}
	else if (strcmp(key, "y1") == 0)
	{
		Esvg_Coord y;

		esvg_length_string_from(&y, value);
		esvg_element_linear_gradient_y1_set(e, &y);
	}
	else if (strcmp(key, "x2") == 0)
	{
		Esvg_Coord x;

		esvg_length_string_from(&x, value);
		esvg_element_linear_gradient_x2_set(e, &x);
	}
	else if (strcmp(key, "y2") == 0)
	{
		Esvg_Coord y;

		esvg_length_string_from(&y, value);
		esvg_element_linear_gradient_y2_set(e, &y);
	}
	else
	{
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static Eina_Bool _esvg_element_linear_gradient_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static Enesim_Renderer * _esvg_element_linear_gradient_renderer_new(Edom_Tag *t)
{
	Enesim_Renderer *r;

	r = enesim_renderer_gradient_linear_new();
	enesim_renderer_gradient_mode_set(r, ENESIM_PAD);
	return r;
}
static Eina_Bool _esvg_element_linear_gradient_propagate(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *ctx,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Element_Gradient_Context *gctx,
		Enesim_Renderer *r,
		Enesim_Error **error)
{
	Esvg_Element_Linear_Gradient *thiz;
	Esvg_Element_Gradient_Units gu;
	Enesim_Repeat_Mode mode;
	Enesim_Matrix m;
	Esvg_Coord lx1;
	Esvg_Coord ly1;
	Esvg_Coord lx2;
	Esvg_Coord ly2;
	double x1;
	double y1;
	double x2;
	double y2;

	thiz = _esvg_element_linear_gradient_get(t);

	gu = gctx->units;
	switch (gctx->spread_method)
	{
		case ESVG_SPREAD_METHOD_PAD:
		mode = ENESIM_PAD;
		break;

		case ESVG_SPREAD_METHOD_REPEAT:
		mode = ENESIM_REPEAT;
		break;

		case ESVG_SPREAD_METHOD_REFLECT:
		mode = ENESIM_REFLECT;
		break;

		default:
		return EINA_FALSE;
	}
	enesim_renderer_gradient_mode_set(r, mode);

	/* the coordinates can come from the href
	 * we need to found which one isnt set and use that
	 * for the calculus
	 */
	_esvg_element_linear_gradient_merge(thiz, t, &lx1, &ly1, &lx2, &ly2);

	if (gu == ESVG_OBJECT_BOUNDING_BOX)
	{
		/* check that the coordinates shold be set with (0,0) -> (1, 1) */
		x1 = esvg_coord_final_get(&lx1, 1, 1);
		y1 = esvg_coord_final_get(&ly1, 1, 1);
		x2 = esvg_coord_final_get(&lx2, 1, 1);
		y2 = esvg_coord_final_get(&ly2, 1, 1);
		enesim_matrix_values_set(&m, ctx->bounds.w, 0, ctx->bounds.x, 0, ctx->bounds.h, ctx->bounds.y, 0, 0, 1);
		/* transform the bounds using the context matrix */
		enesim_matrix_compose(&ctx->transform, &m, &m);
	}
	else
	{
		double w;
		double h;

		/* use the user space coordiantes */
		w = ctx->viewbox.w;
		h = ctx->viewbox.h;
		x1 = esvg_coord_final_get(&lx1, w, ctx->font_size);
		y1 = esvg_coord_final_get(&ly1, h, ctx->font_size);
		x2 = esvg_coord_final_get(&lx2, w, ctx->font_size);
		y2 = esvg_coord_final_get(&ly2, h, ctx->font_size);

		m = ctx->transform;
	}
	if (enesim_matrix_type_get(&gctx->transform) != ENESIM_MATRIX_IDENTITY)
	{
		enesim_matrix_compose(&m, &gctx->transform, &m);
	}
	enesim_renderer_transformation_set(r, &m);
	enesim_renderer_gradient_linear_x0_set(r, x1);
	enesim_renderer_gradient_linear_y0_set(r, y1);
	enesim_renderer_gradient_linear_x1_set(r, x2);
	enesim_renderer_gradient_linear_y1_set(r, y2);

	return EINA_TRUE;

}

static void _esvg_element_linear_gradient_free(Edom_Tag *t)
{
	Esvg_Element_Linear_Gradient *thiz;

	thiz = _esvg_element_linear_gradient_get(t);
	free(thiz);
}

static Esvg_Element_Gradient_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ _esvg_element_linear_gradient_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .free 		= */ _esvg_element_linear_gradient_free,
	/* .initialize 		= */ NULL,
	/* .attribute_set 	= */ _esvg_element_linear_gradient_attribute_set,
	/* .attribute_animated_fetch = */ NULL,
	/* .setup		= */ NULL,
	/* .renderer_new	= */ _esvg_element_linear_gradient_renderer_new,
	/* .propagate		= */ _esvg_element_linear_gradient_propagate,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_element_linear_gradient_new(void)
{
	Esvg_Element_Linear_Gradient *thiz;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Element_Linear_Gradient));

	/* default values */
	thiz->x1.v = ESVG_LENGTH_0;
	thiz->y1.v = ESVG_LENGTH_0;
	thiz->x2.v = ESVG_LENGTH_100_PERCENT;
	thiz->y2.v = ESVG_LENGTH_0;

	t = esvg_element_gradient_new(&_descriptor, ESVG_TYPE_LINEARGRADIENT, thiz);
	return t;
}

static void _esvg_element_linear_gradient_x1_set(Edom_Tag *t, const Esvg_Coord *x1)
{
	Esvg_Element_Linear_Gradient *thiz;

	thiz = _esvg_element_linear_gradient_get(t);
	if (!x1)
	{
		thiz->x1.is_set = EINA_FALSE;
		thiz->x1.v = ESVG_LENGTH_0;
	}
	else
	{
		thiz->x1.v = *x1;
		thiz->x1.is_set = EINA_TRUE;
	}
}

static void _esvg_element_linear_gradient_x1_get(Edom_Tag *t, Esvg_Coord *x1)
{
	Esvg_Element_Linear_Gradient *thiz;

	if (!x1) return;
	thiz = _esvg_element_linear_gradient_get(t);
	*x1 = thiz->x1.v;
}

static Eina_Bool _esvg_element_linear_gradient_x1_is_set(Edom_Tag *t)
{
	Esvg_Element_Linear_Gradient *thiz;

	thiz = _esvg_element_linear_gradient_get(t);
	return thiz->x1.is_set;
}

static void _esvg_element_linear_gradient_y1_set(Edom_Tag *t, const Esvg_Coord *y1)
{
	Esvg_Element_Linear_Gradient *thiz;

	thiz = _esvg_element_linear_gradient_get(t);
	if (!y1)
	{
		thiz->y1.is_set = EINA_FALSE;
		thiz->y1.v = ESVG_LENGTH_0;
	}
	else
	{
		thiz->y1.v = *y1;
		thiz->y1.is_set = EINA_TRUE;
	}
}

static void _esvg_element_linear_gradient_y1_get(Edom_Tag *t, Esvg_Coord *y1)
{
	Esvg_Element_Linear_Gradient *thiz;

	if (!y1) return;
	thiz = _esvg_element_linear_gradient_get(t);
	*y1 = thiz->y1.v;
}

static Eina_Bool _esvg_element_linear_gradient_y1_is_set(Edom_Tag *t)
{
	Esvg_Element_Linear_Gradient *thiz;

	thiz = _esvg_element_linear_gradient_get(t);
	return thiz->y1.is_set;
}

static void _esvg_element_linear_gradient_x2_set(Edom_Tag *t, const Esvg_Coord *x2)
{
	Esvg_Element_Linear_Gradient *thiz;

	thiz = _esvg_element_linear_gradient_get(t);
	if (!x2)
	{
		thiz->x2.is_set = EINA_FALSE;
		thiz->x2.v = ESVG_LENGTH_100_PERCENT;
	}
	else
	{
		thiz->x2.v = *x2;
		thiz->x2.is_set = EINA_TRUE;
	}
}

static void _esvg_element_linear_gradient_x2_get(Edom_Tag *t, Esvg_Coord *x2)
{
	Esvg_Element_Linear_Gradient *thiz;

	if (!x2) return;
	thiz = _esvg_element_linear_gradient_get(t);
	*x2 = thiz->x2.v;
}

static Eina_Bool _esvg_element_linear_gradient_x2_is_set(Edom_Tag *t)
{
	Esvg_Element_Linear_Gradient *thiz;

	thiz = _esvg_element_linear_gradient_get(t);
	return thiz->x2.is_set;
}

static void _esvg_element_linear_gradient_y2_set(Edom_Tag *t, const Esvg_Coord *y2)
{
	Esvg_Element_Linear_Gradient *thiz;

	thiz = _esvg_element_linear_gradient_get(t);
	if (!y2)
	{
		thiz->y2.v = ESVG_LENGTH_0;
		thiz->y2.is_set = EINA_FALSE;
	}
	else
	{
		thiz->y2.v = *y2;
		thiz->y2.is_set = EINA_TRUE;
	}
}

static void _esvg_element_linear_gradient_y2_get(Edom_Tag *t, Esvg_Coord *y2)
{
	Esvg_Element_Linear_Gradient *thiz;

	if (!y2) return;
	thiz = _esvg_element_linear_gradient_get(t);
	*y2 = thiz->y2.v;
}

static Eina_Bool _esvg_element_linear_gradient_y2_is_set(Edom_Tag *t)
{
	Esvg_Element_Linear_Gradient *thiz;

	thiz = _esvg_element_linear_gradient_get(t);
	return thiz->y2.is_set;
}
/* The ender wrapper */
#define _esvg_element_linear_gradient_delete NULL
#include "generated/esvg_generated_element_linear_gradient.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_element_linear_gradient_init(void)
{
	_esvg_element_linear_gradient_log = eina_log_domain_register("esvg_element_linear_gradient", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_element_linear_gradient_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_esvg_element_linear_gradient_init();
}

void esvg_element_linear_gradient_shutdown(void)
{
	if (_esvg_element_linear_gradient_log < 0)
		return;
	_esvg_element_linear_gradient_shutdown();
	eina_log_domain_unregister(_esvg_element_linear_gradient_log);
	_esvg_element_linear_gradient_log = -1;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_element_linear_gradient_new(void)
{
	return ESVG_ELEMENT_NEW("SVGLinearGradientElement");
}

EAPI Eina_Bool esvg_is_linear_gradient(Ender_Element *e)
{
	Edom_Tag *t;
	Esvg_Type type;

	t = (Edom_Tag *)ender_element_object_get(e);
	type = esvg_element_internal_type_get(t);
	return (type == ESVG_TYPE_LINEARGRADIENT) ? EINA_TRUE : EINA_FALSE;
}

EAPI void esvg_element_linear_gradient_x1_set(Ender_Element *e, const Esvg_Coord *x1)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_LINEAR_GRADIENT_X1, x1, NULL);
}

EAPI void esvg_element_linear_gradient_x1_get(Ender_Element *e, Esvg_Coord *x1)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	_esvg_element_linear_gradient_x1_get(t, x1);
}

EAPI Eina_Bool esvg_element_linear_gradient_x1_is_set(Ender_Element *e)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	return _esvg_element_linear_gradient_x1_is_set(t);
}

EAPI void esvg_element_linear_gradient_y1_set(Ender_Element *e, const Esvg_Coord *y1)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_LINEAR_GRADIENT_Y1, y1, NULL);
}

EAPI void esvg_element_linear_gradient_y1_get(Ender_Element *e, Esvg_Coord *y1)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	_esvg_element_linear_gradient_y1_get(t, y1);
}

EAPI Eina_Bool esvg_element_linear_gradient_y1_is_set(Ender_Element *e)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	return _esvg_element_linear_gradient_y1_is_set(t);
}

EAPI void esvg_element_linear_gradient_x2_set(Ender_Element *e, const Esvg_Coord *x2)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_LINEAR_GRADIENT_X2, x2, NULL);
}

EAPI void esvg_element_linear_gradient_x2_get(Ender_Element *e, Esvg_Coord *x2)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	_esvg_element_linear_gradient_x2_get(t, x2);
}

EAPI Eina_Bool esvg_element_linear_gradient_x2_is_set(Ender_Element *e)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	return _esvg_element_linear_gradient_x2_is_set(t);
}

EAPI void esvg_element_linear_gradient_y2_set(Ender_Element *e, const Esvg_Coord *y2)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_LINEAR_GRADIENT_Y2, y2, NULL);
}

EAPI void esvg_element_linear_gradient_y2_get(Ender_Element *e, Esvg_Coord *y2)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	_esvg_element_linear_gradient_y2_get(t, y2);
}

EAPI Eina_Bool esvg_element_linear_gradient_y2_is_set(Ender_Element *e)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	return _esvg_element_linear_gradient_y2_is_set(t);
}
