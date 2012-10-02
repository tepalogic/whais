#Unit name.
UNIT:=client

UNIT_EXES:=
UNIT_SHLS:=
UNIT_LIBS:=connector

connector_SRC=src/connector.c src/client_connection.c
connector_DEF=
connector_LIB=utils/utils custom/custom
connector_SHL=


$(foreach exe, $(UNIT_EXES), $(eval $(call add_output_executable,$(exe),$(UNIT))))
$(foreach shl, $(UNIT_SHLS), $(eval $(call add_output_shared_lib,$(shl),$(UNIT),$($(shl)_MAJ),$($(shl)_MIN))))
$(foreach lib, $(UNIT_LIBS), $(eval $(call add_output_library,$(lib),$(UNIT))))

