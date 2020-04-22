/* chnice.c - chnice */

#include <xinu.h>
extern int nice[NPROC];
/*------------------------------------------------------------------------
 *  chnice  -  Change the niceness of processes 
 *------------------------------------------------------------------------
 */
int	chnice(
      pid32     pid,      /* Process id 	*/
	  int		newnice		/* New priority			*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	pri16	oldnice;		/* Niceness to return	*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}
	prptr = &proctab[pid];
	oldnice = nice[pid];
	nice[pid] = newnice;
	if (prptr->prgroup == MFQSCHED) {//100-prptr->prcpu/2-newnice*2;//
		mfq[pid] = fix16_to_float(fix16_min(fix16_max(fix16_sub(fix16_sub(fix16_from_int(100), fix16_div(cpuusage[pid], fix16_from_int(2))), fix16_from_int(newnice*2)), fix16_from_int(0)), fix16_from_int(100)));
	}
	restore(mask);
	return oldnice;
}
