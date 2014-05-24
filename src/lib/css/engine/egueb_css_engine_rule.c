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
#include "egueb_css_engine_selector_private.h"
#include "egueb_css_engine_rule_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_css_engine_rule_declaration_insert(Egueb_Css_Engine_Rule *thiz, Egueb_Css_Engine_Declaration *d)
{
	printf("New declaration of %s %s\n", d->attribute, d->value);
	thiz->declarations = eina_list_append(thiz->declarations, d);
}

Egueb_Css_Engine_Rule * egueb_css_engine_rule_new(Egueb_Css_Engine_Selector *s)
{
	Egueb_Css_Engine_Rule *thiz;

	if (!s) return NULL;

	thiz = calloc(1, sizeof(Egueb_Css_Engine_Rule));
	thiz->selector = s;

	return thiz;
}

void egueb_css_engine_rule_declaration_add(Egueb_Css_Engine_Rule *thiz, const char *attribute,
		const char *value)
{
	Egueb_Css_Engine_Declaration *d;

	if (!attribute || !value) return;

	d = malloc(sizeof(Egueb_Css_Engine_Declaration));
	d->attribute = strdup(attribute);
	d->value = strdup(value);

	egueb_css_engine_rule_declaration_insert(thiz, d);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

