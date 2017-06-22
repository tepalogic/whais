#Unit name.
UNIT:=custom

UNIT_EXES:=
UNIT_SHLS:=wcustom
UNIT_LIBS:=wslcustom wslcppmemalloc

wslcustom_SRC:=cpp_support/exception.cpp cpp_support/wthread.cpp\
			cpp_support/wfile.cpp cpp_support/wsocket.cpp cpp_support/wmemtracker.cpp

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

ifeq ($(ARCH),windows_vc_x86_64)
SRC_FOLDER:=windows
endif

ifeq ($(MEMORY_TRACE),yes)
wslcustom_SRC+=test/mem_test.c
endif

wslcustom_SRC+=$(SRC_FOLDER)/fileio.c
wslcustom_SRC+=$(SRC_FOLDER)/thread.c  
wslcustom_SRC+=$(SRC_FOLDER)/time.c  
wslcustom_SRC+=$(SRC_FOLDER)/console.c  
wslcustom_SRC+=$(SRC_FOLDER)/network.c
wslcustom_SRC+=$(SRC_FOLDER)/shl.c

wcustom_cmn_DEF:=WVER_MAJ=1 WVER_MIN=0
wcustom_DEF:=USE_CUSTOM_SHL CUSTOM_EXPORTING $(wcustom_cmn_DEF)
wcustom_LIB:=utils/wslutils custom/wslcppmemalloc
wcustom_SHL:=
wcustom_SRC=$(wslcustom_SRC)
	   
wcustom_MAJ=.1
wcustom_MIN=.0

wslcppmemalloc_SRC=$(SRC_FOLDER)/memory.cpp

$(foreach shl, $(UNIT_SHLS), $(eval $(call add_output_shared_lib,$(shl),$(UNIT),$($(shl)_MAJ),$($(shl)_MIN))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))

