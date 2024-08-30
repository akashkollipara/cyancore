#
# CYANCORE LICENSE
# Copyrights (C) 2019, Cyancore Team
#
# File Name		: tc_get.mk
# Description		: This file helps developer get toolchains
# Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
# Organisation		: Cyancore Core-Team
#

include mk/path.mk

T_ALLOWLIST	+= get_all_tc get_avr_tc get_arm_tc get_riscv_tc

OSDEPPKGS	:= build-essential pkg-config libusb-1.0-0-dev cmake cppcheck ccache
OSDEPPKGS	+= tree ninja-build libpixman-1-dev libcairo2-dev libpango1.0-dev
OSDEPPKGS	+= libjpeg8-dev libgif-dev libglib2.0-dev libgcrypt20-dev python3-venv

# GIT REPO RECOMMENDED
# Provide git repo path for toolchains for better experience
ESIZE_REPO	:= https://github.com/VisorFolks/cc_elf_size.git
AVR_TC_REPO	?= https://github.com/VisorFolks/avr-toolchain
RISC_V_TC_REPO	?= https://github.com/VisorFolks/risc-v-toolchain
ARM_TC_REPO	?= https://github.com/VisorFolks/arm-toolchain

get_all_tc: get_avr_tc get_arm_tc get_riscv_tc

--install_os_pkgs:
	@echo "< ! > Installing workspace dependencies, it may take a while ..."
	sudo apt-get install $(OSDEPPKGS) -y -qq > /dev/null
	@echo "< / > Done!"

SIZE	:= $(TOOLS_ROOT)/cc_elf_size/size

.SECONDEXPANSION:
$(SIZE): | $$(@D)/
	$(info < ! > Fetching ELF-Size utility ...)
	cd $(TOOLS_ROOT); git clone $(ESIZE_REPO) --quiet;
	$(MAKE) -C $(@D)
	echo "< / > Done !"

get_arm_tc: $(TOOLS_ROOT)/arm-toolchain/bin/arm-none-eabi-gcc
$(TOOLS_ROOT)/arm-toolchain/bin/arm-none-eabi-gcc:
ifeq ($(ARM_TC_REPO),)
	$(error < x > Please provide valid ARM Toolchain git link)
endif
	$(info < ! > Fetching ARM Toolchain ...)
	$(info < ? > Please be patient as this might take a while ...)
	mkdir -p $(TOOLS_ROOT)
	cd $(TOOLS_ROOT); git clone $(ARM_TC_REPO) arm-toolchain; chmod +x -R $(@D)/..
	echo "< / > Done !"

get_avr_tc: $(TOOLS_ROOT)/avr-toolchain/bin/avr-gcc
$(TOOLS_ROOT)/avr-toolchain/bin/avr-gcc:
ifeq ($(AVR_TC_REPO),)
	$(error < x > Please provide valid AVR Toolchain git link)
endif
	$(info < ! > Fetching AVR Toolchain ...)
	$(info < ? > Please be patient as this might take a while ...)
	mkdir -p $(TOOLS_ROOT)
	cd $(TOOLS_ROOT); git clone $(AVR_TC_REPO) avr-toolchain; chmod +x -R $(@D)/..
	echo "< / > Done !"

get_riscv_tc: $(TOOLS_ROOT)/risc-v-toolchain/bin/riscv64-unknown-elf-gcc
$(TOOLS_ROOT)/risc-v-toolchain/bin/riscv64-unknown-elf-gcc:
ifeq ($(RISC_V_TC_REPO),)
	$(error < x > Please provide valid RISC-V Toolchain git link)
endif
	$(info < ! > Fetching RISC-V Toolchain ...)
	$(info < ? > Please be patient as this might take a while ...)
	mkdir -p $(TOOLS_ROOT)
	cd $(TOOLS_ROOT); git clone $(RISC_V_TC_REPO) risc-v-toolchain; chmod +x -R $(@D)/..
	echo "< / > Done !"

