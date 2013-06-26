#!/bin/bash

#This script provides functionality to setup the environment
#required to execute different test units for the arch of choice.
#

E_DEFAULT=1


BASEDIR=
ARCH=
WORKDIR=
SESSION_ID=''

function die ()
{
	[ -z "$1" ] && echo "Die called." || echo "$1"
	[ -z "$2" ] &&  exit $E_DEFAULT || exit "$2"
}

function env_process_args ()
{
	local reset=''
	while  (( $# )); do
		case "$1" in
			-a | --arch ) 
				[ $# -ge 2 ] || die "Missing parameter for argument '$1'."
				ARCH="$2" 
				shift 2
				;;
			-b | --basedir ) 
				[ $# -ge 2 ] || die "Missing parameter for argument '$1'."
				BASEDIR="$2" 
				shift 2
				;;
			-w | --workdir )  
				[ $# -ge 2 ] || die "Missing parameter for argument '$1'."
				WORKDIR="$2"
				shift 2
				;;
			-s | --session )
				[ $# -ge 2 ] || die "Missing parameter for argument '$1'."
				SESSION_ID="$2"
				shift 2
				;;
			-r | --reset )
				reset="y"
				shift
				;;
			* )
				#Don't know how to handle this argument. Ignore it!
				shift
		esac
	done

	[ -n "${ARCH}" ] || die "An architecture needs to be provided (e.g. -a linux_gcc_x86)."
	
	#Trying to detect the base directory
	if [ -z "${BASEDIR}" ]; then
		BASEDIR=$(expr "$(pwd)" : '\(.*/whisper\)')
	fi

	[ -n "${BASEDIR}" ] || die "An based directory was not provided and could not be auto detected. (e.g. -b /dir/to/whiper/)."

	BASEDIR=${BASEDIR%/}
	[ -n "${WORKDIR}" ] || WORKDIR=${BASEDIR}

	WORKDIR=${WORKDIR%/}/test_${SESSION_ID}/${ARCH}
	[ "${reset}" == "y" ] && rm -rf "${WORKDIR}"
	[ -d "${WORKDIR}" ] || mkdir -p "${WORKDIR}" || die "Could not create the directory '${WORKDIR}.'"

	[ -d "${BASEDIR}/bin/${ARCH}" ] || die "The build directory '${BASEDIR}/bin/${ARCH}' doesn't exists."
}

eval env_process_args $* 

whisper_bindirs=(${BASEDIR}/bin/${ARCH}/*)
bin_dirs=
for dir in ${whisper_bindirs[@]}
do
	if [ -n "${bin_dirs}" ]; then
		bin_dirs="${dir}:${bin_dirs}"
	else
		bin_dirs="${dir}"
	fi
done

export LD_LIBRARY_PATH=${bin_dirs}:${LD_LIBRARY_PATH/"${OLD_WBINDIRS}:"/}
export PATH=${bin_dirs}:${PATH/"${OLD_WBINDIRS}:"/}
export OLD_WBINDIRS=${bin_dirs}

echo
echo '************************************************************************'
echo 
echo "ARCH='${ARCH}'"
echo "BASEDIR='${BASEDIR}'"
echo "WORKDIR='${WORKDIR}'"
echo "SESSION_ID='${SESSION_ID}'"
echo "LD_LIBRARY_PATH='${LD_LIBRARY_PATH}'"
echo "PATH='${PATH}'"
echo
echo '************************************************************************'
echo 

