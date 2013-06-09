/* Egueb
 * Copyright (C) 2011 - 2013 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _EGUEB_SVG_ELEMENT_SVG_PRIVATE_H_
#define _EGUEB_SVG_ELEMENT_SVG_PRIVATE_H_

#include "egueb_svg_renderable.h"
#include "egueb_svg_renderable_container_private.h"

#if 0
#include "egueb_svg_script_provider.h"
#include "egueb_svg_video_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

Etch * egueb_svg_element_svg_etch_get(Ender_Element *e);
Ender_Element * egueb_svg_element_svg_internal_element_find(Egueb_Dom_Tag *t, const char *id);
void egueb_svg_element_svg_element_get(Ender_Element *e, const char *uri, Ender_Element **el);
void egueb_svg_element_svg_image_load(Ender_Element *e, const char *uri, Enesim_Surface **s, double width, double height);
Ender_Element * egueb_svg_element_svg_svg_load(Ender_Element *e, const char *uri);
char * egueb_svg_element_svg_uri_resolve(Ender_Element *e, const char *uri);
Enesim_Surface * egueb_svg_element_svg_surface_new(Ender_Element *e, int w, int h);

void egueb_svg_element_svg_internal_container_width_get(Egueb_Dom_Tag *t, double *container_width);
void egueb_svg_element_svg_internal_container_height_get(Egueb_Dom_Tag *t, double *container_height);

/* application related functions */
void egueb_svg_element_svg_go_to(Ender_Element *e, const char *uri);

/* script related functions */
Egueb_Svg_Script_Provider * egueb_svg_element_svg_script_provider_get(Ender_Element *e, const char *type);

/* video related functions */
Egueb_Svg_Video_Provider_Descriptor * egueb_svg_element_svg_video_provider_descriptor_get(Ender_Element *e);

void egueb_svg_element_svg_init(void);
void egueb_svg_element_svg_shutdown(void);

#ifdef __cplusplus
}
#endif
#endif
#endif
