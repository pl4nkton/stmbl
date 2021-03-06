# Optimization level, can be [0, 1, 2, 3, s].
#     0 = turn off optimization. s = optimize for size.
#
#OPT = -O2 -flto
OPT = -O2
# OPT = -O1         # for debugging

# Object files directory
# Warning: this will be removed by make clean!
#
OBJDIR = obj_f3dfu

# Target file name (without extension)
TARGET = $(OBJDIR)/f3dfu

# Define all C source files (dependencies are generated automatically)
INCDIRS += f3dfu/Inc
SOURCES += f3dfu/Src/main.c
SOURCES += f3dfu/Src/stm32f3xx_hal_msp.c
SOURCES += f3dfu/Src/stm32f3xx_it.c
SOURCES += f3dfu/Src/system_stm32f3xx.c
SOURCES += f3dfu/Src/usb_device.c
SOURCES += f3dfu/Src/usbd_conf.c
SOURCES += f3dfu/Src/usbd_desc.c
SOURCES += f3dfu/Src/usbd_dfu_if.c

INCDIRS += shared
#SOURCES += shared/crc8.c
#SOURCES += shared/common.c
#SOURCES += shared/angle.c
#SOURCES += shared/hal.c
#SOURCES += shared/hal_term.c

#USB Driver
USB_DEVICE_DIR = lib/STM32_USB_Device_Library-2.4.2

#USB class
INCDIRS += $(USB_DEVICE_DIR)/Class/DFU/Inc/
SOURCES += $(USB_DEVICE_DIR)/Class/DFU/Src/usbd_dfu.c

#USB core
INCDIRS += $(USB_DEVICE_DIR)/Core/Inc/
SOURCES += $(USB_DEVICE_DIR)/Core/Src/usbd_core.c
SOURCES += $(USB_DEVICE_DIR)/Core/Src/usbd_ctlreq.c
SOURCES += $(USB_DEVICE_DIR)/Core/Src/usbd_ioreq.c

#CMSIS
CPPFLAGS += -DSTM32F303xC
INCDIRS += lib/CMSIS/Include/
INCDIRS += lib/CMSIS/Device/ST/STM32F3xx/Include/
SOURCES += lib/CMSIS/Device/ST/STM32F3xx/Source/Templates/gcc/startup_stm32f303xc.s
#SOURCES += lib/CMSIS/Device/ST/STM32F3xx/Source/Templates/system_stm32f3xx.c

#stm32f3 HAL Driver
HAL_DRV_DIR = lib/STM32F3xx_HAL_Driver/

INCDIRS += $(HAL_DRV_DIR)/Inc
SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal.c
#SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_adc.c
#SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_adc_ex.c
SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_cortex.c
#SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_dac.c
#SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_dac_ex.c
#SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_dma.c
SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_flash.c
SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_flash_ex.c
SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_gpio.c
#SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_i2c.c
#SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_i2c_ex.c
#SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_opamp.c
#SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_opamp_ex.c
SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_pcd.c
SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_pcd_ex.c
SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_pwr.c
SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_pwr_ex.c
SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_rcc.c
SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_rcc_ex.c
#SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_tim.c
#SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_tim_ex.c
#SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_uart.c
#SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_uart_ex.c
SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_crc.c
SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_crc_ex.c
SOURCES += $(HAL_DRV_DIR)/Src/stm32f3xx_hal_rtc.c

LDSCRIPT = f3dfu/STM32F303CBTx_FLASH.ld

#============================================================================
OBJECTS += $(addprefix $(OBJDIR)/,$(addsuffix .o,$(basename $(SOURCES))))
CPPFLAGS += $(addprefix -I,$(INCDIRS))

#---------------- Preprocessor Options ----------------
#  -fsingle...    make better use of the single-precision FPU
#  -g             generate debugging information
#  -save-temps    preserve .s and .i-files
#
CPPFLAGS += -fsingle-precision-constant
CPPFLAGS += -g
# CPPFLAGS += -save-temps=obj

#---------------- C Compiler Options ----------------
#  -O*            optimization level
#  -f...          tuning, see GCC documentation
#  -Wall...       warning level
#
CFLAGS += $(OPT)
CFLAGS += -std=gnu11
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += -Wall
CFLAGS += -fno-builtin ## from old
CFLAGS += -nostartfiles
CFLAGS += -Wfatal-errors
#CFLAGS += -Wstrict-prototypes
#CFLAGS += -Wextra
#CFLAGS += -Wpointer-arith
#CFLAGS += -Winline
#CFLAGS += -Wunreachable-code
#CFLAGS += -Wundef

# Use a friendly C dialect
CPPFLAGS += -fno-strict-aliasing
CPPFLAGS += -fwrapv

#---------------- C++ Compiler Options ----------------
#
CXXFLAGS += $(OPT)
CXXFLAGS += -ffunction-sections
CXXFLAGS += -fdata-sections
CXXFLAGS += -Wall

#---------------- Assembler Options ----------------
#  -Wa,...    tell GCC to pass this to the assembler
#

#---------------- Linker Options ----------------
#  -Wl,...      tell GCC to pass this to linker
#  -Map         create map file
#  --cref       add cross reference to  map file
#
LDFLAGS += $(OPT)
LDFLAGS += -lm
LDFLAGS += -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += -Wl,--gc-sections

# LDFLAGS += -specs=nano.specs -u _printf_float -u _scanf_float
LDFLAGS += -T$(LDSCRIPT)

#============================================================================

POSTLD   = cp

# Compiler flags to generate dependency files
#
GENDEPFLAGS = -MMD -MP

# Combine all necessary flags and optional flags
# Add target processor to flags.
#
CPU = -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16

CFLAGS   += $(CPU)
CXXFLAGS += $(CPU)
ASFLAGS  += $(CPU)
LDFLAGS  += $(CPU)

# Default target
#
all: gccversion build showsize

build: elf hex bin lss sym

elf: $(TARGET).elf
hex: $(TARGET).hex
bin: $(TARGET).bin
lss: $(TARGET).lss
sym: $(TARGET).sym

# Display compiler version information
#
gccversion:
	@$(CC) --version

# Show the final program size
#
showsize: build
	@echo
	@$(SIZE) $(TARGET).elf 2>/dev/null

# Flash the device
#
btburn: $(TARGET).dfu
	@tools/bootloader.py
	@sleep 1
	@dfu-util -d 0483:df11 -a 0 -s 0x08000000:leave -D $(TARGET).dfu

flash: $(TARGET).bin
	st-flash --reset write $(TARGET).bin 0x08000000

# Create a DFU file from bin file
%.dfu: %.bin
	@cp $< $@
	@dfu-suffix -v 0483 -p df11 -a $@

# Target: clean project
#
clean:
	@echo Cleaning project:
	rm -rf $(OBJDIR)

# Include the base rules
#
include base.mak
include toolchain.mak

# Include the dependency files
#
-include $(OBJECTS:.o=.d)

# Listing of phony targets
#
.PHONY: all build flash clean \
        boot boot_clean boot_flash btburn boot_btflash boot_flash\
        elf lss sym \
        showsize gccversion
