CC = gcc
CFLAGS = -g -Wall
PREFIX = /usr/local
DESTDIR = /tmp/stage
LIBS = m
RLIBS = ncurses
TDIR = ../tefork/tinyexpr/

# handle MacOSx this way Im not sure why
machine := $(shell uname -m)
ifeq ($(machine),Darwin)
	CFLAGS += -fno-stack-protector
endif

all: mikode mikorun

mikode: parse.o operand.o token.o misc.o write.o main.o $(TDIR)tinyexpr_bitw.o expression.o directive.o
	$(CC) -o mikode parse.o operand.o token.o misc.o write.o main.o $(TDIR)tinyexpr_bitw.o expression.o directive.o -l$(LIBS)

.PHONY: clean
clean:
	rm -f parse.o operand.o token.o misc.o write.o main.o $(TDIR)tinyexpr_bitw.o expression.o directive.o run.o load.o execute.o run_misc.o display.o

mikorun: run.o load.o execute.o run_misc.o display.o
	$(CC) -o mikorun run.o load.o execute.o run_misc.o display.o -l$(RLIBS)


