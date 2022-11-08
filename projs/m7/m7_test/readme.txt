# m7_test

This test module tests various LCD service functions
of the ESOS32 operating system on the Hitachi HD44780
LCD display. This application uses the NUCLEO-L452RE
microcontroller board attached to the EduBase-V2-L452
peripheral target board where the LCD display resides on.
The ESOS32 operating system was developed by Dr. J.W. Bruce for
the ECE 4140/5140 Embedded Systems Design
course at Tennessee Technological University.


## Building

Before building the application, make sure the file paths for ESOS_DIR,
CM3_DIR, and GCC_PATH are set as shown below. ESOS_DIR is the
base location of the ESOS32 source tree on your hard disk drive (HDD).
CM3_DIR is the base location of the libOpenCM3 source tree on your HDD.
GCC_PATH is the bin directory of the GCC GNU compiler on your HDD.
Place the following code under the else statement in the section
that says "# ESOS, CM3, and GCC Paths" with your paths:
```bash
  ifeq ($(USER), awesomeeagle)
    ESOS_DIR = /home/awesomeeagle/Documents/GitHub/esos32
    CM3_DIR = /home/awesomeeagle/Documents/libopencm3/libopencm3
    GCC_PATH = /home/awesomeeagle/Documents/gcc-arm-none-eabi-10.3-2021.10/bin
  endif
```

In the m7_test directory (the current directory as this readme.txt file),
enter the following command in the Terminal
to build the application:
```bash
make
```
You should find the .hex file that you can upload onto your NUCLEO-L452RE
in the build directory.

To clear all application files in the build directory, enter the following
command in the Terminal in the m7_test directory:
```bash
make clean
```

## Instructions

1. Connect your NUCLEO-L452RE board to your computer and upload the m7_test.hex
onto the NUCLEO-L452RE using the STM32CubeProgrammer application.

2. Press the black Reset button on the NUCLEO-L452RE and watch the LCD
print text going through testing the following functions (which
all start with "esos_lcd44780_" which each mode on for 2-4 seconds:

a) clearScreen() - blank screen
b) writeChar() - writes message character by character
c) writeBuffer() - this one has a bug in which alternating characters are printed
in different rows (i.e. in the word "buffer," the "u", second "f", and "r" are seen
in row 0 than in row 1 with the rest of the word's letters). This could be a function issue?
d) setCursorDisplay() and setCursorBlink() - should go through three possibilities
for the cursor: blinking cursor, non-blinking cursor, then no cursor shown
e) setDisplayVisible() - no text should be printed for 2 s, then text will be
shown again afterwards
