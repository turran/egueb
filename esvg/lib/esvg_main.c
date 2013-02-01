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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <Enesim.h>
#include <Ender.h>
#include <Edom.h>


#include "esvg_types.h"

#include "esvg_private_main.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_context.h"
#include "esvg_private_element.h"
#include "esvg_private_a.h"
#include "esvg_private_animate.h"
#include "esvg_private_animate_transform.h"
#include "esvg_private_referenceable.h"
#include "esvg_private_renderable.h"
#include "esvg_private_paint_server.h"
#include "esvg_private_gradient.h"
#include "esvg_private_animation.h"
#include "esvg_private_animate_base.h"
#include "esvg_private_clip_path.h"
#include "esvg_private_script.h"
#include "esvg_private_svg.h"
#include "esvg_private_script_provider.h"
#include "esvg_private_clone.h"
#include "esvg_private_parser.h"
#include "esvg_private_types.h"
#include "esvg_private_video.h"
#include "esvg_private_video_provider.h"

#include "esvg_main.h"
#include "esvg_element.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

/* keep track of the initialization */
static int _esvg_init_count = 0;

static void _register_enders(void)
{
	/* register the dependency */
	/* FIXME do we actually need this ? */
	ender_loader_load("edom");
	/* first the types */
	esvg_length_init();
	esvg_color_init();
	esvg_point_init();
	esvg_paint_init();
	esvg_number_init();
	esvg_boolean_init();
	esvg_rect_init();
	esvg_matrix_init();
	esvg_string_init();
	/* now the abstracts */
	esvg_element_init();
	esvg_referenceable_init();
	esvg_renderable_init();
	esvg_paint_server_init();
	esvg_element_gradient_init();
	/* now the classes */
	esvg_element_line_init();
	esvg_element_linear_gradient_init();
	esvg_element_radial_gradient_init();
	esvg_element_circle_init();
	esvg_element_ellipse_init();
	esvg_element_path_init();
	esvg_element_polygon_init();
	esvg_element_polyline_init();
	esvg_element_g_init();
	esvg_element_svg_init();
	esvg_element_rect_init();
	esvg_element_stop_init();
	esvg_element_use_init();
	esvg_element_defs_init();
	esvg_element_a_init();
	esvg_element_clip_path_init();
	esvg_element_style_init();
	esvg_element_image_init();
	esvg_element_text_init();
	esvg_element_video_init();
	esvg_element_script_init();

	esvg_element_animation_init();
	esvg_animate_base_init();
	esvg_element_animate_init();
	esvg_element_animate_transform_init();
	esvg_element_set_init();

	esvg_script_provider_init();
}

static void _namespace_constructor_callback(Ender_Element *e, void *data)
{
	esvg_element_initialize(e);
}

static Eina_Bool _esvg_ender_init(void)
{
	Ender_Descriptor *tag_descriptor;
	Ender_Namespace *edom;

	ender_init();
	edom = ender_namespace_find("edom", 0);
	if (!edom)
	{
		EINA_LOG_ERR("Impossible to find the 'Edom' namespace");
		goto namespace_err;
	}
	_register_enders();

	tag_descriptor = ender_namespace_descriptor_find(edom, "tag");
	if (!tag_descriptor)
	{
		EINA_LOG_ERR("Impossible to find the 'Edom:Tag' descriptor");
		goto descriptor_err;
	}
	/* get the needed properties to avoid the hash lookup */
	EDOM_PARENT = ender_descriptor_property_get(tag_descriptor, "parent");
	EDOM_CHILD = ender_descriptor_property_get(tag_descriptor, "child");
	EDOM_TOPMOST = ender_descriptor_property_get(tag_descriptor, "topmost");
	EDOM_CDATA = ender_descriptor_property_get(tag_descriptor, "cdata");
	EDOM_TEXT = ender_descriptor_property_get(tag_descriptor, "text");


	return EINA_TRUE;
namespace_err:
	// ender_namespace_unref(ns);
descriptor_err:
	ender_shutdown();

	return EINA_FALSE;
}

static void _esvg_ender_shutdown(void)
{
	ender_shutdown();
}

static Eina_Bool _esvg_dependencies_init(void)
{
	if (!eina_init())
	{
		fprintf(stderr, "Esvg: Eina init failed");
		return EINA_FALSE;
	}

	if (!enesim_init())
	{
		fprintf(stderr, "Esvg: Enesim init failed");
		goto shutdown_eina;
	}

	edom_init();
	etch_init();

	if (!_esvg_ender_init())
	{
		fprintf(stderr, "Esvg: Ender init failed");
		goto shutdown_etch;
	}

	esvg_clone_init();
	esvg_parser_init();
	esvg_type_init();

	return EINA_TRUE;

shutdown_etch:
	etch_shutdown();
	edom_shutdown();
shutdown_enesim:
	enesim_shutdown();
shutdown_eina:
	eina_shutdown();
	return EINA_FALSE;
}

static void _esvg_dependencies_shutdown(void)
{
	esvg_type_shutdown();
	esvg_parser_shutdown();
	esvg_clone_shutdown();
#if BUILD_ESVG_ELEMENT_VIDEO
	gst_deinit();
#endif
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* Shortcuts for edom node properties */
Ender_Property *EDOM_CDATA = NULL;
Ender_Property *EDOM_TOPMOST = NULL;
Ender_Property *EDOM_CHILD = NULL;
Ender_Property *EDOM_PARENT = NULL;
Ender_Property *EDOM_TEXT = NULL;

/* The ender wrapper */
Ender_Namespace * esvg_namespace_get(void)
{
	static Ender_Namespace *namespace = NULL;

	if (!namespace)
	{
		namespace = ender_namespace_new("esvg", 0);
		ender_namespace_element_new_listener_add(namespace,
				_namespace_constructor_callback, NULL);
	}
	return namespace;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

/**
 * Initialize the esvg library
 */
EAPI int esvg_init(void)
{
	if (++_esvg_init_count != 1)
		return _esvg_init_count;

	if (!_esvg_dependencies_init())
		return --_esvg_init_count;

	return _esvg_init_count;
}

/**
 * Shutdown the esvg library
 */
EAPI int esvg_shutdown(void)
{
	if (--_esvg_init_count != 0)
		return _esvg_init_count;

	/* TODO shutdown dependencies */
	_esvg_dependencies_shutdown();
	_esvg_ender_shutdown();
	edom_shutdown();
	enesim_shutdown();
	eina_shutdown();

	return _esvg_init_count;
}
