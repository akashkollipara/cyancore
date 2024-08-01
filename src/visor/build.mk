#
# CYANCORE LICENSE
# Copyrights (C) 2019, Cyancore Team
#
# File Name		: build.mk
# Description		: This file accumulates the build scripts
#			  from visor directories
# Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
# Organisation		: Cyancore Core-Team
#

VISOR_DIR	:= $(GET_PATH)

include $(VISOR_DIR)/workers/build.mk
include $(VISOR_DIR)/services/build.mk
