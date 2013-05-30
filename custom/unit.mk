#Unit name.
UNIT:=custom

UNIT_EXES:=
UNIT_SHLS:=wcommon
UNIT_LIBS:=wslcustom
ifeq ($(EXTRA_LIBS),yes)
UNIT_LIBS+=wslcommon
endif

wslcommon_SRC=cpp_support/exception.cpp cpp_support/wthread.cpp\
	         cpp_support/wfile.cpp cpp_support/wsocket.cpp

wcommon_SRC=$(wslcommon_SRC) cpp_support/wmemtracker.cpp 

wcommon_DEF=USE_SHL EXPORT_EXCEP_SHL
wcommon_MAJ=.1
wcommon_MIN=.0
wcommon_LIB=custom/wslcustom

wslcustom_SRC=

ifeq ($(ARCH),linux_gcc_x86_64)
SRC_FOLDER:=posix
endif

ifeq ($(ARCH),linux_gcc_x86)
SRC_FOLDER:=posix
endif

ifeq ($(ARCH),linux_gcc_ppc)
SRC_FOLDER:=posix
endif 

ifeq ($(ARCH),linux_gcc_ppc64)
SRC_FOLDER:=posix
endif 

ifeq ($(ARCH),windows_vc_x86)
SRC_FOLDER:=windows
endif

wslcustom_SRC+=$(SRC_FOLDER)/fileio.c
wslcustom_SRC+=$(SRC_FOLDER)/thread.c  
wslcustom_SRC+=$(SRC_FOLDER)/time.c  
wslcustom_SRC+=$(SRC_FOLDER)/network.c  
wslcustom_SRC+=$(SRC_FOLDER)/memory.cpp

ifeq ($(MEMORY_TRACE),yes)
wslcustom_SRC+=test/mem_test.c
endif

$(foreach shl, $(UNIT_SHLS), $(eval $(call add_output_shared_lib,$(shl),$(UNIT),$($(shl)_MAJ),$($(shl)_MIN))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))

