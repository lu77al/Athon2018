.include "m8def.inc"
.include "Macro.inc"
.include "Pinout.inc"
.include "Vars.inc"

rjmp	RESET		; Reset
reti			; INT0
reti			; INT1
rjmp	TDInt		; TIMER2 COMP
reti			; TIMER2 OVF
reti			; TIMER1 CAPT
reti			; TIMER1 COMPA
reti			; TIMER1 COMPB
reti			; TIMER1 OVF
reti			; TIMER0 OVF
reti			; SPI,STC
reti			; UART, RX
reti			; UART, UDRE
reti			; UART, TX
reti			; ADC
reti			; EE_RDY

.include "UART.inc"
.include "Subs.inc"
.include "ADC.inc"
.include "DS18B20.inc"
.include "Tact.inc"
.include "UserPanel.inc"
.include "Speed.inc"
.include "Regulate.inc"
.include "Stabilize.inc"
.include "Rele.inc"

RESET:

.include "Init.inc"

;	std	Y+yKWCntL,YH
;	std	Y+yKWCntH,YH
	ldi	r16,32 // A
	sts	Izad,r16
	ldi	r16,66  // 220+66 = 286 -> 28.6 -> 57.2
	sts	Ucyc,r16

Main:
	tskTact
RetTact:
	tskUart
RetUart:
	tskADC
RetADC:
	tskDallas
RetDallas:

	EnterStab

	rjmp	Main

;********** BootLoader V0.0 **********
.ORG	SECONDBOOTSTART
BootLoader:
