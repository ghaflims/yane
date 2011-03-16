SUBDIRS = common nes lib6502

bin_PROGRAMS = yane
yane_SOURCES = yane.c yane.h yaneconf.h video.c
yane_CFLAGS = -Wall $(SDL_CFLAGS)
yane_CPPFLAGS = -I./common -I./nes -I./lib6502
yane_LDADD = nes/libnes.la common/libyanecommon.la lib6502/lib6502.la $(SDL_LDADD)

