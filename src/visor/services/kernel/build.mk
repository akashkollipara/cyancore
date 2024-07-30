#
# CYANCORE LICENSE
# Copyrights (C) 2024, Cyancore Team
#
# File Name		: build.mk
# Description		: This file accumulates the build scripts
#			  from various kernel
# Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
# Organisation		: Cyancore Core-Team
#

KERNEL_DIR	:= $(GET_PATH)

$(eval $(call check_and_include,KHELIOS,$(KERNEL_DIR)/helios/build.mk))
