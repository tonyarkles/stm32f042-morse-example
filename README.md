
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


