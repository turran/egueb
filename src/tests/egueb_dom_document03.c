/* Creates a document, process it */
#include "egueb_dom_mylib.h"
#include <unistd.h>
#include <signal.h>

static int run = 1;

static void help(void)
{
	printf("Usage: egueb_dom_document03 FILE.xml\n");
	exit(1);
}

static void _timer_signal_cb(int s)
{
}

static void _abort_signal_cb(int s)
{
	run = 0;
}

static void _signal_setup(void)
{
	struct sigaction sact;
	struct itimerval value;

	/* create the timer callback */
	sact.sa_flags = 0;
	sact.sa_handler = _timer_signal_cb;

	value.it_interval.tv_sec = 0;
	value.it_interval.tv_usec = 33333; /* every 33333 (1/30fps) usecs */
	value.it_value.tv_sec = 0;
	value.it_value.tv_usec = 500000; /* wait 500 usecs, before triggering the first event */
	sigaction(SIGALRM, &sact, NULL);
	setitimer(ITIMER_REAL, &value, NULL);
}

int main(int argc, char **argv)
{
	Egueb_Dom_Node *mydocument;
	Egueb_Dom_Node *root = NULL;
	Enesim_Stream *im;

	if (argc < 2) help();

	mylib_init();
	mydocument = mydocument_new();
	im = enesim_stream_file_new(argv[1], "r+");
	if (!im) return -1;

	egueb_dom_parser_parse(im, &mydocument);
	enesim_stream_unref(im);

	/* setup our own handlers */
	_signal_setup();

	/* process it */
	while (run)
	{
		pause();
		if (!run) break;
		egueb_dom_document_process(mydocument);
	}
	egueb_dom_node_unref(mydocument);

	mylib_shutdown();

	return 0;
}


