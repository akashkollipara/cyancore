#
# CYANCORE LICENSE
# Copyrights (C) 2019-2022, Cyancore Team
#
# File Name		: build.mk
# Description		: This file build and gathers project properties
# Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
# Organisation		: Cyancore Core-Team
#

PROJECT_DIR	:= $(GET_PATH)

OPTIMIZATION	:= s

EXE_MODE	:= supervisor

#include $(PROJECT_DIR)/../demo.src/build.mk
include $(PROJECT_DIR)/config.mk

DIR		:= $(PROJECT_DIR)
include mk/obj.mk
