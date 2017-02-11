
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

# CMake

The first goal is to get CMake to build both host-based tests and
target-ready binaries. 