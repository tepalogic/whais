UNIT:=interpreter

UNIT_EXES:= 
UNIT_LIBS:=prima

prima_INC:=utils/include
prima_SRC= prima/pm_globals.cpp prima/pm_operand.cpp prima/pm_interpreter.cpp\
           prima/pm_typemanager.cpp prima/pm_procedures.cpp prima/pm_units.cpp\
           prima/pm_processor.cpp prima/pm_operand_arrayfields.cpp\
           prima/pm_operand_fields.cpp prima/pm_operand_array.cpp\
		   

ifeq ($(BUILD_TESTS),yes)
-include ./$(UNIT)/test/test.mk
endif

$(foreach exe, $(UNIT_EXES), $(eval $(call add_output_executable,$(exe),$(UNIT))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))
