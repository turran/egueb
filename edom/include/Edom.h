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

/* parser */
typedef Eina_Bool (*Edom_Parser_Tag_Get)(Edom_Parser *parser, const char *content, size_t len, int *tag);
typedef void * (*Edom_Parser_Tag_New)(Edom_Parser *parser, int tag);
typedef void * (*Edom_Parser_Topmost_Get)(Edom_Parser *parser);
typedef Eina_Bool (*Edom_Parser_Tag_Attribute_Set)(Edom_Parser *parser, void *t, const char *attribute, const char *value);
typedef Eina_Bool (*Edom_Parser_Tag_Child_Add)(Edom_Parser *parser, void *t, void *child);
typedef void (*Edom_Parser_Tag_Cdata_Set)(Edom_Parser *parser, void *t, const char *cdata, unsigned int length);
typedef void (*Edom_Parser_Tag_Text_Set)(Edom_Parser *parser, void *t, const char *text, unsigned int length);

typedef struct _Edom_Parser_Descriptor
{
	Edom_Parser_Tag_Get tag_get;
	Edom_Parser_Tag_New tag_new;
	Edom_Parser_Topmost_Get topmost_get;
	Edom_Parser_Tag_Attribute_Set tag_attribute_set;
	Edom_Parser_Tag_Child_Add tag_child_add;
	Edom_Parser_Tag_Cdata_Set tag_cdata_set;
	Edom_Parser_Tag_Text_Set tag_text_set;
} Edom_Parser_Descriptor;

EAPI Edom_Parser * edom_parser_new(Edom_Parser_Descriptor *descriptor, void *data);
EAPI void edom_parser_delete(Edom_Parser *thiz);
EAPI void edom_parser_location_set(Edom_Parser *thiz, const char *location);
EAPI const char * edom_parser_location_get(Edom_Parser *thiz);
EAPI const char * edom_parser_root_get(Edom_Parser *thiz);
EAPI Eina_Bool edom_parser_parse(Edom_Parser *thiz, const char *content, size_t len);
EAPI void * edom_parser_data_get(Edom_Parser *thiz);
EAPI void * edom_parser_topmost_get(Edom_Parser *thiz);

/* tag */
typedef struct _Edom_Attribute
{
	const char *name;
	char *value;
} Edom_Attribute;

typedef Eina_Bool (*Edom_Tag_Foreach)(Edom_Tag *thiz, Edom_Tag *child, void *data);
typedef const char * (*Edom_Tag_Name_Get)(Edom_Tag *t);
typedef Eina_Bool (*Edom_Tag_Attribute_Set)(Edom_Tag *t, const char *attribute, const char *value);
typedef Eina_Bool (*Edom_Tag_Attribute_Get)(Edom_Tag *t, const char *attribute, char **value);
typedef Eina_Bool (*Edom_Tag_Child_Add)(Edom_Tag *t, Edom_Tag *child);
typedef Eina_Bool (*Edom_Tag_Child_Remove)(Edom_Tag *t, Edom_Tag *child);
typedef void (*Edom_Tag_Cdata_Set)(Edom_Tag *t, const char *cdata, unsigned int length);
typedef void (*Edom_Tag_Text_Set)(Edom_Tag *t, const char *text, unsigned int length);
typedef void (*Edom_Tag_Free)(Edom_Tag *t);

typedef struct _Edom_Tag_Descriptor
{
	Edom_Tag_Name_Get name_get;
	/* attribute handling */
	Edom_Tag_Attribute_Set attribute_set;
	Edom_Tag_Attribute_Get attribute_get;
	/* child handling */
	Edom_Tag_Child_Add child_add;
	Edom_Tag_Child_Remove child_remove;
	/* cdata */
	Edom_Tag_Cdata_Set cdata_set;
	/* text */
	Edom_Tag_Text_Set text_set;
	Edom_Tag_Free free;
} Edom_Tag_Descriptor;

EAPI Edom_Tag * edom_tag_new(Edom_Tag_Descriptor *d, void *data);
EAPI void * edom_tag_data_get(Edom_Tag *thiz);

EAPI const char * edom_tag_name_get(Edom_Tag *thiz);

EAPI Eina_Bool edom_tag_attribute_set(Edom_Tag *thiz, const Edom_Attribute *attr);
EAPI Eina_Bool edom_tag_attribute_get(Edom_Tag *thiz, Edom_Attribute *attr);

EAPI void edom_tag_id_set(Edom_Tag *thiz, const char *id);
EAPI char * edom_tag_id_get(Edom_Tag *thiz);
EAPI void edom_tag_class_set(Edom_Tag *thiz, const char *class);
EAPI char * edom_tag_class_get(Edom_Tag *thiz);

EAPI Eina_Bool edom_tag_child_add(Edom_Tag *thiz, Edom_Tag *child);
EAPI Edom_Tag * edom_tag_child_get(Edom_Tag *thiz);
EAPI void edom_tag_child_foreach(Edom_Tag *thiz, Edom_Tag_Foreach foreach, void *data);
EAPI Edom_Tag * edom_tag_next_get(Edom_Tag *thiz);
EAPI Edom_Tag * edom_tag_parent_get(Edom_Tag *thiz);
EAPI Edom_Tag * edom_tag_topmost_get(Edom_Tag *thiz);

EAPI void edom_tag_dump(Edom_Tag *thiz);
EAPI void edom_tag_attributes_from_xml(Edom_Tag *thiz,
		const char *attributes, unsigned int length);

#endif /*_EDOM_H*/
