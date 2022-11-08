##########################################################################################################################
# Makefile    J.W.Bruce (C) 2020
##########################################################################################################################


########################################
# determine OS
########################################
ifeq ($(OS),Windows_NT)
    MAKECLEANCMD = del
    MAKECLEANFLAGS = /f
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
		MAKECLEANCMD = rm
		MAKECLEANFLAGS = -fR
    endif
    ifeq ($(UNAME_S),Darwin)
		MAKECLEANCMD = rm
		MAKECLEANFLAGS = -fR
    endif
endif


######################################
# target
######################################
all: $(TARGET)

TARGET = m7_test

######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og

#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

# ESOS, CM3, and GCC Paths

# Do not modify these paths
ifdef CPATH
  ESOS_DIR = /home/tyler/Documents/Github/esos32
  CM3_DIR = /home/tyler/Documents/Github/libopencm3
  GCC_PATH = /home/tyler/gcc-arm-none-eabi-10.3-2021.10/bin
else
  USER = $(shell whoami)

  ifeq ($(USER), blakemartin2014)
    ESOS_DIR = /home/blakemartin2014/Documents/GitHub/EmbeddedGithub/esos32-master
    CM3_DIR = /home/blakemartin2014/Documents/libopencm3/libopencm3
    GCC_PATH = /home/blakemartin2014/Documents/gcc-arm-none-eabi-10.3-2021.10/bin
  endif

  ifeq ($(USER), kester)
    ESOS_DIR = /home/kester/Documents/Github/EmbeddedGithub/esos32-master
    CM3_DIR = /home/kester/Documents/Github/libopencm3
    GCC_PATH = /home/kester/Documents/gcc-arm-none-eabi-10.3-2021.10/bin
  endif

  ifeq ($(USER), triston)
    ESOS_DIR = /home/triston/Nextcloud/Documents/TNTech/ECE4140/Labs/Lab4/EmbeddedGithub/esos32-master
    CM3_DIR = /home/triston/libopencm3
    GCC_PATH = /usr/bin/gcc-arm-none-eabi-10.3-2021.10/bin
  endif

  ifeq ($(USER), nathan)
    ESOS_DIR = /home/nathan/github/EmbeddedGithub/esos32-master
    CM3_DIR = /home/nathan/Documents/libopencm3/
    GCC_PATH = /home/nathan/Desktop/gcc-arm-none-eabi-10.3-2021.10/bin
  endif
endif

######################################
# source
######################################
# C sources
C_SOURCES =  \
generic_hw_src/esos_stm32l4_edub_sui.c \
src/m7_test.c \
$(ESOS_DIR)/src/esos_sui.c \
$(ESOS_DIR)/src/esos.c \
$(ESOS_DIR)/src/esos_cb.c \
$(ESOS_DIR)/src/esos_utils.c \
$(ESOS_DIR)/src/esos_comm.c \
$(ESOS_DIR)/src/esos_mail.c \
$(ESOS_DIR)/src/stm32l4_ocm3/esos_stm32l4_tick.c \
$(ESOS_DIR)/src/stm32l4_ocm3/esos_stm32l4_utils.c \
$(ESOS_DIR)/src/stm32l4_ocm3/esos_stm32l4_rs232.c \
$(ESOS_DIR)/src/esos_lcd44780wo.c \
$(ESOS_DIR)/src/stm32l4_ocm3/esos_stm32l4_edub_4bit_lcd44780wo.c \


# ASM sources
ASM_SOURCES =  \

#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS =

# C defines
C_DEFS =  \
-DSTM32L4						\
-DESOS_USE_LCD_4BIT \
-DSTM32L452xx  					\
-DHARDWARE_PLATFORM=NUCLEO64	\
-DESOS_USE_SERIAL_PORT			\
-D__NUCLEO_BOARD

# FOR THE APOLLO BOARD  -D__PTB_APOLLO_BOARD

# AS includes
AS_INCLUDES =

# C includes
C_INCLUDES =  \
-Iinclude \
-IMyESOS \
-I$(ESOS_DIR)/include \
-I$(ESOS_DIR)/include/stm32l4_ocm3 \
-I$(CM3_DIR)/include \
-L$(CM3_DIR)/lib

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections -nostartfiles -specs=nosys.specs

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = ld.stm32.basic

# libraries
LIBS = -lc -lm -lnosys -lopencm3_stm32l4
LIBDIR = -L$(CM3_DIR)/lib
LDFLAGS = $(MCU) -specs=nano.specs -specs=nosys.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections -nostartfiles

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))


$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile $(CM3_DIR)/lib/libopencm3_stm32l4.a
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@

#######################################
# clean up
#######################################
clean:
	-$(MAKECLEANCMD) $(MAKECLEANFLAGS) $(BUILD_DIR)


#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

flash: all | $(BUILD_DIR)
	st-flash erase
	st-flash write $(BUILD_DIR)/$(TARGET).bin 0x08000000

# *** EOF ***
