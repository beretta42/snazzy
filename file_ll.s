	export	_open_ll
	export  _read_ll
	export  _write_ll
	export	_close_ll

	import	bihandle
	import	interrupt
	import  bsc_stack

	section .data


	section .bounce
bounce:
	;; flip BASIC into memory
	pshs	cc,d,dp,x,y,u
	orcc	#$50
	sts	kssav
	ifdef	KERNEL
	lds	bsc_stack
	endc
	sts	save
	pshs	d,x,y		; y is a dummy
	clr	rval
	clra
	tfr	a,dp
	ifdef	KERNEL
	ldx	bihandle
	stx	$fef8
	ldx	#$ffa4
	ldd	#$3c3d
	std	,x++
	ldd	#$3e3f
	std	,x
	endc ; KERNEL
	ldd	$192
	std	serr
	ldd	#bad
	std	$192
	ldd	#ret
	std	4,s
	puls	d,x
	andcc	#~$10
	jsr	[vect]		; A = read return
ret	sta	aval
	lds	save
	ifdef KERNEL
	orcc	#$50
	ldx	#$ffa4
	ldd	#$0001
	std	,x++
	ldd	#$0203
	std	,x
	ldx	#interrupt
	stx	$fef8
	endc ; KERNEL
	ldx	serr
	stx	$192
	lds	kssav
	lda	aval
	sta	1,s
	puls	cc,d,dp,x,y,u,pc
bad	com	rval
	bra	ret
kssav	rmb	2
save	rmb	2
vect	rmb	2
rval	rmb	1
serr	rmb	2
aval	rmb	1

	section .text

DEVNUM	equ	$6f


	;; open a decb file
	;; b = BASIC open mode, 'I' or 'O'
_open_ll:
	tfr	b,a		; BASIC want mode in a, and b = devnum
	ldb	DEVNUM
	ldx	#$c48d		; fixme: may not be DECB1.1 !
tail	stx	vect
	jsr	bounce
	ldb	rval
	sex			; wheee!
	tfr	d,x
	rts

	;; read byte from file
_read_ll:
	ldx	#$16a
	stx	vect
	jsr	bounce
	tst	rval
	bne	read_err
	tst	$70		; read BASIC's EOF flag
	bne	read_eof
	tfr	a,b
	clra
	tfr	d,x
	rts
read_err:
	ldx	#-2
	rts
read_eof:
	ldx	#-1
	rts

	;; write byte to file
	;;  b = char
_write_ll:
	tfr	b,a
	ldx	#$167
	bra	tail


_close_ll:
	ldx	#$176
	bra	tail
