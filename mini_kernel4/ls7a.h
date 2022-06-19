#ifndef __mini_kernel_ls7a_h_
#define __mini_kernel_ls7a_h_

#define UART0 0x1fe001e0UL
#define UART0_IRQ 2

#define LS7A_I8042_DATA    0x1fe00060UL
#define LS7A_I8042_COMMAND 0x1fe00064UL
#define LS7A_I8042_STATUS  0x1fe00064UL

#define KEYBOARD_IRQ 3
#define MOUSE_IRQ 4
    
/* ============== LS7A registers =============== */
#define LS7A_PCH_REG_BASE       0x10000000UL
  
#define LS7A_INT_MASK_REG       LS7A_PCH_REG_BASE + 0x020
#define LS7A_INT_EDGE_REG       LS7A_PCH_REG_BASE + 0x060
#define LS7A_INT_CLEAR_REG      LS7A_PCH_REG_BASE + 0x080
#define LS7A_INT_HTMSI_VEC_REG  LS7A_PCH_REG_BASE + 0x200
#define LS7A_INT_STATUS_REG     LS7A_PCH_REG_BASE + 0x3a0
#define LS7A_INT_POL_REG        LS7A_PCH_REG_BASE + 0x3e0

#endif

