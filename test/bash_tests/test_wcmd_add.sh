#!/bin/bash

DB_NAME=t_wcmd_add
DB_DIR=./tmp
DB_FILE=$DB_DIR/temp_file


execute_wcmd_command () {
	wcmd -u $DB_NAME -d $DB_DIR <<< "$1" > $DB_FILE
	[ $? -eq 0 ] || fail_test "Failed to execute test command: $1"
	err_chk=`cat $DB_FILE | grep "\<FAIL\>"`
	echo ""
	[ -z "$err_chk"  ] || fail_test "Failed to execute test command: $1"
}

check_listed_table () {
	wcmd -u $DB_NAME -d $DB_DIR <<< "table" > $DB_FILE
	[ $? -eq 0 ] || fail_test "Failed to list tables!"
	echo ""
	while [[ $# > 0 ]] ; do 
		table=`cat $DB_FILE | grep "$1"`
		[ -n "$table" ] || fail_test "Missing table $1"
		echo "Found table $1."
		shift ;
	done
}

check_table_fields () {
	echo ""
	echo "Testing fields of table "$1" ... "
	wcmd -u $DB_NAME -d $DB_DIR <<< "table $1" > $DB_FILE
	[ $? -eq 0 ] || fail_test "Failed to list tables!"
	shift ;
	echo ""
	while [[ $# > 0 ]] ; do 
		table=`cat $DB_FILE | grep "$1"`
		[ -n "$table" ] || fail_test "Failing field test $1"
		echo "Passing field test $1."
		shift ;
	done
}

fail_test () {
	echo "$1"
	rm -rf "$DB_DIR"
	echo "TEST RESULT:  FAIL"
	exit 1
}


mkdir -p $DB_DIR || fail_test "Failed to create temporal directory '$DB_DIR'."
wcmd -c $DB_NAME -d $DB_DIR <<< "echo $DB_NAME is created...."
[ $? -eq 0 ] || fail_test "Cannot create the database!"
execute_wcmd_command "add test_table text_field TEXT bool_field BOOL char_field \
                     CHAR date_field DATE dt_field DATETIME ht_field HIRESTIME\
                     int8_field INT8 int16_field INT16 int32_field INT32 int64_field INT64\
                     uint8_field UINT8 uint16_field UINT16 uint32_field UINT32 uint64_field UINT64\
                     real_field REAL rr_field RICHREAL\
           	     a_bool_field ARRAY BOOL a_char_field ARRAY CHAR a_date_field ARRAY DATE a_dt_field ARRAY DATETIME a_ht_field ARRAY HIRESTIME\
                     a_int8_field ARRAY INT8 a_int16_field ARRAY INT16 a_int32_field ARRAY INT32 a_int64_field ARRAY INT64\
                     a_uint8_field ARRAY UINT8 a_uint16_field ARRAY UINT16 a_uint32_field ARRAY UINT32 a_uint64_field ARRAY UINT64\
                     a_real_field ARRAY REAL a_rr_field ARRAY RICHREAL; add test_table_2 one_field ARRAY REAL"
execute_wcmd_command "add test_table_3 text_field TEXT "
execute_wcmd_command "add test_table_4 text_field TEXT an_array ARRAY CHAR"
execute_wcmd_command "add test_table_5 the_char CHAR "
check_listed_table "test_table" "test_table_2" "test_table_3" "test_table_4" "test_table_5"
check_table_fields "test_table" 'text_field *: TEXT$' \
				'char_field *: CHAR$' \
				'date_field *: DATE$' \
				'dt_field *: DATETIME$' \
				'ht_field *: HIRESTIME$' \
				'int8_field *: INT8$' \
				'int16_field *: INT16$' \
				'int32_field *: INT32$' \
				'int64_field *: INT64$' \
				'uint8_field *: UINT8$' \
				'uint16_field *: UINT16$' \
				'uint32_field *: UINT32$' \
				'uint64_field *: UINT64$' \
				'real_field *: REAL$' \
				'rr_field *: RICHREAL$' \
				'a_char_field *: ARRAY OF CHAR$' \
				'a_date_field *: ARRAY OF DATE$' \
				'a_dt_field *: ARRAY OF DATETIME$' \
				'a_ht_field *: ARRAY OF HIRESTIME$' \
				'a_int8_field *: ARRAY OF INT8$' \
				'a_int16_field *: ARRAY OF INT16$' \
				'a_int32_field *: ARRAY OF INT32$' \
				'a_int64_field *: ARRAY OF INT64$' \
				'a_uint8_field *: ARRAY OF UINT8$' \
				'a_uint16_field *: ARRAY OF UINT16$' \
				'a_uint32_field *: ARRAY OF UINT32$' \
				'a_uint64_field *: ARRAY OF UINT64$' \
				'a_real_field *: ARRAY OF REAL$' \
				'a_rr_field *: ARRAY OF RICHREAL$'

check_table_fields "test_table_2" '^one_field *: ARRAY OF REAL$'
check_table_fields "test_table_3" '^text_field *: TEXT$'
check_table_fields "test_table_4" '^text_field *: TEXT$'
check_table_fields "test_table_4" '^an_array *: ARRAY OF CHAR$'
check_table_fields "test_table_5" '^the_char *: CHAR$'

rm -rf "$DB_DIR"
echo "TEST RESULT:  PASS"

