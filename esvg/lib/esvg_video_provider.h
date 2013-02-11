#ifndef _ESVG_ELEMENT_VIDEO_PROVIDER_H_
#define _ESVG_ELEMENT_VIDEO_PROVIDER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Esvg_Video_Provider Esvg_Video_Provider;
typedef struct _Esvg_Video_Provider_Descriptor Esvg_Video_Provider_Descriptor;
typedef struct _Esvg_Video_Provider_Context Esvg_Video_Provider_Context;

/* TODO Add functions for playing, pausing, seek, etc */
typedef void * (*Esvg_Video_Provider_Create)(Enesim_Renderer *image);
typedef void (*Esvg_Video_Provider_Setup)(void *data,
		const Esvg_Video_Provider_Context *ctx);
typedef void (*Esvg_Video_Provider_Free)(void *data);
typedef void (*Esvg_Video_Provider_Play)(void *data);

struct _Esvg_Video_Provider_Descriptor
{
	Esvg_Video_Provider_Create create;
	Esvg_Video_Provider_Free free;
	Esvg_Video_Provider_Setup setup;
	Esvg_Video_Provider_Play play;
};

struct _Esvg_Video_Provider_Context
{
	double width;
	double height;
	char *href;
};

#ifdef __cplusplus
}
#endif

#endif
