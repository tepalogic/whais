
ALL_UNITS:=utils custom compiler interpreter dbs server client

VERBOSE ?= no
FLAVOR  ?= debug
BUILD_TESTS ?= no
EXES:=
LIBS:=

ifeq ($(ARCH),)
$(error "You must specify tour ARCH as linux_gcc32, linux_gcc64 or windows_vc") 
endif

ifeq ($(VERBOSE),yes)
ECHO:=
else
ECHO:=@
endif

INCLUDES+=./ ./common

ifeq ($(BUILD_TESTS),yes)
DEFINES+=BUILD_TESTS=1
DEFINES+=ENABLE_MEMORY_TRACE=1
endif
DEFINES+=$(ARCH)=0


include ./makesys/arch/$(ARCH).mk
include ./makesys/defs.mk
include $(foreach unit, $(ALL_UNITS), ./$(unit)/unit.mk) 
include ./makesys/topmake.mk


