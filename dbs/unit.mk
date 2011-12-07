UNIT:=dbs

UNIT_EXES:= 
UNIT_LIBS:=pastra

pastra_INC:=utils/include
pastra_SRC=pastra/dbs_values.cpp pastra/ps_container.cpp pastra/ps_table.cpp\
		   pastra/ps_dbsmgr.cpp pastra/ps_valintep.cpp pastra/ps_varstorage.cpp\
		   pastra/ps_blockcache.cpp pastra/ps_textstrategy.cpp pastra/ps_arraystrategy.cpp\
		   pastra/ps_btree_index.cpp
		   

ifeq ($(BUILD_TESTS),yes)
-include ./$(UNIT)/test/test.mk
endif

$(foreach exe, $(UNIT_EXES), $(eval $(call add_output_executable,$(exe),$(UNIT))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))
