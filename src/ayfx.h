#ifndef _AYFX_H
#define _AYFX_H

#include <stdint.h>
#include <stdbool.h>

// Initialization
extern void afx_init(const void *bank_address) __z88dk_fastcall;

// Start the effect
extern void afx_play(uint8_t effect) __z88dk_fastcall;

// In the interrupt handler
extern void afx_frame(void);

/*
 * This function is tailored for installation as a self-contained IM2 interrupt
 * service routine to play the module in the background.
 */
extern void afx_play_isr(void);

/*
 * Enables (true) or disables (false) the afx_play_isr() interrupt service
 * routine. The afx_play_isr() interrupt service routine is initially enabled.
 */
extern void afx_set_play_isr_enabled(bool enabled) __z88dk_fastcall;

#endif
