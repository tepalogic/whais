#Unit name.
UNIT:=utils

UNIT_EXES:=
UNIT_LIBS:=utils

utils_SRC=src/array.c src/list.c src/msglog.c src/outstream.c src/wfile.cpp


$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))

