#ifndef _ESVG_PRIVATE_CLIP_PATH_H_
#define _ESVG_PRIVATE_CLIP_PATH_H_

typedef struct _Egueb_Svg_Clip_Path_Referenceable_Data
{
	Ender_Element *g;
	Enesim_Renderer *referrer;
	Enesim_Renderer *proxy;
} Egueb_Svg_Clip_Path_Referenceable_Data;

void egueb_svg_clip_path_init(void);
void egueb_svg_clip_path_shutdown(void);

#endif

