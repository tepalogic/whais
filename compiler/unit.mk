#Unit name.
UNIT:=compiler

UNIT_EXES:=whc wod
UNIT_LIBS:=compiler

compiler_INC=utils/include

whc_SRC=whc/whc_main.cpp whc/whc_cmdline.cpp whc/msglog.cpp
whc_LIB=compiler/compiler utils/utils custom/custom

wod_SRC=wod/wod_main.cpp wod/wod_cmdline.cpp wod/wod_dump.cpp \
		wod/wod_decoder.cpp
wod_LIB=compiler/compiler utils/utils custom/custom

compiler_SRC=parser/whisper.tab.c parser/parser.c parser/yy.c parser/whisperc.c parser/strstore.c \
			 semantics/expression.c semantics/op_matrix.c semantics/procdecl.c \
			 semantics/statement.c semantics/vardecl.c semantics/wlog.c \
			 semantics/brlo_stmts.c wraper_cpp/compiledunit.cpp

./$(UNIT)/parser/whisper.tab.c ./$(UNIT)/parser/whisper.tab.h : ./$(UNIT)/parser/whisper.y
	bison -d $? -o $@


ifeq ($(BUILD_TESTS),yes)
-include ./$(UNIT)/test/test.mk
endif

$(foreach exe, $(UNIT_EXES), $(eval $(call add_output_executable,$(exe),$(UNIT))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))



