#include <stdio.h>
#include <string.h>   /* for all the new-fangled string functions */
#include <stdlib.h>     /* malloc, free, rand */



int Fsize=50; /*maximum formula length*/
int inputs =10;/* number of formulas expected in input.txt*/
int ThSize=100;/* maximum size of set of formulas, if needed*/
int TabSize=500; /*maximum length of tableau queue, if needed*/



/* A set will contain a list of words. Use NULL for emptyset.  */
  struct set{
    char *item;/*first word of non-empty set*/
    struct set *tail;/*remaining words in the set*/
  };

/* A tableau will contain a list of pointers to sets (of words).  Use NULL for empty list.*/
struct tableau {
  struct set * S; /* pointer to first set in non-empty list */
  struct tableau *rest; /*list of pointers to other sets*/
};

/* A fuction to free all the allocated bits on the tableau */
void deepFree(struct tableau *t) {
  if(!t) return;
  while(t->S) {
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

/*You need to change this next bit and include functions for parse, closed and complete.*/
int parse(char *g) {
    return 0;
}

void partone(char *fmla, char *dest){
    *dest = '\0';
}

void parttwo(char *fmla, char *dest){
  *dest = '\0';
}

int closed(struct tableau *t) {
  return 0;
}

void complete(struct tableau *t){}

int main()
{/*input 10 strings from "input.txt" */

    /*You should not need to alter the input and output parts of the program below.*/
    char *name = calloc(Fsize, sizeof(char));
    char *left = calloc(Fsize, sizeof(char));
    char *right = calloc(Fsize, sizeof(char));
    FILE *fp, *fpout;

    /* reads from input.txt, writes to output.txt*/
    if ((  fp=fopen("input.txt","r"))==NULL){printf("Error opening file");exit(1);}
    if ((  fpout=fopen("output.txt","w"))==NULL){printf("Error opening file");exit(1);}

    int j;

    for(j=0;j<inputs;j++)
    {
        fscanf(fp, "%s",name);/*read formula*/
        switch (parse(name))
        {
            case(0): fprintf(fpout, "%s is not a formula.  \n", name);break;
            case(1): fprintf(fpout, "%s is a proposition. \n ", name);break;
            case(2): fprintf(fpout, "%s is a negation.  \n", name);break;
            case(3):
              partone(name, left);
              parttwo(name, right);
              fprintf(fpout, "%s is a binary. The first part is %s and the second part is %s  \n", name, left, right);
              break;
            default:fprintf(fpout, "What the f***!  ");
        }



        if (parse(name)!=0)
        {
          /* Iitialise the tableau with the formula */
          char* s = calloc(Fsize, sizeof(char));
          strcpy(s, name);
          struct set* S = calloc(1, sizeof(struct set));
          S->item = s;
          struct tableau* t = calloc(1, sizeof(struct tableau));
          t->S = S;

          /* Completes the tableau and checks if it is closed */
          complete(t);
          if (closed(t))  fprintf(fpout, "%s is not satisfiable.\n", name);
          else fprintf(fpout, "%s is satisfiable.\n", name);

          /* Frees all the bits of memory on the tableau*/
          deepFree(t);
        }
        else  fprintf(fpout, "I told you, %s is not a formula.\n", name);
    }

    fclose(fp);
    fclose(fpout);
    free(left);
    free(right);
    free(name);

  return(0);
}
