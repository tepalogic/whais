#
# Test module used to verify the correct handling of global values.            #
#                                                                              #
################################################################################

#@include whais_std.wh
#@include ./test_exec_db/globals.wh

VAR global_test_field_table TABLE (f1 RICHREAL, f2 RICHREAL, f3 UINT8, f4 UINT16);

VAR g_field_rr, g_field_rr_n  RICHREAL FIELD;
VAR g_field_u, g_field_u_n FIELD;

PROCEDURE dummy_call_for_tables (t TABLE (f1 RICHREAL)) RETURN TABLE (f1 RICHREAL)
DO
	VAR result RETURN;
	result = t;

	result.f1[0] = -1;
	RETURN result;
ENDPROC

PROCEDURE test_whais_api_get_table(tc UINT8) RETURN BOOL
DO
	VAR table_d TABLE (f1 RICHREAL, f3 UINT8);
	VAR l_field_rr RICHREAL FIELD;
	VAR l_field_u FIELD;
	
	IF (tc == 0) DO
		IF (get_table(NULL) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		ELSE IF (get_table(l_field_rr) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		ELSE IF (get_table(l_field_u) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		ELSE IF (get_table(g_field_u_n) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		ELSE IF (get_table(g_field_rr_n) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		END
		RETURN TRUE;

	ELSE IF (tc == 1) DO
		global_test_field_table.f1[0] = 0;
		global_test_field_table.f2[0] = 0;
		global_test_field_table.f3[0] = 0;
		global_test_field_table.f4[0] = 0;

		g_field_rr = global_test_field_table.f1;
		l_field_rr = table_d.f1;

		IF (get_table(g_field_rr) == NULL) DO
			write_log(_FUNCL_ + ": result should not be NULL.");
			RETURN FALSE;
		ELSE IF (count_rows(get_table(g_field_rr)) != 1) DO
			write_log(_FUNCL_ + ": result shouldn't be different than 1: " + count_rows(get_table(g_field_rr)));
			RETURN FALSE;
		END
		
		IF (get_table(l_field_rr) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		ELSE IF (count_rows(get_table(l_field_rr)) != 0) DO
			write_log(_FUNCL_ + ": The table should be NULL but a storage has to exists and 0 rows are allocated");
			RETURN FALSE;
		END
		RETURN TRUE;

	ELSE IF (tc == 2) DO
		global_test_field_table.f1[0] = 0;
		global_test_field_table.f2[0] = 0;
		global_test_field_table.f3[0] = 0;
		global_test_field_table.f4[0] = 0;

		g_field_rr = table_d.f1;
		l_field_rr = global_test_field_table.f2;

		IF (l_field_rr == NULL) DO
			write_log(_FUNCL_ + ": The table is NULL but there is storage for it, so the field reference is valid (non-NULL).");
			RETURN FALSE;
		ELSE IF (g_field_rr == NULL) DO
			write_log(_FUNCL_ + ": The table is NULL but there is storage for it, so the field reference is valid (non-NULL).");
			RETURN FALSE;
		ELSE IF (table_d != NULL) DO
			write_log(_FUNCL_ + ": The table should be NULL (even though is storage for it).");
			RETURN FALSE;
		END

		g_field_rr[0] = 1.1;
		l_field_rr[0] = 2.2;

		IF (global_test_field_table.f1[0] != 0.0) DO
			write_log(_FUNCL_ + ": field 'f1' control value is no what it should be!");
			RETURN FALSE;

		ELSE IF (global_test_field_table.f2[0] != 2.0 + 0.2) DO
			write_log(_FUNCL_ + ": field 'f2' control value is no what it should be: " + global_test_field_table.f2[0]);
			RETURN FALSE;

		ELSE IF (table_d.f1[0] != 1.0 + 0.1) DO
			write_log(_FUNCL_ + ": local table field 'f1' control value is no what it should be: " + table_d.f1[0]);
			RETURN FALSE;

		ELSE IF (table_d.f3[0] != NULL) DO
			write_log(_FUNCL_ + ": local table field 'table_d.f3[0]' should be NULL");
			RETURN FALSE;

		END
		RETURN TRUE;

	ELSE IF (tc == 3) DO
		l_field_rr = dummy_call_for_tables(global_test_field_table).f1;
		l_field_rr[0] += 0.01;

		IF (global_test_field_table.f1[0] != -0.99) DO
			write_log(_FUNCL_ + ": unexpected value of: '" + global_test_field_table.f1[0] + "'");
			RETURN FALSE;
		END

		l_field_rr = dummy_call_for_tables(table_d).f1;
		l_field_rr[0] += -0.01;

		IF (table_d.f1[0] != -1.01) DO
			write_log(_FUNCL_ + ": unexpected value of: '" + table_d.f1[0] + "'");
			RETURN FALSE;
		END
		RETURN TRUE;
	
	ELSE IF  (tc == 4) DO
		IF (l_field_u != NULL) DO
			write_log(_FUNCL_ + ": l_field_u should be NULL by default'");
			RETURN FALSE;
		END

		l_field_u = table_d.f3;
		IF (l_field_u == NULL) DO
			write_log(_FUNCL_ + ": The table is NULL but there is storage for it, so the field reference is valid (non-NULL).");
			RETURN FALSE;
		ELSE IF (count_rows(get_table(l_field_u)) != 0) DO
			write_log(_FUNCL_ + ": Unexpected value encountered: "  + count_rows(get_table(l_field_u)));
			RETURN FALSE;
		END

		g_field_u = table_d.f3;
		IF (g_field_u == NULL) DO
			write_log(_FUNCL_ + ": The table is NULL but there is storage for it, so the field reference is valid (non-NULL).");
			RETURN FALSE;
		ELSE IF (count_rows(get_table(g_field_u)) != 0) DO
			write_log(_FUNCL_ + ": Unexpected value encountered: "  + count_rows(get_table(g_field_u)));
			RETURN FALSE;
		END

		RETURN TRUE;
	END
	
	RETURN NULL;
ENDPROC


PROCEDURE test_whais_api_is_indexed(tc UINT8) RETURN BOOL
DO
	VAR table_d TABLE (f1 RICHREAL, f3 UINT8);
	VAR l_field_rr RICHREAL FIELD;
	VAR l_field_u FIELD;

	IF  (tc == 0) DO
		IF (is_indexed(NULL) != NULL) DO
			write_log(_FUNCL_ + ": This should return NULL! ");
			RETURN FALSE;
		ELSE IF (is_indexed(l_field_rr) != NULL) DO 
			write_log(_FUNCL_ + ": This should return NULL! ");
			RETURN FALSE;
		ELSE IF (is_indexed(table_d.f3) != FALSE) DO
			write_log(_FUNCL_ + ": This should be FALSE! ");
			RETURN FALSE;
		END

		l_field_rr = table_d.f1;
		l_field_u = table_d.f3;

		IF (is_indexed(l_field_rr) != FALSE) DO
			write_log(_FUNCL_ + ": This field should not be indexed.");
			RETURN FALSE;
		ELSE IF (is_indexed(l_field_u) !=FALSE) DO
			write_log(_FUNCL_ + ": This field should not be indexed.");
			RETURN FALSE;
		END
		RETURN TRUE;

	ELSE IF (tc == 1) DO
	 	IF (is_indexed(table_glb_index_fields_n.f1) != TRUE) DO
			write_log(_FUNCL_ + ": This field should be indexed.");
			RETURN FALSE;
	 	ELSE IF (is_indexed(table_glb_index_fields.f1) != TRUE) DO
			write_log(_FUNCL_ + ": This field should be indexed.");
			RETURN FALSE;

	 	ELSE IF (is_indexed(table_glb_index_fields.f5) != FALSE) DO
			write_log(_FUNCL_ + ": This field should not be indexed.");
			RETURN FALSE;
		END

		table_glb_index_fields.f1[0] = 0;
		l_field_u = table_glb_index_fields.f6;
		IF (is_indexed(l_field_u) != FALSE) DO
			write_log(_FUNCL_ + ": This field should not be indexed.");
			RETURN FALSE;
		END

		l_field_u = table_glb_index_fields.f4;
		IF (is_indexed(l_field_u) != FALSE) DO
			write_log(_FUNCL_ + ": This field should not be indexed.");
			RETURN FALSE;
		END

		l_field_u = table_glb_index_fields.f2;
		IF (is_indexed(l_field_u) != TRUE) DO
			write_log(_FUNCL_ + ": This field should be indexed.");
			RETURN FALSE;

		ELSE IF (is_indexed(table_glb_index_fields.f3) != TRUE) DO
			write_log(_FUNCL_ + ": This field should be indexed.");
			RETURN FALSE;
		END

		RETURN TRUE;
	END

	RETURN NULL;
ENDPROC


PROCEDURE test_whais_api_get_name(tc UINT8) RETURN BOOL
DO
	VAR  t TABLE (some_field TEXT, aint UINT8, SmartField DATE ARRAY);
	VAR f FIELD;

	IF  (tc == 0) DO
		IF (get_name(NULL) != NULL) DO
			write_log(_FUNCL_ + ": This result should be NULL.");
			RETURN FALSE;

		ELSE IF (get_name(f) != NULL) DO
			write_log(_FUNCL_ + ": This result should be NULL.");
			RETURN FALSE;
		END
		RETURN TRUE;

	ELSE IF (tc == 1) DO
		IF (get_name(t.SmartField) != "SmartField") DO
			write_log(_FUNCL_ + ": wrong value: " + get_name(t.SmartField));
			RETURN FALSE;
		END
		
		f = t.some_field;
		IF (get_name(f) != "some_field") DO
			write_log(_FUNCL_ + ": wrong value: " + get_name(f));
			RETURN FALSE;
		END

		f = t.aint;
		IF (get_name(f) != "aint") DO
			write_log(_FUNCL_ + ": wrong value: " + get_name(f));
			RETURN FALSE;
		END

		RETURN TRUE;

	ELSE IF (tc == 2) DO
		t.aint[0] = 2;	
		IF (get_name(t.aint) != "aint") DO
			write_log(_FUNCL_ + ": wrong value: " + get_name(t.aint));
			RETURN FALSE;
		END
		f = t.aint;
		IF (get_name(f) != "aint") DO
			write_log(_FUNCL_ + ": wrong value: " + get_name(f));
			RETURN FALSE;
		END

		f = t.some_field;
		IF (get_name(f) != "some_field") DO
			write_log(_FUNCL_ + ": wrong value: " + get_name(f));
			RETURN FALSE;
		END

		t = NULL;
		IF (get_name(f) != "some_field") DO
			write_log(_FUNCL_ + ": wrong value: " + get_name(f));
			RETURN FALSE;
		END

		f = t.aint;
		IF (get_name(f) != "aint") DO
			write_log(_FUNCL_ + ": wrong value: " + get_name(f));
			RETURN FALSE;
		END
		
		IF (t != NULL) DO
			write_log(_FUNCL_ + ": we expected the test table to be NULL");
			RETURN FALSE;
		END

		IF (count_rows(t) != 0) DO
			write_log(_FUNCL_ + ": looks like the test table is not well defined");
			RETURN FALSE;
		END
		
		RETURN TRUE;
	END

	RETURN NULL;
ENDPROC

PROCEDURE test_whais_api_get_smallest(tc UINT8) RETURN BOOL
DO
	VAR result UINT32 ARRAY;
	VAR tab TABLE (f1 INT8);
	VAR f  INT8 FIELD;

	IF (tc == 0) DO
		IF (get_smallest(NULL) != NULL) DO
			write_log(_FUNCL_ + ": expected null result");
			RETURN FALSE;
		ELSE IF (get_smallest(f) != NULL) DO
			write_log(_FUNCL_ + ": expected null result");
			RETURN FALSE;
		END
		RETURN TRUE;
	END

	tab.f1[0] = 1;
	tab.f1[1] = NULL;
	tab.f1[2] = -9;
	tab.f1[3] = 4;
	tab.f1[4] = 2;
	tab.f1[5] = NULL;
	tab.f1[6] = 3;
	tab.f1[7] = -9;
	tab.f1[8] = -3;
	tab.f1[9] = 4;
	tab.f1[10] = 0;
	tab.f1[11] = -2;
	tab.f1[12] = 1;

	IF (tc == 1) DO
		result = get_smallest(tab.f1);
		IF (count(result) != 2) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 2");
			RETURN FALSE;
		ELSE IF (result[0] != 2) DO
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		ELSE IF (result[1] != 7) DO
			write_log(_FUNCL_ + ": element " + result[1] + " not expected");
			RETURN FALSE;
		END

		f = tab.f1;
		result = get_smallest(f, -3);
		IF (count(result) != 1) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 1");
			RETURN FALSE;
		ELSE IF (result[0] != 11) DO
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		END

		result = get_smallest(f, 100);
		IF (count(result) != 0) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 0");
			RETURN FALSE;
		ELSE IF (result != NULL) DO
			write_log(_FUNCL_ + ": the array needs to be NULL");
			RETURN FALSE;
		END
		RETURN TRUE;
	END

	RETURN NULL;
ENDPROC

PROCEDURE test_whais_api_get_biggest(tc UINT8) RETURN BOOL
DO
	VAR result ARRAY UINT32;
	VAR tab TABLE (f1 INT8);
	VAR f FIELD INT8 ;

	IF (tc == 0) DO
		IF (get_biggest(NULL) != NULL) DO
			write_log(_FUNCL_ + ": expected null result");
			RETURN FALSE;
		ELSE IF (get_biggest(f) != NULL) DO
			write_log(_FUNCL_ + ": expected null result");
			RETURN FALSE;
		END
		RETURN TRUE;
	END

	tab.f1[0] = 1;
	tab.f1[1] = NULL;
	tab.f1[2] = -9;
	tab.f1[3] = 4;
	tab.f1[4] = 2;
	tab.f1[5] = NULL;
	tab.f1[6] = 3;
	tab.f1[7] = -9;
	tab.f1[8] = -3;
	tab.f1[9] = 4;
	tab.f1[10] = 0;
	tab.f1[11] = -2;
	tab.f1[12] = 1;

	IF (tc == 1) DO
		result = get_biggest(tab.f1);
		IF (count(result) != 2) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 2");
			RETURN FALSE;
		ELSE IF (result[0] != 3) DO
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		ELSE IF (result[1] != 9) DO
			write_log(_FUNCL_ + ": element " + result[1] + " not expected");
			RETURN FALSE;
		END

		f = tab.f1;
		result = get_biggest(f, 1);
		IF (count(result) != 1) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 1");
			RETURN FALSE;
		ELSE IF (result[0] != 10) DO
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		END

		result = get_biggest(f, -100);
		IF (count(result) != 0) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 0");
			RETURN FALSE;
		ELSE IF (result != NULL) DO
			write_log(_FUNCL_ + ": the array needs to be NULL");
			RETURN FALSE;
		END
		RETURN TRUE;
	END
	RETURN NULL;
ENDPROC

PROCEDURE test_whais_api_match_rows(tc UINT8) RETURN BOOL
DO
	VAR result ARRAY UINT32;
	VAR tab TABLE (f1 INT8);
	VAR f FIELD INT8 ;

	IF (tc == 0) DO
		IF (match_rows(NULL, 0, 100, 0, 12) != NULL) DO
			write_log(_FUNCL_ + ": the result needs to be NULL");
			RETURN FALSE;
		ELSE IF (match_rows(f, 0, 100, 0, 12) != NULL) DO
			write_log(_FUNCL_ + ": the result needs to be NULL");
			RETURN FALSE;
		END
		RETURN TRUE;
	END

        tab.f1[0] = 1;
        tab.f1[1] = NULL;
        tab.f1[2] = -9;
        tab.f1[3] = 4;
        tab.f1[4] = 5;
        tab.f1[5] = NULL;
        tab.f1[6] = 3;
        tab.f1[7] = -9;
        tab.f1[8] = -3;
        tab.f1[9] = 4;
        tab.f1[10] = 0;
        tab.f1[11] = -2;
        tab.f1[12] = 1;

	IF (tc == 1) DO
		result = match_rows(tab.f1, NULL, NULL);
		IF (count(result) != 2) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 2");
			RETURN FALSE;
		ELSE IF (result[0] != 1) DO 
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		ELSE IF (result[1] != 5) DO
			write_log(_FUNCL_ + ": element " + result[1] + " not expected");
			RETURN FALSE;
		END
		f = tab.f1;

		result = match_rows(tab.f1, 0, 3);
		IF (count(result) != 4) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 4");
			RETURN FALSE;
		ELSE IF (result[0] != 0) DO
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		ELSE IF (result[1] != 6) DO
			write_log(_FUNCL_ + ": element " + result[1] + " not expected");
			RETURN FALSE;
		ELSE IF (result[2] != 10) DO
			write_log(_FUNCL_ + ": element " + result[2] + " not expected");
			RETURN FALSE;
		ELSE IF (result[3] != 12) DO
			write_log(_FUNCL_ + ": element " + result[3] + " not expected");
			RETURN FALSE;
		END

		result = NULL;
		IF (count(result) != 0) DO
			write_log(_FUNCL_ + ": could not clean the array result");
			RETURN FALSE;
		END
		
		result = match_rows(tab.f1, 3, 0);
		IF (count(result) != 4) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 4");
			RETURN FALSE;
		ELSE IF (result[0] != 0) DO
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		ELSE IF (result[1] != 6) DO
			write_log(_FUNCL_ + ": element " + result[1] + " not expected");
			RETURN FALSE;
		ELSE IF (result[2] != 10) DO
			write_log(_FUNCL_ + ": element " + result[2] + " not expected");
			RETURN FALSE;
		ELSE IF (result[3] != 12) DO
			write_log(_FUNCL_ + ": element " + result[3] + " not expected");
			RETURN FALSE;
		END
		RETURN TRUE;

	ELSE IF (tc == 2) DO
		result = match_rows(tab.f1, 0, 5, 6, 9);
		IF (count(result) != 2) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 2");
			RETURN FALSE;
		ELSE IF (result[0] != 6) DO 
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		ELSE IF (result[1] != 9) DO
			write_log(_FUNCL_ + ": element " + result[1] + " not expected");
			RETURN FALSE;
		END

		result = NULL;
		IF (count(result) != 0) DO
			write_log(_FUNCL_ + ": could not clean the array result");
			RETURN FALSE;
		END

		result = match_rows(tab.f1, 0, 5, 9, 6);
		IF (count(result) != 2) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 2");
			RETURN FALSE;
		ELSE IF (result[0] != 6) DO 
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		ELSE IF (result[1] != 9) DO
			write_log(_FUNCL_ + ": element " + result[1] + " not expected");
			RETURN FALSE;
		END
		RETURN TRUE;

	ELSE IF (tc == 3) DO
		result = match_rows(tab.f1, NULL, NULL, NULL, 7);
		IF (count(result) != 2) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 2");
			RETURN FALSE;
		ELSE IF (result[0] != 1) DO 
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		ELSE IF (result[1] != 5) DO
			write_log(_FUNCL_ + ": element " + result[1] + " not expected");
			RETURN FALSE;
		END

		result = NULL;
		IF (count(result) != 0) DO
			write_log(_FUNCL_ + ": could not clean the array result");
			RETURN FALSE;
		END

		result = match_rows(tab.f1, NULL, NULL, 2, 7);
		IF (count(result) != 1) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 1");
			RETURN FALSE;
		ELSE IF (result[0] != 5) DO 
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		END
	
		RETURN TRUE;

	ELSE IF (tc == 4) DO
		result = match_rows(tab.f1, NULL, NULL, 2, NULL);
		IF (count(result) != 1) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 2");
			RETURN FALSE;
		ELSE IF (result[0] != 5) DO 
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		END

		result = match_rows(tab.f1, NULL, NULL, 2, 4);
		IF (count(result) != 0) DO
			write_log(_FUNCL_ + ": could not clean the array result");
			RETURN FALSE;
		END

		RETURN TRUE;
	END

	f = table_glb_index_fields.f2;
        f[0] = 1;
        f[1] = NULL;
        f[2] = -9;
        f[3] = 4;
        f[4] = 5;
        f[5] = NULL;
        f[6] = 3;
        f[7] = -9;
        f[8] = -3;
        f[9] = 4;
        f[10] = 0;
        f[11] = -2;
        f[12] = 1;

	IF (tc == 5) DO
		result = match_rows(f, NULL, NULL);
		IF (count(result) != 2) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 2");
			RETURN FALSE;
		ELSE IF (result[0] != 1) DO 
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		ELSE IF (result[1] != 5) DO
			write_log(_FUNCL_ + ": element " + result[1] + " not expected");
			RETURN FALSE;
		END
		f = f;

		result = match_rows(f, 0, 3);
		IF (count(result) != 4) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 4");
			RETURN FALSE;
		ELSE IF (result[0] != 10) DO
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		ELSE IF (result[1] != 0) DO
			write_log(_FUNCL_ + ": element " + result[1] + " not expected");
			RETURN FALSE;
		ELSE IF (result[2] != 12) DO
			write_log(_FUNCL_ + ": element " + result[2] + " not expected");
			RETURN FALSE;
		ELSE IF (result[3] != 6) DO
			write_log(_FUNCL_ + ": element " + result[3] + " not expected");
			RETURN FALSE;
		END

		result = NULL;
		IF (count(result) != 0) DO
			write_log(_FUNCL_ + ": could not clean the array result");
			RETURN FALSE;
		END
		
		result = match_rows(f, 3, 0);
		IF (count(result) != 4) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 4");
			RETURN FALSE;
		ELSE IF (result[0] != 10) DO
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		ELSE IF (result[1] != 0) DO
			write_log(_FUNCL_ + ": element " + result[1] + " not expected");
			RETURN FALSE;
		ELSE IF (result[2] != 12) DO
			write_log(_FUNCL_ + ": element " + result[2] + " not expected");
			RETURN FALSE;
		ELSE IF (result[3] != 6) DO
			write_log(_FUNCL_ + ": element " + result[3] + " not expected");
			RETURN FALSE;
		END
		RETURN TRUE;

	ELSE IF (tc == 6) DO
		result = match_rows(f, 0, 5, 6, 9);
		IF (count(result) != 2) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 2");
			RETURN FALSE;
		ELSE IF (result[0] != 6) DO 
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		ELSE IF (result[1] != 9) DO
			write_log(_FUNCL_ + ": element " + result[1] + " not expected");
			RETURN FALSE;
		END

		result = NULL;
		IF (count(result) != 0) DO
			write_log(_FUNCL_ + ": could not clean the array result");
			RETURN FALSE;
		END

		result = match_rows(f, 0, 5, 9, 6);
		IF (count(result) != 2) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 2");
			RETURN FALSE;
		ELSE IF (result[0] != 6) DO 
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		ELSE IF (result[1] != 9) DO
			write_log(_FUNCL_ + ": element " + result[1] + " not expected");
			RETURN FALSE;
		END
		RETURN TRUE;

	ELSE IF (tc == 7) DO
		result = match_rows(f, NULL, NULL, NULL, 7);
		IF (count(result) != 2) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 2");
			RETURN FALSE;
		ELSE IF (result[0] != 1) DO 
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		ELSE IF (result[1] != 5) DO
			write_log(_FUNCL_ + ": element " + result[1] + " not expected");
			RETURN FALSE;
		END

		result = NULL;
		IF (count(result) != 0) DO
			write_log(_FUNCL_ + ": could not clean the array result");
			RETURN FALSE;
		END

		result = match_rows(f, NULL, NULL, 2, 7);
		IF (count(result) != 1) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 1");
			RETURN FALSE;
		ELSE IF (result[0] != 5) DO 
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		END
	
		RETURN TRUE;

	ELSE IF (tc == 8) DO
		result = match_rows(f, NULL, NULL, 2, NULL);
		IF (count(result) != 1) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 2");
			RETURN FALSE;
		ELSE IF (result[0] != 5) DO 
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		END

		result = match_rows(f, NULL, NULL, 2, 4);
		IF (count(result) != 0) DO
			write_log(_FUNCL_ + ": could not clean the array result");
			RETURN FALSE;
		END

		RETURN TRUE;
	END

	RETURN NULL;
ENDPROC

PROCEDURE test_whais_api_filter_rows(tc UINT8) RETURN BOOL
DO
	VAR vals INT8 ARRAY;
	VAR rows, result, rows_t  ARRAY UINT32;
	VAR f FIELD INT8 ;

	vals = {-4, 2, 0}INT8;
	rows = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12} UINT32;

	IF (tc == 0) DO
		IF (filter_rows(NULL, vals, rows) != NULL) DO
			write_log(_FUNCL_ + ": the result needs to be NULL");
			RETURN FALSE;
		ELSE IF (filter_rows(f, vals, rows) != NULL) DO
			write_log(_FUNCL_ + ": the result needs to be NULL");
			RETURN FALSE;
		END
		RETURN TRUE;
	END

	f = table_glb_index_fields.f2;
        f[0] = -1;
        f[1] = NULL;
        f[2] = 9;
        f[3] = -4;
        f[4] = -5;
        f[5] = NULL;
        f[6] = NULL;
        f[7] = 9;
        f[8] = 3;
        f[9] = -4;
        f[10] = 0;
        f[11] = 2;
        f[12] = -1;

	IF (tc == 1) DO
		vals = NULL;
		IF (count(vals) != 0) DO
			write_log(_FUNCL_ + ": could not clean the values array.");
			RETURN FALSE;
		ELSE IF (filter_rows(f, NULL, rows) != NULL) DO
			write_log(_FUNCL_ + ": could not clean the values array.");
			RETURN FALSE;
		ELSE IF (filter_rows(f, vals, rows) != NULL) DO
			write_log(_FUNCL_ + ": could not clean the values array.");
			RETURN FALSE;
		END

		vals = {-255, 2, -5, 3, 89} INT8;
		result = filter_rows(f, vals, rows);
		IF (count(result) != 3) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 3");
			RETURN FALSE;
		ELSE IF (result[0] != 4)   DO 
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		ELSE IF (result[1] != 8)   DO 
			write_log(_FUNCL_ + ": element " + result[1] + " not expected");
			RETURN FALSE;
		ELSE IF (result[2] != 11)   DO 
			write_log(_FUNCL_ + ": element " + result[2] + " not expected");
			RETURN FALSE;
		END
		RETURN TRUE;

	ELSE IF (tc == 2) DO
		vals = {-255, 2, -5, 3, 89} INT8;
		IF (filter_rows(f, vals, NULL) != NULL) DO
			write_log(_FUNCL_ + ": the result needs to be NULL");
			RETURN FALSE;
		END
		rows = NULL;
		IF (count(rows) != 0) DO
			write_log(_FUNCL_ + ": could not clean the rows array.");
			RETURN FALSE;
		END
		IF (filter_rows(f, vals, rows_t) != NULL) DO
			write_log(_FUNCL_ + ": the result needs to be NULL");
			RETURN FALSE;
		ELSE IF (filter_rows(f, vals, rows) != NULL) DO
			write_log(_FUNCL_ + ": the result needs to be NULL");
			RETURN FALSE;
		END

		rows = {1, 3, 8, 10, 2, 4, 6} UINT32;
		result = filter_rows(f, vals, rows);
		IF (count(result) != 2) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 3");
			RETURN FALSE;
		ELSE IF (result[0] != 8)   DO 
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		ELSE IF (result[1] != 4)   DO 
			write_log(_FUNCL_ + ": element " + result[1] + " not expected");
			RETURN FALSE;
		END

		RETURN TRUE;
	
	ELSE IF (tc == 3) DO
		result = filter_rows(f, NULL, rows, TRUE);
		IF (count(result) != 3) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 3");
			RETURN FALSE;
		ELSE IF (result[0] != 1)   DO 
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		ELSE IF (result[1] != 5)   DO 
			write_log(_FUNCL_ + ": element " + result[1] + " not expected");
			RETURN FALSE;
		ELSE IF (result[2] != 6)   DO 
			write_log(_FUNCL_ + ": element " + result[2] + " not expected");
			RETURN FALSE;
		END

		vals = NULL;
		rows = {8, 7, 1, 9, 6, 3} UINT32; 
		result = filter_rows(f, vals, rows, TRUE);
		IF (count(result) != 2) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 2");
			RETURN FALSE;
		ELSE IF (result[0] != 1)   DO 
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		ELSE IF (result[1] != 6)   DO 
			write_log(_FUNCL_ + ": element " + result[2] + " not expected");
			RETURN FALSE;
		END

		result = filter_rows(f, vals , rows, FALSE);
		IF (count(result) != 0) DO
			write_log(_FUNCL_ + ": the resulted array should be NULL");
			RETURN FALSE;
		END

		result = filter_rows(f, vals , rows);
		IF (count(result) != 0) DO
			write_log(_FUNCL_ + ": the resulted array should be NULL");
			RETURN FALSE;
		END

		RETURN TRUE;
	ELSE IF (tc == 4) DO
		vals = NULL;
		rows = {8, 7, 1, 9, 6, 3} UINT32; 
		result = filter_rows(f, NULL, rows, FALSE, TRUE);
		IF (count(rows) != count(result)) DO 
			write_log(_FUNCL_ + ": the resulted array should be identic");
			RETURN FALSE;
		END
	
		FOR (r : rows)
			IF (r != result[@r]) DO
				write_log(_FUNCL_ + ": at index " + @r + " the result is " + result[@r] + " rather than " + r);
				RETURN FALSE;
			END

		result = filter_rows(f, vals, rows, FALSE, FALSE);
		IF (result != NULL) DO
			write_log(_FUNCL_ + ": the resulted array should be NULL");
			RETURN FALSE;
		END

		result = filter_rows(f, vals, rows, TRUE, FALSE);
		IF (count(result) != 2) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 2");
			RETURN FALSE;
		ELSE IF (result[0] != 1)   DO 
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		ELSE IF (result[1] != 6)   DO 
			write_log(_FUNCL_ + ": element " + result[1] + " not expected");
			RETURN FALSE;
		END

		vals += 3;
		result = filter_rows(f, vals, rows, TRUE, TRUE);
		IF (count(result) != 3) DO
			write_log(_FUNCL_ + ": array size " + count(result) + " not expected, needs to be 3");
			RETURN FALSE;
		ELSE IF (result[0] != 7)   DO 
			write_log(_FUNCL_ + ": element " + result[0] + " not expected");
			RETURN FALSE;
		ELSE IF (result[1] != 9)   DO 
			write_log(_FUNCL_ + ": element " + result[1] + " not expected");
			RETURN FALSE;
		ELSE IF (result[2] != 3)   DO 
			write_log(_FUNCL_ + ": element " + result[1] + " not expected");
			RETURN FALSE;
		END
		RETURN TRUE;
	END

	RETURN NULL;
ENDPROC
