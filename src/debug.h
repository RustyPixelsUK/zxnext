#ifndef _DEBUG_H
#define _DEBUG_H

extern void breakpoint(void);
extern void wait_vblank(void);

/* Inline CSpect breakpoint: emits the FD 00 trap bytes at the call site.
 * CSpect must be launched with -brk to enable the trap; without -brk
 * these bytes run as an undocumented Z80 opcode (FD prefix + NOP, no-op).
 * Opcode changed from DD 01 to FD 00 in CSpect v2.19.9.1. */
#define DBG_BREAK() __asm defb 0xfd, 0x00 __endasm

#endif
