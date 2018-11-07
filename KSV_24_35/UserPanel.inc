;--- Output bits ---
.equ	bitA = 1<<12
.equ	bitB = 1<<5
.equ	bitC = 1<<1
.equ	bitD = 1<<14
.equ	bitE = 1<<15
.equ	bitF = 1<<13
.equ	bitG = 1<<2
.equ	bitH = 1<<3
.equ	bit1 = 1<<11
.equ	bit2 = 1<<4
.equ	bit3 = 1<<6
.equ	bitS = 1<<8
;--- Some assembled chars ---
.equ	dgt0	= bitA + bitB + bitC + bitD + bitE + bitF
.equ	dgt1	= bitB + bitC
.equ	dgt2	= bitA + bitB + bitG + bitE + bitD
.equ	dgt3	= bitA + bitB + bitC + bitD + bitG
.equ	dgt4	= bitB + bitC + bitF + bitG
.equ	dgt5	= bitA + bitF + bitG + bitC + bitD
.equ	dgt6	= bitA + bitF + bitE + bitD + bitC + bitG
.equ	dgt7	= bitA + bitB + bitC
.equ	dgt8	= bitA + bitB + bitC + bitD + bitE + bitF + bitG
.equ	dgt9	= bitA + bitB + bitC + bitD + bitF + bitG
.equ	dgtA	= bitA + bitB + bitC + bitE + bitF + bitG
.equ	dgtB	= bitC + bitD + bitE + bitF + bitG
.equ	dgtC	= bitA + bitD + bitE + bitF
.equ	dgtD	= bitB + bitC + bitD + bitE + bitG
.equ	dgtE	= bitA + bitD + bitE + bitF + bitG
.equ	dgtF	= bitA + bitE + bitF + bitG
.equ	dgtG	= bitA + bitC + bitD + bitE + bitF
.equ	dgtH	= bitB + bitC + bitE + bitF + bitG
.equ	dgtHs	= bitC + bitE + bitF + bitG
.equ	dgtI	= bitE + bitF
.equ	dgtJ	= bitB + bitC + bitD + bitE
.equ	dgtL	= bitD + bitE + bitF
.equ	dgtT	= bitD + bitE + bitF + bitG
.equ	dgtU	= bitB + bitC + bitD + bitE + bitF
.equ	dgtRP	= bitA + bitB + bitC + bitE + bitF
.equ	dgtDG	= bitA + bitB + bitF + bitG
.equ	dgtDGs	= bitC + bitD + bitE + bitG
.equ	dgtRG	= bitA + bitE + bitF
.equ	dgtUL	= bitA
.equ	dgtML	= bitG
.equ	dgtDL	= bitD
.equ	dgtMC	= bitA + bitB + bitC + bitD
.equ	dgtSP	= 0
;--- Charset (digits for values/numbers, etc) ---
CharSet:
.dw	dgt0, dgt1, dgt2, dgt3, dgt4, dgt5, dgt6, dgt7,  dgt8, dgt9
.dw	dgtA, dgtB, dgtC, dgtD, dgtE, dgtF, dgtG, dgtH,  dgtHs,dgtI
.dw	dgtJ, dgtL, dgtT, dgtU, dgtRP,dgtDG,dgtDGs,dgtRG,dgtUL,dgtML
.dw	dgtDL,dgtMC,dgtSP
;--- Char codes ---
.equ ch0=0   .equ ch1=1   .equ ch2=2    .equ ch3=3   .equ ch4=4   .equ ch5=5
.equ ch6=6   .equ ch7=7   .equ ch8=8    .equ ch9=9   .equ chA=10  .equ chB=11
.equ chC=12  .equ chD=13  .equ chE=14   .equ chF=15  .equ chG=16  .equ chH=17
.equ chHs=18 .equ chI=19  .equ chJ=20   .equ chL=21  .equ chT=22  .equ chU=23
.equ chRP=24 .equ chDG=25 .equ chDGs=26 .equ chRG=27 .equ chUL=28 .equ chML=29
.equ chDL=30 .equ chMC=31 .equ chSP=32

;--- Buttons ---
.equ	btLeft	= 4
.equ	btRight	= 5
UserPanel:
	ldd	r17,Y+yButState	;  244 Гц
	ldd	r18,Y+yButPress
	ldd	r19,Y+yButRep
	ldi	XL,ButBuf
	ld	r20,X+	; ButCnt
	ld	r21,X+	; ButCntStart
	ld	r23,X	; BakButCnt
;	ld	r22,X	; BakButState
	in	r16,PINB
	ori	r16,$FF-(1<<btLeft)-(1<<btRight)
	com	r16	; Состояние ножек
;	st	X,r16   ; Фильтр дребезга
	cp	r16,r17
	brne	but_1
	ldi	r23,5
but_1:	subi	r23,1
	brcs	but_2
	st	X,r23
	mov	r16,r17
but_2:	

;	breq	but_1
;	ldi	r23,5
;but_1:	dec	XL
;	subi	r23,1
;	brcs	but_2
;	st	X,r23   ; BakButCnt
;	mov	r16,r17
;but_2:

	mov	r23,r16
	com	r23
	and	r23,r17	; Rel flags
	ldd	r14,Y+yButRel
	or	r14,r23
	std	Y+yButRel,r14
	com	r17
	and	r17,r16	; Press flags
;	andi	r23,0b1010
;	or	r23,r17	 ; Звук при отпускании (для тумблеров)
	breq	but_4
	ldi	r23,3
	std	Y+ySoundCnt,r23
	ldi	r21,172
	ldi	r20,150
but_4:	or	r18,r17
	or	r19,r17
	std	Y+yButPress,r18
	std	Y+yButState,r16
	andi	r16,(1<<btLeft)+(1<<btRight) ; Повтор формируется только для кнопок
	breq	but_3
	dec	r20
	brne	but_3
	or	r19,r16
	ldi	r20,25 ; 32
	lsr	r21
	add	r20,r21
but_3:	std	Y+yButRep,r19
	st	-X,r21	; ButCntStart
	st	-X,r20	; ButCnt
;--- Push data to USER PANEL ---
PushUP:	cbi	STORE
	cbi	SHIFT
	sbi	DATA_DIR
	sbi	SHIFT_DIR
	ldi	XL,PushUPBuf
	ld	r16,X
	andi	r16,0b11
	subi	r16,1
	brcc	pup_2
	ldi	r16,2
pup_2:	st	X+,r16
	lsl	r16
	add	XL,r16
	ld	r16,X+
	ld	r17,X
	sbrc	AFlags,afSound
	ori	r17,High(bitS)
	ldi	r18,16
pup_1:	cbi	SHIFT
	sbrs	r17,7
	cbi	DATA
	sbrc	r17,7
	sbi	DATA
	lsl16	r17,r16
	sbi	SHIFT
	loop	r18,pup_1
	sbi	STORE
	cbi	SHIFT
	cbi	DATA
	cbi	DATA_DIR
	cbi	SHIFT_DIR
	sbi	SHIFT
	sbi	DATA
	ret
;--- Form push bufer from indicate bufer with char codes ---
FormUPBuf:
	ldi	XL,IndBuf
	ldi_w	r19,r18,bit2+bit3
	ldi_w	r21,r20,bit1+bit3
	ldi_w	r23,r22,bit1+bit2
	ld	ZL,X+
	ld	r24,X+
	ld	r25,X+
	ldi	XL,PushUPBuf+1
fup_3:	bst	ZL,7
	lsl	ZL
	ldi	ZH,0
	subi_w	ZH,ZL,-CharSet*2
	lpm	r16,Z+
	lpm	r17,Z+
	brtc	fup_1
	ori	r16,bitH
fup_1:	add	r16,r18
	adc	r17,r19
	st	X+,r16
	st	X+,r17
	cpi	XL,PushUPBuf+1+6
	brsh	fup_2
	mov	ZL,r24
	mov	r24,r25
	movw	r19:r18,r21:r20
	movw	r21:r20,r23:r22
	rjmp	fup_3
fup_2:	ret
