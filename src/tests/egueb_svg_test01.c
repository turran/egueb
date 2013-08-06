#include "Egueb_Svg.h"

static void help(void)
{
	printf("Usage: egueb_svg_test01 FILE.svg FILE.png\n");
	exit(1);
}

static const char * filename_get(void *user_data)
{
	return user_data;
}

int main(int argc, char **argv)
{
	Egueb_Dom_Node *doc;
	Egueb_Dom_Node *topmost;
	Enesim_Stream *im;

	if (argc < 3) help();

	egueb_dom_init();
	egueb_svg_init();

	im = enesim_stream_file_new(argv[1], "r+");
	if (!im)
	{
		printf("[invalid file?]\n");
		goto done;
	}
	doc = egueb_svg_document_new(NULL);
	/* set our handlers */
	egueb_svg_document_filename_get_cb_set(doc, filename_get, (void *)argv[1]);
	egueb_dom_parser_parse(im, doc);
	enesim_stream_free(im);
	egueb_dom_document_element_get(doc, &topmost);
	if (topmost)
	{
		Enesim_Surface *s;
		Enesim_Log *log = NULL;
		double w, h;

		/* first of all set the container size */
		egueb_svg_document_width_set(doc, 640);
		egueb_svg_document_height_set(doc, 480);
		/* it might be possible that the width/height is generated after
		 * the first process (because of a visible overflow for example)
		 * so we always need to do a first process
		 */
		egueb_dom_document_process(doc);

		egueb_svg_document_actual_width_get(doc, &w);
		egueb_svg_document_actual_height_get(doc, &h);

		s = enesim_surface_new(ENESIM_FORMAT_ARGB8888, lround(w), lround(h));
		if (s)
		{
			Enesim_Buffer *b;
			/* finally draw */
			if (!egueb_svg_element_svg_draw(topmost, s, ENESIM_FILL,
				NULL, 0, 0, &log))
			{
				enesim_log_dump(log);
			}
			b = enesim_surface_buffer_get(s);
			enesim_image_file_save(argv[2], b, NULL);
			enesim_buffer_unref(b);
			enesim_surface_unref(s);
		}
		else
		{
			printf("[impossible to create a surface of size %ld %ld]\n", lround(w), lround(h));
		}
		egueb_dom_node_unref(topmost);
	}
	egueb_dom_node_unref(doc);
done:
	egueb_svg_shutdown();
	egueb_dom_shutdown();

	return 0;
}

