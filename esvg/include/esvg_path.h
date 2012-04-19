#ifndef _ESVG_PATH_H_
#define _ESVG_PATH_H_

EAPI Ender_Element * esvg_path_new(void);
EAPI Eina_Bool esvg_is_path(Ender_Element *e);
EAPI void esvg_path_d_add(Ender_Element *e, const Esvg_Path_Command *cmd);

#endif
