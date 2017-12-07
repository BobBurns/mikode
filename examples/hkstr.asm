; blinking an led with mikode and reading
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
        LoadImm         r10,0x00
        StoreDirect     0xe101,r10      ; default setting all pins in
        LoadImm         r10,0x04
        StoreDirect     0xe101,r10      ; set direction out on gpio21
loop:
        LoadImm         temp,0x80
        StoreDirect     0xe100,temp       ; set pin high
        Call            delay
        LoadImm         r9,0x00
        StoreDirect     0xe100,r9       ; set pin low
        Call            delay
        Jump            loop            ; loop indefinetly 
; press F3 to exit
delay:
        LoadImm         r0,0x40
dlp:    
        Call            read            ; check input pin during delay
        Decrement       r0  
        BranchNotEqu    dlp 
        Return
read:
; check input
        LoadDirect      temp,0xe102
        CompareImm      temp,1
        BranchEqu       putx
        LoadImm         temp,' '
        Jump            cont
putx:   LoadImm         temp,'X'
cont:
        StoreDirect     0xc000,temp
        Return

