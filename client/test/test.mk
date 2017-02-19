UNIT_EXES+=c_test_glb_list
c_test_glb_list_SRC=test/test_glb_list.cpp
c_test_glb_list_LIB=client/wslconnector custom/wslcustom utils/wslutils

c_test_glb_list_2_SRC=test/test_glb_list_2.cpp
c_test_glb_list_2_LIB=client/wslconnector custom/wslcustom utils/wslutils 

UNIT_EXES+=c_test_proc_list
c_test_proc_list_SRC=test/test_proc_list.cpp
c_test_proc_list_LIB=client/wslconnector custom/wslcustom utils/wslutils 

c_test_proc_list_2_SRC=test/test_proc_list_2.cpp
c_test_proc_list_2_LIB=client/wslconnector custom/wslcustom utils/wslutils 

UNIT_EXES+=c_test_proc_args
c_test_proc_args_SRC=test/test_proc_args.cpp
c_test_proc_args_LIB=client/wslconnector custom/wslcustom utils/wslutils 

UNIT_EXES+=c_test_glb_types
c_test_glb_types_SRC=test/test_glb_types.cpp
c_test_glb_types_LIB=client/wslconnector custom/wslcustom utils/wslutils 

UNIT_EXES+=c_test_stack_ops
c_test_stack_ops_SRC=test/test_stack_ops.cpp
c_test_stack_ops_LIB=client/wslconnector custom/wslcustom utils/wslutils 

UNIT_EXES+=c_test_stack_update_basics
c_test_stack_update_basics_SRC=test/test_stack_update_basics.cpp
c_test_stack_update_basics_LIB=client/wslconnector custom/wslcustom utils/wslutils 

UNIT_EXES+=c_test_stack_update_array
c_test_stack_update_array_SRC=test/test_stack_update_array.cpp
c_test_stack_update_array_LIB=client/wslconnector custom/wslcustom utils/wslutils 

UNIT_EXES+=c_test_stack_update_table_basics
c_test_stack_update_table_basics_SRC=test/test_stack_update_table_basics.cpp
c_test_stack_update_table_basics_LIB=client/wslconnector custom/wslcustom utils/wslutils 

UNIT_EXES+=c_test_stack_update_table_array
c_test_stack_update_table_array_SRC=test/test_stack_update_table_array.cpp
c_test_stack_update_table_array_LIB=client/wslconnector custom/wslcustom utils/wslutils 

UNIT_EXES+=c_test_stack_update_text
c_test_stack_update_text_SRC=test/test_stack_update_text.cpp
c_test_stack_update_text_LIB=client/wslconnector custom/wslcustom utils/wslutils 

#Disable this momentarly as it taske too long to link for PPC targets
ifeq ($(findstring gcc_ppc,$(ARCH)),)
ifeq ($(findstring _vc_x86,$(ARCH)),)
#UNIT_EXES+=c_test_glb_list_2
#UNIT_EXES+=c_test_proc_list_2
endif
endif

