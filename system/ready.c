/* ready.c - ready */

#include <xinu.h>
#if XTEST
#define XTEST_KPRINTF(...) kprintf(__VA_ARGS__)
#else
#define XTEST_KPRINTF(...)
#endif

#if XDEBUG
#define XDEBUG_KPRINTF(...) kprintf(__VA_ARGS__)
#else
#define XDEBUG_KPRINTF(...)
#endif
qid16	readylist;			/* Index of ready list		*/
extern uint32  clktime;
extern uint32	count1000;
extern uint32 preempt;

/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
status	ready(
	  pid32		pid		/* ID of process to make ready	*/
	)
{
	register struct procent *prptr;
    float p;
	float T;

	if (isbadpid(pid)) {
		return SYSERR;
	}

	/* Set process state to indicate ready and add to ready list */
	prptr = &proctab[pid];
    if (prptr->prstate!=PR_READY && prptr->prgroup == PSSCHED) {
		T = fix16_to_float(fix16_div(fix16_from_int(clktime*1000+count1000), fix16_from_int(preempt)));
		p = ps[pid];
        if (T>p) {
			p = T;
		}
		ps[pid] = p;
		prptr->prstate = PR_READY;
        insert(pid, readylist, prptr->prprio);
        // XDEBUG_KPRINTF("Process %d %s in PS added to ready queue. %f \n\n", pid, prptr->prname, p);
	} else {
        prptr->prstate = PR_READY;
	    insert(pid, readylist, prptr->prprio);
	    // XDEBUG_KPRINTF("Process %d %s added to ready queue. \n\n", pid, prptr->prname);
	}
	resched();

	return OK;
}
