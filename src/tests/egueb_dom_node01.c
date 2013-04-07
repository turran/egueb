/* A test of the new dom interface, here we will
 * create a new element that inherits from a node
 */
#include "Egueb_Dom.h"
#include "egueb_dom_node_private.h"

typedef struct _MyObject
{
	Egueb_Dom_Node parent;
} MyObject;

typedef struct _MyObject_Class
{
	Egueb_Dom_Node_Class parent;
} MyObject_Class;

MyObject * myobject_new(void);

EGUEB_DOM_NODE_CLASS_BOILERPLATE(EGUEB_DOM_NODE_DESCRIPTOR, MyObject, MyObject_Class, EDOM_NAMESPACE, myobject);
#define MYOBJECT_DESCRIPTOR myobject_descriptor_get()
#define MYOBJECT_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, MyObject_Class, MYOBJECT_DESCRIPTOR)
#define MYOBJECT(o) ENESIM_OBJECT_INSTANCE_CHECK(o, MyObject, MYOBJECT_DESCRIPTOR)

static void _myobject_class_init(void *k)
{
	printf("class init\n");
}

static void _myobject_descriptor_init(Ender_Descriptor *d)
{
	printf("descriptor init\n");
}

static void _myobject_instance_init(void *o)
{
	printf("instance init\n");
}

static void _myobject_instance_deinit(void *o)
{
	printf("instance deinit\n");
}

MyObject * myobject_new(void)
{
	MyObject *thiz;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(myobject);
	return thiz;
}

int main(void)
{
	MyObject *my;

	edom_init();

	my = myobject_new();
	edom_shutdown();
	return 0;
}
