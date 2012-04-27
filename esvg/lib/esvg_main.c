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

#include "esvg_types.h"

#include "esvg_private_main.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_element.h"
#include "esvg_private_a.h"

#include "esvg_main.h"
#include "esvg_element.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static int _esvg_init_count = 0;

static void _register_enders(void *data)
{
	/* register the dependency */
	ender_loader_load("edom");
	/* the abstracts first */
	esvg_element_init();
	esvg_referenceable_init();
	esvg_renderable_init();
	esvg_instantiable_init();
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
	Ender_Namespace *tag_ns;

	ender_loader_registry_callback_add(_register_enders, NULL);
	ender_init(NULL, NULL);
	ender_element_new_listener_add(_constructor_callback, NULL);
	tag_ns = ender_namespace_find("edom");
	if (!tag_ns)
	{
		ERR("Impossible to find the 'Edom' namespace");
		goto namespace_err;
	}

	tag_descriptor = ender_namespace_descriptor_find(tag_ns, "tag");
	if (!tag_descriptor)
	{
		ERR("Impossible to find the 'Edom:Tag' descriptor");
		goto descriptor_err;
	}
	/* get the needed properties to avoid the hash lookup */
	EDOM_ATTRIBUTE = ender_descriptor_property_get(tag_descriptor, "attribute");
	EDOM_PARENT = ender_descriptor_property_get(tag_descriptor, "parent");
	EDOM_CHILD = ender_descriptor_property_get(tag_descriptor, "child");
	EDOM_TOPMOST = ender_descriptor_property_get(tag_descriptor, "topmost");
	EDOM_CDATA = ender_descriptor_property_get(tag_descriptor, "cdata");


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
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Property *EDOM_ATTRIBUTE = NULL;
Ender_Property *EDOM_CDATA = NULL;
Ender_Property *EDOM_TOPMOST = NULL;
Ender_Property *EDOM_CHILD = NULL;
Ender_Property *EDOM_PARENT = NULL;
int esvg_log_dom_global = -1;

Ender_Namespace * esvg_namespace_get(void)
{
	static Ender_Namespace *namespace = NULL;

	if (!namespace)
	{
		namespace = ender_namespace_new("esvg");
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

	if (!eina_init())
	{
		fprintf(stderr, "Esvg: Eina init failed");
		return --_esvg_init_count;
	}

	esvg_log_dom_global = eina_log_domain_register("esvg", ESVG_LOG_COLOR_DEFAULT);
	if (esvg_log_dom_global < 0)
	{
		EINA_LOG_ERR("Esvg: Can not create a general log domain.");
		goto shutdown_eina;
	}

	if (!enesim_init())
	{
		ERR("Enesim init failed");
		goto unregister_log_domain;
	}

	if (!etex_init())
	{
		ERR("Etex init failed");
		goto shutdown_enesim;
	}

	if (!_esvg_ender_init())
	{
		ERR("Ender init failed");
		goto shutdown_ender;
	}

	return _esvg_init_count;

shutdown_ender:
	etex_shutdown();
shutdown_enesim:
	enesim_shutdown();
unregister_log_domain:
	eina_log_domain_unregister(esvg_log_dom_global);
	esvg_log_dom_global = -1;
shutdown_eina:
	eina_shutdown();
	return --_esvg_init_count;
}

/**
 * Shutdown the esvg library
 */
EAPI int esvg_shutdown(void)
{
	if (--_esvg_init_count != 0)
		return _esvg_init_count;

	_esvg_ender_shutdown();
	etex_shutdown();
	enesim_shutdown();
	eina_log_domain_unregister(esvg_log_dom_global);
	esvg_log_dom_global = -1;
	eina_shutdown();

	return _esvg_init_count;
}
