#ifndef _ESVG_PRIVATE_VIDEO_PROVIDER_H_
#define _ESVG_PRIVATE_VIDEO_PROVIDER_H_

#ifdef __cplusplus
extern "C" {
#endif

Esvg_Video_Provider * esvg_video_provider_new(
		Esvg_Video_Provider_Descriptor *descriptor,
		Enesim_Renderer *image);
void esvg_video_provider_setup(Esvg_Video_Provider *thiz,
		const Esvg_Video_Provider_Context *ctx);
void esvg_video_provider_free(Esvg_Video_Provider *thiz);
void esvg_video_provider_play(Esvg_Video_Provider *thiz);

#if BUILD_ESVG_VIDEO_GSTREAMER
Esvg_Video_Provider_Descriptor esvg_video_provider_gstreamer_descriptor;
#endif

#ifdef __cplusplus
}
#endif

#endif

