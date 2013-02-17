#ifndef _ESVG_ELEMENT_PATH_H_
#define _ESVG_ELEMENT_PATH_H_

EAPI Ender_Element * esvg_element_path_new(void);
EAPI Eina_Bool esvg_is_path(Ender_Element *e);
EAPI void esvg_element_path_d_add(Ender_Element *e, const Esvg_Element_Path_Command *cmd);
EAPI void esvg_element_path_d_set(Ender_Element *e, const Eina_List *cmds);
EAPI void esvg_element_path_d_get(Ender_Element *e, const Eina_List **cmds);

#endif
