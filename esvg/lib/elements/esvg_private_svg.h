#ifndef _ESVG_PRIVATE_SVG_H_
#define _ESVG_PRIVATE_SVG_H_

#include "esvg_script_provider.h"
#include "esvg_video_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

Etch * esvg_element_svg_etch_get(Ender_Element *e);
Ender_Element * esvg_element_svg_internal_element_find(Edom_Tag *t, const char *id);
void esvg_element_svg_element_get(Ender_Element *e, const char *uri, Ender_Element **el);
void esvg_element_svg_image_load(Ender_Element *e, const char *uri, Enesim_Surface **s, double width, double height);
Ender_Element * esvg_element_svg_svg_load(Ender_Element *e, const char *uri);
char * esvg_element_svg_uri_resolve(Ender_Element *e, const char *uri);
Enesim_Surface * esvg_element_svg_surface_new(Ender_Element *e, int w, int h);

void esvg_element_svg_internal_container_width_get(Edom_Tag *t, double *container_width);
void esvg_element_svg_internal_container_height_get(Edom_Tag *t, double *container_height);

/* application related functions */
void esvg_element_svg_go_to(Ender_Element *e, const char *uri);

/* script related functions */
Esvg_Script_Provider * esvg_element_svg_script_provider_get(Ender_Element *e, const char *type);

/* video related functions */
Esvg_Video_Provider_Descriptor * esvg_element_svg_video_provider_descriptor_get(Ender_Element *e);

void esvg_element_svg_init(void);
void esvg_element_svg_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif
