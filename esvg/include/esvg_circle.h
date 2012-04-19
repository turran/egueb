#ifndef _ESVG_CIRCLE_H_
#define _ESVG_CIRCLE_H_

EAPI Ender_Element * esvg_circle_new(void);
EAPI Eina_Bool esvg_is_circle(Ender_Element *e);
EAPI void esvg_circle_cx_set(Ender_Element *e, const Esvg_Coord *cx);
EAPI void esvg_circle_cx_get(Ender_Element *e, Esvg_Coord *cx);
EAPI void esvg_circle_cy_set(Ender_Element *e, const Esvg_Coord *cy);
EAPI void esvg_circle_cy_get(Ender_Element *e, Esvg_Coord *cy);
EAPI void esvg_circle_r_set(Ender_Element *e, const Esvg_Length *r);
EAPI void esvg_circle_r_get(Ender_Element *e, Esvg_Length *r);

#endif
