#
# CYANCORE LICENSE
# Copyrights (C) 2023, Cyancore Team
#
# File Name		: build.mk
# Description		: This file builds worker sources
# Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
# Organisation		: Cyancore Core-Team
#

VWORKER_DIR	:= $(GET_PATH)

include $(VWORKER_DIR)/vcall/build.mk

DIR		:= $(VWORKER_DIR)

include mk/obj.mk
