UNIT:=stdlib

UNIT_EXES:=
UNIT_LIBS:=
UNIT_SHLS:=wnl_math

wnl_math_INC:=
wnl_math_SRC:=math/math.cpp math/math_constants.cpp
wnl_math_LIB:=utils/wslutils custom/wslcustom
wnl_math_DEF:=USE_DBS_SHL USE_INTERP_SHL
wnl_math_SHL:=dbs/wpastra interpreter/wprima custom/wcommon
	   
wnl_math_MAJ=.1
wnl_math_MIN=.0

ifeq ($(BUILD_TESTS),yes)
-include ./$(UNIT)/test/test.mk
endif

$(foreach exe, $(UNIT_EXES), $(eval $(call add_output_executable,$(exe),$(UNIT))))
$(foreach shl, $(UNIT_SHLS), $(eval $(call add_output_shared_lib,$(shl),$(UNIT),$($(shl)_MAJ),$($(shl)_MIN))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))

