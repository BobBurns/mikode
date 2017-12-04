#include "runheader.h"
#include <time.h>		/* for rand seed */

int show_reg = 0;
int
run_io (uint16_t op, run_state * state, uint8_t ** prog, _rom * w)
{
  /* scan screen data space and print character to screen */
  int i, ch, r;
  uint8_t key;
  char *reg_str;
  for (i = 0; i < (SCR_MAXX * SCR_MAXY); i++)
    {
      /* only put if there is a change */
      if ((*prog)[i + SCR_OFF] != w->old_win[i])
	{
	  w->old_win[i] = (*prog)[i + SCR_OFF];
	  if ((*prog)[i + SCR_OFF] > 31)
	    {
	      mvwaddch (w->screen, i / SCR_MAXX, i % SCR_MAXX,
			(char) (*prog)[i + SCR_OFF]);
	    }
	}

    }
  wrefresh (w->screen);

  /* scan for input */
  ch = getch ();
  key = (uint8_t) ch;
  if (ch == KEY_F (1))
    {
      /* printt out reg info */
      /* toggle show registers */
      if (show_reg)
	show_reg = 0;
      else
	show_reg = 1;
    }
  else if (ch == KEY_F (2))
    {
      /* toggle show registers */
      wclear (w->text);
      wrefresh (w->text);
    }
  else if (ch == KEY_F (3))
    {
      /* break */
      wprintw (w->text, "break. press any key");
      getch ();
      return -1;
    }
  else if (ch == KEY_F (4))
    {
      /* stop execution */
      nodelay (stdscr, FALSE);
      wrefresh (w->text);
      getch ();
      nodelay (stdscr, TRUE);
    }
  else if (key <= (uint8_t) 0x7f)
    {
      /* store char */
      /* as int so that should be interesing */
      (*prog)[0xe000] = ch;
    }
  if (show_reg)
    {
//              wclear(w->text);
      reg_str = malloc (255);

      sprintf (reg_str,
	       "op %04x sreg %02x ip:%02x sp:%02x \nr0:%02x r1:%02x r2:%02x "
	       "r3:%02x r4:%02x r5:%02x r6:%02x r7:%02x r8:%02x r9:%02x "
	       "r10:%02x r11:%02x r12:%02x r13:%02x r14:%02x r15:%02x\n", op,
	       state->sreg, state->ip, state->sp, state->reg[0],
	       state->reg[1], state->reg[2], state->reg[3], state->reg[4],
	       state->reg[5], state->reg[6], state->reg[7], state->reg[8],
	       state->reg[9], state->reg[10], state->reg[11], state->reg[12],
	       state->reg[13], state->reg[14], state->reg[15]);

      mvwaddstr (w->text, 1, 0, reg_str);
      free (reg_str);
      wrefresh (w->text);

    }
  /* generate random number at 0xe004 */
  srand (time (NULL));
  r = rand ();
  (*prog)[0xe004] = (uint8_t) r;
  (*prog)[0xe005] = (uint8_t) (r >> 8);

  /* gpio */


#ifdef HAVE__OPT_VC_INCLUDE_BCM_HOST_H
  int ret;
  if (w->gpio_rom == 1)
    {
      uint8_t new_val = (*prog)[0xe100];
      uint8_t tst_val = 0;
      /* check gpio_out */
      if (new_val != w->old_rom[0])
        {
          tst_val = new_val ^ w->old_rom[0];
          for (i = 0; i < 8; i++)
    	    {
	      if (tst_val & (1 << i))
	        {
	          ret = gpio_write(i, ((new_val >> i) & 0x01));
	          if (ret == -1)
	            return ret;
	        } 
	    }
          w->old_rom[0] = new_val;
        }
    
      /* check gpio_dir */
      new_val = (*prog)[0xe101];
      tst_val = 0;
      if (new_val != w->old_rom[1])
        {
          tst_val = new_val ^ w->old_rom[1];
          for (i = 0; i < 8; i++)
	    {
	      if (tst_val & (1 << i))
	        {
	          ret = gpio_direction(i, ((new_val >> i) & 0x01));
	          if (ret == -1)
	            return ret; 
	        } 
	    }
              w->old_rom[0] = new_val;
        }

  /* read in gpio */
      ret = gpio_read();
      if (ret == -1)
        return ret;

      (*prog)[0xe102] = ret;
    }
#endif
    
  return 0;

}
