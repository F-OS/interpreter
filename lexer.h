#ifndef LEXER_H
#define LEXER_H
#include <stddef.h>
#include <stdio.h>
#include <ctype.h>
typedef enum TokenType {
    TOK_EOF,
    TOK_OPAREN,
    TOK_CPAREN,
    TOK_VEC,
    TOK_TICK,
    TOK_BACKTICK,
    TOK_OBRACKET,
    TOK_CBRACKET,
    TOK_OBRACE,
    TOK_CBRACE,
    TOK_AT,
    TOK_COMMA,
    TOK_DOT,
    TOK_STRING,
    TOK_NUMBER_I,
    TOK_NUMBER_F,
    TOK_TRUE,
    TOK_FALSE,
    TOK_IDENTIFIER,
    TOK_CHARACTER
} TokenType;
typedef struct Token {
	TokenType type;
	union as {
		long long number;
		short boolean;
		double floating;
		char* ident;
		char character;
	} as;
} Token;

typedef struct LexerState {
	char *str;
	char *cur;
	size_t token_len;
	size_t curidx;
	size_t readidx;
	Token* tokens;
} LexerState;

void tokenize(char *instr);
Token peek();
Token pop();
Token lookback(int idx);
Token lookforward(int idx);
#ifdef STATIC
static void push(Token tok);
static int match_and_advance(const char* str2);
static char *string_literal_end(char *p);
static int read_escaped_char(char **new_pos, char *p);
static int from_hex(char c);
#define separator(c) \
	((c) == ' '  || (c) == '\t' || (c) == '\n' || \
	 (c) == '\r' || (c) == '('  || (c) == ')'  || \
	 (c) == ';'  || (c) == '\'' || (c) == '`'  || \
	 (c) == ','  || (c) == '\"'  || (c) == '['  || \
	 (c) == ']'  || (c) == '\0')
#define get() tolower(state.cur[0])
#endif
#endif