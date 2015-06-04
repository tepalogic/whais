EXTERN LET test_restore AS TABLE OF (field_text AS TEXT, field_int AS INT8);

PROCEDURE set_table_value (r AS INT32, i AS INT8, t AS TEXT) RETURN UNDEFINED
DO 
	test_restore.field_text[r] = t;
	test_restore.field_int[r] = i;

	RETURN TRUE;
ENDPROC

PROCEDURE get_table_value (r AS UINT32) RETURN UNDEFINED
DO 
	LET selected_row AS TABLE OF (row AS UINT32, field_int AS INT8, field_text AS TEXT);

	IF (r == NULL)
		IF (test_restore != NULL)
			RETURN test_restore;
		ELSE
			RETURN "Table 'test_restore' is null. ";

	selected_row.row[0] = r;
	selected_row.field_text[0] = test_restore.field_text[r];
	selected_row.field_int[0] = test_restore.field_int[r];

	RETURN selected_row;
ENDPROC

