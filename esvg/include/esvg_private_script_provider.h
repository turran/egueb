#ifndef _ESVG_PRIVATE_SCRIPT_PROVIDER_H_
#define _ESVG_PRIVATE_SCRIPT_PROVIDER_H_

#ifdef __cplusplus
extern "C" {
#endif

void esvg_script_provider_init(void);
void esvg_script_provider_shutdown(void);

void esvg_script_provider_register(Esvg_Script_Provider_Descriptor *d, const char *type);
void esvg_script_provider_unregister(Esvg_Script_Provider_Descriptor *d, const char *type);
Esvg_Script_Provider_Descriptor * esvg_script_provider_descriptor_find(const char *type);

Esvg_Script_Provider * esvg_script_provider_new(Esvg_Script_Provider_Descriptor *descriptor, Ender_Element *e);
void esvg_script_provider_free(Esvg_Script_Provider *thiz);
void esvg_script_provider_run(Esvg_Script_Provider *thiz, const char *script);

#ifdef BUILD_ESVG_SCRIPTOR_V8
Esvg_Script_Provider_Descriptor esvg_script_provider_js_v8_descriptor;
void esvg_script_provider_js_v8_init(void);
void esvg_script_provider_js_v8_shutdown(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
