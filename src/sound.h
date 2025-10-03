#ifndef _SOUND_H
#define _SOUND_H

extern uint8_t sfx0[];
extern uint8_t music_module_0[];
extern uint8_t music_module_1[];

extern unsigned char m_tick;
extern unsigned long m_milliseconds;

#define AY_SFX0					1
#define AY_SFX1					2
#define AY_MUSIC0				3
#define AY_MUSIC1				2

#define SELECT_AY(n)			(IO_AY_REG = n | 0xfc)

void audio_isr_init();
void music_init(void);
void music_stop(void);
void sfx_init(void);
void sfx_play(uint8_t effect);
void sfx_update(void);

#endif
