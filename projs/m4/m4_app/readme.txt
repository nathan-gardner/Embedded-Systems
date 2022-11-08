# m4_app

The m4_app module encodes and decodes text provided via the PuTTY serial
monitor using the Vigenere cipher algorithm on the NUCLEO-L452RE
microcontroller board. The UART is set to 57.6k/8/N/1 parameters.
The Vigenere cipher key is "TENNESSEETECH". The blue button (B1)
on the L452RE is used to toggle between the encipher (B1 unpressed)
and decipher (B1 pressed) settings.

The LED on L452RE (LD2) will blink as the application runs.
The LED blink period in ms will be displayed on the screen if "!!!L"
is entered into the serial terminal (i.e., if the LED period is
1000 ms, then "1000" will appear on the screen).
The user can change the LED blink period by entering
"!!!Snnnn" in which "nnnn" is a 4-digit number representing
the new blink period in ms (i.e. "!!!S1528" will change
the blink period to 1528 ms).

This application utilizes the ESOS32 operating system developed
by Dr. J.W. Bruce for the ECE 4140/5140 Embedded Systems Design
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

In the m4_app directory (the current directory as this readme.txt file),
enter the following command in the Terminal
to build the application:
```bash
make
```
You should find the .hex file that you can upload onto your NUCLEO-L452RE
in the build directory.

To clear all application files in the build directory, enter the following
command in the Terminal in the m4_app directory:
```bash
make clean
```

## Instructions

1. Connect your NUCLEO-L452RE board to your computer and upload the m4_app.hex
onto the NUCLEO-L452RE using the STM32CubeProgrammer application.

2. Enter the following command into your Terminal to open PuTTY:
```bash
putty
```

3. When the PuTTY Configurations page opens, set PuTTY to serial
with a baud rate of 57600 and the serial line to the directory
in which your NUCLEO-L452RE is attached (for Linux, it most
likely is /dev/ttyACM0). Make sure that in Serial page under SSH,
the serial line is configured to 8 data bits, no parity,
1 stop bit, and no parity or flow control. In the Terminal page,
make sure that the Force under Local Echo and Local Line Editing
is Auto or Off; do NOT turn on the force.

4. Once PuTTY has been set to 57.6k/8/N/1, press "Open" on the
PuTTY configurations page. Press the black reset button
on the NUCLEO-L452RE to restart your application.

5. Type in characters into the serial monitor. All
letters will be capitalized before encryption/decryption, and
non-alphabetic letters will not be encrypted/decrypted.
The user can perform one of the four following actions:

a) If the blue button (B1) on L452RE is unpressed, the characters
will be encrypted using the Vigenere cipher
(i.e. "AAAAAAAAAAAAA" -> "TENNESSEETECH").

b) If B1 is pressed, the characters will be decrypted
using the Vigenere cipher
(i.e. "TENNESSEETECH" -> "AAAAAAAAAAAAA").

c) Enter "!!!L" to display the length of the blink period of the
LED (LD2) on the L452RE in milliseconds
(i.e. "!!!L" -> "1000" for 1000 ms).

d) Enter "!!!Snnnn" in which "nnnn" is a 4-digit number between
0001 and 9999 (include the leading zeros) to set the
LD2 blink period to "nnnn" I.e. "!!!S2022" will set
LD2 to blink with a period of 2022 ms at a 50% duty cycle).
