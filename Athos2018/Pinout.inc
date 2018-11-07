; --- PINOUT Atmega8 8 ÌÃö ---
;		 _____________     _____________
;		 |	      |___|	       |
;		-| 1  Reset 	   ADC5 PC5 28 |- Uphase(n)
; 	    RXD	-| 2  PD0 RXD	   ADC4 PC4 27 |- Uin (160V)
;     	    TXD	-| 3  PD1 TXD	   ADC3 PC3 26 |- 
;		-| 4  PD2 INT0	A  ADC2 PC2 25 |- 
;		-| 5  PD3 INT1	t  ADC1 PC1 24 |- Uab (80V)
;	    Tin	-| 6  PD4 T0	m  ADC0 PC0 23 |- Ich ACS50^ 5V
;		-| 7  VCC	e      AGND 22 |-
;		-| 8  GND	g      AREF 21 |-
;		-| 9  XTAL1	a      AVCC 20 |-
;		-| 10 XTAL2	    SCK	PB5 19 |- DATA/RIGHT
;	   Tout	-| 11 PD5 T1	8  MISO	PB4 18 |- SHIFT/LEFT
;		-| 12 PD6 AIN0+	   MOSI	PB3 17 |- STORE
;		-| 13 PD7 AIN1-	    ^SS	PB2 16 |-
;      ReleyOut	-| 14 PB0 ICP	    0C1	PB1 15 |- PWM^
;		 |_____________________________|

#define	STORE	  PORTB,3
#define	SHIFT	  PORTB,4
#define	SHIFT_DIR DDRB,4
#define	DATA	  PORTB,5
#define	DATA_DIR  DDRB,5

#define	PHASE_PIN PINC,5

#define	TOUT_PIN PIND,5
#define	TOUT_DIR DDRD,5
#define	TOUT_PRT PORTD,5

#define	TIN_PIN	PIND,4
#define	TIN_DIR	DDRD,4
#define	TIN_PRT	PORTD,4

#define	RELEY_PIN PORTB,0



; --- Initial port state ---
.macro	InitPorts
	outi	DDRB, 0b001011
	outi	PORTB,0b111110
	outi	DDRC, 0b000000
	outi	PORTC,0b001100
	outi	DDRD, 0b00000000
	outi	PORTD,0b11001100
.endm


