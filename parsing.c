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

/* lval == "Lispy value" struct */
typedef struct {
    int type;
    long num;
    int err;
} lval;

/* possible lval type values */
enum { LVAL_NUM, LVAL_ERR };
/* possible error types */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

lval lval_num(long x) {
    lval v;
    v.type = LVAL_NUM;
    v.num = x;
    return v;
}

lval lval_err(int x) {
    lval v;
    v.type = LVAL_ERR;
    v.err = x;
    return v;
}

/* Print an lval in a manner depending on its type */
void lval_print(lval v) {
    switch (v.type) {
    case LVAL_NUM:
        printf("%li", v.num); break;
    case LVAL_ERR:
        if (v.err == LERR_DIV_ZERO) {
            printf("Error: Division by zero");
        }
        if (v.err == LERR_BAD_OP) {
            printf("Error: Invalid operator");
        }
        if (v.err == LERR_BAD_NUM) {
            printf("Error: Invalid number");
        }
        break;
    }
}

void lval_println(lval v) { lval_print(v); putchar('\n'); }

lval eval_op(lval x, char* op, lval y) {
    /* if either value is an error, return it: */
    if (x.type == LVAL_ERR) { return x; }
    if (y.type == LVAL_ERR) { return y; }

    if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
    if (strcmp(op, "-") == 0) { return lval_num(x.num - y.num); }
    if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
    if (strcmp(op, "/") == 0) {
        return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);
    }
    return lval_err(LERR_BAD_OP);
}


lval eval(mpc_ast_t* t) {
    /* if tagged as number return it directly */
    if (strstr(t->tag, "number")) {
        errno = 0;
        long x = strtol(t->contents, NULL, 10);
        return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
    }

    /* The operator is always the second child */
    char *op = t->children[1]->contents;
    lval x = eval(t->children[2]);

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
            lval result = eval(r.output);
            lval_println(result);
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
