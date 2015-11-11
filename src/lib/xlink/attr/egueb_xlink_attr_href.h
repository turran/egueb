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

/*
 * A xlink:href attribute can work on different ways depending on the flags
 * that it has. In case the remote flag is set, the attribute will trigger
 * an EGUEB_DOM_EVENT_IO_DATA event to fetch the data.
 * If the flag FRAGMENT is set, then we require a particular node from the
 * document. If the remote is set, then it can be a remote
 * document too
 */
typedef enum _Egueb_Xlink_Attr_Href_Flag
{
	EGUEB_XLINK_ATTR_HREF_FLAG_REMOTE = 1, 
	EGUEB_XLINK_ATTR_HREF_FLAG_FRAGMENT = 2, 
} Egueb_Xlink_Attr_Href_Flag;

typedef void (*Egueb_Xlink_Attr_Href_Cb)(Egueb_Dom_Node *n);

EAPI Egueb_Dom_Node * egueb_xlink_attr_href_new(int flags);
EAPI Eina_Bool egueb_xlink_attr_href_has_changed(Egueb_Dom_Node *n);
EAPI Eina_Bool egueb_xlink_attr_href_process(Egueb_Dom_Node *n);

EAPI void egueb_xlink_attr_href_automatic_enqueue_set(Egueb_Dom_Node *n,
		Eina_Bool enable);
EAPI void egueb_xlink_attr_href_on_target_removed_set(Egueb_Dom_Node *n,
		Egueb_Xlink_Attr_Href_Cb cb);
EAPI void egueb_xlink_attr_href_on_data_received_set(Egueb_Dom_Node *n,
		Egueb_Xlink_Attr_Href_Cb cb);

EAPI Egueb_Dom_Node * egueb_xlink_attr_href_node_get(Egueb_Dom_Node *n);
EAPI Enesim_Stream * egueb_xlink_attr_href_data_get(Egueb_Dom_Node *n);

#endif
