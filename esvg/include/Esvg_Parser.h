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
#ifndef _ESVG_PARSER_H
#define _ESVG_PARSER_H

#include <Eina.h>
#include <Enesim.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef ESVG_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_ESVG_BUILD */
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

#include <Edom.h>

typedef Eina_Bool (*Esvg_Parser_Tag_Get)(void *data, int *tag, const char *name, size_t length);
typedef Edom_Tag * (*Esvg_Parser_Tag_Open)(void *data, int tag, Edom_Context *context, Eina_Array *contexts);
typedef void (*Esvg_Parser_Tag_Is_Supported)(void *data, int tag, Edom_Context *context, Eina_Array *contexts);
typedef Eina_Bool (*Esvg_Parser_Tag_Attribute_Set)(void *data, Edom_Tag *tag, const char *attr, const char *value);
typedef char * (*Esvg_Parser_Tag_Attribute_Get)(void *data, Edom_Tag *tag, const char *attr);

typedef struct _Esvg_Parser_Descriptor
{
	Esvg_Parser_Tag_Get tag_get; /* whenever a tag name is found but esvg can not handle it */
	Esvg_Parser_Tag_Open tag_open; /* called whenever a tag is opened and esvg can not handle it */
	Esvg_Parser_Tag_Is_Supported tag_is_supported; /* called whenever a tag is found as a child of another but esvg can not recognize it as a valid child */
	Esvg_Parser_Tag_Attribute_Set tag_attribute_set; /* called whenever an attribute is not recognized by esvg */
	Esvg_Parser_Tag_Attribute_Get tag_attribute_get; /* called whenever an attribute is not recognized by esvg */
} Esvg_Parser_Descriptor;

EAPI Enesim_Renderer * esvg_parser_load(const char *filename, Esvg_Parser_Descriptor *descriptor, void *data);

#endif /*_ESVG_PARSER_H*/

