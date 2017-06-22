#Unit name.
UNIT:=server

UNIT_EXES:=whais
UNIT_SHLS:=
UNIT_LIBS:=wslsrv_cmn

ifneq ($(findstring windows,$(ARCH)),)
UNIT_EXES+=whais_srv
endif

ifneq ($(findstring linux,$(ARCH)),)
UNIT_EXES+=whaisd
endif

wslsrv_cmn_SRC:=common/configuration.cpp common/loader.cpp common/server.cpp\
			common/connection.cpp common/commands.cpp common/stack_cmds.cpp

wslsrv_cmn_DEF:=WVER_MAJ=1 WVER_MIN=2

whais_SRC:=common/server_main.cpp
whais_DEF:=USE_CUSTOM_SHL USE_DBS_SHL USE_INTERP_SHL USE_COMPILER_SHL $(wslsrv_cmn_DEF) 
whais_LIB:=server/wslsrv_cmn utils/wslutils custom/wslcppmemalloc
whais_SHL:=dbs/wpastra compiler/wcompiler interpreter/wprima custom/wcustom

whais_srv_SRC:=windows/service.cpp
whais_srv_DEF:=$(whais_DEF)
whais_srv_LIB:=$(whais_LIB)
whais_srv_SHL:=$(whais_SHL)

whaisd_SRC:=linux/daemon.cpp
whaisd_DEF:=$(whais_DEF)
whaisd_LIB:=$(whais_LIB)
whaisd_SHL:=$(whais_SHL)


$(foreach exe, $(UNIT_EXES), $(eval $(call add_output_executable,$(exe),$(UNIT))))
$(foreach shl, $(UNIT_SHLS), $(eval $(call add_output_shared_lib,$(shl),$(UNIT),$($(shl)_MAJ),$($(shl)_MIN))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))

