#include "runheader.h"



int run(uint8_t **prog)
{
	int ret;
	_win main_win;
	run_state state = {0};
	/* set up ncurses windowing */
	initscr();
	cbreak();
	refresh();
	nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
	noecho();
	curs_set(0);
	main_win.screen = newwin(SCR_MAXY, SCR_MAXX, 0, 0);
	main_win.text = newwin(5, SCR_MAXX, SCR_MAXY+1, 0);
	main_win.old_win = malloc(SCR_MAXX * SCR_MAXY);

	ret = execute(&state, prog, &main_win);
	getch();
	delwin(main_win.screen);
	delwin(main_win.text);
	endwin();
	free(main_win.old_win);
	if (ret < 0)
		printf("break received!");
	printf("sreg %02x ip:%02x sp:%02x r0:%02x r1:%02x r2:%02x "
			"r3:%02x r4:%02x r5:%02x r6:%02x r7:%02x \nr8:%02x r9:%02x "
			"r10:%02x r11:%02x r12:%02x r13:%02x r14:%02x r15:%02x\n\n", 
			state.sreg, state.ip, state.sp,
			state.reg[0], state.reg[1], state.reg[2], state.reg[3], 
			state.reg[4], state.reg[5], state.reg[6], state.reg[7], 
			state.reg[8], state.reg[9], state.reg[10], state.reg[11], 
			state.reg[12], state.reg[13], state.reg[14], state.reg[15]);
	printf("stack @ 0xff00:\n");
	uint16_t x;
	for (x = 0xff00; x ; x += 1)
		printf("%02x ", (*prog)[x]);
	printf("\n");

	printf("done\n");
	return 0;

}
int run_main (char* exec)
{
	int ret;
	
	uint8_t *prog;
	ret = load(exec, &prog);
	if (ret < 0)
		return -1;


	ret = run(&prog);
	if (ret < 0) {
		fprintf(stderr, "errors..\n");
		return -1;
	}

	return 0;

}
