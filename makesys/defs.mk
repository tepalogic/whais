#General templates

#Get the right compiler and flags according of the source file extension
#(1) - the source file needed to be compiled
compiler=$(if $(filter .c,$(suffix $(1))),$(CC),$(if $(filter .cpp,$(suffix $(1))),$(CXX),))
compiler_flags=$(if $(filter .c,$(suffix $(1))),$(CC_FLAGS),$(if $(filter .cpp,$(suffix $(1))),$(CXX_FLAGS),))

#dependencies will be always build with gcc or g++
dep_compiler=$(if $(filter .c,$(suffix $(1))),gcc,$(if $(filter .cpp,$(suffix $(1))),g++,))

#Get the object path corresponding to a source file
#(1) - unit where that file belongs
#(2) - code source
#(3) - output type
obj_name=./tmp/$(ARCH)/$(1)/$(3)/$(notdir $(basename $(2)))$(ARCH_OBJ_EXT)

#Get the dependency file corresponding to a source file
#(1) - unit where that file belongs
#(2) - code source
#(3) - output type
dep_name=./tmp/$(ARCH)/$(1)/$(3)/$(notdir $(basename $(2))).d

#set the right dependencies for libs
#(1) - the static lib's name
arch_dependecy_lib=$(foreach _lib,$($(1)_LIB),./bin/$(ARCH)/$(dir $(_lib))$(ARCH_LIB_PREFIX)$(notdir $(_lib))$(ARCH_LIB_EXT))

#set the right dependencies for shared libs 
#(1) - the shared lib's name
arch_dependecy_shlib=$(foreach _lib,$($(1)_SHL),./bin/$(ARCH)/$(dir $(_lib))$(ARCH_SHL_PREFIX)$(notdir $(_lib))$(ARCH_SHL_EXT))

#Create de dependency rules required to compile a source file
#$(1) - name of the library/executable to add in the build system
#$(2) - name of the UNIT the library/executable belongs too
#$(3) - source file to compile
#$(4) - type of the output
define create_source_compile_rule

-include $(call dep_name,$(2),$(3),$(4))

$(call obj_name,$(2),$(3),$(4)) : ./$(2)/$(3)
	@echo Compiling $(3) for $(ARCH)/$(1)
	$(ECHO)$(call compiler,$(3)) $(call compiler_flags,$(3)) \
	   	$(call arch_add_includes,$($(1)_INC) $(2)/include $(INCLUDES) ) $(call arch_add_defines,$(1),$(2)) \
		$(call arch_set_output_object,$(call obj_name,$(2),$(3),$(4))) $(call arch_translate_path,$(2)/$(3))

	@$(call dep_compiler,$(3)) -MM  $(foreach _dir, $(sort $($(1)_INC) $(2)/include $(INCLUDES)), -I$(_dir)) $(foreach _def, $(sort $($(1)_DEF) $(DEFINES)), -D$(_def)) \
		-o $(call dep_name,$(2),$(3),$(4)) $(2)/$(3)
	@sed -i 's/^.*:/$(subst /,\/,$(call obj_name,$(2),$(3),$(4))) : /' $(call dep_name,$(2),$(3),$(4))

endef


#Add an executable on the build system
#$(1) - name of the executable to add in the build system
#$(2) - name of the UNIT the executable belongs too
define add_output_executable
$(1)_SRC=$(sort $($(1)_SRC))
$(1)_LIB_DIR=$(sort $(foreach _lib, $($(1)_LIB) $($(1)_SHL),$(dir $(_lib))))
$(foreach src, $($(1)_SRC), $(eval $(1)_OBJ+=$(call obj_name,$(2),$(src),exes)))
$(foreach src, $($(1)_SRC), $(eval $(1)_DEP+=$(call dep_name,$(2),$(src),exes)))
$(foreach src, $($(1)_SRC), $(eval $(call create_source_compile_rule,$(1),$(2),$(src),exes)))


EXES+=./bin/$(ARCH)/$(2)/$(1)$(ARCH_EXE_EXT)
./bin/$(ARCH)/$(2)/$(1)$(ARCH_EXE_EXT) : $($(1)_OBJ) $(call arch_dependecy_shlib,$(1)) $(call arch_dependecy_lib,$(1)) 
	@echo Building executable $(ARCH)/$(1)  
	$(ECHO)$(LD) $($(1)_OBJ) $$(call arch_add_lib_dirs,$(1))\
		$$(call arch_handle_import_libs,$(1)) $$(call arch_linker_flags,$(1))\
		$$(call arch_handle_import_libs,$(1)) $$(call arch_set_output_executable,$(2)/$(1))
endef

#Add an shared library to the build system
#$(1) - name of the library to add in the build system
#$(2) - name of the UNIT the executable belongs too
#$(3) - major version description
#$(4) - minor version description
define add_output_shared_lib
$(1)_SRC=$(sort $($(1)_SRC))
$(1)_LIB_DIR=$(sort $(foreach _lib, $($(1)_LIB) $($(1)_SHL),$(dir $(_lib))))
$(foreach src, $($(1)_SRC), $(eval $(1)_OBJ+=$(call obj_name,$(2),$(src),shls)))
$(foreach src, $($(1)_SRC), $(eval $(1)_DEP+=$(call dep_name,$(2),$(src),shls)))
$(foreach src, $($(1)_SRC), $(eval $(call create_source_compile_rule,$(1),$(2),$(src),shls)))

SHLS+=./bin/$(ARCH)/$(2)/$(ARCH_SHL_PREFIX)$(1)$(ARCH_SHL_EXT)
SHLS+=$(if $(3),./bin/$(ARCH)/$(2)/$(ARCH_SHL_PREFIX)$(1)$(ARCH_SHL_EXT)$(3)$(4),)
./bin/$(ARCH)/$(2)/$(ARCH_SHL_PREFIX)$(1)$(ARCH_SHL_EXT) : $($(1)_OBJ) $(call arch_dependecy_shlib,$(1)) $(call arch_dependecy_lib,$(1)) 
	@echo Building shared lib $(ARCH)/$(1)  
	$(ECHO)$(LD) $($(1)_OBJ) $$(call arch_shl_linker_flags,$(1),$(3),$(4)) $$(call arch_add_lib_dirs,$(1))\
		$$(call arch_handle_import_libs,$(1)) $$(call arch_set_output_sharedlib,$(1),$(2))

#Odd but the links are kept the other way arround to facilitate multiple jobs
#make runs. 
./bin/$(ARCH)/$(2)/$(ARCH_SHL_PREFIX)$(1)$(ARCH_SHL_EXT)$(3)$(4) : ./bin/$(ARCH)/$(2)/$(ARCH_SHL_PREFIX)$(1)$(ARCH_SHL_EXT)
	$(ECHO)ln -s -f $(ARCH_SHL_PREFIX)$(1)$(ARCH_SHL_EXT) ./bin/$(ARCH)/$(2)/$(ARCH_SHL_PREFIX)$(1)$(ARCH_SHL_EXT)$(3)$(4)
	$(ECHO)ln -s -f $(ARCH_SHL_PREFIX)$(1)$(ARCH_SHL_EXT) ./bin/$(ARCH)/$(2)/$(ARCH_SHL_PREFIX)$(1)$(ARCH_SHL_EXT)$(3)
	
endef



#Add an library on the build system
#$(1) - name of the library to add in the build system
#$(2) - name of the UNIT the executable belongs too
define add_output_library
$(1)_SRC=$(sort $($(1)_SRC))
$(foreach src, $($(1)_SRC), $(eval $(1)_OBJ+=$(call obj_name,$(2),$(src),libs)))
$(foreach src, $($(1)_SRC), $(eval $(1)_DEP+=$(call dep_name,$(2),$(src),libs)))
$(foreach src, $($(1)_SRC), $(eval $(call create_source_compile_rule,$(1),$(2),$(src),libs)))


LIBS+=./bin/$(ARCH)/$(2)/$(ARCH_LIB_PREFIX)$(1)$(ARCH_LIB_EXT)
./bin/$(ARCH)/$(2)/$(ARCH_LIB_PREFIX)$(1)$(ARCH_LIB_EXT) : $($(1)_OBJ)
	@echo Building library $(ARCH)/$(1)  
	$(ECHO)$(AR) $(call arch_archiver_flags,$(1)) $$(call arch_set_output_library,$(1),$(2)) $$^
endef

