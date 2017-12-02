#include "common.h"
#include "tinyexpr_bitw.h"

/* helper val function */
int
val_label (line * in, char **b)
{

  char newtok[MAX_TOK];
  get_token (newtok, b);
  if (!strncmp (newtok, "low(", 4))
    {
      in->patch = PATCH_VALL;
    }
  else if (!strncmp (newtok, "high(", 5))
    {
      in->patch = PATCH_VALH;
    }
  else
    {
      return -1;
    }

  memset (newtok, 0, MAX_TOK);
  get_token (newtok, b);
  if (*((*b) - 1) != ')')
    {
      syntax_error ();
      return -1;
    }
  strncpy (in->patch_label, newtok, strlen (newtok) - 1);
  return 0;
}

/* return 0 on success result in val */
int
get_expr (line * in, int16_t * val, char **b)
{
  if (DEBUG)
    printf ("get_expr\n");
  /* check for low/high address */

  /* parse expression *
   * then make assignments from equates
   */

  char exprstr[100], c;
  int i = 0;
  while ((c = **b) != '\n')
    {
      if (c == ',' || c == ';' || i > MAX_LBL || c == '\0')
	break;
      exprstr[i++] = c;
      if (i > MAX_LBL)
	return -1;
      (*b)++;

    }
  (*b)--;
  exprstr[i] = '\0';

  /* get equates and make assignments */
  int bvars[255], err;
  te_variable *vars;
  vars = malloc (255 * sizeof (te_variable));
  /* initialize vars */
  vars = memset (vars, 0, sizeof (te_variable) * 255);

  int count = 0, ec = 0;
  for (i = 0; i < equate_count; i++)
    {
      vars[i].name = strndup (equates[i].key, MAX_LBL);
      vars[i].address = &bvars[i];
      bvars[i] = equates[i].int_val;
      count++;
    }

  te_expr *n = te_compile (exprstr, vars, count, &err);

  if (n)
    {
      *val = (int16_t) te_eval (n);
      if (DEBUG)
	printf ("result 0x%x\n", *val);
    }
  else
    {
      ec = -1;
    }
  for (i = 0; i < equate_count; i++)
    {
      free (vars[i].name);
    }

  free (vars);
  te_free (n);
  return ec;
}
