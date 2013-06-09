#include "Egueb_Dom.h"
#include "Egueb_Smil.h"

void mylib_init(void)
{
	egueb_dom_init();
	egueb_smil_init();
}

void mylib_shutdown(void)
{
	egueb_dom_shutdown();
	egueb_smil_shutdown();
}
