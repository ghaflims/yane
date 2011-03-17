#ifndef __YANE_H__
#define __YANE_H__

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "yaneconf.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif

#include "common.h"
#include "cpu.h"
#include "mem.h"
#include "ppu.h"
#include "ines.h"

#ifdef FALSE
# undef FALSE
#endif
#define FALSE 0

#ifdef TRUE
# undef TRUE
#endif
#define TRUE 1

#define VIDEO_USER_QUIT 1

typedef char yane_bool;

extern yane_bool flag_verbose;

int yane_video_init(int width, int height, yane_bool fullscreen);
int yane_video_deinit();
int yane_video_loop();

#endif /* not __YANE_H__ */

