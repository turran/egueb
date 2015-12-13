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
#include "egueb_dom_private.h"
#include "egueb_dom_attr.h"
#include "egueb_dom_attr_private.h"
#include "egueb_dom_attr_basic_private.h"
#include "egueb_dom_value_double.h"

#include "egueb_dom_attr_double.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
EGUEB_DOM_ATTR_BASIC_BOILERPLATE(double,
		Egueb_Dom_Attr_Double, egueb_dom_value_double,
		egueb_dom_attr_double)
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_dom_attr_double_new(Egueb_Dom_String *name,
		Egueb_Dom_String *ns, Eina_Bool animatable, Eina_Bool stylable,
		Eina_Bool inheritable)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_attr_double);
	egueb_dom_attr_init(n, name, ns, animatable, stylable, inheritable);
	return n;
}
