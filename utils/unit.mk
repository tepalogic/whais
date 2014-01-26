#Unit name.
UNIT:=utils

UNIT_EXES:=
UNIT_LIBS:=wslutils
UNIT_SHLS:=

wslutils_SRC=src/warray.c src/msglog.c src/woutstream.c src/wrandom.c\
		  src/logger.cpp src/tokenizer.cpp src/wutf.c src/enc_3k.c\
		  src/valtranslator.cpp src/wtypes.c src/wunicode.c src/whash.c

$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))

