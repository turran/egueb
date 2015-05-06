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
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_CLASS;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_CLIP_PATH;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_CLIP_PATH_UNITS;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_COLOR;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_CX;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_CY;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_D;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_DX;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_DY;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_DISPLAY;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_FILL;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_FILL_OPACITY;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_FILL_RULE;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_FONT;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_FONT_SIZE;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_FONT_STYLE;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_FONT_WEIGHT;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_FX;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_FY;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_GRADIENT_UNITS;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_GRADIENT_TRANSFORM;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_HEIGHT;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_ID;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_OFFSET;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_OPACITY;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_OVERFLOW;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_PATTERN_UNITS;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_PATTERN_TRANSFORM;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_POINTER_EVENTS;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_POINTS;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_R;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_RX;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_RY;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_SPREAD_METHOD;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_STOP_COLOR;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_STOP_OPACITY;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_STROKE;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_STROKE_DASHARRAY;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_STROKE_LINECAP;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_STROKE_LINEJOIN;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_STROKE_OPACITY;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_STROKE_WIDTH;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_STYLE;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_TEXT_ANCHOR;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_TRANSFORM;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_VIEWBOX;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_VISIBILITY;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_WIDTH;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_X;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_X1;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_X2;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_Y;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_Y1;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_Y2;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_ZOOM_AND_PAN;

EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_ONCLICK;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_ONMOUSEDOWN;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_ONMOUSEUP;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_ONMOUSEOVER;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_ONMOUSEMOVE;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_ONMOUSEOUT;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_ONFOCUSOUT;
EAPI extern Egueb_Dom_String *EGUEB_SVG_NAME_ONFOCUSIN;

/**
 * @}
 */


#endif
