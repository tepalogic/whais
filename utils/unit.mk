#Unit name.
UNIT:=utils

UNIT_EXES:=
UNIT_LIBS:=utils
UNIT_SHLS:=

utils_SRC=src/array.c src/msglog.c src/outstream.c src/random.c\
		  src/logger.cpp src/tokenizer.cpp src/utf8.c

$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))
