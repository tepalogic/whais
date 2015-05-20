#Unit name.
UNIT:=server

UNIT_EXES:=whais
UNIT_SHLS:=
UNIT_LIBS:=whais_cmn

ifneq ($(findstring windows,$(ARCH)),)
UNIT_EXES+=whais_srv
endif


whais_cmn_SRC=common/configuration.cpp common/loader.cpp common/server.cpp\
			common/connection.cpp common/commands.cpp common/stack_cmds.cpp
whais_cmn_DEF=USE_DBS_SHL USE_INTERP_SHL USE_COMPILER_SHL


whais_SRC:=common/server_main.cpp
whais_DEF=$(whais_cmn_DEF)
whais_LIB=utils/wslutils custom/wslcustom server/whais_cmn
whais_SHL=custom/wcommon dbs/wpastra compiler/wcompiler interpreter/wprima

whais_srv_SRC:=windows/service.cpp
whais_srv_DEF:=$(whais_cmn_DEF)
whais_srv_LIB:=$(whais_LIB)
whais_srv_SHL:=$(whais_SHL)


$(foreach exe, $(UNIT_EXES), $(eval $(call add_output_executable,$(exe),$(UNIT))))
$(foreach shl, $(UNIT_SHLS), $(eval $(call add_output_shared_lib,$(shl),$(UNIT),$($(shl)_MAJ),$($(shl)_MIN))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))

