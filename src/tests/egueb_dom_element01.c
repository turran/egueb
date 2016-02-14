#include "Egueb_Dom.h"
#include "egueb_dom_mydocument.h"
#include "egueb_dom_myelement.h"

static void _mutation_attr_modified_cb(Egueb_Dom_Event *ev, void *data)
{
	Egueb_Dom_String *new_value = NULL;
	Egueb_Dom_Event_Phase phase;

	egueb_dom_event_mutation_value_new_string_get(ev, &new_value);
	phase = egueb_dom_event_phase_get(ev);
	printf("[testing] mutation event attr modified received %d\n", phase);
	printf("[testing] new value %s\n", egueb_dom_string_chars_get(new_value));
}

static void _mutation_node_removed_cb(Egueb_Dom_Event *ev, void *data)
{
	Egueb_Dom_Event_Phase phase;
	Egueb_Dom_Node *target, *current_target;

	phase = egueb_dom_event_phase_get(ev);
	target = egueb_dom_event_target_get(ev);
	current_target = egueb_dom_event_target_current_get(ev);
	printf("[testing] mutation event node removed received target: %p, "
			"phase: %d, current_target: %p\n", target, phase,
			current_target);
	egueb_dom_node_unref(target);
	egueb_dom_node_unref(current_target);
}

static void _mutation_node_inserted_cb(Egueb_Dom_Event *ev, void *data)
{
	Egueb_Dom_Event_Phase phase;
	Egueb_Dom_Node *target, *current_target;

	phase = egueb_dom_event_phase_get(ev);
	target = egueb_dom_event_target_get(ev);
	current_target = egueb_dom_event_target_current_get(ev);
	printf("[testing] mutation event node inserted received target: %p, "
			"phase: %d, current_target: %p\n", target, phase,
			current_target);
	egueb_dom_node_unref(target);
	egueb_dom_node_unref(current_target);
}

static void _mutation_node_inserted_into_document_cb(Egueb_Dom_Event *ev, void *data)
{
	Egueb_Dom_Event_Phase phase;
	Egueb_Dom_Node *target, *current_target;

	phase = egueb_dom_event_phase_get(ev);
	target = egueb_dom_event_target_get(ev);
	current_target = egueb_dom_event_target_current_get(ev);
	printf("[testing] mutation event node inserted into document received "
			"target: %p, phase: %d, current_target: %p\n", target,
			phase, current_target);
	egueb_dom_node_unref(target);
	egueb_dom_node_unref(current_target);
}

static void _mutation_node_removed_from_document_cb(Egueb_Dom_Event *ev, void *data)
{
	Egueb_Dom_Event_Phase phase;
	Egueb_Dom_Node *target, *current_target;

	phase = egueb_dom_event_phase_get(ev);
	target = egueb_dom_event_target_get(ev);
	current_target = egueb_dom_event_target_current_get(ev);
	printf("[testing] mutation event node removed from document received "
			"target: %p, phase: %d, current_target: %p\n", target,
			phase, current_target);
	egueb_dom_node_unref(target);
	egueb_dom_node_unref(current_target);
}

static void _events_register(Egueb_Dom_Node *n)
{
	/* register for a mutation event */
	egueb_dom_event_target_event_listener_add(n,
		EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED,
		_mutation_attr_modified_cb, EINA_TRUE, NULL);
	/* register the document insertion/removal */
	egueb_dom_event_target_event_listener_add(n,
		EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED_INTO_DOCUMENT,
		_mutation_node_inserted_into_document_cb, EINA_TRUE, NULL);
	egueb_dom_event_target_event_listener_add(n,
		EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED_FROM_DOCUMENT,
		_mutation_node_removed_from_document_cb, EINA_TRUE, NULL);
	/* test the node insertion/deletion events */
	egueb_dom_event_target_event_listener_add(n,
		EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
		_mutation_node_removed_cb, EINA_TRUE, NULL);
	egueb_dom_event_target_event_listener_add(n,
		EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
		_mutation_node_inserted_cb, EINA_TRUE, NULL);
}

int main(void)
{
	Egueb_Dom_Node *el1, *el2, *el3;
	Egueb_Dom_Node *doc;
	Egueb_Dom_String *name;
	Egueb_Dom_String *value_set;
	Egueb_Dom_String *value_get = NULL;

	mylib_init();
	/* test the creation of an element */
	el1 = myelement_new();
	_events_register(el1);
	el2 = myelement_new();
	_events_register(el2);
	el3 = myelement_new();
	_events_register(el3);
	printf("[testing] elements created el1: %p el2: %p el3: %p\n", el1, el2, el3);

	/* test the property set/get */
	name = egueb_dom_string_new_with_chars("prop1");
	value_set = egueb_dom_string_new_with_chars("value");
	egueb_dom_element_attribute_set(el1, name, value_set, NULL);
	value_get = egueb_dom_element_attribute_get(el1, name);
	printf("[testing] getting 'prop1' = '%s'\n", egueb_dom_string_chars_get(value_get));
	egueb_dom_string_unref(value_set);
	egueb_dom_string_unref(value_get);
	egueb_dom_string_unref(name);

	/* test the generic property set/get */
	name = egueb_dom_string_new_with_chars("attribute");
	value_set = egueb_dom_string_new_with_chars("value");
	egueb_dom_element_attribute_set(el1, name, value_set, NULL);
	value_get = egueb_dom_element_attribute_get(el1, name);
	printf("[testing] getting 'attribute' = '%s'\n", egueb_dom_string_chars_get(value_get));
	egueb_dom_string_unref(value_set);
	egueb_dom_string_unref(value_get);
	egueb_dom_string_unref(name);

	/* test adding a node as a child of another */
	printf("[testing] appending el2 into el1\n");
	egueb_dom_node_child_append(el1, egueb_dom_node_ref(el2), NULL);
	printf("[testing] appending el2 into el1\n");
	egueb_dom_node_child_append(el2, el3, NULL);

	/* test importing a node into a document */
	doc = mydocument_new();
	printf("[testing] set the main element of the document\n");
	egueb_dom_document_element_set(doc, egueb_dom_node_ref(el1));
	printf("[testing] removing el2\n");
	egueb_dom_node_child_remove(el1, el2, NULL);
	printf("[testing] unrefing el2\n");
	egueb_dom_node_unref(el2);
	printf("[testing] unrefing el1\n");
	egueb_dom_node_unref(el1);
	printf("[testing] unrefing doc\n");
	egueb_dom_node_unref(doc);

	mylib_shutdown();
	return 0;
}

