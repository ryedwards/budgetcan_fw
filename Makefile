##########################################################################################################################
# File automatically-generated by tool: [projectgenerator] version: [3.15.2] date: [Wed Nov 02 23:21:20 EDT 2022]
##########################################################################################################################

# ------------------------------------------------
# Generic Makefile (based on gcc)
#
# ChangeLog :
#	2017-02-10 - Several enhancements + project update mode
#   2015-07-22 - first version
# ------------------------------------------------

######################################
# target
######################################
TARGET = candleLight_fw


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

######################################
# source
######################################
# C sources
C_SOURCES =  \
libs/STM32_HAL/src/cmsis/system_stm32f0xx.c \
libs/STM32_HAL/src/stm32f0xx/stm32f0xx_hal.c \
libs/STM32_HAL/src/stm32f0xx/stm32f0xx_hal_can.c \
libs/STM32_HAL/src/stm32f0xx/stm32f0xx_hal_cortex.c \
libs/STM32_HAL/src/stm32f0xx/stm32f0xx_hal_flash.c \
libs/STM32_HAL/src/stm32f0xx/stm32f0xx_hal_flash_ex.c \
libs/STM32_HAL/src/stm32f0xx/stm32f0xx_hal_gpio.c \
libs/STM32_HAL/src/stm32f0xx/stm32f0xx_hal_pcd.c \
libs/STM32_HAL/src/stm32f0xx/stm32f0xx_hal_pcd_ex.c \
libs/STM32_HAL/src/stm32f0xx/stm32f0xx_hal_rcc.c \
libs/STM32_HAL/src/stm32f0xx/stm32f0xx_hal_rcc_ex.c \
libs/STM32_HAL/src/stm32f0xx/stm32f0xx_hal_tim.c \
libs/STM32_HAL/src/stm32f0xx/stm32f0xx_hal_tim_ex.c \
libs/STM32_HAL/src/stm32f0xx/stm32f0xx_ll_usb.c \
libs/STM32_USB_Device_Library/Core/Src/usbd_core.c \
libs/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
libs/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c \
src/can.c \
src/dfu.c \
src/flash.c \
src/gpio.c \
src/interrupts.c \
src/led.c \
src/main.c \
src/queue.c \
src/startup.c \
src/timer.c \
src/usbd_conf.c \
src/usbd_desc.c \
src/usbd_gs_can.c \
src/util.c 


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-

CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size

HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m0 -std=gnu11 -g

# fpu
# NONE for Cortex-M0/M0+/M3

# float-abi


# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DBOARD_canable \
-DSTM32F0 \
-DSTM32F042x6


# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-Iinclude \
-Ilibs/STM32_HAL/config \
-Ilibs/STM32_HAL/include/cmsis \
-Ilibs/STM32_HAL/include/stm32f0xx \
-Ilibs/STM32_USB_Device_Library/config \
-Ilibs/STM32_USB_Device_Library/Core/Inc \
-Ilibs/STM32_HAL/include/cmsis/device \
-Ilibs/STM32_HAL/include/stm32f0xx/Legacy \
-Ilibs/STM32_HAL/include \
-Ibuild



# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections -fstack-usage

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections -fstack-usage

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" --specs=nano.specs -mfloat-abi=soft


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = ldscripts/STM32F042X6.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

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

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
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
	-rm -fR $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***