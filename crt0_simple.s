	export start
	export _di
	export _ei
	export _call
	export _ashrhi3
	export _mulhi3
	export _udivhi3
	export _memset
	export _strlen
	export _memcpy
	export _index
	export _exit
	import _main

	section	.text

start:	orcc	#$50		; off interrupts
	lds	#$4000
	ldb	#$0e
	tfr	b,dp		; set DP
	lda	$fffc		; are we a coco3?
	cmpa	#$fe
	bne	b@
	sta	$ffd9		; high speed poke
b@	ldx	#.bss_base	; clear bss
	ldy	#.bss_len	;
a@	clr	,x+		;
	leay	-1,y		;
	bne	a@		;
	jsr	_main		; jump to C's main
_exit:	orcc	#$50
	clr	$72
	jmp	[0xfffe]
	

_di:
        orcc    #$50            ; stop interrupts
        rts

_ei:
        andcc   #~$50           ; start interrupts
        rts

_call:	jmp	,x

	
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
