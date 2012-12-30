#ifndef _ESVG_ELEMENT_LINEAR_GRADIENT_H_
#define _ESVG_ELEMENT_LINEAR_GRADIENT_H_

EAPI Ender_Element * esvg_element_linear_gradient_new(void);
EAPI Eina_Bool esvg_is_linear_gradient(Ender_Element *e);
EAPI void esvg_element_linear_gradient_x1_set(Ender_Element *e, const Esvg_Coord *x1);
EAPI void esvg_element_linear_gradient_x1_get(Ender_Element *e, Esvg_Coord *x1);
EAPI Eina_Bool esvg_element_linear_gradient_x1_is_set(Ender_Element *e);
EAPI void esvg_element_linear_gradient_y1_set(Ender_Element *e, const Esvg_Coord *y1);
EAPI void esvg_element_linear_gradient_y1_get(Ender_Element *e, Esvg_Coord *y1);
EAPI Eina_Bool esvg_element_linear_gradient_y1_is_set(Ender_Element *e);
EAPI void esvg_element_linear_gradient_x2_set(Ender_Element *e, const Esvg_Coord *x2);
EAPI void esvg_element_linear_gradient_x2_get(Ender_Element *e, Esvg_Coord *x2);
EAPI Eina_Bool esvg_element_linear_gradient_x2_is_set(Ender_Element *e);
EAPI void esvg_element_linear_gradient_y2_set(Ender_Element *e, const Esvg_Coord *y2);
EAPI void esvg_element_linear_gradient_y2_get(Ender_Element *e, Esvg_Coord *y2);
EAPI Eina_Bool esvg_element_linear_gradient_y2_is_set(Ender_Element *e);

#endif
