EXTERN VAR test_restore TABLE (field_text TEXT, field_int INT8);

PROCEDURE set_table_value (r INT32, i INT8, t TEXT) RETURN UNDEFINED
DO 
	test_restore.field_text[r] = t;
	test_restore.field_int[r] = i;

	RETURN TRUE;
ENDPROC

PROCEDURE get_table_value (r UINT32) RETURN UNDEFINED
DO 
	VAR selected_row TABLE (row UINT32, field_int INT8, field_text TEXT);

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

