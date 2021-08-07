        .inc    fx2-include.asm

;;; Bernd Porr, bp1@cn.stir.ac.uk
;;;
;;; 
	.org	0000h
	ljmp	main

	.org	0043h		; the IRQ2-vector
	ljmp	int2		; irq service-routine
	
	.org	0200h
	
	;; main
main:
	mov	DPTR,#CPUCS
	mov	a,#000010100b	; 48MHz clock
	movx	@DPTR,a

	mov	DPTR,#I2CS	; i2c control
	movx	a,@dptr		; get the status
	mov	dptr,#100h	; point to addr 100h
	movx	@dptr,a		; store status there

	;;
	mov	dptr,#I2CS	; start
	mov	a,#80h
	movx	@dptr,a
	;;
	mov	dptr,#I2DAT	; address
	mov	a,#10100000b	; write to eprom
	movx	@dptr,a		; write to addr 0
	;;
	mov	r3,#00h		; address:0h
	lcall	sendi2c
	mov	r3,#0c0h	; only usb id
	lcall	sendi2c
	mov	r3,#0d8h	; univ of stir,L
	lcall	sendi2c
	mov	r3,#013h	; univ of stir,H
	lcall	sendi2c
	mov	r3,#20h		; usbduxsigma,L
	lcall	sendi2c
	mov	r3,#00h		; usbduxsigma,H
	lcall	sendi2c
	mov	r3,#00h		; rev
	lcall	sendi2c
	mov	r3,#00h		; rev
	lcall	sendi2c
	mov	r3,#00h		; connected, 100kHz
	lcall	sendi2c	
	;;
	mov	dptr,#I2CS	; stop
	mov	a,#40h
	movx	@dptr,a

i2cov:	
	mov	DPTR,#I2CS	; i2c control
	movx	a,@dptr		; get the status
	mov	dptr,#101h	; point to addr 100h
	movx	@dptr,a		; store status there

	mov	DPTR,#0E680H
	mov	a,#0CH
	movx	@dptr,a		; disconnect

	;;; do nothing
loop:	ljmp	loop
	
	


;;; sends byte in r3 to the i2c controller
sendi2c:
	mov	dptr,#I2CS	; status
waiti2c:	
	movx	a,@dptr		; get it
	anl	a,#1		; done bit
	jz	waiti2c		; have to wait
	mov	dptr,#I2DAT	; data register
	mov	a,r3		; get the data
	movx	@dptr,a		; send it
	ret
	
;;; 
;;;
;;;
;;;
;;;
;;;
int2:	push	dps
	push	dpl
	push	dph
	push	dpl1
	push	dph1
	push	acc
	push	psw
	push	00h		; R0
	push	01h		; R1
	push	02h		; R2
	push	03h		; R3
	push	04h		; R4
	push	05h		; R5
	push	06h		; R6
	push	07h		; R7
		
	;; clear INT2
	mov	a,EXIF		; FIRST clear the USB (INT2) interrupt request
	clr	acc.4
	mov	EXIF,a		; Note: EXIF reg is not 8051 bit-addressable

	pop	07h
	pop	06h
	pop	05h
	pop	04h		; R4
	pop	03h		; R3
	pop	02h		; R2
	pop	01h		; R1
	pop	00h		; R0
	pop	psw
	pop	acc 
	pop	dph1 
	pop	dpl1
	pop	dph 
	pop	dpl 
	pop	dps
	reti






.End


