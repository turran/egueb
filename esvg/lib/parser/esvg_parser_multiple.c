/* Esvg - SVG
 * Copyright (C) 2011 Jorge Luis Zapata, Vincent Torri
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
#include "Esvg.h"
#include "esvg_parser_private.h"
#include "esvg_values.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Parser_Multiple
{
	Eina_List 
} Esvg_Parser_Multiple;

typedef struct _Esvg_Parser_Multiple_Element
{
	const Esvg_Parser_Descriptor *descriptor;
	void *data;
} Esvg_Parser_Multiple_Element;

static Esvg_Parser_Descriptor _multiple_descriptor = {

};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Esvg_Parser_Multiple * esvg_parser_multiple_new(void)
{
	Esvg_Parser_Multiple *thiz;

	thiz = calloc(1, sizeof(Esvg_Parser_Multiple));
	return thiz;
}

EAPI void esvg_parser_multiple_descriptor_add(Esvg_Parser_Multiple *thiz,
		Esvg_Parser_Descriptor *descriptor, const void *data)
{

}

EAPI const Esvg_Parser_Descriptor * esvg_parser_multiple_descriptor_get(void)
{
	return &_multiple_descriptor;
}
