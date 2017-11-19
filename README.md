# Mikode

Mikode is an Assembly language Compiler and Emulator, written as a tool to teach kids.
The Syntax is a little less daunting, with Instructions like LoadImm
instead of LDA or Increment instead of INC.

The Compiler parses this faux Assembly code into binary instructions based off the Atmel
AVR MCU.

The Emulator has an address space of 64K including Monitor ROM to display characters to the screen.

The inspiration to write this program came from the desire to teach kids and interested adults about the foundations of computer logic.

Using Mikode, one should ultimately be able to teach how to write simple games in Assembly.

Doesn't that sound like fun?

Note that this repository is a work in progress :]

## Building

Mikode uses a fork of the recursive descent parse, TinyExpr that has been modified to handle bitwise instructions.

To build Mikode clone the repo `https://github.com/BobBurns/tinyexpr`
and put `tinyexpr_bitw.h` and `tinyexpr_bitw.c` in the path `../tefork/tinyexpr/`

Also, if you dont have libncurses installed:

`sudo apt install libncurses-dev`

Then, in the mikode directory

`$ make; make clean`

Currently, Mikode has been tested on Ubuntu Linux.

## Example

Compile the following code with 

`./mikode example1.asm`

```
; example1.asm
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


```

This will generate `example1` and `example1.asm.list`

Checkout the compiled code `less example1.asm.list`

Then run it

`./mikorun example1`


For a more complex example see snake.asm

-------------------------------------------------

## Memory

**Stack**

0xe000 - 0xffff

**Screen Rom**

0xc000 - 0xd800

0x80 X 0x30

**Registers**

r0 - r15

X = r13:r12

Y = r15:r14


**Flags - Sreg**

| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
| - | - | - | - | - | - | - | - |
| - | T | - | S | V | N | Z | C |



[T] Transfer flag

[S] Sign flag

[V] Overflow flag

[N] Negative flag

[Z] Zero flag

[C] Carry flag


------------------------------------------------

## Special Keys during execution

F1 - toggle display registers

F2 - clear display registers

F3 - break (quit)

F4 - stop/start execution


------------------------------------------------

More at ...

[Mikode Docs](https://gitlab.com/bobburns/basm/wikis/Mikode-Documentation)
