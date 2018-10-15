Rele:
	lds	r16,Tab
	ldd	r17,Y+yTHeatOn
	subi	r17,-55
	cp	r17,r16
	brlo	r_1
	sbi	RELEY_PIN
	ret
r_1:
	ldd	r17,Y+yTHeatOff
	subi	r17,-55
	cp	r16,r17
	brlo	r_2
	cbi	RELEY_PIN
r_2:
	ret

