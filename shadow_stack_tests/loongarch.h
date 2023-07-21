#ifndef __mini_kernel_loongarch_h_
#define __mini_kernel_loongarch_h_

#include <larchintrin.h>

/* interrupt enable bit */
#define  CSR_CRMD_IE_SHIFT		    2
#define  CSR_CRMD_IE	            ( 0x1 << CSR_CRMD_IE_SHIFT )

static inline unsigned int r_csr_crmd()
{
  unsigned int x;
  asm volatile("csrrd %0, 0x0" : "=r" (x) );
  return x;
}

static inline void w_csr_crmd(unsigned int x)
{
  asm volatile("csrwr %0, 0x0" : : "r" (x));
}

#define PRMD_PPLV (3U << 0)  // Previous Privilege
#define PRMD_PIE  (1U << 2)  // Previous Int_enable

static inline unsigned int r_csr_prmd()
{
  unsigned int x;
  asm volatile("csrrd %0, 0x1" : "=r" (x) );
  return x;
}

static inline void w_csr_prmd(unsigned int x)
{
  asm volatile("csrwr %0, 0x1" : : "r" (x));
}

static inline unsigned long r_csr_era()
{
  unsigned long x;
  asm volatile("csrrd %0, 0x6" : "=r" (x) );
  return x;
}

static inline void w_csr_era(unsigned long x)
{
  asm volatile("csrwr %0, 0x6" : : "r" (x));
}

/* ESTAT bit 16..21 is ecode field */
#define CSR_ESTAT_ECODE  (0x3fU << 16)

static inline unsigned int r_csr_estat()
{
  unsigned int x;
  asm volatile("csrrd %0, 0x5" : "=r" (x) );
  return x;
}

/* vector interrupt scale, when bit 16..18 == 0
 * all interrupt share the same entry
 */
#define CSR_ECFG_VS_SHIFT  16 
/* 8 external hardware interrupt */
#define HWI_VEC  0x3fcU
/* timer interrupt bit */
#define CSR_ECFG_LIE_TI_SHIFT  11
#define TI_VEC  (0x1 << CSR_ECFG_LIE_TI_SHIFT)

static inline unsigned int r_csr_ecfg()
{
  unsigned int x;
  asm volatile("csrrd %0, 0x4" : "=r" (x) );
  return x;
}

static inline void w_csr_ecfg(unsigned int x)
{
  asm volatile("csrwr %0, 0x4" : : "r" (x) );
}

/* timer interrupt clear */
#define CSR_TICLR_CLR  (0x1 << 0)

static inline unsigned int r_csr_ticlr()
{
  unsigned int x;
  asm volatile("csrrd %0, 0x44" : "=r" (x) );
  return x;
}

static inline void w_csr_ticlr(unsigned int x)
{
  asm volatile("csrwr %0, 0x44" : : "r" (x) );
}

static inline unsigned long r_csr_eentry()
{
  unsigned long x;
  asm volatile("csrrd %0, 0xc" : "=r" (x) );
  return x;
}

static inline unsigned long r_csr_tlbrelo0()
{
  unsigned long x;
  asm volatile("csrrd %0, 0x8c" : "=r" (x) );
  return x;
}

static inline unsigned long r_csr_tlbrelo1()
{
  unsigned long x;
  asm volatile("csrrd %0, 0x8d" : "=r" (x) );
  return x;
}

static inline void w_csr_eentry(unsigned long x)
{
  asm volatile("csrwr %0, 0xc" : : "r" (x) );
}

static inline void w_csr_tlbrentry(unsigned long x)
{
  asm volatile("csrwr %0, 0x88" : : "r" (x) );
}

static inline void w_csr_merrentry(unsigned long x)
{
  asm volatile("csrwr %0, 0x93" : : "r" (x) );
}

static inline void w_csr_stlbps(unsigned int x)
{
  asm volatile("csrwr %0, 0x1e" : : "r" (x) );
}

static inline void w_csr_asid(unsigned int x)
{
  asm volatile("csrwr %0, 0x18" : : "r" (x) );
}

/* timer enable */
#define CSR_TCFG_EN            (1U << 0)
/* timer perioid */
#define CSR_TCFG_PER           (1U << 1)

static inline void w_csr_tcfg(unsigned long x)
{
  asm volatile("csrwr %0, 0x41" : : "r" (x) );
}

/* IOCSR */
static inline unsigned int iocsr_readl(unsigned int reg)
{
	return __iocsrrd_w(reg);
}

static inline unsigned long iocsr_readq(unsigned int reg)
{
	return __iocsrrd_d(reg);
}

static inline void iocsr_writel(unsigned int val, unsigned int reg)
{
	__iocsrwr_w(val, reg);
}

static inline void iocsr_writeq(unsigned long val, unsigned int reg)
{
	__iocsrwr_d(val, reg);
}

static inline int intr_get()
{
  unsigned int x = r_csr_crmd();
  return (x & CSR_CRMD_IE) != 0;
}

// enable device interrupts
static inline void intr_on()
{
  w_csr_crmd(r_csr_crmd() | CSR_CRMD_IE);
}

// disable device interrupts
static inline void intr_off()
{
  w_csr_crmd(r_csr_crmd() & ~CSR_CRMD_IE);
}

#endif
