; Read GPIO example
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
	LoadImm		r10,0x40
	StoreDirect	0xe101,r10	; set direction out on gpio20
	LoadImm		r10,0x00	; toggle it to read
	StoreDirect	0xe101,r10
loop:
	LoadDirect	r9,0xe102
	CompareImm	r9,1
	BranchEqu	putx
	LoadImm		r8,' '		; line is low so put space
	StoreDirect	0xc000,r8
	Jump		loop
putx:
	LoadImm		r8,'X'		; line is high so put X
	StoreDirect	0xc000,r8
	Jump		loop

