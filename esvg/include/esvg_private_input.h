#ifndef _ESVG_PRIVATE_INPUT_H_
#define _ESVG_PRIVATE_INPUT_H_

/* input */
typedef struct _Esvg_Input Esvg_Input;
Esvg_Input * esvg_input_new(Edom_Tag *t);
void esvg_input_feed_mouse_move(Esvg_Input *thiz, double x, double y);

#endif

