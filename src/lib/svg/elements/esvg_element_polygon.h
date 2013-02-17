#ifndef _ESVG_ELEMENT_POLYGON_H_
#define _ESVG_ELEMENT_POLYGON_H_

EAPI Ender_Element * esvg_element_polygon_new(void);
EAPI Eina_Bool esvg_is_polygon(Ender_Element *e);
EAPI void esvg_element_polygon_point_add(Ender_Element *e, Esvg_Point *p);

#endif
