#Unit name.
UNIT:=compiler

UNIT_EXES:=whc wod
UNIT_LIBS:=
ifeq ($(EXTRA_LIBS),yes)
UNIT_LIBS+=wslcompiler
endif
UNIT_SHLS:=wcompiler

wcompiler_MAJ=.1
wcompiler_MIN=.0


whc_SRC:=whc/whc_main.cpp whc/whc_cmdline.cpp whc/msglog.cpp \
		whc/whc_preprocess.cpp
whc_DEF:=USE_COMPILER_SHL USE_CUSTOM_SHL WVER_MAJ=1 WVER_MIN=2
whc_LIB:=utils/wslutils  custom/wslcppmemalloc
whc_SHL:=compiler/wcompiler custom/wcustom

wod_SRC:=wod/wod_main.cpp wod/wod_cmdline.cpp wod/wod_dump.cpp \
		wod/wod_decoder.cpp
wod_DEF:=USE_COMPILER_SHL USE_CUSTOM_SHL WVER_MAJ=1 WVER_MIN=2
wod_LIB:=$(whc_LIB)
wod_SHL:=$(whc_SHL)
 

wcompiler_INC=
wcompiler_SRC=parser/whais.tab.c parser/parser.c parser/yy.c parser/whaisc.c parser/strstore.c \
			 semantics/expression.c semantics/op_matrix.c semantics/procdecl.c \
			 semantics/statement.c semantics/vardecl.c semantics/wlog.c \
			 semantics/brlo_stmts.c semantics/table_stmts.c wraper_cpp/compiledunit.cpp
wcompiler_DEF=COMPILER_EXPORTING USE_COMPILER_SHL USE_CUSTOM_SHL WVER_MAJ=1 WVER_MIN=0
wcompiler_LIB=utils/wslutils custom/wslcppmemalloc
wcompiler_SHL=custom/wcustom

wslcompiler_SRC=$(wcompiler_SRC)
wslcompiler_INC=$(wcompiler_INC)

./compiler/parser/whais.tab.c ./compiler/parser/whais.tab.h : ./compiler/parser/whais.y
	$(ECHO)if [ ! -f $@ ] ; then bison -d $? -o $@ ; fi


ifeq ($(BUILD_TESTS),yes)
-include ./$(UNIT)/test/test.mk
endif

$(foreach exe, $(UNIT_EXES), $(eval $(call add_output_executable,$(exe),$(UNIT))))
$(foreach shl, $(UNIT_SHLS), $(eval $(call add_output_shared_lib,$(shl),$(UNIT),$($(shl)_MAJ),$($(shl)_MIN))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))

