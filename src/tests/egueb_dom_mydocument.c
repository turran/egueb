#include <Etch.h>
#include "Egueb_Dom.h"
#include "Egueb_Smil.h"

#include "Eina_Extra.h"
#include "egueb_dom_node_private.h"
#include "egueb_dom_document_private.h"
#include "egueb_smil_animation_private.h"

#include "egueb_dom_myelement.h"
#include "egueb_dom_mydocument.h"

typedef struct _MyDocument
{
	Egueb_Dom_Document parent;
	Etch *etch;
} MyDocument;

typedef struct _MyDocument_Class
{
	Egueb_Dom_Document_Class parent;
} MyDocument_Class;

#define MYDOCUMENT_DESCRIPTOR mydocument_descriptor_get()
#define MYDOCUMENT_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, MyDocument_Class, MYDOCUMENT_DESCRIPTOR)
#define MYDOCUMENT(o) ENESIM_OBJECT_INSTANCE_CHECK(o, MyDocument, MYDOCUMENT_DESCRIPTOR)

ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_DOCUMENT_DESCRIPTOR, MyDocument, MyDocument_Class, mydocument);

static Egueb_Dom_Node * _mydocument_class_element_create(Egueb_Dom_Document *d,
		const char *name)
{
	MyDocument *thiz;
	Egueb_Dom_Node *new_node = NULL;

	thiz = MYDOCUMENT(d);
	printf("[mydocument] requesting tag name = %s\n", name);
	if (!strcmp(name, "myelement"))
		new_node = myelement_new();
	else if (!strcmp(name, "set"))
	{
		new_node = egueb_smil_set_new();
		egueb_smil_animation_etch_set(new_node, thiz->etch);
	}
	return new_node;
}

static void _mydocument_class_init(void *k)
{
	Egueb_Dom_Document_Class *klass = EGUEB_DOM_DOCUMENT_CLASS(k);

	klass->element_create = _mydocument_class_element_create;
	printf("mydocument class init\n");
}

static void _mydocument_instance_init(void *o)
{
	MyDocument *thiz;

	thiz = MYDOCUMENT(o);
	printf("mydocument instance init\n");
	thiz->etch = etch_new();
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

void mydocument_tick(Egueb_Dom_Node *n)
{
	MyDocument *thiz;

	thiz = MYDOCUMENT(n);
	etch_timer_tick(thiz->etch);
}
