#ifndef _ESVG_ELEMENT_A_H_
#define _ESVG_ELEMENT_A_H_

EAPI Ender_Element * egueb_svg_element_a_new(void);
EAPI Eina_Bool egueb_svg_is_a(Ender_Element *e);
EAPI void egueb_svg_element_a_xlink_href_set(Ender_Element *e, const char *href);

#endif
