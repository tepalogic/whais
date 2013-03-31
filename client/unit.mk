#Unit name.
UNIT:=client

UNIT_EXES:=wcmd
UNIT_SHLS:=
UNIT_LIBS:=connector

connector_SRC=src/connector.c src/client_connection.c
connector_DEF=
connector_LIB=utils/utils custom/custom
connector_SHL=

wcmd_SRC=wcmd/wcmd.cpp wcmd/wcmd_optglbs.cpp wcmd/wcmd_cmdsmgr.cpp\
		 wcmd/wcmd_tabcomds.cpp wcmd/wcmd_onlinecmds.cpp wcmd/wcmd_execcmd.cpp
		 
wcmd_DEF=USE_DBS_SHL
wcmd_LIB=client/connector utils/utils custom/custom 
wcmd_SHL=dbs/pastra custom/common
wcmd_INC=


ifeq ($(BUILD_TESTS),yes)
connector_SRC+=test/test_client_common.cpp
-include ./$(UNIT)/test/test.mk
endif

$(foreach exe, $(UNIT_EXES), $(eval $(call add_output_executable,$(exe),$(UNIT))))
$(foreach shl, $(UNIT_SHLS), $(eval $(call add_output_shared_lib,$(shl),$(UNIT),$($(shl)_MAJ),$($(shl)_MIN))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))

