#include "lexer.h"
#include "eval.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *scan(size_t buflen)
{
    char* buffer = malloc(buflen - 1);
    if (fgets(buffer, buflen, stdin) != 0)
    {
        buffer[strcspn(buffer, "\n")] = '\0';
        return buffer;
    }
    return 0;
}

// Function to run the REPL (Read-Eval-Print Loop)
void repl()
{
	char *input;
    int status = 0;

    while (1) {
        // Print prompt
    	printf((status == 0 ? "> " : "$> "));
        input = scan(4096);
        // Evaluate input
        status = eval(input);
    }
}

// Function to execute a file
void execute_file(const char* filename)
{
	FILE *file = fopen(filename, "r");
    fseek(file, 0, SEEK_END);
    size_t len = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buf = malloc(len);
    fread(buf, 1, len, file);
    int status = 0;
    status = eval(buf);
    if(status)
    {
	    printf("Error: %s", errmsg);
    }
}

// Main function
int main(int argc, char* argv[])
{
    if (argc == 2) {
        // Execute file
        execute_file(argv[1]);
    }
    else if (argc == 1) {
        // Run REPL
        repl();
    }
    else {
        // Print usage
        printf("Usage: %s [filename]\n", argv[0]);
        exit(1);
    }

    return 0;
}