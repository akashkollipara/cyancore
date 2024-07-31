#
# CYANCORE LICENSE
# Copyrights (C) 2019, Cyancore Team
#
# File Name		: makefile
# Description		: Cyancore framework top level makefile
# Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
# Organisation		: Cyancore Core-Team
#

#*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*#
#------< Cyancore Top Level Makefile >------#
#*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*#

SHELL		:= /bin/bash
N_JOBS		?= $(shell grep -c ^processor /proc/cpuinfo)
HOST_ARCH	:= $(shell lscpu | grep -Po '(?<='Architecture:'        )[^"]*')
EN_BUILD_CACHE	?= 1
V		?= 0
PP		?= 0

include mk/help.mk
include mk/path.mk
include mk/ccache.mk
include mk/tc_get.mk
include mk/project.mk

ifneq ($(V),1)
.SILENT:
endif
