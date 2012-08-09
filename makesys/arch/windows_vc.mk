
#Following are windows visual c++ specific build settings

ARCH_OBJ_EXT:=.obj
ARCH_EXE_EXT:=.exe
ARCH_SHL_PREFIX:=
ARCH_SHL_EXT:=.dll
ARCH_LIB_PREFIX:=sl
ARCH_LIB_EXT:=.lib
CC:='/cygdrive/c/Program Files/Microsoft Visual Studio 9.0/VC/bin/cl.exe'
CXX:='/cygdrive/c/Program Files/Microsoft Visual Studio 9.0/VC/bin/cl.exe'
LD:='/cygdrive/c/Program Files/Microsoft Visual Studio 9.0/VC/bin/cl.exe'
AR:='/cygdrive/c/Program Files/Microsoft Visual Studio 9.0/VC/bin/lib.exe'

ifeq ($(FLAVOR),debug)
CC_FLAGS?=/LDd /W3 /TC /c  /Y- /arch:SSE2 /GF /ZI /RTC1 /nologo /wd4242 /wd4244
CXX_FLAGS?=$(subst /TC,/TP,$(CC_FLAGS)) /EHsc
endif

ifeq ($(FLAVOR),release)
DEFINES+=-DNDEBUG
CC_FLAGS?=/LD /W3 /TC /c  /Y- /arch:SSE2 /GF /O2 /nologo /wd4242 /wd4244
CXX_FLAGS?=$(subst /TC,/TP,$(CC_FLAGS)) /EHsc
endif

DEFINES+=ARCH_WINDOWS_VC=2
DEFINES+=INLINE=__inline
DEFINES+=_CRT_SECURE_NO_WARNINGS

#translate input files
arch_translate_path=$(subst /,\\,$(1))

#set the outputfile
arch_set_output_object=/Fo$(call arch_translate_path,$(1))

#set the right option for include headers
arch_add_includes=$(foreach _dir, $(sort $(1)),/I$(call arch_translate_path,$(_dir)))

#set the right option for defines options
arch_add_defines=$(foreach _def, $(sort $($(1)_DEF) $(DEFINES)),/D$(_def))

#set the right libraries directories
arch_add_lib_dirs=$(foreach _dir,$($(1)_LIB_DIR),/LIBPATH:$(call arch_translate_path,./bin/$(ARCH)/$(_dir)))

#set the right libraries adds
arch_handle_import_libs=$(foreach _lib,$($(1)_LIB),$(ARCH_LIB_PREFIX)$(notdir $(_lib)).lib) $(foreach _lib,$($(1)_SHL),$(notdir $(_lib)).lib)

#set the right argument to output executables
arch_set_output_executable=/OUT:$(call arch_translate_path,./bin/$(ARCH)/$(1)$(ARCH_EXE_EXT))

#set the right argument to output executables
arch_set_output_sharedlib=/OUT:$(call arch_translate_path,./bin/$(ARCH)/$(2)/$(1)$(ARCH_SHL_EXT))

#set the right argument to output executables
arch_set_output_library=/OUT:$(call arch_translate_path,./bin/$(ARCH)/$(2)/$(ARCH_LIB_PREFIX)$(1)$(ARCH_LIB_EXT))

#set the right  flags for the linker

arch_linker_flags=/link /nologo 
ifeq ($(FLAVOR),debug)
arch_linker_flags+=/DEBUG
endif
arch_shl_linker_flags= $(arch_linker_flags) /DLL 
arch_archiver_flags=/NOLOGO

