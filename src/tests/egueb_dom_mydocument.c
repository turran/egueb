#include "Egueb_Dom.h"
#include "Egueb_Smil.h"

#include "egueb_dom_node_private.h"
#include "egueb_dom_document_private.h"
#include "egueb_smil_animation_private.h"

#include "egueb_dom_myelement.h"
#include "egueb_dom_mydocument.h"

typedef struct _MyDocument
{
	Egueb_Dom_Document parent;
} MyDocument;

typedef struct _MyDocument_Class
{
	Egueb_Dom_Document_Class parent;
} MyDocument_Class;

#define MYDOCUMENT_DESCRIPTOR mydocument_descriptor_get()
#define MYDOCUMENT_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, MyDocument_Class, MYDOCUMENT_DESCRIPTOR)
#define MYDOCUMENT(o) ENESIM_OBJECT_INSTANCE_CHECK(o, MyDocument, MYDOCUMENT_DESCRIPTOR)

ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_DOCUMENT_DESCRIPTOR, MyDocument, MyDocument_Class, mydocument);

static Egueb_Dom_Node * _mydocument_element_create(Egueb_Dom_Document *d,
		const char *ns, const char *name)
{
	Egueb_Dom_Node *new_node = NULL;

	printf("[mydocument] requesting tag name = %s\n", name);
	if (!strcmp(name, "myelement"))
		new_node = myelement_new();
	return new_node;
}

static Eina_Bool _mydocument_child_appendable(Egueb_Dom_Document *d,
		Egueb_Dom_Node *child)
{
	return EINA_TRUE;
}

static void _mydocument_class_init(void *k)
{
	Egueb_Dom_Document_Class *klass = EGUEB_DOM_DOCUMENT_CLASS(k);

	klass->element_create = _mydocument_element_create;
	klass->child_appendable = _mydocument_child_appendable;
	printf("mydocument class init\n");
}

static void _mydocument_instance_init(void *o)
{
	printf("mydocument instance init\n");
}

static void _mydocument_instance_deinit(void *o)
{
	printf("mydocument instance deinit\n");
}

Egueb_Dom_Node * mydocument_new(void)
{
	Egueb_Dom_Node *ret;

	ret = EGUEB_DOM_NODE (ENESIM_OBJECT_INSTANCE_NEW(mydocument));
	return ret;
}
