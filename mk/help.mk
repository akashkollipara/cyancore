#
# CYANCORE LICENSE
# Copyrights (C) 2023, Cyancore Team
#
# File Name		: help.mk
# Description		: This file prints cyancore build help
# Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
# Organisation		: Cyancore Core-Team
#

T_ALLOWLIST	+= help

HELP		:="\
==================< Cyancore Make Help >==================\n\
Usage: make [<project_name>|default|...] (V=|PP=|...)\n\
\n\
Examples:\n\
$$ make demo_avr \# Builds project demo_avr\n\
$$ make demo_avr clean \# Cleans demo_avr build\n\
$$ make demo_avr V=1 \# Builds demo_avr with verbose\n\
$$ make list \# Displays all the available projects\n\
$$ make get_all_tc \# Fetches all toolchains\n\
\n\
----------\n\
Variables:\n\
----------\n\
V=(*0/1) Verbose build logs.\n\
If 1, the build will spew all the build logs.\n\
\n\
PP=(*0/1) PreProcess stage enable.\n\
If 1, the build engine shall generate/save preprocessed\n\
file which can be used for debugging.\n\
\n\
DEBUG=(*0/1/2/3/5) Enable build with debug code.\n\
This Flag is used to enable debug routines to log info\n\
while program executes. Visit libsyslog directory for\n\
more info.\n\
\n\
* - marks default value\n\
For more variablea that can be passed,\n\
please visit [DOCS REPO]\n\
\n\
--------\n\
Targets:\n\
--------\n\
project_name: Name of the project.\n\
The build engine will automatically create dynamic target\n\
using project name. Any following target which will\n\
depend on project name will be marked with \`~\`.\n\
\n\
~ default: The default target of make, this need not be\n\
passed. This uses multi threaded build.\n\
~ cyancore: This performs 1 thread build.\n\
Use this for debug as it will be slow than default.\n\
~ clean: Cleans the project build artifacts.\n\
~ check: Perform static analysis.\n\
clean_workspace: Cleans workspace to new checkout state.\n\
list: Displays all the projects available for build.\n\
get_all_tc: Fetches all Toolchains.\n\
get_avr_tc: Fetches AVR Toolchain.\n\
get_arm_tc: Fetches ARM Toolchain.\n\
get_riscv_tc: Fetches RISC-V Toolchain.\n\
qemu_test: Performs build and runs a demo on qemu.\n\
\n\
For more targets/info, please visit [DOCS REPO].\n\
\n\
----------------------------------------------------------\n\
DOCS REPO: https://github.com/VisorFolks/cc_docs\n\
----------------------------------------------------------\
"

help:
	echo -e $(HELP)
