#
# CYANCORE LICENSE
# Copyrights (C) 2019, Cyancore Team
#
# File Name		: build.mk
# Description		: This file accumulates the build scripts from 
#			  all the platform directories
# Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
# Organisation		: Cyancore Core-Team
#

PLAT_DIR	:= $(GET_PATH)
FAM_PATH	:= $(PLAT_DIR)/$(FAMILY)
PLAT_PATH	:= $(FAM_PATH)/$(PLATFORM)

ifeq ($(realpath $(FAM_PATH)),)
$(info < ! > Family of chip used in project is not suppoted !)
$(error < x > Build Failed !)
endif
ifeq ($(realpath $(PLAT_PATH)),)
$(info < ! > Platform for project is not supported !)
$(error < x > Build Failed !)
endif

include $(PLAT_DIR)/$(FAMILY)/$(PLATFORM)/build.mk
