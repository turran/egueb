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
#ifndef _EGUEB_DOM_PARSER_PRIVATE_H_
#define _EGUEB_DOM_PARSER_PRIVATE_H_

typedef struct _Egueb_Dom_Parser Egueb_Dom_Parser;
typedef void (*Egueb_Dom_Parser_Free)(void *data);
typedef void (*Egueb_Dom_Parser_Parse)(void *data, Enesim_Image_Data *im);

typedef struct _Egueb_Dom_Parser_Descriptor
{
	Egueb_Dom_Parser_Parse parse;
	Egueb_Dom_Parser_Free free;
} Egueb_Dom_Parser_Descriptor;

Egueb_Dom_Parser * egueb_dom_parser_new(
		Egueb_Dom_Parser_Descriptor * descriptor, void *data);
void egueb_dom_parser_free(Egueb_Dom_Parser *thiz);
void * egueb_dom_parser_data_get(Egueb_Dom_Parser *thiz);

Egueb_Dom_Node * egueb_dom_parser_document_get(Egueb_Dom_Parser *thiz);
void egueb_dom_parser_document_set(Egueb_Dom_Parser *thiz, Egueb_Dom_Node *doc);

#endif
