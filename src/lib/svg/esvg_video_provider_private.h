#ifndef _ESVG_PRIVATE_VIDEO_PROVIDER_H_
#define _ESVG_PRIVATE_VIDEO_PROVIDER_H_

#ifdef __cplusplus
extern "C" {
#endif

Egueb_Svg_Video_Provider * egueb_svg_video_provider_new(
		Egueb_Svg_Video_Provider_Descriptor *descriptor,
		Enesim_Renderer *image);
void egueb_svg_video_provider_setup(Egueb_Svg_Video_Provider *thiz,
		const Egueb_Svg_Video_Provider_Context *ctx);
void egueb_svg_video_provider_free(Egueb_Svg_Video_Provider *thiz);
void egueb_svg_video_provider_play(Egueb_Svg_Video_Provider *thiz);

#if BUILD_ESVG_VIDEO_GSTREAMER
Egueb_Svg_Video_Provider_Descriptor egueb_svg_video_provider_gstreamer_descriptor;
#endif

#ifdef __cplusplus
}
#endif

#endif

