/* Egueb_Css_Engine - CSS
 * Copyright (C) 2011 Jorge Luis Zapata
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

#ifndef EGUEB_CSS_ENGINE_NODE_PRIVATE
#define EGUEB_CSS_ENGINE_NODE_PRIVATE

void egueb_css_engine_node_attribute_set(Egueb_Dom_Node *n,
		Egueb_Dom_String *attr, Egueb_Dom_String *val);
void egueb_css_engine_node_attribute_set_simple(Egueb_Dom_Node *n,
		const char *attr, const char *val);
Egueb_Dom_String * egueb_css_engine_node_attribute_get(Egueb_Dom_Node *n,
		Egueb_Dom_String *attr);
Eina_Bool egueb_css_engine_node_attribute_is_simple(Egueb_Dom_Node *n,
		const char *attr, const char *is_val);

#endif
