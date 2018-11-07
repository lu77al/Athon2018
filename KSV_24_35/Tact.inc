InitTact:
	outi	TCCR0,0b00000010 ; CK/8
	ret

.macro	tskTact		;----- Такт 3906.25 Hz -----
	in	r16,TIFR
	sbrc	r16,TOV0
	rjmp	Tact
.endm

Tact:	outi	TIFR,1<<TOV0

;**** RXTimer ****
	ldd	r16,Y+yRXTimer
	inc	r16
	breq	rxtm_1
	std	Y+yRXTimer,r16
	cpi	r16,UARTByteDelay
	brne	rxtm_1
	stdi	yRXPointer,RXBuf-1	
rxtm_1:
;**** RXTimer ****

	rcall	CheckPhase	

;-- MainCnt 24bit
	ldd	r23,Y+yMainCntL
	ldd	r24,Y+yMainCntM
	ldd	r25,Y+yMainCntH
	subi_w	r24,r23,-1
	sbci	r25,$FF
	std	Y+yMainCntL,r23
	std	Y+yMainCntM,r24
	std	Y+yMainCntH,r25

;--- Br0 ---
	cpi	r23,0b00011111 ; 15 Гц
	brne	skBr0

	rcall	Pult

	rcall	Indicate

	rjmp	RetTact
skBr0:


;--- Br1 ---
	cpi	r23,0b00111111; 15 Гц
	brne	sk_BR1

	rcall	CalcSpeed

	DS_Timers

	ldd	r24,Y+yMainCntM
	andi	r24,0b1111
	brne	b1_1
	rcall	Rele
b1_1:
	rjmp	RetTact
sk_BR1:

;--- Br2 ---
	cpi	r23,0b01011111	; 15 Гц
	brne	sk_BR2

	ldd	r24,Y+yMainCntM
	andi	r24,0b11
	brne	b2_2
	rcall	SelectCharge
b2_2:
;	ldd	r24,Y+yMainCntM
;	andi	r24,0b11
;	cpi	r24,0b10
;	brne	br2_4
;	rcall	CalcPower
;br2_4:

	ldd	r16,Y+yClosedTime
	ldd	r17,Y+yRealPWML
	ldd	r18,Y+yRealPWMH
	or	r17,r18
	breq	br2_20
	ldi	r16,8
br2_20:	subi	r16,1
	brcc	br2_21
	GetADC	adcIch
	lds	r18,IzeroL
	lds	r19,IzeroH
	dec	r19
	cp	r18,r16
	cpc	r19,r17
	brlo	br2_22
	set
	bld	AFlags,afKeyError
br2_22:	ldi	r16,0
br2_21:	std	Y+yClosedTime,r16


	rjmp	RetTact
sk_BR2:

;--- Br3 ---
	cpi	r23,0b01111111	; 15 Гц
	brne	sk_BR3

	ldd	r16,Y+yRegCmdCnt
	cpi	r16,0
	brne	br3_5

	lds	r16,ManUCnt
	subi	r16,1
	brcc	br3_1
	rcall	CalcU
	ldi	r16,0
br3_1:	sts	ManUCnt,r16

	rcall	CalcIbase

	lds	r16,ManICnt
	subi	r16,1
	brcc	br3_2
	rcall	CalcI
	ldi	r16,0
br3_2:	sts	ManICnt,r16
br3_5:

	rjmp	RetTact
sk_BR3:

;--- Br4 ---
	cpi	r23,0b10011111	; 15 Гц
	breq	st_BR4
	rjmp	sk_BR4
st_BR4:
	cpi	r24,0
	brne	br4_3
	lds	r16,TestKeyCnt
	subi	r16,1
	brcc	br4_4
	stsi	ClosedKey,40
	ldi	r16,54
br4_4:	sts	TestKeyCnt,r16
br4_3:

	ldd	r24,Y+yMainCntM
	cpi	r24,0
	brne	br4_5
	lds	r16,LongClose
	subi	r16,1
	brcs	br4_5
	sts	LongClose,r16
br4_5:
	GetADC	adcUab
	MulKoef16 800
	ldi_w	ZH,ZL,UabL
	rcall	SaveIndVal	

	GetADC	adcUin
	MulKoef16 1500
	ldi_w	ZH,ZL,UinL
	rcall	SaveIndVal	

	GetADC	adcIch
	lds	r18,IzeroL
	lds	r19,IzeroH
	sub	r18,r16
	sbc	r19,r17
	brcc	id_1
	ldi_w	r19,r18,0
id_1:	ldi_w	r17,r16,640
	rcall	K16x16
	cpi	r16,2
	cpc	r17,ZeroReg
	brsh	id_2
	ldi	r16,0
id_2:	ldi_w	ZH,ZL,IchL
	rcall	SaveIndVal	



	rjmp	RetTact
sk_BR4:


;---------  ------- 61 Гц
	andi	r23,0b00111111
	cpi	r23,0b00000111
	brne	skBR6

	clt
	bld	AFlags,afADCCompl

	rjmp	RetTact
skBR6:

;---------  ------- 122 Гц
	andi	r23,0b00011111 
	cpi	r23,0b00000011
	brne	skBR7

	clt
	ldd	r16,Y+ySoundCnt
	subi	r16,1
	brcs	snd_1
	std	Y+ySoundCnt,r16
	set
snd_1:	bld	AFlags,afSound

	rjmp	RetTact
skBR7:

;--------- Обработка UserPanel ------- 244 Гц
	andi	r23,0b00001111 
	cpi	r23,0b00000001
	brne	skUP

	rcall	UserPanel

	rcall	SetRealPWM

	rjmp	RetTact
skUP:

	rjmp	RetTact

SaveIndVal:
	ld	r18,Z+
	ld	r19,Z+
	ld	r20,Z
	cp	r16,r18
	cpc	r17,r19
	brne	sind_1
	ldi	r20,9
sind_1:	subi	r20,1
	st	Z,r20
	brcc	sind_2
	ldi	r20,8
	st	Z,r20
	st	-Z,r17
	st	-Z,r16
sind_2:	ret

Pult:
; Правая - перебор осовных с дальнейшим автолистанием
; Левая  - перебор основгых с фиксацией параметра
; Правая (при левой)  - перебор всех с дальнейшим автолистанием (но более длительным показом)
; Левая  (при правой) - тест ключа (закрыть / открыть надолго) 
; Можно еще длительными удержаниями входить в меню

	ldd	r25,Y+yButRep
	ldd	r24,Y+yButPress
	ldd	r23,Y+yButState
	ldd	r22,Y+yCurScr
	ldd	r21,Y+yScrTimeCnt


	sbrs	r24,btRight ; Правая
	rjmp	pu_1
	sbrs	r23,btLeft ; с удерживанием левой
	rjmp	pu_2
	ldi	r21,240
	ldi	r20,6
	rjmp	pu_7
pu_2:			; Правая без удерживания левой
	ldi	r21,60
pu_6:	ldi	r20,4
pu_7:	inc	r22
	cp	r22,r20
	brlo	pu_3
	ldi	r22,0
pu_3:	rjmp	pu_ex
pu_1:
	sbrs	r24,btLeft ; Левая
	rjmp	pu_4
	sbrs	r23,btRight ; с удерживанием правой
	rjmp	pu_5

	sbis	RELEY_PIN
	rjmp	pu_8
	cbi	RELEY_PIN
	rjmp	pu_9
pu_8:	sbi	RELEY_PIN
pu_9:	stdi	yNoReleyCnt,86

	ldi	r17,5
	lds	r16,LongClose
	cpse	r16,ZeroReg
	ldi	r17,0
	sts	LongClose,r17
	rjmp	pu_ex
pu_5:				; Левая без удерживания правой
	ldi	r21,0
	rjmp	pu_6
pu_4:		


pu_ex:
	std	Y+yCurScr,r22
	std	Y+yScrTimeCnt,r21
	std	Y+yButPress,ZeroReg
	std	Y+yButRep,ZeroReg
	ret


Indicate:
	ldd	r25,Y+yMainCntM
	ldd	r16,Y+yPWMPhaseL
	ldd	r17,Y+yPWMPhaseH
	cpi	r16,$FF
	cpc	r16,r17
	brne	ind_5
	ldi	r25,$FF
ind_5:
	ldd	r24,Y+yCurScr
	cpi	r24,10
	brlo	ind_1
	ret
ind_1:	ldd	r23,Y+yScrTimeCnt
	subi	r23,1
	brcs	ind_2
	brne	ind_3
	ldi	r23,60
	inc	r24
	cpi	r24,4
	brlo	ind_4
	ldi	r24,0
ind_4:	std	Y+yCurScr,r24
ind_3:	std	Y+yScrTimeCnt,r23
ind_2:
	cpi	r24,0 ; Uab
	brne	ind0_1
	lds	r16,UabL
	lds	r17,UabH
	lsr16	r17,r16
	PrintAt	0,3
ind0_3:	sbrs	r25,1
	rjmp	ind0_2
	ldi	XL,IndBuf+1
	ld	r16,X
	ori	r16,$80
	st	X,r16
ind0_2:	rjmp	FormUPBuf
ind0_1:
	cpi	r24,1 ; Ich
	brne	ind1_1
	lds	r16,IchL
	lds	r17,IchH
	PrintAt	0,3
	ldi	XL,IndBuf+2
ind1_2:	ld	r16,X
	ori	r16,$80
	st	X,r16
	rjmp	ind0_3
ind1_1:
	cpi	r24,2 ; Ntur
	brne	ind2_1
	lds	r16,TurNL
	lds	r17,TurNH
	PrintAt	0,3
	rjmp	FormUPBuf
ind2_1:
	cpi	r24,3 ; Tab
	brne	ind3_1
	lds	r16,Tab
	lds	r17,TabErr
	ldi	r18,0
ind3_5:	ldi	r19,0
	cpi	r17,0
	brne	ind3_3
	ldi	r19,$80
ind3_3:	ldi	r20,chT
	subi	r16,55
	brcc	ind3_2
	ldi	r20,chML
	neg	r16
ind3_2:	ldi	XL,IndBuf
	st	X+,r20
	rcall	Print2
ind3_4:	ldi	XL,IndBuf
	ld	r16,X
	or	r16,r18
	st	X,r16
	ldi	XL,IndBuf+2
	ld	r16,X
	or	r16,r19
	st	X,r16
	rjmp	FormUPBuf
ind3_1:
	cpi	r24,4 ; Trad
	brne	ind4_1
	lds	r16,Trad
	lds	r17,TradErr
	ldi	r18,$80
	rjmp	ind3_5
ind4_1:
	cpi	r24,5 ; Uin
	brne	ind5_1
	lds	r16,UinL
	lds	r17,UinH
	lsr16	r17,r16
	PrintAt	0,3
	ldi	XL,IndBuf
	rjmp	ind1_2
ind5_1:
	ret

