Rele:
	ldd	r16,Y+yNoReleyCnt
	subi	r16,1
	brcs	r_6
	std	Y+yNoReleyCnt,r16
	ret

r_6:	ldd	r25,Y+yReleyCnt
	sbic	RELEY_PIN
	rjmp	r_1		; ����� "���� ���������"
	sbrc	AFlags,afChHigh
	rjmp	r_3		; �� �������� ��� ������ "�� �����" ChHigh
	ldd	r16,Y+yPWMPhaseH
	cpi	r16,245
	brlo	r_2
	subi	r25,4	; ��������� ������� ��� ���������� ����������� (x4)
	brcc	r_ex
r_3:	ldi	r25,0
	rjmp	r_ex
r_2:	inc	r25	; ����������� ��� �����������
	cpi	r25,57*2 ; ������� 2 ������
	brlo	r_ex
	sbi	RELEY_PIN
	ldi	r25,0
	rjmp	r_ex
r_1:				; ����� "���� ��������"
	sbrc	AFlags,afChHigh
	rjmp	r_4		; ����� ��������� ��� ������ "�� �����" ChHigh
	lds	r16,UabL
	lds	r17,UabH
	cpi_w	r17,r16, r18,450 ; ����� ��������� ��� U<45V
	brlo	r_4
	cpi_w	r17,r16, r18,490 ; ����� 2 ������ ��� U<49V
	brlo	r_5
	subi	r25,1
	brcc	r_ex
	ldi	r25,0
	rjmp	r_ex
r_5:	inc	r25
	cpi	r25,57*2 ; ������� 2 ������
	brlo	r_ex
r_4:	cbi	RELEY_PIN
	ldi	r25,0

r_ex:	std	Y+yReleyCnt,r25
	ret
