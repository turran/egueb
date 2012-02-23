/* Edom - DOM
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
#ifndef _EDOM_H
#define _EDOM_H

#include <Eina.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EDOM_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EGUEB_BUILD */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

typedef struct _Edom_Parser Edom_Parser;
typedef struct _Edom_Tag Edom_Tag;
typedef struct _Edom_Context Edom_Context;

/* parser */
typedef Eina_Bool (*Edom_Parser_Tag_Get)(Edom_Parser *parser, const char *content, size_t len, int *tag);
typedef Edom_Tag * (*Edom_Parser_Tag_New)(Edom_Parser *parser, int tag);
typedef struct _Edom_Parser_Descriptor
{
	Edom_Parser_Tag_Get tag_get;
	Edom_Parser_Tag_New tag_new;
} Edom_Parser_Descriptor;

EAPI Edom_Parser * edom_parser_new(Edom_Parser_Descriptor *descriptor, void *data);
EAPI void edom_parser_delete(Edom_Parser *thiz);
EAPI void edom_parser_location_set(Edom_Parser *thiz, const char *location);
EAPI const char * edom_parser_location_get(Edom_Parser *thiz);
EAPI const char * edom_parser_root_get(Edom_Parser *thiz);
EAPI Eina_Bool edom_parser_parse(Edom_Parser *thiz, Edom_Context *doc, const char *content, size_t len);
EAPI void * edom_parser_data_get(Edom_Parser *thiz);
EAPI void edom_parser_context_push(Edom_Parser *thiz, Edom_Context *c);
EAPI Edom_Context * edom_parser_context_pop(Edom_Parser *thiz);

/* context */
typedef Eina_Bool (*Edom_Context_Tag_Open)(void *data, int tag, Edom_Context *context, const char *attrs, unsigned int attrs_length);
typedef void (*Edom_Context_Tag_Close)(void *data, int tag, Edom_Context *context);
typedef void (*Edom_Context_Data)(void *data, const char *cdata, unsigned int length);
typedef void (*Edom_Context_Cdata)(void *data, const char *cdata, unsigned int length);
typedef void (*Edom_Context_Free)(void *data);

typedef struct _Edom_Context_Descriptor
{
	Edom_Context_Tag_Open tag_open;
	Edom_Context_Tag_Close tag_close;
	Edom_Context_Data data;
	Edom_Context_Cdata cdata;
	Edom_Context_Free free;
} Edom_Context_Descriptor;

EAPI Edom_Context * edom_context_new(Edom_Parser *parser, Edom_Context_Descriptor *d, void *data);
EAPI void edom_context_delete(Edom_Context *c);
EAPI void * edom_context_data_get(Edom_Context *c);
EAPI Edom_Parser * edom_context_parser_get(Edom_Context *context);

/* tag */
typedef struct _Edom_Tag_Descriptor
{
	const char * (*name_get)(Edom_Tag *t);
	/* attribute handling */
	Eina_Bool (*attribute_set)(Edom_Tag *t, const char *attribute, const char *value);
	const char * (*attribute_get)(Edom_Tag *t, const char *attribute);
	/* child handling */
	Eina_Bool (*child_add)(Edom_Tag *t, Edom_Tag *child);
	Eina_Bool (*child_remove)(Edom_Tag *t, Edom_Tag *child);
	/* cdata */
	void (*cdata_set)(Edom_Tag *t, const char *cdata, unsigned int length);
	/* text */
	void (*text_set)(Edom_Tag *t, const char *text, unsigned int length);
	void (*tag_free)(Edom_Tag *t);
} Edom_Tag_Descriptor;

EAPI Edom_Tag * edom_tag_new(Edom_Context *c, Edom_Tag_Descriptor *d, int type,
		Edom_Tag *topmost,
		void *data);
EAPI void * edom_tag_data_get(Edom_Tag *thiz);
EAPI Edom_Context * edom_tag_context_get(Edom_Tag *thiz);
EAPI Edom_Parser * edom_tag_parser_get(Edom_Tag *thiz);

EAPI int edom_tag_type_get(Edom_Tag *thiz);
EAPI const char * edom_tag_name_get(Edom_Tag *thiz);
EAPI void edom_tag_id_set(Edom_Tag *thiz, const char *id);
EAPI const char * edom_tag_id_get(Edom_Tag *thiz);
EAPI void edom_tag_class_set(Edom_Tag *thiz, const char *class);
EAPI const char * edom_tag_class_get(Edom_Tag *thiz);

EAPI void edom_tag_child_add(Edom_Tag *thiz, Edom_Tag *child);
EAPI Edom_Tag * edom_tag_child_get(Edom_Tag *thiz);
EAPI Edom_Tag * edom_tag_next_get(Edom_Tag *thiz);
EAPI Edom_Tag * edom_tag_topmost_get(Edom_Tag *thiz);

EAPI void edom_tag_dump(Edom_Tag *thiz);
EAPI void edom_tag_attributes_from_xml(Edom_Tag *thiz,
		const char *attributes, unsigned int length);
EAPI Eina_Bool edom_tag_attribute_set(Edom_Tag *thiz, const char *key, const char *value);

#endif /*_EDOM_H*/
