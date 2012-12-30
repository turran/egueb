#ifndef _ESVG_ELEMENT_IMAGE_H_
#define _ESVG_ELEMENT_IMAGE_H_

EAPI Ender_Element * esvg_element_image_new(void);
EAPI Eina_Bool esvg_is_image(Ender_Element *e);
EAPI void esvg_element_image_x_set(Ender_Element *e,  const Esvg_Coord *x);
EAPI void esvg_element_image_x_get(Ender_Element *e, Esvg_Coord *x);
EAPI void esvg_element_image_y_set(Ender_Element *e,  const Esvg_Coord *y);
EAPI void esvg_element_image_y_get(Ender_Element *e, Esvg_Coord *y);
EAPI void esvg_element_image_width_set(Ender_Element *e,  const Esvg_Length *width);
EAPI void esvg_element_image_width_get(Ender_Element *e, Esvg_Length *width);
EAPI void esvg_element_image_height_set(Ender_Element *e,  const Esvg_Length *height);
EAPI void esvg_element_image_height_get(Ender_Element *e, Esvg_Length *height);
EAPI void esvg_element_image_xlink_href_set(Ender_Element *e, const char *href);
EAPI void esvg_element_image_xlink_href_get(Ender_Element *e, const char **href);

#endif

