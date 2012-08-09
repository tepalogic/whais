#Unit name.
UNIT:=utils

UNIT_EXES:=
UNIT_LIBS:=
UNIT_SHLS:=utils

utils_DEF=UTILS_EXPORTING=1
utils_SRC=src/array.c src/msglog.c src/outstream.c\
		  src/wfile.cpp src/random.c src/wthread.cpp

utils_SHL=custom/custom

utils_MAJ=.1
utils_MIN=.0

$(foreach shl, $(UNIT_SHLS), $(eval $(call add_output_shared_lib,$(shl),$(UNIT),$($(shl)_MAJ),$($(shl)_MIN))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))
