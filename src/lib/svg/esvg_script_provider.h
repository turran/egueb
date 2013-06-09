#ifndef _ESVG_SCRIPT_PROVIDER_H_
#define _ESVG_SCRIPT_PROVIDER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Egueb_Svg_Script_Provider Egueb_Svg_Script_Provider;
typedef struct _Egueb_Svg_Script_Provider_Descriptor Egueb_Svg_Script_Provider_Descriptor;

typedef void * (*Egueb_Svg_Script_Provider_Context_New)(Ender_Element *e);
typedef void (*Egueb_Svg_Script_Provider_Context_Free)(void *ctx);
typedef void (*Egueb_Svg_Script_Provider_Run)(void *ctx, const char *script, Ender_Value *v);

struct _Egueb_Svg_Script_Provider_Descriptor {
	Egueb_Svg_Script_Provider_Context_New context_new;
	Egueb_Svg_Script_Provider_Context_Free context_free;
	Egueb_Svg_Script_Provider_Run run;
};

#ifdef __cplusplus
}
#endif

#endif
