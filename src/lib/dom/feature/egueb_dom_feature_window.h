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
#ifndef _EGUEB_DOM_FEATURE_WINDOW_H_
#define _EGUEB_DOM_FEATURE_WINDOW_H_

/**
 * @file
 * @ender_group{Egueb_Dom_Feature_Window_Definition}
 * @ender_group{Egueb_Dom_Feature_Window}
 */

/**
 * @defgroup Egueb_Dom_Feature_Window_Definition Definitions
 * @ingroup Egueb_Dom_Feature_Window
 * @{
 */

EAPI extern Egueb_Dom_String *EGUEB_DOM_FEATURE_WINDOW_NAME;

/**
 * The different window types
 */
typedef enum _Egueb_Dom_Feature_Window_Hint
{
	/** The window have a min/max size */
	EGUEB_DOM_FEATURE_WINDOW_HINT_MIN_MAX = 1,
	/** The window have a preferred size */
	EGUEB_DOM_FEATURE_WINDOW_HINT_PREFERRED = 2,
} Egueb_Dom_Feature_Window_Hint;

/**
 * The window size
 */
typedef struct _Egueb_Dom_Feature_Window_Hint_Data
{
	/* for the min/max hint */
	/* the min must be at least 0, the max can have a -1 and thus is not taken into account */
	int min_width;
	int min_height;
	int max_width;
	int max_height;
	/* fot the preferred hint */
	/* in case of -1, such cordinate is not taken into account */
	int pref_width;
	int pref_height;
} Egueb_Dom_Feature_Window_Hint_Data;

/**
 * @ender_name{egueb.dom.feature.window.descriptor.hints_get_cb}
 */
typedef int (*Egueb_Dom_Feature_Window_Descriptor_Hints_Get_Cb)(
		Egueb_Dom_Node *n, Egueb_Dom_Feature_Window_Hint_Data *data);

/**
 * @ender_name{egueb.dom.feature.window.descriptor.size_set_cb}
 */
typedef Eina_Bool (*Egueb_Dom_Feature_Window_Descriptor_Size_Set_Cb)(
		Egueb_Dom_Node *n, int w, int h);

#define EGUEB_DOM_FEATURE_WINDOW_DESCRIPTOR_VERSION 0
typedef struct _Egueb_Dom_Feature_Window_Descriptor
{
	int version;
	Egueb_Dom_Feature_Window_Descriptor_Hints_Get_Cb hints_get;
	Egueb_Dom_Feature_Window_Descriptor_Size_Set_Cb size_set;
} Egueb_Dom_Feature_Window_Descriptor;

/**
 * @}
 * @defgroup Egueb_Dom_Feature_Window Window
 * @brief Node window features @ender_inherits{Egueb_Dom_Feature}
 *
 * For SVG we need to have a WINDOW_RELATIVE hint, that way the size
 * of the svg depends on size of the window
 * another case would be the CONTENT_RELATIVE hint, that way the size
 * of the window depends on the size of the content
 * @ingroup Egueb_Dom_Feature
 * @{
 */

EAPI int egueb_dom_feature_window_hints_get(Egueb_Dom_Feature *f,
		Egueb_Dom_Feature_Window_Hint_Data *data);
EAPI Eina_Bool egueb_dom_feature_window_size_set(Egueb_Dom_Feature *f, int w, int h);
EAPI void egueb_dom_feature_window_view_set(Egueb_Dom_Feature *f, Egueb_Dom_Window *w);

EAPI Eina_Bool egueb_dom_feature_window_add(Egueb_Dom_Node *n,
		const Egueb_Dom_Feature_Window_Descriptor *d);

/**
 * @}
 */

#endif
