#ifndef __INES_H__
#define __INES_H__

#include "yane.h"

typedef uint8_t ines_header_t[16];

int yane_ines_load(const char *file);
int yane_ines_unload();

#endif /* not __INES_H__ */

