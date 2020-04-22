/* xsh_hello.c - xsh_hello */

#include <xinu.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_hello - greetings
 *------------------------------------------------------------------------
 */
shellcmd xsh_hello(int nargs, char *args[])
{

    /* Ensure no arguments were passed */

    if (nargs > 1) {
	fprintf(stderr,"use is: %s\n", args[0]);
	return 1;
    }

    hello();
    return 0;
}
