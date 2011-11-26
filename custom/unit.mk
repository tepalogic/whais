#Unit name.
UNIT:=custom

UNIT_EXES:=
UNIT_LIBS:=custom

custom_SRC=cpp_support/overloading.cpp

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
custom_SRC+=$(SRC_FOLDER)/sync.c  

ifeq ($(BUILD_TESTS),yes)
custom_SRC+=$(SRC_FOLDER)/memory_test.c test/test_fmw.c
else
custom_SRC+=$(SRC_FOLDER)/memory.c
endif


$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))
