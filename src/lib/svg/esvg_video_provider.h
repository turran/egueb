#ifndef _ESVG_ELEMENT_VIDEO_PROVIDER_H_
#define _ESVG_ELEMENT_VIDEO_PROVIDER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Egueb_Svg_Video_Provider Egueb_Svg_Video_Provider;
typedef struct _Egueb_Svg_Video_Provider_Descriptor Egueb_Svg_Video_Provider_Descriptor;
typedef struct _Egueb_Svg_Video_Provider_Context Egueb_Svg_Video_Provider_Context;

/* TODO Add functions for playing, pausing, seek, etc */
typedef void * (*Egueb_Svg_Video_Provider_Create)(Enesim_Renderer *image);
typedef void (*Egueb_Svg_Video_Provider_Setup)(void *data,
		const Egueb_Svg_Video_Provider_Context *ctx);
typedef void (*Egueb_Svg_Video_Provider_Free)(void *data);
typedef void (*Egueb_Svg_Video_Provider_Play)(void *data);

struct _Egueb_Svg_Video_Provider_Descriptor
{
	Egueb_Svg_Video_Provider_Create create;
	Egueb_Svg_Video_Provider_Free free;
	Egueb_Svg_Video_Provider_Setup setup;
	Egueb_Svg_Video_Provider_Play play;
};

struct _Egueb_Svg_Video_Provider_Context
{
	double width;
	double height;
	char *href;
};

#ifdef __cplusplus
}
#endif

#endif
