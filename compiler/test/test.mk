UNIT_EXES+=test_yylex
test_yylex_SRC=test/test_yylex.c 
test_yylex_LIB=compiler/compiler utils/utils custom/custom
test_yylex_INC=utils/include custom/include

UNIT_EXES+=test_a_decls
test_a_decls_SRC=test/test_a_decls.c 
test_a_decls_LIB=compiler/compiler utils/utils custom/custom
test_a_decls_INC=utils/include custom/include

UNIT_EXES+=test_b_decls
test_b_decls_SRC=test/test_b_decls.c 
test_b_decls_LIB=compiler/compiler utils/utils custom/custom
test_b_decls_INC=utils/include custom/include

UNIT_EXES+=test_c_decls
test_c_decls_SRC=test/test_c_decls.c 
test_c_decls_LIB=compiler/compiler utils/utils custom/custom
test_c_decls_INC=utils/include custom/include

UNIT_EXES+=test_p_decls
test_p_decls_SRC=test/test_p_decls.c 
test_p_decls_LIB=compiler/compiler utils/utils custom/custom
test_p_decls_INC=utils/include custom/include

UNIT_EXES+=test_add_op
test_add_op_SRC=test/test_add_op.c 
test_add_op_LIB=compiler/compiler utils/utils custom/custom
test_add_op_INC=utils/include custom/include

UNIT_EXES+=test_sub_op
test_sub_op_SRC=test/test_sub_op.c 
test_sub_op_LIB=compiler/compiler utils/utils custom/custom
test_sub_op_INC=utils/include custom/include

UNIT_EXES+=test_mul_op
test_mul_op_SRC=test/test_mul_op.c 
test_mul_op_LIB=compiler/compiler utils/utils custom/custom
test_mul_op_INC=utils/include custom/include

UNIT_EXES+=test_div_op
test_div_op_SRC=test/test_div_op.c 
test_div_op_LIB=compiler/compiler utils/utils custom/custom
test_div_op_INC=utils/include custom/include

UNIT_EXES+=test_mod_op
test_mod_op_SRC=test/test_mod_op.c 
test_mod_op_LIB=compiler/compiler utils/utils custom/custom
test_mod_op_INC=utils/include custom/include

UNIT_EXES+=test_lt_op
test_lt_op_SRC=test/test_lt_op.c 
test_lt_op_LIB=compiler/compiler utils/utils custom/custom
test_lt_op_INC=utils/include custom/include

UNIT_EXES+=test_le_op
test_le_op_SRC=test/test_le_op.c 
test_le_op_LIB=compiler/compiler utils/utils custom/custom
test_le_op_INC=utils/include custom/include

UNIT_EXES+=test_gt_op
test_gt_op_SRC=test/test_gt_op.c 
test_gt_op_LIB=compiler/compiler utils/utils custom/custom
test_gt_op_INC=utils/include custom/include

UNIT_EXES+=test_ge_op
test_ge_op_SRC=test/test_ge_op.c 
test_ge_op_LIB=compiler/compiler utils/utils custom/custom
test_ge_op_INC=utils/include custom/include

UNIT_EXES+=test_eq_op
test_eq_op_SRC=test/test_eq_op.c 
test_eq_op_LIB=compiler/compiler utils/utils custom/custom
test_eq_op_INC=utils/include custom/include

UNIT_EXES+=test_ne_op
test_ne_op_SRC=test/test_ne_op.c 
test_ne_op_LIB=compiler/compiler utils/utils custom/custom
test_ne_op_INC=utils/include custom/include

UNIT_EXES+=test_and_op
test_and_op_SRC=test/test_and_op.c 
test_and_op_LIB=compiler/compiler utils/utils custom/custom
test_and_op_INC=utils/include custom/include

UNIT_EXES+=test_xor_op
test_xor_op_SRC=test/test_xor_op.c 
test_xor_op_LIB=compiler/compiler utils/utils custom/custom
test_xor_op_INC=utils/include custom/include

UNIT_EXES+=test_or_op
test_or_op_SRC=test/test_or_op.c 
test_or_op_LIB=compiler/compiler utils/utils custom/custom
test_or_op_INC=utils/include custom/include

UNIT_EXES+=test_not_op
test_not_op_SRC=test/test_not_op.c 
test_not_op_LIB=compiler/compiler utils/utils custom/custom
test_not_op_INC=utils/include custom/include

UNIT_EXES+=test_inc_op
test_inc_op_SRC=test/test_inc_op.c 
test_inc_op_LIB=compiler/compiler utils/utils custom/custom
test_inc_op_INC=utils/include custom/include

UNIT_EXES+=test_dec_op
test_dec_op_SRC=test/test_dec_op.c 
test_dec_op_LIB=compiler/compiler utils/utils custom/custom
test_dec_op_INC=utils/include custom/include

UNIT_EXES+=test_index_op
test_index_op_SRC=test/test_index_op.c 
test_index_op_LIB=compiler/compiler utils/utils custom/custom
test_index_op_INC=utils/include custom/include


;UNIT_EXES+=test_self_op
;test_self_op_SRC=test/test_self_op.c 
;test_self_op_LIB=compiler/compiler utils/utils custom/custom
;test_self_op_INC=utils/include custom/include

UNIT_EXES+=test_assign_op
test_assign_op_SRC=test/test_assign_op.c 
test_assign_op_LIB=compiler/compiler utils/utils custom/custom
test_assign_op_INC=utils/include custom/include

UNIT_EXES+=test_call_op
test_call_op_SRC=test/test_call_op.c 
test_call_op_LIB=compiler/compiler utils/utils custom/custom
test_call_op_INC=utils/include custom/include

UNIT_EXES+=test_ifstmts
test_ifstmts_SRC=test/test_ifstmts.c 
test_ifstmts_LIB=compiler/compiler utils/utils custom/custom
test_ifstmts_INC=utils/include custom/include

UNIT_EXES+=test_loopstmts
test_loopstmts_SRC=test/test_loopstmts.c 
test_loopstmts_LIB=compiler/compiler utils/utils custom/custom
test_loopstmts_INC=utils/include custom/include

UNIT_EXES+=test_msg_var_defined
test_msg_var_defined_SRC=test/test_msg_var_defined.c 
test_msg_var_defined_LIB=compiler/compiler utils/utils custom/custom
test_msg_var_defined_INC=utils/include custom/include

UNIT_EXES+=test_msg_not_declared
test_msg_not_declared_SRC=test/test_msg_not_declared.c 
test_msg_not_declared_LIB=compiler/compiler utils/utils custom/custom
test_msg_not_declared_INC=utils/include custom/include

;UNIT_EXES+=test_msg_not_table
;test_msg_not_table_SRC=test/test_msg_not_table.c 
;test_msg_not_table_LIB=compiler/compiler utils/utils custom/custom
;test_msg_not_table_INC=utils/include custom/include

UNIT_EXES+=test_msg_same_field
test_msg_same_field_SRC=test/test_msg_same_field.c 
test_msg_same_field_LIB=compiler/compiler utils/utils custom/custom
test_msg_same_field_INC=utils/include custom/include

UNIT_EXES+=test_msg_proc_adecl
test_msg_proc_adecl_SRC=test/test_msg_proc_adecl.c 
test_msg_proc_adecl_LIB=compiler/compiler utils/utils custom/custom
test_msg_proc_adecl_INC=utils/include custom/include

UNIT_EXES+=test_msg_incs
test_msg_incs_SRC=test/test_msg_incs.c 
test_msg_incs_LIB=compiler/compiler utils/utils custom/custom
test_msg_incs_INC=utils/include custom/include

UNIT_EXES+=test_msg_decs
test_msg_decs_SRC=test/test_msg_decs.c 
test_msg_decs_LIB=compiler/compiler utils/utils custom/custom
test_msg_decs_INC=utils/include custom/include

UNIT_EXES+=test_msg_index
test_msg_index_SRC=test/test_msg_index.c 
test_msg_index_LIB=compiler/compiler utils/utils custom/custom
test_msg_index_INC=utils/include custom/include

;UNIT_EXES+=test_msg_memsel
;test_msg_memsel_SRC=test/test_msg_memsel.c 
;test_msg_memsel_LIB=compiler/compiler utils/utils custom/custom
;test_msg_memsel_INC=utils/include custom/include

UNIT_EXES+=test_msg_store
test_msg_store_SRC=test/test_msg_store.c 
test_msg_store_LIB=compiler/compiler utils/utils custom/custom
test_msg_store_INC=utils/include custom/include

UNIT_EXES+=test_msg_proc_call
test_msg_proc_call_SRC=test/test_msg_proc_call.c 
test_msg_proc_call_LIB=compiler/compiler utils/utils custom/custom
test_msg_proc_call_INC=utils/include custom/include

UNIT_EXES+=test_msg_gen
test_msg_gen_SRC=test/test_msg_gen.c 
test_msg_gen_LIB=compiler/compiler utils/utils custom/custom
test_msg_gen_INC=utils/include custom/include