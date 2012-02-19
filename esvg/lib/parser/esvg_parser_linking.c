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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Esvg_Parser_Linking
{
	Esvg_Parser_Linking_Descriptor *descriptor;
	void *data;
};

/*----------------------------------------------------------------------------*
 *                         The context interface                               *
 *----------------------------------------------------------------------------*/
#if 0
static Eina_Bool _context_tag_open(void *data, int tag,
		Eina_Array *contexts, const char *attributes,
		unsigned int length)
{
	Esvg_Parser_Context *thiz = data;
	Edom_Context *context;
	Edom_Tag *new_tag = NULL;
	Eina_Bool ret = EINA_TRUE;

	context = eina_array_data_get(contexts, eina_array_count_get(contexts) - 1);
	/* TODO first check if the tag is supported */
	if (!thiz->tag_is_supported(tag))
	{
		printf("parsing error. tag is not suppoted %d\n", tag);
		return;
	}
	/* if so, create it */
	new_tag = _context_tag_new(thiz, tag, context, contexts);
	if (new_tag)
	{
		edom_tag_attributes_from_xml(new_tag, attributes, length);
		edom_tag_child_add(thiz->parent, new_tag);
		esvg_parser_svg_tag_add(thiz->svg, new_tag);
	}
	/* if there's a tag open callback call it with the new tag passed */

	return ret;
}

static void _context_tag_close(void *data, int tag,
		Eina_Array *contexts)
{
	Esvg_Parser_Context *thiz = data;
	Edom_Context *context;

	/* destroy ourselves */
	if (thiz->tag == tag)
	{
		context = eina_array_pop(contexts);
		edom_context_delete(context);
		return;
	}
}

static void _context_free(void *data)
{
	Esvg_Parser_Context *thiz = data;
	free(thiz);
}

static Edom_Context_Descriptor _descriptor = {
	/* .tag_open = 	*/ _context_tag_open,
	/* .tag_close =	*/ _context_tag_close,
	/* .data =	*/ NULL,
	/* .cdata =	*/ NULL,
	/* .free = 	*/ _context_free,
};

#endif

static Edom_Tag * _linking_tag_open(void *data, int tag, Edom_Context *context, Eina_Array *contexts)
{
	/* create the a tag */
	/* create the a context defined here */
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

