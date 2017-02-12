
# What is this?

This is a sample project for the STM32F042K6 Nucleo board that I'm
happy with. It has the following features:

- CMake for builds
- crosstool-ng for building a toolchain
- Unity for unit testing on the host
- OpenOCD and gdb for running code on the target

# How do I get started?

## Toolchain

First, you need to have [crosstool-ng](http://crosstool-ng.org/)
installed. I'm using version
[1.22.0](http://crosstool-ng.org/download/crosstool-ng/crosstool-ng-1.22.0.tar.bz2). The
instructions are pretty straight-forward. Once you've got it
installed, don't worry about the rest of their instructions (i.e. stop
after the `export PATH` line).

Once it's installed, you can use the configuration file in the
`crosstool/` directory in this repository to build a compiler and
newlib (libc) for the microcontroller. To do this, you first tell
crosstool-ng to use the configuration provided, and then you tell it
to build it:

```
stm32f042-morse-example/crosstool$ DEFCONFIG=stm32f0-defconfig ct-ng defconfig
stm32f042-morse-example/crosstool$ ct-ng build
```

This will take a while. On my i7 desktop, it took about 16  minutes.

This will install the toolchain into `~/x-tools/arm-stm32f0-eabi`.

## OpenOCD

The crosstool-ng toolchain comes with an ARM-compatible gdb, but
without OpenOCD, there's no way to connect to the target board.

I was lazy here and just installed it through apt: `apt install -y openocd`

To test that it's working, plug your Nucleo board into a USB port and run this command. You should get similar output:

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

## CMake

CMake is a build system. The two-second intro is that instead of
having a Makefile, you have files called `CMakeLists.txt` that are
used to generate Makefiles (and other things, in different
environments).

Here, too, I was lazy and just installed it via apt: `sudo apt install -y cmake`

## Building and testing on the host

You should now have all the pieces in place necessary to do both
host-unit-test builds and target-binary builds!

First, let's do a host build. For CMake projects, you generally make a
new directory to do the build in, which keeps all of the build objects
out of your source tree (makes for really easy cleanup!)

To do a host build, make a directory called `build/` in the source
directory. The `.gitignore` file is already set up to ignore this
directory. In there, you run CMake to process the `CMakeLists.txt` files and generate a Makefile. Then you run `make`.

```
stm32f042-morse-example$ mkdir build
stm32f042-morse-example$ cd build
stm32f042-morse-example/build$ cmake ..
-- The C compiler identification is GNU 5.4.0
-- The ASM compiler identification is GNU
-- Found assembler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: /home/aja042/workspace/crosstool/stm32f042-morse-example/build
stm32f042-morse-example/build$ make
... lots of output ...
```

Once this has finished, there's a unit test runner stored in
`build/test/testrunner`. Executing this should result in output like
this:

```
stm32f042-morse-example/build$ test/testrunner
Unity test run 1 of 1
..............

-----------------------
14 Tests 0 Failures 0 Ignored
OK
```

## Building and flashing onto the microcontroller

Now that you can see that the unit tests are passing, it's time to try
running the code on the microcontroller!

We're going to make a new build directory for this; this one will
contain all of the ARM-compiled pieces instead of the x86-compiled
pieces from above. The process here is pretty similar, but we're going
to pass an additional parameter to CMake to tell it what to use for a
cross-compilation toolchain. We'll also tell CMake to do a Debug
build, so that we can easily see what's going on in GDB.

```
stm32f042-morse-example$ mkdir build-target
stm32f042-morse-example$ cd build-target
stm32f042-morse-example/build-target$ cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-stm32f042.cmake -DCMAKE_BUILD_TYPE=Debug ..
stm32f042-morse-example/build-target$ make
```

The output from this is going to be `target/target.elf`. This file
contains all of the code that's going to go onto the microcontroller,
plus additional things like debugging symbols.

To do these steps, make sure that you've got OpenOCD running (see
above). We're going to use `gdb` to connect to OpenOCD and upload the
firmware. I'll talk about what is happening with each step as we go
along.

Start the ARM version of gdb:
```
stm32f042-morse-example/build-target$ ~/x-tools/arm-stm32f0-eabi/bin/arm-stm32f0-eabi-gdb
```

Tell GDB to connect to OpenOCD. OpenOCD by defaults listens on port
4444 for general commands, and port 3333 for GDB commands:

```
(gdb) target remote localhost:3333
Remote debugging using localhost:3333
0x00000000 in ?? ()
```

Tell GDB which file has the firmware:

```
(gdb) file target/target.elf
A program is being debugged already.
Are you sure you want to change the file? (y or n) y
Reading symbols from target/target.elf...done.
```

And tell GDB to tell OpenOCD to flash that firmware onto the chip:

```
(gdb) load
Loading section .isr_vector, size 0xc4 lma 0x8000000
Loading section .ARM.exidx, size 0x8 lma 0x80000c4
Loading section .text, size 0xbcc lma 0x80000cc
Loading section .data, size 0x4c8 lma 0x8000c98
Start address 0x8000364, load size 4448
Transfer rate: 13 KB/sec, 1112 bytes/write.
```

Now tell GDB to tell OpenOCD to reset the chip and hold it in the halt state:

```
(gdb) monitor reset halt
target state: halted
target halted due to debug-request, current mode: Thread
xPSR: 0xc1000000 pc: 0x08000364 msp: 0x20001800
```

At this point, the firmware's loaded on the chip. To get it to run,
all we need to do is issue the continue command:

```
(gdb) continue
```

And if you look over at the board, the LED should now be blinking out
"HELLO WORLD" in Morse Code.
