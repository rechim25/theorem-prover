#include <stdio.h>
#include <string.h> /* for all the new-fangled string functions */
#include <stdlib.h> /* malloc, free, rand */

#define FORMULA_SIZE 70  /* max string length of formulas*/
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
static char *g_current_formula;

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

/*
 * Returns left part of the formula as string with allocated heap memory.
 */
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

/*
 * Returns right part of the formula as string with allocated heap memory.
 */
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

void resetParsing()
{
  g_index = 0;
  g_main_connective_index = 0;
}

int parse(char *str)
{
  resetParsing();
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

/* A set will contain a list of words, formulas.
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

/*
 * Returns a deep copy of the given set with heap allocated memory.
 */
struct set *deepCopy(struct set *set)
{
  struct set *set_copy = calloc(1, sizeof(struct set));
  if (set == NULL)
  {
    set_copy = NULL;
    return set_copy;
  }

  struct set *set_copy_iterator = set_copy;
  while (set != NULL)
  {
    set_copy_iterator->item = calloc(FORMULA_SIZE, sizeof(char));
    strcpy(set_copy_iterator->item, set->item);
    if (set->tail != NULL)
    {
      set_copy_iterator->tail = calloc(1, sizeof(struct set));
    }
    //set->tail;

    set = set->tail;
    set_copy_iterator = set_copy_iterator->tail;
  }
  return set_copy;
}

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

/*
 * Removes first formula from set and returns it.
 * 
 * Maintains set structure and invariant.
 * 
 * Returns NULL if set is empty.
 */
char *dequeueFormula(struct set **set)
{
  if (*set == NULL)
  {
    return NULL;
  }

  char *first = (*set)->item;

  if ((*set)->tail == NULL)
  {
    *set = NULL;
    return first;
  }

  (*set)->item = (*set)->tail->item;
  (*set)->tail = (*set)->tail->tail;
  return first;
}

/**
 * Adds formula to set if not present added.
 * 
 * Maintains set invariant.
 * 
 * Returns 0 if formula present in set.
 * 
 * Returns 1 if formula has been added succesfully.
 */
int enqueueFormula(char *formula, struct set **set)
{
  if (*set == NULL)
  {
    *set = calloc(1, sizeof(struct set));
    (*set)->item = formula;
    return 1;
  }

  if (strcmp((*set)->item, formula) == 0)
  {
    return 0;
  }

  struct set *last = *set;
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

  while (set != NULL)
  {
    // strcmp returns non-zero if the 2 strings differ
    if (set->item == NULL ||
        (strcmp(set->item, "p") != 0 &&
         strcmp(set->item, "q") != 0 &&
         strcmp(set->item, "r") != 0 &&
         strcmp(set->item, NOT_P) != 0 &&
         strcmp(set->item, NOT_Q) != 0 &&
         strcmp(set->item, NOT_R) != 0))
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

  int hasP = 0, hasQ = 0, hasR = 0;
  while (set != NULL)
  {
    char *formula = set->item;
    if (strcmp(formula, "p") == 0)
    {
      hasP = 1;
    }
    else if (strcmp(formula, "q") == 0)
    {
      hasQ = 1;
    }
    else if (strcmp(formula, "r") == 0)
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
    set = set->tail;
  }
  return 0;
}

/*
 * (A^B) is alphaAnd.
 * 
 * -(AvB) is alphaNegatedOr.
 * 
 * -(A>B) is alphaNegatedImplies.
 * 
 * --A is alphaDoubleNegation.
 * 
 * notAlpha corresponds to beta formulas.
 * 
 */
enum alphaType
{
  notAlpha,
  alphaAnd,
  alphaNegatedOr,
  alphaNegatedImplies,
  alphaDoubleNegation
};

/*
 * Returns the alphaType value resulted from parsing formula.
 * 
 * After its execution, if the formula contained a binary formula, 
 * then g_main_connective_index will be correctly set.
 */
int alpha(char *formula)
{
  int parse_result = parse(formula);
  if (parse_result == isNegation)
  {
    char *negated_part = formula + 1;
    int second_parse_result = parse(negated_part);

    if (second_parse_result == isNegation)
    {
      return alphaDoubleNegation;
    }
    else if (second_parse_result == isBinaryFormula)
    {
      if (negated_part[g_main_connective_index] == OR)
      {
        return alphaNegatedOr;
      }
      if (negated_part[g_main_connective_index] == IMPLIES)
      {
        return alphaNegatedImplies;
      }
    }
  }
  else if (parse_result == isBinaryFormula)
  {
    if (formula[g_main_connective_index] == AND)
    {
      return alphaAnd;
    }
  }
  return notAlpha;
}

/*
 * (AvB) is betaOr.
 * 
 * (A>B) is betaImplies.
 * 
 * -(A^B) is betaNegatedAnd.
 * 
 * notBeta corresponds to alpha formulas.
 */
enum betaType
{
  notBeta,
  betaOr,
  betaImplies,
  betaNegatedAnd
};

/*
 * Returns the betaType value resulted from parsing formula.
 * 
 * After its execution, if the formula contained a binary formula, 
 * then g_main_connective_index will be correctly set.
 */
int beta(char *formula)
{
  int parse_result = parse(formula);
  if (parse_result == isBinaryFormula)
  {
    if (formula[g_main_connective_index] == OR)
    {
      return betaOr;
    }

    if (formula[g_main_connective_index] == IMPLIES)
    {
      return betaImplies;
    }
  }
  else if (parse_result == isNegation)
  {
    char *negated_part = formula + 1;
    if (parse(negated_part) == isBinaryFormula)
    {
      if (negated_part[g_main_connective_index] == AND)
      {
        return betaNegatedAnd;
      }
    }
  }
  return notBeta;
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
 * Removes first set from tableau and returns it.
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

/*
 * Adds set to the end of the tableau queue if set is not contradictory.
 * 
 * Maintains tableau structure.
 * 
 * If tableau is empty, allocates heap memory.
 */
void enqueue(struct set *set, struct tableau **t)
{
  if (contradictory(set))
  {
    return;
  }

  if (emptied(*t))
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

/*
 * Returns 1 if tableau is closed (all branches closed), 0 otherwise.
 */
int closed(struct tableau *t)
{
  int has_unclosed_branch = 0;
  while (!emptied(t))
  {
    if (!contradictory(t->S))
    {
      has_unclosed_branch = 1;
    }
    t = t->rest;
  }
  return !has_unclosed_branch;
}

/*
 * Negates the given formula by adding '-' at the beggining of formula.
 * 
 * Returns negated version of formula.
 */
char *negate(char *formula)
{
  char *negated = calloc(FORMULA_SIZE, sizeof(char));
  strcpy(negated, "-");
  strcat(negated, formula);
  //free(formula);
  return negated;
}

enum tableauType
{
  unsatisfiable,
  satisfiable
};

int singleTermFormula(char *formula)
{
  if (strcmp(formula, "p") == 0 ||
      strcmp(formula, "q") == 0 ||
      strcmp(formula, "r") == 0 ||
      strcmp(formula, "-p") == 0 ||
      strcmp(formula, "-q") == 0 ||
      strcmp(formula, "-r") == 0)
  {
    return 1;
  }
  return 0;
}

int complete(struct tableau *t)
{
  while (!emptied(t))
  {
    struct set *s = dequeue(&t);
    if (s == NULL)
    {
      return 0;
    }
    if (expanded(s) && !contradictory(s))
    {
      return satisfiable;
    }

    char *formula = dequeueFormula(&s);
    if (formula == NULL)
    {
      continue;
    }

    if (singleTermFormula(formula))
    {
      // Enqueue set back in tableau
      enqueueFormula(formula, &s);
      enqueue(s, &t);
      continue;
    }

    // TODO: ADD CASE for -(p^q)

    int alpha_result = alpha(formula);
    if (alpha_result != notAlpha) // Alpha Expansion Case
    {
      if (alpha_result == alphaNegatedOr)
      {
        char *left_formula = getLeftPartFormula(formula + 1);
        char *right_formula = getRightPartFormula(formula + 1);
        left_formula = negate(left_formula);
        right_formula = negate(right_formula);

        enqueueFormula(left_formula, &s);
        enqueueFormula(right_formula, &s);

        enqueue(s, &t);
      }
      else if (alpha_result == alphaNegatedImplies)
      {
        char *left_formula = getLeftPartFormula(formula + 1);
        char *right_formula = getRightPartFormula(formula + 1);
        right_formula = negate(right_formula);

        enqueueFormula(left_formula, &s);
        enqueueFormula(right_formula, &s);

        enqueue(s, &t);
      }
      else if (alpha_result == alphaAnd)
      {
        char *left_formula = getLeftPartFormula(formula);
        char *right_formula = getRightPartFormula(formula);

        enqueueFormula(left_formula, &s);
        enqueueFormula(right_formula, &s);

        enqueue(s, &t);
      }
      else if (alpha_result == alphaDoubleNegation)
      {
        enqueueFormula(formula + 2, &s);
        enqueue(s, &t);
      }
    }
    else // Beta Expansion Case
    {
      int beta_result = beta(formula);
      if (beta_result == betaOr)
      {
        char *left_formula = getLeftPartFormula(formula);
        char *right_formula = getRightPartFormula(formula);

        struct set *s_copy = deepCopy(s);
        enqueueFormula(left_formula, &s);
        enqueueFormula(right_formula, &s_copy);

        enqueue(s, &t);
        enqueue(s_copy, &t);
      }
      else if (beta_result == betaImplies)
      {
        char *left_formula = getLeftPartFormula(formula);
        char *right_formula = getRightPartFormula(formula);
        left_formula = negate(left_formula);

        struct set *s_copy = deepCopy(s);
        enqueueFormula(left_formula, &s);
        enqueueFormula(right_formula, &s_copy);

        enqueue(s, &t);
        enqueue(s_copy, &t);
      }
      else if (beta_result == betaNegatedAnd)
      {
        char *left_formula = getLeftPartFormula(formula + 1);
        char *right_formula = getRightPartFormula(formula + 1);
        left_formula = negate(left_formula);
        right_formula = negate(right_formula);

        struct set *s_copy = deepCopy(s);
        enqueueFormula(left_formula, &s);
        enqueueFormula(right_formula, &s_copy);

        enqueue(s, &t);
        enqueue(s_copy, &t);
      }
      else
      {
        printf("\nUnable to expand %s in %s, returning...", formula, g_current_formula);
        return 2;
      }
    }
  }
  return unsatisfiable;
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
      g_current_formula = str;
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
        int tableau_result = complete(t);
        if (tableau_result != satisfiable)
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
