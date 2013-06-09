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
#include "egueb_svg_main_private.h"
#include "egueb_svg_video_provider.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Egueb_Svg_Video_Provider
{
	Egueb_Svg_Video_Provider_Descriptor *descriptor;
	void *data;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Svg_Video_Provider * egueb_svg_video_provider_new(
		Egueb_Svg_Video_Provider_Descriptor *descriptor,
		Enesim_Renderer *image)
{
	Egueb_Svg_Video_Provider *thiz;
	void *data;

	if (!descriptor)
		return NULL;
	if (!descriptor->create)
		return NULL;

	data = descriptor->create(image);
	if (!data) return NULL;

	thiz = calloc(1, sizeof(Egueb_Svg_Video_Provider));
	thiz->data = data;
	thiz->descriptor = descriptor;

	return thiz;
}

void egueb_svg_video_provider_setup(Egueb_Svg_Video_Provider *thiz,
		const Egueb_Svg_Video_Provider_Context *ctx)
{
	if (!thiz) return;
	if (!thiz->descriptor->setup) return;
	thiz->descriptor->setup(thiz->data, ctx);
}

void egueb_svg_video_provider_free(Egueb_Svg_Video_Provider *thiz)
{
	if (!thiz) return;
	if (!thiz->descriptor->free) return;
	thiz->descriptor->free(thiz->data);
	free(thiz);
}

void egueb_svg_video_provider_play(Egueb_Svg_Video_Provider *thiz)
{
	if (!thiz) return;
	if (!thiz->descriptor->play) return;
	thiz->descriptor->play(thiz->data);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

