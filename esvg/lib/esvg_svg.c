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
#include "Esvg.h"
#include "esvg_private.h"
#include "esvg_values.h"
/*
 * Given that a svg element can clip, we should use a clipper with a compound
 * inside as the renderer
 * TODO
 * Handle the following attributes
 * onunload
 * onabort 
 * onerror 
 * onresize
 * onscroll
 * onzoom
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_SVG_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_SVG_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_SVG_MAGIC);\
	} while(0)

typedef struct _Esvg_Svg
{
	EINA_MAGIC
	/* properties */
	double version;
	Esvg_View_Box view_box;
	Eina_Bool view_box_set;
	Esvg_Coord x;
	Esvg_Coord y;
	Esvg_Length width;
	Esvg_Length height;
	/* private */
	Eina_List *styles; /* the list of styles found on this svg scope */
	Eina_List *svgs; /* the list of svg documents found on the svg */
	Enesim_Renderer *r; /* the renderer associated with this svg */
	Eina_Hash *ids; /* the ids found */
} Esvg_Svg;

static Esvg_Svg * _esvg_svg_get(Edom_Tag *t)
{
	Esvg_Svg *thiz;

	thiz = esvg_renderable_data_get(t);
	ESVG_SVG_MAGIC_CHECK(thiz);

	return thiz;
}

/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_svg_attribute_set(Edom_Tag *t, const char *key, const char *value)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	if (strcmp(key, "version") == 0)
	{
		double version = esvg_number_get(value, 0.0);
		esvg_svg_version_set(t, version);
	}
	else if (strcmp(key, "x") == 0)
	{
		Esvg_Coord x;

		esvg_length_get(&x, value, ESVG_COORD_0);
		esvg_svg_x_set(t, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Esvg_Coord y;

		esvg_length_get(&y, value, ESVG_COORD_0);
		esvg_svg_y_set(t, &y);
	}
	else if (strcmp(key, "width") == 0)
	{
		Esvg_Length width;

		esvg_length_get(&width, value, ESVG_LENGTH_0);
		esvg_svg_width_set(t, &width);
	}
	else if (strcmp(key, "height") == 0)
	{
		Esvg_Length height;

		esvg_length_get(&height, value, ESVG_LENGTH_0);
		esvg_svg_height_set(t, &height);
	}
	else if (strcmp(key, "viewBox") == 0)
	{
		Esvg_View_Box vb = esvg_view_box_get(value);
		esvg_svg_viewbox_set(t, &vb);
	}

	return EINA_TRUE;
}

static const char * _esvg_svg_attribute_get(Edom_Tag *tag, const char *attribute)
{
	return NULL;
}

static const char * _esvg_svg_name_get(Edom_Tag *tag)
{
	return "svg";
}

static Eina_Bool _esvg_svg_child_add(Edom_Tag *tag, Edom_Tag *child)
{
	Enesim_Renderer *r = NULL;
	Esvg_Svg *thiz;
	int tag_id;

	thiz = _esvg_svg_get(tag);
	tag_id = edom_tag_type_get(child);
	switch (tag_id)
	{
		case ESVG_USE:
		case ESVG_SVG:
		case ESVG_CIRCLE:
		case ESVG_ELLIPSE:
		case ESVG_RECT:
		case ESVG_LINE:
		case ESVG_PATH:
		case ESVG_POLYLINE:
		case ESVG_POLYGON:
		case ESVG_TEXT:
		case ESVG_G:
		case ESVG_IMAGE:
		r = esvg_parser_element_renderer_get(child);

		default:
		break;
	}
	if (r)
		esvg_container_element_add(thiz->r, r);
	esvg_parser_svg_tag_add(tag, child);

	return EINA_TRUE;
}

static const char * _esvg_svg_name_get(Enesim_Renderer *r)
{
	return "esvg_svg";
}

static Eina_Bool _esvg_svg_element_add(Edom_Tag *t, Enesim_Renderer *child)
{
	Esvg_Svg *thiz;
	Edom_Tag *teal_r;

	if (!esvg_element_is_renderable(child))
	{
		return EINA_FALSE;
	}

	thiz = _esvg_svg_get(t);
	enesim_renderer_compound_layer_add(thiz->r, child);

	return EINA_TRUE;
}

static void _esvg_svg_element_remove(Edom_Tag *t, Enesim_Renderer *child)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	enesim_renderer_compound_layer_remove(thiz->t, child);
}

static Enesim_Renderer * _esvg_svg_element_at(Edom_Tag *t, double x, double y)
{
	/* TODO */
	return NULL;
}

static Eina_Bool _esvg_svg_setup(Edom_Tag *t, Esvg_Element_State *state,
		Esvg_Attribute_Presentation *attr,
		Enesim_Surface *s,
		Enesim_Error **error)
{
	Esvg_Svg *thiz;
	Eina_List *l;
	Enesim_Renderer *child;
	Enesim_Renderer *parent;
	double width, height;

	thiz = _esvg_svg_get(t);
	width = esvg_length_final_get(&thiz->width, state->viewbox_w);
	height = esvg_length_final_get(&thiz->height, state->viewbox_h);
	/* the viewbox will set a new user space coordinate */
	/* FIXME check zeros */
	if (thiz->view_box_set)
	{
		Enesim_Matrix scale;
		double new_vw;
		double new_vh;

		new_vw = thiz->view_box.width / width;
		new_vh = thiz->view_box.height / height;

		printf("setting scale %p to %g %g - %g %g (%g %g)\n",
				state,
				thiz->view_box.width,
				thiz->view_box.height,
				width,
				height,
				new_vw, new_vh);
		width = thiz->view_box.width;
		height = thiz->view_box.height;

		enesim_matrix_scale(&scale, 1.0/new_vw, 1.0/new_vh);
		enesim_matrix_compose(&scale, &state->transform, &state->transform);
		/* TODO handle current matrix */
	}
	state->viewbox_w = width;
	state->viewbox_h = height;

	return EINA_TRUE;
}

static Enesim_Renderer * _esvg_svg_renderer_get(Edom_Tag *t,
		const Esvg_Element_State *state,
		const Esvg_Attribute_Presentation *attr)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(r);
	return thiz->r;
}

static void _esvg_svg_clone(Edom_Tag *t, Enesim_Renderer *dr)
{

}

static void _esvg_svg_free(Edom_Tag *t)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	free(thiz);
}

static Esvg_Renderable_Descriptor _descriptor = {
	/* .name_get 		= */ _esvg_svg_name_get,
	/* .child_add		= */ _esvg_svg_child_add,
	/* .child_remove	= */ NULL,
	/* .attribute_set 	= */ _esvg_svg_attribute_set,
	/* .attribute_get 	= */ _esvg_svg_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .free 		= */ _esvg_svg_free,
	/* .clone		= */ _esvg_svg_clone,
	/* .setup		= */ _esvg_svg_setup,
	/* .renderer_get	= */ _esvg_svg_renderer_get,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_svg_style_add(Edom_Tag *tag, Esvg_Parser_Style *s)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(tag);
	thiz->styles = eina_list_append(thiz->styles, s);
}

void esvg_svg_style_apply(Edom_Tag *tag)
{
	Esvg_Svg *thiz;
	Esvg_Parser_Style *s;
	Eina_List *l;

	/* FIXME  we are iterating over every style and going through the
	 * tag tree on ecss, we better merge the styles to only parse the tree
	 * once
	 */
	thiz = _esvg_svg_get(tag);
	EINA_LIST_FOREACH(thiz->styles, l, s)
	{
		printf("applying style %p\n", s);
		esvg_style_apply(s, thiz->tag);
	}
}

void esvg_svg_svg_add(Edom_Tag *tag, Edom_Tag *svg)
{

}

Edom_Tag * esvg_svg_tag_find(Edom_Tag *tag, const char *id)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(tag);
	return eina_hash_find(thiz->ids, id);
}

void esvg_svg_tag_add(Edom_Tag *tag, Edom_Tag *child_tag)
{
	Esvg_Svg *thiz;
	const char *id;

	thiz = _esvg_svg_get(tag);
	id = edom_tag_id_get(child_tag);
	if (id)
	{
		eina_hash_add(thiz->ids, id, child_tag);
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Edom_Tag * esvg_svg_new(void)
{
	Esvg_Svg *thiz;
	Edom_Tag *t;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Svg));
	if (!thiz) return NULL;
	EINA_MAGIC_SET(thiz, ESVG_SVG_MAGIC);

	t = enesim_renderer_compound_new();
	enesim_renderer_rop_set(t, ENESIM_BLEND);
	thiz->r = r;
	thiz->ids = eina_hash_string_superfast_new(NULL);

	/* Default values */
	thiz->version = 1.0;
	thiz->x = ESVG_COORD_0;
	thiz->y = ESVG_COORD_0;
	thiz->width = ESVG_LENGTH_100_PERCENT;
	thiz->height = ESVG_LENGTH_100_PERCENT;

	/* no default value for the view_box */

	t = esvg_renderable_new(&_descriptor, thiz);
	return t;
}

EAPI Eina_Bool esvg_is_svg(Edom_Tag *t)
{
	Esvg_Svg *thiz;
	Eina_Bool ret;

	if (!esvg_is_container(t))
		return EINA_FALSE;
	thiz = esvg_container_data_get(t);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_SVG_MAGIC);

	return ret;
}

EAPI void esvg_svg_version_set(Edom_Tag *t, double version)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	thiz->version = version;
}

EAPI void esvg_svg_version_get(Edom_Tag *t, double *version)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	if (version) *version = thiz->version;
}

EAPI void esvg_svg_x_set(Edom_Tag *t, Esvg_Coord *x)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	thiz->x = *x;
}

EAPI void esvg_svg_y_set(Edom_Tag *t, Esvg_Coord *y)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	thiz->y = *y;
}

EAPI void esvg_svg_width_set(Edom_Tag *t, Esvg_Length *width)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	thiz->width = *width;
}

EAPI void esvg_svg_height_set(Edom_Tag *t, Esvg_Length *height)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	thiz->height = *height;
}

EAPI void esvg_svg_viewbox_set(Edom_Tag *t, Esvg_View_Box *vb)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	if (!vb)
	{
		thiz->view_box_set = EINA_FALSE;
	}
	else
	{
		thiz->view_box = *vb;
		thiz->view_box_set = EINA_TRUE;
	}
}

/* FIXME the below two functions should return the actual width/height based
 * on the width and height properties, if it is relative it should use the
 * the container width/height to compute them, this is are helper functions
 * to know the real size of the svg, this way we'll know on the upper libraries
 * the preferred area
 */
EAPI void esvg_svg_actual_width_get(Edom_Tag *t, double *actual_width)
{
	Esvg_Svg *thiz;
	double aw;
	double cw;

	thiz = _esvg_svg_get(t);
	esvg_element_container_width_get(t, &cw);
	aw = esvg_length_final_get(&thiz->width, cw);
	*actual_width = aw;
}

EAPI void esvg_svg_actual_height_get(Edom_Tag *t, double *actual_height)
{
	Esvg_Svg *thiz;
	double ah;
	double ch;

	thiz = _esvg_svg_get(t);
	esvg_element_container_height_get(t, &ch);
	ah = esvg_length_final_get(&thiz->height, ch);
	*actual_height = ah;
}
