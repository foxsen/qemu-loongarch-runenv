#include "lib.h"
#include "loongarch.h"
#include "ls7a.h"

extern void trap_entry(void);

extern void ls7a_intc_init(void);
extern void ls7a_intc_complete(unsigned long irq);

extern void extioi_init(void);
extern void extioi_complete(unsigned long irq);
extern unsigned long extioi_claim(void);
extern void extioi_complete(unsigned long irq);
extern void i8042_init(void);
extern unsigned char kbd_read_byte(void);
extern int kbd_has_data(void);

void timer_interrupt(void)
{
#if 0
    int i = 0;
    unsigned char data;

    do {
        data = kbd_read_byte();
        printf("%x ", data);
    } while (data != 0xff && i++ < 32);

    printf("\n");
#else
    printf("timer interrupt\n");
#endif
    /* ack */
    w_csr_ticlr(r_csr_ticlr() | CSR_TICLR_CLR);
}

void uart0_interrupt(void)
{
}

void keyboard_interrupt(void)
{
    while (kbd_has_data()) {
        printf("%x ", kbd_read_byte());
    } 
    printf("key done\n");
}

void mouse_interrupt(void)
{
    printf("mouse interrupt\n");
}

void trap_handler(void)
{
  unsigned long era = r_csr_era();
  unsigned long prmd = r_csr_prmd();
  unsigned int estat = r_csr_estat();
  unsigned int ecfg = r_csr_ecfg();

  if((prmd & PRMD_PPLV) != 0)
    printf("kerneltrap: not from privilege0");
  if(intr_get() != 0)
    printf("kerneltrap: interrupts enabled");

  printf("\n");
  printf("estat %x, ecfg %x\n", estat, ecfg);
  printf("era=%p eentry=%p\n", r_csr_era(), r_csr_eentry());

  if (estat & ecfg & TI_VEC) {
      timer_interrupt();
  } else if (estat & ecfg & HWI_VEC) {
      // hardware interrupt
      unsigned long irq = extioi_claim();

      printf("irq=%x\n", irq);

      if(irq & (1UL << UART0_IRQ)){
          uart0_interrupt();
      }

      if(irq & (1UL << KEYBOARD_IRQ)){
          keyboard_interrupt();
      }

      if(irq & (1UL << MOUSE_IRQ)){
          mouse_interrupt();
      }

      //ack
      ls7a_intc_complete(irq);
      extioi_complete(irq);

  } else {
      printf("unexpected interrupt\n");
      while (1);
  }

  // restore era
  w_csr_era(era);
  w_csr_prmd(prmd);
}

void trap_init(void)
{
  unsigned int ecfg = ( 0U << CSR_ECFG_VS_SHIFT ) | HWI_VEC | TI_VEC;
  //unsigned long tcfg = 0x10000000UL | CSR_TCFG_EN | CSR_TCFG_PER;
  w_csr_ecfg(ecfg);
  //w_csr_tcfg(tcfg);
  w_csr_eentry((unsigned long)trap_entry);

  extioi_init();
  ls7a_intc_init();

  i8042_init();

  intr_on();
}
