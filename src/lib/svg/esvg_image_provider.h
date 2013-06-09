#ifndef _ESVG_ELEMENT_IMAGE_PROVIDER_H_
#define _ESVG_ELEMENT_IMAGE_PROVIDER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Egueb_Svg_Element_Image_Provider Egueb_Svg_Element_Image_Provider;
typedef struct _Egueb_Svg_Element_Image_Provider_Descriptor Egueb_Svg_Element_Image_Provider_Descriptor;

typedef void * (*Egueb_Svg_Element_Image_Provider_Create)(Enesim_Renderer *image);
typedef void (*Egueb_Svg_Element_Image_Provider_Load)(void *data);
typedef void (*Egueb_Svg_Element_Image_Provider_Free)(void *data);

struct _Egueb_Svg_Element_Image_Provider_Descriptor
{
	Egueb_Svg_Element_Image_Provider_Create create;
	Egueb_Svg_Element_Image_Provider_Free free;
	Egueb_Svg_Element_Image_Provider_Load load;
};

#ifdef __cplusplus
}
#endif

#endif

