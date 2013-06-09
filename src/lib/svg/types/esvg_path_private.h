#ifndef _ESVG_PATH_PRIVATE
#define _ESVG_PATH_PRIVATE

struct _Egueb_Svg_Path_Seg_List
{
	Ender_Element *owner;
	Eina_List *commands;
	Eina_Bool changed;
	int ref;
};

#endif
