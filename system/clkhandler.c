/* clkhandler.c - clkhandler */

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
extern fix16_t loadavg;      /* Overall system workload  */
// extern uint32  clktime;
// extern uint32 preempt;
// extern uint32	count1000;		/* Milliseconds since last clock tick   */
extern pri16 mfqs_prior;
extern pri16 mfqscnt;
extern int nice[NPROC];
extern fix16_t cpuusage[NPROC];
/*------------------------------------------------------------------------
 * clkhandler - high level clock interrupt handler
 *------------------------------------------------------------------------
 */
void	clkhandler(
		int32	arg	/* Interrupt handler argument	*/
		)
{
    // struct procent *prptr;  /* Ptr to table entry for temporary process */
	pid32 pid;
	
	if(!(hpet->gis & HPET_GIS_T0)) {
		return;
	}

	hpet->gis |= HPET_GIS_T0;
    
	if((++count1000) >= 1000) {
		clktime++; 
	}
	
	/* Decrement the ms counter, and see if a second has passed */
    if (mfqscnt>mfqs_prior) {
		cpuusage[currpid] += fix16_from_int(1);
	    if(count1000 >= 1000) {
		    /* One second has passed, so increment seconds count */
            loadavg = fix16_add(fix16_mul(fix16_div(fix16_from_int(59), fix16_from_int(60)), loadavg), fix16_div(fix16_from_int(mfqscnt-mfqs_prior),fix16_from_int(60)));
		    // XDEBUG_KPRINTF("loadavg: %f \n\n", fix16_to_float(loadavg));
		    //recentcpui
		    pid = firstid(readylist);
		    while (pid != lastid(readylist)) {
			    if (pid!=NULLPROC && proctab[pid].prgroup == MFQSCHED) {
			        cpuusage[pid] = fix16_add(fix16_mul(fix16_div(fix16_mul(fix16_from_int(2),loadavg),fix16_add(fix16_mul(fix16_from_int(2),loadavg),fix16_from_int(1))), (fix16_t)cpuusage[pid]),fix16_from_int(nice[pid]));
				    // XDEBUG_KPRINTF("cpu: %f \n\n", fix16_to_float(cpuusage[pid]));
			    }
			    pid = queuetab[pid].qnext;
		    }
		    if (pid!=NULLPROC && proctab[pid].prgroup == MFQSCHED) {
			    cpuusage[pid] = fix16_add(fix16_mul(fix16_div(fix16_mul(fix16_from_int(2),loadavg),fix16_add(fix16_mul(fix16_from_int(2),loadavg),fix16_from_int(1))), (fix16_t)cpuusage[pid]),fix16_from_int(nice[pid]));
			    // XDEBUG_KPRINTF("cpu: %f \n\n", fix16_to_float(cpuusage[pid]));
		    }
		}
		if(count1000 % 10 == 0) {
			/* 10ms have passed, so increment seconds count */
            // priorityi
		    pid = firstid(readylist);
		    while ( pid != lastid(readylist)) {
			    if (pid!=NULLPROC && proctab[pid].prgroup == MFQSCHED) {
			        mfq[pid] = fix16_to_float(fix16_min(fix16_max(fix16_sub(fix16_sub(fix16_from_int(100), fix16_div((fix16_t)cpuusage[pid], fix16_from_int(2))), fix16_mul(fix16_from_int(nice[pid]), fix16_from_int(2))), fix16_from_int(0)), fix16_from_int(100)));
			    }
			    pid = queuetab[pid].qnext;
		    }
		    if (pid!=NULLPROC && proctab[pid].prgroup == MFQSCHED) {
		        mfq[pid] = fix16_to_float(fix16_min(fix16_max(fix16_sub(fix16_sub(fix16_from_int(100), fix16_div((fix16_t)cpuusage[pid], fix16_from_int(2))), fix16_mul(fix16_from_int(nice[pid]), fix16_from_int(2))), fix16_from_int(0)), fix16_from_int(100)));
		    }
		}
        /* Reset the local ms counter for the next second */
		count1000 = 0;
	}	

	/* Handle sleeping processes if any exist */

	if(!isempty(sleepq)) {

		/* Decrement the delay for the first process on the	*/
		/*   sleep queue, and awaken if the count reaches zero	*/

		if((--queuetab[firstid(sleepq)].qkey) <= 0) {
			wakeup();
		}
	}

	/* Decrement the preemption counter, and reschedule when the */
	/*   remaining time reaches zero			     */

	if((--preempt) <= 0) {
		preempt = QUANTUM;
		resched();
	}
}
