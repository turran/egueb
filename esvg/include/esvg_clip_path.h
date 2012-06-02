#ifndef _ESVG_CLIP_PATH_
#define _ESVG_CLIP_PATH_

EAPI Ender_Element * esvg_clip_path_new(void);
EAPI Eina_Bool esvg_is_clip_path(Ender_Element *e);
EAPI void esvg_clip_path_units_set(Ender_Element *e, Esvg_Clip_Path_Units units);
EAPI void esvg_clip_path_units_get(Ender_Element *e, Esvg_Clip_Path_Units *units);

#endif
