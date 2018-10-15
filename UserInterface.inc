
// Interface states 
.equ	isVoltage	= 0	; 52.5	- Battery voltage with tenths
.equ	isCharge	= 1	; 08.8. - Charge current with tenths (last point to distinguish from voltage)
// Decimal point blinks if regulation is active
.equ	isSpeed		= 2	; 150   - Turbine rotation speed without points
.equ	isTab		= 3	; t12	- External (battery) temperature with prefix "t" 
.equ	isTrad		= 4	; r12   - Internal (radiator) temperature with prefix "r" (additional)
.equ	isUin		= 5	; 123.  - Input voltage integer part (last point to distinguish from battery voltage)
.equ	isSetVoltage	= 6	; 54.5  - Battery charge voltage with tenths (hole panel blinks) 
.equ	isSetTLow	= 7	; L12	- Temperature to turn reley on
.equ	isSetTHigh	= 8	; H15	- Temperature to turn reley off


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

	sbrs	AFlags,afWaitKeyReleased
	rjmp	pu_10
	andi	r23,(1<<btLeft)+(1<<btRight)
	brne	pu_11
	clt
	bld	AFlags,afWaitKeyReleased
pu_11:	rjmp	pu_ex
pu_10:

	cpi	r22,isSetVoltage
	brsh	pu_12

	sbrs	r24,btRight ; Правая ButPress
	rjmp	pu_1

;	sbrs	r23,btLeft ; с удерживанием левой
;	rjmp	pu_2
;	ldi	r21,240
;	ldi	r20,6
;	rjmp	pu_7
;pu_2:			; Правая без удерживания левой
	ldi	r21,60
pu_6:	ldi	r20,4
pu_7:	inc	r22
	cp	r22,r20
	brlo	pu_3
	ldi	r22,0
pu_3:	rjmp	pu_ex
pu_1:
	sbrs	r24,btLeft ; Левая ButPress
	rjmp	pu_4
	sbrs	r23,btRight ; с удерживанием правой
	rjmp	pu_5
	ldi	r21,240
	ldi	r20,6
	rjmp	pu_7
/*	sbis	RELEY_PIN
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
	rjmp	pu_ex */
pu_5:			; Левая без удерживания правой
	ldi	r21,0
	rjmp	pu_6
pu_4:		
	sbrs	r25,btLeft ; Левая ButRep
	rjmp	pu_13
	set
	bld	AFlags,afWaitKeyReleased
	ldi	r22,isSetVoltage
	ldi	r21,0
	rjmp	pu_ex
pu_13:	
	rjmp	pu_ex

pu_12:	
	sbrs	r25,btRight ; Change val
	rjmp	pu_15
	cpi	r22,isSetVoltage ; ChargeVoltage
	brne	pu_16
	ldd	r16,Y+yChargeVoltage
	inc	r16
	cpi	r16,151
	brlo	pu_17
	ldi	r16,0
pu_17:	std	Y+yChargeVoltage,r16
	rjmp	pu_ex
pu_16:
	cpi	r22,isSetTLow ; T turn heat on
	brne	pu_18
	ldd	r16,Y+yTHeatOn
	inc	r16
	ldd	r17,Y+yTHeatOff
	subi	r17,2
	cp	r17,r16
	brsh	pu_19
	ldi	r16,0
pu_19:	std	Y+yTHeatOn,r16
	rjmp	pu_ex
pu_18:
	cpi	r22,isSetTHigh ; T turn heat off
	brne	pu_20
	ldd	r16,Y+yTHeatOff
	inc	r16
	cpi	r16,36
	brlo	pu_21
	ldd	r16,Y+yTHeatOn
	subi	r16,-2
pu_21:	std	Y+yTHeatOff,r16
pu_20:	rjmp	pu_ex
pu_15:
	sbrs	r24,btLeft
	rjmp	pu_22
	inc	r22
	cpi	r22,isSetTHigh+1
	brlo	pu_23
	ldi	r22,isSetVoltage
pu_23:	rjmp	pu_ex
pu_22:
	sbrs	r25,btLeft
	rjmp	pu_14
	rcall	SaveMem
	ldi	r21,60
	set
	bld	AFlags,afWaitKeyReleased
	ldi	r22,isVoltage
	rjmp	pu_ex
pu_14:


pu_ex:
	std	Y+yCurScr,r22
	std	Y+yScrTimeCnt,r21
	std	Y+yButPress,ZeroReg
	std	Y+yButRep,ZeroReg
	ret


Print3at0:
	ldi	XL,IndBuf+0
; Int2Str (3 or 2 digits)
Print3:	ldi	r18,$FF
pr_1:	inc	r18
	subi_w	r17,r16,100
	brcc	pr_1
	subi	r16,-100
	st	X+,r18
Print2:	ldi	r17,$FF
pr_2:	inc	r17
	subi	r16,10
	brcc	pr_2	
	st	X+,r17
	subi	r16,-10
	st	X,r16
	ret


SetDecPointAt0:
	ldi	XL,IndBuf+0
	rjmp	SetDecPoint
SetDecPointAt2:
	ldi	XL,IndBuf+2
	rjmp	SetDecPoint
SetDecPointAt1:
	ldi	XL,IndBuf+1
SetDecPoint:
	ld	r16,X
	ori	r16,$80
	st	X,r16
	ret

Indicate:
; --- Prepare blinking counter
	ldd	r25,Y+yMainCntM
	ldd	r16,Y+yPWMPhaseL
	ldd	r17,Y+yPWMPhaseH
	cpi	r16,$FF
	cpc	r16,r17
	brne	ind_5
	ldi	r25,$FF		; 15.25 Hz counter for blinking ($FF if no regulation)
ind_5:
	ldd	r24,Y+yCurScr
	cpi	r24,10
	brlo	ind_1
	ret
; --- Scrolling screens
ind_1:	ldd	r23,Y+yScrTimeCnt
	subi	r23,1
	brcs	ind_2	; if yScrTimeCnt = 0 -> not scrolling
	brne	ind_3
	ldi	r23,60  ;  Scroll event occured -> reset yScrTimeCnt to 4sec
	inc	r24
	cpi	r24,4	;  scroll screens 0,1,2,3,0,1,...
	brlo	ind_4
	ldi	r24,0
ind_4:	std	Y+yCurScr,r24
ind_3:	std	Y+yScrTimeCnt,r23
ind_2:
;--- Battery voltage with tenths
	cpi	r24,isVoltage
	brne	ind0_1
	lds	r16,UabL
	lds	r17,UabH
	rcall	Print3at0
ind0_3:	sbrc	r25,1
	rcall	SetDecPointAt1
	rjmp	FormUPBuf
ind0_1:
; --- Charge current with tenths (last point to distinguish from voltage)
	cpi	r24,isCharge
	brne	ind1_1
	lds	r16,IchL
	lds	r17,IchH
	rcall	Print3at0
	rcall	SetDecPoint
	rjmp	ind0_3
ind1_1:
; --- Turbine rotation speed without points
	cpi	r24,isSpeed
	brne	ind2_1
	lds	r16,TurNL
	lds	r17,TurNH
	rcall	Print3at0
	rjmp	FormUPBuf
ind2_1:
	ldd	r25,Y+yMainCntM ; Counter for blinking regardless regulation
; --- External (battery) temperature with prefix "t" 
	cpi	r24,isTab
	brne	ind3_1
	lds	r16,Tab
	lds	r23,TabErr
	ldi	r22,chT
ind3_4:	subi	r16,55
	brcc	ind3_2
	ldi	r22,chML
	neg	r16
ind3_2:	ldi	XL,IndBuf
	sbrs	r25,1
	rjmp	ind3_3
	cpi	r23,0
	brne	ind3_3
	ldi	r22,chSP
ind3_3:	st	X+,r22
	rcall	Print2
	rjmp	FormUPBuf
ind3_1:
; --- Internal (radiator) temperature with prefix "r" (additional)
	cpi	r24,isTrad
	brne	ind4_1
	lds	r16,Trad
	lds	r17,TradErr
	ldi	r22,chRs
	rjmp	ind3_4
ind4_1:
; --- Input voltage integer part (last point to distinguish from battery voltage)
	cpi	r24,isUin
	brne	ind5_1
	lds	r16,UinL
	lds	r17,UinH
	rcall	Print3at0
	rcall	SetDecPoint
	rjmp	FormUPBuf
ind5_1:
; --- Param items (hole panel blinks)
	sbrs	r25,1
	rjmp	ind6_2
	ldi	XL,IndBuf
	ldi	r16,chSP
	st	X+,r16
	st	X+,r16
	st	X+,r16
	rjmp	FormUPBuf
ind6_2:
; --- Battery charge voltage with tenths
	cpi	r24,isSetVoltage
	brne	ind6_1
	ldi_w	r17,r16,450
	ldd	r18,Y+yChargeVoltage
	add	r16,r18
	adc	r17,ZeroReg
	rcall	Print3at0
	rcall	SetDecPointAt1
	rjmp	FormUPBuf
ind6_1:
; --- L12 Temperature to turn reley on
	cpi	r24,isSetTLow
	brne	ind7_1
	ldd	r16,Y+yTHeatOn
	ldi	r17,chL
ind7_2:	ldi	XL,IndBuf
	st	X+,r17
	rcall	Print2
	rjmp	FormUPBuf
ind7_1:
; --- H15 Temperature to turn reley off
	cpi	r24,isSetTHigh
	brne	ind8_1
	ldd	r16,Y+yTHeatOff
	ldi	r17,chH
	rjmp	ind7_2
ind8_1:
	ldi	XL,IndBuf
	ldi	r16,chML
	st	X+,r16
	st	X+,r16
	st	X+,r16
	rjmp	FormUPBuf

/*
Индикация и настройка.

Отображаемые параметры:

Напряжение АБ 55.4 (3 цифры с десятичной точкой)
Ток заряда 05.2 (3 цифры с десятичной точкой + точка в конце для отличия от напряжения)
Скорость вращения 187 (3 цифры)
Окружающая температура t25 (2 цифры с префиксом "t" или "-" если температура отрицательная)

Если в данный момент идёт ограничение зарядного тока, десятичная точка в напряжении или зарядном токе мигает.

Нажатие любой кнопки - переход к следующему параметру.
Если была нажата правая, параметры циклически меняются с периодом 4 секунды.
Если была нажата правая, непрерывно отображается выбранный параметр.

Для входа в режим настройки удержать левую кнопку.
В режиме настройки индикатор мигает полностью.
Настраиваются
 напряжение заряда АБ (3 цифры с точкой)
 нижний порог температуры (включение реле) (две цифры с префиксом L)
 верхний порог температуры (выключение реле) (две цифры с префиксом H)
Правой кнопкой (возможно удержание) изменяется значение параметра
Левая кнопка - переход между параметрами
Удержание левой - выход из режима настройки и сохранение их в память

*/

