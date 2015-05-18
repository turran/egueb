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

#ifndef _EGUEB_XLINK_ATTR_HREF_H_
#define _EGUEB_XLINK_ATTR_HREF_H_

typedef void (*Egueb_Xlink_Attr_Href_Cb)(Egueb_Dom_Node *n);

EAPI Egueb_Dom_Node * egueb_xlink_attr_href_new(Egueb_Dom_String *name,
		Egueb_Dom_String *def);
EAPI Eina_Bool egueb_xlink_attr_href_process(Egueb_Dom_Node *n);
EAPI void egueb_xlink_attr_href_automatic_enqueue_set(Egueb_Dom_Node *n,
		Eina_Bool enable);
EAPI void egueb_xlink_attr_href_on_target_removed_set(Egueb_Dom_Node *n,
		Egueb_Xlink_Attr_Href_Cb cb);
EAPI Egueb_Dom_Node * egueb_xlink_attr_href_node_get(Egueb_Dom_Node *n);

#endif
