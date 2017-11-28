bin_PROGRAMS = mikode mrun

mikode_SOURCES = tinyexpr_bitw.h common.h instructions.h main.c parse.c operand.c token.c misc.c write.c tinyexpr_bitw.c expression.c directive.c
mikode_LDADD = -lm

mrun_SOURCES = runheader.h run.c load.c execute.c run_misc.c display.c
mrun_LDADD = -lncurses

check_SCRIPTS = asmcheck.sh
TESTS = $(check_SCRIPTS)

example1.asm:


asmcheck.sh:
	echo 'cp $(srcdir)/../examples/example1.asm ${PWD}; \
	./mikode example1.asm | grep "success!" ' > asmcheck.sh
	chmod +x asmcheck.sh

CLEANFILES = asmcheck.sh example1.asm example1 example1.asm.list
