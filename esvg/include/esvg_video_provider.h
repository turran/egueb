#ifndef _ESVG_VIDEO_PROVIDER_H_
#define _ESVG_VIDEO_PROVIDER_H_

typedef struct _Esvg_Video_Provider Esvg_Video_Provider;

/* TODO Add functions for playing, pausing, seek, etc */
typedef void * (*Esvg_Video_Provider_New)(Edom_Tag *video);
typedef void (*Esvg_Video_Provider_Setup)(Edom_Tag *video, void *data);
typedef void (*Esvg_Video_Provider_Free)(Edom_Tag *video, void *data);

typedef struct _Esvg_Video_Provider_Descriptor
{
	Esvg_Video_Provider_New new;
	Esvg_Video_Provider_Free free;
	Esvg_Video_Provider_Setup setup;
} Esvg_Video_Provider_Descriptor;


#endif
