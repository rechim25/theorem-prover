#include <stdio.h>
#include <string.h> /* for all the new-fangled string functions */
#include <stdlib.h> /* malloc, free, rand */

#define FORMULA_SIZE 50  /* max string length of formulas*/
#define INPUT_SIZE 10    /* number of formulas expected in input.txt*/
#define THEOREM_SIZE 100 /* maximum size of set of formulas, if needed*/
#define TABLEAU_SIZE 500 /*maximum length of tableau queue, if needed*/

/* Define propositional symbols*/
#define P 'p'
#define Q 'q'
#define R 'r'
#define NOT_P "-p"
#define NOT_Q "-q"
#define NOT_R "-r"
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

/*
 * Parser Rules
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

int proposition(char *str)
{
  char c = *str;
  return c == P || c == Q || c == R;
}

int negation(char *str)
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
  char *leftPart = calloc(FORMULA_SIZE, sizeof(char));
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
  char *rightPart = calloc(FORMULA_SIZE, sizeof(char));
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
  if (strlen(str) != g_index)
    return incorrectSyntax;
  if (proposition(str))
    return isProposition;
  if (negation(str))
    return isNegation;
  g_main_connective_index = getMainConnectiveIndex(str);
  if (g_main_connective_index > 0)
    return isBinaryFormula;
  return unexpectedInput;
}

/*
 * Tableau
*/

/* A set will contain a list of words. 
 * 
 * Use NULL for emptyset.   
 * 
 * Has recursive structure, i.e. set(p^q, set(-p, set(r>q))))
 */
struct set
{
  char *item;       /*first word of non-empty set*/
  struct set *tail; /*remaining words in the set*/
};

int containsFormula(char *formula, struct set *set)
{
  if (strcmp(set->item, formula) == 0)
  {
    return 1;
  }

  struct set *current = set;
  while (current->tail != NULL)
  {
    if (strcmp(current->item, formula) == 0)
    {
      return 1;
    }
    current = current->tail;
  }
  return 0;
}

/**
 * Adds formula to set if not present added.
 * 
 * Returns 0 if formula present in set.
 * 
 * Returns 1 if formula has been added succesfully.
 */
int addFormula(char *formula, struct set *set)
{
  if (strcmp(set->item, formula) == 0)
  {
    return 0;
  }

  struct set *last = set;
  while (last->tail != NULL)
  {
    if (strcmp(last->item, formula) == 0)
    {
      return 0;
    }
    last = last->tail;
  }

  struct set *new_set = calloc(1, sizeof(struct set));
  char *s = calloc(FORMULA_SIZE, sizeof(char));
  strcpy(s, formula);
  new_set->item = s;

  last->tail = new_set;
}

int expand(struct set *set)
{
  // Case alpha:

  // Case beta:

  return 0;
}

/* 
 * Checks if set contains only propositions.
 * 
 * Returns 1 if the set is fully expanded, 0 otherwise.
*/
int expanded(struct set *set)
{
  if (set == NULL)
  {
    return 1;
  }

  while (set->tail != NULL)
  {
    // strcmp returns non-zero if the 2 strings differ
    if (set->item == NULL || strcmp(set->item, P) || strcmp(set->item, Q) || strcmp(set->item, R))
    {
      return 0;
    }
    set = set->tail;
  }
  return 1;
}

/* 
 * Checks if set contains a contradiction.
 * 
 * Returns 0 if there exists a contradiction in set, 0 otherwise.
 */
int contradictory(struct set *set)
{
  if (set == NULL)
  {
    return 0;
  }

  int hasP, hasQ, hasR = 0;
  while (set->tail != NULL)
  {
    char *formula = set->item;
    if (strcmp(formula, P) == 0)
    {
      hasP = 1;
    }
    else if (strcmp(formula, Q) == 0)
    {
      hasQ = 1;
    }
    else if (strcmp(formula, R) == 0)
    {
      hasR = 1;
    }
    else if (strcmp(formula, NOT_P) == 0 && hasP)
    {
      return 1;
    }
    else if (strcmp(formula, NOT_Q) == 0 && hasQ)
    {
      return 1;
    }
    else if (strcmp(formula, NOT_R) == 0 && hasR)
    {
      return 1;
    }
  }
  return 0;
}

/* A tableau will contain a list of pointers to sets (of words).  
 * 
 * Use NULL for empty list.
 * 
 * A tableau is comprised of multe
 * 
 * Different set within tableau represent different branches.
 */
struct tableau
{
  struct set *S;        /* pointer to first set in non-empty list */
  struct tableau *rest; /*list of pointers to other sets*/
};

/* A fuction to free all the allocated bits on the tableau */
void deepFree(struct tableau *t)
{
  if (!t)
    return;
  while (t->S)
  {
    free(t->S->item);
    struct set *tempSet = t->S;
    t->S = t->S->tail;
    free(tempSet);
  }
  struct tableau *tempTableau = t;
  t = t->rest;
  free(tempTableau);
  deepFree(t);
}

int emptied(struct tableau *t)
{
  return t == NULL;
}

/* 
 * Dequeues first set in tableau and returns it.
 * 
 * Mantains tableau structure. 
 * 
 * Returns NULL if t is empty.
*/
struct set *dequeue(struct tableau **t)
{
  if (emptied(*t))
  {
    return NULL;
  }

  struct set *first = (*t)->S;

  if (emptied((*t)->rest))
  {
    *t = NULL;
    return first;
  }

  (*t)->S = (*t)->rest->S;
  (*t)->rest = (*t)->rest->rest;
  return first;
}

void enqueue(struct set *set, struct tableau **t)
{
  if (emptied(t))
  {
    *t = calloc(1, sizeof(struct tableau));
    (*t)->S = set;
    return;
  }

  struct tableau *last = *t;
  while (!emptied(last->rest))
  {
    last = last->rest;
  }

  struct tableau *new_tableau = calloc(1, sizeof(struct tableau));
  new_tableau->S = set;
  last->rest = new_tableau;
}

int closed(struct tableau *t)
{
  int hasUnclosedBranch = 0;
  while (!emptied(t))
  {
    if (!contradictory(t->S))
    {
      hasUnclosedBranch = 1;
    }
    t = t->rest;
  }
  return !hasUnclosedBranch;
}

void complete(struct tableau *t)
{
  while (!emptied(t))
  {
    struct set *s = dequeue(&t);
  }
}

int main()
{
  char *str = calloc(FORMULA_SIZE, sizeof(char));
  char *left = calloc(FORMULA_SIZE, sizeof(char));
  char *right = calloc(FORMULA_SIZE, sizeof(char));
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
  for (j = 0; j < INPUT_SIZE; j++)
  {
    if (fscanf(fp, "%s", str))
    {
      int parseResult = parse(str);
      switch (parseResult)
      {
      case (incorrectSyntax):
        fprintf(fpout, "%s is not a formula.  \n", str);
        break;
      case (1):
        fprintf(fpout, "%s is a proposition. \n ", str);
        break;
      case (2):
        fprintf(fpout, "%s is a negation.  \n", str);
        break;
      case (3):
        fprintf(fpout,
                "%s is a binary. The first part is %s and the second part is %s  \n",
                str,
                getLeftPartFormula(str),
                getRightPartFormula(str));
        break;
      default:
        fprintf(fpout, "What the f***!  ");
      }

      if (parseResult != incorrectSyntax)
      {
        /* Initialise the tableau with the formula */
        char *s = calloc(FORMULA_SIZE, sizeof(char));
        strcpy(s, str);
        struct set *S = calloc(1, sizeof(struct set));
        S->item = s;
        struct tableau *t = calloc(1, sizeof(struct tableau));
        t->S = S;

        /* Completes the tableau and checks if it is closed */
        complete(t);
        if (closed(t))
          fprintf(fpout, "%s is not satisfiable.\n", str);
        else
          fprintf(fpout, "%s is satisfiable.\n", str);

        /* Frees all the bits of memory on the tableau*/
        deepFree(t);
      }
      else
      {
        fprintf(fpout, "I told you, %s is not a formula.\n", str);
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
  free(left);
  free(right);

  return (0);
}
