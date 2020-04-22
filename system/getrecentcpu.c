/* getrecentcpu.c - getrecentcpu */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getrecentcpu  -  Get the recent CPU usage of processes
 *------------------------------------------------------------------------
 */
fix16_t	getrecentcpu(
        pid32     pid      /* Process id */
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	//struct	procent *prptr;		/* Ptr to process's table entry	*/
	fix16_t	rcpu;		/* Priority to return	*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}

	rcpu = cpuusage[pid];
	restore(mask);
	return rcpu;
}
