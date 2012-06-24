#include "Ecss.h"
#include <stdio.h>

int main(int argc, char **argv)
{
	Ecss_Style *s;

	ecss_init();

	s = ecss_style_load_from_file(argv[1]);
	printf("style = %p %s\n", s, argv[1]);
	ecss_shutdown();
	return 0;
}
