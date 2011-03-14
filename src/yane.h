#ifndef __YANE_H__
#define __YANE_H__

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

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

#ifdef FALSE
# undef FALSE
#endif
#define FALSE 0

#ifdef TRUE
# undef TRUE
#endif
#define TRUE 1

typedef char yane_bool;

extern yane_bool flag_verbose;

#endif /* not __YANE_H__ */
