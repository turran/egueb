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

#include <Etex.h>
#include <Ender.h>


#if BUILD_ESVG_VIDEO
#include <gst/gst.h>
#endif

#include "esvg_types.h"

#include "esvg_private_main.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_context.h"
#include "esvg_private_element.h"
#include "esvg_private_a.h"
#include "esvg_private_referenceable.h"
#include "esvg_private_renderable.h"
#include "esvg_private_paint_server.h"
#include "esvg_private_gradient.h"
#include "esvg_private_animation.h"
#include "esvg_private_animate_base.h"
#include "esvg_private_clip_path.h"

#include "esvg_main.h"
#include "esvg_element.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/* simple struct to initialize all the domains easily */
struct log
{
	int *d;
	const char *name;
} logs[] = {
	{ &esvg_log_a,			"esvg_a" },
	{ &esvg_log_animate,		"esvg_animate" },
	{ &esvg_log_animate_base,	"esvg_animate_base" },
	{ &esvg_log_animate_transform,	"esvg_animate_transform" },
	{ &esvg_log_animation,		"esvg_animation" },
	{ &esvg_log_clone,		"esvg_clone" },
	{ &esvg_log_circle,		"esvg_circle" },
	{ &esvg_log_clip_path,		"esvg_clip_path" },
	{ &esvg_log_element,		"esvg_element" },
	{ &esvg_log_ellipse,		"esvg_ellipse" },
	{ &esvg_log_gradient,		"esvg_gradient" },
	{ &esvg_log_image,		"esvg_image" },
	{ &esvg_log_line,		"esvg_line" },
	{ &esvg_log_parser, 		"esvg_parser" },
	{ &esvg_log_path,		"esvg_path" },
	{ &esvg_log_polygon,		"esvg_polygon" },
	{ &esvg_log_polyline,		"esvg_polyline" },
	{ &esvg_log_rect,		"esvg_rect" },
	{ &esvg_log_renderable,		"esvg_renderable" },
	{ &esvg_log_set,		"esvg_set" },
	{ &esvg_log_stop,		"esvg_stop" },
	{ &esvg_log_svg,		"esvg_svg" },
	{ &esvg_log_text,		"esvg_text" },
	{ &esvg_log_type,		"esvg_type" },
	{ &esvg_log_use,		"esvg_use" },
	{ &esvg_log_video,		"esvg_video" },
};

/* keep track of the initialization */
static int _esvg_init_count = 0;

void esvg_types_init(void);

static void _register_enders(void)
{
	/* register the dependency */
	/* FIXME do we actually need this ? */
	ender_loader_load("edom");
	/* the types first */
	esvg_types_init();
	/* now the abstracts */
	esvg_element_init();
	esvg_referenceable_init();
	esvg_renderable_init();
	esvg_paint_server_init();
	esvg_gradient_init();
	/* now the classes */
	esvg_line_init();
	esvg_linear_gradient_init();
	esvg_radial_gradient_init();
	esvg_circle_init();
	esvg_ellipse_init();
	esvg_path_init();
	esvg_polygon_init();
	esvg_polyline_init();
	esvg_g_init();
	esvg_svg_init();
	esvg_rect_init();
	esvg_stop_init();
	esvg_use_init();
	esvg_defs_init();
	esvg_a_init();
	esvg_clip_path_init();
	esvg_style_init();
	esvg_image_init();
	esvg_text_init();
	esvg_video_init();

	esvg_animation_init();
	esvg_animate_base_init();
	esvg_animate_init();
	esvg_animate_transform_init();
	esvg_set_init();
}

/* FIXME the constructor should be done per namespace, not generic */
static void _constructor_callback(Ender_Element *e, void *data)
{
	if (!esvg_is_element(e))
		return;
	esvg_element_initialize(e);
}

static Eina_Bool _esvg_ender_init(void)
{
	Ender_Descriptor *tag_descriptor;
	Ender_Namespace *edom;

	ender_init(NULL, NULL);
	ender_element_new_listener_add(_constructor_callback, NULL);
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
	EDOM_ATTRIBUTE = ender_descriptor_property_get(tag_descriptor, "attribute");
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
	ender_element_new_listener_remove(_constructor_callback, NULL);
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

	if (!etex_init())
	{
		fprintf(stderr, "Esvg: Etex init failed");
		goto shutdown_enesim;
	}

	if (!emage_init())
	{
		fprintf(stderr, "Esvg: Emage init failed");
		goto shutdown_etex;
	}

	if (!edom_init())
	{
		fprintf(stderr, "Esvg: Emage init failed");
		goto shutdown_emage;
	}

	etch_init();

	if (!_esvg_ender_init())
	{
		fprintf(stderr, "Esvg: Ender init failed");
		goto shutdown_edom;
	}
#if BUILD_ESVG_VIDEO
	gst_init(NULL, NULL);
#endif
	return EINA_TRUE;

shutdown_edom:
	edom_shutdown();
shutdown_emage:
	etch_shutdown();
	emage_shutdown();
shutdown_etex:
	etex_shutdown();
shutdown_enesim:
	enesim_shutdown();
shutdown_eina:
	eina_shutdown();
	return EINA_FALSE;
}

static void _esvg_dependencies_shutdown(void)
{
#if BUILD_ESVG_VIDEO
	gst_deinit();
#endif
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* Shortcuts for edom node properties */
Ender_Property *EDOM_ATTRIBUTE = NULL;
Ender_Property *EDOM_CDATA = NULL;
Ender_Property *EDOM_TOPMOST = NULL;
Ender_Property *EDOM_CHILD = NULL;
Ender_Property *EDOM_PARENT = NULL;
Ender_Property *EDOM_TEXT = NULL;
/* The log domaings */
int esvg_log_a	 		= -1;
int esvg_log_animate 		= -1;
int esvg_log_animate_base 	= -1;
int esvg_log_animate_transform 	= -1;
int esvg_log_animation	 	= -1;
int esvg_log_clone 		= -1;
int esvg_log_circle 		= -1;
int esvg_log_clip_path 		= -1;
int esvg_log_element 		= -1;
int esvg_log_ellipse 		= -1;
int esvg_log_gradient 		= -1;
int esvg_log_image 		= -1;
int esvg_log_line 		= -1;
int esvg_log_parser 		= -1;
int esvg_log_path 		= -1;
int esvg_log_polygon 		= -1;
int esvg_log_polyline 		= -1;
int esvg_log_rect 		= -1;
int esvg_log_renderable		= -1;
int esvg_log_set 		= -1;
int esvg_log_stop 		= -1;
int esvg_log_svg		= -1;
int esvg_log_text       	= -1;
int esvg_log_type       	= -1;
int esvg_log_use       		= -1;
int esvg_log_video      	= -1;

/* The ender wrapper */
Ender_Namespace * esvg_namespace_get(void)
{
	static Ender_Namespace *namespace = NULL;

	if (!namespace)
	{
		namespace = ender_namespace_new("esvg", 0);
	}
	return namespace;
}
#include "generated/esvg_generated_types.c"
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

/**
 * Initialize the esvg library
 */
EAPI int esvg_init(void)
{
	int i;
	int count;

	if (++_esvg_init_count != 1)
		return _esvg_init_count;

	if (!_esvg_dependencies_init())
		return --_esvg_init_count;

	/* register every domain here */
	count = sizeof(logs)/sizeof(struct log);
	for (i = 0; i < count; i++)
	{
		struct log *l = &logs[i];
		*l->d = eina_log_domain_register(l->name, ESVG_LOG_COLOR_DEFAULT);
		if (*l->d < 0)
		{
			fprintf(stderr, "Esvg: Can not create domaing log '%s'", l->name);
			goto log_error;
		}
	}

	return _esvg_init_count;
log_error:
	for (; i >= 0; i--)
	{
		struct log *l = &logs[i];
		eina_log_domain_unregister(*l->d);
	}

	return --_esvg_init_count;
}

/**
 * Shutdown the esvg library
 */
EAPI int esvg_shutdown(void)
{
	int i;
	int count;

	if (--_esvg_init_count != 0)
		return _esvg_init_count;

	/* remove every log */
	count = sizeof(logs)/sizeof(struct log);
	for (i = 0; i < count; i++)
	{
		struct log *l = &logs[i];
		eina_log_domain_unregister(*l->d);
	}
	/* TODO shutdown dependencies */
	_esvg_ender_shutdown();
	edom_shutdown();
	etex_shutdown();
	enesim_shutdown();
	eina_shutdown();

	return _esvg_init_count;
}
