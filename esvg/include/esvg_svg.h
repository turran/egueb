#ifndef _ESVG_SVG_H_
#define _ESVG_SVG_H_

typedef const char * (*Esvg_Svg_Base_Dir_Get)(Ender_Element *e, const void *data);
typedef struct _Esvg_Svg_Application_Descriptor
{
	Esvg_Svg_Base_Dir_Get base_dir_get;
	/* function to create a surface */
	/* function to go to another url */
	/* function to get data from an external url */
} Esvg_Svg_Application_Descriptor;

EAPI Ender_Element * esvg_svg_new(void);
EAPI Eina_Bool esvg_is_svg(Ender_Element *e);
EAPI void esvg_svg_version_set(Ender_Element *e, double version);
EAPI void esvg_svg_version_get(Ender_Element *e, double *version);
EAPI void esvg_svg_x_set(Ender_Element *e, Esvg_Coord *x);
EAPI void esvg_svg_y_set(Ender_Element *e, Esvg_Coord *y);
EAPI void esvg_svg_width_set(Ender_Element *e, Esvg_Length *width);
EAPI void esvg_svg_width_get(Ender_Element *e, Esvg_Length *width);
EAPI void esvg_svg_height_set(Ender_Element *e, Esvg_Length *height);
EAPI void esvg_svg_height_get(Ender_Element *e, Esvg_Length *height);
EAPI void esvg_svg_viewbox_set(Ender_Element *e, Esvg_View_Box *vb);
EAPI void esvg_svg_container_width_set(Ender_Element *e, double container_width);
EAPI void esvg_svg_container_width_get(Ender_Element *e, double *container_width);
EAPI void esvg_svg_container_height_set(Ender_Element *e, double container_height);
EAPI void esvg_svg_container_height_get(Ender_Element *e, double *container_height);

EAPI void esvg_svg_actual_width_get(Ender_Element *e, double *actual_width);
EAPI void esvg_svg_actual_height_get(Ender_Element *e, double *actual_height);
EAPI void esvg_svg_x_dpi_set(Ender_Element *e, double x_dpi);
EAPI void esvg_svg_x_dpi_get(Ender_Element *e, double *x_dpi);
EAPI void esvg_svg_y_dpi_set(Ender_Element *e, double y_dpi);
EAPI void esvg_svg_y_dpi_get(Ender_Element *e, double *y_dpi);

EAPI Ender_Element * esvg_svg_element_find(Ender_Element *e, const char *id);

EAPI void esvg_svg_animations_pause(Ender_Element *e);
EAPI void esvg_svg_animations_unpause(Ender_Element *e);
EAPI Eina_Bool esvg_svg_animations_paused(Ender_Element *e);
EAPI void esvg_svg_animations_fps_set(Ender_Element *e, int fps);
EAPI double esvg_svg_time_get(Ender_Element *e);
EAPI void esvg_svg_time_set(Ender_Element *e, double secs);
EAPI void esvg_svg_time_tick(Ender_Element *e);

EAPI void esvg_svg_feed_mouse_move(Ender_Element *e, int x, int y);
EAPI void esvg_svg_feed_mouse_down(Ender_Element *e, int button);
EAPI void esvg_svg_feed_mouse_up(Ender_Element *e, int button);

EAPI Eina_List * esvg_svg_intersection_list_get(Ender_Element *e, Enesim_Rectangle *rect);

EAPI void esvg_svg_application_descriptor_set(Ender_Element *e, const Esvg_Svg_Application_Descriptor *descriptor, const void *data);
EAPI const char * esvg_svg_base_dir_get(Ender_Element *e);

EAPI double esvg_svg_base_font_size_get(Ender_Element *e);
EAPI void esvg_svg_base_font_size_set(Ender_Element *e, double base_font_size);

EAPI Eina_Bool esvg_svg_setup(Ender_Element *e, Enesim_Error **error);

typedef Eina_Bool (*Esvg_Svg_Damage_Cb)(Ender_Element *e, Eina_Rectangle *damage,
		void *data);
EAPI void esvg_svg_damages_get(Ender_Element *e, Esvg_Svg_Damage_Cb cb, void *data);
EAPI Eina_Bool esvg_svg_draw(Ender_Element *e, Enesim_Surface *s,
		Eina_Rectangle *clip, int x, int y, Enesim_Error **error);
EAPI Eina_Bool esvg_svg_draw_list(Ender_Element *e, Enesim_Surface *s,
		Eina_List *clips, int x, int y, Enesim_Error **error);


#endif
