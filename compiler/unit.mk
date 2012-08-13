#Unit name.
UNIT:=compiler

UNIT_EXES:=whc wod
UNIT_SHLS:=compiler
UNIT_LIBS:=slcompiler

whc_SRC=whc/whc_main.cpp whc/whc_cmdline.cpp whc/msglog.cpp
whc_DEF=USE_COMPILER_SHL
whc_LIB=utils/utils custom/custom
whc_SHL=compiler/compiler custom/common


wod_SRC=wod/wod_main.cpp wod/wod_cmdline.cpp wod/wod_dump.cpp \
		wod/wod_decoder.cpp
wod_DEF=USE_COMPILER_SHL
wod_LIB=utils/utils custom/custom
wod_SHL=compiler/compiler custom/common
 

compiler_INC=utils/include
compiler_SRC=parser/whisper.tab.c parser/parser.c parser/yy.c parser/whisperc.c parser/strstore.c \
			 semantics/expression.c semantics/op_matrix.c semantics/procdecl.c \
			 semantics/statement.c semantics/vardecl.c semantics/wlog.c \
			 semantics/brlo_stmts.c wraper_cpp/compiledunit.cpp
compiler_DEF=COMPILER_EXPORTING=1 USE_COMPILER_SHL
compiler_LIB=utils/utils custom/custom 
compiler_SHL=custom/common

slcompiler_SRC=$(compiler_SRC)
slcompiler_INC=$(compiler_INC)

./$(UNIT)/parser/whisper.tab.c ./$(UNIT)/parser/whisper.tab.h : ./$(UNIT)/parser/whisper.y
	bison -d $? -o $@


ifeq ($(BUILD_TESTS),yes)
-include ./$(UNIT)/test/test.mk
endif

compiler_MAJ=.1
compiler_MIN=.0

$(foreach exe, $(UNIT_EXES), $(eval $(call add_output_executable,$(exe),$(UNIT))))
$(foreach shl, $(UNIT_SHLS), $(eval $(call add_output_shared_lib,$(shl),$(UNIT),$($(shl)_MAJ),$($(shl)_MIN))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))



