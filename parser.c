#include <stdio.h>
#include <string.h> /* for all the new-fangled string functions */
#include <stdlib.h> /* malloc, free, rand */

#define F_SIZE 50 /* max string length of formulas*/
#define INPUTS 10

/* Define propositional symbols*/
#define P 'p'
#define Q 'q'
#define R 'r'
#define OR 'v'
#define AND '^'
#define IMPLIES '>'
#define NOT '-'
#define LPAREN '('
#define RPAREN ')'
#define END '\n'

/* Define globals */
static int g_index;

/*put all your functions here.  You will need
1.
int parse(char *g) which returns 1 if a proposition, 2 if neg, 3 if binary, ow 0
Of course you will almost certainly need other functions.

For binary formulas you will also need functions that return the first part and the second part of the binary formula.

char *partone(char *g)

char *parttwo(char *g)


You may vary this program provided it reads 10 formulas in a file called "input.txt" and outputs in the way indicated below to a file called "output.txt".
*/

// prop ::= p|q|r.
// BC ::= v | ^ | > .
// fmla ::= prop | − fmla | (fmla BC fmla).

/*
  current |  next
  ----------------------------
  prop    |  BC, \n
  ----------------------------
  fmla    |  ), BC, \n
  -----------------------------
  BC      |  (, prop, -,  
*/

int match(char *str, char symbol)
{
  if (str[g_index] == symbol)
  {
    g_index++;
    return 1;
  }
  return 0;
}

int matchProposition(char *str)
{
  return match(str, P) || match(str, Q) || match(str, R);
}

int matchBinaryConnector(char *str)
{
  return match(str, OR) || match(str, AND) || match(str, IMPLIES);
}

int matchFormula(char *str)
{
  if (matchProposition(str))
  {
    return 1;
  }
  else if (match(str, NOT))
  {
    if (!matchFormula(str))
      return 0;
    return 1;
  }
  else if (match(str, LPAREN))
  {
    if (!matchFormula(str))
      return 0;
    if (!matchBinaryConnector(str))
      return 0;
    if (!matchFormula(str))
      return 0;
    if (!match(str, RPAREN))
      return 0;
    return 1;
  }
  return 0;
}

int parse(char *str)
{
  g_index = 0;
  if (match(str, END))
  {
    return 0;
  }
  if (!matchFormula(str))
    return 0;
  if (!match(str, END))
    return 0;
  return 1;
}

int main()
{
  char *str = malloc(F_SIZE);
  FILE *fp, *fpout;

  if ((fp = fopen("input.txt", "r")) == NULL)
  {
    printf("Error opening file");
    exit(1);
  }
  if ((fpout = fopen("output.txt", "w")) == NULL)
  {
    printf("Error opening file");
    exit(1);
  }

  int j;
  for (j = 0; j < INPUTS; j++)
  {
    if (fscanf(fp, "%s", str))
    {
      switch (parse(str))
      {
      case (0):
        fprintf(fpout, "%s is not a formula.  \n", str);
        break;
      case (1):
        fprintf(fpout, "%s is a formula.  \n", str);
        // case (1):
        //   fprintf(fpout, "%s is a proposition. \n ", str);
        //   break;
        // case (2):
        //   fprintf(fpout, "%s is a negation.  \n", str);
        //   break;
        // case (3):
        //   fprintf(fpout, "%s is a binary. The first part is %s and the second part is %s  \n", str, partone(str), parttwo(str));
        //   break;
        // default:
        //   fprintf(fpout, "What the f***!  ");
      }
    }
    else
    {
      printf("input.txt: Empty input on line %i.", j + 1);
    }
  }

  fclose(fp);
  fclose(fpout);
  free(str);

  return (0);
}
