
ALL_UNITS:=utils custom compiler interpreter dbs server client stdlib

ifeq ($(FLAVOR),release)
OPTIMISE?=speed
ASSERTS?=no
DEBUGINFO?=no
endif

VERBOSE?=no
OPTIMISE?=none
ASSERTS?=yes
DEBUGINFO?=yes
PROFILE?=no
BUILD_TESTS?=no

EXES:=
LIBS:=

ifeq ($(ARCH),)
$(error "You must specify tour ARCH as linux_gcc_x86, linux_gcc_x86_64 or windows_vc_x86, etc.") 
endif

ifeq ($(VERBOSE),yes)
ECHO:=
else
ECHO:=@
endif

INCLUDES+=./ ./headers

ifeq ($(BUILD_TESTS),yes)
DEFINES+=BUILD_TESTS=1
MEMORY_TRACE?=yes
EXTRA_LIBS=yes
endif

ifeq ($(MEMORY_TRACE),yes)
DEFINES+=ENABLE_MEMORY_TRACE=1
endif

DEFINES+=$(ARCH)=0

include ./makesys/arch/$(ARCH).mk
include ./makesys/defs.mk
include $(foreach unit, $(ALL_UNITS), ./$(unit)/unit.mk) 
include ./makesys/topmake.mk
