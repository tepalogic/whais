
.PHONY: all prepare_env generate_files exes clean

all: generate_files

prepare_env:
	@echo -n "Preparing environment..."
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
	@echo "Done"
		
generate_files: prepare_env
	@echo -n "Generating parser source code..."
	$(ECHO)bison -d compiler/parser/whisper.y -o compiler/parser/whisper.tab.c
	@echo "Done generating"
	@echo "Start the actual build ... "
	$(MAKE) -otarget exes 
		
exes:  $(EXES) $(SHLS) $(LIBS)

clean:
	rm -rf ./tmp
	rm -rf ./bin
	rm -rf compiler/parser/whisper.tab.c compiler/parser/whisper.tab.h compiler/parser/whisper.output
	rm -rf vc90.idb vc90.pdb vc100.idb vc100.pdb
	rm -rf _CL_*

%.c : generate_files
%.cpp : generate_files
