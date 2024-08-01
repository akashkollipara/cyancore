#
# CYANCORE LICENSE
# Copyrights (C) 2019, Cyancore Team
#
# File Name		: lobj.mk
# Description		: This file build sources to generate objects
#			  used to build library
# Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
# Organisation		: Cyancore Core-Team
#

#*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*#
#--------< Library Object Builder >---------#
#*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*#

C_SRCS		:= $(wildcard $(DIR)/*.c) $(filter %.c,$(Ex_SRCS))
CPP_SRCS	:= $(wildcard $(DIR)/*.cpp) $(filter %.cpp,$(Ex_SRCS))
S_SRCS		:= $(wildcard $(DIR)/*.S) $(filter %.S,$(Ex_SRCS))

C_OBJS		:= $(addprefix $(OUT)/,$(C_SRCS:.c=.o))
CPP_OBJS	:= $(addprefix $(OUT)/,$(CPP_SRCS:.cpp=.o))
S_OBJS		:= $(addprefix $(OUT)/,$(S_SRCS:.S=.o))

LIB_OBJS	+= $(C_OBJS) $(CPP_OBJS) $(S_OBJS)
DEP_SRCS	+= $(C_SRCS) $(CPP_SRCS)

.SECONDEXPANSION:
$(CPP_OBJS): $(OUT)/%.o: %.cpp | $$(@D)/
	@echo "Elf: Compiling $(@F:.o=.cpp) ..."
ifeq ($(PP),1)
	$(CCACHE) $(CCP) $(CPPFLAGS) $(CFLAGS) -E -p $< -o $(@:.o=.pre.cpp)
endif
	$(CCACHE) $(CCP) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(C_OBJS): $(OUT)/%.o: %.c | $$(@D)/
	@echo "Lib: Compiling $(@F:.o=.c) ..."
ifeq ($(PP),1)
	$(CCACHE) $(CC) $(CCFLAGS) $(CFLAGS) -E -p $< -o $(@:.o=.pre.c)
endif
	$(CCACHE) $(CC) $(CCFLAGS) $(CFLAGS) -c $< -o $@

$(S_OBJS): $(OUT)/%.o: %.S | $$(@D)/
	@echo "Lib: Assembling $(@F:.o=.S) ..."
	$(CCACHE) $(CC) -E $(CCFLAGS) $(CFLAGS) -c $< -o $(@:.o=.pre.S)
	$(CCACHE) $(AS) $(ASFLAGS) $(@:.o=.pre.S) -o $@
ifneq ($(PP),1)
	rm $(@:.o=.pre.S)
endif

Ex_SRCS		:=
