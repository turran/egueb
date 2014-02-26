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
#include "egueb_svg_main.h"
#include "egueb_svg_document.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/* keep track of the initialization */
static int _egueb_svg_init_count = 0;

/*----------------------------------------------------------------------------*
 *                      Implementation interface                              *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Node * _impl_document_create(void)
{
	return egueb_svg_document_new();
}

static Egueb_Dom_Implementation_Descriptor _impl_descriptor = {
	/* .document_create 	= */ _impl_document_create,
};

/*----------------------------------------------------------------------------*
 *                    Implementation source interface                         *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Implementation * _impl_source_get_by_mime(
		Egueb_Dom_String *mime)
{
	const char *str;

	str = egueb_dom_string_string_get(mime);
	if (!strcmp(str, "image/svg+xml"))
		return egueb_dom_implementation_new(&_impl_descriptor);
	else
		return NULL;
}

static Egueb_Dom_Implementation_Source_Descriptor _impl_source_descriptor = {
	/* .implementation_get_by_mime 	= */ _impl_source_get_by_mime,
};

/* our helpful strings */
static void _egueb_svg_strings_init(void)
{
	/* elements */
	EGUEB_SVG_NAME_A = egueb_dom_string_new_with_string("a");
	EGUEB_SVG_NAME_CIRCLE = egueb_dom_string_new_with_string("circle");
	EGUEB_SVG_NAME_CLIP_PATH = egueb_dom_string_new_with_string("clipPath");
	EGUEB_SVG_NAME_DEFS = egueb_dom_string_new_with_string("defs");
	EGUEB_SVG_NAME_DESC = egueb_dom_string_new_with_string("desc");
	EGUEB_SVG_NAME_ELLIPSE = egueb_dom_string_new_with_string("ellipse");
	EGUEB_SVG_NAME_G = egueb_dom_string_new_with_string("g");
	EGUEB_SVG_NAME_LINE = egueb_dom_string_new_with_string("line");
	EGUEB_SVG_NAME_LINEAR_GRADIENT = egueb_dom_string_new_with_string("linearGradient");
	EGUEB_SVG_NAME_PATH = egueb_dom_string_new_with_string("path");
	EGUEB_SVG_NAME_PATTERN = egueb_dom_string_new_with_string("pattern");
	EGUEB_SVG_NAME_POLYGON = egueb_dom_string_new_with_string("polygon");
	EGUEB_SVG_NAME_POLYLINE = egueb_dom_string_new_with_string("polyline");
	EGUEB_SVG_NAME_RADIAL_GRADIENT = egueb_dom_string_new_with_string("radialGradient");
	EGUEB_SVG_NAME_RECT = egueb_dom_string_new_with_string("rect");
	EGUEB_SVG_NAME_TEXT = egueb_dom_string_new_with_string("text");
	EGUEB_SVG_NAME_TSPAN = egueb_dom_string_new_with_string("tspan");
	EGUEB_SVG_NAME_USE = egueb_dom_string_new_with_string("use");
	EGUEB_SVG_NAME_STOP = egueb_dom_string_new_with_string("stop");
	EGUEB_SVG_NAME_SVG = egueb_dom_string_new_with_string("svg");
	/* attributes */
	EGUEB_SVG_CLASS = egueb_dom_string_new_with_string("class");
	EGUEB_SVG_CLIP_PATH = egueb_dom_string_new_with_string("clip-path");
	EGUEB_SVG_CLIP_PATH_UNITS = egueb_dom_string_new_with_string("clipPathUnits");
	EGUEB_SVG_COLOR = egueb_dom_string_new_with_string("color");
	EGUEB_SVG_CX = egueb_dom_string_new_with_string("cx");
	EGUEB_SVG_CY = egueb_dom_string_new_with_string("cy");
	EGUEB_SVG_D = egueb_dom_string_new_with_string("d");
	EGUEB_SVG_DISPLAY = egueb_dom_string_new_with_string("display");
	EGUEB_SVG_FILL = egueb_dom_string_new_with_string("fill");
	EGUEB_SVG_FILL_OPACITY = egueb_dom_string_new_with_string("fill-opacity");
	EGUEB_SVG_FONT = egueb_dom_string_new_with_string("font");
	EGUEB_SVG_FONT_FAMILY = egueb_dom_string_new_with_string("font-family");
	EGUEB_SVG_FONT_SIZE = egueb_dom_string_new_with_string("font-size");
	EGUEB_SVG_FONT_STYLE = egueb_dom_string_new_with_string("font-style");
	EGUEB_SVG_FONT_WEIGHT = egueb_dom_string_new_with_string("font-weight");
	EGUEB_SVG_FX = egueb_dom_string_new_with_string("fx");
	EGUEB_SVG_FY = egueb_dom_string_new_with_string("fy");
	EGUEB_SVG_HEIGHT = egueb_dom_string_new_with_string("height");
	EGUEB_SVG_GRADIENT_UNITS = egueb_dom_string_new_with_string("gradientUnits");
	EGUEB_SVG_GRADIENT_TRANSFORM = egueb_dom_string_new_with_string("gradientTransform");
	EGUEB_SVG_ID = egueb_dom_string_new_with_string("id");
	EGUEB_SVG_OFFSET = egueb_dom_string_new_with_string("offset");
	EGUEB_SVG_OPACITY = egueb_dom_string_new_with_string("opacity");
	EGUEB_SVG_OVERFLOW = egueb_dom_string_new_with_string("overflow");
	EGUEB_SVG_PATTERN_UNITS = egueb_dom_string_new_with_string("patternUnits");
	EGUEB_SVG_PATTERN_TRANSFORM = egueb_dom_string_new_with_string("patternTransform");
	EGUEB_SVG_POINTS = egueb_dom_string_new_with_string("points");
	EGUEB_SVG_R = egueb_dom_string_new_with_string("r");
	EGUEB_SVG_RX = egueb_dom_string_new_with_string("rx");
	EGUEB_SVG_RY = egueb_dom_string_new_with_string("ry");
	EGUEB_SVG_SPREAD_METHOD = egueb_dom_string_new_with_string("spreadMethod");
	EGUEB_SVG_STROKE = egueb_dom_string_new_with_string("stroke");
	EGUEB_SVG_STROKE_DASHARRAY = egueb_dom_string_new_with_string("stroke-dasharray");
	EGUEB_SVG_STROKE_LINECAP = egueb_dom_string_new_with_string("stroke-linecap");
	EGUEB_SVG_STROKE_LINEJOIN = egueb_dom_string_new_with_string("stroke-linejoin");
	EGUEB_SVG_STROKE_OPACITY = egueb_dom_string_new_with_string("stroke-opacity");
	EGUEB_SVG_STROKE_WIDTH = egueb_dom_string_new_with_string("stroke-width");
	EGUEB_SVG_STYLE = egueb_dom_string_new_with_string("style");
	EGUEB_SVG_STOP_COLOR = egueb_dom_string_new_with_string("stop-color");
	EGUEB_SVG_STOP_OPACITY = egueb_dom_string_new_with_string("stop-opacity");
	EGUEB_SVG_TEXT_ANCHOR = egueb_dom_string_new_with_string("text-anchor");
	EGUEB_SVG_TRANSFORM = egueb_dom_string_new_with_string("transform");
	EGUEB_SVG_VIEWBOX = egueb_dom_string_new_with_string("viewBox");
	EGUEB_SVG_VISIBILITY = egueb_dom_string_new_with_string("visibility");
	EGUEB_SVG_WIDTH = egueb_dom_string_new_with_string("width");
	EGUEB_SVG_X = egueb_dom_string_new_with_string("x");
	EGUEB_SVG_X1 = egueb_dom_string_new_with_string("x1");
	EGUEB_SVG_X2 = egueb_dom_string_new_with_string("x2");
	EGUEB_SVG_Y = egueb_dom_string_new_with_string("y");
	EGUEB_SVG_Y1 = egueb_dom_string_new_with_string("y1");
	EGUEB_SVG_Y2 = egueb_dom_string_new_with_string("y2");
}

static void _egueb_svg_strings_shutdown(void)
{
	/* elements */
	egueb_dom_string_unref(EGUEB_SVG_NAME_A);
	egueb_dom_string_unref(EGUEB_SVG_NAME_CIRCLE);
	egueb_dom_string_unref(EGUEB_SVG_NAME_CLIP_PATH);
	egueb_dom_string_unref(EGUEB_SVG_NAME_DEFS);
	egueb_dom_string_unref(EGUEB_SVG_NAME_DESC);
	egueb_dom_string_unref(EGUEB_SVG_NAME_ELLIPSE);
	egueb_dom_string_unref(EGUEB_SVG_NAME_G);
	egueb_dom_string_unref(EGUEB_SVG_NAME_LINE);
	egueb_dom_string_unref(EGUEB_SVG_NAME_LINEAR_GRADIENT);
	egueb_dom_string_unref(EGUEB_SVG_NAME_PATH);
	egueb_dom_string_unref(EGUEB_SVG_NAME_PATTERN);
	egueb_dom_string_unref(EGUEB_SVG_NAME_POLYGON);
	egueb_dom_string_unref(EGUEB_SVG_NAME_POLYLINE);
	egueb_dom_string_unref(EGUEB_SVG_NAME_RADIAL_GRADIENT);
	egueb_dom_string_unref(EGUEB_SVG_NAME_RECT);
	egueb_dom_string_unref(EGUEB_SVG_NAME_TEXT);
	egueb_dom_string_unref(EGUEB_SVG_NAME_TSPAN);
	egueb_dom_string_unref(EGUEB_SVG_NAME_USE);
	egueb_dom_string_unref(EGUEB_SVG_NAME_STOP);
	egueb_dom_string_unref(EGUEB_SVG_NAME_SVG);
	/* attributes */
	egueb_dom_string_unref(EGUEB_SVG_CLASS);
	egueb_dom_string_unref(EGUEB_SVG_CLIP_PATH);
	egueb_dom_string_unref(EGUEB_SVG_CLIP_PATH_UNITS);
	egueb_dom_string_unref(EGUEB_SVG_COLOR);
	egueb_dom_string_unref(EGUEB_SVG_CX);
	egueb_dom_string_unref(EGUEB_SVG_CY);
	egueb_dom_string_unref(EGUEB_SVG_D);
	egueb_dom_string_unref(EGUEB_SVG_DISPLAY);
	egueb_dom_string_unref(EGUEB_SVG_FILL);
	egueb_dom_string_unref(EGUEB_SVG_FILL_OPACITY);
	egueb_dom_string_unref(EGUEB_SVG_FONT);
	egueb_dom_string_unref(EGUEB_SVG_FONT_FAMILY);
	egueb_dom_string_unref(EGUEB_SVG_FONT_SIZE);
	egueb_dom_string_unref(EGUEB_SVG_FONT_STYLE);
	egueb_dom_string_unref(EGUEB_SVG_FONT_WEIGHT);
	egueb_dom_string_unref(EGUEB_SVG_FX);
	egueb_dom_string_unref(EGUEB_SVG_FY);
	egueb_dom_string_unref(EGUEB_SVG_GRADIENT_UNITS);
	egueb_dom_string_unref(EGUEB_SVG_GRADIENT_TRANSFORM);
	egueb_dom_string_unref(EGUEB_SVG_HEIGHT);
	egueb_dom_string_unref(EGUEB_SVG_ID);
	egueb_dom_string_unref(EGUEB_SVG_OFFSET);
	egueb_dom_string_unref(EGUEB_SVG_OPACITY);
	egueb_dom_string_unref(EGUEB_SVG_OVERFLOW);
	egueb_dom_string_unref(EGUEB_SVG_PATTERN_UNITS);
	egueb_dom_string_unref(EGUEB_SVG_PATTERN_TRANSFORM);
	egueb_dom_string_unref(EGUEB_SVG_POINTS);
	egueb_dom_string_unref(EGUEB_SVG_R);
	egueb_dom_string_unref(EGUEB_SVG_RX);
	egueb_dom_string_unref(EGUEB_SVG_RY);
	egueb_dom_string_unref(EGUEB_SVG_SPREAD_METHOD);
	egueb_dom_string_unref(EGUEB_SVG_STOP_COLOR);
	egueb_dom_string_unref(EGUEB_SVG_STOP_OPACITY);
	egueb_dom_string_unref(EGUEB_SVG_STROKE);
	egueb_dom_string_unref(EGUEB_SVG_STROKE_DASHARRAY);
	egueb_dom_string_unref(EGUEB_SVG_STROKE_LINECAP);
	egueb_dom_string_unref(EGUEB_SVG_STROKE_LINEJOIN);
	egueb_dom_string_unref(EGUEB_SVG_STROKE_OPACITY);
	egueb_dom_string_unref(EGUEB_SVG_STROKE_WIDTH);
	egueb_dom_string_unref(EGUEB_SVG_STYLE);
	egueb_dom_string_unref(EGUEB_SVG_TEXT_ANCHOR);
	egueb_dom_string_unref(EGUEB_SVG_TRANSFORM);
	egueb_dom_string_unref(EGUEB_SVG_VIEWBOX);
	egueb_dom_string_unref(EGUEB_SVG_VISIBILITY);
	egueb_dom_string_unref(EGUEB_SVG_WIDTH);
	egueb_dom_string_unref(EGUEB_SVG_X);
	egueb_dom_string_unref(EGUEB_SVG_X1);
	egueb_dom_string_unref(EGUEB_SVG_X2);
	egueb_dom_string_unref(EGUEB_SVG_Y);
	egueb_dom_string_unref(EGUEB_SVG_Y1);
	egueb_dom_string_unref(EGUEB_SVG_Y2);
}

static Eina_Bool _egueb_svg_dependencies_init(void)
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

	egueb_dom_init();
	egueb_smil_init();
	etch_init();
	return EINA_TRUE;

shutdown_eina:
	eina_shutdown();
	return EINA_FALSE;
}

static void _egueb_svg_dependencies_shutdown(void)
{
	etch_shutdown();
	egueb_dom_shutdown();
	egueb_smil_shutdown();
	enesim_shutdown();
	eina_shutdown();
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
int egueb_svg_log_dom_global = -1;
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/* elements */
Egueb_Dom_String *EGUEB_SVG_NAME_A;
Egueb_Dom_String *EGUEB_SVG_NAME_CIRCLE;
Egueb_Dom_String *EGUEB_SVG_NAME_CLIP_PATH;
Egueb_Dom_String *EGUEB_SVG_NAME_DEFS;
Egueb_Dom_String *EGUEB_SVG_NAME_DESC;
Egueb_Dom_String *EGUEB_SVG_NAME_ELLIPSE;
Egueb_Dom_String *EGUEB_SVG_NAME_G;
Egueb_Dom_String *EGUEB_SVG_NAME_LINE;
Egueb_Dom_String *EGUEB_SVG_NAME_LINEAR_GRADIENT;
Egueb_Dom_String *EGUEB_SVG_NAME_PATH;
Egueb_Dom_String *EGUEB_SVG_NAME_PATTERN;
Egueb_Dom_String *EGUEB_SVG_NAME_POLYGON;
Egueb_Dom_String *EGUEB_SVG_NAME_POLYLINE;
Egueb_Dom_String *EGUEB_SVG_NAME_RADIAL_GRADIENT;
Egueb_Dom_String *EGUEB_SVG_NAME_RECT;
Egueb_Dom_String *EGUEB_SVG_NAME_TEXT;
Egueb_Dom_String *EGUEB_SVG_NAME_TSPAN;
Egueb_Dom_String *EGUEB_SVG_NAME_USE;
Egueb_Dom_String *EGUEB_SVG_NAME_STOP;
Egueb_Dom_String *EGUEB_SVG_NAME_SVG;
/* attributes */
Egueb_Dom_String *EGUEB_SVG_CLASS;
Egueb_Dom_String *EGUEB_SVG_CLIP_PATH;
Egueb_Dom_String *EGUEB_SVG_CLIP_PATH_UNITS;
Egueb_Dom_String *EGUEB_SVG_COLOR;
Egueb_Dom_String *EGUEB_SVG_CX;
Egueb_Dom_String *EGUEB_SVG_CY;
Egueb_Dom_String *EGUEB_SVG_D;
Egueb_Dom_String *EGUEB_SVG_DISPLAY;
Egueb_Dom_String *EGUEB_SVG_FILL;
Egueb_Dom_String *EGUEB_SVG_FILL_OPACITY;
Egueb_Dom_String *EGUEB_SVG_FONT;
Egueb_Dom_String *EGUEB_SVG_FONT_FAMILY;
Egueb_Dom_String *EGUEB_SVG_FONT_SIZE;
Egueb_Dom_String *EGUEB_SVG_FONT_STYLE;
Egueb_Dom_String *EGUEB_SVG_FONT_WEIGHT;
Egueb_Dom_String *EGUEB_SVG_FX;
Egueb_Dom_String *EGUEB_SVG_FY;
Egueb_Dom_String *EGUEB_SVG_GRADIENT_UNITS;
Egueb_Dom_String *EGUEB_SVG_GRADIENT_TRANSFORM;
Egueb_Dom_String *EGUEB_SVG_HEIGHT;
Egueb_Dom_String *EGUEB_SVG_ID;
Egueb_Dom_String *EGUEB_SVG_OFFSET;
Egueb_Dom_String *EGUEB_SVG_OPACITY;
Egueb_Dom_String *EGUEB_SVG_OVERFLOW;
Egueb_Dom_String *EGUEB_SVG_PATTERN_UNITS;
Egueb_Dom_String *EGUEB_SVG_PATTERN_TRANSFORM;
Egueb_Dom_String *EGUEB_SVG_POINTS;
Egueb_Dom_String *EGUEB_SVG_R;
Egueb_Dom_String *EGUEB_SVG_RX;
Egueb_Dom_String *EGUEB_SVG_RY;
Egueb_Dom_String *EGUEB_SVG_STOP_COLOR;
Egueb_Dom_String *EGUEB_SVG_STOP_OPACITY;
Egueb_Dom_String *EGUEB_SVG_SPREAD_METHOD;
Egueb_Dom_String *EGUEB_SVG_STROKE;
Egueb_Dom_String *EGUEB_SVG_STROKE_DASHARRAY;
Egueb_Dom_String *EGUEB_SVG_STROKE_LINECAP;
Egueb_Dom_String *EGUEB_SVG_STROKE_LINEJOIN;
Egueb_Dom_String *EGUEB_SVG_STROKE_OPACITY;
Egueb_Dom_String *EGUEB_SVG_STROKE_WIDTH;
Egueb_Dom_String *EGUEB_SVG_STYLE;
Egueb_Dom_String *EGUEB_SVG_TEXT_ANCHOR;
Egueb_Dom_String *EGUEB_SVG_TRANSFORM;
Egueb_Dom_String *EGUEB_SVG_VIEWBOX;
Egueb_Dom_String *EGUEB_SVG_VISIBILITY;
Egueb_Dom_String *EGUEB_SVG_WIDTH;
Egueb_Dom_String *EGUEB_SVG_X;
Egueb_Dom_String *EGUEB_SVG_X1;
Egueb_Dom_String *EGUEB_SVG_X2;
Egueb_Dom_String *EGUEB_DOM_XLINK_HREF;
Egueb_Dom_String *EGUEB_SVG_Y;
Egueb_Dom_String *EGUEB_SVG_Y1;
Egueb_Dom_String *EGUEB_SVG_Y2;

/**
 * Initialize the egueb_svg library
 */
EAPI int egueb_svg_init(void)
{
	Egueb_Dom_Implementation_Source *s;

	if (++_egueb_svg_init_count != 1)
		return _egueb_svg_init_count;

	if (!_egueb_svg_dependencies_init())
		return --_egueb_svg_init_count;

	egueb_svg_log_dom_global = eina_log_domain_register("egueb_svg", 0);
	if (egueb_svg_log_dom_global < 0)
	{
		EINA_LOG_ERR("Egueb_Svg: Can not create a general log domain.");
		_egueb_svg_dependencies_shutdown();
		return --_egueb_svg_init_count;
	}
	_egueb_svg_strings_init();
	/* register our own source */
	s = egueb_dom_implementation_source_new(&_impl_source_descriptor);
	egueb_dom_registry_source_add(s);

	return _egueb_svg_init_count;
}

/**
 * Shutdown the egueb_svg library
 */
EAPI int egueb_svg_shutdown(void)
{
	if (--_egueb_svg_init_count != 0)
		return _egueb_svg_init_count;

	_egueb_svg_strings_shutdown();
	eina_log_domain_unregister(egueb_svg_log_dom_global);
        egueb_svg_log_dom_global = -1;
	/* TODO shutdown dependencies */
	_egueb_svg_dependencies_shutdown();

	return _egueb_svg_init_count;
}
