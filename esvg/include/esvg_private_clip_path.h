#ifndef _ESVG_PRIVATE_CLIP_PATH_H_
#define _ESVG_PRIVATE_CLIP_PATH_H_

typedef struct _Esvg_Clip_Path_Referenceable_Data
{
	Ender_Element *g;
	Enesim_Renderer *referrer;
	Enesim_Renderer *proxy;
} Esvg_Clip_Path_Referenceable_Data;

void esvg_clip_path_init(void);

#endif

