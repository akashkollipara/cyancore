#
# CYANCORE LICENSE
# Copyrights (C) 2019, Cyancore Team
#
# File Name		: slib.mk
# Description		: This file accumulates all the object files
#			  and generates the shared library
# Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
# Organisation		: Cyancore Core-Team
#

#*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*#
#---------< Shared Library Builder >----------#
#*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*#

include mk/lobj.mk

SLIB		:= $(addprefix $(OUT)/,$(SLIB))
HEADER		:= $(addprefix $(LIB_INCLUDE_PATH)/,$(LIB_HEADER))

slib: $(SLIB)

$(SLIB): $(HEADER) $(LIB_OBJS)
	echo "Generating $(notdir $@) ..."
	$(CCACHE) $(LD) --shared -Wl,-soname,$(notdir $@) -o $@ $<

$(HEADER): --dependency
	cp $@ $(OUT)

LIB_INCLUDE_PATH:=
LIB_HEADER	:=
LIB_OBJS	:=
SLIB		:=
