
#Following are windows visual c++ specific build settings

ARCH_OBJ_EXT:=.obj
ARCH_EXE_EXT:=.exe
ARCH_SHL_PREFIX:=
ARCH_SHL_EXT:=.dll
ARCH_LIB_PREFIX:=sl
ARCH_LIB_EXT:=.lib
CC:='/cygdrive/c/Program Files/Microsoft Visual Studio 12.0/VC/bin/cl.exe'
CXX:='/cygdrive/c/Program Files/Microsoft Visual Studio 12.0/VC/bin/cl.exe'
LD:='/cygdrive/c/Program Files/Microsoft Visual Studio 12.0/VC/bin/link.exe'
AR:='/cygdrive/c/Program Files/Microsoft Visual Studio 12.0/VC/bin/lib.exe'

#Default output directories
WHAIS_OUT_DIR?=/cygdrive/c/WHAIS
EXES_OUT_DIR?=$(WHAIS_OUT_DIR)/bin/
SHLS_OUT_DIR?=$(WHAIS_OUT_DIR)/lib/
LIBS_OUT_DIR?=$(WHAIS_OUT_DIR)/lib/
HDRS_OUT_DIR?=$(WHAIS_OUT_DIR)/include/

CC_FLAGS:=/LD /W3 /TC /c  /Y- /arch:SSE2 /nologo /wd4242 /wd4244 /wd4290 /wd4355 /wd4800 

ifeq ($(ASSERTS),no)
DEFINES+=NDEBUG=1
endif

ifeq ($(DEBUGINFO),yes)
CC_FLAGS:=$(subst /LD,/LDd /Z7 /RTC1,$(CC_FLAGS))
endif

ifeq ($(OPTIMISE),speed)
CC_FLAGS+= /O2
CC_FLAGS:=$(subst /RTC1,,$(CC_FLAGS))
else
ifeq ($(OPTIMISE),size)
CC_FLAGS+= /O1
CC_FLAGS:=$(subst /RTC1,,$(CC_FLAGS))
endif
endif

CXX_FLAGS:=$(subst /TC,/TP,$(CC_FLAGS)) /EHsc $(EXT_CXX_FLAGS)
CC_FLAGS+=$(EXT_CC_FLAGS)

DEFINES+=ARCH_WINDOWS_VC
DEFINES+=INLINE=__inline
DEFINES+=_CRT_SECURE_NO_WARNINGS _USING_V110_SDK71_
DEFINES+=snprintf=_snprintf
DEFINES+=QWORDS_PER_OP=4
DEFINES+=WOS=Windows
DEFINES+=WARCH=x86_32

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

arch_linker_flags=/NOLOGO /OPT:REF shlwapi.lib advapi32.lib
ifeq ($(DEBUGINFO),yes)
arch_linker_flags+=/DEBUG
endif 
arch_shl_linker_flags= $(arch_linker_flags) /DLL 
arch_archiver_flags=/NOLOGO

