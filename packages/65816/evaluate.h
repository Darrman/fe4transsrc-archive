#ifndef __evaluate_
#define __evaluate_

#include "machine.h"
 
typedef struct {
	unsigned long result;
	unsigned long flag;
	char special;
	} opstruct;
typedef struct {
	opstruct val;
	void *last;
	} stack;

int evaluateType(char *);
address evaluate(char *,int *,address *);
char *toPostfix(char *);
char *scanForNumber(char *,int *,char *);
char *scanForOperator(char *,int *,char *);
char *append(char *,char *);
char *deleteLast(char *,char *);
int priority(char *);
int scanForCloseParen(char *, int *);
opstruct pop(stack **);
void push(stack **,opstruct);

#endif
