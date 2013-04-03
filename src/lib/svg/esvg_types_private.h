#ifndef _ESVG_PRIVATE_TYPES_H_
#define _ESVG_PRIVATE_TYPES_H_

#define ESVG_IS_HEXA(c) \
	(((c >= 48) && (c <= 57)) || \
	((c >= 65) && (c <= 70)) || \
	((c >= 97) && (c <= 102)))

#define ESVG_SPACE_SKIP(t) \
	while (*t) { if ((*t == ' ') || (*t == '\n') || (*t == '\t') || (*t == '\r')) t++; else break; }

#define ESVG_SPACE_COMMA_SKIP(t) \
	ESVG_SPACE_SKIP(t); \
	if (*t == ',') t++; \
	ESVG_SPACE_SKIP(t);

Eina_Bool esvg_long_get(const char *iter, const char **tmp, long *l);

void esvg_paint_copy(Esvg_Paint *dst, Esvg_Paint *src);

void esvg_type_init(void);
void esvg_type_shutdown(void);

#endif

