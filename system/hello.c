/* hello.c - hello */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  hello  -  always return 1
 *------------------------------------------------------------------------
 */
syscall	hello(void)
{
        intmask		mask;		/* Saved interrupt mask		*/
        int32		retval;		/* Value to return to caller	*/

        mask = disable();
        retval = 1;
	kprintf("Hello system call invoked\n");
        restore(mask);
	return retval;
}
