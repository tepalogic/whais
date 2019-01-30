#Unit name.
UNIT:=extutils

UNIT_EXES:=
UNIT_LIBS:=wslextutils
UNIT_SHLS:=

wslextutils_SRC=src/table_filter.cpp src/ext_exception.cpp src/arrays_ops.cpp\
				src/alter_table.cpp
wslextutils_DEF:=USE_CUSTOM_SHL USE_DBS_SHL
 

ifeq ($(BUILD_TESTS),yes)
-include ./$(UNIT)/test/test.mk
endif

$(foreach exe, $(UNIT_EXES), $(eval $(call add_output_executable,$(exe),$(UNIT))))
$(foreach shl, $(UNIT_SHLS), $(eval $(call add_output_shared_lib,$(shl),$(UNIT),$($(shl)_MAJ),$($(shl)_MIN))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))

