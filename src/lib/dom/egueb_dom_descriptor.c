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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Egueb_Dom_Descriptor
{
	Egueb_Dom_Descriptor *parent;
	Eina_Hash *attributes;
	char *name;
	size_t size;
};

struct _Egueb_Dom_Descriptor_Attribute
{
	Egueb_Dom_Descriptor_Accessor set;
	Egueb_Dom_Descriptor_Accessor get;
	Egueb_Dom_Descriptor_Accessor unset;
	char *name;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Descriptor * egueb_dom_element_descriptor_new(
		Egueb_Dom_Descriptor *parent,
		const char *name,
		Egueb_Dom_Element_Klass_Init init, void *klass)
{

}

EAPI const Egueb_Dom_Descriptor_Attribute * egueb_dom_descriptor_attribute_add(
		Egueb_Dom_Descriptor *thiz, const char *name,
		Egueb_Dom_Descriptor_Accessor set,
		Egueb_Dom_Descriptor_Accessor get,
		Egueb_Dom_Descriptor_Unsetter unset)
{

}

EAPI const Egueb_Dom_Descriptor_Attribute * egueb_dom_descriptor_attrbute_get(
		Egueb_Dom_Descriptor *thiz)
{

}

EAPI Eina_Error egueb_dom_descriptor_attribute_value_set(
		const Egueb_Dom_Descriptor_Attribute *attr, void *value)
{

}

EAPI Eina_Error egueb_dom_descriptor_attribute_value_get(
		const Egueb_Dom_Descriptor_Attribute *attr, void *value)
{

}
