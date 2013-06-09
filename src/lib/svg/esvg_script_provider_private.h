#ifndef _ESVG_PRIVATE_SCRIPT_PROVIDER_H_
#define _ESVG_PRIVATE_SCRIPT_PROVIDER_H_

#ifdef __cplusplus
extern "C" {
#endif

void egueb_svg_script_provider_init(void);
void egueb_svg_script_provider_shutdown(void);

void egueb_svg_script_provider_register(Egueb_Svg_Script_Provider_Descriptor *d, const char *type);
void egueb_svg_script_provider_unregister(Egueb_Svg_Script_Provider_Descriptor *d, const char *type);
Egueb_Svg_Script_Provider_Descriptor * egueb_svg_script_provider_descriptor_find(const char *type);

Egueb_Svg_Script_Provider * egueb_svg_script_provider_new(Egueb_Svg_Script_Provider_Descriptor *descriptor, Ender_Element *e);
void egueb_svg_script_provider_free(Egueb_Svg_Script_Provider *thiz);
void egueb_svg_script_provider_run(Egueb_Svg_Script_Provider *thiz, const char *script, Ender_Value *v);

#ifdef BUILD_ESVG_SCRIPTOR_V8
Egueb_Svg_Script_Provider_Descriptor egueb_svg_script_provider_js_v8_descriptor;
void egueb_svg_script_provider_js_v8_init(void);
void egueb_svg_script_provider_js_v8_shutdown(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
