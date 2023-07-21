/* minimal kernel for loongarch64
 * it print out information passed by BIOS
 */
#include "lib.h"
#include "boot_param.h"

extern void trap_init(void);

void start_kernel(int a0, char **args, struct bootparamsinterface *a2)
{
    printf("hello\n");

    trap_init();

    while(1);
}


