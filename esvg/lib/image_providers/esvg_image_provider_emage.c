/* Esvg - SVG
 * Copyright (C) 2012 Jorge Luis Zapata
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
#include "Esvg.h"
#include <gst/gst.h>
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_COLOR_DEFAULT EINA_COLOR_ORANGE
/* Whenever a file needs to generate a log, it must declare this first */

#ifdef ERR
# undef ERR
#endif
#define ERR(...) EINA_LOG_DOM_ERR(_esvg_element_image_provider_enesim_image_log, __VA_ARGS__)

#ifdef WARN
# undef WARN
#endif
#define WARN(...) EINA_LOG_DOM_WARN(_esvg_element_image_provider_enesim_image_log, __VA_ARGS__)

#ifdef INFO
# undef INFO
#endif
#define INFO(...) EINA_LOG_DOM_INFO(_esvg_element_image_provider_enesim_image_log, __VA_ARGS__)

#ifdef DBG
# undef DBG
#endif
#define DBG(...) EINA_LOG_DOM_DBG(_esvg_element_image_provider_enesim_image_log, __VA_ARGS__)

/* FIXME we still need to register the log domain */
static int _esvg_element_image_provider_enesim_image_log = -1;

typedef struct _Esvg_Element_Image_Emage
{
	Enesim_Renderer *image;
} Esvg_Element_Image_Emage;
/*----------------------------------------------------------------------------*
 *                           The Image interface                              *
 *----------------------------------------------------------------------------*/
static void * _esvg_element_image_enesim_image_create(Enesim_Renderer *image)
{
	Esvg_Element_Image_Emage *thiz;

	thiz = calloc(1, sizeof(Esvg_Element_Image_Emage));
	thiz->image = image;

	return thiz;
}

static void _esvg_element_image_enesim_image_free(void *data)
{
	Esvg_Element_Image_Emage *thiz = data;

	enesim_renderer_unref(thiz->image);
	free(thiz);
}

static void _esvg_element_image_enesim_image_load(void *data)
{
	Esvg_Element_Image_Emage *thiz = data;
	Enesim_Surface *s = NULL;
	Eina_Bool ret;

	ret = enesim_image_file_load("TODO", &s, ENESIM_FORMAT_ARGB8888, NULL, NULL);
#if 0
	if (!ret)
	{
		Eina_Error err;

		err = eina_error_get();
		ERR("Image '%s' failed to load with error: %s", name, eina_error_msg_get(err));
		return;
	}
#endif
	enesim_renderer_image_src_set(thiz->image, s);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Esvg_Element_Image_Provider_Descriptor esvg_element_image_provider_enesim_image_descriptor = {
	/* .create 	= */ _esvg_element_image_enesim_image_create,
	/* .free 	= */ _esvg_element_image_enesim_image_free,
	/* .load 	= */ _esvg_element_image_enesim_image_load,
};
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

