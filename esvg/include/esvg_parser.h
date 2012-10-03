#ifndef _ESVG_PARSER_H_
#define _ESVG_PARSER_H_

typedef void (*Esvg_Parser_Href_Set)(void *data, Enesim_Renderer *r, const char *href);
typedef struct _Esvg_Parser_Descriptor
{
	Esvg_Parser_Href_Set href_set;
} Esvg_Parser_Descriptor;


EAPI Ender_Element * esvg_parser_load(const char *filename, Esvg_Parser_Descriptor *descriptor, void *data);
EAPI Eina_Bool esvg_parser_info_load(const char *filename, Esvg_Length *width, Esvg_Length *height);
EAPI void esvg_parser_save(Enesim_Renderer *r, const char *filename);

#endif
