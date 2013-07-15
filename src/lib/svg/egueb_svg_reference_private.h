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

#ifndef _EGUEB_SVG_REFERENCE_PRIVATE_H_
#define _EGUEB_SVG_REFERENCE_PRIVATE_H_

struct _Egueb_Svg_Reference
{
	Enesim_Object_Instance base;
	Egueb_Dom_Node *referencer;
	Egueb_Dom_Node *referenceable;
	Egueb_Svg_Reference_Destroy_Cb destroy_cb;
	/* we dont store anything here, is part of the
	 * inherited classes to decide  what to store
	 * a renderer in case of the linear and radial
	 * gradients. And a node which holds the cloned
	 * <g> node in case of a pattern or a clipPath
	 */
};

/* TODO we need function to inform the referencer and referenceable
 * whenever the other end has been unlinked
typedef void (*Egueb_Svg_Reference_Referencer_Unlink)(Egueb_Svg_Reference *thiz);
typedef void (*Egueb_Svg_Reference_Referenceable_Unlink)(Egueb_Svg_Reference *thiz);
 */

typedef Eina_Bool (*Egueb_Svg_Reference_Process)(Egueb_Svg_Reference *thiz);
typedef void (*Egueb_Svg_Reference_Setup)(Egueb_Svg_Reference *thiz);

typedef struct _Egueb_Svg_Reference_Class
{
	Enesim_Object_Class base;
	Egueb_Svg_Reference_Setup setup;
	Egueb_Svg_Reference_Process process;
} Egueb_Svg_Reference_Class;

#define EGUEB_SVG_REFERENCE_DESCRIPTOR egueb_svg_reference_descriptor_get()
Enesim_Object_Descriptor * egueb_svg_reference_descriptor_get(void);

#define EGUEB_SVG_REFERENCE_CLASS_GET(o) EGUEB_SVG_REFERENCE_CLASS(		\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SVG_REFERENCE_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,		\
		Egueb_Svg_Reference_Class, EGUEB_SVG_REFERENCE_DESCRIPTOR)
#define EGUEB_SVG_REFERENCE(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Svg_Reference, EGUEB_SVG_REFERENCE_DESCRIPTOR)

/* reference */
void egueb_svg_reference_init(Egueb_Svg_Reference *thiz,
		Egueb_Dom_Node *referenceable, Egueb_Dom_Node *referencer);

/* The clip path reference */
Egueb_Svg_Reference * egueb_svg_reference_clip_path_new(void);
void egueb_svg_reference_clip_path_g_get(Egueb_Svg_Reference *r, Egueb_Dom_Node **g);
void egueb_svg_reference_clip_path_renderer_get(Egueb_Svg_Reference *r,
		Enesim_Renderer **ren);
void egueb_svg_reference_clip_path_resolved_renderer_get(Egueb_Svg_Reference *r,
		Enesim_Renderer **ren);

#endif
