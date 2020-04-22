/*  main.c  - main */

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

process	main(void)
{
    // pid32 pid;
	/* Run the Xinu shell */

	recvclr();
    resume(create(shell, 8192, INITGROUP, 50, "shell", 1, CONSOLE));

	/* Wait for shell to exit and recreate it */

	while (TRUE) {
		receive();
		sleepms(200);
		kprintf("\n\nMain process recreating shell\n\n");
        resume(create(shell, 4096, INITGROUP, 20, "shell", 1, CONSOLE));
	}
	return OK;
    
}
