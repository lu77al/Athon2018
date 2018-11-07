CheckPhase:	; 3906.25 Hz
	ldd	r17,Y+yPhaseRepCnt ; бит 0 - состояние;
	andi	r17,0b11111	   ; биты 1-4 - счетчик постоянного состояния
	clt			;  флаг срабатывания (заготовка)
	ldi	r16,0
	sbic	PHASE_PIN
;	ldd	r20,Y+yMainCntL
;	sbrs	r20,6
	ldi	r16,1		; Формируем текущий
	eor	r16,r17
	andi	r16,1
	breq	phc_1		; Выход c взведением если нет изменения
	subi	r17,2
	brcc	phc_2		; Выход если изменения не долгое
	com	r17		; Смена сохранённого состояния
	bst	r17,0		;  флаг срабатывания
phc_1:	ori	r17,0b11110	;   взведение счетчика
phc_2:	std	Y+yPhaseRepCnt,r17 ; и сохранение счетчика  
	ldd	r17,Y+yPhaseTime  ; Счетчик длины фазы
	ldd	r16,Y+yMainCntL
	andi	r16,0b11
	brne	phc_4	; увеличеини с частотой 3906.25/4 = 976.6 Hz
	inc	r17
	breq	phc_3	; Сохранить в буфер если переполнение
	std	Y+yPhaseTime,r17
phc_4:	brts	phc_3	;   или был флаг срабатывания
	ret
phc_3:	ldd	r16,Y+yPhasePos	; Сохранение в буфер 
	subi	r16,1
	brcc	phc_5
	ldd	r16,Y+yGenPoles	; с установленной длиной
phc_5:	std	Y+yPhasePos,r16
	ldi	XL,PHTimeBuf
	add	XL,r16
	st	X,r17
	std	Y+yPhaseTime,ZeroReg
	ret

CalcSpeed:
	ldi	XL,PHTimeBuf
	ldd	r16,Y+yGenPoles
	ldi_w	r19,r18,0
csp_2:	ld	r17,X+
	cpi	r17,0
	breq	csp_1
	add	r18,r17
	adc	r19,ZeroReg
	subi	r16,1
	brcc	csp_2
csp_3:	sts	RPeriodL,r18
	sts	RPeriodH,r19
	ldi_w	r17,r16,62311 ;58593
	rcall	Div
	ldi_w	ZH,ZL,TurNL
	rjmp	SaveIndVal
csp_1:	sts	RPeriodL,ZeroReg
	sts	RPeriodH,ZeroReg
	ldi_w	r17,r16,0
	ldi_w	ZH,ZL,TurNL
	rjmp	SaveIndVal

