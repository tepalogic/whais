#Unit name.
UNIT:=extutils

UNIT_EXES:=
UNIT_LIBS:=wslextutils
UNIT_SHLS:=

wslextutils_SRC=src/table_filter.cpp src/ext_exception.cpp
wslextutils_DEF:=USE_CUSTOM_SHL USE_DBS_SHL
 
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))

