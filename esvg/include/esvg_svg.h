#ifndef _ESVG_SVG_H_
#define _ESVG_SVG_H_

EAPI Ender_Element * esvg_svg_new(void);
EAPI Eina_Bool esvg_is_svg(Ender_Element *e);
EAPI void esvg_svg_version_set(Ender_Element *e, double version);
EAPI void esvg_svg_version_get(Ender_Element *e, double *version);
EAPI void esvg_svg_x_set(Ender_Element *e, Esvg_Coord *x);
EAPI void esvg_svg_y_set(Ender_Element *e, Esvg_Coord *y);
EAPI void esvg_svg_width_set(Ender_Element *e, Esvg_Length *width);
EAPI void esvg_svg_height_set(Ender_Element *e, Esvg_Length *height);
EAPI void esvg_svg_viewbox_set(Ender_Element *e, Esvg_View_Box *vb);
EAPI void esvg_svg_actual_width_get(Ender_Element *e, double *actual_width);
EAPI void esvg_svg_actual_height_get(Ender_Element *e, double *actual_height);

EAPI Ender_Element * esvg_svg_element_find(Ender_Element *e, const char *id);

EAPI void esvg_svg_animations_pause(Ender_Element *e);
EAPI void esvg_svg_animations_unpause(Ender_Element *e);
EAPI Eina_Bool esvg_svg_animations_paused(Ender_Element *e);
EAPI double esvg_svg_time_get(Ender_Element *e);
EAPI void esvg_svg_time_set(Ender_Element *e, double secs);

EAPI void esvg_svg_feed_mouse_move(Ender_Element *e, int x, int y);
EAPI void esvg_svg_feed_mouse_down(Ender_Element *e);
EAPI void esvg_svg_feed_mouse_up(Ender_Element *e);

#endif
