#ifndef _ESVG_GRADIENT_H_
#define _ESVG_GRADIENT_H_

EAPI Eina_Bool esvg_is_gradient(Ender_Element *e);
EAPI void esvg_gradient_stop_add(Ender_Element *e, Esvg_Gradient_Stop *s);
EAPI void esvg_gradient_stop_get(Ender_Element *e, const Eina_List **l);
EAPI void esvg_gradient_units_set(Ender_Element *e, Esvg_Gradient_Units units);
EAPI void esvg_gradient_units_get(Ender_Element *e, Esvg_Gradient_Units *units);
EAPI Eina_Bool esvg_gradient_units_is_set(Ender_Element *e);
EAPI void esvg_gradient_transform_set(Ender_Element *e, const Enesim_Matrix *transform);
EAPI void esvg_gradient_transform_get(Ender_Element *e, Enesim_Matrix *transform);
EAPI Eina_Bool esvg_gradient_transform_is_set(Ender_Element *e);
EAPI void esvg_gradient_spread_method_set(Ender_Element *e, Esvg_Spread_Method spread_method);
EAPI void esvg_gradient_spread_method_get(Ender_Element *e, Esvg_Spread_Method *spread_method);

#endif
