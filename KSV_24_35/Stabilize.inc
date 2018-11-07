.equ	ClosedTime = 8
.equ	ClosedKeyPeriod = 70
;.equ	InitPWMPeriod = 228
.equ	PWMFreq	= 165 ; 0.1 kHz
.equ	PWMPeriod = 80000/PWMFreq
.equ	MinPWMSignal = 10
.equ	MinPWMDelay = 10 ; 30
.equ	PWMRange = PWMPeriod - MinPWMDelay - MinPWMSignal

.macro	InitStab
	ldi_w	r17,r16,64000
	sts	IzeroL,r16
	sts	IzeroH,r17
	std	Y+yIzadL,r16
	std	Y+yIzadH,r17
	stdi	yClosedKeyCnt,ClosedTime+1
	stdi	yManualPWMCnt,48
	set
	bld	AFlags,afChHigh
.endm

SetRealPWM:
	ldd	r16,Y+yRealPWML
	ldd	r17,Y+yRealPWMH
	ldd	r18,Y+yNeedPWML
	ldd	r19,Y+yNeedPWMH
	cp	r18,r16
	cpc	r19,r17
	breq	srp_1
	brlo	srp_2
	subi_w	r17,r16,-1
	rjmp	srp_3
srp_2:	subi_w	r17,r16,1
srp_3:	std	Y+yRealPWML,r16
	std	Y+yRealPWMH,r17
	cp	r16,ZeroReg
	cpc	r17,ZeroReg
	brne	srp_4
	out	TCCR1A,ZeroReg
	out	TCCR1B,ZeroReg
	sbi	PORTB,1
	ret
srp_4:	ldi	r18,High(PWMRange-1)
	cpi	r16,Low(PWMRange-1)
	cpc	r17,r18
	brlo	srp_5
	out	TCCR1A,ZeroReg
	out	TCCR1B,ZeroReg
	cbi	PORTB,1
	ret
srp_5:	subi_w	r17,r16,-MinPWMSignal
	out	OCR1AH,r17
	out	OCR1AL,r16
	ldi_w	r17,r16,PWMPeriod
	out	ICR1H,r17
	out	ICR1L,r16
	outi	TCCR1A,0b11000010 ; Запуск ШИМ
	outi	TCCR1B,0b00011001
srp_1:	ret


;	Ucyc = Uset(1 - (t-20)/715) = Uvs*65536/400*(1+30/715)*(1-(t+10)/715)
;	(1+30/715) -> 34143<<1
;	Uvs*65535/400*(1+30/715)=Uvs*170.7118 -> Uvs*128*(k43702)*2
;	(1-(t+10)/715) -> 65535-T*92 -> *733=*512=*221/8
CalcU:
	lds	r17,Ucyc
	ldi	r16,0
	lsr16	r17,r16
	subi_w	r17,r16,-(220*128)
	ldi_w	r19,r18,43702
	rcall	K16x16
	lsl16	r17,r16
	lds	r18,Tab
	FitByte	r18,55-10,55+40
	subi	r18,55-10
	ldi	r19,221
	mul	r18,r19
	add	r1,r18
	add	r1,r18
	lsr16	r1,r0
	lsr16	r1,r0
	lsr16	r1,r0
	ldi_w	r19,r18,65535
	sub	r18,r0
	sbc	r19,r1
	rcall	K16x16
	std	Y+yUstabL,r16
	std	Y+yUstabH,r17
	ret
;CalcU:	; Ucyc = 57.2 - 0.08*(t-20)
;	; Ures = Ucyc - 2.7 = 54.5 ....
;	lds	r16,Tab
;	FitByte	r16,55-10,55+40 ; Ограничить температуру -10..+40
;	ldi	r17,66
;	mul	r16,r17
;	ldi_w	r17,r16,51808
;	sub	r16,r0
;	sbc	r17,r1
;	std	Y+yUstabL,r16
;	std	Y+yUstabH,r17
;	ret

; Izero (64000) = 62.5A;  Ibase=C/5
; Ib=C/5/125*Izero = C/600*Iz = (C/10)*Iz/60
; 1/60 = 1092
CalcIBase:
	lds	r16,IzeroL
	lds	r17,IzeroH
	MulKoef16 1048	;1 A
	lds	r20,Izad
	mul	r16,r20
	movw	r19:r18,r1:r0
	mul	r17,r20
	add	r19,r0
	sts	IbaseL,r18
	sts	IbaseH,r19	; Базовый ток
	ret

.equ	MaxDI = 300
.equ	MinDI = -300

; Izero (64000) = 125 A
; Ток уменьшается до 35% от базового (С/5) в диапазоне Ucyc-15 ... Ucyc
CalcI:
; Вычисляем превышение напряжения над границей ограничения
	GetADC	adcUab
	ldd	r18,Y+yUstabL
	ldd	r19,Y+yUstabH
	subi_w	r19,r18,2458
	sub	r16,r18
	sbc	r17,r19
	brcc	ci_1
	ldi_w	r17,r16,0 ; 0 если не превышает
ci_1:
; Ограничиваем её на уровне 23В (чтобы не было переполнений)
	ldi	r18,High(3770)
	cpi	r16,Low(3770)
	cpc	r17,r18
	brlo	ci_2
	ldi_w	r17,r16,3770
ci_2:
; Вычисляем коэффициент уменьшения базового тока
	ldi	r20,17
	mul	r16,r20
	movw	r19:r18,r1:r0
	mul	r17,r20
	add	r19,r0
	com	r18	; Коэффициент от базового тока
	com	r19
	lds	r16,IbaseL
	lds	r17,IbaseH
	rcall	K16x16	; Заданный ток от напряжения
	movw	r23:r22,r17:r16

;---- Ограничение тока по температуре 17(35)->0(65) ////= 30 - (t-30)
;---- Ограничение тока по температуре 35A(35)->0(65) ////= 30 - (t-30)
	ldi_w	r17,r16,594*2 ; 594
	lds	r20,Trad
	subi	r20,55+35
	brcc	ci_3
	ldi	r20,0
ci_3:	cpi	r20,30
	brlo	ci_4
	ldi	r20,30
ci_4:	mul	r16,r20
	movw	r19:r18,r1:r0
	mul	r17,r20
	add	r19,r0
	ldi_w	r17,r16,36700 ; 17825
	sub	r16,r18
	sbc	r17,r19
 	lds	r18,IzeroL
	lds	r19,IzeroH
	rcall	K16x16
	cp	r22,r16
	cpc	r23,r17
	brlo	ci_5
	movw	r23:r22,r17:r16
ci_5:
	lds	r16,IzeroL
	lds	r17,IzeroH

	sub	r16,r22
	sbc	r17,r23 ; Izad new
;-- Плавное изменение базового тока
	ldd	r18,Y+yIzadL
	ldd	r19,Y+yIzadH
	sub	r16,r18
	sbc	r17,r19
	brcc	ci_6
	ldi	r20,High(MinDI)
	cpi	r16,Low(MinDI)
	cpc	r17,r20
	brge	ci_7
	ldi_w	r17,r16,MinDI
	rjmp	ci_7
ci_6:	ldi	r20,High(MaxDI)
	cpi	r16,Low(MaxDI)
	cpc	r17,r20
	brlt	ci_7
	ldi_w	r17,r16,MaxDI
ci_7:	add	r18,r16
	adc	r19,r17
	std	Y+yIzadL,r18
	std	Y+yIzadH,r19
	ret

SelectCharge:	; 3.8147 Гц
	ldd	r20,Y+yUcntL
	ldd	r21,Y+yUcntH
	GetADC	adcUab
	sbrs	AFlags,afChHigh
	rjmp	sch_8
; --- переход в резервный режим --
; при нахождении Uab выше Ucyc - 1.5 в течение 45 минут (реверсивный счетчик 1/4)
	clt
	ldi_w	r19,r18,36048
	std	Y+yLowCntL,r18
	std	Y+yLowCntH,r19
	ldd	r18,Y+yUstabL
	ldd	r19,Y+yUstabH
	subi_w	r19,r18,1229
	cp	r16,r18
	cpc	r17,r19
	brlo	sch_1
	subi_w	r21,r20,-1
	ldi	r19,High(10299)
	cpi	r20,Low(10299)
sch_7:	cpc	r21,r19
	brlo	sch_2
sch_5:	bld	AFlags,afChHigh
	rjmp	sch_3
sch_1:	ldd	r16,Y+yMainCntM
	andi	r16,0b1100
	brne	sch_2
	subi_w	r21,r20,1
	brcc	sch_2
sch_3:	ldi_w	r21,r20,0
sch_2:	std	Y+yUcntL,r20
	std	Y+yUcntH,r21
	ret
sch_8: ; --- переход в циклический режим --
; - при нахождении Uab ниже 48В в течение 30 минут (реверсивный счетчик)
; - через 7 дней
	set
	ldd	r18,Y+yMainCntM
	cpi	r18,0
	brne	sch_4
	ldd	r18,Y+yLowCntL
	ldd	r19,Y+yLowCntH
	subi_w	r19,r18,1
	brcs	sch_5
	std	Y+yLowCntL,r18
	std	Y+yLowCntH,r19
sch_4:	ldi	r18,High(39322)
	cpi	r16,low(39322)
	cpc	r17,r18
	brsh	sch_1
	subi_w	r21,r20,-1
	ldi	r19,High(6974)
	cpi	r20,Low(6974)
	rjmp	sch_7

