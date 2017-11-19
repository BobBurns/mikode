#include "common.h"

/* returns with b indexed */
int issymbol(char c)
{
	if (c == '=' || c == ',' || c == ':' || c == ';' 
		|| c == '"' || c == '\'' || c == '(' || c == ')')
		return 1;
	else
		return 0;
}
int get_token(char *tok, char **b)
{
	char c;
	int i = 0, j = 0;

	/* get leading spaces */
	while(isspace(c = *(*b)))
	{
		if (i > MAX_BUF)
			return -1;
		if (c == '\n') {
			tok[0] = c; tok[1] = '\0';
			return 2;
		}
		i++;
		(*b)++;
	}
	//--i;
	/* handle = ( */
	while (!isspace(c = *(*b))) {
		if (i > MAX_BUF)
			return -1;
		tok[j] = c;
		(*b)++;
		i++;j++;
		if (issymbol(c)) {
			break;
		}
	}
	tok[j] = '\0';
	if (DEBUG) 
		printf("token: %s\n", tok);
	
	return i;
}
int get_patch_label(char *tok, char **b)
{
	char c;
	int i = 0, j = 0;

	/* get leading spaces */
	while(isspace(c = *(*b)))
	{
		if (i > MAX_BUF)
			return -1;
		if (c == '\n') {
			tok[0] = c; tok[1] = '\0';
			return 2;
		}
		i++;
		(*b)++;
	}
	/* handle = ( */
	while (i++ < MAX_BUF) {
		c = *(*b);
		tok[j] = c;
		j++;
		if (c == ',' || c == ';' || c == '\n') {
			break;
		}
		(*b)++;
	}
	/* terminate string */
	tok[j] = '\n', tok[j+1] = '\0';
	if (DEBUG) 
		printf("patch Label: %s\n", tok);
	
	return i;
}

int get_comment(line *in, char *b)
{
	int i = 0;
	while(((in->comment[i] = b[i])) != '\n')
		if (++i > MAX_DATA)
			return -1;
	/* terminate string */
	in->comment[i] = '\0';
	return 0;
}

