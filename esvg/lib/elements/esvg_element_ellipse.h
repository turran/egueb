#ifndef _ESVG_ELEMENT_ELLIPSE_H_
#define _ESVG_ELEMENT_ELLIPSE_H_

EAPI Ender_Element * esvg_element_ellipse_new(void);
EAPI Eina_Bool esvg_is_ellipse(Ender_Element *e);
EAPI void esvg_element_ellipse_cx_set(Ender_Element *e, const Esvg_Coord *cx);
EAPI void esvg_element_ellipse_cx_get(Ender_Element *e, Esvg_Coord *cx);
EAPI void esvg_element_ellipse_cy_set(Ender_Element *e, const Esvg_Coord *cy);
EAPI void esvg_element_ellipse_cy_get(Ender_Element *e, Esvg_Coord *cy);
EAPI void esvg_element_ellipse_rx_set(Ender_Element *e, const Esvg_Length *rx);
EAPI void esvg_element_ellipse_rx_get(Ender_Element *e, Esvg_Length *rx);
EAPI void esvg_element_ellipse_ry_set(Ender_Element *e, const Esvg_Length *ry);
EAPI void esvg_element_ellipse_ry_get(Ender_Element *e, Esvg_Length *ry);

#endif
