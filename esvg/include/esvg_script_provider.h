#ifndef _ESVG_SCRIPT_PROVIDER_H_
#define _ESVG_SCRIPT_PROVIDER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Esvg_Script_Provider Esvg_Script_Provider;
typedef struct _Esvg_Script_Provider_Descriptor Esvg_Script_Provider_Descriptor;

typedef void * (*Esvg_Script_Provider_Context_New)(Ender_Element *e);
typedef void (*Esvg_Script_Provider_Context_Free)(void *ctx);
typedef void (*Esvg_Script_Provider_Run)(void *ctx, const char *script);

struct _Esvg_Script_Provider_Descriptor {
	Esvg_Script_Provider_Context_New context_new;
	Esvg_Script_Provider_Context_Free context_free;
	Esvg_Script_Provider_Run run;
};

#ifdef __cplusplus
}
#endif

#endif
