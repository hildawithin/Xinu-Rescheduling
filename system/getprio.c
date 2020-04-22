/* getprio.c - getprio */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getprio  -  Return the scheduling priority of a process
 *------------------------------------------------------------------------
 */
syscall	getprio(
	  pid32		pid		/* Process ID			*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	uint32	prio;			/* Priority to return		*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}
	if (proctab[pid].prgroup==PSSCHED) {
		prio = (int)ps[pid];
	} else if (proctab[pid].prgroup==MFQSCHED) {
		prio = (int)mfq[pid];
	} else {
		prio = proctab[pid].prprio;
	}
	
	restore(mask);
	return prio;
}
