/* Ecss - CSS
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Eina.h>

#include "Ecss.h"
#include "ecss_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ecss_Style * ecss_style_new(void)
{
	Ecss_Style *thiz;

	thiz = calloc(1, sizeof(Ecss_Style));
	return thiz;
}

EAPI Ecss_Style * ecss_style_load_from_file(const char *file)
{
	Ecss_Style *thiz;
	Ecss_Parser parser;
	void *scanner;
	FILE *f;
	int ret;

	if (!file) return NULL;

	f = fopen(file, "r");
	if (!f) return NULL;

	thiz = ecss_style_new();
	parser.style = thiz;

	ecss_lex_init(&scanner);
	ecss_set_in(f, scanner);
	ret = ecss_parse(scanner, &parser);

	ecss_lex_destroy(scanner);
	fclose(f);

	if (ret)
	{
		printf("bad parsing\n");
		free(thiz);
		return NULL;
	}
	return thiz;
}

EAPI Ecss_Style * ecss_style_load_from_content(const char *content, size_t len)
{
	Ecss_Style *thiz;
	Ecss_Parser parser;
	char *tmp;
	void *buffer_state;
	void *scanner;
	int ret;

	thiz = ecss_style_new();
	parser.style = thiz;

	tmp = malloc(len + 2);
	strncpy(tmp, content, len);
	tmp[len] = tmp [len+1] = 0; // looks like flex/yacc need a double null terminated string

	ecss_lex_init(&scanner);
	buffer_state = ecss__scan_string(tmp, scanner);
	ecss__switch_to_buffer(buffer_state, scanner); // switch flex to the buffer we just created
	ret = ecss_parse(scanner, &parser);
	ecss__delete_buffer(buffer_state, scanner);

	ecss_lex_destroy(scanner);

	if (ret)
	{
		printf("bad parsing\n");
		free(thiz);
		return NULL;
	}

	return thiz;
}

EAPI void ecss_style_rule_add(Ecss_Style *thiz, Ecss_Rule *r)
{
	if (!thiz) return;
	if (!r) return;
	thiz->rules = eina_list_append(thiz->rules, r);
}
