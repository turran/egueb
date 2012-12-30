#ifndef _ESVG_ELEMENT_RADIAL_GRADIENT_H_
#define _ESVG_ELEMENT_RADIAL_GRADIENT_H_

EAPI Ender_Element * esvg_element_radial_gradient_new(void);
EAPI Eina_Bool esvg_is_radial_gradient(Ender_Element *e);
EAPI void esvg_element_radial_gradient_cx_set(Ender_Element *e, const Esvg_Coord *cx);
EAPI void esvg_element_radial_gradient_cx_get(Ender_Element *e, Esvg_Coord *cx);
EAPI Eina_Bool esvg_element_radial_gradient_cx_is_set(Ender_Element *e);
EAPI void esvg_element_radial_gradient_cy_set(Ender_Element *e, const Esvg_Coord *cy);
EAPI void esvg_element_radial_gradient_cy_get(Ender_Element *e, Esvg_Coord *cy);
EAPI Eina_Bool esvg_element_radial_gradient_cy_is_set(Ender_Element *e);
EAPI void esvg_element_radial_gradient_fx_set(Ender_Element *e, const Esvg_Coord *fx);
EAPI void esvg_element_radial_gradient_fx_get(Ender_Element *e, Esvg_Coord *fx);
EAPI Eina_Bool esvg_element_radial_gradient_fx_is_set(Ender_Element *e);
EAPI void esvg_element_radial_gradient_fy_set(Ender_Element *e, const Esvg_Coord *fy);
EAPI void esvg_element_radial_gradient_fy_get(Ender_Element *e, Esvg_Coord *fy);
EAPI Eina_Bool esvg_element_radial_gradient_fy_is_set(Ender_Element *e);
EAPI void esvg_element_radial_gradient_r_set(Ender_Element *e, const Esvg_Length *rad);
EAPI void esvg_element_radial_gradient_r_get(Ender_Element *e, Esvg_Length *rad);
EAPI Eina_Bool esvg_element_radial_gradient_r_is_set(Ender_Element *e);

#endif

