#include "Egueb_Svg.h"

static void help(void)
{
	printf("Usage: egueb_svg_test01 FILE.svg FILE.png\n");
	exit(1);
}

int main(int argc, char **argv)
{
	Egueb_Dom_Node *doc;
	Egueb_Dom_Node *topmost;
	Enesim_Image_Data *im;

	if (argc < 3) help();

	egueb_dom_init();
	egueb_svg_init();

	im = enesim_image_data_file_new(argv[1], "r+");
	if (!im)
	{
		printf("[invalid file?]\n");
		goto done;
	}
	doc = egueb_svg_document_new(NULL);
	egueb_dom_parser_parse(im, doc);
	enesim_image_data_free(im);
	egueb_dom_document_element_get(doc, &topmost);
	if (topmost)
	{
		Enesim_Surface *s;
		Enesim_Log *log = NULL;
		double w, h;

		/* first of all set the container size */
		egueb_svg_document_width_set(doc, 640);
		egueb_svg_document_height_set(doc, 480);
		/* now process the svg document */
		egueb_dom_document_process(doc);

		/* FIXME for now we create a surface like
	 	 * this, later we should get the real size
		 */
		egueb_svg_element_svg_actual_width_get(topmost, &w);
		egueb_svg_element_svg_actual_height_get(topmost, &h);
		s = enesim_surface_new(ENESIM_FORMAT_ARGB8888, lround(w), lround(h));
		if (s)
		{
			if (!egueb_svg_element_svg_draw(topmost, s,
				NULL, 0, 0, &log))
			{
				enesim_log_dump(log);
			}
			enesim_image_file_save(argv[2], s, NULL);
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

