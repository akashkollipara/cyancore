#
# CYANCORE LICENSE
# Copyrights (C) 2024, Cyancore Team
#
# File Name		: mk_helper.mk
# Description		: This file defines make helper utilities
# Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
# Organisation		: Cyancore Core-Team
#

#*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.#
#-----------< Helpers >------------#
#*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.#

define add_define
	CFLAGS	+= -D$(1)=$($(1))
endef

define add_include
	INCLUDE	+= $(1)
endef

define add_cflag
	CFLAGS	+= $(1)
endef

define add_ldflag
	LD_FLAGS	+= $(1)
endef

define check_and_include
ifeq ($($(1)),1)
include $(2)
endif
endef

define get_tc_version
	TC_VER	:= $(firstword $(strip $(shell $(1)-gcc --version | grep -m 1 -Po '\s(\d{1,}\.\d{1,}\.\d{1,})')))
endef

define check_config_and_include
ifeq ($(realpath $(1)),)
$$(info < ! > Invalid $(2) ...)
$$(error < x > Stopping Build !)
endif
include $(realpath $(1))
endef
