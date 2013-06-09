#ifndef _ESVG_ELEMENT_PATH_H_
#define _ESVG_ELEMENT_PATH_H_

EAPI Ender_Element * egueb_svg_element_path_new(void);
EAPI Eina_Bool egueb_svg_is_path(Ender_Element *e);
EAPI void egueb_svg_element_path_d_add(Ender_Element *e, const Egueb_Svg_Element_Path_Command *cmd);
EAPI void egueb_svg_element_path_d_set(Ender_Element *e, Egueb_Svg_Path_Seg_List *d);
EAPI void egueb_svg_element_path_d_get(Ender_Element *e, Egueb_Svg_Path_Seg_List **d);

#endif
