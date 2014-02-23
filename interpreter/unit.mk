UNIT:=interpreter

UNIT_EXES:= 
UNIT_LIBS:=
ifeq ($(EXTRA_LIBS),yes)
UNIT_LIBS+=wslprima
endif
UNIT_SHLS:=wprima

wprima_INC:=
wprima_SRC:=prima/pm_globals.cpp prima/pm_operand.cpp prima/pm_interpreter.cpp\
           prima/pm_typemanager.cpp prima/pm_procedures.cpp prima/pm_units.cpp\
           prima/pm_processor.cpp prima/pm_operand_arrayfields.cpp\
           prima/pm_operand_fields.cpp prima/pm_operand_array.cpp\
           prima/pm_general_table.cpp prima/pm_operand_undefined.cpp\
           prima/pm_exception.cpp
           
wprima_DEF:=USE_INTERP_SHL USE_DBS_SHL INTERP_EXPORTING
wprima_LIB:=utils/wslutils custom/wslcustom
wprima_SHL:=dbs/wpastra compiler/wcompiler custom/wcommon

wslprima_SRC=$(wprima_SRC)
wslprima_INC=$(wprima_INC)
wslprima_DEF:=

wprima_MAJ=.1
wprima_MIN=.0

ifeq ($(BUILD_TESTS),yes)
-include ./$(UNIT)/test/test.mk
endif

$(foreach exe, $(UNIT_EXES), $(eval $(call add_output_executable,$(exe),$(UNIT))))
$(foreach shl, $(UNIT_SHLS), $(eval $(call add_output_shared_lib,$(shl),$(UNIT),$($(shl)_MAJ),$($(shl)_MIN))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))
