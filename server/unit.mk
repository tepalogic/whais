#Unit name.
UNIT:=server

UNIT_EXES:=whais
UNIT_SHLS:=
UNIT_LIBS:=

whais_SRC=common/configuration.cpp common/loader.cpp common/server.cpp\
			common/connection.cpp common/commands.cpp common/stack_cmds.cpp\
			common/server_main.cpp
			
whais_DEF=USE_DBS_SHL USE_INTERP_SHL USE_COMPILER_SHL
whais_LIB=utils/wslutils custom/wslcustom
whais_SHL=custom/wcommon dbs/wpastra compiler/wcompiler interpreter/wprima

whais_LINUX_SRC=linux/main.cpp
whais_WINDOWS_SRC=

$(foreach exe, $(UNIT_EXES), $(eval $(call add_output_executable,$(exe),$(UNIT))))
$(foreach shl, $(UNIT_SHLS), $(eval $(call add_output_shared_lib,$(shl),$(UNIT),$($(shl)_MAJ),$($(shl)_MIN))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))

