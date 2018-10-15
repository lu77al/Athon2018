; Обработка термодатчиков DS18B20 (кварц 8МГц)
; трехпроводноек включения, ножки данных датчиков разделены
; Timer2

; Signal timing
.equ	DST60  = 60
.equ	DST12  = 12
.equ	DST68  = 68
.equ	DST200 = 200
.equ	DST40  = 40

.equ	MinimalT = 55 -30
.equ	MaximalT = 55 +110
.equ	DefaultT = 55 +20
.equ	TErrCntLen  = 15
.equ	TErrRestart = 6

.macro	PULL_DS
	sbrc	AFlags,afOutInTemp
	sbi	TOUT_DIR
	sbrs	AFlags,afOutInTemp
	sbi	TIN_DIR
.endm

.macro	REL_DS
	sbrc	AFlags,afOutInTemp
	cbi	TOUT_DIR
	sbrs	AFlags,afOutInTemp
	cbi	TIN_DIR
.endm

.macro	READPIN_DS
	clt
	sbrs	AFlags,afOutInTemp
	rjmp	rds_1
	sbic	TOUT_PIN
	set
	rjmp	rds_2
rds_1:	sbic	TIN_PIN
	set
rds_2:
.endm

.macro	InitDallas
	cbi	TOUT_DIR
	cbi	TOUT_PRT
	cbi	TIN_DIR
	cbi	TIN_PRT
	outi	TCCR2,0b00000000	; Stop timer
	in	r16,TIMSK
	ori	r16,0b10000000
	out	TIMSK,r16		; Comp2  enable
	stdi	yW1PosL,Low(W1Init*2)
	stdi	yW1PosH,High(W1Init*2)
	stdi	yW1Delay,15		; 1 с перед инициализацией
	clr	w1Control
	clr	w1Byte
	ldi	XL,DSResBuf
	ldi	r16,75
	st	X+,r16
	st	X+,YH
	st	X+,r16
	st	X+,YH
.endm

.macro	tskDallas
	ldd	r16,Y+yW1Delay
	or	r16,W1Control
	brne	RetDallas
	rjmp	Dallas	; Войти если не пауза и завершена транзакция
.endm

.macro	DS_Timers
	tst	W1Control
	brne	dsd_1
	ldd	r16,Y+yW1Delay
	subi	r16,1
	brcs	dsd_1
	std	Y+yW1Delay,r16
	rjmp	dsd_2
dsd_1:	outi	TCCR2,0b00000010	; CK/8
dsd_2:
.endm

TDInt:	in	SSREG,SREG
	push	r16
	push	r17
	ldi	r17,254
	tst	W1Control
	brne	w1_0		; Переход к обработке текущей транзакции
	lds	r16,$60+yW1Delay
	cpi	r16,0
	breq	w1_15
	outi	TCCR2,0b00000000	; Stop timer
w1_15:	rjmp	sk_w1
w1_0:	mov	r16,W1Control
	sbrs	W1Control,w1WriteByte	; Запись байта
	rjmp	w1_1
	andi	r16,0b00110000
	brne	w1_3
	PULL_DS			; Старт записи бита
	ldi	r16,0b00010000
	add	W1Control,r16
	sbrs	w1Byte,0
	rjmp	w1_4
	ldi	r17,DST60	; Запись "1"
	ldi	r16,6
w1p1:	loop	r16,w1p1
	REL_DS
	rjmp	w1_5
w1_4:	ldi	r17,DST60	; Запись "0"
	rjmp	sk_w1
w1_3:	REL_DS			; Конец записи "0"
	ldi	r17,DST12	; Время восстановления 12 мкс
w1_5:	lsr	W1Byte			; Конец записи бита
	mov	r16,W1Control
	andi	r16,0b00001111
	subi	r16,1
	breq	w1_61
w1_6:	ori	r16,1<<w1WriteByte
w1_61:	mov	W1Control,r16
	rjmp	sk_w1
w1_1:	sbrs	W1Control,w1ReadByte	; Чтение байта
	rjmp	w1_2
	andi	r16,0b00110000
	brne	w1_7
	PULL_DS			; Старт чтения бита
	ldi	r16,0b00010000
	add	W1Control,r16
	ldi	r17,DST12	; Задержка перед чтением 12 мкс
	ldi	r16,8
w1p2:	loop	r16,w1p2
	REL_DS
	rjmp	sk_w1
w1_7:	ldi	r17,DST68 ; Чтение линии Слот 12+68 = 80 мкс
	ldi	r16,0b01111111
	and	W1Byte,r16
	READPIN_DS
	bld	W1Byte,7
	mov	r16,W1Control
	andi	r16,0b00001111
	subi	r16,1
	breq	w1_9
	lsr	W1Byte
	ori	r16,1<<w1ReadByte
w1_9:	mov	W1Control,r16
	rjmp	sk_w1
w1_2:	dec	W1Control		; Сброс линии
	cpi	r16,4
	brlo	w1_10
	PULL_DS			; Начало импульса сброса
	ldi	r16,$ff
	mov	w1Byte,r16
w1_14:	ldi	r17,DST200	; Импульс 200*3=600 мкс
	rjmp	sk_w1
w1_10:	cpi	r16,3
	brne	w1_11
	REL_DS			; Конец импульса сброса
w1_13:	ldi	r17,DST40	; Проверка через 40 и 80 мкс
	rjmp	sk_w1
w1_11:	cpi	r16,2
	breq	w1_13
	rjmp	w1_14
sk_w1:	in	r16,TCNT2
	add	r16,r17
	out	OCR2,r16
	pop	r17
	pop	r16
	out	SREG,SSREG
	reti

Dallas:	ldd	XL,Y+yW1Buf
	tst	XL
	breq	w1p_1		; Если получен очередной байт
	st	X,W1Byte  	; сохранить его в буфер
	std	Y+yW1Buf,ZeroReg
w1p_1:	ldd	ZL,Y+yW1PosL
	ldd	ZH,Y+yW1PosH
	lpm	r16,Z+		; прочитать очередную команду из таблицы
	cpi	r16,0
	breq	w1p_2		; если команды закончились перейти к обработке буфера
	lpm	r17,Z+
	mov	w1Byte,r17
	mov	w1Control,r16	; Запустить выполнение команды
	cpi	r16,$48
	brne	w1p_4		; Если предусмотренл чтение байта
	std	Y+yW1Buf,r17	;  сохранить указатель на буфер
	rjmp	w1p_3
w1p_4:	cpi	r17,$44
	brne	w1p_3		; Если предусмотрена оцифровка
	ldi	r16,8		;  установить задержку на оцифровку
w1p_5:	std	Y+yW1Delay,r16
w1p_3:	std	Y+yW1PosL,ZL
	std	Y+yW1PosH,ZH
sk_w1p:	rjmp	RetDallas	;ret
w1p_2:	ldi	XL,DSReadBuf	; Обработка буфера
	ld	r16,X+	; TL
	ld	r17,X+	; TH
	ld	r20,X+	; User $AA
	ld	r18,X+	; TL
	ld	r19,X	; TH
	ldi	XL,DSResBuf
	sbrs	AFlags,afOutInTemp
	ldi	XL,DSResBuf+2 ; Указатель на буфер результата
	ld	r21,X+	; предидущее значение T 
	ld	r22,X+	; счетчик ошибок
	cpi	r20,$AA
	brne	w1p_10
	cp	r16,r18
	cpc	r17,r19
	breq	w1p_7
w1p_10:	subi	r22,1	; Если была ошибка уменьшить счетчик
	brcc	w1p_15	;  если счетчик закончился
	ldi	r22,0
	ldi	r21,DefaultT ; принять температуру "по умолчанию"
	rjmp	w1p_15
w1p_7:	swap	r16	; Приведение температуры к "нормализованному" виду
	swap	r17	;  (0..255 = -55..200 С)
	andi	r17,$F0
	andi	r16,$0F
	or	r16,r17
	subi	r16,-55
	FitByte	r16,MinimalT,MaximalT
	mov	r21,r16
	ldi	r22,TErrCntLen ; взвод счетчика ошибок
w1p_15:	st	-X,r22
	st	-X,r21
	ldi	r16,1<<afOutInTemp ; Переключение на дследующий датчик
	eor	AFlags,r16
	ldi	XL,DSResBuf+1
	sbrs	AFlags,afOutInTemp
	ldi	XL,DSResBuf+3
	ld	r17,X		; Счетчик ошибок запускаемого канала
	ldi_w	ZH,ZL,W1Prot*2
	ldi	r16,7		; Задержка перед следующей оцифровкой
	cpi	r17,TErrRestart
	brsh	w1p_5		; Если много ошибок подряд
	ldi_w	ZH,ZL,W1Init*2	;  переинициализировать датчик
	rjmp	w1p_5


W1Init:	.db	$06,$00	; Reset
	.db	$88,$CC	; SkipRom
	.db	$88,$4E	; Write Stratchpad
	.db	$88,$AA, $88,$00, $88,0b00000000
	.db	$06,$00	; Reset
	.db	$88,$CC	; SkipRom
	.db	$88,$4E	; Write Stratchpad
	.db	$88,$AA, $88,$00, $88,0b00000000
	.db	$06,$00	; Reset
	.db	$88,$CC	; SkipRom
	.db	$88,$44	; Convert
W1Prot:	.db	$06,$00	; Reset
	.db	$88,$CC	; SkipRom
	.db	$88,$44	; Convert
	.db	$06,$00	; Reset
	.db	$88,$CC	; SkipRom
	.db	$88,$BE	; Read stratchpad
	.db	$48,DSReadBuf+0	; Read TL
	.db	$48,DSReadBuf+1	; Read TH
	.db	$48,DSReadBuf+2	; Read TH User
	.db	$06,$00	; Reset
	.db	$88,$CC	; SkipRom
	.db	$88,$BE	; Read stratchpad
	.db	$48,DSReadBuf+3	; Read TL
	.db	$48,DSReadBuf+4	; Read TH
	.db	$00,$00

