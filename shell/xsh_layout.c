/* xsh_layout.c - xsh_layout */

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


/*------------------------------------------------------------------------
 * xsh_layout - print addresses
 *------------------------------------------------------------------------
 */
int layout_var_init = 1;
int layout_var_uninit;

shellcmd xsh_layout(int nargs, char *args[])
{
    int layout_stack;
    /* Ensure no arguments were passed */

    if (nargs > 1) {
	XDEBUG_KPRINTF(stderr,"use is: %s\n", args[0]);
	return 1;
    }

    XDEBUG_KPRINTF("Initialized Global Var Address: %x\n\n", &layout_var_init);
    XDEBUG_KPRINTF("Uninitialized Global Var Address: %x\n\n", &layout_var_uninit);
    XDEBUG_KPRINTF("Uninitialized Local Var Address: %x\n\n", &layout_stack);
    XDEBUG_KPRINTF("Function Address: %x\n\n", xsh_layout);
    XTEST_KPRINTF("Initialized Global Var Address: 0x%x\n\n", &layout_var_init);
    XTEST_KPRINTF("Uninitialized Global Var Address: 0x%x\n\n", &layout_var_uninit);
    XTEST_KPRINTF("Uninitialized Local Var Address: 0x%x\n\n", &layout_stack);
    XTEST_KPRINTF("Function Address: 0x%x\n\n", xsh_layout);
    return 0;
}
