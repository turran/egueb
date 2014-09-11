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
#ifndef _EGUEB_CSS_MAIN_H_

EAPI extern Egueb_Dom_String *EGUEB_CSS_STYLE;
EAPI extern Egueb_Dom_String *EGUEB_CSS_NAME_FONT;
EAPI extern Egueb_Dom_String *EGUEB_CSS_NAME_FONT_FAMILY;
EAPI extern Egueb_Dom_String *EGUEB_CSS_NAME_FONT_SIZE;
EAPI extern Egueb_Dom_String *EGUEB_CSS_NAME_FONT_STYLE;
EAPI extern Egueb_Dom_String *EGUEB_CSS_NAME_FONT_VARIANT;
EAPI extern Egueb_Dom_String *EGUEB_CSS_NAME_FONT_WEIGHT;

EAPI int egueb_css_init(void);
EAPI int egueb_css_shutdown(void);

#endif
