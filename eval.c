#include "eval.h"

#include "lexer.h"
char* errmsg = "PLACEHOLDER";
int eval(const char* in)
{
	tokenize(in);
	Node* head = parse();
}