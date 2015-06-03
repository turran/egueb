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
#ifndef _EGUEB_DOM_FEATURE_H_
#define _EGUEB_DOM_FEATURE_H_

/**
 * @defgroup Egueb_Dom_Feature Features
 * @brief Node features
 *
 * Node features are ways to inform the application developer about
 * the features a particular node implements.
 * @ingroup Egueb_Dom_Group
 * @{
 */
typedef struct _Egueb_Dom_Feature Egueb_Dom_Feature;

EAPI Enesim_Object_Descriptor * egueb_dom_feature_descriptor_get(void);

EAPI Egueb_Dom_Feature * egueb_dom_feature_ref(Egueb_Dom_Feature *thiz);
EAPI void egueb_dom_feature_unref(Egueb_Dom_Feature *thiz);

/**
 * @}
 */

#endif
