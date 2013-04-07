/* Egueb
 * Copyright (C) 2011 - 2013 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _EGUEB_DOM_NODE_PRIVATE_H_
#define _EGUEB_DOM_NODE_PRIVATE_H_

struct _Egueb_Dom_Node
{
	Enesim_Object_Instance base;
	EINA_INLIST;
	Egueb_Dom_Node *parent;
	Eina_Inlist *children;
	Eina_Hash *user_data;
	void *external_data;
};

typedef struct _Egueb_Dom_Node_Class
{
	Enesim_Object_Class base;
} Egueb_Dom_Node_Class;

#define EGUEB_DOM_NODE_DESCRIPTOR egueb_dom_node_descriptor_get()
Enesim_Object_Descriptor * egueb_dom_node_descriptor_get(void);

/* Macro to define the common node descriptor functions */
#define EGUEB_DOM_NODE_DESCRIPTOR_DEFINITION_BOILERPLATE(prefix)		\
	static void _##prefix##_class_init(void *k);				\
	static void _##prefix##_descriptor_init(Ender_Descriptor *d);		\
	static void _##prefix##_instance_init(void *o);				\
	static void _##prefix##_instance_deinit(void *o); 			\

/* Macro to create the enesim object descriptor get function */
#define EGUEB_DOM_NODE_DESCRIPTOR_BOILERPLATE(parent, type, class_type, prefix)	\
	Enesim_Object_Descriptor * prefix##_descriptor_get(void)		\
	{									\
		static Enesim_Object_Descriptor *d = NULL;			\
		if (!d)								\
		{								\
			Egueb_Dom_Descriptor *edd;				\
										\
			d = enesim_object_descriptor_new(parent,		\
				sizeof(class_type), _##prefix##_class_init,	\
				sizeof(type), _##prefix##_instance_init,	\
				_##prefix##_instance_deinit, #type);		\
										\
			edd = egueb_dom_descriptor_new();			\
			egueb_dom_descriptor_ender_set(edd, 			\
					_prefix_##ender_descriptor_get());	\
			enesim_object_descriptor_private_set(d, edd);		\
			egueb_dom_descriptor_enesim_set(edd, d);		\
		}								\
		return d;							\
	}									\
/* Macro to define an ender abstract descriptor */
#define EGUEB_DOM_NODE_ENDER_ABSTRACT_BOILERPLATE(parent, type, prefix)		\
	static Ender_Descriptor * _prefix_##ender_descriptor_get(void) 		\
	{									\
		static Ender_Descriptor *d = NULL;				\
		if (!d)								\
		{								\
			Egueb_Dom_Descriptor *edd;				\
			Ender_Descriptor *ed;					\
										\
			edd = enesim_object_descriptor_private_get(parent);	\
			ed = egueb_dom_descriptor_ender_get(edd);		\
			d = ender_namespace_descriptor_add(ns, #type, 		\
					NULL,					\
					ENDER_DESTRUCTOR(egueb_dom_node_unref)	\
					ed, ENDER_TYPE_ABSTRACT, -1);		\
			_##prefix##_descriptor_init(d);				\
		}								\
		return d;							\
	}									\
/* Macro to define an ender class descriptor */
#define EGUEB_DOM_NODE_ENDER_CLASS_BOILERPLATE(parent, type, prefix)		\
	static Ender_Descriptor * _prefix_##ender_descriptor_get(void) 		\
	{									\
		static Ender_Descriptor *d = NULL;				\
		if (!d)								\
		{								\
			Egueb_Dom_Descriptor *edd;				\
			Ender_Descriptor *ed;					\
										\
			edd = enesim_object_descriptor_private_get(parent);	\
			ed = egueb_dom_descriptor_ender_get(edd);		\
			d = ender_namespace_descriptor_add(ns, #type, 		\
					ENDER_CONSTRUCTOR(prefix##_new),	\
					ENDER_DESTRUCTOR(egueb_dom_node_unref)	\
					ed, ENDER_TYPE_CLASS, -1);		\
			_##prefix##_descriptor_init(d);				\
		}								\
		return d;							\
	}									\

/* Macro to define a new abstract node  */
#define EGUEB_DOM_NODE_ABSTRACT_BOILERPLATE(parent, type, class_type, prefix) 	\
	EGUEB_DOM_NODE_DESCRIPTOR_DEFINITION_BOILERPLATE(prefix)		\
	EGUEB_DOM_NODE_ENDER_ABSTRACT_BOILERPLATE(parent, type, prefix)		\
	EGUEB_DOM_NODE_DESCRIPTOR_BOILERPLATE(parent, type, class_type, prefix)

/* Macro to define a new class node */
#define EGUEB_DOM_NODE_CLASS_BOILERPLATE(parent, type, class_type, prefix) 	\
	EGUEB_DOM_NODE_DESCRIPTOR_DEFINITION_BOILERPLATE(prefix)		\
	EGUEB_DOM_NODE_ENDER_CLASS_BOILERPLATE(parent, type, prefix)		\
	EGUEB_DOM_NODE_DESCRIPTOR_BOILERPLATE(parent, type, class_type, prefix)

#endif
