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

#ifndef _EGUEB_SVG_MAIN_H_
#define _EGUEB_SVG_MAIN_H_

/*
 * @defgroup Ender_Main_Group Main
 * @{
 */
EAPI int egueb_svg_init(void);
EAPI int egueb_svg_shutdown(void);


/* TODO add a string per element */
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_A;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_ANIMATE_TRANSFORM;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_CIRCLE;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_CLIP_PATH;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_DEFS;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_DESC;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_ELLIPSE;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_G;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_IMAGE;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_LINE;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_LINEAR_GRADIENT;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_PATH;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_PATTERN;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_POLYGON;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_POLYLINE;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_RADIAL_GRADIENT;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_RECT;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_TEXT;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_TSPAN;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_USE;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_STOP;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_SVG;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_VIDEO;
/* TODO add a string per attribute */
EAPI extern Egueb_Dom_String *EGUEB_SVG_CLASS;
EAPI extern Egueb_Dom_String *EGUEB_SVG_CLIP_PATH;
EAPI extern Egueb_Dom_String *EGUEB_SVG_CLIP_PATH_UNITS;
EAPI extern Egueb_Dom_String *EGUEB_SVG_COLOR;
EAPI extern Egueb_Dom_String *EGUEB_SVG_CX;
EAPI extern Egueb_Dom_String *EGUEB_SVG_CY;
EAPI extern Egueb_Dom_String *EGUEB_SVG_D;
EAPI extern Egueb_Dom_String *EGUEB_SVG_DISPLAY;
EAPI extern Egueb_Dom_String *EGUEB_SVG_FILL;
EAPI extern Egueb_Dom_String *EGUEB_SVG_FILL_OPACITY;
EAPI extern Egueb_Dom_String *EGUEB_SVG_FONT;
EAPI extern Egueb_Dom_String *EGUEB_SVG_FONT_FAMILY;
EAPI extern Egueb_Dom_String *EGUEB_SVG_FONT_SIZE;
EAPI extern Egueb_Dom_String *EGUEB_SVG_FONT_STYLE;
EAPI extern Egueb_Dom_String *EGUEB_SVG_FONT_WEIGHT;
EAPI extern Egueb_Dom_String *EGUEB_SVG_FX;
EAPI extern Egueb_Dom_String *EGUEB_SVG_FY;
EAPI extern Egueb_Dom_String *EGUEB_SVG_GRADIENT_UNITS;
EAPI extern Egueb_Dom_String *EGUEB_SVG_GRADIENT_TRANSFORM;
EAPI extern Egueb_Dom_String *EGUEB_SVG_HEIGHT;
EAPI extern Egueb_Dom_String *EGUEB_SVG_ID;
EAPI extern Egueb_Dom_String *EGUEB_SVG_OFFSET;
EAPI extern Egueb_Dom_String *EGUEB_SVG_OPACITY;
EAPI extern Egueb_Dom_String *EGUEB_SVG_OVERFLOW;
EAPI extern Egueb_Dom_String *EGUEB_SVG_PATTERN_UNITS;
EAPI extern Egueb_Dom_String *EGUEB_SVG_PATTERN_TRANSFORM;
EAPI extern Egueb_Dom_String *EGUEB_SVG_POINTS;
EAPI extern Egueb_Dom_String *EGUEB_SVG_R;
EAPI extern Egueb_Dom_String *EGUEB_SVG_RX;
EAPI extern Egueb_Dom_String *EGUEB_SVG_RY;
EAPI extern Egueb_Dom_String *EGUEB_SVG_SPREAD_METHOD;
EAPI extern Egueb_Dom_String *EGUEB_SVG_STOP_COLOR;
EAPI extern Egueb_Dom_String *EGUEB_SVG_STOP_OPACITY;
EAPI extern Egueb_Dom_String *EGUEB_SVG_STROKE;
EAPI extern Egueb_Dom_String *EGUEB_SVG_STROKE_DASHARRAY;
EAPI extern Egueb_Dom_String *EGUEB_SVG_STROKE_LINECAP;
EAPI extern Egueb_Dom_String *EGUEB_SVG_STROKE_LINEJOIN;
EAPI extern Egueb_Dom_String *EGUEB_SVG_STROKE_OPACITY;
EAPI extern Egueb_Dom_String *EGUEB_SVG_STROKE_WIDTH;
EAPI extern Egueb_Dom_String *EGUEB_SVG_STYLE;
EAPI extern Egueb_Dom_String *EGUEB_SVG_TEXT_ANCHOR;
EAPI extern Egueb_Dom_String *EGUEB_SVG_TRANSFORM;
EAPI extern Egueb_Dom_String *EGUEB_SVG_VIEWBOX;
EAPI extern Egueb_Dom_String *EGUEB_SVG_VISIBILITY;
EAPI extern Egueb_Dom_String *EGUEB_SVG_WIDTH;
EAPI extern Egueb_Dom_String *EGUEB_SVG_X;
EAPI extern Egueb_Dom_String *EGUEB_SVG_X1;
EAPI extern Egueb_Dom_String *EGUEB_SVG_X2;
EAPI extern Egueb_Dom_String *EGUEB_SVG_Y;
EAPI extern Egueb_Dom_String *EGUEB_SVG_Y1;
EAPI extern Egueb_Dom_String *EGUEB_SVG_Y2;

EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_ONCLICK;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_ONMOUSEDOWN;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_ONMOUSEUP;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_ONMOUSEOVER;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_ONMOUSEMOVE;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_ONMOUSEOUT;

/**
 * @}
 */


#endif
