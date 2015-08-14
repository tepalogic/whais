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
	field_sort_table (table_field_by_id (t, f), reverse);

	RETURN t;
ENDPROC


PROCEDURE test_field_mimim (t TABLE, f UINT64)
RETURN UNDEFINED
DO
	return field_smallest (table_field_by_id (t, f));
ENDPROC


PROCEDURE test_field_maxim (t TABLE, f UINT64)
RETURN UNDEFINED
DO
	return field_biggest (table_field_by_id (t, f));
ENDPROC


PROCEDURE test_field_average (t TABLE, f UINT64)
RETURN RICHREAL
DO
	return field_average (table_field_by_id (t, f));
ENDPROC


PROCEDURE test_field_name (t TABLE, f UINT64)
RETURN TEXT
DO
	return field_name (table_field_by_id (t, f));
ENDPROC


PROCEDURE test_field_match (t		TABLE, 
							f		UINT64,
						    min		UNDEFINED,
						    max		UNDEFINED,
						    fromRow UINT64,
						    toRow   UINT64)
RETURN UINT64 ARRAY
DO
	return match_rows (table_field_by_id (t, f), 
	                   min,
                       max,
                       fromRow,
                       toRow);
ENDPROC

