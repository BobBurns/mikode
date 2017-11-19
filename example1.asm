; Using mikode.
; example program to draw characters on the screen
; 
.def xL = r12
.def xH = r13
.def temp = r11
.def count = r3
.org 0
	LoadImm		xL,low(screen)
	LoadImm		xH,high(screen)
	LoadImm		r5,0
	LoadImm		count,0xff
	LoadImm		r4,0x01
loop:
	LoadImm		temp,'/'
	Call		put
	Call		delay
	LoadImm		temp,'-'
	Call		put
	Call		delay
	LoadImm		temp,'\'
	Call		put
	Call		delay
	LoadImm		temp,'|'
	Call		put
;put key in values on screen
	Push		count
	Call		delay
	Call		erase
	Pop		count
	Add		xL,r4
	AddWithCarry	xH,r5	
	Decrement	count
	BranchEqu	done
	Jump		loop
done:
	Break
put:
	Move		r0,temp
	StoreIndX	X,r0
	Return
erase:
	LoadImm		r0,0x20
	StoreIndX	X,r0
	Return
delay:
	LoadImm		count,0x80
dlp:	
	Decrement	count
	BranchNotEqu	dlp
	Return
.org 0xc000
screen:
	.db 0

