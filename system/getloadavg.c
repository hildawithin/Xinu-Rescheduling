/* getloadavg.c - getloadavg */

#include <xinu.h>
extern fix16_t loadavg;
/*------------------------------------------------------------------------
 *  getloadavg  -  Get the overal system load average
 *------------------------------------------------------------------------
 */
fix16_t	getloadavg()
{
	intmask	mask;			/* Saved interrupt mask		*/

	mask = disable();
	if (isbadpid(currpid)) {
		restore(mask);
		return SYSERR;
	}

	restore(mask);
	return loadavg;
}
