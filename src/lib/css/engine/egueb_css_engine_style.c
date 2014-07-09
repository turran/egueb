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

#include "egueb_css_private.h"

#include "egueb_css_engine_context_private.h"
#include "egueb_css_engine_selector_private.h"
#include "egueb_css_engine_rule_private.h"
#include "egueb_css_engine_style_private.h"
#include "egueb_css_engine_parser_private.h"

void * egueb_css_engine__scan_string(const char *yy_str, void *scanner);
void egueb_css_engine__delete_buffer(void *state, void *scanner);
void egueb_css_engine__switch_to_buffer(void *state, void *scanner);
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_css_engine_style_inline_apply(const char *style, Egueb_Css_Engine_Context *c, void *e)
{
	char *orig;
	char *v;
	char *sc;
	char *ch;

	if (!c->attribute_set)
		return;

	orig = v = strdup(style);
	EGUEB_DOM_SPACE_SKIP(v);
	/* split the style by ';' */
	while ((sc = strchr(v, ';')))
	{
		*sc = '\0';
		/* split it by ':' */
		ch = strchr(v, ':');
		if (ch)
		{
			char *vv;

			*ch = '\0';
			vv = ch + 1;
			EGUEB_DOM_SPACE_SKIP(vv);
			/* and call the attr_cb */
			c->attribute_set(e, v, vv);
		}
		v = sc + 1;
		EGUEB_DOM_SPACE_SKIP(v);
	}
	/* do the last one */
	ch = strchr(v, ':');
	if (ch)
	{
		char *vv;

		*ch = '\0';
		vv = ch + 1;
		EGUEB_DOM_SPACE_SKIP(vv);
		/* and call the attr_cb */
		c->attribute_set(e, v, vv);
	}

	free(orig);

}

Egueb_Css_Engine_Style * egueb_css_engine_style_new(void)
{
	Egueb_Css_Engine_Style *thiz;

	thiz = calloc(1, sizeof(Egueb_Css_Engine_Style));
	return thiz;
}

Egueb_Css_Engine_Style * egueb_css_engine_style_load_from_file(const char *file)
{
	Egueb_Css_Engine_Style *thiz;
	Egueb_Css_Engine_Parser parser;
	void *scanner;
	FILE *f;
	int ret;

	if (!file) return NULL;

	f = fopen(file, "r");
	if (!f) return NULL;

	thiz = egueb_css_engine_style_new();
	parser.style = thiz;

	egueb_css_engine_lex_init(&scanner);
	egueb_css_engine_set_in(f, scanner);
	ret = egueb_css_engine_parse(scanner, &parser);

	egueb_css_engine_lex_destroy(scanner);
	fclose(f);

	if (ret)
	{
		printf("bad parsing\n");
		free(thiz);
		return NULL;
	}
	return thiz;
}

Egueb_Css_Engine_Style * egueb_css_engine_style_load_from_content(const char *content, size_t len)
{
	Egueb_Css_Engine_Style *thiz;
	Egueb_Css_Engine_Parser parser;
	char *tmp;
	void *buffer_state;
	void *scanner;
	int ret;

	thiz = egueb_css_engine_style_new();
	parser.style = thiz;

	tmp = malloc(len + 2);
	strncpy(tmp, content, len);
	tmp[len] = tmp [len+1] = 0; // looks like flex/yacc need a double null terminated string

	egueb_css_engine_lex_init(&scanner);
	buffer_state = egueb_css_engine__scan_string(tmp, scanner);
	egueb_css_engine__switch_to_buffer(buffer_state, scanner); // switch flex to the buffer we just created
	ret = egueb_css_engine_parse(scanner, &parser);
	egueb_css_engine__delete_buffer(buffer_state, scanner);

	egueb_css_engine_lex_destroy(scanner);

	if (ret)
	{
		printf("bad parsing\n");
		free(thiz);
		return NULL;
	}

	return thiz;
}

void egueb_css_engine_style_rule_add(Egueb_Css_Engine_Style *thiz, Egueb_Css_Engine_Rule *r)
{
	if (!thiz) return;
	if (!r) return;
	thiz->rules = eina_list_append(thiz->rules, r);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

