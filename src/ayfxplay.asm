;-Minimal ayFX player v0.15 06.05.06---------------------------;
;                                                              ;
; The simplest effects player. Plays effects on one AY,        ;
; without music in the background.                             ;
; Priority of channel selection: if available                  ;
; if a free channel, selects one of them.                      ;
; if no channels, select the most long-sounding.               ;
; Playback uses the registers AF, BC, DE, HL, IX.              ;
;                                                              ;
; Initialization:                                              ;
; ld hl, the address of the effects bank                       ;
; call AFXINIT                                                 ;
;                                                              ;
; Start the effect:                                            ;
; ld a, the number of the effect (0..255)                      ;
; call AFXPLAY                                                 ;
;                                                              ;
; In the interrupt handler:                                    ;
; call AFXFRAME                                                ;
;                                                              ;
;--------------------------------------------------------------;

SECTION BANK_14

afxStart:

afxChDesc:          DS 3*4
;--------------------------------------------------------------;
; Initializing the effects player                              ;
; Turns off all channels, sets variables.                      ;
; Input: HL = bank address with effects                        ;
; a = Ay chip to use                                           ;
;--------------------------------------------------------------;
PUBLIC AYFX_INIT

AYFX_INIT:
        inc hl
        ld (afxBnkAdr+1),hl         ;save the address of the table of offsets

        ld hl,afxChDesc             ;mark all channels as empty
        ld de,$00ff
        ld bc,$03fd
afxInit0:
        ld (hl),d
        inc hl
        ld (hl),d
        inc hl
        ld (hl),e
        inc hl
        ld (hl),e
        inc hl
        djnz afxInit0

        ld hl,$ffbf                 ;initialize AY
        ld e,$15
afxInit1:
        dec e
        ld b,h
        out (c),e
        ld b,l
        out (c),d
        jr nz,afxInit1

        ld (afxNseMix+1),de         ;reset the player variables
        ret

;--------------------------------------------------------------;
; Playback of the current frame.                               ;
;--------------------------------------------------------------;

PUBLIC AYFX_FRAME

AYFX_FRAME:
        ld bc,$03fd
        ld ix,afxChDesc
afxFrame0:
        push bc

        ld a,11
        ld h,(ix+1)                 ;the comparison of the high-order byte of the address is <11
        cp h
        jr nc,afxFrame7             ;the channel cannot play, we skip
        ld l,(ix+0)

        ld e,(hl)                   ;get the value of the information byte
        inc hl

        sub b                       ;select the volume register:
        ld d,b                      ;(11-3=8, 11-2=9, 11-1=10)

        ld b,$ff                    ;output the volume value
        out (c),a
        ld b,$bf
        ld a,e
        and $0f
        out (c),a

        bit 5,e
        jr z,afxFrame1              ;tone does not change

        ld a,3                      ;select the tone registers
        sub d
        add a,a

        ld b,$ff                    ;output the tone values
        out (c),a
        ld b,$bf
        ld d,(hl)
        inc hl
        out (c),d
        ld b,$ff
        inc a
        out (c),a
        ld b,$bf
        ld d,(hl)
        inc hl
        out (c),d

afxFrame1:
        bit 6,e
        jr z,afxFrame3              ;noise does not change

        ld a,(hl)                   ;read the noise value
        sub $20
        jr c,afxFrame2              ;if less than $ 20, play on
        ld h,a
        ;ld b,$ff
        ld b,c                      ;in BC we record the longest time
        jr afxFrame6

afxFrame2:
        inc hl
        ld (afxNseMix+1),a          ;keep the noise value

afxFrame3:
        pop bc
        push bc                     ;restore the value of the cycle in B
        inc b                       ;number of shifts for flags TN

        ld a,%01101111              ;mask for flags TN
afxFrame4:
        rrc e
        rrca                        ;shift flags and mask
        djnz afxFrame4
        ld d,a

        ld bc,afxNseMix+2           ;store the values of the flags
        ld a,(bc)
        xor e
        and d
        xor e                       ;E is masked by D
        ld (bc),a

afxFrame5:                          ;increase the time counter
        ld c,(ix+2)
        ld b,(ix+3)
        inc bc

afxFrame6:
        ld (ix+2),c
        ld (ix+3),b

        ld (ix+0),l
        ld (ix+1),h                 ;save the changed address

afxFrame7:
        ld bc,4
        add ix,bc                   ;go to the next channel
        pop bc
        djnz afxFrame0

        ld hl,$ffbf                 ;output the value of noise and mixer
afxNseMix:
        ld de,0                     ;+1(E)=noise, +2(D)=mixer
        ld a,6
        ld b,h
        out (c),a
        ld b,l
        out (c),e
        inc a
        ld b,h
        out (c),a
        ld b,l
        out (c),d

        ret

;--------------------------------------------------------------;
; Launch the effect on a free channel. Without                 ;
; Without a free channel selects the longest sounding.         ;
; Input: A = Effect number 0..255                              ;
;--------------------------------------------------------------;

PUBLIC AYFX_PLAY

AYFX_PLAY:
        ld de,0                     ;in DE the longest time in search
        ld h,e
        ld l,a
        add hl,hl
afxBnkAdr:
        ld bc,0                     ;address of the effect offset table
        add hl,bc
        ld c,(hl)
        inc hl
        ld b,(hl)
        add hl,bc                   ;the effect address is in hl
        push hl                     ;save the effect address on the stack

        ld hl,afxChDesc             ;empty channel search
        ld b,3
afxPlay0:
        inc hl
        inc hl
        ld a,(hl)                   ;compare the channel time with the largest
        inc hl
        cp e
        jr c,afxPlay1
        ld c,a
        ld a,(hl)
        cp d
        jr c,afxPlay1
        ld e,c
        ld d,a                      ;remember the longest time
        push hl                     ;remember the channel address + 3 in IX
        pop ix
afxPlay1:
        inc hl
        djnz afxPlay0

        pop de                      ;take the effect address from the stack
        ld (ix-3),e
        ld (ix-2),d                 ;enter in the channel descriptor
        ld (ix-1),b
        ld (ix-0),b                 ;zero the playing time

        ret

afxEnd:
