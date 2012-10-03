#ifndef _ESVG_PARSER_H_
#define _ESVG_PARSER_H_

EAPI Ender_Element * esvg_parser_load(const char *filename);
EAPI Eina_Bool esvg_parser_info_load(const char *filename, Esvg_Length *width, Esvg_Length *height);
EAPI void esvg_parser_save(Enesim_Renderer *r, const char *filename);

#endif
