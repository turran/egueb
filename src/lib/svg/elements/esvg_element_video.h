#ifndef _ESVG_ELEMENT_VIDEO_H_
#define _ESVG_ELEMENT_VIDEO_H_

EAPI Ender_Element * egueb_svg_element_video_new(void);
EAPI Eina_Bool egueb_svg_is_video(Ender_Element *e);
EAPI void egueb_svg_element_video_x_set(Ender_Element *e,  const Egueb_Svg_Coord *x);
EAPI void egueb_svg_element_video_x_get(Ender_Element *e, Egueb_Svg_Coord *x);
EAPI void egueb_svg_element_video_y_set(Ender_Element *e,  const Egueb_Svg_Coord *y);
EAPI void egueb_svg_element_video_y_get(Ender_Element *e, Egueb_Svg_Coord *y);
EAPI void egueb_svg_element_video_width_set(Ender_Element *e,  const Egueb_Svg_Length *width);
EAPI void egueb_svg_element_video_width_get(Ender_Element *e, Egueb_Svg_Length *width);
EAPI void egueb_svg_element_video_height_set(Ender_Element *e,  const Egueb_Svg_Length *height);
EAPI void egueb_svg_element_video_height_get(Ender_Element *e, Egueb_Svg_Length *height);
EAPI void egueb_svg_element_video_xlink_href_set(Ender_Element *e, const char *href);
EAPI void egueb_svg_element_video_xlink_href_get(Ender_Element *e, const char **href);

#endif


