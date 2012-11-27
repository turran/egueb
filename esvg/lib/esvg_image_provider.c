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
#include "esvg_private_main.h"
#include "esvg_image_provider.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Esvg_Image_Provider
{
	Esvg_Image_Provider_Descriptor *descriptor;
	void *data;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Esvg_Image_Provider * esvg_image_provider_new(
		Esvg_Image_Provider_Descriptor *descriptor,
		Enesim_Renderer *image)
{
	Esvg_Image_Provider *thiz;
	void *data;

	if (!descriptor)
		return NULL;
	if (!descriptor->create)
		return NULL;

	data = descriptor->create(image);
	if (!data) return NULL;

	thiz = calloc(1, sizeof(Esvg_Image_Provider));
	thiz->data = data;
	thiz->descriptor = descriptor;

	return thiz;
}

void esvg_image_provider_free(Esvg_Image_Provider *thiz)
{
	if (!thiz) return;
	if (!thiz->descriptor->free) return;
	thiz->descriptor->free(thiz->data);
	free(thiz);
}

void esvg_image_provider_load(Esvg_Image_Provider *thiz)
{
	if (!thiz) return;
	if (!thiz->descriptor->load) return;
	thiz->descriptor->load(thiz->data);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
