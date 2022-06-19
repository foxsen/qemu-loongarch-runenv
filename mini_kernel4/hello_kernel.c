/* minimal kernel for loongarch64
 * it print out information passed by BIOS
 */
#include "lib.h"
#include "boot_param.h"

extern void trap_init(void);

void kernel_entry(int a0, char **args, struct bootparamsinterface *a2)
{
    int i;

    printf("There is %d args for kernel:\n", a0);
    for (i=0; i < a0; i++) {
        printf("cmd arg %d: %s\n", i, args[i]);
    }

    printf("efi system table at %p\n", a2->systemtable);
    printf("efi extend list at %p\n", a2->extlist);

    /* ... read the linux kernel source for how to decode these data */

    trap_init();

    while(1);
}


