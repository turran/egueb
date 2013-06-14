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
extern Egueb_Dom_String *EGUEB_SVG_NAME_CIRCLE;
extern Egueb_Dom_String *EGUEB_SVG_NAME_CLIP_PATH;
extern Egueb_Dom_String *EGUEB_SVG_NAME_DEFS;
extern Egueb_Dom_String *EGUEB_SVG_NAME_DESC;
extern Egueb_Dom_String *EGUEB_SVG_NAME_ELLIPSE;
extern Egueb_Dom_String *EGUEB_SVG_NAME_G;
extern Egueb_Dom_String *EGUEB_SVG_NAME_LINE;
extern Egueb_Dom_String *EGUEB_SVG_NAME_LINEAR_GRADIENT;
extern Egueb_Dom_String *EGUEB_SVG_NAME_RADIAL_GRADIENT;
extern Egueb_Dom_String *EGUEB_SVG_NAME_RECT;
extern Egueb_Dom_String *EGUEB_SVG_NAME_TEXT;
extern Egueb_Dom_String *EGUEB_SVG_NAME_TSPAN;
extern Egueb_Dom_String *EGUEB_SVG_NAME_USE;
extern Egueb_Dom_String *EGUEB_SVG_NAME_STOP;
extern Egueb_Dom_String *EGUEB_SVG_NAME_SVG;
/* TODO add a string per attribute */
extern Egueb_Dom_String *EGUEB_SVG_CLASS;
extern Egueb_Dom_String *EGUEB_SVG_CLIP_PATH;
extern Egueb_Dom_String *EGUEB_SVG_CLIP_PATH_UNITS;
extern Egueb_Dom_String *EGUEB_SVG_COLOR;
extern Egueb_Dom_String *EGUEB_SVG_CX;
extern Egueb_Dom_String *EGUEB_SVG_CY;
extern Egueb_Dom_String *EGUEB_SVG_FILL;
extern Egueb_Dom_String *EGUEB_SVG_FILL_OPACITY;
extern Egueb_Dom_String *EGUEB_SVG_FONT_SIZE;
extern Egueb_Dom_String *EGUEB_SVG_FX;
extern Egueb_Dom_String *EGUEB_SVG_FY;
extern Egueb_Dom_String *EGUEB_SVG_GRADIENT_UNITS;
extern Egueb_Dom_String *EGUEB_SVG_GRADIENT_TRANSFORM;
extern Egueb_Dom_String *EGUEB_SVG_HEIGHT;
extern Egueb_Dom_String *EGUEB_SVG_ID;
extern Egueb_Dom_String *EGUEB_SVG_OFFSET;
extern Egueb_Dom_String *EGUEB_SVG_OPACITY;
extern Egueb_Dom_String *EGUEB_SVG_R;
extern Egueb_Dom_String *EGUEB_SVG_RX;
extern Egueb_Dom_String *EGUEB_SVG_RY;
extern Egueb_Dom_String *EGUEB_SVG_SPREAD_METHOD;
extern Egueb_Dom_String *EGUEB_SVG_STOP_COLOR;
extern Egueb_Dom_String *EGUEB_SVG_STOP_OPACITY;
extern Egueb_Dom_String *EGUEB_SVG_STROKE;
extern Egueb_Dom_String *EGUEB_SVG_STROKE_LINECAP;
extern Egueb_Dom_String *EGUEB_SVG_STROKE_LINEJOIN;
extern Egueb_Dom_String *EGUEB_SVG_STROKE_OPACITY;
extern Egueb_Dom_String *EGUEB_SVG_STROKE_WIDTH;
extern Egueb_Dom_String *EGUEB_SVG_STYLE;
extern Egueb_Dom_String *EGUEB_SVG_TRANSFORM;
extern Egueb_Dom_String *EGUEB_SVG_VIEWBOX;
extern Egueb_Dom_String *EGUEB_SVG_VISIBILITY;
extern Egueb_Dom_String *EGUEB_SVG_WIDTH;
extern Egueb_Dom_String *EGUEB_SVG_X;
extern Egueb_Dom_String *EGUEB_SVG_X1;
extern Egueb_Dom_String *EGUEB_SVG_X2;
extern Egueb_Dom_String *EGUEB_SVG_XLINK_HREF;
extern Egueb_Dom_String *EGUEB_SVG_Y;
extern Egueb_Dom_String *EGUEB_SVG_Y1;
extern Egueb_Dom_String *EGUEB_SVG_Y2;

/**
 * @}
 */


#endif
