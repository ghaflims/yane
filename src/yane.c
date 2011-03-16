/* src/yane.c - YANE main function
 *
 * Copyright (c) 2011 Tosi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "yane.h"

const char *compiled_str = "compiled on " __DATE__ " at " __TIME__ ".";
const char *shortopts = "hvW:H:PN";
char *progname;
char *filename;

yane_bool flag_verbose = FALSE;
yane_bool flag_fullscreen = FALSE;

/* Emulation is NTSC by default, unless detected otherwise
   or forced by the user */
yane_bool flag_PAL = FALSE;
yane_bool flag_force_tv = FALSE;

int width = VIDEO_DEFAULT_WIDTH;
int height = VIDEO_DEFAULT_HEIGHT;

void
yane_usage()
{
	fprintf(stderr, "Usage: %s [options] file\n", progname);
	fprintf(stderr, "Options:\n"
		"   -h      Display this message\n"
		"   -v      Print extra information to stdout\n"
		"   -W px   Width of the display screen in pixels\n"
		"   -H px   Height of the display screen in pixels\n"
		"   -P      Force the NES to be PAL.\n"
		"   -N      Force the NES to be NTSC.\n"
	);
	fprintf(stderr, "\n%s %s\n", PACKAGE_STRING, compiled_str);
}

int
main(int argc, char **argv)
{
	int opt;
	
	progname = argv[0];
	opt = 0;
	while(opt != -1)
	{
		opt = getopt(argc, argv, shortopts);
		switch(opt)
		{
			case 'h':
				yane_usage();
				return 0;

			case 'v':
				flag_verbose = TRUE;
				break;
			
			case 'W':
				width = strtol(optarg, NULL, 10);
				break;
			
			case 'H':
				height = strtol(optarg, NULL, 10);
				break;
			
			case 'P':
				flag_PAL = TRUE;
				flag_force_tv = TRUE;
				break;
			
			case 'N':
				flag_PAL = FALSE;
				flag_force_tv = TRUE;
				break;
			
			case -1:
				break;
			
			default:
				yane_usage();
				return 1;
		}
	}
	
	if(optind >= argc)
	{
		fprintf(stderr, "%s: no input file\n", progname);
		yane_usage();
		return -1;
	}
	
	filename = argv[optind];
	
	if(yane_mem_init() < 0)
	{
		yane_error("failed to initialize memory");
		return -1;
	}

	if(yane_cpu_init() < 0)
	{
		yane_error("failed to initialize CPU");
		return -1;
	}
	
	if(yane_video_init(width, height, flag_fullscreen) < 0)
	{
		yane_error("failed to initialize video");
		return -1;
	}
	
	/* Main loop */
	for(;;)
	{
		int rv;
		
		rv = yane_video_loop();
		
		if(rv == VIDEO_USER_QUIT)
			break;
	}
	
	
	/* Deinitialization */
	yane_video_deinit();

	return 0;
}

