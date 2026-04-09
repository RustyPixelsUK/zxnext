#include <arch/zxn.h>
#include <intrinsic.h>
#include <string.h>
#include "sprite.h"
#include "../../config/zconfig.h"
#include "bank.h"

void sprite_set_display_palette(bool first_palette)
{
	IO_NEXTREG_REG = REG_PALETTE_CONTROL;
	IO_NEXTREG_DAT = (IO_NEXTREG_DAT & 0xF7) | (first_palette ? 0x00 : 0x08);
}

void sprite_set_rw_palette(bool first_palette)
{
	IO_NEXTREG_REG = REG_PALETTE_CONTROL;
	IO_NEXTREG_DAT = (IO_NEXTREG_DAT & 0x8F) | (first_palette ? 0x20 : 0x60);
}

void sprite_set_pattern(uint8_t sprite_slot, const void *sprite_pattern, bool pattern_4bit)
{
	if (pattern_4bit)
	{
		IO_SPRITE_SLOT = (sprite_slot >> 1) | (sprite_slot << 7);
		intrinsic_outi((void *) sprite_pattern, __IO_SPRITE_PATTERN, 128);
	}
	else
	{
		IO_SPRITE_SLOT = sprite_slot & SPRITE_SLOT_MASK;
		intrinsic_outi((void *) sprite_pattern, __IO_SPRITE_PATTERN, 256);
	}
}

void sprite_set_attributes(uint8_t sprite_index,
							uint8_t pattern_slot,
							uint16_t x,
							uint16_t y,
							uint8_t scale_x,
							uint8_t scale_y,
							uint8_t palette_offset,
							uint8_t sprite_flags)
{
	const uint8_t N = (sprite_flags & SPRITE_4BIT ? pattern_slot >> 1 : pattern_slot) & SPRITE_SLOT_MASK;
	const uint8_t N6 = (sprite_flags & SPRITE_4BIT ? pattern_slot & 1 : 0);
	
	IO_SPRITE_SLOT = sprite_index;
	IO_SPRITE_ATTRIBUTE = X_LSB(x);
	IO_SPRITE_ATTRIBUTE = Y_LSB(y);
	IO_SPRITE_ATTRIBUTE = (palette_offset << PALETTE_OFFSET_SHIFT) | X_MSB(x) | (sprite_flags & 0xF);
	IO_SPRITE_ATTRIBUTE = (sprite_flags & SPRITE_VISIBLE) | SPRITE_ENABLE_ATTRIB_4 | N;

	uint8_t attrib4 = 0;
	
	if (sprite_flags & SPRITE_ANCHOR)
	{
		attrib4 |= (SPRITE_4BIT << 1); // 4-bit
		attrib4 |= (N6 << 6); // N6 pattern bit
		attrib4 |= (1 << 5); // unified
		attrib4 |= Y_MSB(y); // 9th bit of y coord
	}
	else // unified
	{
		attrib4 |= (1 << 6);
		attrib4 |= (N6 << 5); // N6 pattern bit
		attrib4 |= 1; // pattern number is relative to the anchor’s
	}

	attrib4 |= scale_x << 3; // magnification in the x direction
	attrib4 |= scale_y << 1; // magnification in the y direction
	
	IO_SPRITE_ATTRIBUTE = attrib4;
}

void sprite_set_palette(const uint16_t *colors, uint16_t length, uint8_t palette_index)
{
	uint8_t *color_bytes = (uint8_t *) colors;
	uint16_t i;

	if ((colors == NULL) || (length == 0))
	{
		//return;
	}

	if (palette_index + length > 256)
	{
		length = 256 - palette_index;
	}

	IO_NEXTREG_REG = REG_PALETTE_INDEX;
	IO_NEXTREG_DAT = palette_index;

	IO_NEXTREG_REG = REG_PALETTE_VALUE_16;
	for (i = 0; i < (length << 1); i++)
	{
		IO_NEXTREG_DAT = color_bytes[i];
	}
}

void sprite_set_color(uint16_t color, uint8_t palette_index)
{
	sprite_set_palette(&color, 1, palette_index);
}

void sprites_clear(void)
{
	ZXN_NEXTREG(REG_SPRITE_SELECT, 0);
	
	IO_NEXTREG_REG = REG_SPRITE_ATTRIBUTES_INC;

	for (unsigned int i = 0; i != 128; ++i)
		IO_NEXTREG_DAT = 0;
}

void sprites_hide(void)
{
	for (uint8_t i = 0; i < 32; i++)
	{
		uint8_t sprite_index = i * 4;
		sprite_set_attributes(sprite_index, sprite_index, 0, 0, 0, 0, 0, SPRITE_ANCHOR | SPRITE_4BIT);
	}
}

void sprite_update(uint8_t i, uint8_t x, uint8_t y)
{
	uint8_t sprite_index = i * 4;
	sprite_set_attributes(sprite_index, sprite_index, x, y, 0, 0, 0, SPRITE_VISIBLE | SPRITE_ANCHOR | SPRITE_4BIT);
}
