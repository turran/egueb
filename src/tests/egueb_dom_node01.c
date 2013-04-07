/* A test of the new dom interface, here we will
 * create a new element that inherits from a node
 */
#include "Egueb_Dom.h"

typedef struct _MyObject
{
	Egueb_Dom_Node parent;
} MyObject;

typedef struct _MyObject_Class
{
	Egueb_Dom_Node_Class parent;
} MyObject_Class;

ENESIM_OBJECT_INSTANCE_BOILERPLATE(abstract02_descriptor_get(), MyObject, MyObject_Class, myobject);
#define MYOBJECT_DESCRIPTOR myobject_descriptor_get()
#define MYOBJECT_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, MyObject_Class, MYOBJECT_DESCRIPTOR)
#define MYOBJECT(o) ENESIM_OBJECT_INSTANCE_CHECK(o, MyObject, MYOBJECT_DESCRIPTOR)

EGUEB_DOM_NODE_CLASS_DEFINE_PROPERTY(prop_type, type, prefix, value)
EGUEB_DOM_NODE_CLASS_DEFINE_PROPERTY(prop_type, type, prefix, value)

static void _myobject_class_init(void *k)
{
	/* add the ender descriptor associated with this class */
	/* we need to associate en ender_descriptor for each enesim_object_descriptor
	 * which will describe the klass such descriptor refers to
	 *
	 * If we add an ender descriptor on the node class, then we will
	 * only get an ender descriptor for the last class on the inheritance
	 * not the middle ones.
	 */
	EGUEB_DOM_NODE_CLASS_DESCRIPTOR_ADD(ENESIM_OBJECT_CLASS(k),
			EGUEB_DOM_NAMESPACE, EGUEB_DOM_NODE_NAME,
			MYOBJECT_NAMESPACE, myobject);
	EGUEB_DOM_NODE_CLASS_ADD_PROPERTY((EGUEB_DOM_NODE_CLASS(k), named_type, prefix, value);
	EGUEB_DOM_NODE_CLASS_ADD_PROPERTY((EGUEB_DOM_NODE_CLASS(k), named_type, prefix, value);
}

static void _myobject_instance_init(void *o)
{

}

static void _myobject_instance_deinit(void *o)
{

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

	my = myobject_new();
	return 0;
}
