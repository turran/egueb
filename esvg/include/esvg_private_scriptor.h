#ifndef _ESVG_PRIVATE_SCRIPTOR_H_
#define _ESVG_PRIVATE_SCRIPTOR_H_

#ifdef __cplusplus
extern "C" {
#endif

/* the interface for handling scripting languages */
typedef struct _Esvg_Scriptor_Descriptor Esvg_Scriptor_Descriptor;
typedef void * (*Esvg_Scriptor_Context_New)(void);
typedef void (*Esvg_Scriptor_Context_Free)(void *ctx);
typedef void (*Esvg_Scriptor_Run)(void *ctx, const char *script);

struct _Esvg_Scriptor_Descriptor {
	Esvg_Scriptor_Context_New context_new;
	Esvg_Scriptor_Context_Free context_free;
	Esvg_Scriptor_Run run;
};

typedef struct _Esvg_Scriptor Esvg_Scriptor;

void esvg_scriptor_init(void);
void esvg_scriptor_shutdown(void);

void esvg_scriptor_register(Esvg_Scriptor_Descriptor *d, const char *type);
void esvg_scriptor_unregister(Esvg_Scriptor_Descriptor *d, const char *type);

Esvg_Scriptor * esvg_scriptor_new(Esvg_Scriptor_Descriptor *descriptor);
void esvg_scriptor_free(Esvg_Scriptor *thiz);
void esvg_scriptor_run(Esvg_Scriptor *thiz, const char *script);

#ifdef BUILD_ESVG_SCRIPTOR_V8
void esvg_scriptor_js_v8_init(void);
void esvg_scriptor_js_v8_shutdown(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
