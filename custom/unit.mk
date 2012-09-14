#Unit name.
UNIT:=custom

UNIT_EXES:=
UNIT_SHLS:=common
UNIT_LIBS:=custom slcommon


common_DEF=USE_SHL EXPORT_EXCEP_SHL
common_SRC=cpp_support/exception.cpp cpp_support/wthread.cpp\
			cpp_support/wfile.cpp cpp_support/wsocket.cpp 
common_MAJ=.1
common_MIN=.0
common_LIB=custom/custom

custom_SRC=

ifeq ($(ARCH),linux_gcc64)
SRC_FOLDER:=posix
endif

ifeq ($(ARCH),linux_gcc32)
SRC_FOLDER:=posix
endif
ifeq ($(ARCH),wine_vc)
SRC_FOLDER:=windows
endif

ifeq ($(ARCH),windows_vc)
SRC_FOLDER:=windows
endif

custom_SRC+=$(SRC_FOLDER)/fileio.c
custom_SRC+=$(SRC_FOLDER)/thread.c  
custom_SRC+=$(SRC_FOLDER)/time.c  
custom_SRC+=$(SRC_FOLDER)/network.c  

ifeq ($(BUILD_TESTS),yes)
custom_SRC+=$(SRC_FOLDER)/memory_test.c test/test_fmw.c
else
custom_SRC+=$(SRC_FOLDER)/memory.c
endif

slcommon_SRC=$(common_SRC)

$(foreach shl, $(UNIT_SHLS), $(eval $(call add_output_shared_lib,$(shl),$(UNIT),$($(shl)_MAJ),$($(shl)_MIN))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))
