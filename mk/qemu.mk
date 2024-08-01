#
# CYANCORE LICENSE
# Copyrights (C) 2023, Cyancore Team
#
# File Name		: qemu.mk
# Description		: This file helps fetch and build qemu
# Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
# Organisation		: Cyancore Core-Team
#

T_ALLOWLIST	+= get_qemu clean_qemu qemu_test

QEMU_CHECKOUT	:= v9.0.0
QEMU_PATH	:= $(TOOLS_ROOT)/qemu
QEMU_BUILD_PATH	:= $(QEMU_PATH)/build
QEMU_OUT_PATH	:= $(TOOLS_ROOT)/cc_qemu
QEMU_TLIST	:= avr-softmmu arm-softmmu aarch64-softmmu
QEMU_TLIST	+= riscv32-softmmu riscv64-softmmu x86_64-softmmu

ifneq ($(V),1)
QEMU_QUIET	:= 2> /dev/null 1> /dev/null
endif

get_qemu: $(QEMU_OUT_PATH)

$(TOOLS_ROOT)/qemu:
	@echo "< ! > Using qemu version: $(QEMU_CHECKOUT)"
	@echo "< ! > Fetching qemu ..."
	mkdir -p $(TOOLS_ROOT)
	cd $(TOOLS_ROOT); git clone https://gitlab.com/qemu-project/qemu.git --quiet;
	cd $@; git checkout -b $(QEMU_CHECKOUT) $(QEMU_CHECKOUT) 1> /dev/null 2> /dev/null
	@echo "< / > Done !"


s		:= $() $()
c		:= ,

$(QEMU_OUT_PATH): $(QEMU_PATH)
	@echo "< ! > Building qemu ..."
	@echo "< ? > Please be patient as this might take a while ..."
	cd $<; ./configure --prefix=$(QEMU_OUT_PATH) --target-list=$(subst $(s),$(c),$(QEMU_TLIST)) $(QEMU_QUIET)
	make -j $(N_JOBS) -C $< install $(QEMU_QUIET)
	@echo "< ! > Cleaning up build space ..."
	rm -rf $(QEMU_PATH)
	@echo "< ! > Adding load_qemu alias to bashrc ..."
	@echo "< ! > run 'load_qemu' before trying to launch qemu!"
	echo "alias load_qemu='export PATH=\"\$$PATH\":$@/bin/'" >> ~/.bash_aliases
	@echo "< / > Done !"

clean_qemu:
	@echo "< ! > Removing cc-qemu installation ..."
	rm -rf $(QEMU_OUT_PATH) $(QEMU_PATH)
	sed "/cc_qemu/d" -i ~/.bash_aliases
	@echo "< / > Done!"

qemu_test:
ifeq ($(realpath $(QEMU_OUT_PATH)/bin/qemu-system-riscv32),)
	$(info < x > QEMU is not installed...)
	$(info < ! > Please run `make get_qemu`)
	$(error < x > Stopping demo simulation!)
endif
	make demo_qemu_sifive_e
	@echo
	@echo "Press Ctrl+A - X to exit!"
	@echo
	$(QEMU_OUT_PATH)/bin/qemu-system-riscv32 -machine sifive_e -device loader,file=out/qemu_sifive_e_bl/qemu_sifive_e_bl.elf -kernel out/demo_qemu_sifive_e/demo_qemu_sifive_e.elf -nographic 
