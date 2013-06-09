#ifndef _ESVG_PARSER_H_
#define _ESVG_PARSER_H_

EAPI Ender_Element * egueb_svg_parser_load(const char *filename);
EAPI Ender_Element * egueb_svg_parser_load_from_buffer(const char *buffer, size_t size);

EAPI Eina_Bool egueb_svg_parser_info_load(const char *filename,
		Egueb_Svg_Length *width, Egueb_Svg_Length *height);
EAPI Eina_Bool egueb_svg_parser_info_load_from_buffer(const char *buffer,
		size_t size, Egueb_Svg_Length *width, Egueb_Svg_Length *height);
EAPI void egueb_svg_parser_save(Enesim_Renderer *r, const char *filename);

#endif
