# Makefile for main.c

#Compiler and Linker
CC = gcc

#The Target Binary
PETOOL = PETool

#The Directories, Source, Includes, Objects, Binary and Resources
BUILDDIR    := obj
INCDIR      := .
SRCDIR      := .
INC         := -I$(INCDIR)
SRCS = main.c

OBJS = $(patsubst %.c, $(BUILDDIR)/%.o, $(SRCS))

#flags, Libraries and Includes
CFLAGS = -fPIC -Wall -Wextra -O2 -g -std=c11 -I$(INCDIR)
LDFLAGS = -g -lpetool -L.

#Other Definitions
@RM = rm -f

#Default make
all: directories ${PETOOL}

#Make the Directories
directories:
	@mkdir -p $(BUILDDIR)

$(PETOOL):$(OBJS)
	$(CC) -o $@ $^ ${LDFLAGS}

$(BUILDDIR)/%.o:$(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<


clean:
	-${RM} ${PETOOL} ${OBJS}

.PHONY: all clean
