#ifndef _ESVG_ELEMENT_RECT_H_
#define _ESVG_ELEMENT_RECT_H_

EAPI Ender_Element *esvg_element_rect_new(void);
EAPI Eina_Bool esvg_is_rect(Ender_Element *e);
EAPI void esvg_element_rect_x_set(Ender_Element *e, const Esvg_Coord *x);
EAPI void esvg_element_rect_x_get(Ender_Element *e, Esvg_Coord *x);
EAPI void esvg_element_rect_y_set(Ender_Element *e, const Esvg_Coord *y);
EAPI void esvg_element_rect_y_get(Ender_Element *e, Esvg_Coord *y);
EAPI void esvg_element_rect_width_set(Ender_Element *e, const Esvg_Length *width);
EAPI void esvg_element_rect_width_get(Ender_Element *e, Esvg_Length *width);
EAPI void esvg_element_rect_height_set(Ender_Element *e, const Esvg_Length *height);
EAPI void esvg_element_rect_height_get(Ender_Element *e, Esvg_Length *height);
EAPI void esvg_element_rect_rx_set(Ender_Element *e, const Esvg_Coord *rx);
EAPI void esvg_element_rect_rx_get(Ender_Element *e, Esvg_Coord *rx);
EAPI void esvg_element_rect_ry_set(Ender_Element *e, const Esvg_Coord *ry);
EAPI void esvg_element_rect_ry_get(Ender_Element *e, Esvg_Coord *ry);

#endif
