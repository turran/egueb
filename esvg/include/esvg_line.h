#ifndef _ESVG_LINE_H_
#define _ESVG_LINE_H_

EAPI Ender_Element * esvg_line_new(void);
EAPI Eina_Bool esvg_is_line(Ender_Element *e);
EAPI void esvg_line_x1_set(Ender_Element *e, const Esvg_Coord *x1);
EAPI void esvg_line_x1_get(Ender_Element *e, Esvg_Coord *x1);
EAPI void esvg_line_y1_set(Ender_Element *e, const Esvg_Coord *y1);
EAPI void esvg_line_y1_get(Ender_Element *e, Esvg_Coord *y1);
EAPI void esvg_line_x2_set(Ender_Element *e, const Esvg_Coord *x2);
EAPI void esvg_line_x2_get(Ender_Element *e, Esvg_Coord *x2);
EAPI void esvg_line_y2_set(Ender_Element *e, const Esvg_Coord *y2);
EAPI void esvg_line_y2_get(Ender_Element *e, Esvg_Coord *y2);

#endif
