UNIT:=interpreter

UNIT_EXES:= 
UNIT_LIBS:=prima

prima_INC:=utils/include
prima_SRC=prima/pm_interpreter.cpp
		   

ifeq ($(BUILD_TESTS),yes)
-include ./$(UNIT)/test/test.mk
endif

$(foreach exe, $(UNIT_EXES), $(eval $(call add_output_executable,$(exe),$(UNIT))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))
