################################################################################
# Test module used to verify the handling of global variable description.
# 
# Iulian Popa
#
# Notes:
#  1. It contains a lot of procedures with long names to make sure one uses
#     more than one comm frame to list them.
#  2. Same things for table fields. It should facilitate the usage of multiple
#     comm frames for a full description.
################################################################################


PROCEDURE bool_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN BOOL
DO RETURN NULL; ENDPROC

PROCEDURE char_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN CHARACTER
DO RETURN NULL; ENDPROC

PROCEDURE date_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN DATE
DO RETURN NULL; ENDPROC

PROCEDURE datetime_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN DATETIME
DO RETURN NULL; ENDPROC

PROCEDURE hirestime_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN HIRESTIME
DO RETURN NULL; ENDPROC

PROCEDURE int8_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN INT8
DO RETURN NULL; ENDPROC

PROCEDURE int16_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN INT16
DO RETURN NULL; ENDPROC

PROCEDURE int32_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN INT32
DO RETURN NULL; ENDPROC

PROCEDURE int64_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN INT64
DO RETURN NULL; ENDPROC

PROCEDURE uint8_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN UNSIGNED INT8
DO RETURN NULL; ENDPROC

PROCEDURE uint16_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN UNSIGNED INT16
DO RETURN NULL; ENDPROC

PROCEDURE uint32_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN UNSIGNED INT32
DO RETURN NULL; ENDPROC

PROCEDURE uint64_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN UNSIGNED INT64
DO RETURN NULL; ENDPROC

PROCEDURE real_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN REAL
DO RETURN NULL; ENDPROC

PROCEDURE richreal_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN RICHREAL
DO RETURN NULL; ENDPROC

PROCEDURE text_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN TEXT
DO RETURN NULL; ENDPROC


#
# Array returned values part.
#

PROCEDURE array_bool_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN ARRAY OF BOOL
DO RETURN NULL; ENDPROC

PROCEDURE array_char_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN ARRAY OF CHARACTER
DO RETURN NULL; ENDPROC

PROCEDURE array_date_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN ARRAY OF DATE
DO RETURN NULL; ENDPROC

PROCEDURE array_datetime_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN ARRAY OF DATETIME
DO RETURN NULL; ENDPROC

PROCEDURE array_hirestime_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN ARRAY OF HIRESTIME
DO RETURN NULL; ENDPROC

PROCEDURE array_int8_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN ARRAY OF INT8
DO RETURN NULL; ENDPROC

PROCEDURE array_int16_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN ARRAY OF INT16
DO RETURN NULL; ENDPROC

PROCEDURE array_int32_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN ARRAY OF INT32
DO RETURN NULL; ENDPROC

PROCEDURE array_int64_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN ARRAY OF INT64
DO RETURN NULL; ENDPROC

PROCEDURE array_uint8_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN ARRAY OF UNSIGNED INT8
DO RETURN NULL; ENDPROC

PROCEDURE array_uint16_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN ARRAY OF UNSIGNED INT16
DO RETURN NULL; ENDPROC

PROCEDURE array_uint32_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN ARRAY OF UNSIGNED INT32
DO RETURN NULL; ENDPROC

PROCEDURE array_uint64_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN ARRAY OF UNSIGNED INT64
DO RETURN NULL; ENDPROC

PROCEDURE array_real_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN ARRAY OF REAL
DO RETURN NULL; ENDPROC

PROCEDURE array_richreal_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN ARRAY OF RICHREAL
DO RETURN NULL; ENDPROC

#PROCEDURE array_text_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN ARRAY OF TEXT
#DO RETURN NULL; ENDPROC

PROCEDURE array_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN ARRAY 
DO RETURN NULL; ENDPROC

#
# Field returned values part.
#

PROCEDURE field_bool_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF BOOL
DO RETURN NULL; ENDPROC

PROCEDURE field_char_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF CHARACTER
DO RETURN NULL; ENDPROC

PROCEDURE field_date_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF DATE
DO RETURN NULL; ENDPROC

PROCEDURE field_datetime_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF DATETIME
DO RETURN NULL; ENDPROC

PROCEDURE field_hirestime_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF HIRESTIME
DO RETURN NULL; ENDPROC

PROCEDURE field_int8_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF INT8
DO RETURN NULL; ENDPROC

PROCEDURE field_int16_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF INT16
DO RETURN NULL; ENDPROC

PROCEDURE field_int32_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF INT32
DO RETURN NULL; ENDPROC

PROCEDURE field_int64_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF INT64
DO RETURN NULL; ENDPROC

PROCEDURE field_uint8_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF UNSIGNED INT8
DO RETURN NULL; ENDPROC

PROCEDURE field_uint16_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF UNSIGNED INT16
DO RETURN NULL; ENDPROC

PROCEDURE field_uint32_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF UNSIGNED INT32
DO RETURN NULL; ENDPROC

PROCEDURE field_uint64_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF UNSIGNED INT64
DO RETURN NULL; ENDPROC

PROCEDURE field_real_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF REAL
DO RETURN NULL; ENDPROC

PROCEDURE field_richreal_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF RICHREAL
DO RETURN NULL; ENDPROC

PROCEDURE field_text_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF TEXT
DO RETURN NULL; ENDPROC

#
# Field array returned values part.
#

PROCEDURE field_array_bool_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF ARRAY OF BOOL
DO RETURN NULL; ENDPROC

PROCEDURE field_array_char_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF ARRAY OF CHARACTER
DO RETURN NULL; ENDPROC

PROCEDURE field_array_date_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF ARRAY OF DATE
DO RETURN NULL; ENDPROC

PROCEDURE field_array_datetime_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF ARRAY OF DATETIME
DO RETURN NULL; ENDPROC

PROCEDURE field_array_hirestime_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF ARRAY OF HIRESTIME
DO RETURN NULL; ENDPROC

PROCEDURE field_array_int8_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF ARRAY OF INT8
DO RETURN NULL; ENDPROC

PROCEDURE field_array_int16_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF ARRAY OF INT16
DO RETURN NULL; ENDPROC

PROCEDURE field_array_int32_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF ARRAY OF INT32
DO RETURN NULL; ENDPROC

PROCEDURE field_array_int64_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF ARRAY OF INT64
DO RETURN NULL; ENDPROC

PROCEDURE field_array_uint8_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF ARRAY OF UNSIGNED INT8
DO RETURN NULL; ENDPROC

PROCEDURE field_array_uint16_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF ARRAY OF UNSIGNED INT16
DO RETURN NULL; ENDPROC

PROCEDURE field_array_uint32_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF ARRAY OF UNSIGNED INT32
DO RETURN NULL; ENDPROC

PROCEDURE field_array_uint64_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF ARRAY OF UNSIGNED INT64
DO RETURN NULL; ENDPROC

PROCEDURE field_array_real_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF ARRAY OF REAL
DO RETURN NULL; ENDPROC

PROCEDURE field_array_richreal_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF ARRAY OF RICHREAL
DO RETURN NULL; ENDPROC

#PROCEDURE field_array_text_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF ARRAY OF TEXT
#DO RETURN NULL; ENDPROC

PROCEDURE field_array_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD OF ARRAY 
DO RETURN NULL; ENDPROC

PROCEDURE field_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN FIELD 
DO RETURN NULL; ENDPROC

#
# Table returned values part.
#

PROCEDURE one_field_table_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN TABLE OF ( field1 AS TEXT) 
DO RETURN NULL; ENDPROC

PROCEDURE two_field_table_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN TABLE OF ( field1 AS CHARACTER, field2 AS ARRAY OF UNSIGNED INT64) 
DO RETURN NULL; ENDPROC

PROCEDURE table_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good () RETURN TABLE 
DO RETURN NULL; ENDPROC

#
# The complex procedure definition
#

PROCEDURE table_return_proc_all_type_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good (
	bool_arg AS BOOL,
	char_arg AS CHARACTER,
	date_arg AS DATE,
	datetime_arg AS DATETIME,
	hirestime_arg AS HIRESTIME,
	int8_arg AS INT8,
	int16_arg AS INT16,
	int32_arg AS INT32,
	int64_arg AS INT64,
	uint8_arg AS UNSIGNED INT8,
	uint16_arg AS UNSIGNED INT16,
	uint32_arg AS UNSIGNED INT32,
	uint64_arg AS UNSIGNED INT64,
	real_arg AS REAL,
	richreal_arg AS RICHREAL,
	text_arg AS TEXT,

	array_bool_arg AS ARRAY OF BOOL,
	array_char_arg AS ARRAY OF CHARACTER,
	array_date_arg AS ARRAY OF DATE,
	array_datetime_arg AS ARRAY OF DATETIME,
	array_hirestime_arg AS ARRAY OF HIRESTIME,
	array_int8_arg AS ARRAY OF INT8,
	array_int16_arg AS ARRAY OF INT16,
	array_int32_arg AS ARRAY OF INT32,
	array_int64_arg AS ARRAY OF INT64,
	array_uint8_arg AS ARRAY OF UNSIGNED INT8,
	array_uint16_arg AS ARRAY OF UNSIGNED INT16,
	array_uint32_arg AS ARRAY OF UNSIGNED INT32,
	array_uint64_arg AS ARRAY OF UNSIGNED INT64,
	array_real_arg AS ARRAY OF REAL,
	array_richreal_arg AS ARRAY OF RICHREAL,
#	array_text_arg AS ARRAY OF TEXT,
	array_arg AS ARRAY,

	field_bool_arg AS FIELD OF BOOL,
	field_char_arg AS FIELD OF CHARACTER,
	field_date_arg AS FIELD OF DATE,
	field_datetime_arg AS FIELD OF DATETIME,
	field_hirestime_arg AS FIELD OF HIRESTIME,
	field_int8_arg AS FIELD OF INT8,
	field_int16_arg AS FIELD OF INT16,
	field_int32_arg AS FIELD OF INT32,
	field_int64_arg AS FIELD OF INT64,
	field_uint8_arg AS FIELD OF UNSIGNED INT8,
	field_uint16_arg AS FIELD OF UNSIGNED INT16,
	field_uint32_arg AS FIELD OF UNSIGNED INT32,
	field_uint64_arg AS FIELD OF UNSIGNED INT64,
	field_real_arg AS FIELD OF REAL,
	field_richreal_arg AS FIELD OF RICHREAL,
	field_text_arg AS FIELD OF TEXT,

	field_array_bool_arg AS FIELD OF ARRAY OF BOOL,
	field_array_char_arg AS FIELD OF ARRAY OF CHARACTER,
	field_array_date_arg AS FIELD OF ARRAY OF DATE,
	field_array_datetime_arg AS FIELD OF ARRAY OF DATETIME,
	field_array_hirestime_arg AS FIELD OF ARRAY OF HIRESTIME,
	field_array_int8_arg AS FIELD OF ARRAY OF INT8,
	field_array_int16_arg AS FIELD OF ARRAY OF INT16,
	field_array_int32_arg AS FIELD OF ARRAY OF INT32,
	field_array_int64_arg AS FIELD OF ARRAY OF INT64,
	field_array_uint8_arg AS FIELD OF ARRAY OF UNSIGNED INT8,
	field_array_uint16_arg AS FIELD OF ARRAY OF UNSIGNED INT16,
	field_array_uint32_arg AS FIELD OF ARRAY OF UNSIGNED INT32,
	field_array_uint64_arg AS FIELD OF ARRAY OF UNSIGNED INT64,
	field_array_real_arg AS FIELD OF ARRAY OF REAL,
	field_array_richreal_arg AS FIELD OF ARRAY OF RICHREAL,
#	field_array_text_arg AS FIELD OF ARRAY OF TEXT,
	field_array_arg AS FIELD OF ARRAY,
	field_arg AS FIELD,

	table_arg AS TABLE,
	table_complete_arg AS TABLE OF (
		bool_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS BOOL,
		char_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS CHARACTER,
		date_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS DATE,
		datetime_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS DATETIME,
		hirestime_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS HIRESTIME,
		int8_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS INT8,
		int16_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS INT16,
		int32_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS INT32,
		int64_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS INT64,
		uint8_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS UNSIGNED INT8,
		uint16_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS UNSIGNED INT16,
		uint32_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS UNSIGNED INT32,
		uint64_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS UNSIGNED INT64,
		real_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS REAL,
		richreal_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS RICHREAL,
		text_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS TEXT,

		array_bool_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF BOOL,
		array_char_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF CHARACTER,
		array_date_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF DATE,
		array_datetime_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF DATETIME,
		array_hirestime_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF HIRESTIME,
		array_int8_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF INT8,
		array_int16_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF INT16,
		array_int32_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF INT32,
		array_int64_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF INT64,
		array_uint8_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF UNSIGNED INT8,
		array_uint16_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF UNSIGNED INT16,
		array_uint32_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF UNSIGNED INT32,
		array_uint64_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF UNSIGNED INT64,
		array_real_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF REAL,
		array_richreal_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF RICHREAL))
#		array_text_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF TEXT))
RETURN TABLE OF (
		bool_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS BOOL,
		char_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS CHARACTER,
		date_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS DATE,
		datetime_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS DATETIME,
		hirestime_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS HIRESTIME,
		int8_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS INT8,
		int16_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS INT16,
		int32_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS INT32,
		int64_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS INT64,
		uint8_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS UNSIGNED INT8,
		uint16_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS UNSIGNED INT16,
		uint32_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS UNSIGNED INT32,
		uint64_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS UNSIGNED INT64,
		real_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS REAL,
		richreal_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS RICHREAL,
		text_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS TEXT,

		array_bool_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF BOOL,
		array_char_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF CHARACTER,
		array_date_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF DATE,
		array_datetime_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF DATETIME,
		array_hirestime_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF HIRESTIME,
		array_int8_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF INT8,
		array_int16_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF INT16,
		array_int32_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF INT32,
		array_int64_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF INT64,
		array_uint8_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF UNSIGNED INT8,
		array_uint16_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF UNSIGNED INT16,
		array_uint32_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF UNSIGNED INT32,
		array_uint64_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF UNSIGNED INT64,
		array_real_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF REAL,
		array_richreal_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF RICHREAL)
#		array_text_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad AS ARRAY OF TEXT)
DO RETURN table_complete_arg; ENDPROC

