
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

