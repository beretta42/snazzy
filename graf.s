	export _graf_clear
	export _graf_setpixel
	export _graf_cset
	export _graf_char_draw
	export _graf_put_mouse
	export _graf_unput_mouse
	export _graf_setclip
	export _graf_setbuf
	export _graf_bar

	import _font

	section .text

;;; sets the graphics buffer location
;;; returns the actualy buffer used
_graf_setbuf:
	tfr	x,d
	ldx	#$ffc6
	lsra			; a = pia setting
	pshs	a
	ldb	#7		; shift it 7 times
a@	lsra
	bcs	b@
	clr	,x
	bra	c@
b@	clr	1,x
c@	leax	2,x
	decb
	bne	a@
	puls	a
	lsla
	clrb
	tfr	d,x
	rts


;;; sets the clipping rectangle
;;; r y w h
_graf_setclip:
	jsr	set_xywh
	ldd	<xin
	std	<west
	addd	<win
	std	<east
	ldd	<yin
	std	<north
	addd	<hin
	std	<south

;;; sets DP args from C stack
;;; r r y w h, X has x
set_xywh:
	stx	<xin
	ldx	4,s
	stx	<yin
	ldx	6,s
	stx	<win
	ldx	8,s
	stx	<hin
	ldx	<xin
	rts

;;; clear the screen
;;; fixme: should clear to pen color
_graf_clear
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
_graf_setpixel
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


_graf_cset
	cmpx	#0
	beq	a@
	ldx	#tab
	stx	smc1+2
	;; patch up hline_setup
	ldb	#$12		; noop
	stb	smc40
	stb	smc41
	ldb	#$8a		; ora opcode
	stb	smc21
	stb	smc24
	ldb	#$ff		; lda #$ff opcode
	stb	smc23+1
	;;
	ldb	#$ea
	stb	smc2
	rts
a@
	ldx	#tabi
	stx	smc1+2
	;; patch up hline_setup
	ldb	#$53		; comb
	stb	smc40
	stb	smc41
	ldb	#$84		; anda #0 opcode
	stb	smc21
	stb	smc24
	ldb	#$00		; lda #$ff opcode
	stb	smc23+1
	;;
	ldb	#$e4
	stb	smc2
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


;;; put a char on screen
;;;  fixme: cliping for X axis
;;;   b X y, u, r Y PTR
_graf_char_draw
	pshs	b,x,y,u
	;; new code reuse
	stx	<xin
	ldx	9,s
	stx	<yin
	ldd	#8
	std	<win
	ldd	#6
	std	<hin
	jsr	calc
	bcs	out@
	;; get new height push as counter
	ldb	<hin+1
	pshs	b
	;; if cut off top then jump through source
	ldu	12,s
	ldb	<vtop
	ble	b@
	leau	b,u
	;; ptrs for copying from glyph to screen
b@	ldy	<scrpos
	;; tos = find rotation calc duff's
	ldb	<hleft		; see if we shift left (only off west edge)
	beq	c@
	tst	<west+1		; only apply if are west boundary isn't zero
	bne	c@
	negb
	addb	#7
	stb	smc102+1
	ldb	#smc102-smc99-2
	stb	smc99+1
	bra	a@
c@	ldb	3,s
	andb	#7
	negb
	addb	#7
	lslb			; multiply by two
	stb	smc99+1
	;; blit
a@	lda	,u+
	clrb
smc99	bra	end@
	lsra
	rorb
	lsra
	rorb
	lsra
	rorb
	lsra
	rorb
	lsra
	rorb
	lsra
	rorb
	lsra
	rorb
end@	anda	<fmask
	andb	<lmask
	ora	,y
	orb	1,y
	std	,y
	leay	32,y
	dec	,s		; dec counter
	bne	a@		; loop if more
	puls	b
	;; return
out@	puls	b,x,y,u,pc	; pull counter, restore
	;; if over west edge of boundary, the rotate left
smc102	bra	end2@
	lsla
	lsla
	lsla
	lsla
	lsla
	lsla
	lsla
end2@	bra	end@

mouse_new:
	.db	%00000000,%11000000
	.db	%01000000,%11100000
	.db	%01100000,%11110000
	.db	%01110000,%11111000
	.db	%01111000,%11111100
	.db	%01111100,%11111110
	.db	%00010000,%01111100
	.db	%00011000,%00111100
	.db	%00000000,%00011000

mouse_scrptr:	.dw	0
mouse_data:	rmb	2*9

	;; c X y u r Y
_graf_put_mouse:
	pshs	x,y,u
	stx	<xin
	ldx	8,s
	stx	<yin
	ldx	#8
	stx	<win
	ldx	#9
	stx	<hin
	jsr	calc
	bcs	out@
	;; get new height push as counter
	ldb	<hin+1
	pshs	b
	;; set undraw's height
	stb	smc101+1
	;; setup loop vars
	ldx	#mouse_data
	ldu	#mouse_new
	ldy	<scrpos
	sty	mouse_scrptr
	;; find shift for duff's rotate
	ldb	2,s
	andb	#7
	negb
	addb	#7
	lslb
	stb	smc100+1
a@	lda	,u+
	bsr	foo
	pshs	d
	lda	,u+
	bsr	foo
	coma
	comb
	pshs	d
	ldd	,y
	std	,x++
	anda	,s+
	andb	,s+
	ora	,s+
	orb	,s+
	std	,y
	leay	32,y
	dec	,s
	bne	a@
	puls	b
out@	puls	x,y,u,pc
foo	clrb
smc100	bra	end@
	lsra
	rorb
	lsra
	rorb
	lsra
	rorb
	lsra
	rorb
	lsra
	rorb
	lsra
	rorb
	lsra
	rorb
	anda	<fmask
	andb	<lmask
end@	rts

_graf_unput_mouse:
	pshs	u
	ldu	#mouse_data
	ldx	mouse_scrptr
smc101	ldb	#9
	pshs	b
a@	ldd	,u++
	std	,x
	leax	32,x
	dec	,s
	bne	a@
end@	puls	b,u,pc


;;; Draw a bar
;;; r Y W H
_graf_bar:
	jsr	set_xywh
	jsr	calc
	ldx	<scrpos
	stx	smc20+1
	ldb	<fmask
smc40	nop
	stb	smc21+1
	ldb	<whole
	stb	smc22+1
	ldb	<lmask
smc41	nop
	stb	smc24+1
	ldb	<hin+1
	pshs	b
a@	bsr	_graf_hline_go
	dec	,s
	bne	a@
	puls	a,pc


_graf_hline_go:
	tst	<nodraw
	bne	out@
smc20	ldx	#0		; screen loc
	;; apply first byte
	lda	,x		; get screen data
smc21	ora	#0		; apply mask
	sta	,x+		; save to screen
	;; apply whole bytes
smc22	ldb	#0
	beq	last@
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
out@	rts

	ifdef	0
;;; Scroll lie veritcally
;;; ll_scroll_up(int x, int y, int w, int o);
;;;   r  Y  W  O
;;;   0  2  4  6
_graf_scroll_up_set:
	stx	<xin
	ldx	2,s
	stx	<yin
	ldx	4,s
	stx	<win
	jsr	calc
	ldx	<scrpos
	stx	smc51+1
	ldd	6,s
	leax	d,x
	stx	smc50+1
	ldb	<fmask
	stb	smc52+1
	comb
	stb	smc53+1
	ldb	<whole
	stb	smc54+1
	ldb	<lmask
	stb	smc55+1
	comb
	stb	smc56+1
	ldd	#32
	std	smc57+1
	rts

;;; Scroll lie veritcally
;;; ll_scroll_up(int x, int y, int w, int o);
;;;   r  Y  W  O
;;;   0  2  4  6
_graf_scroll_down_set:
	stx	<xin
	ldx	2,s
	stx	<yin
	ldx	4,s
	stx	<win
	jsr	calc
	ldx	<scrpos
	stx	smc50+1
	ldd	6,s
	leax	d,x
	stx	smc51+1
	ldb	<fmask
	stb	smc52+1
	comb
	stb	smc53+1
	ldb	<whole
	stb	smc54+1
	ldb	<lmask
	stb	smc55+1
	comb
	stb	smc56+1
	ldd	#-32
	std	smc57+1
	rts


_graf_scroll_go:
	pshs	u
smc50	ldx	#0		; from
smc51	ldu	#0		; to
	;; first byte is masked
smc52	lda	#0		; from mask
	anda	,x+		; get a byte from src
	pshs	a
smc53	lda	#0		; to mask
	anda	,u		; get masks byte from dest
	ora	,s+		; or the two together
	sta	,u+		; and put to dest
	;; copy whole bytes
smc54	ldb	#0		; how many
a@	lda	,x+
	sta	,u+
	decb
	bne	a@
	;; apply last byte
smc55	lda	#0		; from mask
	anda	,x+		; get byte from src
	pshs	a
smc56	lda	#0		; to mask
	anda	,u		; get masks from dest
	ora	,s+		; put to dest
	sta	,u
	;; increment ptrs
smc57	ldd	#32
	ldx	smc50+1
	leax	d,x
	stx	smc50+1
	ldx	smc51+1
	leax	d,x
	stx	smc51+1
	puls	u,pc

	endc

	section .dp
xin	.dw	0
yin	.dw	0
win	.dw	0
hin	.dw	0
west	.dw	0
east	.dw	256
north	.dw	0
south	.dw	192
scrpos	.dw	0
fmask	.db	0
whole	.db	0
lmask   .db	0
nodraw	.db	0
vtop	.db	0
hleft	.db	0

x2	.dw	0
y2	.dw	0


	section .text

calc:   clr	<vtop
	clr	<hleft
	;; Clip the X asis
	;; calc x prime
	ldd	<xin
	addd	<win
	std	<x2
	;; clip width
	ldd	<west
	cmpd	<xin
	ble	a@
	subd	<xin
	stb	<hleft
	ldd	<west
	std	<xin
a@	ldd	<east
	cmpd	<x2
	bgt	b@		; fixme: s/b bge ?
	std	<x2
	;; recalc width
b@	ldd	<x2
	subd	<xin
	std	<win
	ble	nd@
	;; Clip the Y axis
	;; calc y prime
	ldd	<yin
	addd	<hin
	std	<y2
	;; clip height
	ldd	<north
	cmpd	<yin
	ble	c@
	subd	<yin
	stb	<vtop
	ldd	<north
	std	<yin
c@	ldd	<south
	cmpd	<y2
	bgt	d@		; fixme s/b bge?
	std	<y2
	;; recalc height
d@	ldd	<y2
	subd	<yin
	std	<hin
	ble	nd@
	bra	e@
nd@	clr	<nodraw
	com	<nodraw
	rts
e@

	;; calc screen buffer position
	ldd	<xin
	lsrb
	lsrb
	lsrb
	tfr	d,x
	ldb	<yin+1
	lda	#32
	mul
	leax	d,x
	leax	$6000,x
	stx	<scrpos
	;; figure first byte mask
	ldb	<xin+1
	andb	#7		; bottom three are pixel address
	ldx	#tab
	lda	b,x
	ldx	<win
	clr	,-s
a@	ora	,s
	sta	,s
	lsra
	bcs	c@
	leax	-1,x
	bne	a@
	bra	b@
	;; we're on a byte boundary now
c@	leax	-1,x
b@	puls	b
	stb	<fmask
	tfr	x,d
	lsrb
	lsrb
	lsrb
	stb	<whole
	;; last byte
	tfr	x,d
	andb	#7
	ldx	#tabl
	ldb	b,x
	stb	<lmask

	clr	<nodraw
	rts
