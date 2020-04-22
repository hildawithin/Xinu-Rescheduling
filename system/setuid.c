/* setuid.c - setuid */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  setuid  -  Set user id for a process 
 *------------------------------------------------------------------------
 */
syscall	setuid(
      int       newuid      /* New user id  */
	)
{
	intmask 	mask;    	/* Interrupt mask		       */
	struct	procent	*prptr;		/* Pointer to proc. table entry        */

	mask = disable();
	prptr = &proctab[currpid];
    if (prptr->uid != 0) {
        restore(mask);
        return SYSERR;
    }

	/* Change the user id of the current running process */
    prptr->uid = newuid;
	restore(mask);
	return OK;
}
