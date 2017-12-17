mbase = tinyexpr_bitw.h common.h instructions.h runheader.h main.c parse.c opcode.c operand.c token.c misc.c write.c tinyexpr_bitw.c expression.c directive.c run.c load.c execute.c run_misc.c display.c

bin_PROGRAMS = mikode

mikode_CFLAGS = -g -Wall

if PI
mikode_SOURCES = $(mbase) gheader.h pi_gpio.c
else
if CHIP
mikode_SOURCES = $(mbase) gheader.h pi_gpio.c
else
mikode_SOURCES = $(mbase)
endif
endif

check_SCRIPTS = asmcheck.sh
TESTS = $(check_SCRIPTS)

asmcheck.sh:
	echo 'cp $(srcdir)/../examples/example1.asm ${PWD}; \
	./mikode example1.asm | grep "success!" ' > asmcheck.sh
	chmod +x asmcheck.sh

CLEANFILES = asmcheck.sh example1.asm example1 example1.asm.list
