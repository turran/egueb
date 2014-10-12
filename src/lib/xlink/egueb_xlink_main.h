/* Egueb_Xlink - XLINK
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

#ifndef _EGUEB_XLINK_MAIN_H_
#define _EGUEB_XLINK_MAIN_H_

/**
 * @file
 * @ender_group{Egueb_Xlink_Main}
 */

/**
 * @defgroup Egueb_Xlink_Main Main
 * @ingroup Egueb_Xlink_Group
 * @{
 */

EAPI void egueb_xlink_init(void);
EAPI void egueb_xlink_shutdown(void);

/* Shared strings */
EAPI extern Egueb_Dom_String *EGUEB_XLINK_NAME_NS;
EAPI extern Egueb_Dom_String *EGUEB_XLINK_NAME_HREF;

/**
 * @}
 */

#endif

