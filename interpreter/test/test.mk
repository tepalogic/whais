UNIT_EXES+=test_session
test_session_SRC=test/test_session.cpp
test_session_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom      

UNIT_EXES+=test_namespaces
test_namespaces_SRC=test/test_namespaces.cpp
test_namespaces_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom 

UNIT_EXES+=test_bool_operand
test_bool_operand_SRC=test/test_bool_operand.cpp
test_bool_operand_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom 

UNIT_EXES+=test_date_operand
test_date_operand_SRC=test/test_date_operand.cpp
test_date_operand_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom 

UNIT_EXES+=test_datetime_operand
test_datetime_operand_SRC=test/test_datetime_operand.cpp
test_datetime_operand_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom 

UNIT_EXES+=test_hirestime_operand
test_hirestime_operand_SRC=test/test_hirestime_operand.cpp
test_hirestime_operand_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom

UNIT_EXES+=test_uint8_operand
test_uint8_operand_SRC=test/test_uint8_operand.cpp
test_uint8_operand_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom

UNIT_EXES+=test_uint16_operand
test_uint16_operand_SRC=test/test_uint16_operand.cpp
test_uint16_operand_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom

UNIT_EXES+=test_uint32_operand
test_uint32_operand_SRC=test/test_uint32_operand.cpp
test_uint32_operand_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom

UNIT_EXES+=test_uint64_operand
test_uint64_operand_SRC=test/test_uint64_operand.cpp
test_uint64_operand_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom

UNIT_EXES+=test_int8_operand
test_int8_operand_SRC=test/test_int8_operand.cpp
test_int8_operand_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom

UNIT_EXES+=test_int16_operand
test_int16_operand_SRC=test/test_int16_operand.cpp
test_int16_operand_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom

UNIT_EXES+=test_int32_operand
test_int32_operand_SRC=test/test_int32_operand.cpp
test_int32_operand_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom

UNIT_EXES+=test_int64_operand
test_int64_operand_SRC=test/test_int64_operand.cpp
test_int64_operand_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom

UNIT_EXES+=test_real_operand
test_real_operand_SRC=test/test_real_operand.cpp
test_real_operand_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom

UNIT_EXES+=test_richreal_operand
test_richreal_operand_SRC=test/test_richreal_operand.cpp
test_richreal_operand_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom

UNIT_EXES+=test_char_operand
test_char_operand_SRC=test/test_char_operand.cpp
test_char_operand_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom

UNIT_EXES+=test_nativeobj_operand
test_nativeobj_operand_SRC=test/test_nativeobj_operand.cpp
test_nativeobj_operand_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom

UNIT_EXES+=test_ops_ldXX
test_ops_ldXX_SRC=test/test_ops_ldXX.cpp
test_ops_ldXX_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom

UNIT_EXES+=test_ops_stXX
test_ops_stXX_SRC=test/test_ops_stXX.cpp
test_ops_stXX_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom

UNIT_EXES+=test_ops_bin_arithm
test_ops_bin_arithm_SRC=test/test_ops_bin_arithm.cpp
test_ops_bin_arithm_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom

UNIT_EXES+=test_ops_bin_comp
test_ops_bin_comp_SRC=test/test_ops_bin_comp.cpp
test_ops_bin_comp_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom

UNIT_EXES+=test_ops_bits
test_ops_bits_SRC=test/test_ops_bits.cpp
test_ops_bits_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom

UNIT_EXES+=test_ops_generic
test_ops_generic_SRC=test/test_ops_generic.cpp
test_ops_generic_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom

UNIT_EXES+=test_ops_selfs
test_ops_selfs_SRC=test/test_ops_selfs.cpp
test_ops_selfs_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom

UNIT_EXES+=test_globals_desc
test_globals_desc_SRC=test/test_globals_desc.cpp
test_globals_desc_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom 

UNIT_EXES+=test_procs_decl
test_procs_decl_SRC=test/test_procs_decl.cpp
test_procs_decl_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom 

UNIT_EXES+=test_stackvalue_size
test_stackvalue_size_SRC=test/test_stackvalue_size.cpp
test_stackvalue_size_LIB=dbs/wslpastra  custom/wslcommon interpreter/wslprima compiler/wslcompiler utils/wslutils custom/wslcustom 

