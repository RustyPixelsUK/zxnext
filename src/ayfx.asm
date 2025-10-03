SECTION BANK_14
;SECTION code_user

EXTERN AYFX_INIT
EXTERN AYFX_PLAY
EXTERN AYFX_FRAME
EXTERN VT_START

PUBLIC _afx_init
PUBLIC _afx_play
PUBLIC _afx_frame
PUBLIC _afx_play_isr
PUBLIC _afx_set_play_isr_enabled

DEFC AY_SFX         = 1
DEFC AY_SFX2        = 2
DEFC AY_MUSIC1      = 3 ; Main music channel (in game)
DEFC AY_MUSIC2      = 2

DEFC _IO_AY_REG     = 0xfffd
DEFC VT_PLAY = VT_START + 5

_silence_ay:
    ld hl,$FFBF                 ;initialize AY
    ld e,$15
silenceAyInit:
    dec e
    ld b,h
    out (c),e                   ;AY_SOUND_CONTROL_PORT
    ld b,l
    out (c),d                   ;AY_REGISTER_WRITE_PORT
    jr nz,silenceAyInit
    ret

_afx_init:
    ; hl contains afx bank address
    di
    push af
    push bc
    push de
    push hl
    push ix
    call AYFX_INIT
    pop ix
    pop hl
    pop de
    pop bc
    pop af
    ei
    ret

_afx_play:
    di
    push af
    push bc
    push de
    push hl
    push ix
    ld a,l
    call AYFX_PLAY
    pop ix
    pop hl
    pop de
    pop bc
    pop af
    ei
    ret

_afx_frame:
    di
    push af
    push bc
    push de
    push hl
    push ix
    call AYFX_FRAME
    pop ix
    pop hl
    pop de
    pop bc
    pop af
    ei
    ret

_afx_play_isr:
    di
    push af
    push bc
    push de
    push hl
    ex af,af'
    exx
    push af
    push bc
    push de
    push hl
    push ix
    push iy

    ; Check if afx_play_isr is enabled.
;    ld a,(_afx_play_isr_enabled)
;    or a
;    jr z,end

    ; Check if 60 Hz mode.
;    ld a,5
;    ld bc,$243B
;    out (c),a
;    inc b
;    in a,(c)
;    bit 2,a
;    jr z,play

    ; Compensate for 60 Hz mode by skip calling AYFX_FRAME every 6th frame.
;    ld a,(_frame_counter)
;    cp 5
;    jr nz,inc_frame_counter
;    ld a,0
;    ld (_frame_counter),a
;    jr end

;inc_frame_counter:
;    ld hl,_frame_counter
;    inc (hl)

play:
;    ld l,0x1c
;    ld a,l
;    mmu0 a
;    ld l,0x1d
;    ld a,l
;    mmu1 a

;    ld a,AY_SFX
;    ld bc,_IO_AY_REG
;    or 0xfc
;    out (c), a

    call AYFX_FRAME

;    ld a,AY_MUSIC1
;    ld bc,_IO_AY_REG
;    or 0xfc
;    out (c), a

;    call VT_PLAY

;    ld l,0xff
;    ld a,l
;    mmu0 a
;    ld l,0xff
;    ld a,l
;    mmu1 a

end:
    pop iy
    pop ix
    pop hl
    pop de
    pop bc
    pop af
    exx
    ex af,af'
    pop hl
    pop de
    pop bc
    pop af
    ei
    reti

_afx_set_play_isr_enabled:
    ; l contains enablement/disablement parameter
    ld a,l
    ld (_afx_play_isr_enabled),a
    ret

_afx_play_isr_enabled:
    DEFB 1

_frame_counter:
    DEFB 0
