#Unit name.
UNIT:=extutils

UNIT_EXES:=
UNIT_LIBS:=wslextutils
UNIT_SHLS:=

wslextutils_SRC=src/table_filter.cpp

$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))

