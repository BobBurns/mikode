; testing gpio on pi with mikode
; 0xe100 output register
; 0xe101 direction register
; 0xe102 input register
; b0 - gpio 6
; b1 - gpio 13
; b2 - gpio 19
; b3 - gpio 26
; b4 - gpio 12
; b5 - gpio 16
; b6 - gpio 20
; b7 - gpio 21
	LoadImm		r10,0x80
	StoreDirect	0xe101,r10	; set direction out on gpio21
loop:
	LoadImm		r9,0x80
	StoreDirect	0xe100,r9	; set pin high
	Call		delay
	LoadImm		r9,0x00
	StoreDirect	0xe100,r9	; set pin low
	Call		delay
	Jump		loop
delay:
	LoadImm		r0,0x40
dlp:	Decrement	r0
	BranchNotEqu	dlp
	Return
