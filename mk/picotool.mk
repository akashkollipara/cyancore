#
# CYANCORE LICENSE
# Copyrights (C) 2022, Cyancore Team
#
# File Name		: picotool.mk
# Description		: This is a build script to generate picotool
# Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
# Organisation		: Cyancore Core-Team
#

PICO_SDK_GIT	:= https://github.com/raspberrypi/pico-sdk.git
PICO_TOOL_GIT	:= https://github.com/raspberrypi/picotool.git
PICO_SDK_PATH	:= $(TOOLS_ROOT)/pico-sdk/
PICO_TOOL_PATH	:= $(TOOLS_ROOT)/picotool/
PICO_TOOL	:= $(MISC_TOOLS)/picotool
PICO_TOOL_BUILD	:= $(PICO_TOOL_PATH)/build/picotool
ELF2UF2		:= $(MISC_TOOLS)/elf2uf2

ifneq ($(V),1)
SILENT_LOGS	:= > cbuild.log 2> /dev/null
PICOTOOL_QUIET	:= 2>/dev/null 1>/dev/null
endif

P_TARGETS	+= elf2uf2
T_ALLOWLIST	+= get_picotool clean_picotool

get_picotool: $(PICO_TOOL) $(ELF2UF2)

$(PICO_TOOL): $(PICO_TOOL_BUILD)
	mkdir -p $(dir $@)
	cp $(PICO_TOOL_PATH)/build/picotool $@

$(PICO_TOOL_BUILD): $(PICO_SDK_PATH) $(PICO_TOOL_PATH)
	@echo "< ! > Building picotool ..."
	cd $(PICO_TOOL_PATH) && mkdir -p build && \
	cd build && export PICO_SDK_PATH=$(PICO_SDK_PATH) && \
	cmake ../ $(SILENT_LOGS) \
	&& make $(SILENT_LOGS)
	@echo "< / > Done!"

$(ELF2UF2): $(PICO_SDK_PATH)
	@echo "< ! > Building elf2uf2 ..."
	mkdir -p $(MISC_TOOLS)/temp
	cd $(MISC_TOOLS)/temp; \
	cmake $(PICO_SDK_PATH)/tools/elf2uf2 $(PICOTOOL_QUIET); \
	make >/dev/null 2>/dev/null
	cp $(MISC_TOOLS)/temp/elf2uf2 $@
	rm -rf $(MISC_TOOLS)/temp
	@echo "< ! > Cleaning up build space ..."
	rm -rf $(PICO_SDK_PATH) $(PICO_TOOL_PATH)
	@echo "< / > Done!"

elf2uf2: $(ELF2UF2) elf
	@echo "Generating $(notdir $(ELF:.elf=.uf2)) ..."
	$(ELF2UF2) $(ELF) $(ELF:.elf=.uf2)
	@echo "< / > Done!"

$(PICO_SDK_PATH):
	@echo "< ! > Fetching PICO SDK ..."
	git clone $(PICO_SDK_GIT) --quiet $@
	@echo "< / > Done!"

$(PICO_TOOL_PATH):
	@echo "< ! > Fetching PICO Tool ..."
	git clone $(PICO_TOOL_GIT) --quiet $@
	@echo "< / > Done!"

clean_picotool:
	@echo "< ! > Removing picotools ..."
	rm -rf $(PICO_SDK_PATH) $(PICO_TOOL_PATH) $(PICO_TOOL) $(ELF2UF2)
	@echo "< / > Done!"
