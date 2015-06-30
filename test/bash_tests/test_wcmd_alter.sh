#!/bin/bash

DB_NAME=t_wcmd_add
DB_DIR=./tmp
DB_FILE=$DB_DIR/temp_file


execute_wcmd_command () {
	wcmd -u $DB_NAME -d $DB_DIR <<< "$1" > $DB_FILE
	[ $? -eq 0 ] || fail_test "Failed to execute test command: $1"
	err_chk=`cat $DB_FILE | grep "\<FAIL\>"`
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
	echo "Testing fields of table "$1" ... "
	wcmd -u $DB_NAME -d $DB_DIR <<< "table $1" > $DB_FILE
	[ $? -eq 0 ] || fail_test "Failed to list tables!"
	shift ;
	while [[ $# > 0 ]] ; do 
		table=`cat $DB_FILE | grep "$1"`
		[ -n "$table" ] || fail_test "Failing field test $1"
		echo "Passing field test $1."
		shift ;
	done
}

check_missing_table_fields () {
	echo "Testing lack of field for table "$1" ... "
	wcmd -u $DB_NAME -d $DB_DIR <<< "table $1" > $DB_FILE
	[ $? -eq 0 ] || fail_test "Failed to list tables!"
	shift ;
	while [[ $# > 0 ]] ; do 
		table=`cat $DB_FILE | grep "$1"`
		[ -z "$table" ] || fail_test "Field '$1' should not be present $1"
		echo "Passing missing field '$1' test."
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
check_table_fields "test_table_4" '^text_field *: TEXT$' '^an_array *: ARRAY OF CHAR$'
check_table_fields "test_table_5" '^the_char *: CHAR$'

execute_wcmd_command "alter test_table_3 rename text_field r_text_field, add new_field_char CHAR new_field_uint32 UINT32 new_field_a_rr ARRAY RICHREAL"
execute_wcmd_command "alter test_table_4 retype an_array ARRAY BOOL"
execute_wcmd_command "alter test_table_5 remove the_char, add the_char ARRAY CHAR"

check_table_fields "test_table_3" '^r_text_field *: TEXT$' \
			          '^new_field_char *: CHAR$'\
				  '^new_field_uint32 *: UINT32$'\
 				  '^new_field_a_rr *: ARRAY OF RICHREAL$'
check_missing_table_fields "test_table_3" '\<text_field\>'
check_table_fields "test_table_4" '^text_field *: TEXT$' '^an_array *: ARRAY OF BOOL$'
check_table_fields "test_table_5" '^the_char *: ARRAY OF CHAR$'
execute_wcmd_command "alter test_table_3 remove r_text_field new_field_a_rr, add text_field INT64"
check_missing_table_fields "test_table_3" '\<r_text_field\>' '\<new_field_a_rr\>'
check_table_fields "test_table_3" '^new_field_char *: CHAR$'\
				  '^new_field_uint32 *: UINT32$'\
 				  '^text_field *: INT64$'

echo "Starting the second part of the test .... "


execute_wcmd_command "rows test_table reuse text_field='This is a simple text',int8_field=1,rr_field=0.99"
execute_wcmd_command "rows test_table reuse text_field='This is another smart test',rr_field=-1.11"
execute_wcmd_command "rows test_table reuse a_int16_field=[5 -2 100 10 101 -10111]"
execute_wcmd_command "rows test_table add rr_field=10.13"
execute_wcmd_command "rows test_table reuse text_field='Once again another smart test test!'"


TEXT_FIELD_ROWS=`wcmd -u $DB_NAME -d $DB_DIR <<< "rows test_table list text_field" | cut -s -d \| -f 3`
INT8_FIELD_ROWS=`wcmd -u $DB_NAME -d $DB_DIR <<< "rows test_table list int8_field" | cut -s -d \| -f 3`
RR_FIELD_ROWS=`wcmd -u $DB_NAME -d $DB_DIR <<< "rows test_table list rr_field" | cut -s -d \| -f 3`
A_INT16_FIELD_ROWS=`wcmd -u $DB_NAME -d $DB_DIR <<< "rows test_table list a_int16_field" | cut -s -d \| -f 3`

echo "TEXT_FIELD_ROWS: "
echo "$TEXT_FIELD_ROWS"
echo "INT8_FIELD_ROWS: "
echo "$INT8_FIELD_ROWS"
echo "RR_FIELD_ROWS: "
echo "$RR_FIELD_ROWS"
echo "A_INT16_FIELD_ROWS: "
echo "$A_INT16_FIELD_ROWS"


execute_wcmd_command "alter test_table add new_int8 INT8 new_a_int8 ARRAY INT8, remove a_int64_field uint32_field"
execute_wcmd_command "alter test_table retype a_int16_field ARRAY INT32, rename text_field text_field_n"


check_table_fields "test_table" 'text_field_n *: TEXT$' \
				'char_field *: CHAR$' \
				'date_field *: DATE$' \
				'dt_field *: DATETIME$' \
				'ht_field *: HIRESTIME$' \
				'int8_field *: INT8$' \
				'new_int8 *: INT8$' \
				'new_a_int8 *: ARRAY OF INT8$' \
				'int16_field *: INT16$' \
				'int32_field *: INT32$' \
				'int64_field *: INT64$' \
				'uint8_field *: UINT8$' \
				'uint16_field *: UINT16$' \
				'uint64_field *: UINT64$' \
				'real_field *: REAL$' \
				'rr_field *: RICHREAL$' \
				'a_char_field *: ARRAY OF CHAR$' \
				'a_date_field *: ARRAY OF DATE$' \
				'a_dt_field *: ARRAY OF DATETIME$' \
				'a_ht_field *: ARRAY OF HIRESTIME$' \
				'a_int8_field *: ARRAY OF INT8$' \
				'a_int16_field *: ARRAY OF INT32$' \
				'a_int32_field *: ARRAY OF INT32$' \
				'a_uint8_field *: ARRAY OF UINT8$' \
				'a_uint16_field *: ARRAY OF UINT16$' \
				'a_uint32_field *: ARRAY OF UINT32$' \
				'a_uint64_field *: ARRAY OF UINT64$' \
				'a_real_field *: ARRAY OF REAL$' \
				'a_rr_field *: ARRAY OF RICHREAL$' \
				'a_rr_field *: ARRAY OF RICHREAL$'
check_missing_table_fields "test_table" '\<text_field\>' '\<a_int64_field\>' '\<uint32_field\>'

TEXT_FIELD_ROWS_2=`wcmd -u $DB_NAME -d $DB_DIR <<< "rows test_table list text_field_n" | cut -s -d \| -f 3`
INT8_FIELD_ROWS_2=`wcmd -u $DB_NAME -d $DB_DIR <<< "rows test_table list int8_field" | cut -s -d \| -f 3`
RR_FIELD_ROWS_2=`wcmd -u $DB_NAME -d $DB_DIR <<< "rows test_table list rr_field" | cut -s -d \| -f 3`
A_INT16_FIELD_ROWS_2=`wcmd -u $DB_NAME -d $DB_DIR <<< "rows test_table list a_int16_field" | cut -s -d \| -f 3`

[ "$TEXT_FIELD_ROWS" == "$TEXT_FIELD_ROWS_2" ] || fail_test "Text field rows does not match."
[ "$INT8_FIELD_ROWS" == "$INT8_FIELD_ROWS_2" ] || fail_test "Int8 field rows does not match."
[ "$RR_FIELD_ROWS" == "$RR_FIELD_ROWS_2" ] || fail_test "Int8 field rows does not match."
[ "$A_INT16_FIELD_ROWS" == "$A_INT16_FIELD_ROWS_2" ] || fail_test "Array field rows does not match."


rm -rf "$DB_DIR"
echo "TEST RESULT:  PASS"

