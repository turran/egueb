#ifndef _ESVG_ELEMENT_SET_H_
#define _ESVG_ELEMENT_SET_H_

EAPI Ender_Element * esvg_element_set_new(void);
EAPI void esvg_element_set_to_set(Ender_Element *e, const char *v);
EAPI void esvg_element_set_to_get(Ender_Element *e, const char **v);

#endif
