#ifndef _ESVG_PRIVATE_INPUT_H_
#define _ESVG_PRIVATE_INPUT_H_

typedef Ender_Element * (*Esvg_Input_Element_At)(void *data, int x, int y);

typedef struct _Esvg_Input Esvg_Input;
typedef struct _Esvg_Input_Descriptor
{
	Esvg_Input_Element_At element_at;
} Esvg_Input_Descriptor;

Esvg_Input * esvg_input_new(Esvg_Input_Descriptor *descriptor, void *data);
void esvg_input_free(Esvg_Input *thiz);
void esvg_input_feed_mouse_move(Esvg_Input *thiz, int x, int y);
void esvg_input_feed_mouse_down(Esvg_Input *thiz, int button);
void esvg_input_feed_mouse_up(Esvg_Input *thiz, int button);

#endif

