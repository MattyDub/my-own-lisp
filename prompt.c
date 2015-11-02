#include <stdio.h>

const static int MAX_INPUT_SIZE = 2048;

/* input buffer */
static char input[MAX_INPUT_SIZE];

int main(int argc, char **argv) {

    /* Print Version and Exit information */
    puts("Lispy Version 0.0.0.0.1");
    puts("Press Ctrl-C to exit\n");

    while(1) {
        /* prompt: */
        fputs("lispy> ", stdout);

        /* read a line of user input; max size == MAX_INPUT_SIZE */
        fgets(input, MAX_INPUT_SIZE, stdin);

        printf("No, you're a %s", input);
    }
    return 0;
}
