#include "shadow_stack.h"
#include "lib.h"

void initialize_shadow_stack(void)
{
    printf("en %d, size %d, base %x, top %x\n",
            r_scfg_w(0), r_scfg_w(8), 
            r_scfg_d(0x20), r_scfg_d(0x28));

    w_scfg_w(1, 0);

    printf("en %d, size %d, base %x, top %x\n",
            r_scfg_w(0), r_scfg_w(8), 
            r_scfg_d(0x20), r_scfg_d(0x28));
}
