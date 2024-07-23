#
# CYANCORE LICENSE
# Copyrights (C) 2023, Cyancore Team
#
# File Name		: build.mk
# Description		: This file accumulates the build scripts from
#			  all other directories that have RPi-pico RP2040
#			  board support sources
# Primary Author	: Mayuri Lokhande [mayurilokhande01@gmail.com]
# Organisation		: Cyancore Core-Team
#

AN536_DIR	:= $(GET_PATH)

ARCH		:= arm-r
BIT		:= 32
ARCH_VARIANT	:= AN536
TL_TYPE		:= thumb/v8-r
TARGET_FLAGS	+= -mcpu=cortex-r52 -mfloat-abi=softfp
#PLAT_INCLUDE	+= $(RP2040_DIR)/include
OUTPUT_FORMAT	:= elf32-littlearm

#include $(RP2040_DIR)/config.mk
#include $(RP2040_DIR)/resources/build.mk
include $(AN536_DIR)/../common_rp2/build.mk
