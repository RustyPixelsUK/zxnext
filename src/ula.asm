SECTION rodata_user

EXTERN _font_spr
PUBLIC _ula_print_char
PUBLIC _ula_print_dec
PUBLIC _ula_print_hex

; a = char
; de = y/x
_ula_print_char:
	push hl
	push de
	pixelad
	sub $20
	ld d,8
	ld e,a
	mul d,e
	add de,_font_spr
	ld a,(de)
	ld (hl),a
	inc de
	pixeldn
	ld a,(de)
	ld (hl),a
	inc de
	pixeldn
	ld a,(de)
	ld (hl),a
	inc de
	pixeldn
	ld a,(de)
	ld (hl),a
	inc de
	pixeldn
	ld a,(de)
	ld (hl),a
	inc de
	pixeldn
	ld a,(de)
	ld (hl),a
	inc de
	pixeldn
	ld a,(de)
	ld (hl),a
	inc de
	pixeldn
	ld a,(de)
	ld (hl),a
	inc de
	pixeldn
	pop de
	pop hl
	ret

; hl = number
_ula_print_dec:
	pop af
	pop hl
	pop de
	push de
	push hl
	push af
	ld bc,-10000
	call num1
	ld bc,-1000
	call num1
	ld bc,-100
	call num1
	ld c,-10
	call num1
	ld c,-1
num1:
	ld a,'0'-1
num2:
	inc a
	add hl,bc
	jr c,num2
	sbc hl,bc
	call _ula_print_char
	add de,8
	ret

; hl = number
_ula_print_hex:
	pop af
	pop hl
	pop de
	push de
	push hl
	push af
	ld c,h
	call out_hex8
	ld c,l
out_hex8:
	ld a,c
	rra
	rra
	rra
	rra
	call conv
	ld a,c
conv:
	and $0F
	add a,$90
	daa
	adc a,$40
	daa
	call _ula_print_char
	add de,8
	ret
