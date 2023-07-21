#ifndef SHADOW_STACK_H_
#define SHADOW_STACK_H_

__asm__(".macro parse_v var val\n\t"
        "\\var = \\val\n\t"
        ".endm\n\t");

/* Match an individual register number and assign to \var */
#define _IFC_REG(r, n)      \
    ".ifc \\r, $" #r "\n\t" \
    "\\var = " #n "\n\t"    \
    ".endif\n\t"

__asm__(".macro parse_r var r\n\t"
    "\\var = -1\n\t"
    _IFC_REG(r0,0) _IFC_REG(r1,1)  _IFC_REG(r2,2)  _IFC_REG(r3,3)
    _IFC_REG(r4,4)   _IFC_REG(r5,5)  _IFC_REG(r6,6)  _IFC_REG(r7,7)
    _IFC_REG(r8,8)   _IFC_REG(r9,9)  _IFC_REG(r10,10) _IFC_REG(r11,11)
    _IFC_REG(r12,12)  _IFC_REG(r13,13) _IFC_REG(r14,14) _IFC_REG(r15,15)
    _IFC_REG(r16,16)  _IFC_REG(r17,17) _IFC_REG(r18,18) _IFC_REG(r19,19)
    _IFC_REG(r20,20)  _IFC_REG(r21,21)  _IFC_REG(r22,22) _IFC_REG(r23,23)
    _IFC_REG(r24,24)  _IFC_REG(r25,25) _IFC_REG(r26,26) _IFC_REG(r27,27)
    _IFC_REG(r28,28)  _IFC_REG(r29,29) _IFC_REG(r30,30) _IFC_REG(r31,31)
    ".iflt \\var\n\t"
    ".error \"Unable to parse register name \\r\"\n\t"
    ".endif\n\t"
    ".endm");

__asm__(".macro def_insn insn, val, addr\n\t"
        "parse_v __insn, \\insn \n\t"
        "parse_r __val, \\val\n\t"
        "parse_r __addr, \\addr\n\t"
        ".word __insn | (__addr & 0x1f) << 5 | __val\n\t"
        ".endm");

__asm__(".macro scfgr_w val, addr\n\t"
        "def_insn 0x06492000 \\val, \\addr \n\t"
        ".endm");

__asm__(".macro scfgr_d val, addr\n\t"
        "def_insn 0x06492400 \\val, \\addr \n\t"
        ".endm");

__asm__(".macro scfgw_w val, addr\n\t"
        "def_insn 0x06492800 \\val, \\addr \n\t"
        ".endm");

__asm__(".macro scfgw_d val, addr\n\t"
        "def_insn 0x06492c00 \\val, \\addr \n\t"
        ".endm");

static inline unsigned int r_scfg_w(unsigned int addr)
{
  unsigned int val;
  asm volatile("scfgr_w %0, %1" : "=r" (val) : "r"(addr));
  return val;
}

static inline unsigned long r_scfg_d(unsigned int addr)
{
  unsigned long val;
  asm volatile("scfgr_d %0, %1" : "=r" (val) : "r"(addr));
  return val;
}

static inline void w_scfg_w(unsigned int val, unsigned int addr)
{
  asm volatile("scfgw_w %0, %1" : :"r"(val), "r"(addr));
}

static inline void w_scfg_d(unsigned long val, unsigned int addr)
{
  asm volatile("scfgw_d %0, %1" : :"r"(val), "r"(addr));
}

extern void initialize_shadow_stack(void);

#endif
