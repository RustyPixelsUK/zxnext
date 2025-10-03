SECTION code_user

PUBLIC _breakpoint
PUBLIC _wait_vblank

NEXTREG_REGISTER_SELECT_PORT	= $243b
RASTER_LINE_MSB_REGISTER	= $1e
RASTER_LINE_LSB_REGISTER	= $1f

_breakpoint:
	DEFB $dd, $01
	ret

_wait_vblank:
	ld bc, NEXTREG_REGISTER_SELECT_PORT
	ld a, RASTER_LINE_MSB_REGISTER
	out (c), a
	inc b
_wait_vblank_loop1:
	in a, (c)
	and 1
	jp nz, _wait_vblank_loop1
	dec b
	ld a, RASTER_LINE_LSB_REGISTER
	out (c), a
	inc b
_wait_vblank_loop2:
	in a, (c)
	cp 192-64
	jp nz, _wait_vblank_loop2			; moved up so we can see on the border how long it is taking
	ret
