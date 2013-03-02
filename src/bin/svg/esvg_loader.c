/* ESVG - SVG
 * Copyright (C) 2011 Jorge Luis Zapata, Vincent Torri
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <Esvg.h>

/*
 * TODO:
 * add the dpi argument
 */
/*----------------------------------------------------------------------------*
 *                        Esvg Application Descriptor                         *
 *----------------------------------------------------------------------------*/
/* given that we only support this callback, we pass the dir name as the data */
static const char * _base_dir_get(Ender_Element *e, void *data)
{
	return data;
}

static Esvg_Element_Svg_Application_Descriptor _descriptor = {
	/* .base_dir_get 	= */ _base_dir_get,
	/* .go_to 		= */ NULL,
};

static void help(void)
{
	printf("Usage: esvg_loader FILE.svg FILE.png [width height]\n");
	printf("Where width and height defines the size of the destination image\n");
}

int main(int argc, char *argv[])
{
	Ender_Element *tag;
	Enesim_Renderer *compound;
	Enesim_Renderer *background;
	Enesim_Renderer *r;
	Enesim_Surface *s;
	Enesim_Log *err = NULL;
	Eina_Bool ret;
	char tmp[PATH_MAX];
	char *base_dir;
	int width = 640;
	int height = 480;
	double aw;
	double ah;

	if (argc < 3)
	{
		help();
		return -1;
	}

	if (argc > 3)
	{
		width = atoi(argv[3]);
 		if (argc < 5)
		{
			help();
			return -1;
		}
		height = atoi(argv[4]);
	}

	if (!esvg_init())
		return -1;

	tag = esvg_parser_load(argv[1]);
	if (!tag)
	{
		printf("Fail to parse %s\n", argv[1]);
		goto shutdown_esvg;
	}

	if (!esvg_is_svg(tag))
	{
		printf("The parsed element is not a topmost SVG element\n");
		goto shutdown_esvg;
	}
	/* set the application descriptor in case the svg needs it */
	strncpy(tmp, argv[1], PATH_MAX);
	base_dir = dirname(tmp);
	esvg_element_svg_application_descriptor_set(tag, &_descriptor, base_dir);

	/* set the final image size as the container size */
	printf("setting %d %d\n", width, height);
	esvg_element_svg_container_width_set(tag, width);
	esvg_element_svg_container_height_set(tag, height);
	/* FIXME once the renderable class is implemented
	 * get the renderer, for now test the parsing
	 */
	if (!esvg_element_svg_setup(tag, &err))
	{
		printf("The setup failed\n");
	}

	/* get the actual svg size to use */
	esvg_element_svg_actual_width_get(tag, &aw);
	esvg_element_svg_actual_height_get(tag, &ah);

	width = ceil(aw);
	height = ceil(ah);

	printf("actual size %d %d (%g %g)\n", width, height, aw, ah);
	r = esvg_renderable_renderer_get(tag);

	compound = enesim_renderer_compound_new();
	enesim_renderer_rop_set(compound, ENESIM_FILL);

	background = enesim_renderer_background_new();
	/* FIXME make this configurable */
	enesim_renderer_background_color_set(background, 0xffffffff);
	enesim_renderer_rop_set(background, ENESIM_FILL);
	enesim_renderer_compound_layer_add(compound, background);

	enesim_renderer_rop_set(r, ENESIM_BLEND);
	enesim_renderer_compound_layer_add(compound, r);

	s = enesim_surface_new(ENESIM_FORMAT_ARGB8888, width, height);
	ret = enesim_renderer_draw(compound, s, NULL, 0, 0, &err);
	if (!ret || err)
	{
		enesim_log_dump(err);
	}
	enesim_image_file_save(argv[2], s, NULL);

	enesim_surface_unref(s);
	esvg_shutdown();

	return 0;

shutdown_esvg:

	esvg_shutdown();

	return -1;
}
