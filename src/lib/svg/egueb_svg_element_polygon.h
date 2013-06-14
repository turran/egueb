#ifndef _ESVG_ELEMENT_POLYGON_H_
#define _ESVG_ELEMENT_POLYGON_H_

EAPI Ender_Element * egueb_svg_element_polygon_new(void);
EAPI Eina_Bool egueb_svg_is_polygon(Ender_Element *e);
EAPI void egueb_svg_element_polygon_point_add(Ender_Element *e, Egueb_Svg_Point *p);

#endif
