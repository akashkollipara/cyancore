#
# CYANCORE LICENSE
# Copyrights (C) 2024, Cyancore Team
#
# File Name		: build.mk
# Description		: This file builds timer driver and related sources.
# Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
# Organisation		: Cyancore Core-Team
#

SCHEDCLK_DIR	:= $(GET_PATH)

$(eval $(call check_config_and_include,$(SCHEDCLK_DIR)/schedclk_$(FAMILY)/build.mk,Family: $(FAMILY)))

DIR		:= $(SCHEDCLK_DIR)
include mk/obj.mk
