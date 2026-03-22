#ifndef _CTC_H
#define _CTC_H

#include <stdint.h>
#include <stdbool.h>

// ZX Spectrum Next CTC channel ports
#define CTC_CH0_PORT			0x183B
#define CTC_CH1_PORT			0x193B
#define CTC_CH2_PORT			0x1A3B
#define CTC_CH3_PORT			0x1B3B
#define CTC_CH4_PORT			0x1C3B
#define CTC_CH5_PORT			0x1D3B
#define CTC_CH6_PORT			0x1E3B
#define CTC_CH7_PORT			0x1F3B

// CTC control word bits
#define CTC_CONTROL				0x01	// Bit 0: 1 = control word
#define CTC_RESET				0x02	// Bit 1: 1 = software reset
#define CTC_TC_FOLLOWS			0x04	// Bit 2: 1 = time constant follows
#define CTC_TRIGGER_PULSE		0x08	// Bit 3: 1 = CLK/TRG pulse trigger (counter mode)
#define CTC_EDGE_RISING			0x10	// Bit 4: 1 = rising edge (counter mode)
#define CTC_PRESCALER_256		0x20	// Bit 5: 0 = prescaler 16, 1 = prescaler 256
#define CTC_MODE_COUNTER		0x40	// Bit 6: 0 = timer mode, 1 = counter mode
#define CTC_INT_ENABLE			0x80	// Bit 7: 1 = interrupt enabled

// Prescaler values for ctc_init
#define CTC_PRESCALER_16		0
//#define CTC_PRESCALER_256		// Already defined above as control word bit

// Timer mode: interrupt enabled, timer mode, time constant follows, reset
#define CTC_TIMER_INT			(CTC_CONTROL | CTC_RESET | CTC_TC_FOLLOWS | CTC_INT_ENABLE)
// Timer mode: no interrupt, timer mode, time constant follows, reset
#define CTC_TIMER_NOINT			(CTC_CONTROL | CTC_RESET | CTC_TC_FOLLOWS)
// Stop: reset channel, no interrupt
#define CTC_STOP				(CTC_CONTROL | CTC_RESET)

/*
 * Initialise a CTC channel in timer mode.
 *
 * channel:        CTC channel number (0-7)
 * time_constant:  Timer divisor (1-256, 0 means 256)
 * use_prescaler_256: false = prescaler 16, true = prescaler 256
 * interrupt:      true to enable interrupt on terminal count
 *
 * Timer rate = clk28 / (prescaler * time_constant)
 * where prescaler is 16 or 256.
 *
 * Example: For 8 kHz at 28 MHz with prescaler 16:
 *   time_constant = 28000000 / (16 * 8000) = 219
 */
void ctc_init(uint8_t channel, uint8_t time_constant, bool use_prescaler_256, bool interrupt);

/*
 * Stop a CTC channel (reset and disable interrupt).
 *
 * channel: CTC channel number (0-7)
 */
void ctc_stop(uint8_t channel);

/*
 * Get the CTC port address for a channel number.
 *
 * channel: CTC channel number (0-7)
 * Returns the 16-bit I/O port address.
 */
uint16_t ctc_port(uint8_t channel);

/*
 * Get the native video FPS and CTC tick duration for the current hardware.
 *
 * fp_ctc_tick_us: OUT parameter, receives the duration of one CTC tick
 *                 in microseconds as a 8.8 fixed point value.
 *                 CTC tick = clk28 / 16.
 *
 * Returns the native FPS as a 8.8 fixed point value (e.g. 12800 = 50.0 Hz).
 *
 * Reads NextReg 0x05 (50/60 Hz base) and 0x11 (video timing mode) to
 * determine clk28 frequency and actual refresh rate.
 */
uint16_t ctc_get_native_fps(uint16_t *fp_ctc_tick_us);

/*
 * Get the clk28 frequency in Hz for the current video timing mode.
 *
 * Returns the clock frequency (e.g. 28000000 for base VGA timing).
 */
uint32_t ctc_get_clk28(void);

#endif
