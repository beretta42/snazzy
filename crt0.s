	export start
	export _di
	export _ei
	export _timer
	export _ashrhi3
	export _mulhi3
	export _udivhi3
	export _enable_poll
	import _main
	import _kpoll
	import poll

	section	.text

_timer	.dw	0
_enable_poll
	.db	0

start:	orcc	#$50		; off interrupts
	lda	$fffc		; are we a coco3?
	cmpa	#$fe
	bne	b@
	sta	$ffd9		; high speed poke
b@	ldx	#noop
	stx	$5c0c
	ldx	#interrupt	; set interrupt vector
	stx	$10d		;
	lds	#$8000		; set stack
	ldx	#.bss_base	; clear bss
	ldy	#.bss_len	;
a@	clr	,x+		;
	leay	-1,y		;
	bne	a@		;
	andcc	#~$50		; on interrupts
	jmp	_main		; jump to C's main

noop	rts
	
interrupt:
	ldd	_timer
	addd	#1
	std	_timer
        lda     $ff02           ; clear vsync pia
*	jsr	_do_joy
*       jsr     _kpoll          ; go poll keyboard
*	tst	_enable_poll	; is call serial polling ok
	beq	a@		; nope - skip
*       jsr     poll            ; go poll serial device
a@      rti                     ; return from interrupt

_di:
        orcc    #$50            ; stop interrupts
        rts

_ei:
        andcc   #~$50           ; start interrupts
        rts

_ashrhi3:
        pshs    x
1$:
        leax    -1,x
        cmpx    #-1
        beq     2$
        asra
        rorb
        bra     1$
2$:
        puls    x,pc


_mulhi3:
        pshs    x
        lda   5,s   ; left msb * right lsb * 256
        ldb   ,s
        mul
        tfr   b,a
        clrb
        tfr   d,x
        ldb   1,s   ; left lsb * right msb * 256
        lda   4,s
        mul
        tfr   b,a
        clrb
        leax  d,x
        ldb   1,s   ; left lsb * right lsb
        lda   5,s
        mul
        leax  d,x
        puls    d,pc  ; kill D to remove initial push


_udivhi3:
        ldd     2,s
        beq     div0
        pshs    x
        jsr     _euclid
        puls    x,pc

        left=5
        right=1                 ; word
        count=0                 ; byte
        CARRY=1                 ; alias
_euclid:
        leas    -3,s            ; 2 local variables
        clr     count,s         ; prescale divisor
        inc     count,s
        tsta
presc:
        bmi     presc_done
        inc     count,s
        aslb
        rola
        bra     presc
presc_done:
        std     right,s
        ldd     left,s
        clr     left,s          ; quotient = 0
        clr     left+1,s
mod1:
        subd    right,s         ; check subtract
        bcc     mod2
        addd    right,s
        andcc   #~CARRY
        bra     mod3
mod2:
        orcc    #CARRY
mod3:
        rol     left+1,s        ; roll in carry
        rol     left,s
        lsr     right,s
        ror     right+1,s
        dec     count,s
        bne     mod1
        leas    3,s
	rts
div0:	bra	div0
