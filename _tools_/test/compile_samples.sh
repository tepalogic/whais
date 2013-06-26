#!/bin/bash

#This script takes a list of Whisper code files and generates the 
#corresponding objects and their dumps. It's used mainly during
#to test the compiler (and the disassembler) generate the same ouput
#independent of the used architecture.

declare -a SRC_FILES

#First set the work env
eval source env_setup.sh $*

#Used to retrieve the list of files to be processed
function parse_for_filenames ()
{
	while  (( $# )); do
		case "$1" in
			-f | --files ) 
				file_togle='y'
				shift
				;;
			-*)
				unset file_togle
				shift
				;;
			*)
				if [ -n "${file_togle}" ]; then
					SRC_FILES=("${SRC_FILES[@]}" "$1")
				fi
				shift
				;;
		esac
	done

	[ "${#SRC_FILES[@]}" -gt  "0" ] || die "No source file provided (e.g. -f file1 file2 file3)".
}

eval parse_for_filenames $*

#Compile each specified file
for src in "${SRC_FILES[@]}"
do
	out_obj=${WORKDIR}/$(basename ${src})
	out_obj=${out_obj%.w}
	out_obj=${out_obj}.wo
	
	echo "Processing ${src}."
	whc "${src}" -o "${out_obj}"
	err=$?
	if [ "${err}" -ne 0 ]
	then
		echo "Failed to compile '${src} (${err})'."
		exit ${err}
	fi

	wod "${out_obj}" > "${out_obj%.wo}.wdump"
	err=$?
	if [ "${err}" -ne 0 ]
	then
		echo "Failed to dump object '${obj_dump} (${err})'."
		exit ${err}
	fi
done

