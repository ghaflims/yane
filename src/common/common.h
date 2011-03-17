#ifndef __YANE_COMMON_H__
#define __YANE_COMMON_H__

#include "yane.h"

#define yane_error(error) \
	fprintf(stderr, "ERROR: %s:%u: %s\n", __FILE__, __LINE__, error)

#define yane_warn(warning) \
	fprintf(stderr, "WARNING: %s:%u: %s\n", __FILE__, __LINE__, warning)

void *yane_malloc(size_t amt);
void yane_free(void *ptr);
int yane_verbose(const char *subsyst, const char *fmt, ...);
FILE *yane_fopen(const char *fname, const char *mode);

#endif /* not __YANE_COMMON_H__ */

