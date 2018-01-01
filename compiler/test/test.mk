UNIT_EXES+=test_yylex
test_yylex_SRC=test/test_yylex.c 
test_yylex_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_a_decls
test_a_decls_SRC=test/test_a_decls.c 
test_a_decls_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_b_decls
test_b_decls_SRC=test/test_b_decls.c 
test_b_decls_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_c_decls
test_c_decls_SRC=test/test_c_decls.c 
test_c_decls_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_p_decls
test_p_decls_SRC=test/test_p_decls.c 
test_p_decls_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_add_op
test_add_op_SRC=test/test_add_op.c 
test_add_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_sub_op
test_sub_op_SRC=test/test_sub_op.c 
test_sub_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_mul_op
test_mul_op_SRC=test/test_mul_op.c 
test_mul_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_div_op
test_div_op_SRC=test/test_div_op.c 
test_div_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_mod_op
test_mod_op_SRC=test/test_mod_op.c 
test_mod_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_lt_op
test_lt_op_SRC=test/test_lt_op.c 
test_lt_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_le_op
test_le_op_SRC=test/test_le_op.c 
test_le_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_gt_op
test_gt_op_SRC=test/test_gt_op.c 
test_gt_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_ge_op
test_ge_op_SRC=test/test_ge_op.c 
test_ge_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_eq_op
test_eq_op_SRC=test/test_eq_op.c 
test_eq_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_ne_op
test_ne_op_SRC=test/test_ne_op.c 
test_ne_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc
test_ne_op_INC=utils/include custom/include

UNIT_EXES+=test_and_op
test_and_op_SRC=test/test_and_op.c 
test_and_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_xor_op
test_xor_op_SRC=test/test_xor_op.c 
test_xor_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_or_op
test_or_op_SRC=test/test_or_op.c 
test_or_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_not_op
test_not_op_SRC=test/test_not_op.c 
test_not_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_index_op
test_index_op_SRC=test/test_index_op.c 
test_index_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_assign_op
test_assign_op_SRC=test/test_assign_op.c 
test_assign_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_call_op
test_call_op_SRC=test/test_call_op.c 
test_call_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_ifstmts
test_ifstmts_SRC=test/test_ifstmts.c 
test_ifstmts_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_loopstmts
test_loopstmts_SRC=test/test_loopstmts.c 
test_loopstmts_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_msg_var_defined
test_msg_var_defined_SRC=test/test_msg_var_defined.c 
test_msg_var_defined_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_msg_not_declared
test_msg_not_declared_SRC=test/test_msg_not_declared.c 
test_msg_not_declared_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_msg_same_field
test_msg_same_field_SRC=test/test_msg_same_field.c 
test_msg_same_field_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_msg_proc_adecl
test_msg_proc_adecl_SRC=test/test_msg_proc_adecl.c 
test_msg_proc_adecl_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_msg_index
test_msg_index_SRC=test/test_msg_index.c 
test_msg_index_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_msg_store
test_msg_store_SRC=test/test_msg_store.c 
test_msg_store_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_msg_proc_call
test_msg_proc_call_SRC=test/test_msg_proc_call.c 
test_msg_proc_call_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_msg_gen
test_msg_gen_SRC=test/test_msg_gen.c 
test_msg_gen_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_self_op
test_self_op_SRC=test/test_self_op.c 
test_self_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_msg_memsel
test_msg_memsel_SRC=test/test_msg_memsel.c 
test_msg_memsel_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_nulls_op
test_nulls_op_SRC=test/test_nulls_op.c 
test_nulls_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_sadd_op
test_sadd_op_SRC=test/test_sadd_op.c 
test_sadd_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_ssub_op
test_ssub_op_SRC=test/test_ssub_op.c 
test_ssub_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_smul_op
test_smul_op_SRC=test/test_smul_op.c 
test_smul_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_sdiv_op
test_sdiv_op_SRC=test/test_sdiv_op.c 
test_sdiv_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_smod_op
test_smod_op_SRC=test/test_smod_op.c 
test_smod_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_sand_op
test_sand_op_SRC=test/test_sand_op.c 
test_sand_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_sxor_op
test_sxor_op_SRC=test/test_sxor_op.c 
test_sxor_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_sor_op
test_sor_op_SRC=test/test_sor_op.c 
test_sor_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_sel_op
test_sel_op_SRC=test/test_sel_op.c 
test_sel_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_row_copy
test_row_copy_SRC=test/test_row_copy.c 
test_row_copy_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc


UNIT_EXES+=test_msg_sops
test_msg_sops_SRC=test/test_msg_sops.c 
test_msg_sops_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_msg_sel_op
test_msg_sel_op_SRC=test/test_msg_sel_op.c 
test_msg_sel_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_msg_row_copy
test_msg_row_copy_SRC=test/test_msg_row_copy.c 
test_msg_row_copy_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc


UNIT_EXES+=test_itoffset_op
test_itoffset_op_SRC=test/test_itoffset_op.c 
test_itoffset_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc

UNIT_EXES+=test_fid_op
test_fid_op_SRC=test/test_fid_op.c 
test_fid_op_LIB=compiler/wslcompiler utils/wslutils custom/wslcustom custom/wslcppmemalloc
