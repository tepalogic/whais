UNIT:=stdlib

UNIT_EXES:=
UNIT_LIBS:=
UNIT_SHLS:=wnl_base wnl_dev_prima

wnl_base_INC:=
wnl_base_SRC:=base/base.cpp base/base_types.cpp base/base_generics.cpp base/base_constants.cpp\
	          base/base_dates.cpp base/base_text.cpp base/base_arrays.cpp base/base_fields.cpp\
			  base/base_tables.cpp
wnl_base_LIB:=utils/wslutils custom/wslcppmemalloc
wnl_base_DEF:=USE_DBS_SHL USE_CUSTOM_SHL USE_INTERP_SHL
wnl_base_SHL:=dbs/wpastra interpreter/wprima custom/wcustom
	   
wnl_base_MAJ=.1
wnl_base_MIN=.0


wnl_dev_prima_INC:=interpreter/include
wnl_dev_prima_SRC:=dev_prima/dev_prima.cpp dev_prima/dev_prima_accessors.cpp
wnl_dev_prima_LIB:=utils/wslutils custom/wslcppmemalloc interpreter/wslprima
wnl_dev_prima_DEF:=USE_DBS_SHL USE_CUSTOM_SHL
wnl_dev_prima_SHL:=dbs/wpastra custom/wcustom

wnl_dev_prima_MAJ=.1
wnl_dev_prima_MIN=.0

ifeq ($(BUILD_TESTS),yes)
-include ./$(UNIT)/test/test.mk
endif

$(foreach exe, $(UNIT_EXES), $(eval $(call add_output_executable,$(exe),$(UNIT))))
$(foreach shl, $(UNIT_SHLS), $(eval $(call add_output_shared_lib,$(shl),$(UNIT),$($(shl)_MAJ),$($(shl)_MIN))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))

