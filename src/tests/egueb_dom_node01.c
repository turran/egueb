/* A test of the new dom interface, here we will
 * create a new element that inherits from a node
 */
#include "Egueb_Dom.h"
#include "egueb_dom_node_private.h"

typedef struct _MyNode
{
	Egueb_Dom_Node parent;
} MyNode;

typedef struct _MyNode_Class
{
	Egueb_Dom_Node_Class parent;
} MyNode_Class;

MyNode * mynode_new(void);

#define MYNODE_DESCRIPTOR mynode_descriptor_get()
#define MYNODE_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, MyNode_Class, MYNODE_DESCRIPTOR)
#define MYNODE(o) ENESIM_OBJECT_INSTANCE_CHECK(o, MyNode, MYNODE_DESCRIPTOR)

ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_NODE_DESCRIPTOR, MyNode, MyNode_Class, mynode);

static void _mynode_class_init(void *k)
{
	printf("class init\n");
}

static void _mynode_instance_init(void *o)
{
	printf("instance init\n");
}

static void _mynode_instance_deinit(void *o)
{
	printf("instance deinit\n");
}

MyNode * mynode_new(void)
{
	MyNode *thiz;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(mynode);
	return thiz;
}

int main(void)
{
	MyNode *my;

	mylib_init();

	my = mynode_new();
	mylib_shutdown();
	return 0;
}
