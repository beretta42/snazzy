	export _ll_clear
	export _ll_setpixel
	export _ll_cset
*	export _tgi_char_blit
	export _ll_char_draw
	export _tgi_char_blit_erase
	export _tgi_char_blit_rewrite
*	export _ll_hline
	export _ll_vline
	export _tgi_put_mouse
	export _tgi_unput_mouse
	export _testfrm
	export _ll_hline_setup
	export _ll_hline_go

	import _font

	section .text

;;; clear the screen
;;; fixme: should clear to pen color
_ll_clear
	ldb	#128
	pshs	b,y,u
	ldu	#$6000+(32*192)
	ldd	#0		; fixme: self modify from cset here
	ldy	#0
	ldx	#0
a@	pshu	d,x,y
	pshu	d,x,y
	pshu	d,x,y
	pshu	d,x,y
	pshu	d,x,y
	pshu	d,x,y
	pshu	d,x,y
	pshu	d,x,y
	dec	,s
	bne	a@
	puls	b,y,u,pc


;;; put a pixel on screen  x y r y
_ll_setpixel
	pshs	x,y
	ldb	1,s
	andb	#$7
smc1	ldy	#tab
	leay	b,y
	ldd	,s
	lsrb
	lsrb
	lsrb
	pshs	d
	ldb	9,s
	lda	#32
	mul
	addd	,s++
	addd	#$6000
	tfr	d,x
	ldb	,x
smc2	orb	,y
	stb	,x
	puls	x,y,pc


_ll_cset
	cmpx	#0
	beq	a@
	ldx	#tab
	stx	smc1+2
	stx	smc3+1
	stx	smc11+1
	;; patch up hline_setup
	stx	smc25+2 	; set tab addr
	ldb	#$00		; set initial mask
	stb	smc26+1
	ldb	#$aa		; ora ,s opcode
	stb	smc27
	ldb	#$25		; bcs
	stb	smc28
	ldd	#tabl		; set tabl
	std	smc29+2
	ldb	#$8a		; ora opcode
	stb	smc21
	stb	smc24
	ldb	#$ff		; lda #$ff opcode
	stb	smc23+1
	ldd	#$1cfe		; andcc opcode	
	std	smc30
	;; 
	ldb	#$ea
	stb	smc2
	stb	smc5
	ldb	#$ca
	stb	smc12
	ldb	#$80
	stb	smc4+1
	ldb	#$24
	stb	smc6
	rts
a@
	ldx	#tabi
	stx	smc1+2
	stx	smc3+1
	stx	smc11+1
	;; patch up hline_setup
	stx	smc25+2 	; set tabi addr
	ldb	#$ff		; set initial mask
	stb	smc26+1
	ldb	#$a4		; anda ,s opcode
	stb	smc27
	ldb	#$24		; bcc
	stb	smc28
	ldd	#tabli		; set tabl
	std	smc29+2
	ldb	#$84		; anda #0 opcode
	stb	smc21
	stb	smc24
	ldb	#$00		; lda #$ff opcode
	stb	smc23+1
	ldd	#$1a01
	std	smc30
	;; 
	ldb	#$e4
	stb	smc2
	stb	smc5
	ldb	#$c4
	stb	smc12
	ldb	#~$80
	stb	smc4+1
	ldb	#$25
	stb	smc6
	rts
	
	;; table of shifted bit masks
tab
	.db	$80
	.db	$40
	.db	$20
	.db	$10
	.db	$08
	.db	$04
	.db	$02
	.db	$01
tabi
	.db	~$80
	.db	~$40
	.db	~$20
	.db	~$10
	.db	~$08
	.db	~$04
	.db	~$02
	.db	~$01

tabl:
	.db	$00
	.db	$80
	.db	$c0
	.db	$e0
	.db	$f0
	.db	$f8
	.db	$fc
	.db	$fe

tabli:
	.db	$ff
	.db	$7f
	.db	$3f
	.db	$1f
	.db	$0f
	.db	$07
	.db	$03
	.db	$01

;;; put a char on screen
;;;   b x y, u, r y ptr
_ll_char_draw
*_tgi_char_blit
	pshs	b,x,y,u
	;; u = find ptr to glyph
	ldu	11,s
	;; y = find screen ptr
	tfr	x,d
	lsrb
	lsrb
	lsrb
	pshs	d
	ldb	12,s
	lda	#32
	mul
	addd	,s++
	addd	#$6000
	tfr	d,y
	;; tos = find rotation
	ldb	2,s
	andb	#7
	pshs	b
	;; blit
	ldb	,u+
	bsr	foo
	ldb	,u+
	bsr	foo
	ldb	,u+
	bsr	foo
	ldb	,u+
	bsr	foo
	ldb	,u+
	bsr	foo
	ldb	,u+
	bsr	foo
	;; return
	puls	d,x,y,u,pc

foo
	lda	2,s
	pshs	a
	beq	b@
	clra
a@	lsrb
	rora
	dec	,s
	bne	a@
	ora	1,y
	sta	1,y
b@	orb	,y
	stb	,y
	leay	32,y
	puls	b,pc


;;; put a char on screen
;;;   b x y, u, r y ptr
_tgi_char_blit_erase
	pshs	b,x,y,u
	;; u = find ptr to glyph
	ldu	11,s
	;; y = find screen ptr
	tfr	x,d
	lsrb
	lsrb
	lsrb
	pshs	d
	ldb	12,s
	lda	#32
	mul
	addd	,s++
	addd	#$6000
	tfr	d,y
	;; tos = find rotation
	ldb	2,s
	andb	#7
	pshs	b
	;; blit
	ldb	,u+
	bsr	foo1
	ldb	,u+
	bsr	foo1
	ldb	,u+
	bsr	foo1
	ldb	,u+
	bsr	foo1
	ldb	,u+
	bsr	foo1
	ldb	,u+
	bsr	foo1
	;; return
	puls	d,x,y,u,pc

foo1	lda	2,s
	pshs	a
	beq	b@
	clra
a@	lsrb
	rora
	dec	,s
	bne	a@
	coma
	anda	1,y
	sta	1,y
b@	comb
	andb	,y
	stb	,y
	leay	32,y
	puls	b,pc

;;; put a char on screen - rewrite mode
;;;   b x y, u, r y ptr
_tgi_char_blit_rewrite
	pshs	b,x,y,u
	;; u = find ptr to glyph
	ldu	11,s
	;; y = find screen ptr
	tfr	x,d
	lsrb
	lsrb
	lsrb
	pshs	d
	ldb	12,s
	lda	#32
	mul
	addd	,s++
	addd	#$6000
	tfr	d,y
	;; tos = find rotation
	ldb	2,s
	andb	#7
	pshs	b
	;; find masks
	pshs	b
	ldd	#$f000
	tst	,s
	beq	a@
b@	lsra
	rorb
	dec	,s
	bne	b@
a@	leas	1,s
	coma
	comb
	sta	smc10+1
	stb	smc10+3
	;; blit
	lda	,u+
	bsr	foo2
	lda	,u+
	bsr	foo2
	lda	,u+
	bsr	foo2
	lda	,u+
	bsr	foo2
	lda	,u+
	bsr	foo2
	lda	,u+
	bsr	foo2
	;; return
	puls	d,x,y,u,pc

foo2	ldb	2,s
	pshs	b
	clrb
	tst	,s
	beq	b@
a@	lsra
	rorb
	dec	,s
	bne	a@
b@	pshs	d
	ldd	,y
smc10	anda	#0
	andb	#0
	ora	,s+
	orb	,s+
	std	,y
	leay	32,y
	puls	b,pc


;;; draw a vertial line
;;; t X y r Y H
_ll_vline:
	pshs	x,y
	;; y = find screen ptr
	tfr	x,d
	lsrb
	lsrb
	lsrb
	pshs	d
	ldb	9,s
	lda	#32
	mul
	addd	,s++
	addd	#$6000
	tfr	d,y
	;; b = mask
	ldb	1,s
	andb	#7
smc11	ldx	#tab
	ldb	b,x
	stb	smc12+1
	lda	9,s
	pshs	a
a@	ldb	,y
smc12	orb	#0
	stb	,y
	leay	32,y
	dec	,s
	bne	a@
	puls	b,x,y,pc

;;; draw a horizontal line
;;;   X y r Y W
;;;  fixme: more speedups possible, do we bother?
*_ll_hline
	pshs	x,y
	;; y = find screen ptr
	tfr	x,d
	lsrb
	lsrb
	lsrb
	pshs	d
	ldb	9,s
	lda	#32
	mul
	addd	,s++
	addd	#$6000
	tfr	d,y
	;; tos = bit mask
	ldb	1,s
	andb	#7
smc3	ldx	#tab
	ldb	b,x
	pshs	b
	;; loop
	lda	10,s
a@	ldb	,y
smc5	orb	,s
	stb	,y
	lsr	,s
smc6	bcc	b@
	leay	1,y
smc4	ldb	#$80
	stb	,s
b@	deca
	bne	a@
	puls	b,x,y,pc

mouse	.dw	0x8000, 0xe000, 0xf800, 0xfc00, 0xf000, 0x9800, 0x0c00, 0x0600
	.dw	0x0800, 0x0e00, 0x0f80, 0x0fc0, 0x0f00, 0x0980, 0x00c0, 0x0060
	.dw	0x8000, 0xe000, 0xf800, 0xfc00, 0xf000, 0x9800, 0x0c00, 0x0600
	.dw	0x4000, 0x7000, 0x7c00, 0x7e00, 0x7800, 0x4c00, 0x0600, 0x0300
	.dw	0x2000, 0x3800, 0x3e00, 0x3f00, 0x3c00, 0x2600, 0x0300, 0x0180
	.dw	0x1000, 0x1c00, 0x1f00, 0x1f80, 0x1e00, 0x1300, 0x0180, 0x00c0
	.dw	0x0800, 0x0e00, 0x0f80, 0x0fc0, 0x0f00, 0x0980, 0x00c0, 0x0060
	.dw	0x0400, 0x0700, 0x07c0, 0x07e0, 0x0780, 0x04c0, 0x0060, 0x0030
	.dw	0x0200, 0x0380, 0x03e0, 0x03f0, 0x03c0, 0x0260, 0x0030, 0x0018
	.dw	0x0100, 0x01c0, 0x01f0, 0x01f8, 0x01e0, 0x0130, 0x0018, 0x000c
_tgi_put_mouse:
_tgi_unput_mouse:
	;; X y u r Y
	pshs	x,y,u
	;; adjust from mouse 512x512 to screen ratio (256x192)
	tfr	x,d		; adjust X
	lsra
	rorb
	tfr	d,x
	ldd	8,s
	lsra			; divide by 4 to get 2y
	rorb
	lsrb
	pshs	b		; push 2y
	lsrb			; b = 1y
	addb	,s+		; add together for 3y
	std	8,s
	;; 
	tfr	x,d
	andb	#7
	pshs	b		; push modulus 8
	tfr	x,d
	lsrb
	lsrb
	lsrb
	pshs	b		; push row offset
	ldb	11,s		; get Y
	lda	#32
	mul
	addb	,s+		; add offset
	adca	#0		; 16-bitify add
	addd	#$6000		; add screen base
	tfr	d,x		; X = screen bytes
	ldb	,s+		; pull modulus (bit shifts)
	lda	#16
	mul
	addd	#mouse
	tfr	d,y
	ldb	#4
	pshs	b
	;; begin loop
a@
	ldd	,y++		; get first row
	eora	,x		; xor to screen data
	eorb	1,x		;
	std	,x		; and save back to screen
	leax	32,x		; next row
	ldd	,y++		; get first row
	eora	,x		; xor to screen data
	eorb	1,x		;
	std	,x		; and save back to screen
	leax	32,x		; next row
	;; loop inc
	dec	,s
	bne	a@
	puls	b,x,y,u,pc	; pull counter, retore, return

_testfrm:
	includebin "test.frm"


;;; test code down here
;;; X = X, r Y W
;;; X  y  r  Y  W
;;; 01 xx xx 02 10   $80 $1f
_ll_hline_setup:	
	pshs	x,y
	;; figure out screen buffer position
	tfr	x,d
	lsrb
	lsrb
	lsrb
	pshs	d
	ldb	9,s
	lda	#32
	mul
	addd	,s++
	addd	#$6000
	std	smc20+1
	;; figure first bytes out
	ldx	8,s		; get width
*	beq	out@		; no width quit fixme!
	ldb	1,s		; get low bits of x
	andb	#7		; bottom 3 bits are pixel  adress
smc25	ldy	#tab
	lda	b,y		; a = mask of first bit
smc26	ldb	#0		; push a zero as mask accumulator
	pshs	b
smc27
a@	ora	,s		; apply mask to accumulator
	sta	,s
smc30	orcc	#$01		; 
	rora			; shift mask one bit over
smc28	bcs	c@
	leax	-1,x		; decrement width
	bne	a@
	;; we're on a byte boundary now
	;; figure out how many whole bytes
c@	leax	-1,x
b@	puls	b
	stb	smc21+1		; store it to be ored/anded
	tfr	x,d		; width
	lsrb
	lsrb
	lsrb			; b is no of whole bytes
	stb	smc22+1		; store it in counter
	;; last byte
last@	tfr	x,d		; get width
	andb	#7		; how many bits are remaining ?
smc29	ldy	#tabl
	ldb	b,y		; 
	stb	smc24+1		; store mask to be ored/anded
out@	puls	x,y,pc	
	
	
_ll_hline_go:
smc20	ldx 	#0		; screen loc
	;; apply first byte
	lda	,x		; get screen data
smc21	ora	#0		; apply mask
	sta	,x+		; save to screen
	;; apply whole bytes
smc22	ldb	#0
	beq 	last@
smc23	lda	#$ff
a@	sta	,x+
	decb
	bne	a@
	;; apply last byte
last@	lda	,x		; get screen data
smc24	ora	#0
	sta	,x		; save to screen
	ldd	smc20+1		; increment the screen position
	addd	#32		; to be ready for next hline call
	std	smc20+1		;
	rts
