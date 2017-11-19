; testing
.def xL = r12
.def xH = r13
.def yL	= r14
.def yH = r15
.def temp = r11
.def tmp2 = r10
.def count = r3
.org 0
	LoadImm		xL,low(screen)
	LoadImm		xH,high(screen)
	LoadImm		r7,0
	LoadImm		count,0xff
	LoadImm		r4,0x01
loop:
	LoadDirect	r8,0xe005
	LoadImm		r1,'/'
	Call		put
	Call		delay
	LoadImm		r1,'-'
	Call		put
	Call		delay
	LoadImm		r1,'\'
	Call		put
	Call		delay
	LoadImm		r1,'|'
	Call		put
;put key in values on screen
	LoadImm		yL,low(sc2)
	LoadImm		yH,high(sc2)
	LoadDirect	temp,0xe000
	Push		temp
;	Call		itoa
	Call		delay
	Call		erase
	Add		xL,r4
	AddWithCarry	xH,r7	
	Decrement	count
	BranchEqu	done
	Jump		loop
done:
	Break
put:
	Move		r0,r1
	StoreIndX	X,r0
	Return
erase:
	LoadImm		r0,0x20
	StoreIndX	X,r0
	Return
delay:
	LoadImm		r5,0x10
dlp:	
	Decrement	r5
	BranchNotEqu	dlp
	Return
itoa:
	Pop		r5
	Pop		r6
	Pop 		temp
	Push		count
	Push		r0
	LoadImm		count,2
	Move 		tmp2,temp
	LogicalShiftRt	temp
	LogicalShiftRt	temp
	LogicalShiftRt	temp
	LogicalShiftRt	temp
itoa2:
	AndImm		temp,0x0f
	CompareImm	temp,0x0a
	BranchLower	itoa1
	LoadImm		r0,0x07
	Add		temp,r0
itoa1:  LoadImm		r0,0x30
	Add		temp,r0
	StoreIndY	Y+,temp
	Decrement	count
	BranchEqu	itoa4
	Move		temp,tmp2
	Jump		itoa2
itoa4:
	Pop		r0
	Pop		count
	Push		r6
	Push		r5
	Return
.org 0xc000
screen:
	.db 0
.org 0xc030
sc2:
	.db 0
