/* Parses the file provided as a command line argument
 * Dumps the whole tree parsed
 */
#include "egueb_dom_mylib.h"

static void help(void)
{
	printf("Usage: egueb_dom_document01 FILE.xml\n");
	exit(1);
}

int main(int argc, char **argv)
{
	Egueb_Dom_Node *mydocument;
	Egueb_Dom_Node *root = NULL;
	Enesim_Stream *im;

	if (argc < 2) help();

	mylib_init();
	mydocument = mydocument_new();
	im = enesim_stream_file_new(argv[1], "r+");
	if (!im) return -1;

	egueb_dom_parser_parse(im, &mydocument);
	/* test that that we have a root element */
	root = egueb_dom_document_document_element_get(mydocument);
	printf("[testing] the root element is %p\n", root);

	if (root)
	{
		/* check the tree that we have parsed */
		printf("[testing] the tree hierarchy\n");
		myelement_dump(root, EINA_TRUE);
		egueb_dom_node_unref(root);
	}

	/* destroy the whole document */
	egueb_dom_node_unref(mydocument);

	mylib_shutdown();

	return 0;
}

