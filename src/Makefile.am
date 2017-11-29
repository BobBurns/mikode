bin_PROGRAMS = mikode

mikode_SOURCES = tinyexpr_bitw.h common.h instructions.h ruheader.h main.c parse.c operand.c token.c misc.c write.c tinyexpr_bitw.c expression.c directive.c run.c load.c execute.c run_misc.c display.c

check_SCRIPTS = asmcheck.sh
TESTS = $(check_SCRIPTS)

asmcheck.sh:
	echo 'cp $(srcdir)/../examples/example1.asm ${PWD}; \
	./mikode example1.asm | grep "success!" ' > asmcheck.sh
	chmod +x asmcheck.sh

CLEANFILES = asmcheck.sh example1.asm example1 example1.asm.list
