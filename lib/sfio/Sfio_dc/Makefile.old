# makefile for discipline functions
#
# Written by Kiem-Phong Vo

# Native compiler if building libsfdisc.a for a cross compilation environment
NC=	cc

# Compiler and flags to use
LIBTYPE=""
CXFLAGS=
CCMODE=	-O
CCFLAGS= -I. -I.. $(CCMODE) $(LIBTYPE) $(CXFLAGS)
AR=	ar
CC=	cc

# sfdisc
SRCS=	sfdcdio.c sfdcdos.c sfdcfilter.c sfdclzw.c sfdcseekable.c sfdcslow.c \
	sfdcsubstream.c sfdctee.c sfdcunion.c
OBJS=	sfdcdio.o sfdcdos.o sfdcfilter.o sfdclzw.o sfdcseekable.o sfdcslow.o \
	sfdcsubstream.o sfdctee.o sfdcunion.o

.c.o:
	$(CC) -c $(CCFLAGS) $*.c

must:	$(OBJS)

clean:
	-(rm $(OBJS); exit 0) >/dev/null 2>&1
