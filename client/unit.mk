#Unit name.
UNIT:=client

UNIT_EXES:=wcmd
UNIT_LIBS:=wslconnector
UNIT_SHLS:=wconnector

wslconnector_INC:=
wslconnector_SRC:=src/connector.c src/client_connection.c
wslconnector_DEF:=
wslconnector_LIB:=utils/wslutils custom/wslcustom
wslconnector_SHL:=

wconnector_INC:=
wconnector_SRC:=$(wslconnector_SRC)
wconnector_DEF:=WVER_MAJ=1 WVER_MIN=0 USE_CONNECTOR_SHL CONNECTOR_EXPORTING
wconnector_LIB:=utils/wslutils custom/wslcustom custom/wslcppmemalloc

wconnector_MAJ=.1
wconnector_MIN=.0

wcmd_SRC:=wcmd/wcmd.cpp wcmd/wcmd_optglbs.cpp wcmd/wcmd_cmdsmgr.cpp\
		 wcmd/wcmd_tabcomds.cpp wcmd/wcmd_onlinecmds.cpp wcmd/wcmd_execcmd.cpp\
		 wcmd/wcmd_valparser.cpp wcmd/wcmd_dbcheck.cpp wcmd/wcmd_altertable.cpp
		 
wcmd_DEF:=USE_CUSTOM_SHL USE_DBS_SHL USE_INTERP_SHL WVER_MAJ=1 WVER_MIN=2
wcmd_LIB:=client/wslconnector utils/wslutils custom/wslcppmemalloc  
wcmd_SHL:=dbs/wpastra compiler/wcompiler interpreter/wprima custom/wcustom
wcmd_INC:=


ifeq ($(BUILD_TESTS),yes)
wslconnector_SRC+=test/test_client_common.cpp
-include ./$(UNIT)/test/test.mk
endif

$(foreach exe, $(UNIT_EXES), $(eval $(call add_output_executable,$(exe),$(UNIT))))
$(foreach shl, $(UNIT_SHLS), $(eval $(call add_output_shared_lib,$(shl),$(UNIT),$($(shl)_MAJ),$($(shl)_MIN))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))

