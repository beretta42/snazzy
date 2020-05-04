	export start
	export _di
	export _ei
	export _timer
	export _ashrhi3
	export _mulhi3
	export _udivhi3
	export _enable_poll
	export _memset
	export _strlen
	export _memcpy
	export _index
	export _exit
	export _timer
	export _tobasic

	export bihandle
	export interrupt
	export bsc_stack

	import _main
	import _kpoll
	import poll


	section .bounce


_tobasic:
	pshs	cc,dp,y,u
	orcc	#$50
	sts	szy_stack
	lds	bsc_stack
	ldx	bihandle
	stx	$fef8
	ldx	#$ffa4
	ldd	#$3c3d
	std	,x++
	ldd	#$3e3f
	std	,x
	puls	cc,dp,pc

swi3	jsr	tosnaz		; after this call
	puls	cc		; grab CC but drop the rest and return
	leas	11,s		; go back to the code that called
	rts			; the code that issued the swi3

reset	bra	reset


tosnaz: pshs	cc,dp
	orcc	#$50
	sts	bsc_stack
	lds	szy_stack	; actualy mem not mapped in yet
	ldx	#interrupt
	stx	$fef8
	ldx	#$ffa4
	ldd	#$0001
	std	,x++
	ldd	#$0203
	std	,x
	ldb	#$f0
	stb	$ff22
	clr	$ffc0
	clr	$ffc3
	clr	$ffc5
	ldx	#$6000
	jsr	_graf_setbuf
	puls	cc,dp,y,u,pc

szy_stack:
	.dw	0
bsc_stack:
	.dw	0

	section .data
	rmb	256
sttop
inkern  rmb	0

	section	.text

bihandle
	.dw	0
_timer	.dw	0
_enable_poll
	.db	0

start:	orcc	#$50		; off interrupts
	;; conjigure BASIC to hide bounce area
	ldx	#.bounce_base
	stx	$74		; TOPRAM
	stx	$27
	stx	$23
	leax	-200,x
	stx	$21
	;; dummy up a new BASIC stack
	tfr	x,s
	ldx	#$ac73		; stack return
	ldd	#$4000		; stack cc,dp  CC of just F, DP = 0
	pshs	d,x
	sts	bsc_stack	; save it
	lds	#sttop		; switch to our kernel stack
	ldb	#$80
	tfr	b,dp		; set DP
	lda	$fffc		; are we a coco3?
	cmpa	#$fe
	bne	b@
	sta	$ffd9		; high speed poke
b@	ldx	#reset
	stx	>$72
	ldx	#tosnaz
	ldx	$fef8		; save BASIC interrupt
	leax	$fefa,x		; convert to JMP address (was a lbsr)
	stx	bihandle
	ldb	#$7e		; set virq to jump
	stb	$fef7
	stb	$100		; set swi3 to jump
	ldx	#interrupt	; set interrupt vector
	stx	$fef8		;
	ldx	#swi3		; set swi3 vector
	stx	$101
	ldx	#.bss_base	; clear bss
	ldy	#.bss_len	;
a@	clr	,x+		;
	leay	-1,y		;
	bne	a@		;
	jsr	_main		; jump to C's main
c@	bra	c@		; if we return from main, then quit


_exit:	orcc	#$50
	clr	$72
	jmp	[0xfffe]


interrupt:
	;; fixme:
	ldd	_timer
	addd	#1
	std	_timer
	lslb
	lslb
	andb	#$80
	pshs	b
	ldb	$6000
	andb	#$7f
	orb	,s+
	stb	$6000
	jsr	_do_joy
	jsr     _kpoll          ; go poll keyboard
	lda     $ff02           ; clear vsync pia
a@      rti                     ; return from interrupt


_di:
	orcc    #$50            ; stop interrupts
	rts

_ei:
	andcc   #~$50           ; start interrupts
	rts

	;; set memory to a byte
_memset:
	pshs	y
	ldy	4,s
a@	stb	,x+
	leay	-1,y
	bne	a@
	puls	y,pc

	;; how long is a string
_strlen:
	pshs	x
a@	lda	,x+
	bne	a@
	leax	-1,x
	tfr	x,d
	subd	,s++
	tfr	d,x
	rts

	;; copy memory
	;; y u r s len
_memcpy:
	pshs	y,u
	ldu	6,s
	ldy	8,s
	beq	out@
a@	lda	,u+
	sta	,x+
	leay	-1,y
	bne	a@
out@	puls	y,u,pc

	;; find char in memory
_index:
	pshs	b
a@	ldb	,x+
	cmpb	,s
	beq	o@
	tstb
	bne	a@
	ldx	#0
	puls	b,pc
o@	leax	-1,x
	puls	b,pc



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
