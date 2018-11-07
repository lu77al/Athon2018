;*************************
; Разные полезные макросы
;*************************
.macro	stsi
	ldi	r16,@1
	sts	@0,r16
.endm

.macro	stdi
	ldi	r16,@1
	std	Y+@0,r16
.endm

.macro	ldi_w	; r17,r16,$f3e4
	ldi	@0,high(@2)
	ldi	@1,low(@2)
.endm

.macro	subi_w	; r17,r16,$5476
	subi	@1,low(@2)
	sbci	@0,high(@2)
.endm

.macro	addi_w	; r17,r16,$5476
	subi	@1,low(-@2)
	sbci	@0,high(-@2)
.endm

.macro	outi	; out	TCCR0,0b011
	ldi	r16,@1
	out	@0,r16
.endm

.macro	loop	; r18,Label
	dec	@0
	brne	@1
.endm

.macro	IntStart
	push	r16
	in	r16,SREG
	push	r16
.endm

.macro	IntEnd
	pop	r16
	out	SREG,r16
	pop	r16
	reti
.endm

.macro	SetFlag
	set
	bld	FlagReg,@0
.endm

.macro	ClearFlag
	clt
	bld	FlagReg,@0
.endm

.macro	OnFlag
	sbrs	FlagReg,@0
	rjmp	@1
	clt
	bld	FlagReg,@0
.endm

.macro	PrintAt
	ldi	XL,Low(@0+IndBuf)
	ldi	r18,@1-1
	ldi	r19,$20
	rcall	B2Dec
.endm

.macro	Print
	ldi	r18,@0-1
	ldi	r19,$20
	rcall	B2Dec
.endm

.macro	PrintZAt
	ldi	XL,Low(@0+LCDBuf)
	ldi	r18,@1-1
	ldi	r19,$30
	rcall	B2Dec
.endm

.macro	PrintZ
	ldi	r18,@0-1
	ldi	r19,$30
	rcall	B2Dec
.endm


.macro	lsr16
	lsr	@0
	ror	@1
.endm

.macro	asr16
	asr	@0
	ror	@1
.endm

.macro	lsl16
	lsl	@1
	rol	@0
.endm

.macro	SetBit
	set
	bld	@0,@1
.endm

.macro	ClearBit
	clt
	bld	@0,@1
.endm

.macro	MulKoef16
	ldi	r19,high(@0)
	ldi	r18,low(@0)
	rcall	K16x16
.endm

.macro	FitByte ; @0-reg, @1-min, @2-max
	cpi	@0,@1
	brsh	fbm_1
	ldi	@0,@1
fbm_1:	cpi	@0,@2
	brlo	fbm_2
	ldi	@0,@2
fbm_2:
.endm


.macro	sub_w	;
	sub	@1,@3
	sbc	@0,@2
.endm

.macro	add_w	;
	add	@1,@3
	adc	@0,@2
.endm

.macro	neg_w
	com	@0
	com	@1
	subi	@1,$FF
	sbci	@0,$FF
.endm

.macro	cp_w	;
	cp	@1,@3
	cpc	@0,@2
.endm

.macro	cpi_w
	ldi	@2,high(@3)
	cpi	@1,low(@3)
	cpc	@0,@2
.endm
