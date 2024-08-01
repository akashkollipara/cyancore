#
# CYANCORE LICENSE
# Copyrights (C) 2024, Cyancore Team
#
# File Name		: ccache.mk
# Description		: This file provides recipes for build cache
# Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
# Organisation		: Cyancore Core-Team
#

#*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*#
#--------------< Build Cache >--------------#
#*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*#

CCACHE_DIR			:= $(CC_ROOT)/.buildcache

# For Flags read ccache documentation
CCACHE_ABSSTDERR		:= true
export CCACHE_ABSSTDERR
CCACHE_BASEDIR			:=
export CCACHE_BASEDIR
CCACHE_DIR			:= $(CCACHE_DIR)/cache
export CCACHE_DIR
CCACHE_COMPILER			:=
export CCACHE_COMPILER
CCACHE_COMPILERCHECK		:= mtime
export CCACHE_COMPILERCHECK
CCACHE_COMPILERTYPE		:= auto
export CCACHE_COMPILERTYPE
export CCACHE_COMPRESS
CCACHE_COMPRESSLEVEL		:= 0
export CCACHE_COMPRESSLEVEL
CCACHE_EXTENSION		:=
export CCACHE_EXTENSION
# export CCACHE_DEBUG
export CCACHE_NODEBUG
CCACHE_DEBUGDIR			:= $(CCACHE_DIR)/dbg
export CCACHE_DEBUGDIR
CCACHE_DEBUGLEVEL		:= 2
export CCACHE_DEBUGLEVEL
# export CCACHE_DEPEND
export CCACHE_NODEPEND
export CCACHE_DIRECT
# export CCACHE_NODIRECT
# export CCACHE_DISABLE
export CCACHE_NODISABLE
CCACHE_EXTRAFILES		:= 
export CCACHE_EXTRAFILES
# export CCACHE_FILECLONE
export CCACHE_NOFILECLONE
# export CCACHE_HARDLINK
export CCACHE_NOHARDLINK
export CCACHE_HASHDIR
# export CCACHE_NOHASHDIR
CCACHE_IGNOREHEADERS		:=
export CCACHE_IGNOREHEADERS
CCACHE_IGNOREOPTIONS		:=
export CCACHE_IGNOREOPTIONS
# export CCACHE_INODECACHE
export CCACHE_NOINODECACHE
# export CCACHE_COMMENTS
export CCACHE_NOCOMMENTS
CCACHE_LOGFILE			:= $(CCACHE_DIR)/log
export CCACHE_LOGFILE
CCACHE_MAXFILES			:= 0
export CCACHE_MAXFILES
CCACHE_MAXSIZE			:= 1.0 GiB
export CCACHE_MAXSIZE
CCACHE_MSVC_DEP_PREFIX		:= Note: including file:
export CCACHE_MSVC_DEP_PREFIX
CCACHE_NAMESPACE		:= cyancore
export CCACHE_NAMESPACE
CCACHE_PATH			:= 
export CCACHE_PATH
# export CCACHE_PCH_EXTSUM
export CCACHE_NOPCH_EXTSUM
CCACHE_PREFIX			:=
export CCACHE_PREFIX
CCACHE_PREFIX_CPP		:=
export CCACHE_PREFIX_CPP
# export CCACHE_READONLY
export CCACHE_NOREADONLY
# export CCACHE_READONLY_DIRECT
export CCACHE_NOREADONLY_DIRECT
# export CCACHE_RECACHE
export CCACHE_NORECACHE
# export CCACHE_REMOTE_ONLY
export CCACHE_NOREMOTE_ONLY
CCACHE_REMOTE_STORAGE		:=
export CCACHE_REMOTE_STORAGE
# export CCACHE_RESHARE
export CCACHE_NORESHARE
export CCACHE_CPP2
# export CCACHE_NOCPP2
CCACHE_SLOPPINESS		:=
export CCACHE_SLOPPINESS
# export CCACHE_NOSTATS
export CCACHE_STATS
export CCACHE_STATSLOG		:= $(CCACHE_DIR)/stats
export CCACHE_TEMPDIR		:= $(CCACHE_DIR)/
CCACHE_UMASK			:=
export CCACHE_UMASK


ifeq ($(EN_BUILD_CACHE),1)
CCACHE		:= $(shell which ccache)

T_ALLOWLIST	+= show_ccache_config show_ccache_stats clean_ccache

show_ccache_stats:
ifneq ($(realpath $(CCACHE_LOGFILE)),)
	$(CCACHE) -s -x -v
else
	$(info < ! > No log file present, try to perform build and try!)
endif

show_ccache_config: --prepare-cache
	$(CCACHE) -p

clean_ccache:
ifneq ($(realpath $(CCACHE_LOGFILE)),)
	$(CCACHE) -c -C -z
else
	$(info < ! > No log file present, try to perform build and try!)
endif


.PHONY: --prepare-cache
--prepare-cache: $(CCACHE_LOGFILE)

.SECONDEXPANSION:
$(CCACHE_LOGFILE): | $$(@D)/
	touch $@
endif

