;-- Init ports state --
	InitPorts

;-- Стек и указатели --
	outi	SPH,high(RAMEND)
	outi	SPL,low(RAMEND)
	ldi_w	YH,YL,$60
	ldi_w	XH,XL,$100

;-- Очиститка памяти с $100 по $1ff
	ldi_w	ZH,ZL,$60
ini_1:	st	Z+,YH
	cpi	ZH,2
	brlo	ini_1

	ldi_w	YH,YL,YBUF
	ldi_w	XH,XL,XSEG

	clr	AFlags
	clr	BFlags

	rcall	InitReg

	rcall	InitADC

	rcall	InitTact

	rcall	InitUart

	InitDallas

	ldi_w	r17,r16,$8600 //8748
	ldi	XL,ADCKoef+2
	st	X+,r16
	st	X+,r17

	ldi_w	r17,r16, $7B0F //$7D53 //7E86
	ldi	XL,ADCKoef+4
	st	X+,r16
	st	X+,r17

	stdi	yGenPoles,10
//	stdi	yGenPoles,13  // 14-1

	stdi	yScrTimeCnt,45

	rcall	InitMem

	sei


