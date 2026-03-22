#include <arch/zxn.h>
#include <z80.h>
#include <intrinsic.h>
#include <stdint.h>
#include <stdbool.h>

#include "ctc.h"

static uint16_t ctc_ports[] =
{
	CTC_CH0_PORT,
	CTC_CH1_PORT,
	CTC_CH2_PORT,
	CTC_CH3_PORT,
	CTC_CH4_PORT,
	CTC_CH5_PORT,
	CTC_CH6_PORT,
	CTC_CH7_PORT
};

uint16_t ctc_port(uint8_t channel)
{
	return ctc_ports[channel & 0x07];
}

void ctc_init(uint8_t channel, uint8_t time_constant, bool use_prescaler_256, bool interrupt)
{
	uint16_t port = ctc_port(channel);
	uint8_t control = CTC_TIMER_NOINT;

	if (use_prescaler_256)
		control |= CTC_PRESCALER_256;

	if (interrupt)
		control |= CTC_INT_ENABLE;

	z80_outp(port, control);
	z80_outp(port, time_constant);
}

void ctc_stop(uint8_t channel)
{
	uint16_t port = ctc_port(channel);
	z80_outp(port, CTC_STOP);
}

// Video Timing register 0x11, bits 2-0 = mode:
//   000 = Base VGA,   clk28 = 28000000 Hz
//   001 = VGA set 1,  clk28 = 28571429 Hz
//   010 = VGA set 2,  clk28 = 29464286 Hz
//   011 = VGA set 3,  clk28 = 30000000 Hz
//   100 = VGA set 4,  clk28 = 31000000 Hz
//   101 = VGA set 5,  clk28 = 32000000 Hz
//   110 = VGA set 6,  clk28 = 33000000 Hz
//   111 = HDMI,       clk28 = 27000000 Hz

static uint32_t clk28_table[] =
{
	28000000,	// 0: Base VGA timing
	28571429,	// 1: VGA setting 1
	29464286,	// 2: VGA setting 2
	30000000,	// 3: VGA setting 3
	31000000,	// 4: VGA setting 4
	32000000,	// 5: VGA setting 5
	33000000,	// 6: VGA setting 6
	27000000	// 7: HDMI
};

// Native FPS as 8.8 fixed point, indexed by [base_fps_is_60][video_mode]
static uint16_t fps_table[2][8] =
{
	// 50 Hz base
	{
		12800,	// 50.00 Hz
		13061,	// 51.02 Hz
		13469,	// 52.61 Hz
		13714,	// 53.57 Hz
		14171,	// 55.36 Hz
		14629,	// 57.14 Hz
		15086,	// 58.93 Hz
		12800	// HDMI, always 50 Hz
	},
	// 60 Hz base
	{
		15360,	// 60.00 Hz
		15686,	// 61.22 Hz
		16000,	// 63.14 Hz
		16384,	// 64.29 Hz
		16857,	// 66.43 Hz
		17333,	// 68.57 Hz
		17857,	// 70.71 Hz
		15360	// HDMI, always 60 Hz
	}
};

uint32_t ctc_get_clk28(void)
{
	uint8_t mode = ZXN_READ_REG(0x11) & 0x07;
	return clk28_table[mode];
}

uint16_t ctc_get_native_fps(uint16_t *fp_ctc_tick_us)
{
	uint8_t base_is_60 = (ZXN_READ_REG(0x05) & 0x04) ? 1 : 0;
	uint8_t mode = ZXN_READ_REG(0x11) & 0x07;

	// CTC tick duration in microseconds as 8.8 fixed point.
	// One CTC tick = 16 / clk28 seconds = (16 * 1000000) / clk28 microseconds.
	// As 8.8 fixed point: (256 * 16 * 1000000) / clk28
	if (fp_ctc_tick_us)
		*fp_ctc_tick_us = (uint16_t)((256UL * 16UL * 1000000UL) / clk28_table[mode]);

	return fps_table[base_is_60][mode];
}
