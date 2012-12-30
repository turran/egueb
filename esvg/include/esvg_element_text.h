#ifndef _ESVG_ELEMENT_TEXT_H_
#define _ESVG_ELEMENT_TEXT_H_

EAPI Ender_Element *esvg_element_text_new(void);
EAPI Eina_Bool esvg_is_text(Ender_Element *e);
EAPI void esvg_element_text_x_set(Ender_Element *e, const Esvg_Coord *x);
EAPI void esvg_element_text_x_get(Ender_Element *e, Esvg_Coord *x);
EAPI void esvg_element_text_y_set(Ender_Element *e, const Esvg_Coord *y);
EAPI void esvg_element_text_y_get(Ender_Element *e, Esvg_Coord *y);
EAPI void esvg_element_text_text_set(Ender_Element *e, const char *text);
EAPI void esvg_element_text_text_get(Ender_Element *e, const char **text);

#endif

