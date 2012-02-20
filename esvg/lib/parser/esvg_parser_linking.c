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
#include "Esvg_Parser.h"
#include "esvg_parser_private.h"
#include "esvg_values.h"
/* TODO here we should have the parser descriptor to handle the <a> tag */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Esvg_Parser_Linking
{
	Esvg_Parser_Linking_Descriptor *descriptor;
	void *data;
};

/*----------------------------------------------------------------------------*
 *                        The A context interface                             *
 *----------------------------------------------------------------------------*/
static Eina_Bool _a_context_tag_open(void *data, int tag,
		Edom_Context *context, const char *attributes,
		unsigned int length)
{
	Esvg_Parser_Linking *thiz = data;
	Eina_Bool ret = EINA_TRUE;

	return ret;
}

static void _a_context_tag_close(void *data, int tag,
		Edom_Context *context)
{
	Esvg_Parser_Linking *thiz = data;

	/* destroy ourselves */
#if 0
	if (thiz->tag == ESVG_A)
	{
		context = eina_array_pop(contexts);
		edom_context_delete(context);
		return;
	}
#endif
}

static Edom_Context_Descriptor _descriptor = {
	/* .tag_open = 	*/ _a_context_tag_open,
	/* .tag_close =	*/ _a_context_tag_close,
	/* .data =	*/ NULL,
	/* .cdata =	*/ NULL,
	/* .free = 	*/ NULL,
};

/*----------------------------------------------------------------------------*
 *                     The parser descriptor interface                         *
 *----------------------------------------------------------------------------*/
static Eina_Bool _linking_tag_open(void *data, int tag, Edom_Context *context,
		const char *attributes, size_t len)
{
	/* create the a tag */
	printf("tag open! %d\n", tag);
	/* create the a context defined here */
	return EINA_FALSE;
}

/* FIXME do we really need this? */
static void _linking_tag_is_supported(void *data, int tag, Edom_Context *context, Eina_Array *contexts)
{

}

static Esvg_Parser_Descriptor _linking_descriptor = {
	/* .tag_get = */ NULL,
	/* .tag_open = */ _linking_tag_open,
	/* .tag_is_supported = */ _linking_tag_is_supported,
	/* .tag_attribute_set = */ NULL,
	/* .tag_attribute_get = */ NULL,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Esvg_Parser_Linking * esvg_parser_linking_new(
		Esvg_Parser_Linking_Descriptor *descriptor,
		void *data)
{
	Esvg_Parser_Linking *thiz;

	thiz = calloc(1, sizeof(Esvg_Parser_Linking));
	thiz->descriptor = descriptor;
	thiz->data = data;
	return thiz;
}

EAPI const Esvg_Parser_Descriptor * esvg_parser_linking_descriptor_get(void)
{
	return &_linking_descriptor;
}

