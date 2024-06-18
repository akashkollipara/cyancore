#
# CYANCORE LICENSE
# Copyrights (C) 2024, Cyancore Team
#
# File Name		: build.mk
# Description		: This file verifies the Flash config and updates
#			  build parameters
# Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
# Organisation		: Cyancore Core-Team
#

RP2_FLASH_DIR	:= $(GET_PATH)

$(eval $(call check_config_and_include,$(RP2_FLASH_DIR)/config_$(RP2_FLASH).mk,Flash: $(RP2_FLASH)))

