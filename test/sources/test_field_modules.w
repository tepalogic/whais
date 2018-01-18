################################################################################
# Test module used to verify the correct execution of field related functions
# provided by the native base library.
# 
# Iulian Popa
#
################################################################################


#@include whais_std.wh

PROCEDURE test_field_sort (t TABLE, f UINT64, reverse BOOL)
RETURN TABLE
DO
	sort_table (t, {f} UINT16, {reverse} );

	RETURN t;
ENDPROC


PROCEDURE test_field_mimim (t TABLE, f UINT64)
RETURN UNDEFINED
DO
	return get_smallest (get_fieldth (t, f));
ENDPROC


PROCEDURE test_field_maxim (t TABLE, f UINT64)
RETURN UNDEFINED
DO
	return get_biggest (get_fieldth (t, f));
ENDPROC

PROCEDURE test_field_name (t TABLE, f UINT64)
RETURN TEXT
DO
	return get_name (get_fieldth (t, f));
ENDPROC


PROCEDURE test_field_match (t		TABLE, 
	     		    f		UINT64,
			    min		UNDEFINED,
			    max		UNDEFINED,
			    fromRow UINT64,
			    toRow   UINT64)
RETURN UINT32 ARRAY
DO
	return match_rows (get_fieldth (t, f), 
	                   min,
                       	   max,
	                   fromRow,
         	           toRow);
ENDPROC

