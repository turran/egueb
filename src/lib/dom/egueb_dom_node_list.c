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
struct _Egueb_Dom_Node_List
{
	Egueb_Dom_Node *node;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/*
 * readonly attribute unsigned long length;
 */
EAPI int egueb_dom_node_list_length(Egueb_Dom_Node_List *thiz)
{
	return 0;
}

/*
 * Node item(in unsigned long index);
 */
EAPI Egueb_Dom_Node * egueb_dom_node_list_item_at(Egueb_Dom_Node_List *thiz, int index)
{
	return NULL;
}
