################################################################################
# Test module used to verify the correct execution of field related functions
# provided by the native base library.
# 
# Iulian Popa
#
################################################################################

EXTERN PROCEDURE table_field_by_id (t AS TABLE, f AS UNSIGNED INT64) RETURN FIELD;

EXTERN PROCEDURE field_name (t as FIELD) RETURN TEXT;

EXTERN PROCEDURE field_minimum (t as FIELD) RETURN UNSIGNED INT64;
EXTERN PROCEDURE field_maximum (t as FIELD) RETURN UNSIGNED INT64;
EXTERN PROCEDURE field_average (t as FIELD) RETURN RICHREAL;

EXTERN PROCEDURE field_match_rows (t    AS FIELD, 
							       min  AS UNDEFINED,
								   max  AS UNDEFINED,
								   from AS UNSIGNED INT64,
								   to   AS UNSIGNED INT64) RETURN ARRAY OF UNSIGNED INT64;

EXTERN PROCEDURE field_sort_table (t as FIELD, reverse AS BOOL) RETURN TABLE;

PROCEDURE test_field_sort (t AS TABLE, f AS UNSIGNED INT64, reverse AS BOOL)
RETURN TABLE
DO
	field_sort_table (table_field_by_id (t, f), reverse);

	RETURN t;
ENDPROC


PROCEDURE test_field_mimim (t AS TABLE, f AS UNSIGNED INT64)
RETURN UNDEFINED
DO
	return field_minimum (table_field_by_id (t, f));
ENDPROC


PROCEDURE test_field_maxim (t AS TABLE, f AS UNSIGNED INT64)
RETURN UNDEFINED
DO
	return field_maximum (table_field_by_id (t, f));
ENDPROC


PROCEDURE test_field_average (t AS TABLE, f AS UNSIGNED INT64)
RETURN RICHREAL
DO
	return field_average (table_field_by_id (t, f));
ENDPROC


PROCEDURE test_field_name (t AS TABLE, f AS UNSIGNED INT64)
RETURN TEXT
DO
	return field_name (table_field_by_id (t, f));
ENDPROC


PROCEDURE test_field_match (t		AS TABLE, 
							f		AS UNSIGNED INT64,
						    min		AS UNDEFINED,
						    max		AS UNDEFINED,
						    fromRow AS UNSIGNED INT64,
						    toRow   AS UNSIGNED INT64)
RETURN ARRAY OF UNSIGNED INT64
DO
	return field_match_rows (table_field_by_id (t, f),
							 min,
							 max,
							 fromRow,
							 toRow);
ENDPROC

