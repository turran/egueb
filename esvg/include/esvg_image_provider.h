#ifndef _ESVG_ELEMENT_IMAGE_PROVIDER_H_
#define _ESVG_ELEMENT_IMAGE_PROVIDER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Esvg_Element_Image_Provider Esvg_Element_Image_Provider;
typedef struct _Esvg_Element_Image_Provider_Descriptor Esvg_Element_Image_Provider_Descriptor;

typedef void * (*Esvg_Element_Image_Provider_Create)(Enesim_Renderer *image);
typedef void (*Esvg_Element_Image_Provider_Load)(void *data);
typedef void (*Esvg_Element_Image_Provider_Free)(void *data);

struct _Esvg_Element_Image_Provider_Descriptor
{
	Esvg_Element_Image_Provider_Create create;
	Esvg_Element_Image_Provider_Free free;
	Esvg_Element_Image_Provider_Load load;
};

#ifdef __cplusplus
}
#endif

#endif

