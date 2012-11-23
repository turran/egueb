#ifndef _ESVG_PRIVATE_VIDEO_H_
#define _ESVG_PRIVATE_VIDEO_H_

typedef void * (*Esvg_Video_New)(Edom_Tag *video);
typedef void (*Esvg_Video_Setup)(Edom_Tag *video, void *data);
typedef void (*Esvg_Video_Free)(Edom_Tag *video, void *data);

typedef struct _Esvg_Video_Descriptor
{
	Esvg_Video_New new;
	Esvg_Video_Free free;
	Esvg_Video_Setup setup;
} Esvg_Video_Descriptor;

void esvg_video_init(void);
void esvg_video_shutdown(void);

#endif
