CC = gcc
CFLAGS = -g -Wall
LIBS = m
RLIBS = ncurses

machine := $(shell uname -s)
ifeq ($(machine),Darwin)
	CFLAGS += -fno-stack-protector
endif

all: mikode mrun

kodeobj = parse.o operand.o token.o misc.o write.o main.o tinyexpr_bitw.o expression.o directive.o

parse.o: parse.c common.h
operand.o: operand.c common.h
token.o: token.c common.h
misc.o: misc.c common.h
write.o: write.c common.h
main.o: main.c common.h
tinyexpr_bitw.o: tinyexpr_bitw.c tinyexpr_bitw.h
expression.o: expression.c common.h
directive..o: directive.c common.h

runobj = run.o load.o execute.o run_misc.o display.o

run.o: run.c runheader.h
load.o: load.c runheader.h
execute.o: execute.c runheader.h
run_misc.o: run_misc.c runheader.h
display.o: display.c runheader.h

#main rules

mikode: $(kodeobj)
	$(CC) $(CFLAGS) -o $@ $(kodeobj) -l$(LIBS)

mrun: $(runobj)
	$(CC) $(CFLAGS) -o $@ $(runobj) -l$(RLIBS)

check: all
	./mikode ../examples/example1.asm | grep "success!"
	@echo "*** ALL TESTS PASSED ***"

install:
	install -d $(DESTDIR)$(bindir)
	install -m 0755 mikode $(DESTDIR)$(bindir)
	install -m 0755 mrun $(DESTDIR)$(bindir)

uninstall:
	-rm $(DESTDIR)$(bindir)/mikode
	-rm $(DESTDIR)$(bindir)/mrun

.PHONY: clean all check install uninstall
clean: 
	rm -f mikode mrun $(kodeobj) $(runobj)

