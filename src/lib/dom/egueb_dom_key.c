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
#include "egueb_dom_private.h"

#include "egueb_dom_string.h"
#include "egueb_dom_key.h"
#include "egueb_dom_key_private.h"
/* TODO we should create a tree for easy matching if a key is or not printable
 * like:
 * input = Return
 * first check if there is a non-printable string that starts with R, then
 * if the next char can be e, then t, and so on ...
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Egueb_Dom_Key_Dict
{
	Eina_Inlist *roots;
} Egueb_Dom_Key_Dict;

typedef struct _Egueb_Dom_Key_Dict_Entry
{
	EINA_INLIST;
	Eina_Unicode k;
	Eina_Inlist *children;
} Egueb_Dom_Key_Dict_Entry;

static Egueb_Dom_Key_Dict *_non_printable = NULL;

static void _egueb_dom_key_dict_entry_free(Egueb_Dom_Key_Dict_Entry *entry)
{
	Egueb_Dom_Key_Dict_Entry *child;
	EINA_INLIST_FREE(entry->children, child)
	{
		entry->children = eina_inlist_remove(entry->children,
				EINA_INLIST_GET(child));
		_egueb_dom_key_dict_entry_free(child);
	}
	free(entry);
}

static Eina_Bool _egueb_dom_key_dict_exists_recursive(Egueb_Dom_Key_Dict_Entry *entry,
		const char *str)
{
	Egueb_Dom_Key_Dict_Entry *child;
	Eina_Unicode uni;
	int index = 0;

	uni = eina_unicode_utf8_next_get(str, &index);
	/* The '\0' is a valid one */
	EINA_INLIST_FOREACH(entry->children, child)
	{
		/* if found, continue with the next entry */
		if (child->k == uni)
		{
			if (!uni && !index)
				return EINA_TRUE;
			else
				return _egueb_dom_key_dict_exists_recursive(child,
					str + index);
		}
	}
	return EINA_FALSE;
}

static Egueb_Dom_Key_Dict * _egueb_dom_key_dict_new(void)
{
	Egueb_Dom_Key_Dict *thiz;

	thiz = calloc(1, sizeof(Egueb_Dom_Key_Dict));
	return thiz;
}

static void _egueb_dom_key_dict_free(Egueb_Dom_Key_Dict *thiz)
{
	Egueb_Dom_Key_Dict_Entry *child;

	EINA_INLIST_FREE(thiz->roots, child)
	{
		thiz->roots = eina_inlist_remove(thiz->roots,
				EINA_INLIST_GET(child));
		_egueb_dom_key_dict_entry_free(child);
	}
}

static void _egueb_dom_key_dict_add(Egueb_Dom_Key_Dict *thiz,
		const char *str)
{
	Egueb_Dom_Key_Dict_Entry *root = NULL;
	Egueb_Dom_Key_Dict_Entry *child;
	Eina_Unicode uni;
	int index = 0;

	/* find the root */
	uni = eina_unicode_utf8_next_get(str, &index);
	EINA_INLIST_FOREACH(thiz->roots, child)
	{
		/* if found, continue with the next entry */
		if (child->k == uni)
		{
			root = child;
			break;
		}
	}
	if (!root)
	{
		root = calloc(1, sizeof(Egueb_Dom_Key_Dict_Entry));
		root->k = uni;
		thiz->roots = eina_inlist_append(thiz->roots, 
				EINA_INLIST_GET(root));
	}

	/* go creating an entry for every unicode char */
	while (1)
	{
		Egueb_Dom_Key_Dict_Entry *next = NULL;

		uni = eina_unicode_utf8_next_get(str, &index);
		EINA_INLIST_FOREACH(root->children, child)
		{
			/* if found, continue with the next entry */
			if (child->k == uni)
			{
				next = child;
				break;
			}
		}
		if (!next)
		{
			next = calloc(1, sizeof(Egueb_Dom_Key_Dict_Entry));
			next->k = uni;
			root->children = eina_inlist_append(root->children, 
					EINA_INLIST_GET(next));

		}
		root = next;
		if (!uni)
			break;
	}
}

static Eina_Bool _egueb_dom_key_dict_exists(Egueb_Dom_Key_Dict *thiz,
		const char *str)
{
	Egueb_Dom_Key_Dict_Entry *child;
	Eina_Unicode uni;
	int index = 0;

	uni = eina_unicode_utf8_next_get(str, &index);
	if (!uni)
		return EINA_FALSE;
		
	/* iterate over the roots to find the first char */
	EINA_INLIST_FOREACH(thiz->roots, child)
	{
		/* if found, continue with the next entry */
		if (child->k == uni)
		{
			return _egueb_dom_key_dict_exists_recursive(child,
					str + index);
		}
	}
	return EINA_FALSE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_dom_key_init(void)
{
	_non_printable = _egueb_dom_key_dict_new();
	/* add every non-printable string, http://www.w3.org/TR/DOM-Level-3-Events-key/ */
	/* Special keys */
	_egueb_dom_key_dict_add(_non_printable, "Unidentified");
	/* Modifier keys */
	_egueb_dom_key_dict_add(_non_printable, "Accel");
	_egueb_dom_key_dict_add(_non_printable, "Alt");
	_egueb_dom_key_dict_add(_non_printable, "AltGraph");
	_egueb_dom_key_dict_add(_non_printable, "CapsLock");
	_egueb_dom_key_dict_add(_non_printable, "Control");
	_egueb_dom_key_dict_add(_non_printable, "Fn");
	_egueb_dom_key_dict_add(_non_printable, "FnLock");
	_egueb_dom_key_dict_add(_non_printable, "Hyper");
	_egueb_dom_key_dict_add(_non_printable, "Meta");
	_egueb_dom_key_dict_add(_non_printable, "NumLock");
	_egueb_dom_key_dict_add(_non_printable, "OS");
	_egueb_dom_key_dict_add(_non_printable, "ScrollLock");
	_egueb_dom_key_dict_add(_non_printable, "Shift");
	_egueb_dom_key_dict_add(_non_printable, "Super");
	_egueb_dom_key_dict_add(_non_printable, "Symbol");
	_egueb_dom_key_dict_add(_non_printable, "SymbolLock");
	/* Whitespace keys */
	_egueb_dom_key_dict_add(_non_printable, "Enter");
	_egueb_dom_key_dict_add(_non_printable, "Separator");
	_egueb_dom_key_dict_add(_non_printable, "Tab");
	/* Navigation keys */
	_egueb_dom_key_dict_add(_non_printable, "ArrowDown");
	_egueb_dom_key_dict_add(_non_printable, "ArrowLeft");
	_egueb_dom_key_dict_add(_non_printable, "ArrowRight");
	_egueb_dom_key_dict_add(_non_printable, "ArrowUp");
	_egueb_dom_key_dict_add(_non_printable, "End");
	_egueb_dom_key_dict_add(_non_printable, "Home");
	_egueb_dom_key_dict_add(_non_printable, "PageDown");
	_egueb_dom_key_dict_add(_non_printable, "PageUp");
	/* Editing keys */
	_egueb_dom_key_dict_add(_non_printable, "BackSpace");
	_egueb_dom_key_dict_add(_non_printable, "Clear");
	_egueb_dom_key_dict_add(_non_printable, "Copy");
	_egueb_dom_key_dict_add(_non_printable, "CrSel");
	_egueb_dom_key_dict_add(_non_printable, "Cut");
	_egueb_dom_key_dict_add(_non_printable, "Delete");
	_egueb_dom_key_dict_add(_non_printable, "EraseEof");
	_egueb_dom_key_dict_add(_non_printable, "ExSel");
	_egueb_dom_key_dict_add(_non_printable, "Insert");
	_egueb_dom_key_dict_add(_non_printable, "Paste");
	_egueb_dom_key_dict_add(_non_printable, "Redo");
	_egueb_dom_key_dict_add(_non_printable, "Undo");
}

void egueb_dom_key_shutdown(void)
{
	_egueb_dom_key_dict_free(_non_printable);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_dom_key_is_printable(Egueb_Dom_String *s)
{
	if (!egueb_dom_string_is_valid(s))
		return EINA_FALSE;
	/* check if the string exists on the non_printable tree */
	return !_egueb_dom_key_dict_exists(_non_printable,
			egueb_dom_string_string_get(s));
}
