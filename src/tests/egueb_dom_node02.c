/* Reads a xml file and then tries to clone the topmost element
 */
#include "egueb_dom_mylib.h"

static void help(void)
{
	printf("Usage: egueb_dom_node02 FILE.xml\n");
	exit(1);
}

int main(int argc, char **argv)
{
	Egueb_Dom_Node *mydocument;
	Egueb_Dom_Node *root = NULL;
	Egueb_Dom_Node *clone = NULL;
	Egueb_Dom_Node *node = NULL;
	Egueb_Dom_Node *new_node;
	Enesim_Image_Data *im;

	if (argc < 2) help();

	mylib_init();
	mydocument = mydocument_new();
	im = enesim_image_data_file_new(argv[1], "r+");
	egueb_dom_parser_parse(im, mydocument);
	enesim_image_data_free(im);

	egueb_dom_document_element_get(mydocument, &root);
	/* check the tree that we have parsed */
	printf("[testing] original tree\n");
	myelement_dump(root, EINA_TRUE);

	/* clone the root element deeply */
	printf("[testing] Cloned tree\n");
	egueb_dom_node_clone(root, EINA_TRUE, EINA_TRUE, &clone);
	myelement_dump(clone, EINA_TRUE);

	/* given that the clone is live, modify the root element
	 * and check if the cloned element has the same value
	 */
	printf("[testing] Modifying the original node\n");
	myelement_prop1_set(root, egueb_dom_string_new_with_string("modifying"));
	/* check the new state */
	printf("[testing] Cloned tree\n");
	myelement_dump(clone, EINA_TRUE);

	/* remove the last node from the original tree */
	printf("[testing] Removing the last node from the original tree\n");
	egueb_dom_node_child_last_get(root, &node);
	egueb_dom_node_child_remove(root, node);
	myelement_dump(clone, EINA_TRUE);

	/* append the node again */
	printf("[testing] Adding the last node from the original tree\n");
	egueb_dom_node_child_append(root, egueb_dom_node_ref(node));
	myelement_dump(clone, EINA_TRUE);

	printf("[testing] Adding the previois las node before the current last node in the original tree\n");
	new_node = myelement_new();
	myelement_prop1_set(new_node, egueb_dom_string_new_with_string("new node"));
	egueb_dom_node_insert_before(root, new_node, node);
	egueb_dom_node_unref(node);
	myelement_dump(clone, EINA_TRUE);
	myelement_dump(root, EINA_TRUE);

	/* destroy the clone */
	printf("[testing] Destroy the clone tree\n");
	egueb_dom_node_unref(clone);

	/* destroy the root and the document */
	printf("[testing] Modifying the original node after the clone deletion\n");
	myelement_prop1_set(root, egueb_dom_string_new_with_string("modifying"));

	egueb_dom_node_unref(root);
	printf("[testing] Destroy the original tree %d\n", egueb_dom_node_ref_get(root));
	/* destroy the whole document */
	egueb_dom_node_unref(mydocument);

	mylib_shutdown();

	return 0;
}
