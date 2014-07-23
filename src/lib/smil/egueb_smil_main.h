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

#ifndef _EGUEB_SMIL_MAIN_H_
#define _EGUEB_SMIL_MAIN_H_

EAPI void egueb_smil_init(void);
EAPI void egueb_smil_shutdown(void);

EAPI extern Egueb_Dom_String *EGUEB_SMIL_NAME_SET;
EAPI extern Egueb_Dom_String *EGUEB_SMIL_NAME_ANIMATE;

EAPI extern Egueb_Dom_String *EGUEB_SMIL_ADDITIVE;
EAPI extern Egueb_Dom_String *EGUEB_SMIL_ATTRIBUTE_NAME;
EAPI extern Egueb_Dom_String *EGUEB_SMIL_BEGIN;
EAPI extern Egueb_Dom_String *EGUEB_SMIL_DUR;
EAPI extern Egueb_Dom_String *EGUEB_SMIL_END;
EAPI extern Egueb_Dom_String *EGUEB_SMIL_FILL;
EAPI extern Egueb_Dom_String *EGUEB_SMIL_TO;
EAPI extern Egueb_Dom_String *EGUEB_SMIL_FROM;
EAPI extern Egueb_Dom_String *EGUEB_SMIL_BY;
EAPI extern Egueb_Dom_String *EGUEB_SMIL_VALUES;
EAPI extern Egueb_Dom_String *EGUEB_SMIL_REPEAT_COUNT;
EAPI extern Egueb_Dom_String *EGUEB_SMIL_NAME_CALC_MODE;

#endif
