#ifndef _ESVG_A_H_
#define _ESVG_A_H_

EAPI Ender_Element * esvg_a_new(void);
EAPI Eina_Bool esvg_is_a(Ender_Element *e);
EAPI void esvg_a_href_set(Ender_Element *e, const char *href);

#endif