#include <arch/zxn.h>
#include <z80.h>
#include <im2.h>
#include <intrinsic.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "../../config/zconfig.h"
#include "bank.h"
#include "sound.h"
#include "vt_sound.h"
#include "ayfx.h"
#include "layer2.h"
#include "tilemap.h"
//#include "ide_friendly.h"

unsigned char frame_counter = 0;

//IM2_DEFINE_ISR_8080(audio_isr)
IM2_DEFINE_ISR(audio_isr)
{
	// save nextreg register
	uint8_t save = IO_NEXTREG_REG;
	
	// update the clock
	++m_tick;
	
	m_milliseconds += 20; // 50 Hz

	// Check if 60 Hz mode.
	if (ZXN_READ_REG(REG_TURBO_MODE) & RTM_28MHZ)
	{
		// Compensate for 60 Hz mode by skip calling AYFX_FRAME every 6th frame.
		if (++frame_counter == 6)
		{
			frame_counter = 0;
			IO_NEXTREG_REG = save;
			return;
		}
	}

	intrinsic_di();

	uint8_t old_page = bank_set_8k(MMU_AUDIO, PAGE_AUDIO);
	
#if (ENABLE_SFX == 1)
	bank_set_8k(MMU_SFX0, PAGE_SFX0);
	SELECT_AY(AY_SFX0);
	afx_play_isr();
#endif

#if (ENABLE_MUSIC0 == 1)
	bank_set_8k(MMU_MUSIC0, PAGE_MUSIC0);
	SELECT_AY(AY_MUSIC0);
	vt_play_isr();
#endif
	
#if (ENABLE_MUSIC1 == 1)
	bank_set_8k(MMU_MUSIC1, PAGE_MUSIC1);
	SELECT_AY(AY_MUSIC1);
	vt_play_isr();
#endif

	bank_set_8k(MMU_AUDIO, old_page);

	intrinsic_ei();

	// restore nextreg register
	IO_NEXTREG_REG = save;
}

void audio_isr_init()
{
	intrinsic_di();
#if (CRT_6000 == 1)
	z80_bpoke(0x6161, 0xC3); // jp
	z80_wpoke(0x6162, (uint16_t) audio_isr);
#else
	z80_bpoke(0x8181, 0xC3); // jp
	z80_wpoke(0x8182, (uint16_t) audio_isr);
#endif
	intrinsic_ei();
}

void music_init()
{
	// Place the vt_sound binary and music module (located in bank 14 with origin
	// address 0) in MMU slots 0 and 1 for execution.
	// Note: The vt_sound binary (~2.2 KB) and music module must fit within 16 KB.
	uint8_t old_page = bank_set_8k(MMU_AUDIO, PAGE_AUDIO);

	// Initialize vt_sound with the music module.
#if (ENABLE_MUSIC0 == 1)
	bank_set_8k(MMU_MUSIC0, PAGE_MUSIC0);
	vt_init(music_module_0);
#endif

#if (ENABLE_MUSIC1 == 1)
	bank_set_8k(MMU_MUSIC1, PAGE_MUSIC1);
	vt_init(music_module_1);
#endif

	// Install the vt_play_isr() interrupt service routine. The music will now
	// play in the background. Note: Pages 28 and 29 must remain in MMU slots
	// 0 and 1 while music is playing and until stop_music() is called.
	//intrinsic_di();
#if (CRT_6000 == 1)
	//z80_bpoke(0x6161, 0xC3); // jp
	//z80_wpoke(0x6162, (uint16_t) vt_play_isr);
#else
	//z80_bpoke(0x8181, 0xC3); // jp
	//z80_wpoke(0x8182, (uint16_t) vt_play_isr);
#endif
	//intrinsic_ei();

	bank_set_8k(MMU_AUDIO, old_page);
}

void music_stop(void)
{
	// Disable the vt_play_isr() interrupt service routine and mute the sound.
#if (ENABLE_MUSIC0 == 1 || ENABLE_MUSIC1 == 1)
	vt_set_play_isr_enabled(false);
	vt_mute();
#endif

	// Install an empty interrupt service routine.
	intrinsic_di();
#if (CRT_6000 == 1)
	z80_bpoke(0x6161, 0xFB); // ei
	z80_bpoke(0x6162, 0xED); // reti
	z80_bpoke(0x6163, 0x4D);
#else
	z80_bpoke(0x8181, 0xFB); // ei
	z80_bpoke(0x8182, 0xED); // reti
	z80_bpoke(0x8183, 0x4D);
#endif
	intrinsic_ei();

	// Remove the vt_sound binary and music module by restoring the Spectrum ROM
	// in MMU slots 0 and 1.
	bank_set_esx();
}

void sfx_init(void)
{
	uint8_t old_page = bank_set_8k(MMU_AUDIO, PAGE_AUDIO);
	
	bank_set_8k(MMU_SFX0, PAGE_SFX0);
	afx_init(sfx0);

	bank_set_8k(MMU_AUDIO, old_page);
}

void sfx_play(uint8_t effect)
{
	uint8_t old_page = bank_set_8k(MMU_AUDIO, PAGE_AUDIO);
	
	bank_set_8k(MMU_SFX0, PAGE_SFX0);
	afx_play(effect);

	bank_set_8k(MMU_AUDIO, old_page);
}

void sfx_update(void)
{
	uint8_t old_page = bank_set_8k(MMU_AUDIO, PAGE_AUDIO);
	
	bank_set_8k(MMU_SFX0, PAGE_SFX0);
	afx_frame();

	bank_set_8k(MMU_AUDIO, old_page);
}
