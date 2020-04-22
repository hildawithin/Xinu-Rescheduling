/* chgprio.c - chgprio */

#include <xinu.h>
pri16 pss_prior;
pri16 mfqs_prior;

/*------------------------------------------------------------------------
 *  chgprio  -  Change the initial priority of groups
 *------------------------------------------------------------------------
 */
pri16	chgprio(
      int       group,      /* Group identifier 	*/
	  pri16		newprio		/* New priority			*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	pri16  oldpri;

	mask = disable();
	/*if (group!=PSSCHED && group!=MFQSCHED) {
		restore(mask);
		return (pri16) SYSERR;
	}*/
	if (group == MFQSCHED) {
		oldpri = mfqs_prior;
		mfqs_prior = newprio;
	} else {
		oldpri = pss_prior;
		pss_prior = newprio;
	}

	restore(mask);
	return oldpri;
}
