/* Creates a document manually
 * Creates different elements and append them
 * Set the document main element as the root of the hierarchy created
 */
#include "egueb_dom_mylib.h"

int main(int argc, char **argv)
{
	Egueb_Dom_Node *mydocument;
	Egueb_Dom_Node *el1, *el2;

	mylib_init();
	mydocument = mydocument_new();

	/* create our own hierarchy */
	el1 = myelement_new();
	el2 = myelement_new();
	egueb_dom_node_child_append(el1, el2, NULL);
	egueb_dom_document_element_set(mydocument, egueb_dom_node_ref(el1));
	/* dump it */
	myelement_dump(el1, EINA_TRUE);
	egueb_dom_node_unref(el1);

	/* process the document */
	egueb_dom_document_process(mydocument);
	/* now set some variable and do the process again */
	myelement_prop1_set(el2, egueb_dom_string_new_with_string("var1"));
	egueb_dom_document_process(mydocument);

	/* destroy the whole document */
	egueb_dom_node_unref(mydocument);

	mylib_shutdown();

	return 0;
}

