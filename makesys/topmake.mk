.PHONY: all prepare_env generate_files executables clean install

all: generate_files

prepare_env:
	$(ECHO)if [ ! -d ./bin ]; then mkdir ./bin ; fi;
	$(ECHO)if [ ! -d ./bin/$(ARCH) ]; then mkdir ./bin/$(ARCH) ; fi;
	$(ECHO)if [ ! -d ./tmp ]; then mkdir ./tmp ; fi;
	$(ECHO)if [ ! -d ./tmp/$(ARCH) ]; then mkdir ./tmp/$(ARCH) ; fi;
	$(ECHO)for unit in $(ALL_UNITS) ; do \
			if [ ! -d ./bin/$(ARCH)/$$unit ]; then mkdir ./bin/$(ARCH)/$$unit ; fi; \
			if [ ! -d ./tmp/$(ARCH)/$$unit ]; then mkdir ./tmp/$(ARCH)/$$unit ; fi; \
        	if [ ! -d ./tmp/$(ARCH)/$$unit/exes ]; then mkdir ./tmp/$(ARCH)/$$unit/exes ; fi;\
			if [ ! -d ./tmp/$(ARCH)/$$unit/shls ]; then mkdir ./tmp/$(ARCH)/$$unit/shls ; fi;\
			if [ ! -d ./tmp/$(ARCH)/$$unit/libs ]; then mkdir ./tmp/$(ARCH)/$$unit/libs ; fi;\
		done ;
		
generate_files: prepare_env ./compiler/parser/whais.tab.c
	$(ECHO)$(MAKE) --no-print-directory -otarget executables 
		
executables:  $(EXES) $(SHLS) $(LIBS)

clean:
	rm -rf ./tmp
	rm -rf ./bin
	rm -rf compiler/parser/whais.tab.c compiler/parser/whais.tab.h compiler/parser/whais.output
	rm -rf vc90.idb vc90.pdb vc100.idb vc100.pdb
	rm -rf _CL_*

install:
	if [ ! -d $(EXES_OUT_DIR) ]; then mkdir -p $(EXES_OUT_DIR) ; fi
	if [ ! -d $(SHLS_OUT_DIR) ]; then mkdir -p $(SHLS_OUT_DIR) ; fi
	if [ ! -d $(LIBS_OUT_DIR) ]; then mkdir -p $(LIBS_OUT_DIR) ; fi
	if [ ! -d $(HDRS_OUT_DIR) ]; then mkdir -p $(HDRS_OUT_DIR) ; fi
	cp $(EXES) $(EXES_OUT_DIR) 
	cp $(SHLS) $(SHLS_OUT_DIR)
	cp $(LIBS) $(LIBS_OUT_DIR)
	cp -RfL --copy-contents headers/* $(HDRS_OUT_DIR)
	if [ ! -d $(HDRS_OUT_DIR)/whais_std_hdrs ]; then mkdir -p $(HDRS_OUT_DIR)/whais_std_hdrs ; fi
	cp -RfL --copy-contents stdlib/whais_inc/* $(HDRS_OUT_DIR)/whais_std_hdrs/

%.c : generate_files
%.cpp : generate_files
