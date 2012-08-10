UNIT:=interpreter

UNIT_EXES:= 
UNIT_LIBS:=
UNIT_SHLS:=prima

prima_INC:=utils/include
prima_SRC= prima/pm_globals.cpp prima/pm_operand.cpp prima/pm_interpreter.cpp\
           prima/pm_typemanager.cpp prima/pm_procedures.cpp prima/pm_units.cpp\
           prima/pm_processor.cpp prima/pm_operand_arrayfields.cpp\
           prima/pm_operand_fields.cpp prima/pm_operand_array.cpp
prima_SHL:=utils/utils custom/custom dbs/pastra compiler/compiler
prima_DEF:=INTERP_EXPORTING=1


prima_MAJ=.1
prima_MIN=.0

ifeq ($(BUILD_TESTS),yes)
prima_DEF:=PRIMA_EXPORTING=1
-include ./$(UNIT)/test/test.mk
endif

$(foreach exe, $(UNIT_EXES), $(eval $(call add_output_executable,$(exe),$(UNIT))))
$(foreach shl, $(UNIT_SHLS), $(eval $(call add_output_shared_lib,$(shl),$(UNIT),$($(shl)_MAJ),$($(shl)_MIN))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))
