/* Edom - DOM
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

#ifndef _EDOM_PRIVATE_H
#define _EDOM_PRIVATE_H

/* parser */

/* context */
Eina_Bool edom_context_tag_open(Edom_Context *c, int tag, const char *attrs, unsigned int length);
void edom_context_tag_close(Edom_Context *c, int tag);
Edom_Parser * edom_context_parser_get(Edom_Context *c);
void edom_context_cdata(Edom_Context *c, const char *cdata, unsigned int length);
void edom_context_data(Edom_Context *c, const char *data, unsigned int length);


#endif