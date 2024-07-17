#
# CYANCORE LICENSE
# Copyrights (C) 2024, Cyancore Team
#
# File Name		: build.mk
# Description		: Build script for this directory.
# Primary Authod	: Debadarshana parida [debadarshanaparida@yahoo.com]
# Organisation		: Cyancore Core-Team
#

ARM32_R_H_ARCH_DIR	:= $(GET_PATH)

$(eval $(call add_include,$(ARM32_R_H_ARCH_DIR)/include/))

DIR		:= $(ARM32_R_H_ARCH_DIR)
include mk/obj.mk
