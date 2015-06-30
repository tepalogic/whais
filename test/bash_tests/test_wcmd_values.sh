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



check_rows_info () {
	wcmd -u $DB_NAME -d $DB_DIR <<< "rows $1 info" > $DB_FILE
	allock_chk=`grep "Allocated rows *: $2" $DB_FILE`
	reuse_chk=`grep "Reusable rows *: $3" $DB_FILE`
	first_chk=`grep "First reusable row *: $4" $DB_FILE`
	[ -n "$allock_chk" ] || fail_test "Table $1 failed to check allocated rows"
	[ -n "$reuse_chk" ] || fail_test "Table $1 failed to check reuse rows"
	[ -n "$first_chk" ] || fail_test "Table $1 failed to first reused rows"
	echo "Table $1 info ($2, $3, $4) check passed ... "
}


check_row_value () {
	wcmd -u $DB_NAME -d $DB_DIR <<< "rows $1 list $2" > $DB_FILE
	val_check=`cat $DB_FILE | cut -d \| -s -f 3`
	val_check="${val_check#[}"
	val_check="${val_check%]}"
	val_check=`echo ${val_check//\'/}`
	input="${3#[}"
	input="${input%]}"
	input=`echo ${input//\'/}`
#	echo "'$val_check'"
#	echo "'$input'"
	[ "$input" == "$val_check" ] || fail_test "Failed to check 'rows $1 list $2' against '$input'".
	echo "Check of 'rows $1 list $2' against '$input' has passed...".
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

fail_test () {
	echo "$1"
	echo "file"
	cat $DB_FILE
	rm -rf "$DB_DIR"
	echo "TEST RESULT:  FAIL"
	exit 1
}


mkdir -p $DB_DIR || fail_test "Failed to create temporal directory '$DB_DIR'."
wcmd -c $DB_NAME -d $DB_DIR <<< "echo $DB_NAME is created...."
[ $? -eq 0 ] || fail_test "Cannot create the database!"
execute_wcmd_command "add test_table bool_field BOOL char_field \
                     CHAR date_field DATE dt_field DATETIME ht_field HIRESTIME\
                     int8_field INT8 int16_field INT16 int32_field INT32 int64_field INT64\
                     uint8_field UINT8 uint16_field UINT16 uint32_field UINT32 uint64_field UINT64\
                     real_field REAL rr_field RICHREAL"

execute_wcmd_command "index test_table bool_field "
execute_wcmd_command "index test_table char_field date_field "
execute_wcmd_command "index test_table \
                     dt_field ht_field int8_field int16_field int32_field int64_field \
                     uint8_field uint16_field uint32_field uint64_field \
                     real_field rr_field "

check_rows_info "test_table" 0 0 0

check_table_fields "test_table" 'char_field *: CHAR (indexed)$' \
				'date_field *: DATE (indexed)$' \
				'dt_field *: DATETIME (indexed)$' \
				'ht_field *: HIRESTIME (indexed)$' \
				'int8_field *: INT8 (indexed)$' \
				'int16_field *: INT16 (indexed)$' \
				'int32_field *: INT32 (indexed)$' \
				'int64_field *: INT64 (indexed)$' \
				'uint8_field *: UINT8 (indexed)$' \
				'uint16_field *: UINT16 (indexed)$' \
				'uint32_field *: UINT32 (indexed)$' \
				'uint64_field *: UINT64 (indexed)$' \
				'real_field *: REAL (indexed)$' \
				'rr_field *: RICHREAL (indexed)$'





execute_wcmd_command "rows test_table add \
bool_field=min,\
char_field=min,\
date_field=min,\
dt_field=min,\
ht_field=min,\
int8_field=min,\
int16_field=min,\
int32_field=min,\
int64_field=min,\
uint8_field=min,\
uint16_field=min,\
uint32_field=min,\
uint64_field=min,\
real_field=min,\
rr_field=min"

check_rows_info "test_table" 1 0 1

execute_wcmd_command "rows test_table reuse \
bool_field=max,\
char_field=max,\
date_field=max,\
dt_field=max,\
ht_field=max,\
int8_field=max,\
int16_field=max,\
int32_field=max,\
int64_field=max,\
uint8_field=max,\
uint16_field=max,\
uint32_field=max,\
uint64_field=max,\
real_field=max,\
rr_field=max"


check_rows_info "test_table" 2 0 2

wcmd -u $DB_NAME -d $DB_DIR <<< "rows test_table list"
[ $? -eq 0 ] || fail_test "Failed to list the rows from 'test_table'."

check_table_fields "test_table" 'char_field *: CHAR (indexed)$' \
				'date_field *: DATE (indexed)$' \
				'dt_field *: DATETIME (indexed)$' \
				'ht_field *: HIRESTIME (indexed)$' \
				'int8_field *: INT8 (indexed)$' \
				'int16_field *: INT16 (indexed)$' \
				'int32_field *: INT32 (indexed)$' \
				'int64_field *: INT64 (indexed)$' \
				'uint8_field *: UINT8 (indexed)$' \
				'uint16_field *: UINT16 (indexed)$' \
				'uint32_field *: UINT32 (indexed)$' \
				'uint64_field *: UINT64 (indexed)$' \
				'real_field *: REAL (indexed)$' \
				'rr_field *: RICHREAL (indexed)$'



check_row_value "test_table" "bool_field 0" "0"
#check_row_value "test_table" "char_field 0" "(null)"
check_row_value "test_table" "date_field 0" "-32768/1/1"
check_row_value "test_table" "dt_field 0" "-32768/1/1 0:0:0"
check_row_value "test_table" "ht_field 0" "-32768/1/1 0:0:0.000000"
check_row_value "test_table" "int8_field 0" "-128"
check_row_value "test_table" "int16_field 0" "-32768"
check_row_value "test_table" "int32_field 0" "-2147483648"
check_row_value "test_table" "int64_field 0" "-9223372036854775808"
check_row_value "test_table" "uint8_field 0" "0"
check_row_value "test_table" "uint16_field 0" "0"
check_row_value "test_table" "uint32_field 0" "0"
check_row_value "test_table" "uint64_field 0" "0"
check_row_value "test_table" "real_field 0" "-549755813888.999999"
check_row_value "test_table" "rr_field 0" "-9223372036854775808.99999999999999"

check_row_value "test_table" "bool_field 1" "1"
#check_row_value "test_table" "char_field 1" "(null)"
check_row_value "test_table" "date_field 1" "32767/12/31"
check_row_value "test_table" "dt_field 1" "32767/12/31 23:59:59"
check_row_value "test_table" "ht_field 1" "32767/12/31 23:59:59.999999"
check_row_value "test_table" "int8_field 1" "127"
check_row_value "test_table" "int16_field 1" "32767"
check_row_value "test_table" "int32_field 1" "2147483647"
check_row_value "test_table" "int64_field 1" "9223372036854775807"
check_row_value "test_table" "uint8_field 1" "255"
check_row_value "test_table" "uint16_field 1" "65535"
check_row_value "test_table" "uint32_field 1" "4294967295"
check_row_value "test_table" "uint64_field 1" "18446744073709551615"
check_row_value "test_table" "real_field 1" "549755813887.999999"
check_row_value "test_table" "rr_field 1" "9223372036854775807.99999999999999"



execute_wcmd_command "rows test_table update \
bool_field=null,\
char_field=null,\
date_field=null,\
dt_field=null,\
ht_field=null,\
int8_field=null,\
int16_field=null,\
int32_field=null,\
int64_field=null,\
uint8_field=null,\
uint16_field=null,\
uint32_field=null,\
uint64_field=null,\
real_field=null,\
rr_field=null \
 * bool_field=min "

check_rows_info "test_table" 2 1 0

check_row_value "test_table" "bool_field 0" "(null)"
check_row_value "test_table" "char_field 0" "(null)"
check_row_value "test_table" "date_field 0" "(null)"
check_row_value "test_table" "dt_field 0" "(null)"
check_row_value "test_table" "ht_field 0" "(null)"
check_row_value "test_table" "int8_field 0" "(null)"
check_row_value "test_table" "int16_field 0" "(null)"
check_row_value "test_table" "int32_field 0" "(null)"
check_row_value "test_table" "int64_field 0" "(null)"
check_row_value "test_table" "uint8_field 0" "(null)"
check_row_value "test_table" "uint16_field 0" "(null)"
check_row_value "test_table" "uint32_field 0" "(null)"
check_row_value "test_table" "uint64_field 0" "(null)"
check_row_value "test_table" "real_field 0" "(null)"
check_row_value "test_table" "rr_field 0" "(null)"

check_table_fields "test_table" 'char_field *: CHAR (indexed)$' \
				'date_field *: DATE (indexed)' \
				'dt_field *: DATETIME (indexed)$' \
				'ht_field *: HIRESTIME (indexed)$' \
				'int8_field *: INT8 (indexed)' \
				'int16_field *: INT16 (indexed)$' \
				'int32_field *: INT32 (indexed)$' \
				'int64_field *: INT64 (indexed)$' \
				'uint8_field *: UINT8 (indexed)$' \
				'uint16_field *: UINT16 (indexed)$' \
				'uint32_field *: UINT32 (indexed)$' \
				'uint64_field *: UINT64 (indexed)$' \
				'real_field *: REAL (indexed)$' \
				'rr_field *: RICHREAL (indexed)$'


execute_wcmd_command "rmindex test_table bool_field "
execute_wcmd_command "rmindex test_table char_field date_field "
execute_wcmd_command "rmindex test_table \
                     dt_field ht_field int8_field int16_field int32_field int64_field \
                     uint8_field uint16_field uint32_field uint64_field \
                     real_field rr_field "

check_row_value "test_table" "bool_field 1" "1"
#check_row_value "test_table" "char_field 1" "(null)"
check_row_value "test_table" "date_field 1" "32767/12/31"
check_row_value "test_table" "dt_field 1" "32767/12/31 23:59:59"
check_row_value "test_table" "ht_field 1" "32767/12/31 23:59:59.999999"
check_row_value "test_table" "int8_field 1" "127"
check_row_value "test_table" "int16_field 1" "32767"
check_row_value "test_table" "int32_field 1" "2147483647"
check_row_value "test_table" "int64_field 1" "9223372036854775807"
check_row_value "test_table" "uint8_field 1" "255"
check_row_value "test_table" "uint16_field 1" "65535"
check_row_value "test_table" "uint32_field 1" "4294967295"
check_row_value "test_table" "uint64_field 1" "18446744073709551615"
check_row_value "test_table" "real_field 1" "549755813887.999999"
check_row_value "test_table" "rr_field 1" "9223372036854775807.99999999999999"

check_row_value "test_table" "bool_field 0" "(null)"
check_row_value "test_table" "char_field 0" "(null)"
check_row_value "test_table" "date_field 0" "(null)"
check_row_value "test_table" "dt_field 0" "(null)"
check_row_value "test_table" "ht_field 0" "(null)"
check_row_value "test_table" "int8_field 0" "(null)"
check_row_value "test_table" "int16_field 0" "(null)"
check_row_value "test_table" "int32_field 0" "(null)"
check_row_value "test_table" "int64_field 0" "(null)"
check_row_value "test_table" "uint8_field 0" "(null)"
check_row_value "test_table" "uint16_field 0" "(null)"
check_row_value "test_table" "uint32_field 0" "(null)"
check_row_value "test_table" "uint64_field 0" "(null)"
check_row_value "test_table" "real_field 0" "(null)"
check_row_value "test_table" "rr_field 0" "(null)"

check_table_fields "test_table" 'char_field *: CHAR$' \
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
				'rr_field *: RICHREAL$'

execute_wcmd_command "index test_table bool_field "
execute_wcmd_command "index test_table char_field date_field "
execute_wcmd_command "index test_table \
                     dt_field ht_field int8_field int16_field int32_field int64_field \
                     uint8_field uint16_field uint32_field uint64_field \
                     real_field rr_field "


check_row_value "test_table" "bool_field 1" "1"
#check_row_value "test_table" "char_field 1" "(null)"
check_row_value "test_table" "date_field 1" "32767/12/31"
check_row_value "test_table" "dt_field 1" "32767/12/31 23:59:59"
check_row_value "test_table" "ht_field 1" "32767/12/31 23:59:59.999999"
check_row_value "test_table" "int8_field 1" "127"
check_row_value "test_table" "int16_field 1" "32767"
check_row_value "test_table" "int32_field 1" "2147483647"
check_row_value "test_table" "int64_field 1" "9223372036854775807"
check_row_value "test_table" "uint8_field 1" "255"
check_row_value "test_table" "uint16_field 1" "65535"
check_row_value "test_table" "uint32_field 1" "4294967295"
check_row_value "test_table" "uint64_field 1" "18446744073709551615"
check_row_value "test_table" "real_field 1" "549755813887.999999"
check_row_value "test_table" "rr_field 1" "9223372036854775807.99999999999999"

check_row_value "test_table" "bool_field 0" "(null)"
check_row_value "test_table" "char_field 0" "(null)"
check_row_value "test_table" "date_field 0" "(null)"
check_row_value "test_table" "dt_field 0" "(null)"
check_row_value "test_table" "ht_field 0" "(null)"
check_row_value "test_table" "int8_field 0" "(null)"
check_row_value "test_table" "int16_field 0" "(null)"
check_row_value "test_table" "int32_field 0" "(null)"
check_row_value "test_table" "int64_field 0" "(null)"
check_row_value "test_table" "uint8_field 0" "(null)"
check_row_value "test_table" "uint16_field 0" "(null)"
check_row_value "test_table" "uint32_field 0" "(null)"
check_row_value "test_table" "uint64_field 0" "(null)"
check_row_value "test_table" "real_field 0" "(null)"
check_row_value "test_table" "rr_field 0" "(null)"

check_table_fields "test_table" 'char_field *: CHAR (indexed)$' \
				'date_field *: DATE (indexed)$' \
				'dt_field *: DATETIME (indexed)$' \
				'ht_field *: HIRESTIME (indexed)$' \
				'int8_field *: INT8 (indexed)$' \
				'int16_field *: INT16 (indexed)$' \
				'int32_field *: INT32 (indexed)$' \
				'int64_field *: INT64 (indexed)$' \
				'uint8_field *: UINT8 (indexed)$' \
				'uint16_field *: UINT16 (indexed)$' \
				'uint32_field *: UINT32 (indexed)$' \
				'uint64_field *: UINT64 (indexed)$' \
				'real_field *: REAL (indexed)$' \
				'rr_field *: RICHREAL (indexed)$'


rm -rf "$DB_DIR"
echo "TEST RESULT:  PASS"

