#!/bin/bash

rm -rf ./admin_db ./echo_proc_db ./obj ./test_exec_db ./test_list_db ./test_list_db_frame_size

if [ "$1" == "--clean" ]; then
	echo "Clean done!" ;
	exit 0;
fi


mkdir admin_db
if [ $? -ne 0 ]; then
	echo "Failed to create 'admin_db' directory"
	exit 1
fi
echo 'table_add users password TEXT username TEXT; quit' |  wcmd -c administrator -d ./admin_db -v 5
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
	echo 'quit' |  wcmd -c $1 -d ./$1 -v 5
	if [ $? -ne 0 ]; then
		echo "Failed to create '$1' database."
		exit 1
	fi
}

create_empty_test_db echo_proc_db
create_empty_test_db test_exec_db
create_empty_test_db test_list_db
create_empty_test_db test_list_db_frame_size


mkdir ./obj
if [ $? -ne 0 ]; then
	echo "Failed to create 'obj' directory"
	exit 1
fi

for src in ./sources/*.w
do
	whc ${src}
	if [ $? -ne 0 ]; then
		echo "Failed to compile '${src}' directory"
		exit 1
	fi
done
mv ./sources/*.wo ./obj

echo -e "\nPreparation complete!"


