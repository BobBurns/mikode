/* main entry and parse loop */

#include "common.h"
#include <unistd.h>

/* takes reference to line struct and allocated data and line buffer */
int
parse (line * in, uint8_t *** dp, char *b)
{
  memset (in, 0, sizeof (line));
  int ret, i = 0;
  char tok[MAX_TOK];

  /* is there a label */
  if (!isspace (b[0]))
    {
      /* could be a comment */
      if (b[0] == ';')
	{
	  if (get_comment (in, b) == -1)
	    {
	      fprintf (stderr, "bad comment\n");
	      return -1;
	    }
	  in->opcode = NOCODE;
	  return 0;
	}
      else if (b[0] == '.')
	{
	  if (!strncmp (b, ".def", 4))
	    {
	      /* could be .equ */
	      if (get_directive (b, DEF_T) == -1)
		{
		  fprintf (stderr, "bad directive\n");
		  return -1;
		}
	      in->opcode = NOCODE;
	      return 0;
	    }
	  else if (!strncmp (b, ".equ", 4))
	    {
	      /* could be .equ */
	      if (get_directive (b, EQU_T) == -1)
		{
		  fprintf (stderr, "bad directive\n");
		  return -1;
		}
	      in->opcode = NOCODE;
	      return 0;
	    }
	  else if (!strncmp (b, ".org", 4))
	    {
	      if (get_org (in, b) == -1)
		{
		  fprintf (stderr, "bad org\n");
		  return -1;
		}
	      in->opcode = NOCODE;
	      return 0;
	    }
	}

      /* must be a label */
      i = get_token (tok, &b);
      if (tok[0] == '\n')
	/*shouldn't happen */
	return 0;
      if (*(b - 1) != ':')
	{
	  fprintf (stderr, "missing : at end of label\n");
	  return -1;
	}
      strncpy (in->label, tok, strlen (tok));
    }
  /* reuse tok */
  memset (&tok, 0, MAX_TOK - 1);

  i += get_token (tok, &b);
  /* blank */
  if (tok[0] == '\n')
    {
      in->size = 0;
      in->opcode = NOCODE;
      return 0;
    }

  /* get opcode from mneumonic */
  if ((ret = getopcode (in, tok)) == -1)
    return -1;
  memset (&tok, 0, MAX_TOK);

  if ((ret = get_operand (in, &b, dp)) == -1)
    {
      syntax_error ();
      return -1;
    }
  memset (&tok, 0, MAX_TOK);

  /* check for comment */
  i += get_token (tok, &b);
  if (tok[0] == ';')
    if ((get_comment (in, --b)) == -1)
      {
	syntax_error ();
	return -1;
      }

  return 0;
}

int
get_expr_proxy (line * line, int16_t * val, char *b)
{
  int err;
  char *c = b;
  err = get_expr (line, val, &b);
  b = c;
  return err;
}

int
compile (char *filename)
{
  /* init globals */
  realloc_count = 1;
  equate_count = 0;
  def_count = 0;

  int ret, i = 0;
  uint32_t check;
  int count = 0;
  ssize_t nread;
  FILE *input_file;
  char c;

  /* get number lines to allocate line structs */
  if (strcmp(filename + (strlen(filename) - 4), ".asm") != 0)
    {
      fprintf(stderr, "Bad file format.  Please use .asm extension.\n");
      return -1;
    }

  if (access(filename, F_OK) == -1)
    {
      fprintf(stderr, "File %s does not exist!.\nAborting... \n", filename);
      return -1;
    }


  input_file = fopen (filename, "r");
  if (input_file == NULL)
    {
      fprintf (stderr, "Bad input file. Do you have read permissions?\n");
      return -1;
    }

  /* having trouble when vim is editing file */
  if ((fsync (fileno (input_file))) == -1)
    {
      perror ("fsync()");
      return -1;
    }


  /* count newlines to calculate lines to allocate */
  while ((c = fgetc (input_file)) != EOF)
    if (c == '\n')
      count++;

  /* allocate memory */
  line *in = malloc (sizeof (line) * count);
  if (in == NULL)
    {
      fprintf (stderr, "Bad malloc!\n");
      return -1;
    }

  rewind (input_file);

  char *line_buf = malloc (sizeof (char) * MAX_BUF);
  size_t len = MAX_BUF;
  /* allocat data (to be reallocated when needed */
  uint8_t **dp;
  dp = malloc (DATA_ALLOC * sizeof (uint8_t *));
  for (i = 0; i < DATA_ALLOC; i++)
    {
      dp[i] = (uint8_t *) malloc (MAX_DATA * sizeof (uint8_t));
      //      printf("allocated %d at index %d address %p\n", MAX_DATA, i, dp[i]);
    }

  /* pass 1 */
  in[0].address = 0x0000;
  i = 0;
  printf ("pass 1...\n");
  while ((nread = getline (&line_buf, &len, input_file)) != -1)
    {
      if (DEBUG)
	printf ("line: %d\n", i);

      ret = parse (&in[i], &dp, line_buf);
      if (ret == -1)
	{
	  fprintf (stderr, "Error around line %04x\n", i + 1);
	  goto fatal;
	}
      /* copy text */
      in[i].text = strndup (line_buf, MAX_DATA);

      /* get address */
      if (i > 0)
	{
	  if (!in[i].address)
	    {

	      check = in[i - 1].address + in[i - 1].size;
	      if (check > 0xffff)
		{
		  fprintf (stderr, "Address out of range: 0x%x\n",
			   in[i].address);
		  goto fatal;
		}
	      in[i].address = (uint16_t) check;
	    }
	}

      i++;
    }

  /* pass 2 patch label branch */
  printf ("pass 2...\n");
  int j, k = 0, err;
  uint16_t addr_from;
  int16_t addr_to;
  int16_t br_val;
  uint16_t kval, kH, kL;
  /* look for ops that need patching */
  for (k = 0; k < i; k++)
    {
      /* add label addresses to equates */
      if (strlen (in[k].label) > 0)
	{
	  in[k].label[strlen (in[k].label) - 1] = '\0';
	  if (DEBUG)
	    printf ("label put: [%s] \n", in[k].label);
	  err = put_equate (in[k].label, 0, in[k].address);
	  if (err < 0)
	    {
	      printf ("Error adding label to equate\n");
	      goto fatal;
	    }
	}
    }
  for (j = 0; j < i; j++)
    {
      if (in[j].patch)
	{
	  addr_from = in[j].address;
	  char *plabelc = malloc (sizeof (char) * MAX_LBL);
	  plabelc = strncpy (plabelc, in[j].patch_label, MAX_LBL);
	  //plabelc = strncpy(plabelc, plabel, MAX_LBL);
	  if (plabelc == NULL)
	    {
	      printf ("strncpy error\n");
	      goto fatal;
	    }
	  err = get_expr_proxy (&in[j], &addr_to, plabelc);
	  free (plabelc);
	  if (err < 0)
	    {
	      printf ("Invalid expression at %d\n", j + 1);
	      goto fatal;
	    }
	  /* loop over lines looking for address label */
	  if (in[j].patch == PATCH_BRANCH)
	    {
	      if (DEBUG)
		printf ("found branch label\n");
	      /* find address and get diff */
	      br_val = (int16_t) ((uint16_t) addr_to - addr_from);
	      if (DEBUG)
		printf ("branch value: %x\n", br_val);
	      if (br_val > 63 || br_val < -64)
		{
		  fprintf (stderr, "Branch value out of range!\n");
		  goto fatal;
		}
	      in[j].opcode =
		in[j].opcode | ((uint16_t) (br_val & 0x007f) << 3);
	    }
	  else if (in[j].patch == PATCH_DIR32)
	    {
	      in[j].opextra = (uint16_t) addr_to;	//in[k].address;
	    }
	  else if (in[j].patch == PATCH_VALL || in[j].patch == PATCH_IMM_BYTE)
	    {
	      //kval = (in[k].address & 0x00ff);
	      kval = ((uint16_t) addr_to & 0x00ff);
	      kH = (kval << 4) & 0x0f00;
	      kL = kval & 0x000f;
	      in[j].opcode |= kH | kL;
	    }
	  else if (in[j].patch == PATCH_VALH)
	    {
	      //kval = (in[k].address & 0xff00);
	      kval = ((uint16_t) addr_to & 0xff00);
	      kH = (kval >> 4) & 0x0f00;
	      kL = (kval >> 8) & 0x000f;
	      in[j].opcode |= kH | kL;
	      //break;
	    }
	  else if (in[j].patch == PATCH_IMM_WORD)
	    {
	      kval = (uint16_t) addr_to;
	      if ((char) kval > 63 || (char) kval < 0)
		{
		  printf ("Value out of range\n");
		  goto fatal;
		}
	      kH = ((kval << 2) & 0x00c0);
	      kL = (kval & 0x000f);
	      in[j].opcode |= kH | kL;
	    }

	}
    }

  /* write listing and bin */
  ret = write_listing (&in, &dp, i, filename);
  if (ret < 0)
    {
      fprintf (stderr, "Problem with write_listing\n");
      goto fatal;
    }
  ret = write_asm (&in, &dp, i, filename);
  if (ret < 0)
    {
      fprintf (stderr, "Problem with write_asm\n");
      goto fatal;
    }

  printf ("success!\n");
  /* free the pointers */

fatal:
  free (line_buf);
  for (i = 0; i < count; i++)
    free (in[i].text);
  free (in);
  for (i = 0; i < (DATA_ALLOC); i++)
    {
      free (dp[i]);
    }
  free (dp);			/* i might have to free each pointer */
  fclose (input_file);
  return 0;
}
