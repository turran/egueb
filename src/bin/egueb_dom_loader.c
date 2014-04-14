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

#include <Egueb_Dom.h>

/*----------------------------------------------------------------------------*
 *                                 Helpers                                    *
 *----------------------------------------------------------------------------*/
static void help(void)
{
	printf("Usage: egueb_dom_loader FILE FILE.png [width height]\n");
	printf("Where width and height defines the size of the destination image\n");
}
/*----------------------------------------------------------------------------*
 *                           Application interface                            *
 *----------------------------------------------------------------------------*/
#if 0
/* given that we only support this callback, we pass the dir name as the data */
static const char * _filename_get(void *user_data)
{
	char *file = user_data;
	return file;
}
#endif

int main(int argc, char *argv[])
{
	Egueb_Dom_Node *doc = NULL;
	Egueb_Dom_Feature *render = NULL;
	Egueb_Dom_Feature *window = NULL;
	Egueb_Dom_Feature_Window_Type type;
	Enesim_Surface *s;
	Enesim_Buffer *b;
	Enesim_Log *err = NULL;
	Enesim_Stream *stream;
	Eina_Bool ret;
	Eina_Bool needs_process = EINA_TRUE;
	int width = 640;
	int height = 480;

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

	egueb_dom_init();
	stream = enesim_stream_file_new(argv[1], "r+");
	if (!stream)
	{
		printf("Fail to load file %s\n", argv[1]);
		goto shutdown;
	}
	/* create the document */
	ret = egueb_dom_parser_parse(stream, &doc);
	enesim_stream_unref(stream);
	if (!ret)
	{
		printf("Fail to parse file %s\n", argv[1]);
		goto shutdown;
	}
	/* set the different application callbacks */
#if 0
	/* TODO for later */
	egueb_svg_document_filename_get_cb_set(doc, _filename_get, argv[1]);
#endif

	render = egueb_dom_node_feature_get(doc, EGUEB_DOM_FEATURE_RENDER_NAME, NULL);
	if (!render)
	{
		printf("Fail to get the render feature\n");
		goto shutdown;
	}
	window = egueb_dom_node_feature_get(doc, EGUEB_DOM_FEATURE_WINDOW_NAME, NULL);
	if (window)
	{
		egueb_dom_feature_window_type_get(window, &type);
		if (type == EGUEB_DOM_FEATURE_WINDOW_TYPE_MASTER)
		{
			egueb_dom_feature_window_content_size_set(window, width, height);
		}
		/* now process the document */
		egueb_dom_document_process(doc);
		egueb_dom_feature_window_content_size_get(window, &width, &height);
		if (width <= 0 || height <= 0)
		{
			printf("Invalid size of the window %d %d\n", width, height);
			goto shutdown;
		}
		needs_process = EINA_FALSE;
	}

	if (needs_process)
		egueb_dom_document_process(doc);

	s = enesim_surface_new(ENESIM_FORMAT_ARGB8888, width, height);
	egueb_dom_feature_render_draw(render, s, ENESIM_ROP_FILL, NULL, 0, 0, &err);
	if (err)
	{
		enesim_log_dump(err);
		enesim_log_delete(err);
	}
	/* now save the rendered image */
	b = enesim_surface_buffer_get(s);
	enesim_image_file_save(argv[2], b, NULL, NULL);
	enesim_buffer_unref(b);
	enesim_surface_unref(s);

shutdown:
	if (render)
		egueb_dom_feature_unref(render);
	if (window)
		egueb_dom_feature_unref(window);
	if (doc)
		egueb_dom_node_unref(doc);
	egueb_dom_shutdown();

	return 0;
}
