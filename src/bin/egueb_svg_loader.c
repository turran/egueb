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

#include <Egueb_Svg.h>

/*----------------------------------------------------------------------------*
 *                                 Helpers                                    *
 *----------------------------------------------------------------------------*/
static void help(void)
{
	printf("Usage: egueb_svg_loader FILE.svg FILE.png [width height]\n");
	printf("Where width and height defines the size of the destination image\n");
}
/*----------------------------------------------------------------------------*
 *                           Application interface                            *
 *----------------------------------------------------------------------------*/
/* given that we only support this callback, we pass the dir name as the data */
static const char * _filename_get(void *user_data)
{
	char *file = user_data;
	return file;
}

int main(int argc, char *argv[])
{
	Egueb_Dom_Node *doc = NULL;
	Egueb_Dom_Node *svg = NULL;
	Enesim_Surface *s;
	Enesim_Buffer *b;
	Enesim_Log *err = NULL;
	Enesim_Stream *stream;
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

	if (!egueb_svg_init())
		return -1;

	stream = enesim_stream_file_new(argv[1], "r+");
	if (!stream)
	{
		printf("Fail to parse %s\n", argv[1]);
		goto shutdown_esvg;
	}
	/* create the document */
	doc = egueb_svg_document_new(NULL);
	/* set the different application callbacks */
	egueb_svg_document_filename_get_cb_set(doc, _filename_get, argv[1]);

	egueb_dom_parser_parse(stream, doc);
	enesim_stream_unref(stream);

	svg = egueb_dom_document_element_get(doc);
	if (!svg)
	{
		printf("The parsed element is not a topmost SVG element\n");
		goto shutdown_esvg;
	}

	/* set the final image size as the container size */
	egueb_svg_document_width_set(doc, width);
	egueb_svg_document_height_set(doc, height);
	/* now process the document */
	egueb_dom_document_process(doc);

	s = enesim_surface_new(ENESIM_FORMAT_ARGB8888, width, height);
	egueb_svg_element_svg_draw(svg, s, ENESIM_ROP_FILL, NULL, 0, 0, &err);
	if (err)
	{
		enesim_log_dump(err);
		enesim_log_delete(err);
	}
	b = enesim_surface_buffer_get(s);
	enesim_image_file_save(argv[2], b, NULL);
	enesim_buffer_unref(b);
	enesim_surface_unref(s);

shutdown_esvg:
	if (doc)
		egueb_dom_node_unref(doc);
	if (svg)
		egueb_dom_node_unref(svg);
	egueb_svg_shutdown();

	return 0;
}
