; Интегрально - дифференциальный регулятор
; с несколькими критериями регулирования
; критерий и аргумент - 16бит (полный диапазон)
;  коэффициент для рассогласования 256*k16 (1/256..256)
; X - буфер
; 

.macro	EnterStab
	sbrc	AFlags,afStartReg
	rcall	Regulate
.endm

InitReg:
	std	Y+yPWMPhaseL,ZeroReg
	std	Y+yPWMPhaseH,ZeroReg
	ldi_w	r17,r16,52*65535/80
	std	Y+yUstabL,r16
	std	Y+yUstabH,r17
;	ldi_w	r17,r16,80*65535/150
;	std	Y+yUmpptL,r16
;	std	Y+yUmpptH,r17
	ldi_w	r17,r16,64000-50*65535/128
	std	Y+yIzadL, r16
	std	Y+yIzadH, r17
	ldi	XL,RegDataBuf	; По два байта: Kint, Ybak, Kdif, Result
; Uout
	ldi_w	r17,r16,160	; 640 ; Kint
	st	X+,r17
	st	X+,r16
	ldi_w	r17,r16,65535*50/80	;65535*250/400 ; Ybak
	st	X+,r17
	st	X+,r16
	ldi_w	r17,r16,400	; 1600 ; Kdif
	st	X+,r17
	st	X+,r16
	ldi_w	r17,r16,$7FFF ; Result
	st	X+,r17
	st	X+,r16
; Ich
	ldi_w	r17,r16,13 ;50 ; Kint
	st	X+,r17
	st	X+,r16
	ldi_w	r17,r16,(65536/256)*250 ; Ybak
	st	X+,r17
	st	X+,r16
	ldi_w	r17,r16,4 ; 14 ; Kdif	
	st	X+,r17
	st	X+,r16
	ldi_w	r17,r16,$7FFF ; Result
	st	X+,r17
	st	X+,r16

	stsi	TestKeyCnt,3
	stsi	ClosedKey,180

	ret

; Вычисление рассогласования, умножение на коэфффициент, проверка диапазона
; r23:r22 - заданнное
; r25:r24 - текущее
; X - буфер параметра (коэффициент)
; r16 - направление регулирования (младший бит 0-прямое;1-обратное) 
GetReg:	ld	r21,X+
	ld	r20,X+	; r21,r22 - коэффициент
	clt
	sub_w	r23,r22,  r25,r24
	brcc	grg_1
	set
	neg_w	r23,r22
grg_1:	mul	r21,r23
	movw	r19:r18,r1:r0
	mul	r21,r22
	mov	r17,r0
	add	r18,r1
	adc	r19,ZeroReg
	mul	r23,r20
	add	r17,r0
	adc	r18,r1
	adc	r19,ZeroReg
	mul	r22,r20
	add	r17,r1
	adc	r18,ZeroReg
	adc	r19,ZeroReg
	cpi	r18,$30
	cpc	r19,ZeroReg
	brlo	grg_4
	ldi_w	r18,r17,$3000
grg_4:	mov	r19,r16
	brtc	grg_2
	com	r19
grg_2:	sbrs	r19,0
	rjmp	grg_3
	neg_w	r18,r17
grg_3:	ret


; Интегрально дифференциальный регулятор
; r23:r22 - заданнное
; r25:r24 - текущее
; X - буфер параметра
; r16 - направление регулирования (младший бит 0-прямое;1-обратное) 
RegPar:
	rcall	GetReg
	mov	r15,r18
	mov	r14,r17
	ld	r23,X+	; предыдущее
	ld	r22,X
	st	X,r24
	st	-X,r25	; сохр. текущее
	subi	XL,-2
	rcall	GetReg
	add	r17,r14
	adc	r18,r15
	st	X+,r18
	st	X+,r17
	ret



REGULATE:
	clt
	bld	AFlags,afStartReg

	GetADC	adcUab
	ldi_w	r19,r18,54067
	cp	r16,r18
	cpc	r17,r19
	brlo	reg_30
	stsi	LongClose,107
reg_30:
	lds	r16,ClosedKey
	subi	r16,1
	brcs	reg_16
	ldd	r17,Y+yRealPWML
	ldd	r18,Y+yRealPWMH
	or	r17,r18
	brne	reg_17
	sts	ClosedKey,r16
	tst	r16
	brne	reg_17
	GetADC	adcIch
	ldi	r18,High(58000)
	cpi	r16,Low(58000)
	cpc	r17,r18
	clt
	brsh	reg_19
	ldi_w	r17,r16,64000
	set
reg_19:	sts	IzeroL,r16
	sts	IzeroH,r17
	bld	AFlags,afKeyError
reg_17:	ldi_w	r17,r16,0
	rjmp	reg_6
reg_16:
	ldd	r16,Y+yManualPWMCnt
	subi	r16,1
	brcs	reg_18
	std	Y+yManualPWMCnt,r16
	rjmp	reg_15
reg_18:
	lds	r16,LongClose
	cpse	r16,ZeroReg
	rjmp	reg_21

;	lds	r16,LongClose
;	cpi	r16,0
;	brne	reg_21_
;	ldd	r16,Y+yButState
;	sbrc	r16,btSwitch
;reg_21_: rjmp	reg_21

;	lds	r16,UartRCNT
;	subi	r16,1
;	brcs	reg_21_
;	sts	UartRCNT,r16
;	GetADC	adcUin
;	cpi	r17,$40
;	brlo	reg_21_

	ldi	XL,RegDataBuf	; Старт буфера

; -- Вычисление регулирования по перечню критериев
; Uout
	GetADC_	r25,r24,adcUab
	ldd	r22,Y+yUstabL
	ldd	r23,Y+yUstabH
;	sbrc	AFlags,afChHigh
;	rjmp	reg_20
;	subi_w	r23,r22,2212
;reg_20:
	ldi	r16,0
	rcall	RegPar
	ldd	r22,Y+yUstabL
	ldd	r23,Y+yUstabH
	cpi_w	r23,r22, r16,$FFFF
	brne	reg_10
	subi	XL,2
	ldi_w	r18,r17,$7FFF
	st	X+,r18
	st	X+,r17
reg_10:	
; Iin
	GetADC_	r25,r24,adcIch
	ldd	r22,Y+yIzadL
	ldd	r23,Y+yIzadH
	ldi	r16,1
	rcall	RegPar
	ldd	r22,Y+yIzadL
	ldd	r23,Y+yIzadH
	cpi_w	r23,r22, r16,$FFFF
	brne	reg_11
	subi	XL,2
	ldi_w	r18,r17,$7FFF
	st	X+,r18
	st	X+,r17
reg_11	:

;-- Выбор наименьшего
	ldi	XL,RegDataBuf+$06
	ld	r19,X+
	ld	r18,X
	ldi	r20,2
	ldi	r21,0
reg_2:	subi	XL,-7
	ld	r17,X+
	ld	r16,X
	cp_w	r17,r16, r19,r18
	brge	reg_1
	movw	r19:r18,r17:r16
	mov	r20,r21
reg_1:	subi	r21,1
	brcc	reg_2

	cpi_w	r19,r18, r16,$7FFF
	breq	reg_15

;-- Ограничение скорости
	ldi_w	r17,r16,$0700
	cp_w	r19,r18, r17,r16
	brlt	reg_3
	movw	r19:r18,r17:r16
reg_3:	ldi_w	r17,r16,-$0700
	cp_w	r19,r18, r17,r16
	brge	reg_4
	movw	r19:r18,r17:r16
reg_4:
;-- Изменение управления
	ldd	r16,Y+yPWMPhaseL
	ldd	r17,Y+yPWMPhaseH
	add_w	r17,r16,  r19,r18
	brcc	reg_5
	sbrc	r19,7
	rjmp	reg_6
	ldi_w	r17,r16,$FFFF
	rjmp	reg_6
reg_5:	sbrs	r19,7
	rjmp	reg_6
reg_21:	ldi_w	r17,r16,$0000
reg_6:	std	Y+yPWMPhaseL,r16
	std	Y+yPWMPhaseH,r17
reg_15:
	rjmp	RRefreshPWM
reg_14:	ret

RRefreshPWM:	; "Применение" нового значения ШИМ
	ldd	r16,Y+yPWMPhaseL
	ldd	r17,Y+yPWMPhaseH
	ldi_w	r19,r18,PWMRange
	rcall	K16x16
	std	Y+yNeedPWML,r16
	std	Y+yNeedPWMH,r17
	ret



