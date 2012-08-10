UNIT:=dbs

UNIT_EXES:=wcmd
UNIT_LIBS:=
UNIT_SHLS:=pastra

pastra_INC:=utils/include
pastra_SRC:=pastra/ps_values.cpp pastra/ps_container.cpp pastra/ps_table.cpp\
		   	pastra/ps_dbsmgr.cpp pastra/ps_valintep.cpp pastra/ps_varstorage.cpp\
		   	pastra/ps_blockcache.cpp pastra/ps_textstrategy.cpp pastra/ps_arraystrategy.cpp\
		   	pastra/ps_btree_index.cpp pastra/ps_btree_fields.cpp pastra/ps_templatetable.cpp
pastra_SHL:=utils/utils custom/custom
pastra_DEF:=DBS_EXPORTING=1

wcmd_SRC=wcmd/wcmd.cpp wcmd/wcmd_optglbs.cpp wcmd/wcmd_cmdsmgr.cpp wcmd/wcmd_tabcomds.cpp
wcmd_SHL=utils/utils custom/custom dbs/pastra
		   
pastra_MAJ=.1
pastra_MIN=.0

ifeq ($(BUILD_TESTS),yes)
pastra_DEF+=PASTRA_EXPORTING=1
-include ./$(UNIT)/test/test.mk
endif

$(foreach exe, $(UNIT_EXES), $(eval $(call add_output_executable,$(exe),$(UNIT))))
$(foreach shl, $(UNIT_SHLS), $(eval $(call add_output_shared_lib,$(shl),$(UNIT),$($(shl)_MAJ),$($(shl)_MIN))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))

