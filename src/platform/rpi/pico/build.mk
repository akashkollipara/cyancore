#
# CYANCORE LICENSE
# Copyrights (C) 2024, Cyancore Team
#
# File Name		: build.mk
# Description		: This file accumulates the build scripts from 
#			  all other directories that have RPi-pico
#			  board support sources
# Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
# Organisation		: Cyancore Core-Team
#

PICO_DIR	:= $(GET_PATH)

RP2_FLASH	:= w25q128

include $(PICO_DIR)/../rp2040/build.mk
