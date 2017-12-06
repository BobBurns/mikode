#include "runheader.h"

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

int
run (uint8_t ** prog, int usage_flag)
{
  int ret;
  _rom main_rom;
  run_state state = { 0 };
  /* set up gpio */
#ifdef HAVE__OPT_VC_INCLUDE_BCM_HOST_H
  if (usage_flag == 3)
    {
      if (getuid() != 0)
        {
          fprintf(stderr, "must run as root for gpio access!\n");
          return -1;
        }
      main_rom.old_rom = malloc (3);
      main_rom.gpio_rom = 1;
      ret = gpio_init();
      if (ret == -1)
        { 
          free(main_rom.old_rom);
          return ret;
        }
    }
#endif
  /* set up ncurses windowing */
  initscr ();
  /* capture control c to unexport gpio when exiting */
  cbreak ();
  raw();

  refresh ();
  nodelay (stdscr, TRUE);
  keypad (stdscr, TRUE);
  noecho ();
  curs_set (0);
  main_rom.screen = newwin (SCR_MAXY, SCR_MAXX, 0, 0);
  main_rom.text = newwin (5, SCR_MAXX, SCR_MAXY + 1, 0);
  main_rom.old_win = malloc (SCR_MAXX * SCR_MAXY);


  ret = execute (&state, prog, &main_rom);
  getch ();
  delwin (main_rom.screen);
  delwin (main_rom.text);
  endwin ();
  free (main_rom.old_win);
#ifdef HAVE__OPT_VC_INCLUDE_BCM_HOST_H
  if (main_rom.gpio_rom == 1)
    {
      free (main_rom.old_rom);
      if ((gpio_close()) == -1)
	{ 
	  printf("could not unexport gpio\n");
	}
    }
#endif
  if (ret < 0)
    printf ("break received!");
  printf ("sreg %02x ip:%02x sp:%02x r0:%02x r1:%02x r2:%02x "
	  "r3:%02x r4:%02x r5:%02x r6:%02x r7:%02x \nr8:%02x r9:%02x "
	  "r10:%02x r11:%02x r12:%02x r13:%02x r14:%02x r15:%02x\n\n",
	  state.sreg, state.ip, state.sp,
	  state.reg[0], state.reg[1], state.reg[2], state.reg[3],
	  state.reg[4], state.reg[5], state.reg[6], state.reg[7],
	  state.reg[8], state.reg[9], state.reg[10], state.reg[11],
	  state.reg[12], state.reg[13], state.reg[14], state.reg[15]);
  printf ("stack @ 0xff00:\n");
  uint16_t x;
  for (x = 0xff00; x; x += 1)
    printf ("%02x ", (*prog)[x]);
  printf ("\n");

  printf ("done\n");
  return 0;

}

int
run_main (char *exec, int usage_flag, int slp_v)
{
  int ret;

  uint8_t *prog;
  sleep_v = slp_v;
  ret = load (exec, &prog);
  if (ret < 0)
    return -1;

  ret = run (&prog, usage_flag);
  if (ret < 0)
    {
      fprintf (stderr, "errors..\n");
      return -1;
    }

  return 0;

}
