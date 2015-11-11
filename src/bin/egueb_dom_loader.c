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
int main(int argc, char *argv[])
{
	Egueb_Dom_Node *doc = NULL;
	Egueb_Dom_Node *topmost = NULL;
	Egueb_Dom_Feature *render = NULL;
	Egueb_Dom_Feature *window = NULL;
	Egueb_Dom_Feature *io = NULL;
	Egueb_Dom_Feature_Window_Hint_Data wdata;
	Enesim_Surface *s;
	Enesim_Buffer *b;
	Enesim_Log *err = NULL;
	Enesim_Stream *stream;
	Eina_Bool ret;
	Eina_Error error;
	Eina_Bool user_provided_size = EINA_FALSE;
	int width = 640;
	int height = 480;
	int whints;

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
		user_provided_size = EINA_TRUE;
	}

	egueb_dom_init();
	stream = enesim_stream_file_new(argv[1], "r");
	if (!stream)
	{
		printf("Fail to load file %s\n", argv[1]);
		goto shutdown;
	}
	/* create the document */
	ret = egueb_dom_parser_parse(stream, &doc);
	if (!ret)
	{
		printf("Fail to parse file %s\n", argv[1]);
		goto shutdown;
	}
	topmost = egueb_dom_document_document_element_get(doc);
	if (!topmost)
	{
		printf("No topmost node\n");
		goto shutdown;
	}

	render = egueb_dom_node_feature_get(topmost, EGUEB_DOM_FEATURE_RENDER_NAME, NULL);
	if (!render)
	{
		printf("Fail to get the render feature\n");
		goto shutdown;
	}
	io = egueb_dom_node_feature_get(topmost, EGUEB_DOM_FEATURE_IO_NAME, NULL);
	if (io)
	{
		egueb_dom_feature_io_default_enable(io, EINA_TRUE);
	}

	window = egueb_dom_node_feature_get(topmost, EGUEB_DOM_FEATURE_WINDOW_NAME, NULL);
	if (window)
	{
		whints = egueb_dom_feature_window_hints_get(window, &wdata);
		if (!user_provided_size && (whints & EGUEB_DOM_FEATURE_WINDOW_HINT_PREFERRED))
		{
			if (wdata.pref_width != -1)
				width = wdata.pref_width;
			if (wdata.pref_height != -1)
				height = wdata.pref_height;
		}

		if (whints & EGUEB_DOM_FEATURE_WINDOW_HINT_MIN_MAX)
		{
			if (wdata.min_width != -1 && width < wdata.min_width)
				width = wdata.min_width;
			if (wdata.min_height != -1 && height < wdata.min_height)
				height = wdata.min_height;

			if (wdata.max_width != -1 && width > wdata.max_width)
				width = wdata.max_width;
			if (wdata.max_height != -1 && height > wdata.max_height)
				height = wdata.max_height;
		}
		egueb_dom_feature_window_size_set(window, width, height);
	}

	egueb_dom_document_process(doc);

	s = enesim_surface_new(ENESIM_FORMAT_ARGB8888, width, height);
	egueb_dom_feature_render_draw(render, s, ENESIM_ROP_FILL, NULL, 0, 0, &err);
	if (err)
	{
		enesim_log_dump(err);
		enesim_log_unref(err);
	}
	/* now save the rendered image */
	b = enesim_surface_buffer_get(s);
	if (!enesim_image_file_save(argv[2], b, NULL, &error))
	{
		printf("Error while saving '%s'\n", eina_error_msg_get(error));
	}
	enesim_buffer_unref(b);
	enesim_surface_unref(s);

shutdown:
	if (render)
		egueb_dom_feature_unref(render);
	if (window)
		egueb_dom_feature_unref(window);
	if (io)
		egueb_dom_feature_unref(io);
	if (topmost)
		egueb_dom_node_unref(topmost);
	if (doc)
		egueb_dom_node_unref(doc);
	egueb_dom_shutdown();

	return 0;
}
