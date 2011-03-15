SUBDIRS = common nes

bin_PROGRAMS = yane
yane_SOURCES = yane.c yane.h video.c
yane_CFLAGS = -Wall $(SDL_CFLAGS)
yane_CPPFLAGS = -I./common -I./nes
yane_LDADD = nes/libnes.la common/libyanecommon.la $(SDL_LDADD)

