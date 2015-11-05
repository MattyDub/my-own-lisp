#include <stdio.h>
#include <stdlib.h>

/* I am intentionally not doing the Windows portability stuff here
 since I am doing this for the language-implementation aspect,
 rather than the portability aspect. If it turns out the portability
 stuff is a big deal, I will come back and add in that code. -MAW */

/* Fun SO link on target platform detection:
   http://stackoverflow.com/a/5920028/3435397 */

#include <editline/readline.h>

int main(int argc, char **argv) {

    /* Print Version and Exit information */
    puts("Lispy Version 0.0.0.0.1");
    puts("Press Ctrl-C to exit\n");

    while(1) {
        /* prompt: */
        char *input = readline("lispy> ");

        /* add input to command history */
        add_history(input);

        printf("No, you're a %s\n", input);

        /* input was dynamically allocated */
        free(input);
    }
    return 0;
}
