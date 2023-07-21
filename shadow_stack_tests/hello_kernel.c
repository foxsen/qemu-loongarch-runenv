/* minimal kernel for loongarch64
 * it is used to test shadow stack functionalities.
 */
#include "serial.h"
#include "lib.h"
#include "shadow_stack.h"

extern void trap_init(void);

void start_kernel(void *a0, void *a1, void *a2)
{
    trap_init();

    printf("en %d, size %d, base %x, top %x\n",
            r_scfg_w(0), r_scfg_w(8),
            r_scfg_d(0x20), r_scfg_d(0x28));

    w_scfg_w(1, 0);

    printf("en %d, size %d, base %x, top %x\n",
            r_scfg_w(0), r_scfg_w(8),
            r_scfg_d(0x20), r_scfg_d(0x28));

    puts("hello kernel!\n");

    while(1);
}
