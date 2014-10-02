#Unit name.
UNIT:=compiler

UNIT_EXES:=whc wod
UNIT_LIBS:=
ifeq ($(EXTRA_LIBS),yes)
UNIT_LIBS+=wslcompiler
endif
UNIT_SHLS:=wcompiler


whc_SRC=whc/whc_main.cpp whc/whc_cmdline.cpp whc/msglog.cpp \
		whc/whc_preprocess.cpp

whc_DEF=USE_COMPILER_SHL
whc_LIB=utils/wslutils custom/wslcustom
whc_SHL=compiler/wcompiler custom/wcommon


wod_SRC=wod/wod_main.cpp wod/wod_cmdline.cpp wod/wod_dump.cpp \
		wod/wod_decoder.cpp
wod_DEF=USE_COMPILER_SHL
wod_LIB=utils/wslutils custom/wslcustom
wod_SHL=compiler/wcompiler custom/wcommon
 

wcompiler_INC=
wcompiler_SRC=parser/whisper.tab.c parser/parser.c parser/yy.c parser/whisperc.c parser/strstore.c \
			 semantics/expression.c semantics/op_matrix.c semantics/procdecl.c \
			 semantics/statement.c semantics/vardecl.c semantics/wlog.c \
			 semantics/brlo_stmts.c wraper_cpp/compiledunit.cpp
wcompiler_DEF=COMPILER_EXPORTING=1 USE_COMPILER_SHL
wcompiler_LIB=utils/wslutils custom/wslcustom 
wcompiler_SHL=custom/wcommon

wslcompiler_SRC=$(wcompiler_SRC)
wslcompiler_INC=$(wcompiler_INC)

./compiler/parser/whisper.tab.c ./compiler/parser/whisper.tab.h : ./compiler/parser/whisper.y
	$(ECHO)if [ ! -f $@ ] ; then bison -d $? -o $@ ; fi


ifeq ($(BUILD_TESTS),yes)
-include ./$(UNIT)/test/test.mk
endif

wcompiler_MAJ=.1
wcompiler_MIN=.0

$(foreach exe, $(UNIT_EXES), $(eval $(call add_output_executable,$(exe),$(UNIT))))
$(foreach shl, $(UNIT_SHLS), $(eval $(call add_output_shared_lib,$(shl),$(UNIT),$($(shl)_MAJ),$($(shl)_MIN))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))

