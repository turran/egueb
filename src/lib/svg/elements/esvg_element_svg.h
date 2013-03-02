#ifndef _ESVG_ELEMENT_SVG_H_
#define _ESVG_ELEMENT_SVG_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef const char * (*Esvg_Element_Svg_Base_Dir_Get)(Ender_Element *e, void *data);

/* Function to be called whenever the svg wants to go to another uri */
typedef void (*Esvg_Element_Svg_Go_To)(Ender_Element *e, void *data, const char *uri);

/* Function to be called whenever the scripting engine calls the alert()
 * function
 */
typedef void (*Esvg_Element_Svg_Script_Alert)(Ender_Element *e, void *data,
		const char *msg);

/* Function to be called whenever a <video> tag needs to render a video */
typedef Esvg_Video_Provider_Descriptor *
		(*Esvg_Element_Svg_Video_Provider_Descriptor_Get)(Ender_Element *e,
		void *data);

/* TODO Function to be called whenever a <image> tag needs to load an image
 * resource
 * typedef Esvg_Element_Image_Provider_Descriptor * (*Esvg_Element_Svg_Image_Provider_Descriptor_Get)
 * Ender_Element *e, void *data);
 */

typedef struct _Esvg_Element_Svg_Application_Descriptor
{
	Esvg_Element_Svg_Base_Dir_Get base_dir_get;
	Esvg_Element_Svg_Go_To go_to;
	/* function to create a surface */
	/* function to get data from an external url */
	/* script based functions */
	Esvg_Element_Svg_Script_Alert script_alert;
	Esvg_Element_Svg_Video_Provider_Descriptor_Get video_provider_descriptor_get;
} Esvg_Element_Svg_Application_Descriptor;

EAPI Ender_Element * esvg_element_svg_new(void);
EAPI Eina_Bool esvg_is_svg(Ender_Element *e);
EAPI void esvg_element_svg_version_set(Ender_Element *e, double version);
EAPI void esvg_element_svg_version_get(Ender_Element *e, double *version);
EAPI void esvg_element_svg_x_set(Ender_Element *e, Esvg_Coord *x);
EAPI void esvg_element_svg_y_set(Ender_Element *e, Esvg_Coord *y);
EAPI void esvg_element_svg_width_set(Ender_Element *e, Esvg_Length *width);
EAPI void esvg_element_svg_width_get(Ender_Element *e, Esvg_Length *width);
EAPI void esvg_element_svg_height_set(Ender_Element *e, Esvg_Length *height);
EAPI void esvg_element_svg_height_get(Ender_Element *e, Esvg_Length *height);
EAPI void esvg_element_svg_viewbox_set(Ender_Element *e, Esvg_Rect *v);
EAPI void esvg_element_svg_content_script_type_set(Ender_Element *e, const char *v);
EAPI void esvg_element_svg_content_script_type_get(Ender_Element *e, const char **v);

EAPI void esvg_element_svg_container_width_set(Ender_Element *e, double container_width);
EAPI void esvg_element_svg_container_width_get(Ender_Element *e, double *container_width);
EAPI void esvg_element_svg_container_height_set(Ender_Element *e, double container_height);
EAPI void esvg_element_svg_container_height_get(Ender_Element *e, double *container_height);
EAPI void esvg_element_svg_actual_width_get(Ender_Element *e, double *actual_width);
EAPI void esvg_element_svg_actual_height_get(Ender_Element *e, double *actual_height);
EAPI void esvg_element_svg_x_dpi_set(Ender_Element *e, double x_dpi);
EAPI void esvg_element_svg_x_dpi_get(Ender_Element *e, double *x_dpi);
EAPI void esvg_element_svg_y_dpi_set(Ender_Element *e, double y_dpi);
EAPI void esvg_element_svg_y_dpi_get(Ender_Element *e, double *y_dpi);

EAPI Ender_Element * esvg_element_svg_element_find(Ender_Element *e, const char *id);

EAPI void esvg_element_svg_animations_pause(Ender_Element *e);
EAPI void esvg_element_svg_animations_unpause(Ender_Element *e);
EAPI Eina_Bool esvg_element_svg_animations_paused(Ender_Element *e);
EAPI void esvg_element_svg_animations_fps_set(Ender_Element *e, int fps);
EAPI double esvg_element_svg_time_get(Ender_Element *e);
EAPI void esvg_element_svg_time_set(Ender_Element *e, double secs);
EAPI void esvg_element_svg_time_tick(Ender_Element *e);

EAPI void esvg_element_svg_script_alert(Ender_Element *e, const char *msg);

EAPI void esvg_element_svg_feed_mouse_move(Ender_Element *e, int x, int y);
EAPI void esvg_element_svg_feed_mouse_down(Ender_Element *e, int button);
EAPI void esvg_element_svg_feed_mouse_up(Ender_Element *e, int button);

EAPI Eina_List * esvg_element_svg_intersection_list_get(Ender_Element *e, Enesim_Rectangle *rect);

EAPI void esvg_element_svg_application_descriptor_set(Ender_Element *e, const Esvg_Element_Svg_Application_Descriptor *descriptor, void *data);
EAPI const char * esvg_element_svg_base_dir_get(Ender_Element *e);

EAPI double esvg_element_svg_base_font_size_get(Ender_Element *e);
EAPI void esvg_element_svg_base_font_size_set(Ender_Element *e, double base_font_size);

EAPI Eina_Bool esvg_element_svg_setup(Ender_Element *e, Enesim_Log **error);

typedef Eina_Bool (*Esvg_Element_Svg_Damage_Cb)(Ender_Element *e, Eina_Rectangle *damage,
		void *data);
EAPI void esvg_element_svg_damages_get(Ender_Element *e, Esvg_Element_Svg_Damage_Cb cb, void *data);
EAPI Eina_Bool esvg_element_svg_draw(Ender_Element *e, Enesim_Surface *s,
		Eina_Rectangle *clip, int x, int y, Enesim_Log **error);
EAPI Eina_Bool esvg_element_svg_draw_list(Ender_Element *e, Enesim_Surface *s,
		Eina_List *clips, int x, int y, Enesim_Log **error);

#ifdef __cplusplus
}
#endif

#endif
