SUBDIRS = common cpu

bin_PROGRAMS = yane
yane_SOURCES = yane.c yane.h
yane_CPPFLAGS = -I./common -I./cpu
yane_LDADD = cpu/lib6502.a common/libyane.a

