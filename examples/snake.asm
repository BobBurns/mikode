; adaptation of snake 6502
; testing cur_asm
.def yH = r15
.def yL = r14
.def xH = r13
.def xL = r12
.def temp = r11
.def tmp2 = r10
.def count = r9
.def dreg = r1
; 
; Screen 0x40 X 0x9 0xc000
;
; Addresses
; direction 	0x102
; lenght 	0x103
; head 		0x110
; body 		0x112
	Jump 		start
;simulate 0 page
.org 0x100
opage:
	.db 0
.org 0x200
start:
	Call 		drawbrd
	Call		inits
	Call 		genapppos
	Call		loop
	Break
; borders
drawbrd:
	LoadImm		xL,0x40
	LoadImm		xH,0xc0
	LoadImm		tmp2,0x80
	LoadImm		r8,0x0
	LoadImm		temp,0x2b
	LoadImm		count,0x12
drblp:
	StoreIndX	X,temp
	Add		xL,tmp2
	AddWithC	xH,r8	
	Decrement	count
	BranchNotEqu	drblp
	LoadImm		xL,0x0
	LoadImm		xH,0xc9
	LoadImm		temp,0x2b
	LoadImm		count,0x40
drbotlp:
	StoreIndX	X,temp
	AddImmWord	xH:xL,1
	Decrement	count
	BranchNotEqu	drbotlp
	Return
inits:
;delay time
	LoadImm		dreg,0x31
	LoadImm		xL,0x02
	LoadImm		xH,0x01
	LoadImm		temp,2 ;start direction
	StoreIndX	X+,temp
	LoadImm		temp,4 ;snake length
	StoreIndX	X+,temp
	LoadImm		xL,0x10
; begining position
;
	LoadImm		tmp2,0xc2
	LoadImm		temp,0x10
	StoreIndX	X+,temp
	StoreIndX	X+,tmp2
	Decrement	temp ; 0x31
	StoreIndX	X+,temp
	StoreIndX	X+,tmp2
	Decrement	temp ; 0x30
	StoreIndX	X+,temp
	StoreIndX	X+,tmp2
	Return
genapppos:
	LoadImm		xL,0x04
	LoadImm		xH,0xe0 ; 0xe004 rand number	
	LoadIndX	temp,X+
	AndImm		temp,0xbf
	StoreDirect	0x100,temp
	LoadIndX	temp,X
	AndImm		temp,0x07
	LoadImm		tmp2,0xc0
	Add		temp,tmp2
	StoreDirect	0x101,temp
	Return
loop:
	Call		readkeys
	Call		checkcoll
	Call		updatesna
	Call		drawapple
	Call		drawsnake
	Call		spinwheels	
	Jump		loop
readkeys:
	LoadDirect	temp,0xe000 ; key in space
	CompareImm	temp,0x77 ; 'W'
	BranchEqu	upkey
	CompareImm	temp,0x64 ; 'D'
	BranchEqu	rightkey
	CompareImm	temp,0x73 ; 'S'
	BranchEqu	downkey
	CompareImm	temp,0x61 ; 'A'
	BranchEqu	lftkj
	Return
lftkj:
	Jump		leftkey
upkey:
	LoadImm		temp,0x04
	LoadDirect	tmp2,0x102
	And		tmp2,temp
	BranchNotEqu	upkjmp
	LoadImm		temp,0x01 ; 1 is up value
	StoreDirect	0x102,temp
	Return
upkjmp:
	Jump		illegalmv
rightkey:
	LoadImm		temp,0x08 ; left key
	LoadDirect	tmp2,0x102
	And		tmp2,temp
	BranchNotEqu	rtkjmp
	LoadImm		temp,0x02 ; 2 is right value
	StoreDirect	0x102,temp
	Return
rtkjmp:
	Jump		illegalmv
downkey:
	LoadImm		temp,0x01
	LoadDirect	tmp2,0x102
	And		tmp2,temp
	BranchNotEqu	dwnkjmp
	LoadImm		temp,0x04 ; 4 is down value
	StoreDirect	0x102,temp
	Return
dwnkjmp:
	Jump		illegalmv
leftkey:
	LoadImm		temp,0x02
	LoadDirect	tmp2,0x102
	And		tmp2,temp
	BranchNotEqu	lftkjmp
	LoadImm		temp,0x08 ; 8 is left value
	StoreDirect	0x102,temp
	Return
lftkjmp:
illegalmv:
	Return
checkcoll:
	Call		chkapcol
	Call		chksnkcol
	Return
chkapcol:
	LoadImm		xL,0x00
	LoadImm		xH,0x01
	LoadImm		yL,0x10
	Move		yH,xH
	LoadIndX	temp,X+
	LoadIndY	tmp2,Y+
	Compare		temp,tmp2
	BranchNotEqu	dnchkapp
	LoadIndX	temp,X+
	LoadIndY	tmp2,Y
	Compare		temp,tmp2
	BranchNotEqu	dnchkapp
	LoadImm		xL,0x03
	LoadIndX	count,X
	Increment	count
	Increment	count
	StoreIndX	X,count
	LoadImm		temp,0x05
	Subtract	dreg,temp
	BranchCarryClr	apcolcnt
	LoadImm		dreg,1
apcolcnt:
	Call 		genapppos
dnchkapp:
	Return
chksnkcol:
	LoadImm		count,0
	LoadImm		xL,0x12 ; snake segment
	LoadImm		xH,0x01
	LoadImm		yL,0x10	; snake head
	LoadImm		yH,0x01
snkcollp:
	LoadIndX	temp,X+
	LoadIndY	tmp2,Y+
	Compare		temp,tmp2
	BranchNotEqu	cntcollp
	LoadIndX	temp,X+
	LoadIndY	tmp2,Y	
	Compare 	temp,tmp2
	BranchEqu	didcollide
cntcollp:
	Decrement 	yL ; back to head
	Increment	xL
	Increment	count
	LoadDirect	temp,0x103
	Compare		temp,count
	BranchEqu	didntcoll
	Jump		snkcollp
didcollide:
	Jump		gameover
didntcoll:
	Return
updatesna:
	LoadDirect	count,0x0103
	LoadImm		xL,0x10
	Add		xL,count
	LoadImm		xH,0x01
	LoadImm		yL,0x12
	Add		yL,count
	Move		yH,xH
updateloop:
	LoadIndX	temp,-X
	StoreIndY	-Y,temp
	Decrement	count
	BranchNotEqu	updateloop
; next move the head
	LoadImm		xL,0x10
	LoadImm		xH,0x01
	LoadDirect	temp,0x102 ;location of direction
	LogicalShiftRt	temp
	BranchCarrySet	up
	LogicalShiftRt	temp
	BranchCarrySet	rtjmp
	LogicalShiftRt	temp
	BranchCarrySet	down
	LogicalShiftRt	temp
	BranchCarryClr	upddone
	Jump		left
rtjmp:	Jump		right
upddone:
	Return
up:
	LoadIndX 	yL,X+ ;low byte
	LoadIndX	yH,X
	LoadImm		temp,0
	LoadImm		tmp2,0x80
	Subtract	yL,tmp2
	SubtractWithC	yH,temp 
	CompareImm	yH,0xc0
	BranchSameHigh	updone
	Jump		collision
updone:
	StoreIndX	X,yH
	StoreIndX	-X,yL
	Return
down:
	LoadIndX	yL,X+
	LoadIndX	yH,X
	LoadImm		temp,0
	LoadImm		tmp2,0x80
	Add		yL,tmp2
	AddWithC	yH,temp
	CompareImm	yH,0xc9
	BranchLower	downdone
	Jump		collision
downdone:
	StoreIndX	X,yH
	StoreIndX	-X,yL
	Return
right:
	LoadIndX	yL,X+
	LoadIndX	yH,X
	AddImmWord	yH:yL,1
	LoadImm		temp,0x3f
	And		temp,yL		;test 0xc0
	BranchEqu	rcoll
	LoadImm		temp,0xbf
	And		temp,yL		;test 0x40
	BranchEqu	rcoll
	StoreIndX	X,yH
	StoreIndX	-X,yL
	Return
rcoll:
	Jump		collision
left:	
	LoadIndX	yL,X+
	LoadIndX	yH,X
	SubtractImmWord	yH:yL,1
	LoadImm		temp,0x7f
	And		temp,yL
	CompareImm	temp,0x7f
	BranchNotEqu	leftdone
	Jump		collision
leftdone:
	StoreIndX	X,yH
	StoreIndX	-X,yL
	Return

collision:
	Jump		gameover
drawapple:
	LoadImm		temp,0x40
	LoadImm		xL,0x00
	LoadImm		xH,0x01
	LoadIndX	yL,X+
	LoadIndX	yH,X
	StoreIndY	Y,temp
	Return
drawsnake:	
	LoadImm 	temp,0x20
	LoadImm		xL,0x10
	LoadImm		xH,0x01
	LoadDirect	tmp2,0x0103
	Add		xL,tmp2
	LoadIndX	yL,X+
	LoadIndX	yH,X
	StoreIndY	Y,temp
	LoadImm		xL,0x10
	LoadIndX	yL,X+
	LoadIndX	yH,X
	LoadImm		temp,0x3e
	StoreIndY	Y,temp
	Return
spinwheels:
	Move		count,dreg
swhlp:	
	Decrement	count
	BranchNotEqu	swhlp
	Return
gameover:
	Break
