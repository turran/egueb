#include "Egueb_Dom.h"
#include "egueb_dom_mydocument.h"
#include "egueb_dom_myelement.h"

static void _monitor_original_cb(Egueb_Dom_Event *ev, void *data)
{
	Egueb_Dom_Node *other = data;

	/* TODO get the name of the event */
	printf("[testing] the monitor received an event, propagating\n");
	egueb_dom_node_event_propagate(other, ev);
}

static void _mutation_attr_modified_original_cb(Egueb_Dom_Event *ev, void *data)
{
	Egueb_Dom_Node *other = data;
	Egueb_Dom_Event_Phase phase;

	phase = egueb_dom_event_phase_get(ev);
	printf("[testing] mutation event attr modified received at phase %d\n", phase);
	printf("[testing] propagating\n");
	egueb_dom_node_event_propagate(other, ev);
}

static void _mutation_attr_modified_propagated_cb(Egueb_Dom_Event *ev, void *data)
{
	Egueb_Dom_Event_Phase phase;

	phase = egueb_dom_event_phase_get(ev);
	printf("[testing] mutation event attr modified propagated received at phase %d\n", phase);
}

int main(void)
{
	Egueb_Dom_Node *el1, *el2, *el3, *el4;
	Egueb_Dom_String *name;
	Egueb_Dom_String *value_set;

	mylib_init();
	el1 = myelement_new();
	el2 = myelement_new();
	el3 = myelement_new();
	el4 = myelement_new();

	egueb_dom_node_child_append(el1, el2, NULL);
	/* register for a mutation event */
	egueb_dom_node_event_listener_add(el1,
		EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED,
		_mutation_attr_modified_propagated_cb, EINA_TRUE, NULL);
	egueb_dom_node_event_listener_add(el1,
		EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED,
		_mutation_attr_modified_propagated_cb, EINA_FALSE, NULL);

	/* register for a mutation event */
	egueb_dom_node_event_listener_add(el3,
		EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED,
		_mutation_attr_modified_original_cb, EINA_TRUE, el2);
	egueb_dom_node_event_listener_add(el3,
		EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED,
		_mutation_attr_modified_original_cb, EINA_FALSE, el2);
	egueb_dom_node_event_monitor_add(el3, _monitor_original_cb, el2);
	/* the other tree */
	egueb_dom_node_child_append(el3, el4, NULL);
	/* set a property */
	name = egueb_dom_string_new_with_string("prop1");
	value_set = egueb_dom_string_new_with_string("value");
	egueb_dom_element_attribute_set(el4, name, value_set, NULL);

	return 0;
}	
