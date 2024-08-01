#
# CYANCORE LICENSE
# Copyrights (C) 2019, Cyancore Team
#
# File Name		: path.mk
# Description		: This file defines standard variables used
#			  in cyacnore build system
# Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
# Organisation		: Cyancore Core-Team
#

#*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.#
#------------< Paths >-------------#
#*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.#

CC_ROOT		:= .
TOOLS_ROOT	?= $(abspath $(CC_ROOT))/tools
MISC_TOOLS	:= $(TOOLS_ROOT)/misc_bins

SRC		:= $(CC_ROOT)/src
OUT_PATH	?= $(CC_ROOT)/out
OUT		?= $(OUT_PATH)/$(PROJECT)

GET_PATH	= $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

%/:
	mkdir -p $@
