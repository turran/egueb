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
#include "egueb_svg_paint.h"
#include "egueb_svg_painter.h"
#include "egueb_svg_reference.h"
#include "egueb_svg_referenceable.h"
#include "egueb_svg_document.h"
#include "egueb_svg_paint_server.h"

#include "egueb_svg_painter_private.h"
#include "egueb_svg_reference_paint_server_private.h"
#include "egueb_dom_string_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_PAINTER_G_DESCRIPTOR egueb_svg_painter_g_descriptor_get()
#define EGUEB_SVG_PAINTER_G_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Painter_G_Class, EGUEB_SVG_PAINTER_G_DESCRIPTOR)
#define EGUEB_SVG_PAINTER_G(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Painter_G, EGUEB_SVG_PAINTER_G_DESCRIPTOR)

typedef struct _Egueb_Svg_Painter_G {
	Egueb_Svg_Painter base;
} Egueb_Svg_Painter_G;

typedef struct _Egueb_Svg_Painter_G_Class {
	Egueb_Svg_Painter_Class base;
} Egueb_Svg_Painter_G_Class;

/*----------------------------------------------------------------------------*
 *                             Painter interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_painter_g_resolve(Egueb_Svg_Painter *p,
		Egueb_Svg_Element *e)
{
	egueb_svg_painter_resolve_visibility(p, e);
	INFO("Common visibility: %d", p->visibility);
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_PAINTER_DESCRIPTOR,
		Egueb_Svg_Painter_G, Egueb_Svg_Painter_G_Class,
		egueb_svg_painter_g);

static void _egueb_svg_painter_g_class_init(void *k)
{
	Egueb_Svg_Painter_Class *klass;

	klass = EGUEB_SVG_PAINTER_CLASS(k);
	klass->resolve = _egueb_svg_painter_g_resolve;
}

static void _egueb_svg_painter_g_instance_init(void *o)
{
}

static void _egueb_svg_painter_g_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Svg_Painter * egueb_svg_painter_g_new(void)
{
	Egueb_Svg_Painter *p;

	p = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_painter_g);
	return p;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

