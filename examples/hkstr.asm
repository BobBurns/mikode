; blinking an led and reading gpio 26
;
; special rom address
; screen rom 0xc000
;
; 0xe100 output register
; 0xe101 direction register
; 0xe102 input register
;
; pin mappings
; b0 - gpio 6
; b1 - gpio 13
; b2 - gpio 19
; b3 - gpio 26
; b4 - gpio 12
; b5 - gpio 16
; b6 - gpio 20
; b7 - gpio 21
;
; defs
.def temp = r9
	LoadImm		r10,0x08
	StoreDirect	0xe101,r10 ; toggle pin to set input
	LoadImm		r10,0x00
	StoreDirect 	0xe101,r10 ; set direction input gpio26
	LoadImm		r10,0x04
	StoreDirect	0xe101,r10 ; set direction output gpio19
loop:	
	LoadImm		temp,0x04
	StoreDirect	0xe100,temp
	Call		delay
	LoadDirect	r2,0xe101 ; checking dir value
	LoadDirect	r3,0xe102 ; checking in value
	LoadImm		temp,0x00
	StoreDirect	0xe100,temp
	Call		delay
	LoadDirect	r2,0xe101
	LoadDirect	r3,0xe102
	Jump		loop
delay:
	LoadImm		r0,0x40
dlp:
	Call		read
	Decrement	r0
	BranchNotEqu	dlp
	Return
read:
	LoadDirect	temp,0xe102 ;read pin during delay loop
	AndImm		temp,0x08
	BranchEqu	putx
	LoadImm		temp,' '
	Jump		cont
putx:
	LoadImm		temp,'X'
cont:
	StoreDirect	0xc540,temp ; should be middle of the screen
	Return
	
