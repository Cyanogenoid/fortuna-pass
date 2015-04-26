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
OS_SRC       = os.c
OS_SRC += $(wildcard rios/*.c)
OS_SRC += $(wildcard ruota/*.c)
OS_SRC += $(wildcard fatfs/*.c)
OS_SRC += $(wildcard lcd/*.c)
OS_SRC += $(wildcard kb/*.c)
OS_SRC += avr-crypto-lib/bigint/bigint.c
OS_SRC += avr-crypto-lib/rsa/rsa_basic.c
SRC          = $(TARGET).c $(LUFA_SRC_USB) $(OS_SRC)
LUFA_PATH    = kb/lufa/LUFA
CRYPTO_INC   = -Iavr-crypto-lib/rsa/ -Iavr-crypto-lib/test_src/ -Iavr-crypto-lib/bigint/
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -IConfig/ $(CRYPTO_INC) -Wno-main
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
