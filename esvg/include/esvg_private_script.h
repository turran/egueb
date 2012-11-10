#ifndef _ESVG_PRIVATE_SCRIPT_H_
#define _ESVG_PRIVATE_SCRIPT_H_

/* the interface for handling scripting languages */
typedef struct _Esvg_Script_Descriptor Esvg_Script_Descriptor;

struct _Esvg_Script_Descriptor {
	/* create a new context */
	/* destroy the context */
	/* find a function */
};

void esvg_script_init(void);

#endif
