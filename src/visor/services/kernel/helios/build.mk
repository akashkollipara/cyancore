#
# CYANCORE LICENSE
# Copyrights (C) 2019-2023, Cyancore Team
#
# File Name		: build.mk
# Description		: This file consists of HELIOS kernal configuration
# Primary Author	: Pranjal Chanda[pranjalchanda08@gmail.com]
# Organisation		: Cyancore Core-Team
#

DIR		:= $(GET_PATH)

HELIOS_IDLE_TASK_PRIORITY	:= 1
HELIOS_USE_DYNAMIC 		?= 1
HELIOS_HEAP_SIZE 		?= 1024
HELIOS_MAX_THREAD 		?= 2
HELIOS_IDLE_TASK_NAME		?= \"HELIOS_IDLE\"
HELIOS_IDLE_TASK_STACK_LEN 	?= 255
HELIOS_POWER_SAVE_EN 		?= 0
HELIOS_ANTI_DEADLOCK		?= 1
HELIOS_DEBUG			?= 1

$(eval $(call add_define,HELIOS_USE_DYNAMIC))
$(eval $(call add_define,HELIOS_HEAP_SIZE))
$(eval $(call add_define,HELIOS_MAX_THREAD))
$(eval $(call add_define,HELIOS_IDLE_TASK_NAME))
$(eval $(call add_define,HELIOS_IDLE_TASK_PRIORITY))
$(eval $(call add_define,HELIOS_IDLE_TASK_STACK_LEN))
$(eval $(call add_define,HELIOS_POWER_SAVE_EN))
$(eval $(call add_define,HELIOS_ANTI_DEADLOCK))
$(eval $(call add_define,HELIOS_DEBUG))

include mk/obj.mk
