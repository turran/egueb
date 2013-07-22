/* Egueb
 * Copyright (C) 2011 - 2013 Jorge Luis Zapata
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
#include "egueb_svg_main.h"
#include "egueb_svg_painter.h"
#include "egueb_svg_renderable.h"
#include "egueb_svg_reference.h"
#include "egueb_svg_shape.h"
#include "egueb_svg_reference_private.h"
#include "egueb_svg_painter_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_PAINTER_CLIP_PATH_DESCRIPTOR egueb_svg_painter_clip_path_descriptor_get()
#define EGUEB_SVG_PAINTER_CLIP_PATH_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Painter_Clip_Path_Class, EGUEB_SVG_PAINTER_CLIP_PATH_DESCRIPTOR)
#define EGUEB_SVG_PAINTER_CLIP_PATH(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Painter_Clip_Path, EGUEB_SVG_PAINTER_CLIP_PATH_DESCRIPTOR)

typedef struct _Egueb_Svg_Painter_Clip_Path {
	Egueb_Svg_Painter base;
	/* the clip path reference */
	Egueb_Svg_Reference *ref;
} Egueb_Svg_Painter_Clip_Path;

typedef struct _Egueb_Svg_Painter_Clip_Path_Class {
	Egueb_Svg_Painter_Class base;
} Egueb_Svg_Painter_Clip_Path_Class;

/*----------------------------------------------------------------------------*
 *                             Painter interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_painter_clip_path_resolve(Egueb_Svg_Painter *p,
		Egueb_Svg_Element *e)
{
	Egueb_Svg_Painter_Clip_Path *thiz;
	Enesim_Renderer *fill_renderer;

	thiz = EGUEB_SVG_PAINTER_CLIP_PATH(p);

	DBG("Resolving the clip path painter");
	p->visibility = EINA_TRUE;
	p->color = ENESIM_COLOR_FULL;
	p->fill_color = ENESIM_COLOR_FULL;
	p->draw_mode = ENESIM_SHAPE_DRAW_MODE_FILL;

	/* set the fill renderer */
	egueb_svg_reference_clip_path_resolved_renderer_get(thiz->ref, &fill_renderer);
	p->fill_renderer = fill_renderer;

	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_PAINTER_DESCRIPTOR,
		Egueb_Svg_Painter_Clip_Path, Egueb_Svg_Painter_Clip_Path_Class,
		egueb_svg_painter_clip_path);

static void _egueb_svg_painter_clip_path_class_init(void *k)
{
	Egueb_Svg_Painter_Class *klass;

	klass = EGUEB_SVG_PAINTER_CLASS(k);
	klass->resolve = _egueb_svg_painter_clip_path_resolve;
}

static void _egueb_svg_painter_clip_path_class_deinit(void *k)
{
}

static void _egueb_svg_painter_clip_path_instance_init(void *o)
{
}

static void _egueb_svg_painter_clip_path_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Svg_Painter * egueb_svg_painter_clip_path_new(Egueb_Svg_Reference *ref)
{
	Egueb_Svg_Painter_Clip_Path *thiz;
	Egueb_Svg_Painter *p;

	p = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_painter_clip_path);
	thiz = EGUEB_SVG_PAINTER_CLIP_PATH(p);
	/* FIXME shall we keep a reference ? */
	thiz->ref = ref;
	return p;
}
