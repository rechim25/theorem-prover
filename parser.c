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
#define OPEN_PAREN '('
#define CLOSED_PAREN ')'
#define END '\n'

int i;
int j;

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

struct Node
{
};

int isProp(char c)
{
  return c == P || c == Q || c == R;
}

int isBC(char c)
{
  return c == OR || c == AND || c == IMPLIES;
}

int isNegation(char c)
{
  return c == NOT;
}

int isFormula(char c)
{
  return c == P || c == Q || c == R || c == '-' || c == '(';
}

char *formula(char *str)
{
  char c = *str;
  if (c == P || c == Q || c == R)
  {
  }
  else if (c == NOT)
  {
  }
  else if (c == OPEN_PAREN)
  {
    char *next_str = formula(str++);
    isBC(next_str);
  }
}

int parse(char *str)
{
  return formula(str);
}

int main()

{ /*input 10 strings from "input.txt" */

  char *str = malloc(F_SIZE);
  FILE *fp, *fpout;

  /* reads from input.txt, writes to output.txt*/
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
    fscanf(fp, "%s", str); /*read formula*/
    switch (parse(str))
    {
    case (0):
      fprintf(fpout, "%s is not a formula.  \n", str);
      break;
    case (1):
      fprintf(fpout, "%s is a proposition. \n ", str);
      break;
    case (2):
      fprintf(fpout, "%s is a negation.  \n", str);
      break;
    case (3):
      fprintf(fpout, "%s is a binary. The first part is %s and the second part is %s  \n", str, partone(str), parttwo(str));
      break;
    default:
      fprintf(fpout, "What the f***!  ");
    }
  }

  fclose(fp);
  fclose(fpout);
  free(name);

  return (0);
}
