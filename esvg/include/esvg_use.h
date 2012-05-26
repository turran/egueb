#ifndef _ESVG_USE_H_
#define _ESVG_USE_H_

EAPI Ender_Element * esvg_use_new(void);
EAPI Eina_Bool esvg_is_use(Ender_Element *e);
EAPI void esvg_use_x_set(Ender_Element *e, const Esvg_Coord *x);
EAPI void esvg_use_x_get(Ender_Element *e, Esvg_Coord *x);
EAPI void esvg_use_y_set(Ender_Element *e, const Esvg_Coord *y);
EAPI void esvg_use_y_get(Ender_Element *e, Esvg_Coord *y);
EAPI void esvg_use_width_set(Ender_Element *e, const Esvg_Length *width);
EAPI void esvg_use_width_get(Ender_Element *e, Esvg_Length *width);
EAPI void esvg_use_height_set(Ender_Element *e, const Esvg_Length *height);
EAPI void esvg_use_height_get(Ender_Element *e, Esvg_Length *height);
EAPI void esvg_use_link_set(Ender_Element *e, const char *link);
EAPI void esvg_use_link_get(Ender_Element *e, const char **link);

#endif
