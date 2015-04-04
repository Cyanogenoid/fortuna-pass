#
#             LUFA Library
#     Copyright (C) Dean Camera, 2014.
#
#  dean [at] fourwalledcubicle [dot] com
#           www.lufa-lib.org
#
# --------------------------------------
#         LUFA Project Makefile.
# --------------------------------------

# Run "make help" for target help.

MCU          = at90usb1286
ARCH         = AVR8
BOARD        = LaFortuna
F_CPU        = 8000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
TARGET       = main
OS_SRC       = os.c $(wildcard rios/*.c) $(wildcard ruota/*.c) $(wildcard fatfs/*.c) $(wildcard lcd/*.c) $(wildcard kb/*.c)
SRC          = $(TARGET).c $(LUFA_SRC_USB) $(OS_SRC)
LUFA_PATH    = kb/lufa/LUFA
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -IConfig/ -Wno-main
LD_FLAGS     =

# Default target
all:

upload: main.hex
	dfu-programmer $(MCU) erase
	dfu-programmer $(MCU) flash main.hex

# Include LUFA build script makefiles
include $(LUFA_PATH)/Build/lufa_core.mk
include $(LUFA_PATH)/Build/lufa_sources.mk
include $(LUFA_PATH)/Build/lufa_build.mk
include $(LUFA_PATH)/Build/lufa_cppcheck.mk
include $(LUFA_PATH)/Build/lufa_doxygen.mk
include $(LUFA_PATH)/Build/lufa_dfu.mk
include $(LUFA_PATH)/Build/lufa_hid.mk
include $(LUFA_PATH)/Build/lufa_avrdude.mk
include $(LUFA_PATH)/Build/lufa_atprogram.mk

.PHONY: upload
