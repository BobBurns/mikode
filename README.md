# Mikode

Mikode is an Assembly language Compiler and Emulator, written as a tool 
to teach anyone interested in learning to code in Assembly.
The Syntax is a little less daunting, with Instructions like LoadImm
instead of LDA or Increment instead of INC.

The inspiration to write this program came from the desire to teach 
the foundations of computer logic.
Using Mikode, you can learn how to write simple games in Assembly.
Doesn't that sound like fun?

The Emulator has an address space of 64K including Monitor ROM to display characters to the screen.
The Compiler parses this faux Assembly code into binary instructions based off the Atmel
AVR MCU.


Note that even though the program is working, this repository is a work in progress :]

Also, 
Mikode uses a fork of the awesome recursive descent parser by Lewis Van Winkle, TinyExpr, that has been modified to handle bitwise instructions.

## Building

If you dont have libncurses installed:

`sudo apt install libncurses-dev`

You can build mikode in one of two ways.

If you have autotools, you can generate the configure script

```
$ autoreconf -i
$ ./configure
$ make all check
$ sudo make install
```

If you *do not* have autotools, you'll need to download the source and build it that way

```
$ wget
https://github.com/BobBurns/mikode/releases/download/v1.2.2/mikode-1.2.2.tar.gz
$ gzip -cd mikode-1.2.1.tar.gz | tar xf -
$ cd mikode-1.2.1
$ ./configure
$ make all check
$ sudo make install
```

Currently, Mikode has been tested on Ubuntu and Raspbian Linux. It should run on any Debian based Linux distro.

Update: initial tests on PocketChip are good.  I need to map GPIO.

## Usage

`mikode [-rg] [-s] [--run] [--gpio] [--sleep=TIME] file`

## Example

In the examples directory, compile the following code with 

`mikode example1.asm`

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
; save count register on the stack
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

`mikode -r example1`


For a more complex example see snake.asm

-------------------------------------------------

## Memory

**Stack**

0xf000 - 0xffff

**Screen Rom**

0xc000 - 0xd400  Set to 128 (0x80) by 40 (0x28)

**Key in**

0xe000  Load from this address to get key input

This will return ascii coded byte. Also,
special keys: left arrow (4), right arrow (5)
up arrow (3), down arrow (2)


**GPIO**

Requires root. Works with the Raspberry Pi A+,B+,2B,3B

Register bits correspond to gpio 6, 13, 19, 26, 12, 16, 20, 21
 
0xe100 - output

0xe101 - set direction

0xe102 - input

See examples/blink.asm

**Random**

0xe004 - 0xe007  Random int value every cycle

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

F5 - step


------------------------------------------------


## Documentation and Package

Instruction Set at

[mikode/wiki](https://github.com/BobBurns/mikode/wiki)

Tarball available at

[reburns downloads](https://people.ucsc.edu/~reburns/downloads)

