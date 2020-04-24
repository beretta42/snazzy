	export	_open_ll
	export  _read_ll
	export  _write_ll
	export	_close_ll

	section .data
save	rmb	2

	section .text

DEVNUM	equ	$6f

	;; open a decb file
	;; b = BASIC open mode, 'I' or 'O'
_open_ll:
	pshs	dp,y,u		; save abused regs
	clra			; set DP to BASIC's
	tfr	a,dp
	sts	save		; save stack for error rewinding
	ldy	#open_err	; usurp BASIC error vector
	sty	$192
	tfr	b,a		; BASIC want mode in a, and b = devnum
	ldb	DEVNUM
	;; fixme: this DECB vectors should be grokked from the BASIC
	;; RAM vectors in DP. Don't assume decb1.1 locations!
	jsr	$c48d		; call 1.1 DECB's open
open_ok:
	ldx	#0
	puls	dp,y,u,pc
open_err:
	lds	save		; reload stack
	ldx	#-1		; return NULL
	puls	dp,y,u,pc

	;; read byte from file
_read_ll:
	pshs	dp
	clra
	tfr	a,dp
	sts	save
	ldx	#read_err
	stx	$192
	ldx	#read_ok
	pshs	x
	jsr	$16a		; A = byte read
read_ok:
	tst	<$70		; read BASIC's EOF flag
	bne	read_eof
	tfr	a,b
	clra
	tfr	d,x
	puls	dp,pc
read_err:
	lds	save
	ldx	#-2
	puls	dp,pc
read_eof:
	ldx	#-1
	puls	dp,pc

	;; write byte to file
	;;  b = char
_write_ll:
	pshs	dp
	clra
	tfr	a,dp
	sts	save
	ldx	#write_err
	stx	$192
	ldx	#write_ok
	pshs	x
	tfr	b,a
	jsr	$167
write_ok:
	ldx	#0
	puls	dp,pc
write_err:
	lds	save
	ldx	#-1
	puls	dp,pc


	;; x = fd
_close_ll:
	pshs	dp,y,u
	clra
	tfr	a,dp
	sts	save		; remember stack frame
	tfr	x,d
	stb	DEVNUM
	ldx	#close_err	; usurp any BASIC errors
	stx	$192
	ldx	#close_ok
	pshs	x		; push return address twice BASIC will gobble it
	jsr	$176		; call BASIC close ram hook
close_ok:
	ldx	#0		; closed ok!
	puls	dp,y,u,pc
close_err:
	lds	save		; reset stack frame
	ldx	#-1		; closed bad!
	puls	dp,y,u,pc
