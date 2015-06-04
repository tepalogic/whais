#!/bin/bash

WHAIS_PID=
DB_NAME=test_auto_restore
DB_PASWD=test_password
START_TMO=3
SYNC_TMO=10

INT_VAL_0="0"
INT_VAL_1="3"
INT_VAL_2="100"
INT_VAL_3="42"
INT_VAL_4="1"
INT_VAL_5="101"

TEXT_VAL_0="Am I the king of the World?"
TEXT_VAL_1="I do not know ... May be you are or may you are not."
TEXT_VAL_2="I know! I know! ... Like in << To be or not to be>>?"
TEXT_VAL_3="E = mc^2"
TEXT_VAL_4="Not enough for Nobel thouhg!"
TEXT_VAL_5="<<To be or not to be?>> ... That should the question!"

EXEC_DIR=$(dirname $0)/../

get_whais_pid () {
	WHAIS_PID=$(pidof whais | cut -f 1 -d ' ')
}

fail_test () {
	killall whais &> /dev/null
	echo "$1"
	echo "TEST RESULT:  FAIL"
	exit 1
}

start_whais () {
	get_whais_pid
	[ -z "$WHAIS_PID" ] || fail_test "A Whais program is still running (PID: $WHAIS_PID)"
	echo "Starting Whais in $START_TMO seconds  ... "
	( cd $EXEC_DIR ;  whais confs/auto_restore.cfg ) &
	sleep $START_TMO
	get_whais_pid
	[ -n "$WHAIS_PID" ] || fail_test "Whais did not start!!!"
	wcmd -H localhost -P 1761 -u $DB_NAME -p $DB_PASWD  <<< " exec get_table_value U32'' "
	[ $? -eq 0 ] || "Whais did start ... but the healty check did not go well."
	sleep 1
	get_whais_pid
	[ -n "$WHAIS_PID" ] || fail_test "Whais is not here anymore."
	echo "Whais PID is $WHAIS_PID"
}


start_whais_fail () {
	get_whais_pid
	[ -z "$WHAIS_PID" ] || fail_test "A Whais program is still running (PID: $WHAIS_PID)"
	echo "Starting Whais in $START_TMO seconds  ... "
	( cd $EXEC_DIR ;  whais confs/auto_restore.cfg ) &
	sleep $START_TMO
	get_whais_pid
	[ -z "$WHAIS_PID" ] || fail_test "Whais was started though it should not be!"
	echo "Whais server did not start as it was expected....."
}

update_table () {
	update_cmd=""
	for row in {0..5}
	do
		update_cmd+=$(eval echo exec "set_table_value U32\'$row\' U8\'\$INT_VAL_$row\' T\'\${TEXT_VAL_${row}}\' ';'")
	done

	update_cmd+=" exec get_table_value U32'' ; quit "

	echo "$update_cmd"
	wcmd -H localhost -P 1761 -u $DB_NAME -p $DB_PASWD  <<< "$update_cmd"
	echo "Data base updated!"
}


echo "Testing database consistency recovery..."
killall whais &> /dev/null
sleep 2
( cd $EXEC_DIR ; ./prepare_for_test.sh )
start_whais
get_whais_pid
update_table
kill -9 $WHAIS_PID &> /dev/null
[ $? -eq 0 ] || fail_test "Failed to kill Whais (PID $WHAIS_PID)."
echo "Killed Whais proc (PID $WHAIS_PID) "
sleep 2
start_whais_fail
echo Fixing $DB_NAME an try again
( cd $EXEC_DIR ;   wcmd -u $DB_NAME -d ./test_auto_restore/ -t -f)
start_whais
kill -9 $WHAIS_PID &> /dev/null
( cd $EXEC_DIR ; ./prepare_for_test.sh --clean )
echo "TEST RESULT: PASS"

