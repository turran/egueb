#ifndef _ESVG_ELEMENT_VIDEO_H_
#define _ESVG_ELEMENT_VIDEO_H_

EAPI Ender_Element * esvg_element_video_new(void);
EAPI Eina_Bool esvg_is_video(Ender_Element *e);
EAPI void esvg_element_video_x_set(Ender_Element *e,  const Esvg_Coord *x);
EAPI void esvg_element_video_x_get(Ender_Element *e, Esvg_Coord *x);
EAPI void esvg_element_video_y_set(Ender_Element *e,  const Esvg_Coord *y);
EAPI void esvg_element_video_y_get(Ender_Element *e, Esvg_Coord *y);
EAPI void esvg_element_video_width_set(Ender_Element *e,  const Esvg_Length *width);
EAPI void esvg_element_video_width_get(Ender_Element *e, Esvg_Length *width);
EAPI void esvg_element_video_height_set(Ender_Element *e,  const Esvg_Length *height);
EAPI void esvg_element_video_height_get(Ender_Element *e, Esvg_Length *height);
EAPI void esvg_element_video_xlink_href_set(Ender_Element *e, const char *href);
EAPI void esvg_element_video_xlink_href_get(Ender_Element *e, const char **href);

#endif


