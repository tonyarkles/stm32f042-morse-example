
# What is this?

This is a demo of a from-scratch bring-up of the STM32F042 Nucleo
board, using a gcc toolchain, unit tests, and CMake for
cross-compiling.

# What are the pieces?

crosstool-ng is used to build the toolchain

cmake is used to build the code

openocd is used to talk to the board

Unity is used for unit testing the code, primarily by running the
tests on the host machine

# crosstool-ng

todo

# OpenOCD

OpenOCD provides access to the JTAG/SWD functionality provided by the
integrated ST-Link/V2 interface on the Nucleo.

$ sudo openocd -f /usr/share/openocd/scripts/board/st_nucleo_f0.cfg

Awesome that there's already a pre-configured OpenOCD config for this board.

# Sidebar: Reverse engineering an old Makefile

I've got an old Makefile from an STM32F042 project that was used quite
successfully for a long time. There's bits and pieces from it that I
want to extract.

The final target is called `program`. This calls `dfu-util` to flash
firmware onto the board and depends on `$(PROJ_NAME).bin`.

`$(PROJ_NAME).bin` is generated using `objcopy` and depends on
`$(PROJ_NAME).elf`. Objcopy extracts the executable section out of the
elf file and generates a raw binary that we can write to flash.

`$(PROJ_NAME).elf` is where all the compilation happens. It depends on
the `$(SRCS)` variable (all of the application sources) and
`libpersea-usb.a`, which we can ignore for this project.

The key line from the `$(PROJ_NAME).elf` target is this:

```
	$(CC) $(CFLAGS) $^ -o $@ -L$(STD_PERIPH_LIB) -lstm32f0  -L$(LDSCRIPT_INC) -Tstm32f0.ld -L$(NANOPB_LIB) -lnanopb
```

This takes all of the prerequisites (`$^`), and outputs to the target
(`$@`). This includes the STM32 standard peripheral library and the
NanoPB library (also not relevant here). One of the key pieces here is
going to be the linker script (`-Tstm32f0.ld`). The linker script is
what takes our code and figures out where to position it in memory.

We'll need to come back to this. Let's get the toolchain working first.

http://www.valvers.com/open-software/raspberry-pi/step03-bare-metal-programming-in-c-pt3/

```
add_custom_command(
  TARGET armc-010 POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} ./armc-010${CMAKE_EXECUTABLE_SUFFIX} -O binary ./kernel.img
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Convert the ELF output file to a binary image" )
```

# CMake

The first goal is to get CMake to build both host-based tests and
target-ready binaries. The `toolchain-stm32f042.cmake` defines all of
the bits about the cross-compiler toolchain.

```
build-target$ cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-stm32f042.cmake ..
build-target$ make
```

This generates a target/target.elf in the build directory!

```
build-target$ file target/target.elf
target/target.elf: ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV), statically linked, not stripped
```

That's a pretty good sign!

# Generating a .bin from the .elf

There's a couple pieces here: the startup file and the linker
script. I've got an old linker script, but it's got weird copyright
things in it, and I want to write one from scratch.

What is a linker script? Basically, it tells the linker how to lay out
all of the compiled code in memory. For the STM32F042, we've got two
chunks of memory: RAM and Flash. Also, in the .elf file, there's going
to be a number of "sections" - these are the different parts of your
code: global variables, code, data, debug symbols, etc.

Let's start by looking at what's in the .elf file.

```
build-target$ ~/x-tools/arm-unknown-eabi/bin/arm-unknown-eabi-objdump -h target/target.elf
target/target.elf:     file format elf32-littlearm

Sections:
Idx Name          Size      VMA       LMA       File off  Algn
  0 .init         00000018  00008000  00008000  00008000  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 .text         000021e0  00008018  00008018  00008018  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  2 .fini         00000018  0000a1f8  0000a1f8  0000a1f8  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  3 .rodata       0000000a  0000a210  0000a210  0000a210  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  4 .ARM.exidx    00000008  0000a21c  0000a21c  0000a21c  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  5 .eh_frame     00000004  0000a224  0000a224  0000a224  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  6 .init_array   00000004  0001a228  0001a228  0000a228  2**2
                  CONTENTS, ALLOC, LOAD, DATA
  7 .fini_array   00000004  0001a22c  0001a22c  0000a22c  2**2
                  CONTENTS, ALLOC, LOAD, DATA
  8 .jcr          00000004  0001a230  0001a230  0000a230  2**2
                  CONTENTS, ALLOC, LOAD, DATA
  9 .data         00000958  0001a238  0001a238  0000a238  2**3
                  CONTENTS, ALLOC, LOAD, DATA
 10 .bss          00000104  0001ab90  0001ab90  0000ab90  2**2
                  ALLOC
 11 .comment      0000002e  00000000  00000000  0000ab90  2**0
                  CONTENTS, READONLY
 12 .debug_aranges 00000338  00000000  00000000  0000abc0  2**3
                  CONTENTS, READONLY, DEBUGGING
 13 .debug_info   0000dcb2  00000000  00000000  0000aef8  2**0
                  CONTENTS, READONLY, DEBUGGING
 14 .debug_abbrev 00003002  00000000  00000000  00018baa  2**0
                  CONTENTS, READONLY, DEBUGGING
 15 .debug_line   000035bc  00000000  00000000  0001bbac  2**0
                  CONTENTS, READONLY, DEBUGGING
 16 .debug_frame  00000b0c  00000000  00000000  0001f168  2**2
                  CONTENTS, READONLY, DEBUGGING
 17 .debug_str    000016e6  00000000  00000000  0001fc74  2**0
                  CONTENTS, READONLY, DEBUGGING
 18 .debug_loc    00002d68  00000000  00000000  0002135a  2**0
                  CONTENTS, READONLY, DEBUGGING
 19 .debug_ranges 000001b8  00000000  00000000  000240c2  2**0
                  CONTENTS, READONLY, DEBUGGING
 20 .ARM.attributes 0000002d  00000000  00000000  0002427a  2**0
                  CONTENTS, READONLY

```

That's a lot of stuff! Luckily, we don't really care about most of
them. There's also a few missing. For instance, the `.isr_vector`
section - for the STM32F042, the `.isr_vector` section needs to get
put at the start of flash. The MCU reads this and determines how
interrupts will be handled. We'll get to that later.

We start the linker script by defining the memory regions (RAM and
Flash). These values come from the STM32F042 datasheet.

Trying to build this results in a few errors, the first one is that it
doesn't know where the `Reset_Handler` function is defined. This is
part of an assembly file called the "startup file", which ST provides
for us. We could write this ourselves, but it's a pain in the butt
and... it's already given to us.

Adding the `startup_stm32f0xx.s` file gets us a different set of
errors. Now it's complaining that there's a bunch of undefined
references that look like `__bss_start__`, `_sdata`, `_edata`,
etc. These are variables that the `startup_stm32f0xx.s` file looks
for, which map to the different spots in flash and RAM.

With the basic linker script in place, now it's time to start getting
the hardware going. The startup file tries to call a function called
`SystemInit` before calling `main`.

# Starting up some hardware

The `SystemInit` in the ST libraries initializes the bare minimum set
of hardware required to start `main()`. Looking at the file provided
by ST, this basically just turns on some clocks. Not worth
re-implementing this, so I'll just import it and strip out all of the
Windows newlines.

There's a few files to import from the original CMSIS code that I have
kicking around, which is all under ST's license (you can use this code
if you're targetting an ST CPU).

Now it builds into an ELF file!

# Getting a super simple test running

Awesome! We've got the ability to build code for the target. Let's
prove that it's working. This is going to be a painfully simple chunk
of code.

```
volatile int i = 0;
int main() {
  while(1) {
    for(i = 0; i < 100000; i++) {
    }
  }
}
```

This will allow us to verify, using gdb and OpenOCD, that it's reaching our code and incrementing the `i` variable.

```
$ sudo openocd -f /usr/share/openocd/scripts/board/st_nucleo_f0.cfg
Open On-Chip Debugger 0.9.0 (2015-09-02-10:42)
Licensed under GNU GPL v2
For bug reports, read
        http://openocd.org/doc/doxygen/bugs.html
Info : The selected transport took over low-level target control. The results might differ compared to plain JTAG/SWD
adapter speed: 1000 kHz
adapter_nsrst_delay: 100
none separate
srst_only separate srst_nogate srst_open_drain connect_deassert_srst
Info : Unable to match requested speed 1000 kHz, using 950 kHz
Info : Unable to match requested speed 1000 kHz, using 950 kHz
Info : clock speed 950 kHz
Info : STLINK v2 JTAG v25 API v2 SWIM v14 VID 0x0483 PID 0x374B
Info : using stlink api v2
Info : Target voltage: 3.243541
Info : stm32f0x.cpu: hardware has 4 breakpoints, 2 watchpoints
	
```

This connects to the ST-Link/V2 chip on the Nucleo board, which
identifies the correct adapter and CPU. Now, we want to connect GDB to
OpenOCD:

```
(gdb) target remote localhost:3333
Remote debugging using localhost:3333
0x00000000 in ?? ()
```

That's a good start! It sees that the chip is stuck at the start of
RAM. Let's load our ELF onto it.

```
(gdb) load target/target.elf
Loading section .data, size 0x968 lma 0x20000000
Loading section .init_array, size 0x4 lma 0x20000968
Loading section .fini_array, size 0x4 lma 0x2000096c
Loading section .jcr, size 0x4 lma 0x20000970
Loading section .isr_vector, size 0xc4 lma 0x8000000
Loading section .eh_frame, size 0x4 lma 0x80000c4
Loading section .ARM.exidx, size 0x8 lma 0x80000c8
Loading section .text, size 0x25dc lma 0x80000d0
Loading section .init, size 0x18 lma 0x80026ac
Loading section .fini, size 0x18 lma 0x80026c4
Start address 0x8000324, load size 12368
Transfer rate: 18 KB/sec, 1236 bytes/write.
```

Looking at the memory map of the MCU shows that these addresses seem reasonable: SRAM is at 0x2000_0000, and flash is at 0x0800_0000. We now give the chip a reset and run the code:

```

(gdb) monitor reset halt
target state: halted
target halted due to debug-request, current mode: Thread
xPSR: 0xc1000000 pc: 0x08000324 msp: 0x20001800
(gdb) c
Continuing.
^C
Program received signal SIGINT, Interrupt.
0x08000374 in ?? ()
(gdb) bt
#0  0x08000374 in ?? ()
#1  <signal handler called>
#2  0x08002684 in ?? ()
#3  0x08000356 in ?? ()
Backtrace stopped: previous frame identical to this frame (corrupt stack?)
(gdb)
```

This... doesn't look good. The memory addresses on the stack don't map
to symbols. And that "signal handler called" line isn't great
either. Let's try to figure out what those stack entries are first,
and then figure out what the signal handler is all about.

Objdump is a good place to start:

```
build-target/target$ ~/x-tools/arm-unknown-eabi/bin/arm-unknown-eabi-objdump -t target.elf
```

This dumps the whole symbol table and the addresses where the symbols
are located. Immediately, a bunch of things jump out at me:

```
08000374  w    F .text  00000002 TIM1_BRK_UP_TRG_COM_IRQHandler
08000374  w    F .text  00000002 WWDG_IRQHandler
08000374  w    F .text  00000002 USART1_IRQHandler
etc
```

These are all interrupt handlers, and they're all located at the same
address? Why? In the `startup_stm32f0xx.s` startup file, there's a
default handler defined called `Default_Handler`, which is done with a
"weak link". This means that if we don't define our own function
called, e.g. `USART1_IRQHandler`, then the symbol table will just
point `USART1_IRQHandler` at `Default_Handler`. Looking at the source
of `Default_Handler` shows that it just goes into an infinite loop
(reasonable behaviour for an unhandled interrupt).

Ok, so we know we're hitting an unhandled interrupt and that's why
it's getting into the signal handler. But what's causing this
interrupt? We haven't turned any interrupts on yet!

First place to look: the datasheet. How can we tell which interrupt was triggered? A good place to start would be the fault interrupts: NMI and HardFault.

By defining our own functions for `NMI_Handler` and
`HardFault_Handler` and re-building and running the code, the handler address on the stack has changed!

```
(gdb) c
Continuing.
^C
Program received signal SIGINT, Interrupt.
0x080005c4 in ?? ()
(gdb) bt
#0  0x080005c4 in ?? ()
#1  <signal handler called>
#2  0xffffffff in ?? ()
#3  0x00000000 in ?? ()
(gdb)
```

Now, to once again look up that address in objdump. The closest I found was:

```
080005c0 g     F .text  00000006 HardFault_Handler
```

Seems like a good place to keep investigating. Why would there be a
hard fault? Well, it seems problematic that there's 0xffff_ffff and
0x0000_0000 on the stack.

Let's try single stepping through. This is the gdb command "stepi", and
changing to "layout asm" should let us see everything. Also, why the heck aren't there any symbols? (Note: it was because I hadn't loaded the file into gdb)

"file target.elf"
"load"

There we go, the stack makes a lot more sense now:

```
(gdb) c
Continuing.

Program received signal SIGINT, Interrupt.
0x080005c4 in HardFault_Handler ()
(gdb) bt
#0  0x080005c4 in HardFault_Handler ()
#1  <signal handler called>
#2  0x080026a0 in ____libc_init_array_from_thumb ()
#3  0x08000356 in LoopFillZerobss ()
Backtrace stopped: previous frame identical to this frame (corrupt stack?)
(gdb)
```

Something in the ____libc_init_array_from_thumb function is failing -
this is in the `newlib` that comes included with my cross-compiled
gcc. My first guess is that it's looking for a linker section that I
didn't define.

Looking closer at the newlib source (crt0.S in
`.build/src/newlib-2.2.0/newlib/libc/sys/arm`) shows that it too is
going to be initializing the BSS etc. So why are we crashing in here?
The line in gdb says `<____libc_init_array_from_thumb+4> str.w lr,
[r0, <undefined>]`. What is that undefined?

This also sticks out from `crt0.S`:

```
#ifdef HAVE_INITFINI_ARRAY
#define _init   __libc_init_array
#define _fini   __libc_fini_array
#endif
```

I think there might be some stuff missing from the linker script?

I spent a while hacking away on this, and it's time to go back to the
source and figure out what exactly I need here.

If I use my simple linker script, I get the following undefined pieces:

```
build-target$ ~/x-tools/arm-unknown-eabi/bin/arm-unknown-eabi-objdump -t target/target.elf | grep -i und
00000000  w      *UND*  00000000 __fini_array_end
00000000  w      *UND*  00000000 malloc
00000000  w      *UND*  00000000 __deregister_frame_info
00000000  w      *UND*  00000000 _ITM_registerTMCloneTable
00000000  w      *UND*  00000000 _ITM_deregisterTMCloneTable
00000000  w      *UND*  00000000 __fini_array_start
00000000  w      *UND*  00000000 __init_array_end
00000000  w      *UND*  00000000 __preinit_array_end
00000000  w      *UND*  00000000 __init_array_start
00000000  w      *UND*  00000000 _Jv_RegisterClasses
00000000  w      *UND*  00000000 __preinit_array_start
00000000  w      *UND*  00000000 __register_frame_info
00000000  w      *UND*  00000000 free
```

Those seem important, and missing from my linker script.
http://sushihangover.github.io/arm-cortex-m3-bare-metal-with-newlib/

What do I get if I don't use my linker script? Without my linker script, I get undefined references to:

- _sidata
- _sdata
- _sbss
- _ebss
- _estack

These are all in the startup file. Can these be fixed to point to the
symbols in the stock linker script instead? Adding "-Wl,-verbose" to
the linker command dumps out the linker script it's using.

It's big.

http://www.bravegnu.org/gnu-eprog/data-in-ram.html

Ok, so this is progress. Now I'm stuck at an invalid instruction in libc_init_array_from_thumb?!?
