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

#ifndef _EGUEB_SVG_ELEMENT_SVG_H_
#define _EGUEB_SVG_ELEMENT_SVG_H_

/**
 * @file
 * @listgroup{Egueb_Svg_Element_Svg}
 */

/**
 * @defgroup Egueb_Svg_Element_Svg SVG
 * TODO make it inherit from svg container
 * @brief SVG element @inherits{Egueb_Svg_Element}
 * @ingroup Egueb_Svg_Renderable_Container_Group
 * @{
 */

EAPI Egueb_Dom_Node * egueb_svg_element_svg_new(void);
EAPI Eina_Bool egueb_svg_element_svg_draw(Egueb_Dom_Node *n, Enesim_Surface *s,
		Enesim_Rop rop, Eina_Rectangle *clip, int x, int y,
		Enesim_Log **error);
EAPI Eina_Bool egueb_svg_element_svg_draw_list(Egueb_Dom_Node *n,
		Enesim_Surface *s, Enesim_Rop rop, Eina_List *clips, int x,
		int y, Enesim_Log **error);

EAPI void egueb_svg_element_svg_animations_pause(Egueb_Dom_Node *n);
EAPI void egueb_svg_element_svg_animations_unpause(Egueb_Dom_Node *n);
EAPI Eina_Bool egueb_svg_element_svg_animations_paused(Egueb_Dom_Node *n);
EAPI double egueb_svg_element_svg_time_get(Egueb_Dom_Node *n);
EAPI void egueb_svg_element_svg_time_set(Egueb_Dom_Node *n, double secs);

EAPI void egueb_svg_element_svg_width_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length_Animated *width);
EAPI void egueb_svg_element_svg_height_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length_Animated *height);

EAPI void egueb_svg_element_svg_current_scale_set(Egueb_Dom_Node *n,
		double scale);
EAPI double egueb_svg_element_svg_current_scale_get(Egueb_Dom_Node *n);
EAPI void egueb_svg_element_svg_current_translate_set(Egueb_Dom_Node *n,
		Egueb_Svg_Point *p);
EAPI void egueb_svg_element_svg_current_translate_get(Egueb_Dom_Node *n,
		Egueb_Svg_Point *p);

EAPI Egueb_Svg_Point * egueb_svg_element_svg_svg_point_create(Egueb_Dom_Node *n);

#if 0
typedef const char * (*Egueb_Svg_Element_Svg_Base_Dir_Get)(Ender_Element *e, void *data);

/* Function to be called whenever the svg wants to go to another uri */
typedef void (*Egueb_Svg_Element_Svg_Go_To)(Ender_Element *e, void *data, const char *uri);

/* Function to be called whenever the scripting engine calls the alert()
 * function
 */
typedef void (*Egueb_Svg_Element_Svg_Script_Alert)(Ender_Element *e, void *data,
		const char *msg);

/* Function to be called whenever a <video> tag needs to render a video */
typedef Egueb_Svg_Video_Provider_Descriptor *
		(*Egueb_Svg_Element_Svg_Video_Provider_Descriptor_Get)(Ender_Element *e,
		void *data);

/* TODO Function to be called whenever a <image> tag needs to load an image
 * resource
 * typedef Egueb_Svg_Element_Image_Provider_Descriptor * (*Egueb_Svg_Element_Svg_Image_Provider_Descriptor_Get)
 * Ender_Element *e, void *data);
 */

typedef struct _Egueb_Svg_Element_Svg_Application_Descriptor
{
	Egueb_Svg_Element_Svg_Base_Dir_Get base_dir_get;
	Egueb_Svg_Element_Svg_Go_To go_to;
	/* function to create a surface */
	/* function to get data from an external url */
	/* script based functions */
	Egueb_Svg_Element_Svg_Script_Alert script_alert;
	Egueb_Svg_Element_Svg_Video_Provider_Descriptor_Get video_provider_descriptor_get;
} Egueb_Svg_Element_Svg_Application_Descriptor;

EAPI Ender_Element * egueb_svg_element_svg_new(void);
EAPI Eina_Bool egueb_svg_is_svg(Ender_Element *e);
EAPI void egueb_svg_element_svg_version_set(Ender_Element *e, double version);
EAPI void egueb_svg_element_svg_version_get(Ender_Element *e, double *version);
EAPI void egueb_svg_element_svg_x_set(Ender_Element *e, Egueb_Svg_Coord *x);
EAPI void egueb_svg_element_svg_y_set(Ender_Element *e, Egueb_Svg_Coord *y);
EAPI void egueb_svg_element_svg_width_set(Ender_Element *e, Egueb_Svg_Length *width);
EAPI void egueb_svg_element_svg_width_get(Ender_Element *e, Egueb_Svg_Length *width);
EAPI void egueb_svg_element_svg_height_set(Ender_Element *e, Egueb_Svg_Length *height);
EAPI void egueb_svg_element_svg_height_get(Ender_Element *e, Egueb_Svg_Length *height);
EAPI void egueb_svg_element_svg_viewbox_set(Ender_Element *e, Egueb_Svg_Rect *v);
EAPI void egueb_svg_element_svg_content_script_type_set(Ender_Element *e, const char *v);
EAPI void egueb_svg_element_svg_content_script_type_get(Ender_Element *e, const char **v);

EAPI void egueb_svg_element_svg_container_width_set(Ender_Element *e, double container_width);
EAPI void egueb_svg_element_svg_container_width_get(Ender_Element *e, double *container_width);
EAPI void egueb_svg_element_svg_container_height_set(Ender_Element *e, double container_height);
EAPI void egueb_svg_element_svg_container_height_get(Ender_Element *e, double *container_height);
EAPI void egueb_svg_element_svg_x_dpi_set(Ender_Element *e, double x_dpi);
EAPI void egueb_svg_element_svg_x_dpi_get(Ender_Element *e, double *x_dpi);
EAPI void egueb_svg_element_svg_y_dpi_set(Ender_Element *e, double y_dpi);
EAPI void egueb_svg_element_svg_y_dpi_get(Ender_Element *e, double *y_dpi);

EAPI Ender_Element * egueb_svg_element_svg_element_find(Ender_Element *e, const char *id);

EAPI void egueb_svg_element_svg_script_alert(Ender_Element *e, const char *msg);

EAPI Eina_List * egueb_svg_element_svg_intersection_list_get(Ender_Element *e, Enesim_Rectangle *rect);

EAPI void egueb_svg_element_svg_application_descriptor_set(Ender_Element *e, const Egueb_Svg_Element_Svg_Application_Descriptor *descriptor, void *data);
EAPI const char * egueb_svg_element_svg_base_dir_get(Ender_Element *e);

EAPI double egueb_svg_element_svg_base_font_size_get(Ender_Element *e);
EAPI void egueb_svg_element_svg_base_font_size_set(Ender_Element *e, double base_font_size);

EAPI Eina_Bool egueb_svg_element_svg_setup(Ender_Element *e, Enesim_Log **error);

#ifdef __cplusplus
}
#endif
#endif

/**
 * @}
 */

#endif
