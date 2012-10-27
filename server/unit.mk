#Unit name.
UNIT:=server

UNIT_EXES:=whisper
UNIT_SHLS:=
UNIT_LIBS:=

whisper_SRC=common/configuration.cpp common/loader.cpp common/server.cpp\
			common/connection.cpp common/commands.cpp common/server_main.cpp
			
whisper_DEF=USE_DBS_SHL USE_INTERP_SHL USE_COMPILER_SHL
whisper_LIB=utils/utils custom/custom
whisper_SHL=dbs/pastra interpreter/prima compiler/compiler custom/common

whisper_LINUX_SRC=linux/main.cpp
whisper_WINDOWS_SRC=

$(foreach exe, $(UNIT_EXES), $(eval $(call add_output_executable,$(exe),$(UNIT))))
$(foreach shl, $(UNIT_SHLS), $(eval $(call add_output_shared_lib,$(shl),$(UNIT),$($(shl)_MAJ),$($(shl)_MIN))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))

