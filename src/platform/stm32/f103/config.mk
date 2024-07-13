#
# CYANCORE LICENSE
# Copyrights (C) 2019, Cyancore Team
#
# File Name		: config.mk
# Description		: This file defines configuration for STM32F108
# Primary Author	: Mohd Fahad [mohdfahad12328@gmail.com]
# Organisation		: Cyancore Core-Team
#

# TODO: only updated START and SIZE according to f103c8t6 only

#======================================================================
# Configuration file for Platforms
#======================================================================

#======================================================================
# Platform Configuration
# Do not alter below FLAGS unless explicitly mentioned
#======================================================================
N_CORES		:= 1
$(eval $(call add_define,N_CORES))

$(eval $(call add_define,BIT))

BOOT_CORE_ID:= 0
$(eval $(call add_define,BOOT_CORE_ID))

FLASH_START	:= 0x08000000
FLASH_SIZE	:= 0x10000	# 64K
RAM_START	:= 0x20000000
RAM_SIZE	:= 0x5000	# 20K
HEAP_SIZE	?= 256
STACK_SIZE	?= 256

$(eval $(call add_define,HEAP_SIZE))
$(eval $(call add_define,STACK_SIZE))

# Call this FLAG from Project config file if needed
FCLK		?= 16000000
$(eval $(call add_define,FCLK))

N_IRQ		:= 25
$(eval $(call add_define,N_IRQ))

MAX_INTERRUPTS_PER_DEVICE	:= 2
$(eval $(call add_define,MAX_INTERRUPTS_PER_DEVICE))

USE_SPINLOCK	?= 1
$(eval $(call add_define,USE_SPINLOCK))

#======================================================================
# MEMBUF Configuration
#======================================================================
MEMBUF_SIZE	?= 128
$(eval $(call add_define,MEMBUF_SIZE))
#======================================================================

#======================================================================
# GPIO Configuration
#======================================================================
N_PORT		:= 3
$(eval $(call add_define,N_PORT))
#======================================================================

#======================================================================
# ADC Configuration
#======================================================================
N_ADC_PINS		:= 8
$(eval $(call add_define,N_ADC_PINS))
#======================================================================
