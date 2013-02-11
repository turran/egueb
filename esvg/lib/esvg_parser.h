#ifndef _ESVG_PARSER_H_
#define _ESVG_PARSER_H_

EAPI Ender_Element * esvg_parser_load(const char *filename);
EAPI Ender_Element * esvg_parser_load_from_buffer(const char *buffer, size_t size);

EAPI Eina_Bool esvg_parser_info_load(const char *filename,
		Esvg_Length *width, Esvg_Length *height);
EAPI Eina_Bool esvg_parser_info_load_from_buffer(const char *buffer,
		size_t size, Esvg_Length *width, Esvg_Length *height);
EAPI void esvg_parser_save(Enesim_Renderer *r, const char *filename);

#endif
