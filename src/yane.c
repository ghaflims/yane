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
const char *shortopts = "hv";
char *progname;

yane_bool flag_verbose = FALSE;

void
yane_usage()
{
	fprintf(stderr, "Usage: %s [options] file\n", progname);
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
			
			case -1:
				break;
			
			default:
				yane_usage();
				return 1;
		}
	}
	
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

	return 0;
}
