#include "Egueb_Css.h"
#include <stdio.h>

int main(int argc, char **argv)
{
	Egueb_Css_Style *s;

	egueb_css_init();

	s = egueb_css_style_load_from_file(argv[1]);
	printf("style = %p %s\n", s, argv[1]);
	egueb_css_shutdown();
	return 0;
}
