# barrier

The barrier module implements a semaphore barrier,
an extension of mutual exclusion to N threads.

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

In the barrier directory (the current directory as this readme.txt file),
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

1. Connect your NUCLEO-L452RE board to your computer and upload the barrier.hex
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

5. Watch the output of the application as multiple threads
encounter the barrier.
