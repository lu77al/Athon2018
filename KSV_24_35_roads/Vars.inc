.def	SSREG	= r2

.def	AFlags	= r3
	.equ	afADCCompl  = 0
	.equ	afStartReg  = 1
	.equ	afSound	    = 2
	.equ	afOutInTemp = 3
	.equ	afKeyError  = 4
	.equ	afChHigh    = 5
	.equ	afWaitKeyReleased = 6

.def	BFlags	= r4

.def	W1Control = r5
	.equ	w1WriteByte = 7
	.equ	w1ReadByte  = 6

.def	W1Byte = r6


#define	ZeroReg	YH

.equ	XSEG = $100
; --- Массивы ($100..$1FF)
.equ	RXBuf		= $00
.equ	TXBuf		= $20
.equ	ADCBuf		= $40
.equ	ADCKoef		= $48
.equ	PushUPBuf	= $50
.equ	IndBuf		= $57
.equ	ButBuf		= $5A
.equ	DSResBuf	= $60
	.equ	Tab	= XSEG+DSResBuf+0
	.equ	TabErr	= XSEG+DSResBuf+1
	.equ	Trad	= XSEG+DSResBuf+2
	.equ	TradErr	= XSEG+DSResBuf+3
.equ	DSReadBuf	= $64
.equ	RegDataBuf	= $69
.equ	PHTimeBuf	= $80


.equ	YBUF = $60
; --- Переменные ($60..$9F)
.equ	yMainCntL	= $00
.equ	yMainCntM	= $01
.equ	yMainCntH	= $02
.equ	yRXPointer	= $03
.equ	yTXPointer	= $04
.equ	yTXEnd		= $05
.equ	yRXTimer	= $06
.equ	yRXTXCHS	= $07
.equ	yMyAddr		= $08
.equ	yADCSumL	= $09
.equ	yADCSumH	= $0A
.equ	yADCSamCnt	= $0B
.equ	yADCCurCh	= $0C
.equ	yButState	= $0D
.equ	yButPress	= $0E
.equ	yButRep		= $0F
.equ	yButRel		= $10
.equ	ySoundCnt	= $11
.equ	yW1PosL		= $12
.equ	yW1PosH		= $13
.equ	yW1Delay	= $14
.equ	yW1Buf		= $15

.equ	yPhaseRepCnt	= $18
.equ	yPhaseTime	= $19
.equ	yGenPoles	= $1A
.equ	yPhasePos	= $1B
.equ	yUstabL		= $1C
.equ	yUstabH		= $1D
.equ	yIzadL		= $1E
.equ	yIzadH		= $1F
.equ	yPWMPhaseL	= $20
.equ	yPWMPhaseH	= $21
.equ	yRealPWML	= $22
.equ	yRealPWMH	= $23
.equ	yNeedPWML	= $24
.equ	yNeedPWMH	= $25
.equ	yManualPWMCnt	= $26
.equ	yUcntL		= $27
.equ	yUcntH		= $28
.equ	yLowCntL	= $29
.equ	yLowCntH	= $2A
.equ	yClosedTime	= $2B
.equ	yRegCmdCnt	= $2C
.equ	yCurScr		= $2D
.equ	yScrTimeCnt	= $2E
.equ	yReleyCnt	= $2F
.equ	yNoReleyCnt	= $30

.equ	yChargeVoltage	= $31	; 0..150 -> 45.0..60.0V
.equ	yTHeatOn	= $32	; 0..15 
.equ	yTHeatOff	= $33	; 2..10


;.equ	yTestKeyCnt	= $23



;.equ	yManualPWMCnt	= $22



;.equ	yUab		= $20
;.equ	yUin		= $21
;.equ	yUH		= $22
;.equ	yUL		= $23
;.equ	yStage		= $24
;.equ	yUmax		= $25
;.equ	yMinUH		= $26
;.equ	yONDelay	= $27
;.equ	yOFFDelay	= $28
;.equ	yForceOffCnt	= $29
;.equ	yDecUHTime	= $2A
;.equ	yDecUHCnt	= $2B
;.equ	yRestartCntL	= $2C
;.equ	yRestartCntH	= $2D
;.equ	yRestartTimeL	= $2E
;.equ	yRestartTimeH	= $2F
;.equ	yBPoint1	= $30
;.equ	yBPoint2	= $31
;.equ	yDebIndCnt	= $3F


;*** Переменные прямой индексации ($A0..$FF) ***
.equ	RPeriodL	= $A0
.equ	RPeriodH	= $A1
.equ	ClosedKey	= $A4
.equ	TestKeyCnt	= $A5
.equ	LongClose	= $A6
.equ	IzeroL		= $A7
.equ	IzeroH		= $A8
.equ	Ucyc		= $A9
.equ	Izad		= $AA
.equ	IbaseL		= $AB
.equ	IbaseH		= $AC
.equ	ManUCnt		= $AD
.equ	ManICnt		= $AE
.equ	UabL		= $AF
.equ	UabH		= $B0
.equ	UabChCnt	= $B1
.equ	IchL		= $B2
.equ	IchH		= $B3
.equ	IchChCnt	= $B4
.equ	UinL		= $B5
.equ	UinH		= $B6
.equ	UinChCnt	= $B7
.equ	TurNL		= $B8
.equ	TurNH		= $B9
.equ	TurNChCnt	= $BA





