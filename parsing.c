#include <stdio.h>
#include <stdlib.h>

/* I am intentionally not doing the Windows portability stuff here
 since I am doing this for the language-implementation aspect,
 rather than the portability aspect. If it turns out the portability
 stuff is a big deal, I will come back and add in that code. -MAW */

/* Fun SO link on target platform detection:
   http://stackoverflow.com/a/5920028/3435397 */

#include <editline/readline.h>
#include "mpc.h"

long eval_op(long x, char* op, long y) {
    if (strcmp(op, "+") == 0) { return x + y; }
    if (strcmp(op, "-") == 0) { return x - y; }
    if (strcmp(op, "*") == 0) { return x * y; }
    if (strcmp(op, "/") == 0) { return x / y; }
}


long eval(mpc_ast_t* t) {
    /* if tagged as number return it directly */
    if (strstr(t->tag, "number")) {
        return atoi(t->contents);
    }

    /* The operator is always the second child */
    char *op = t->children[1]->contents;

    /* We store the third child in 'x' */
    long x = eval(t->children[2]);

    /* Iterate the remaining children and conbine */
    int i = 3;
    while (strstr(t->children[i]->tag, "expr")) {
        x = eval_op(x, op, eval(t->children[i]));
        i++;
    }
    return x;
}

int main(int argc, char **argv) {

    /* create some mpc parsers */
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Lispy = mpc_new("lispy");

    /* Define parsers with this grammar: */
    mpca_lang(MPCA_LANG_DEFAULT,
              "\
               number : /-?[0-9]+/ ; \
               operator : '+' | '-' | '*' | '/' ; \
               expr : <number> | '(' <operator> <expr>+ ')' ; \
               lispy : /^/ <operator> <expr>+ /$/ ;",
              Number, Operator, Expr, Lispy);

    
    /* Print Version and Exit information */
    puts("Lispy Version 0.0.0.0.2");
    puts("Press Ctrl-C to exit\n");

    while(1) {
        /* prompt: */
        char *input = readline("lispy> ");

        /* add input to command history */
        add_history(input);

        /* attempt to parse the user input */
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            /* On Success, eval the AST */
            long result = eval(r.output);
            printf("%li\n", result);
            mpc_ast_delete(r.output);
        } else {
            /* Otherwise print the error */
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        /* input was dynamically allocated */
        free(input);
    }
    mpc_cleanup(4, Number, Operator, Expr, Lispy);
    return 0;
}
