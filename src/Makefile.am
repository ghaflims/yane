SUBDIRS = common cpu

bin_PROGRAMS = yane
yane_SOURCES = yane.c yane.h video.c
yane_CFLAGS = -Wall $(SDL_CFLAGS)
yane_CPPFLAGS = -I./common -I./cpu
yane_LDADD = cpu/lib6502.a common/libyane.a $(SDL_LDADD)

