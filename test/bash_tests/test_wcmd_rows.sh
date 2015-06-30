#!/bin/bash

DB_NAME=t_wcmd_add
DB_DIR=./tmp
DB_FILE=$DB_DIR/temp_file

TEXT_VALUE='This is test table value'
BOOL_VALUE='1'
CHAR_VALUE='}'
DATE_VALUE='2019/11/1'
DT_VALUE='-119/11/1 21:1:1'
HT_VALUE='211/1/21 4:2:1.120000'
INT8_VALUE='17'
INT16_VALUE='-7891'
INT32_VALUE='1992331'
INT64_VALUE='-188191111876987'
UINT8_VALUE='123'
UINT16_VALUE='1981'
UINT32_VALUE='10911233'
UINT64_VALUE='188191111991292'
REAL_VALUE='1.0'
RREAL_VALUE='-0.1'

A_BOOL_VALUE="[ 1 0 ]"
A_CHAR_VALUE="[ '}' 'z' 't' ]"
A_DATE_VALUE="[ '2019/11/1' ]"
A_DT_VALUE="[ '-119/11/1 21:1:1' ]"
A_HT_VALUE="[ '211/1/21 4:2:1.120000' ]"
A_INT8_VALUE="[ 17 1 -5 -7 ]"
A_INT16_VALUE="[ -7891 -1]"
A_INT32_VALUE="[ 1992331 ]"
A_INT64_VALUE="[ -188191111876987 ]"
A_UINT8_VALUE="[ 123 ]"
A_UINT16_VALUE="[ 1981 ]"
A_UINT32_VALUE="[ 10911233 ]"
A_UINT64_VALUE="[ 188191111991292 ]"
A_REAL_VALUE="[ 1.0 0.1 0.0012 ]"
A_RREAL_VALUE="[ -0.1 2.0 -3.0 -0.0911081 ]"



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
execute_wcmd_command "add test_table text_field TEXT bool_field BOOL char_field \
                     CHAR date_field DATE dt_field DATETIME ht_field HIRESTIME\
                     int8_field INT8 int16_field INT16 int32_field INT32 int64_field INT64\
                     uint8_field UINT8 uint16_field UINT16 uint32_field UINT32 uint64_field UINT64\
                     real_field REAL rr_field RICHREAL\
           	     a_bool_field ARRAY BOOL a_char_field ARRAY CHAR a_date_field ARRAY DATE a_dt_field ARRAY DATETIME a_ht_field ARRAY HIRESTIME\
                     a_int8_field ARRAY INT8 a_int16_field ARRAY INT16 a_int32_field ARRAY INT32 a_int64_field ARRAY INT64\
                     a_uint8_field ARRAY UINT8 a_uint16_field ARRAY UINT16 a_uint32_field ARRAY UINT32 a_uint64_field ARRAY UINT64\
                     a_real_field ARRAY REAL a_rr_field ARRAY RICHREAL "

check_rows_info "test_table" 0 0 0

execute_wcmd_command "rows test_table add text_field='$TEXT_VALUE'"
execute_wcmd_command "rows test_table add bool_field=$BOOL_VALUE"
execute_wcmd_command "rows test_table add char_field='$CHAR_VALUE'"
execute_wcmd_command "rows test_table add date_field='$DATE_VALUE'"
execute_wcmd_command "rows test_table add dt_field='$DT_VALUE'"
execute_wcmd_command "rows test_table add ht_field='$HT_VALUE'"
execute_wcmd_command "rows test_table add int8_field=$INT8_VALUE"
execute_wcmd_command "rows test_table add int16_field=$INT16_VALUE"
execute_wcmd_command "rows test_table add int32_field=$INT32_VALUE"
execute_wcmd_command "rows test_table add int64_field=$INT64_VALUE"
execute_wcmd_command "rows test_table add uint8_field=$UINT8_VALUE;"
execute_wcmd_command "rows test_table add uint16_field=$UINT16_VALUE; "
execute_wcmd_command "rows test_table add uint32_field=$UINT32_VALUE; "
execute_wcmd_command "rows test_table add uint64_field=$UINT64_VALUE; "
execute_wcmd_command "rows test_table add real_field=$REAL_VALUE; "
execute_wcmd_command "rows test_table add rr_field=$RREAL_VALUE; "
check_rows_info "test_table" 16 0 16
execute_wcmd_command "rows test_table reuse date_field='$DATE_VALUE',int8_field=$INT8_VALUE "
execute_wcmd_command "rows test_table reuse text_field='$DATE_VALUE',bool_field=$BOOL_VALUE,ht_field='$HT_VALUE' "
check_rows_info "test_table" 18 0 18
execute_wcmd_command "rows test_table remove * int16_field=min@max "
check_rows_info "test_table" 18 1 7
execute_wcmd_command "rows test_table reuse int16_field=$INT16_VALUE"
execute_wcmd_command "rows test_table remove * int16_field=null dt_field=min@max"
check_rows_info "test_table" 18 1 4
execute_wcmd_command "rows test_table reuse dt_field='$DT_VALUE'"
check_rows_info "test_table" 18 0 18
execute_wcmd_command "rows test_table remove 1"
check_rows_info "test_table" 18 1 1
execute_wcmd_command "rows test_table reuse bool_field=$BOOL_VALUE"
check_rows_info "test_table" 18 0 18
execute_wcmd_command "rows test_table remove 1-10 int8_field=min@max"
check_rows_info "test_table" 18 1 6
execute_wcmd_command "rows test_table remove * int8_field=min@max"
check_rows_info "test_table" 18 2 6
execute_wcmd_command "rows test_table remove * ht_field=min@max bool_field=min@max"
check_rows_info "test_table" 18 3 6
execute_wcmd_command "rows test_table reuse int8_field=$INT8_VALUE"
check_rows_info "test_table" 18 2 16
execute_wcmd_command "rows test_table reuse date_field='$DATE_VALUE',int8_field=$INT8_VALUE "
check_rows_info "test_table" 18 1 17
execute_wcmd_command "rows test_table reuse text_field='$DATE_VALUE',bool_field=$BOOL_VALUE,ht_field='$HT_VALUE' "
check_rows_info "test_table" 18 0 18

execute_wcmd_command "rows test_table list"

check_row_value "test_table" "text_field 0" "$TEXT_VALUE"
check_row_value "test_table" "bool_field 1" "$BOOL_VALUE"
check_row_value "test_table" "char_field 2" "$CHAR_VALUE"
check_row_value "test_table" "date_field 3" "$DATE_VALUE"
check_row_value "test_table" "dt_field 4" "$DT_VALUE"
check_row_value "test_table" "ht_field 5" "$HT_VALUE"
check_row_value "test_table" "int8_field 6" "$INT8_VALUE"
check_row_value "test_table" "int16_field 7" "$INT16_VALUE"
check_row_value "test_table" "int32_field 8" "$INT32_VALUE"
check_row_value "test_table" "int64_field 9" "$INT64_VALUE"
check_row_value "test_table" "uint8_field 10" "$UINT8_VALUE"
check_row_value "test_table" "uint16_field 11" "$UINT16_VALUE"
check_row_value "test_table" "uint32_field 12" "$UINT32_VALUE"
check_row_value "test_table" "uint64_field 13" "$UINT64_VALUE"
check_row_value "test_table" "real_field 14" "$REAL_VALUE"
check_row_value "test_table" "rr_field 15" "$RREAL_VALUE"
check_row_value "test_table" "date_field 16" "$DATE_VALUE"
check_row_value "test_table" "int8_field 16" "$INT8_VALUE"
check_row_value "test_table" "text_field 17" "$DATE_VALUE"
check_row_value "test_table" "bool_field 17" "$BOOL_VALUE"
check_row_value "test_table" "ht_field 17" "$HT_VALUE"

check_row_value "test_table" "text_field 1" "(null)"
check_row_value "test_table" "bool_field 2" "(null)"
check_row_value "test_table" "char_field 3" "(null)"
check_row_value "test_table" "date_field 4" "(null)"
check_row_value "test_table" "dt_field 5" "(null)"
check_row_value "test_table" "ht_field 6" "(null)"
check_row_value "test_table" "int8_field 7" "(null)"
check_row_value "test_table" "int16_field 8" "(null)"
check_row_value "test_table" "int32_field 9" "(null)"
check_row_value "test_table" "int64_field 10" "(null)"
check_row_value "test_table" "uint8_field 11" "(null)"
check_row_value "test_table" "uint16_field 12" "(null)"
check_row_value "test_table" "uint32_field 13" "(null)"
check_row_value "test_table" "uint64_field 14" "(null)"
check_row_value "test_table" "real_field 15" "(null)"
check_row_value "test_table" "rr_field 16" "(null)"

execute_wcmd_command "rows test_table update text_field='',a_bool_field=$A_BOOL_VALUE,a_ht_field=$A_HT_VALUE * bool_field=$BOOL_VALUE ht_field='$HT_VALUE' "
check_rows_info "test_table" 18 0 18
check_row_value "test_table" "text_field 17" "(null)"
check_row_value "test_table" "bool_field 17" "$BOOL_VALUE"
check_row_value "test_table" "ht_field 17" "$HT_VALUE"
check_row_value "test_table" "a_ht_field 17" "$A_HT_VALUE"
check_row_value "test_table" "a_bool_field 17" "$A_BOOL_VALUE"

execute_wcmd_command "rows test_table update a_char_field=$A_CHAR_VALUE,\
a_date_field=$A_DATE_VALUE,\
a_dt_field=$A_DT_VALUE,\
a_ht_field=$A_HT_VALUE,\
a_int8_field=$A_INT8_VALUE,\
a_int16_field=$A_INT16_VALUE,\
a_int32_field=$A_INT32_VALUE,\
a_int64_field=$A_INT64_VALUE,\
a_uint8_field=$A_UINT8_VALUE,\
a_uint16_field=$A_UINT16_VALUE,\
a_uint32_field=$A_UINT32_VALUE,\
a_uint64_field=$A_UINT64_VALUE,\
a_real_field=$A_REAL_VALUE,\
a_rr_field=$A_RREAL_VALUE\
 0-15 bool_field=min@max char_field=null"


check_row_value "test_table" "bool_field 1" "$BOOL_VALUE"
check_row_value "test_table" "char_field 1" "(null)"
check_row_value "test_table" "a_char_field 1" "$A_CHAR_VALUE"
check_row_value "test_table" "a_date_field 1" "$A_DATE_VALUE"
check_row_value "test_table" "a_dt_field 1" "$A_DT_VALUE"
check_row_value "test_table" "a_int8_field 1" "$A_INT8_VALUE"
check_row_value "test_table" "a_int16_field 1" "$A_INT16_VALUE"
check_row_value "test_table" "a_int32_field 1" "$A_INT32_VALUE"
check_row_value "test_table" "a_int64_field 1" "$A_INT64_VALUE"
check_row_value "test_table" "a_uint8_field 1" "$A_UINT8_VALUE"
check_row_value "test_table" "a_uint16_field 1" "$A_UINT16_VALUE"
check_row_value "test_table" "a_uint32_field 1" "$A_UINT32_VALUE"
check_row_value "test_table" "a_uint64_field 1" "$A_UINT64_VALUE"
check_row_value "test_table" "a_real_field 1" "$A_REAL_VALUE"
check_row_value "test_table" "a_rr_field 1" "$A_RREAL_VALUE"

execute_wcmd_command "rows test_table update \
text_field=null,\
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
rr_field=null,\
a_char_field=null,\
a_date_field=null,\
a_dt_field=null,\
a_ht_field=null,\
a_int8_field=null,\
a_int16_field=null,\
a_int32_field=null,\
a_int64_field=null,\
a_uint8_field=null,\
a_uint16_field=null,\
a_uint32_field=null,\
a_uint64_field=null,\
a_real_field=null,\
a_rr_field=null\
 0-15 bool_field=min@max char_field=null"


check_row_value "test_table" "text_field 1" "(null)"
check_row_value "test_table" "bool_field 1" "(null)"
check_row_value "test_table" "char_field 1" "(null)"

check_row_value "test_table" "date_field 1" "(null)"
check_row_value "test_table" "dt_field 1" "(null)"
check_row_value "test_table" "ht_field 1" "(null)"
check_row_value "test_table" "int8_field 1" "(null)"
check_row_value "test_table" "int16_field 1" "(null)"
check_row_value "test_table" "int32_field 1" "(null)"
check_row_value "test_table" "int64_field 1" "(null)"
check_row_value "test_table" "uint8_field 1" "(null)"
check_row_value "test_table" "uint16_field 1" "(null)"
check_row_value "test_table" "uint32_field 1" "(null)"
check_row_value "test_table" "uint64_field 1" "(null)"
check_row_value "test_table" "real_field 1" "(null)"
check_row_value "test_table" "rr_field 1" "(null)"

check_row_value "test_table" "a_char_field 1" "(null)"
check_row_value "test_table" "a_date_field 1" "(null)"
check_row_value "test_table" "a_dt_field 1" "(null)"
check_row_value "test_table" "a_ht_field 1" "(null)"
check_row_value "test_table" "a_int8_field 1" "(null)"
check_row_value "test_table" "a_int16_field 1" "(null)"
check_row_value "test_table" "a_int32_field 1" "(null)"
check_row_value "test_table" "a_int64_field 1" "(null)"
check_row_value "test_table" "a_uint8_field 1" "(null)"
check_row_value "test_table" "a_uint16_field 1" "(null)"
check_row_value "test_table" "a_uint32_field 1" "(null)"
check_row_value "test_table" "a_uint64_field 1" "(null)"
check_row_value "test_table" "a_real_field 1" "(null)"
check_row_value "test_table" "a_rr_field 1" "(null)"


check_rows_info "test_table" 18 1 1

execute_wcmd_command "rows test_table update \
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
rr_field=min\
 1 "

check_rows_info "test_table" 18 0 18

execute_wcmd_command "rows test_table update \
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
rr_field=max\
 13"

check_rows_info "test_table" 18 0 18

execute_wcmd_command "rows test_table remove * \
bool_field=max \
char_field=max \
date_field=max \
dt_field=max \
ht_field=max \
int8_field=max \
int16_field=max \
int32_field=max \
int64_field=max \
uint8_field=max \
uint16_field=max \
uint32_field=max \
uint64_field=max \
real_field=max \
rr_field=max "

check_rows_info "test_table" 18 1 13




rm -rf "$DB_DIR"
echo "TEST RESULT:  PASS"

