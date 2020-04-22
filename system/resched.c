/* resched.c - resched, resched_cntl */

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

struct	defer	Defer;
float ps[NPROC];
float mfq[NPROC];
fix16_t cpuusage[NPROC];
int nice[NPROC];
int schednum[NPROC];
pri16 mfqscnt;
extern  pri16 pss_prior;
extern  pri16 mfqs_prior;
extern uint32 preempt;
extern int nullstrt;
/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void	resched(void)		/* Assumes interrupts are disabled	*/
{
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/
	// struct procent *prptr;	/* Ptr to table entry for new process	*/
    uint32 t;

	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}
    // XDEBUG_KPRINTF("Entering Rescheduling...... \n");

    if (!isempty(readylist)) {
		XDEBUG_KPRINTF("Rescheduling...... \n");
        /* Aging Scheduler: choose the group with more processes pending */
        /* If both have the same number of processes in the ready queue, pick PSS */
        /* Count the number of ready processes in the ready list   */
	    /* Traverse the process table to count the ready processes */
        pid32 pid;
	    pri16 psscnt;
	    psscnt = pss_prior;
	    mfqscnt = mfqs_prior;
		
		for (pid = firstid(readylist);pid != lastid(readylist);pid = queuetab[pid].qnext) {
		    if (proctab[pid].prgroup == PSSCHED && pid!=NULLPROC) {
			    psscnt++;
		    } else if(proctab[pid].prgroup == MFQSCHED && pid!=NULLPROC) {
			    mfqscnt++;
		    }
		}
		if (proctab[pid].prgroup == PSSCHED && pid!=NULLPROC) {
			psscnt++;
		} else if (proctab[pid].prgroup == MFQSCHED && pid!=NULLPROC) {
			mfqscnt++;
		}
		/* If no non-null process in the ready queue */
		/* Case 1: current process is null process, no valid process in ready queue */
		/* Case 2: current process: non-null process, then compare its priority with null process */
		
		ptold = &proctab[currpid];
        if (psscnt == pss_prior && mfqscnt == mfqs_prior) {
			/* Case 2: current process: non-null process, then compare its priority with null process */
			// XDEBUG_KPRINTF("No non-null process ready. \n\n");
			if (currpid != NULLPROC) {
				// prptr = &proctab[NULLPROC];
				if (ptold->prstate == PR_CURR) {
					if (currpid>=nullstrt) {
						schednum[currpid]++;
						preempt = QUANTUM;		/* Reset time slice for process	*/
						return;
					}
					if (ptold->prgroup == PSSCHED && ps[currpid]>=ps[NULLPROC]) {
						ptold->prstate = PR_READY;
						if (ptold->prprio < 1) {
					        ptold->prprio = 1;
				        }
			            t = getticks();
			            ps[currpid] += fix16_to_float(fix16_div(fix16_mul(fix16_from_int(t),fix16_from_int(100)),fix16_from_int(ptold->prprio)));
						insert(currpid, readylist, ptold->prprio);
						// switch to null process
					    ptold = &proctab[currpid];
					    currpid = NULLPROC;//dequeue(readylist);
			            ptnew = &proctab[currpid];
			            ptnew->prstate = PR_CURR;
				        XDEBUG_KPRINTF("Swtich to %d \n", currpid);
						schednum[currpid]++;
						preempt = QUANTUM;		/* Reset time slice for process	*/
			            ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
                        return;
					} else if (ptold->prgroup == MFQSCHED) {
						// ptold->prstate = PR_READY; && mfq[currpid]<=mfq[NULLPROC]
						// insert(currpid, readylist, ptold->prprio);
						// // switch to null process
					    // ptold = &proctab[currpid];
					    // currpid = dequeue(readylist);
			            // ptnew = &proctab[currpid];
			            // ptnew->prstate = PR_CURR;
						schednum[currpid]++;
						// XDEBUG_KPRINTF("Swtich to %d \n", currpid);
			            preempt = QUANTUM;		/* Reset time slice for process	*/
			            // ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
                        return;
					} else if (ptold->prgroup == PSSCHED && ps[currpid]<ps[NULLPROC]) {
						// if (ptold->prprio < 1) {
					    //     ptold->prprio = 1;
				        // }
			            // t = getticks();
			            // ps[currpid] += fix16_to_float(fix16_div(fix16_mul(fix16_from_int(t),fix16_from_int(100)),fix16_from_int(ptold->prprio)));
						schednum[currpid]++;
						preempt = QUANTUM;		/* Reset time slice for process	*/
						return;
					// } else if (ptold->prgroup == MFQSCHED && mfq[currpid]>mfq[NULLPROC]) {
					// 	schednum[currpid]++;
					// 	preempt = QUANTUM;		/* Reset time slice for process	*/
					// 	return;
					}
				} else {
					// switch to null process
					ptold = &proctab[currpid];
					currpid = dequeue(readylist);
			        ptnew = &proctab[currpid];
			        ptnew->prstate = PR_CURR;
					schednum[currpid]++;
					XDEBUG_KPRINTF("Swtich to %d \n", currpid);
			        preempt = QUANTUM;		/* Reset time slice for process	*/
			        ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
                    return;
				}
			}
			schednum[currpid]++;
			preempt = QUANTUM;		/* Reset time slice for process	*/
			return;
        }
        /* Aging Scheduler */
        if (psscnt >= mfqscnt) {
            /* Proportional Share Scheduler */
            // XDEBUG_KPRINTF("PSS with %d. \n\n", psscnt);
			/* Find the location where the lowest Pi lies */
			float minv;
			pid32 index;
			int flag = 0;
			for (pid = firstid(readylist);pid != lastid(readylist);pid = queuetab[pid].qnext) {
				if (proctab[pid].prgroup == PSSCHED && pid!=NULLPROC) {
					if (flag == 0) {
						minv = ps[pid];
						index = pid;
						flag = 1;
					} else {
						if (ps[pid]<minv) {
							minv = ps[pid];
							index = pid;
						} else if (ps[pid]==minv && schednum[pid]<schednum[index]) {
							minv = ps[pid];
							index = pid;
						}
					}
				}
			}
			if (proctab[pid].prgroup == PSSCHED && pid!=NULLPROC) {
				if (flag == 0) {
					minv = ps[pid];
					index = pid;
					flag = 1;
				} else {
					if (ps[pid]<minv) {
						minv = ps[pid];
						index = pid;
					} else if (ps[pid]==minv && schednum[pid]<schednum[index]) {
						minv = ps[pid];
						index = pid;
					}
				}
			}
			/* If the current process belongs to another group, is null process, not in current state, 
			priority is higher -> switch to the lowest valid process */
			ptold = &proctab[currpid];
			if (currpid==NULLPROC || ptold->prgroup != PSSCHED || ptold->prstate != PR_CURR || (ptold->prgroup == PSSCHED && ps[currpid] >= ps[index])) {
				if (ptold->prstate == PR_CURR) {
					ptold->prstate = PR_READY;
					/* Update the current running process priority && currpid!=NULLPROC*/
                    if (ptold->prgroup == PSSCHED && ptold->prstate == PR_CURR) {
				        if (ptold->prprio < 1) {
					        ptold->prprio = 1;
				        }
			            t = getticks();
			            ps[currpid] += fix16_to_float(fix16_div(fix16_mul(fix16_from_int(t),fix16_from_int(100)),fix16_from_int(ptold->prprio)));
		                // XDEBUG_KPRINTF("Process %d update priority to %f \n\n", currpid, ps[currpid]);
			        }
					insert(currpid, readylist, ptold->prprio);
				}
                // Remove the new process from readylist
			    currpid = getitem(index);
			    // Make the new process current
			    ptnew = &proctab[currpid];
				ptnew->prstate = PR_CURR;
				schednum[currpid]++;
				// XDEBUG_KPRINTF("Swtich to %d \n", currpid);
				preempt = QUANTUM;		/* Reset time slice for process	*/
				ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
				return;
			} else {
				schednum[currpid]++;
				preempt = QUANTUM;
				return;
			}
        } else {
            /* Multi-level Feedback Queue Scheduler */
            // XDEBUG_KPRINTF("MFQS with %d. \n\n", mfqscnt);
			// XDEBUG_KPRINTF("loadavg %f\n\n", fix16_to_float(getloadavg()));
			// XDEBUG_KPRINTF("recentcpu %f\n\n", fix16_to_float(getrecentcpu(currpid)));
            /* Find the location where the largest priorityi lies */
			float maxv;
			pid32 index;
			int flag = 0;
			for (pid = firstid(readylist);pid != lastid(readylist);pid = queuetab[pid].qnext) {
				if (proctab[pid].prgroup == MFQSCHED && pid!=NULLPROC) {
					if (flag == 0) {
						maxv = mfq[pid];
						index = pid;
						flag = 1;
					} else {
						if (ps[pid]>maxv) {
							maxv = ps[pid];
							index = pid;
						} else if (ps[pid]==maxv && schednum[pid]<schednum[index]) {
							maxv = ps[pid];
							index = pid;
						}
					}
				}
			}
			if (proctab[pid].prgroup == MFQSCHED && pid!=NULLPROC) {
				if (flag == 0) {
					maxv = mfq[pid];
					index = pid;
					flag = 1;
				} else {
					if (ps[pid]>maxv) {
						maxv = ps[pid];
						index = pid;
					} else if (ps[pid]==maxv && schednum[pid]<schednum[index]) {
						maxv = ps[pid];
						index = pid;
					}
				}
			}
			/* If the current process belongs to another group, is null process, not in current state, 
			priority is higher -> switch to the lowest valid process */
			if (currpid==NULLPROC || ptold->prstate != PR_CURR || (ptold->prgroup == MFQSCHED && mfq[currpid] <= mfq[index]) || ptold->prgroup == PSSCHED){
				if (ptold->prstate == PR_CURR) {
					ptold->prstate = PR_READY;
					insert(currpid, readylist, ptold->prprio);
				}
				currpid = getitem(index);
				ptnew = &proctab[currpid];
				ptnew->prstate = PR_CURR;
				schednum[currpid]++;
				XDEBUG_KPRINTF("Swtich to %d \n", currpid);
				preempt = QUANTUM;		/* Reset time slice for process	*/
				ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
                return;
			} else {
				schednum[currpid]++;
				preempt = QUANTUM;
				return;
			}
        }
    }
    // XDEBUG_KPRINTF("Ex\n");
	// schednum[currpid]++;
	preempt = QUANTUM;		/* Reset time slice for process	*/
	return;
}

/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
status	resched_cntl(		/* Assumes interrupts are disabled	*/
	  int32	defer		/* Either DEFER_START or DEFER_STOP	*/
	)
{
	switch (defer) {

	    case DEFER_START:	/* Handle a deferral request */

		if (Defer.ndefers++ == 0) {
			Defer.attempt = FALSE;
		}
		return OK;

	    case DEFER_STOP:	/* Handle end of deferral */
		if (Defer.ndefers <= 0) {
			return SYSERR;
		}
		if ( (--Defer.ndefers == 0) && Defer.attempt ) {
			resched();
		}
		return OK;

	    default:
		return SYSERR;
	}
}
