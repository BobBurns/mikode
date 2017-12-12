; new demo program
; shoot the apple to make a led blink
; miss and gave over
;
; gpio out on gpio 21
;
; compile: mikode shootit.asm
; execute: mikode -r shootit  ## or if using with gpio mikode -rg shootit
;
;
.def yH = r15
.def yL = r14
.def xH = r13
.def xL = r12
.def temp = r11
.def tmp2 = r10
.def count = r9
.def firer = r8
; 
; Screen 0x40 X 0x12 0xc000
;
	Jump 		start
;simulate 0 page
.org 0x100
gunxy:
	.db 0,0
bulxy:
	.db 0,0
shpspr:
	.db "<-X->"
noshp:
	.db "     "
appos:
	.db 0,0
gotext:
	.db 18,"G a m e  O v e r !"
.org 0x200
start:
	Call		inits
	Call 		draws
	Call		loop
	Break
; borders
inits:
;delay time
	LoadImm		firer,0	  ; fire off
; init gpio
	LoadImm		temp,0x80
	StoreDirect	0xe101,temp	; set direction out on gpio21
;
	LoadImm		xL,low(gunxy)
	LoadImm		xH,high(gunxy)
	LoadImm		temp,0x20 ;start position low
	StoreIndX	X+,temp
	LoadImm		temp,0xc9 ; start pos high 
	StoreIndX	X+,temp
	LoadImm		temp,0xa0
	StoreDirect	bulxy,temp
	LoadImm		temp,0xc8
	StoreDirect	bulxy+1,temp
	Call		genapppos
	Return
; draw snake takes temp as arguement to draw or erase
draws:  
	LoadDirect	xL,gunxy
	LoadDirect	xH,gunxy+1
	Decrement	xL
	Decrement	xL	      ;left side of ship
	LoadImm		count,5	      ; ship length
	TestZero	temp
	BranchEqu	drnos
	LoadImm		yL,low(shpspr)
	LoadImm		yH,high(shpspr)
	Jump		drwslp
drnos:	
	LoadImm		yL,low(noshp)
	LoadImm		yH,high(noshp)
drwslp:
	LoadIndY	temp,Y+
	StoreIndX	X+,temp
	Decrement	count
	BranchNotEqu	drwslp
	Return
drawb:  
	LoadDirect	xL,bulxy
	LoadDirect	xH,bulxy+1
	TestZero	temp
	BranchEqu	drbno
	LoadImm		temp,'!'
	Jump		drwb
drbno:	
	LoadImm		temp,' '
drwb:
	StoreIndX	X+,temp
	Return
genapppos:
	LoadImm		xL,0x04
	LoadImm		xH,0xe0 ; 0xe004 rand number	
	LoadIndX	temp,X+
	AndImm		temp,0xbc
	StoreDirect	appos,temp
	LoadIndX	temp,X
	AndImm		temp,0x07
	LoadImm		tmp2,0xc0
	Add		temp,tmp2
	StoreDirect	appos+1,temp
; this could be simplified
	LoadImm		temp,0x40
	LoadImm		xL,low(appos)
	LoadImm		xH,high(appos)
	LoadIndX	yL,X+
	LoadIndX	yH,X
	StoreIndY	Y,temp
	Return
loop:
	Call		readkeys
	Call		updatescr
	Call		checkcoll
	Jump		loop
readkeys:
; store x in r3:r2 temporary
	LoadImm		r2,low(gunxy)
	LoadImm		r3,high(gunxy)
	LoadDirect	temp,0xe000 ; key in space
	CompareImm	temp,0x05 ; ->
	BranchEqu	rightkey
	CompareImm	temp,0x04 ; <- 
	BranchEqu	leftkey
	CompareImm	temp,0x20 ; ' ' 
	BranchEqu	shtj
	Return
shtj:
	Jump		shoot
rightkey:
	LoadImm		temp,0
	Call		draws	      ;erase gun
	Move		xL,r2
	Move		xH,r3
	LoadIndX	temp,X
	CompareImm	temp,0x40-2   ; right boundry
	BranchEqu	rkdone
	Increment	temp
rkdone:
	StoreIndX	X,temp
	LoadImm		temp,1
	Call		draws
	LoadImm		temp,0
	StoreDirect	0xe000,temp   ; clear keyin
	Return
leftkey:
	LoadImm		temp,0
	Call		draws	      ;erase gun
	Move		xL,r2
	Move		xH,r3
	LoadIndX	temp,X
	CompareImm	temp,2	      ; left boundry
	BranchEqu	lkdone
	Decrement	temp
lkdone:
	StoreIndX	X,temp
	LoadImm		temp,1
	Call		draws
	LoadImm		temp,0
	StoreDirect	0xe000,temp
	Return
shoot:
	CompareImm	firer,1
	BranchEqu	shdone
	Call		resetb
	LoadImm		firer,1
shdone:
	LoadImm		temp,0
	StoreDirect	0xe000,temp
	Return
checkcoll:
	LoadDirect	temp,bulxy
	LoadDirect	tmp2,appos
	Compare		temp,tmp2
	BranchNotEqu	ccdone
	LoadDirect	temp,bulxy+1
	LoadDirect	tmp2,appos+1
	Compare		temp,tmp2
	BranchNotEqu	ccdone
	Call		collision
ccdone:
	Return
updatescr:
	CompareImm	firer,0
	BranchNotEqu	updfire
	Return
updfire:
	LoadImm		temp,0
	Call		drawb
	LoadDirect	temp,bulxy	
	LoadDirect	tmp2,bulxy+1
	SubtractImm	temp,0x80
	SubtractImmC	tmp2,0
	CompareImm	tmp2,0xbf
	BranchNotEqu	norb
	Jump		gameover
norb:
	StoreDirect	bulxy,temp
	StoreDirect	bulxy+1,tmp2
	LoadImm		temp,1
	Call		drawb
	Call		spinwheels
	Return
resetb:
	LoadImm		firer,0
	LoadDirect	temp,gunxy
	LoadDirect	tmp2,gunxy+1
	SubtractImm	temp,0x80
	SubtractImmC	tmp2,0x00
	StoreDirect	bulxy,temp
	StoreDirect	bulxy+1,tmp2
	Return
collision:
; do cool stuff
	Call		blink
	LoadImm		temp,0
	Call		drawb		; erase bullet
	Call		genapppos
	Call		resetb
	Return
spinwheels:
	LoadImm		count,0x10
swhlp:	
	Decrement	count
	BranchNotEqu	swhlp
	Return
gameover:
	LoadImm		xL,low(gotext)
	LoadImm		xH,high(gotext)
	LoadImm		yL,0x1a
	LoadImm		yH,0xc5
	LoadIndX	r0,X+		; first val count
golp:
	LoadIndX	temp,X+
	StoreIndY	Y+,temp
	LoadImm		count,0x80
	Call		swhlp		; delay after each letter
	Decrement	r0
	BranchNotEqu	golp
	Break
blink:
	LoadImm		r0,5
bloop:
	LoadImm		temp,0x80
	StoreDirect	0xe100,temp	; set pin high
	Call		spinwheels
	LoadImm		temp,0x00
	StoreDirect	0xe100,r9	; set pin low
	Call		spinwheels
	Decrement	r0
	BranchNotEqu	bloop
	Return
	
endlp:
	NOP
	Jump		endlp
