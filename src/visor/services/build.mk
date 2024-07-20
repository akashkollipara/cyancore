#
# CYANCORE LICENSE
# Copyrights (C) 2024, Cyancore Team
#
# File Name		: build.mk
# Description		: This file accumulates the build scripts
#			  from visor services
# Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
# Organisation		: Cyancore Core-Team
#

V_SERVICES	:= $(GET_PATH)

include $(V_SERVICES)/bootstrap/build.mk
include $(V_SERVICES)/kernel/build.mk

DIR		:= $(V_SERVICES)
include mk/obj.mk
