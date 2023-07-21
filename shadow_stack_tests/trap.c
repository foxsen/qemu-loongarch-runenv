#include "lib.h"
#include "loongarch.h"

extern void trap_entry(void);

void timer_interrupt(void)
{
    printf("timer interrupt\n");
    /* ack */
    w_csr_ticlr(r_csr_ticlr() | CSR_TICLR_CLR);
}

void trap_handler(void)
{
  unsigned long era = r_csr_era();
  unsigned long prmd = r_csr_prmd();
  unsigned int estat = r_csr_estat();
  unsigned int ecfg = r_csr_ecfg();

  printf("trap at %x estat %d ecfg %d\n", era, estat, ecfg);

  if((prmd & PRMD_PPLV) != 0)
    printf("kerneltrap: not from privilege0");
  if(intr_get() != 0)
    printf("kerneltrap: interrupts enabled");

  if (estat & ecfg & TI_VEC) {
      timer_interrupt();
  } else if (estat & ecfg) {
      //while(1);
  }

  w_csr_era(era);
  w_csr_prmd(prmd);
}

void trap_init(void)
{
  unsigned int ecfg = ( 0U << CSR_ECFG_VS_SHIFT ) | HWI_VEC | TI_VEC;
  unsigned long tcfg = 0x10000000UL | CSR_TCFG_EN | CSR_TCFG_PER;
  w_csr_ecfg(ecfg);
  w_csr_tcfg(tcfg);
  w_csr_eentry((unsigned long)trap_entry);
  intr_on();
}
