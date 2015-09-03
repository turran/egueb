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
#ifndef _EGUEB_DOM_FEATURE_MULTIMEDIA_H_
#define _EGUEB_DOM_FEATURE_MULTIMEDIA_H_

/**
 * @file
 * @ender_group{Egueb_Dom_Feature_Multimedia}
 * @ender_group{Egueb_Dom_Feature_Multimedia_Definition}
 */

/**
 * @defgroup Egueb_Dom_Feature_Multimedia_Definition Definitions
 * @ingroup Egueb_Dom_Feature_Multimedia
 * @{
 */

EAPI extern Egueb_Dom_String *EGUEB_DOM_FEATURE_MULTIMEDIA_NAME;

/**
 * @}
 * @defgroup Egueb_Dom_Feature_Multimedia Multimedia
 * @brief Node multimedia features @ender_inherits{Egueb_Dom_Feature}
 * @ingroup Egueb_Dom_Feature
 * @{
 */

EAPI Eina_Bool egueb_dom_feature_multimedia_add(Egueb_Dom_Node *n);

/**
 * @}
 */

#endif
