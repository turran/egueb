#ifndef _ESVG_PRIVATE_INPUT_H_
#define _ESVG_PRIVATE_INPUT_H_

typedef Ender_Element * (*Egueb_Svg_Input_Element_At)(void *data, int x, int y);

typedef struct _Egueb_Svg_Input Egueb_Svg_Input;
typedef struct _Egueb_Svg_Input_Descriptor
{
	Egueb_Svg_Input_Element_At element_at;
} Egueb_Svg_Input_Descriptor;

Egueb_Svg_Input * egueb_svg_input_new(Egueb_Svg_Input_Descriptor *descriptor, void *data);
void egueb_svg_input_free(Egueb_Svg_Input *thiz);
void egueb_svg_input_feed_mouse_move(Egueb_Svg_Input *thiz, int x, int y);
void egueb_svg_input_feed_mouse_down(Egueb_Svg_Input *thiz, int button);
void egueb_svg_input_feed_mouse_up(Egueb_Svg_Input *thiz, int button);

#endif

