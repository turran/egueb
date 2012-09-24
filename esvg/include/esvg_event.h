#ifndef _ESVG_INPUT_H_
#define _ESVG_INPUT_H_

typedef struct _Esvg_Event_Mouse
{
	int screen_x;
	int screen_y;
	int client_x;
	int client_y;
	int button;
	Ender_Element *related_target;
	/* TODO
	 *  readonly attribute boolean          ctrlKey;
	 *  readonly attribute boolean          shiftKey;
	 *  readonly attribute boolean          altKey;
	 *  readonly attribute boolean          metaKey;
	 */
} Esvg_Event_Mouse;

#endif
