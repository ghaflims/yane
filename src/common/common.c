#include "yane.h"
#include <stdarg.h>

void *
yane_malloc(size_t amt)
{
	void *ptr;
	
	ptr = malloc(amt);
	
	if(ptr == NULL)
	{
		yane_error("memory allocation failed.");
		abort();
	}
	
	return ptr;
}

void
yane_free(void *ptr)
{
	if(ptr == NULL)
	{
		if(flag_verbose)
			yane_warn("attempt to free a NULL pointer.");
	}
	else
		free(ptr);
}

int
yane_verbose(const char *subsyst, const char *fmt, ...)
{
	if(!flag_verbose)
		return 0;
	
	va_list ap;
	va_start(ap, fmt);
	printf("%s: ", subsyst);
	vprintf(fmt, ap);
	putchar('\n');
	va_end(ap);
	
	return 0;
}

