# S22_yellow

This is the repo for YELLOW TEAM, in the spring 2022 offering of ECE 4140.
The descriptions of each milestone are provided below.

# Targeted Hardware

## Microcontroller
### * [STM32 NUCLEO-L452RE](https://www.google.com/search?q=stm32l452re&rlz=1C1GCEJ_enUS1015US1015&sxsrf=ALiCzsYT0IN-m--FPnoLr7DykOAUSFsesg:1663939446161&source=lnms&tbm=shop&sa=X&ved=2ahUKEwidxfa8gav6AhUelGoFHRiFAycQ_AUoA3oECAIQBQ&biw=1163&bih=525&dpr=1.65#spd=16585791561760532459)

![nucleo_l452re_p](https://user-images.githubusercontent.com/72426318/191967689-47bd9b08-301d-4c1a-b54f-aab56c2f5262.jpg)
![nucleoPic2](https://user-images.githubusercontent.com/72426318/191967463-deff9c6a-f478-46a4-ace1-d1c8ee0e0e32.png)

## Peripheral Target Board
### * EduBase-V2 (Not commercially available)
![EduBase_v2](https://user-images.githubusercontent.com/72426318/191968701-61e1ce15-66b0-4373-ac74-bf4bd5b09417.png)

## Instructions for Building

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