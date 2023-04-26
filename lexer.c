#define STATIC
#include "lexer.h"
#undef STATIC
#include <math.h>
#include <stdlib.h>
#include <string.h>

LexerState state;

void tokenize(const char *instr)
{
	if(state.str != instr)
	{
		memset(&state, 0, sizeof(state));
		size_t strl = strlen(instr);
		state.str = malloc(strl + 1);
		strcpy(state.str, instr);
		state.cur = state.str;
		state.tokens = calloc(1, sizeof(Token));
		state.token_len++;
	}
	while(*state.cur != '\0')
	{
		if(match_and_advance("("))
		{
			Token tok = {TOK_OPAREN, {0}};
			push(tok);
		}
		else if(match_and_advance(")"))
		{
			Token tok = {TOK_CPAREN, {0}};
			push(tok);
		}
		else if(match_and_advance("#("))
		{
			Token tok = {TOK_VEC, {0}};
			push(tok);
		}
		else if(match_and_advance("'"))
		{
			Token tok = {TOK_TICK, {0}};
			push(tok);
		}
		else if(match_and_advance("`"))
		{
			Token tok = {TOK_BACKTICK, {0}};
			push(tok);
		}
		else if(match_and_advance("{"))
		{
			Token tok = {TOK_OBRACKET, {0}};
			push(tok);
		}
		else if(match_and_advance("}"))
		{
			Token tok = {TOK_CBRACKET, {0}};
			push(tok);
		}
		else if(match_and_advance("["))
		{
			Token tok = {TOK_OBRACE, {0}};
			push(tok);
		}
		else if(match_and_advance("]"))
		{
			Token tok = {TOK_CBRACE, {0}};
			push(tok);
		}
		else if(match_and_advance(","))
		{
			Token tok = {TOK_COMMA, {0}};
			push(tok);
		}
		else if(match_and_advance("."))
		{
			Token tok = {TOK_DOT, {0}};
			push(tok);
		}
		else if(match_and_advance("#t"))
		{
			Token tok = {TOK_TRUE, {0}};
			push(tok);
		}
		else if(match_and_advance("#f"))
		{
			Token tok = {TOK_FALSE, {0}};
			push(tok);
		}
		else if(match_and_advance("@"))
		{
			Token tok = {TOK_AT, {0}};
			push(tok);
		}
		else if(match_and_advance(";"))
		{
			while(*state.cur != '\0' && *state.cur != '\n') { state.cur++; }
		}

		else if(match_and_advance("#b"))
		{
			Token tok = {TOK_FALSE, {strtoll(state.cur, &(state.cur), 2)}};
			push(tok);
		}
		else if(match_and_advance("#d"))
		{
			Token tok = {TOK_FALSE, {strtoll(state.cur, &(state.cur), 10)}};
			push(tok);
		}
		else if(match_and_advance("#o"))
		{
			Token tok = {TOK_FALSE, {strtoll(state.cur, &(state.cur), 8)}};
			push(tok);
		}
		else if(match_and_advance("#x"))
		{
			Token tok = {TOK_FALSE, {strtoll(state.cur, &(state.cur), 16)}};
			push(tok);
		}
		else if(match_and_advance("+"))
		{
			if(separator(get()))
			{
				Token tok = {TOK_IDENTIFIER, {.ident = "+"}};
				push(tok);
			}
			else
			{
				int isdecimal = 0;
				char *chk = state.cur;
				while(isdigit(*chk) || *chk == '.') { isdecimal = *chk == '.'; chk++;} 
				if(isdecimal)
				{
					Token tok = {TOK_NUMBER_F, {.floating = strtod(state.cur, &(state.cur))}};
					push(tok);
				}
				else
				{
					Token tok = {TOK_NUMBER_I, {strtoll(state.cur, &(state.cur), 10)}};
					push(tok);
				}
			}
		}
		else if(match_and_advance("-"))
		{
			if(separator(get()))
			{
				Token tok = {TOK_IDENTIFIER, {.ident = "-"}};
				push(tok);
			}
			else
			{
				int isdecimal = 0;
				char *chk = state.cur;
				while(isdigit(*chk) || *chk == '.') { isdecimal = *chk == '.'; chk++;} 
				if(isdecimal)
				{
					Token tok = {TOK_NUMBER_F, {.floating = -1.0 * strtod(state.cur, &(state.cur))}};
					push(tok);
				}
				else
				{
					Token tok = {TOK_NUMBER_I, {-1 * strtoll(state.cur, &(state.cur), 10)}};
					push(tok);
				}
			}
		}
		else if(match_and_advance("..."))
		{
			Token tok = {TOK_IDENTIFIER, {.ident = "..."}};
			push(tok);
		}
		else if(isdigit(state.cur[0]) || state.cur[0] == '.')
		{
			int isdecimal = 0;
			char *chk = state.cur;
			while(isdigit(*chk) || *chk == '.')
			{
				isdecimal = *chk == '.';
				if(isdecimal)
				{
					break;
				}
				chk++;
			} 
			if(isdecimal)
				{
					Token tok = {TOK_NUMBER_F, {strtod(state.cur, &(state.cur))}};
					push(tok);
				}
				else
				{
					Token tok = {TOK_NUMBER_I, strtoll(state.cur, &(state.cur), 10)};
					push(tok);
				}
		}
		else if(state.cur[0] == ' ' || state.cur[0] == '\n' || state.cur[0] == '\t')
		{
			state.cur++;
		}
		else if(state.cur[0] == '\"')
		{
			char *end = string_literal_end(&(state.cur[0]) + 1);
			if(end[0] == '\0')
			{
				state.cur++;
				break;
			}
			char *buf = calloc(1, end - &(state.cur[0]));
			int len = 0;
			for (char *p = &(state.cur[0]) + 1; p < end;) {
				if (*p == '\\')
				{
					buf[len++] = read_escaped_char(&p, p + 1);
				}
				else
				{
					buf[len++] = *p++;
				}
			}
			state.cur += len + 2;
			Token tok = {TOK_STRING, {.ident = buf}};
			push(tok);
		}
		else if(match_and_advance("#\\"))
		{
			char chr = 0;
			if(strncmp(state.cur, "space", 5) == 0)
			{
				state.cur += 5;
				chr = ' ';
			}
			else if(strncmp(state.cur, "newline", 7) == 0)
			{
				state.cur += 7;
				chr = '\n';
			}
			else
			{
				chr = *(state.cur++);
			}
			Token tok = {TOK_CHARACTER, {.character = chr}};
			push(tok);
		}
		else
		{
			char ident[512] = {0};
			size_t idx = 0;
			while(!separator(state.cur[0]))
			{
				ident[idx++] = *(state.cur++);
			}
			char *id = malloc(idx + 1);
			strcpy(id, ident);
			Token tok = {TOK_IDENTIFIER, {.ident = id}};
			push(tok);
		}
	}
	Token tok = {TOK_EOF, {0}};
	push(tok);
	free(state.str);
	return;
}

static int read_escaped_char(char **new_pos, char *p) {
  if ('0' <= *p && *p <= '7') {
    // Read an octal number.
    int c = *p++ - '0';
    if ('0' <= *p && *p <= '7') {
      c = (c << 3) + (*p++ - '0');
      if ('0' <= *p && *p <= '7')
        c = (c << 3) + (*p++ - '0');
    }
    *new_pos = p;
    return c;
  }

  if (*p == 'x') {
    // Read a hexadecimal number.
    p++;
    if (!isxdigit(*p))
      printf("invalid hex escape sequence\n");

    int c = 0;
    for (; isxdigit(*p); p++)
      c = (c << 4) + from_hex(*p);
    *new_pos = p;
    return c;
  }

  *new_pos = p + 1;

  switch (*p) {
  case 'a': return '\a';
  case 'b': return '\b';
  case 't': return '\t';
  case 'n': return '\n';
  case 'v': return '\v';
  case 'f': return '\f';
  case 'r': return '\r';
  case 'e': return 27;
  default: return *p;
  }
}

static int from_hex(char c) {
  if ('0' <= c && c <= '9')
    return c - '0';
  if ('a' <= c && c <= 'f')
    return c - 'a' + 10;
  return c - 'A' + 10;
}

Token peek()
{
	return state.tokens[state.readidx];
}

Token pop()
{
	if(state.readidx >= state.curidx)
	{
		Token out = {TOK_EOF, {0}};
		return out;
	}
	return state.tokens[state.readidx++];
}

static void push(Token tok)
{
	if(state.token_len <= state.curidx)
	{
		state.tokens = realloc(state.tokens, sizeof(Token) * (state.curidx + 1) * 2);
		state.token_len = (state.curidx + 1) * 2;
	}
	state.tokens[state.curidx++] = tok;
}

static char *string_literal_end(char *p) {
  char *start = p;
  for (; *p != '"'; p++) {
    if (*p == '\n' || *p == '\0')
    {
	    printf("unclosed string literal\n");
		return "\0";
    }if (*p == '\\')
      p++;
  }
  return p;
}


static int match_and_advance(const char* str2)
{
	int match = 1;
	int len = 0;
	for(const char *itr = str2; *itr != '\0'; itr++, len++)
	{
		if(*itr != state.cur[len])
		{
			return 0;
		}
	}
	state.cur += len;
	return 1;
}

Token lookback(int idx)
{
	return state.tokens[state.readidx - idx];
}

Token lookforward(int idx)
{
	return state.tokens[state.readidx + idx];
}