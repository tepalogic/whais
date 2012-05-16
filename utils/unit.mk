#Unit name.
UNIT:=utils

UNIT_EXES:=
UNIT_LIBS:=utils

utils_SRC=src/array.c src/msglog.c src/outstream.c src/wfile.cpp src/random.c src/wthread.cpp

$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))
