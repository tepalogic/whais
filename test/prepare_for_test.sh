#!/bin/bash

rm -rf ./admin_db ./echo_proc_db ./obj ./test_exec_db ./test_list_db ./test_list_db_frame_size ./test_auto_restore ./logs

if [ "$1" == "--clean" ]; then
	echo "Clean done!" ;
	exit 0;
fi


mkdir admin_db logs

if [ $? -ne 0 ]; then
	echo "Failed to create 'admin_db' directory"
	exit 1
fi

echo 'Create administrator database...'
echo 'add users password TEXT username TEXT; quit' |  wcmd -c administrator -d ./admin_db > /dev/null
if [ $? -ne 0 ]; then
	echo "Failed to create 'administrator' database"
	exit 1
fi

create_empty_test_db () {
	mkdir $1
	if [ $? -ne 0 ]; then
		echo "Failed to create '$1' directory."
		exit 1
	fi
	echo 'quit' |  wcmd -c $1 -d ./$1  > /dev/null
	if [ $? -ne 0 ]; then
		echo "Failed to create '$1' database."
		exit 1
	fi
}

create_empty_test_db echo_proc_db
create_empty_test_db test_exec_db
create_empty_test_db test_list_db
create_empty_test_db test_list_db_frame_size
create_empty_test_db test_auto_restore

echo 'Create test_auto_restore database...'
echo 'add test_restore field_text TEXT field_int INT8; quit' |  wcmd -u test_auto_restore -d ./test_auto_restore > /dev/null
if [ $? -ne 0 ]; then
	echo "Failed to add tables into 'test_auto_restore' database"
	exit 1
fi


mkdir ./obj
if [ $? -ne 0 ]; then
	echo "Failed to create 'obj' directory"
	exit 1
fi

for src in ./sources/*.w
do
	echo "Compiling '${src}' ... "
	whc ${src} -I ../stdlib/whais_inc/
	if [ $? -ne 0 ]; then
		echo "Failed to compile '${src}' source file."
		exit 1
	fi
done
mv ./sources/*.wo ./obj

echo -e "\nPreparation complete!"


