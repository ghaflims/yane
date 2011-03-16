/* src/video.c - YANE video subsystem
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
#include <SDL.h>

#define yane_sdl_error() \
	fprintf(stderr, "SDL: %s\n", SDL_GetError())

static SDL_Surface *screen = NULL;

int
yane_video_init(int width, int height, yane_bool fullscreen)
{
	const SDL_VideoInfo *vi;
	char namebuf[128];
	SDL_PixelFormat *vfmt;
	int flags, bpp;
	
	if(screen != NULL)
	{
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
		screen = NULL;
	}

	/* Be pedantic about not calling SDL_Init if it already was */
	if(!(SDL_WasInit(SDL_INIT_VIDEO) & SDL_INIT_VIDEO))
	{
		yane_verbose("VIDEO", "Setting mode to %ux%u", width, height);
		if(SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			yane_error("Call to SDL_Init failed.");
			yane_sdl_error();
			return -1;
		}
	}
	
	flags = 0;
	bpp = VIDEO_DEFAULT_BPP;

	vi = SDL_GetVideoInfo();

	if(vi == NULL)
	{
		yane_warn("Call to SDL_GetVideoInfo returned NULL");
		flags = SDL_SWSURFACE;
	}
	else
	{
		if(vi->hw_available)
		{
			yane_verbose("VIDEO", "detected support for hardware surfaces");
			flags |= (SDL_HWSURFACE | SDL_DOUBLEBUF);
		}
		else
			flags |= SDL_SWSURFACE;
		
		vfmt = vi->vfmt;
		
		/* Use the best bits per pixel for this system */
		bpp = vfmt->BitsPerPixel;
		yane_verbose(SDL_VideoDriverName(namebuf, 128), 
					"%u kb memory, %u bits per pixel", vi->video_mem, bpp);
	}
	
	if(fullscreen)
		flags |= SDL_FULLSCREEN;
	
	screen = SDL_SetVideoMode(width, height, bpp, flags);
	if(screen == NULL)
	{
		yane_error("Call to SDL_SetVideoMode failed.");
		yane_sdl_error();
		return -1;
	}
	
	SDL_WM_SetCaption(PACKAGE_STRING, NULL);

	return 0;
}

int
yane_video_deinit()
{
	SDL_Quit();
	
	return 0;
}

int
yane_video_loop()
{
	SDL_Event e;
	
	while(SDL_PollEvent(&e))
	{
		switch(e.type)
		{
			case SDL_QUIT:
				return VIDEO_USER_QUIT;
			
			default:
				break;
		}
	}

	return 0;
}

