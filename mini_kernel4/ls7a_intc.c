#include "loongarch.h"
#include "ls7a.h"

//
// the loongarch 7A1000 I/O Interrupt Controller Registers.
//

#pragma GCC diagnostic ignored "-Warray-bounds"

void
ls7a_intc_init(void)
{
  /* enable uart0/keyboard/mouse */
  *(volatile unsigned long*)(LS7A_INT_MASK_REG) = ~((0x1UL << UART0_IRQ) | (0x1UL << KEYBOARD_IRQ) | (0x1UL << MOUSE_IRQ));

  *(volatile unsigned long*)(LS7A_INT_EDGE_REG) = (0x1UL << (UART0_IRQ | KEYBOARD_IRQ | MOUSE_IRQ));

  /* route to the same irq in extioi */
  *(volatile unsigned char*)(LS7A_INT_HTMSI_VEC_REG + UART0_IRQ) = UART0_IRQ;
  *(volatile unsigned char*)(LS7A_INT_HTMSI_VEC_REG + KEYBOARD_IRQ) = KEYBOARD_IRQ;
  *(volatile unsigned char*)(LS7A_INT_HTMSI_VEC_REG + MOUSE_IRQ) = MOUSE_IRQ;

  *(volatile unsigned long*)(LS7A_INT_POL_REG) = 0x0UL;

}

// tell the apic we've served this IRQ.
void
ls7a_intc_complete(unsigned long irq)
{
  *(volatile unsigned long*)(LS7A_INT_CLEAR_REG) = (irq);
}
