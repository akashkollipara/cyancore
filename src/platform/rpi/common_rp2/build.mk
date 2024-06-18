#
# CYANCORE LICENSE
# Copyrights (C) 2023, Cyancore Team
#
# File Name		: build.mk
# Description		: This file accumulates sources from RP2
#			  common directory
# Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
# Organisation		: Cyancore Core-Team
#

RP2_COMMON_DIR	:= $(GET_PATH)

LD_SCRIPT		:= $(RP2_COMMON_DIR)/sections.ld.sx

PLAT_INCLUDE		+= $(RP2_COMMON_DIR)/include

LINT_FLAGS		+= --platform=unix32

USE_DEFAULT_RESOURCES	?= 1
LOCAL_INTERRUPT_DEVICE	?= 0
PLAT_INTERRUPT_DEVICE	?= 0

include $(RP2_COMMON_DIR)/arch/build.mk
include $(RP2_COMMON_DIR)/flash/build.mk
#include $(RP2_COMMON_DIR)/hal/build.mk
include $(RP2_COMMON_DIR)/platform/build.mk
