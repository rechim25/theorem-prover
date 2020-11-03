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
#define NULLCHAR '\0'

/* Define globals */

enum formulaType
{
  incorrectSyntax,
  isProposition,
  isNegation,
  isBinaryFormula,
  unexpectedInput
};
static int g_index;
static int g_main_connective_index;

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

int checkIsProposition(char *str)
{
  char c = *str;
  return c == P || c == Q || c == R;
}

int checkIsNegation(char *str)
{
  return *str == NOT;
}

char getMainConnectiveIndex(char *str)
{
  int num_unclosed_lparen = 0;
  int main_connective_index = -1;
  for (int i = 0; i < strlen(str); i++)
  {
    if (str[i] == LPAREN)
    {
      num_unclosed_lparen++;
    }
    else if (str[i] == RPAREN)
    {
      num_unclosed_lparen--;
    }
    else if (num_unclosed_lparen == 1 && (str[i] == OR || str[i] == AND || str[i] == IMPLIES))
    {
      main_connective_index = i;
    }
  }
  return main_connective_index;
}

char *getLeftPartFormula(char *str)
{
  char *leftPart = (char *)malloc(sizeof(char) * F_SIZE);
  if (leftPart == NULL)
  {
    exit(EXIT_FAILURE);
  }
  int j = 0;
  for (int i = 1; i < g_main_connective_index; i++)
  {
    leftPart[j] = str[i];
    j++;
  }
  return leftPart;
}

char *getRightPartFormula(char *str)
{
  char *rightPart = (char *)malloc(sizeof(char) * F_SIZE);
  if (rightPart == NULL)
  {
    exit(EXIT_FAILURE);
  }
  int j = 0;
  for (int i = g_main_connective_index + 1; i < strlen(str) - 1; i++)
  {
    rightPart[j] = str[i];
    j++;
  }
  return rightPart;
}

int parse(char *str)
{
  g_index = 0;
  if (match(str, END) || match(str, NULLCHAR) || strlen(str) <= 0)
    return incorrectSyntax;
  if (!matchFormula(str))
    return incorrectSyntax;
  if (!strlen(str) == g_index)
    return incorrectSyntax;
  if (checkIsProposition(str))
    return isProposition;
  if (checkIsNegation(str))
    return isNegation;
  g_main_connective_index = getMainConnectiveIndex(str);
  if (g_main_connective_index > 0)
    return isBinaryFormula;
  return unexpectedInput;
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
      case (incorrectSyntax):
        fprintf(fpout, "%s is not a formula.  \n", str);
        break;
      case (1):
        fprintf(fpout, "%s is a proposition.  \n", str);
        break;
      case (2):
        fprintf(fpout, "%s is a negation.  \n", str);
        break;
      case (3):
        fprintf(fpout, "%s is a binary formula. The first part is %s and the second part is %s  \n",
                str, getLeftPartFormula(str), getRightPartFormula(str));
        break;
      default:
        fprintf(fpout, "What the f*** is %s !? \n", str);
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
