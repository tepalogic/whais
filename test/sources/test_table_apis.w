#
# Test module used to verify the correct handling of global values.            #
#                                                                              #
################################################################################

#@include whais_std.wh
#@include ./test_exec_db/globals.wh

VAR test_global_table_is_persistent, test_global_table_is_persistent_2 TABLE (a_field RICHREAL);
VAR test_global_some_table TABLE;

PROCEDURE test_whais_api_is_persistent(tc UINT16) RETURN BOOL
DO
   VAR tlocal TABLE (f1 TEXT);
   VAR tlocal2 TABLE;

   IF (tc == 0)
      RETURN is_persistent(NULL) == NULL;

   ELSE IF (tc == 1)
      RETURN is_persistent(table_glb_persistent) == TRUE;

   ELSE IF (tc == 2) DO 
      table_glb_persistent_2.f2[0] = -100;
      RETURN is_persistent(table_glb_persistent_2) == TRUE;

   ELSE IF (tc == 3) 
      RETURN is_persistent(test_global_table_is_persistent) == FALSE;

   ELSE IF (tc == 4) DO
      test_global_table_is_persistent_2.a_field[0] = 0.01;
      RETURN is_persistent(test_global_table_is_persistent_2) == FALSE;

   ELSE IF (tc == 5) 
      RETURN is_persistent(tlocal) == FALSE;

   ELSE IF (tc == 6) DO
      tlocal.f1[0] = "Text to add a row!";
      RETURN is_persistent(tlocal)  == FALSE;

   ELSE IF (tc == 7) 
      RETURN is_persistent(test_global_some_table) == NULL;

   ELSE IF (tc == 8) 
      RETURN is_persistent(tlocal2) == NULL;

   RETURN NULL;
ENDPROC

PROCEDURE test_whais_api_add_row(tc UINT16) RETURN BOOL
DO
   VAR tlocal TABLE (f1 TEXT);
   VAR tlocal2 TABLE;

   IF (tc == 0)
      RETURN add_row(NULL) == NULL;

   ELSE IF (tc == 1) DO
      rowsCount = count_rows(test_global_table_is_persistent_2);
      RETURN add_row(test_global_table_is_persistent_2) == rowsCount;

   ELSE IF (tc == 2) DO 
      ri  = add_row(test_global_table_is_persistent_2);
      ri2 = add_row(test_global_table_is_persistent_2);
      IF ((ri == NULL) || (ri2 == NULL) || (ri2 != ri + 1)) DO
	RETURN FALSE;
      ELSE
      	RETURN TRUE;

   ELSE IF (tc == 3) DO 
      IF (tlocal != NULL) DO
	write_log(_LINE_ + ": failed with tc: '" + tc + "' becuse local table is not NULL"); 	
	RETURN FALSE;
      END

      ri = add_row(tlocal);
      IF (ri != 0) DO
	write_log(_LINE_ + ": failed with tc: '" + tc + "',  ri: '" + ri + "'."); 	
	RETURN FALSE;
      END

      RETURN TRUE;
   ELSE IF (tc == 4) DO
      IF (tlocal != NULL) DO
	write_log( _LINE_ + ": failed with tc: '" + tc + "' because local table is not NULL"); 	
	RETURN FALSE;
      END
      ri  = add_row(tlocal);
      ri2 = add_row(tlocal);
      IF ((ri == NULL) || (ri2 == NULL) || (ri2 != ri + 1) || (ri2 != 1)) DO
	write_log( _LINE_+ ": failed with tc: '" + tc + "',  ri: '" + ri + "'  ri2: '" + ri2 + "'."); 	
	RETURN FALSE;
      END
      RETURN TRUE;

   ELSE IF (tc == 5)
      RETURN add_row(test_global_some_table) == NULL;

   ELSE IF (tc == 6)
      RETURN add_row(tlocal2) == NULL;
    
   RETURN NULL;
ENDPROC

PROCEDURE test_whais_api_empty_row(tc UINT8) RETURN BOOL
DO
   VAR tlocal TABLE (f1 TEXT, f2 UINT8, f3 INT8 ARRAY);
   VAR tlocal2 TABLE;

    IF (tc == 0) DO
	IF (empty_row(NULL, 10) == NULL) 
		RETURN TRUE;
	ELSE DO
		write_log (_FUNC_ + ": failed for tc: " + tc);
		RETURN FALSE;	
	END
    ELSE IF (tc == 1) DO 
	IF (empty_row(tlocal2, 0) == NULL) DO
		RETURN TRUE;
	ELSE DO
		write_log (_FUNCL_ + ": failed for tc: " + tc);
		RETURN FALSE;	
	END
   ELSE IF (tc == 2) DO 
	IF (empty_row(tlocal, 0) == FALSE) DO
		RETURN TRUE;
	ELSE DO
		write_log (_FUNCL_ + ": failed for tc: " + tc);
		RETURN FALSE;	
	END
   ELSE IF (tc == 3) DO 
	tlocal2 = tlocal;
	IF (empty_row(tlocal2, 0) == FALSE) DO
		RETURN TRUE;
	ELSE DO
		write_log (_FUNCL_ + ": failed for tc: " + tc);
		RETURN FALSE;	
	END
   ELSE IF (tc == 4) DO 
	tlocal.f1[0]="Test";
	IF (empty_row(tlocal, 0) == TRUE) DO
		RETURN TRUE;
	ELSE DO
		write_log (_FUNCL_ + ": failed for tc: " + tc);
		RETURN FALSE;
	END
    ELSE IF (tc == 5) DO 
	tlocal.f1[0]="Test";
	tlocal2 = tlocal;
	IF (empty_row(tlocal2, 0) == TRUE) DO
		RETURN TRUE;
	ELSE DO
		write_log (_FUNCL_ + ": failed for tc: " + tc);
		RETURN FALSE;
	END
    ELSE IF (tc == 6) DO 
	IF (table_glb_persistent != NULL) DO
		write_log(_FUNCL_ + ": failed for tc: " + tc + " because table_glb_persistent is not NULL.");
		RETURN NULL;
	END
	IF (empty_row(table_glb_persistent, 0) == FALSE) DO
		RETURN TRUE;
	ELSE DO
		write_log (_FUNCL_ + ": failed for tc: " + tc);
		RETURN FALSE;
	END
    ELSE IF (tc == 7) DO 
      	table_glb_persistent_2.f2[0] = -100;
	IF (empty_row(table_glb_persistent_2, 0) == TRUE) DO
		RETURN TRUE;
	ELSE DO
		write_log (_FUNCL_ + ": failed for tc: " + tc);
		RETURN FALSE;
	END
    ELSE IF (tc == 8) DO
	IF (empty_row(table_glb_persistent_2, NULL) == FALSE) DO
		RETURN TRUE;
	ELSE
		RETURN FALSE;
    ELSE IF (tc == 9) DO
	IF (empty_row(table_glb_persistent_2, 1000000) == FALSE) DO
		RETURN TRUE;
	ELSE
		RETURN FALSE;
    END

    RETURN NULL;
ENDPROC

PROCEDURE test_whais_api_get_empty(tc UINT8) RETURN BOOL
DO
   VAR tlocal TABLE (f1 TEXT, f2 UINT8, f3 INT16 ARRAY);
   VAR tlocal2 TABLE;

   IF (tc == 0) DO
	IF (get_empty(NULL) != NULL)
		RETURN FALSE;
	ELSE
	   RETURN TRUE;
   ELSE IF (tc == 1) DO
	IF (get_empty(tlocal2) != NULL)
		RETURN FALSE;
	ELSE
	   RETURN TRUE;
   ELSE IF (tc == 2)
	IF (get_empty(test_global_some_table) != NULL)
		RETURN FALSE;
	ELSE
	   	RETURN TRUE;
   ELSE IF (tc == 3) DO
        tlocal2 = tlocal;
	IF (get_empty(tlocal2) != 0)
		RETURN FALSE;
	ELSE
		RETURN TRUE;
   ELSE IF (tc == 4) DO
	tlocal.f2[0] = 1;
	tlocal.f2[1] = 2;
	tlocal.f2[2] = 2;

	empty_row(tlocal, 1);
	IF (get_empty(tlocal) != 1)
		RETURN FALSE;

	tlocal.f2[1] = 4;
	tlocal.f2[0] = NULL;

	IF (get_empty(tlocal) != 0) DO
		write_log(_FUNCL_ + ": the second phase of the log did not work");
		RETURN FALSE;
	ELSE
		RETURN TRUE;
    ELSE IF (tc == 5) DO
	test_global_table_is_persistent_2.a_field[0] = -100;

	IF (get_empty(test_global_table_is_persistent_2) < 1)
		RETURN FALSE;
	empty_row(test_global_table_is_persistent_2, 0);

	IF (get_empty(test_global_table_is_persistent_2) != 0) DO
		write_log(_FUNCL_ + ": failed because the empty row did not work as expcted");
		RETURN FALSE;
	ELSE
		RETURN TRUE;
     END

    RETURN NULL;
ENDPROC


PROCEDURE test_whais_api_count_fields(tc UINT8) RETURN BOOL
DO
    VAR tlocal TABLE;
    VAR tlocal2 TABLE (f1 REAL, f2 TEXT, f3 DATE, f4 RICHREAL);

    IF (tc == 0)
	IF (count_fields(NULL) != 0) 
		RETURN FALSE;
	ELSE
		RETURN TRUE;
    ELSE IF (tc == 1)
        IF (count_fields(tlocal) != 0)
		RETURN FALSE;
	ELSE
		RETURN TRUE;
    ELSE IF (tc == 2)
	IF (count_fields(test_global_some_table) != 0)
		RETURN FALSE;
	ELSE
		RETURN TRUE;
    ELSE IF (tc == 3)
	IF (count_fields(tlocal2) != 4)
		RETURN FALSE;
	ELSE
		RETURN TRUE;
    ELSE IF (tc == 4)
	IF (count_fields(table_glb_persistent_2) != 1)
		RETURN FALSE;
	ELSE
		RETURN TRUE;

     ELSE IF (tc == 5)
	IF (count_fields(test_global_table_is_persistent) != 1)
		RETURN FALSE;
	ELSE
		RETURN TRUE;
	
     RETURN NULL;    
ENDPROC

PROCEDURE test_whais_api_get_fieldth(tc UINT8) RETURN BOOL
DO
    VAR tlocal TABLE (f1 REAL, f2 TEXT, f3 DATE, f4 RICHREAL);
    VAR tlocal2 TABLE;

    IF (tc == 0)
	IF (get_fieldth(NULL, 0) == NULL)
		RETURN TRUE;
	ELSE
		RETURN FALSE;
    ELSE IF (tc == 1)
	IF (get_fieldth(tlocal2, 0) == NULL)
		RETURN TRUE;
	ELSE
		RETURN FALSE;
    ELSE IF (tc == 2)
	IF (get_fieldth(test_global_some_table, 0) == NULL)
		RETURN TRUE;
	ELSE
		RETURN FALSE;

    ELSE IF (tc == 3)
	IF (get_fieldth(tlocal, 0) != NULL)
		RETURN TRUE;
	ELSE
		RETURN FALSE;

    ELSE IF (tc == 4)
	IF (get_fieldth(tlocal, 5) != NULL)
		RETURN FALSE;
	ELSE
		RETURN TRUE;
    ELSE IF (tc == 5)
	IF (get_fieldth(test_global_table_is_persistent_2, 0) != NULL)
		RETURN TRUE;
	ELSE
		RETURN FALSE;
    ELSE IF (tc == 6)
	IF (get_fieldth(test_global_table_is_persistent_2, NULL) == NULL)
		RETURN TRUE;
	ELSE
		RETURN FALSE;

    RETURN NULL;
ENDPROC

PROCEDURE test_whais_api_get_field(tc UINT8) RETURN BOOL
DO
    VAR tlocal TABLE (f1 REAL, f2 TEXT, f3 DATE, f4 RICHREAL);
    VAR tlocal2 TABLE;

    IF (tc == 0)
	IF (get_field(NULL, "f1") == NULL)
		RETURN TRUE;
	ELSE
		RETURN FALSE;
    ELSE IF (tc == 1)
	IF (get_field(tlocal2, "f2") == NULL)
		RETURN TRUE;
	ELSE
		RETURN FALSE;
    ELSE IF (tc == 2)
	IF (get_field(tlocal) == NULL)
		RETURN TRUE;
	ELSE
		RETURN FALSE;
    ELSE IF (tc == 3)
	IF (get_field(test_global_table_is_persistent, "A_random_field_name") == NULL)
		RETURN TRUE;
	ELSE
		RETURN FALSE;

    RETURN NULL;
ENDPROC


PROCEDURE test_whais_api_count_rows(tc UINT8) RETURN BOOL
DO
    VAR tlocal TABLE (f1 REAL, f2 TEXT, f3 DATE, f4 RICHREAL);
    VAR tlocal2 TABLE;
    
    IF (tc == 0)
	IF (count_rows(NULL, FALSE) == NULL)
		RETURN TRUE;
	ELSE
	        RETURN FALSE;

    ELSE IF (tc == 1)
	IF (count_rows(tlocal2, FALSE) == NULL)
		RETURN TRUE;
	ELSE
		RETURN FALSE;
    ELSE IF (tc == 2)
	IF (count_rows(test_global_some_table, FALSE) == NULL)
		RETURN TRUE;
	ELSE
		RETURN FALSE;
    ELSE IF (tc == 3)
	IF (count_rows(tlocal) == 0)
		RETURN TRUE;
	ELSE
		RETURN FALSE;
    ELSE IF (tc == 4)
	IF (count_rows(test_global_table_is_persistent, FALSE) == 0)
		RETURN TRUE;
	ELSE
		RETURN FALSE;
    ELSE IF (tc == 5) DO
	tlocal.f1[0]=0.35;
	IF (count_rows(tlocal) != 1)
		RETURN FALSE;

	IF (count_rows(tlocal, TRUE) != 0) DO
		write_log(_FUNCL_ + ": failed because there should be no empty rows");
		RETURN FALSE;
	END

	add_row(tlocal);
	IF (count_rows(tlocal, TRUE) != 1) DO
		write_log(_FUNCL_ + ": failed because there should just one empty row.");
		RETURN FALSE;
	ELSE IF (count_rows(tlocal) != 2) DO
		write_log(_FUNCL_ + ": failed because there should be exactly two rows allocated.");
		RETURN FALSE;
	ELSE
		RETURN TRUE;
    END

    RETURN NULL;
ENDPROC

PROCEDURE test_whais_api_exchg_rows(tc UINT8) RETURN BOOL
DO
    VAR tlocal TABLE (f1 REAL, f2 TEXT, f3 DATE, f4 RICHREAL);
    VAR tlocal2 TABLE;

    IF (tc == 0)
    	IF (exchg_rows(NULL, 0, 1) != NULL)
		RETURN FALSE;
	ELSE
		RETURN TRUE;
    
    ELSE IF (tc == 1)
	IF (exchg_rows(test_global_some_table, 1, 0) != NULL)
		RETURN FALSE;
	ELSE
		RETURN TRUE;

    tlocal.f1[0] = 0.01;
    tlocal.f4[0] = 0.04;
    add_row(tlocal);
    tlocal.f1[2] = 2.01;
    tlocal.f4[2] = 2.04;

    IF (tc == 2) DO
	IF (exchg_rows(tlocal, 0, 3) != FALSE)
		RETURN FALSE;
	ELSE
		RETURN TRUE;
    ELSE IF (tc == 3) DO	
	IF (exchg_rows(tlocal, -1, 2) != FALSE)
		RETURN FALSE;
	ELSE
		RETURN TRUE;
    ELSE IF (tc == 4) DO
	tlocal2 = tlocal;
	IF (exchg_rows(tlocal2, 0, 2) != TRUE) DO
		write_log(_FUNCL_ + ": this should have been OK the alias.");
		RETURN FALSE;
	END

	IF ((tlocal.f1[2] != 0.01) OR (tlocal.f4[2] != 0.04)
            OR (tlocal.f1[0] != 2.01) OR (tlocal.f4[0] != 2.04)) DO
		write_log(_FUNCL_ + ": exch rows failed with tlocal.f1[0]='" + tlocal.f1[0] +
                          "' tlocal.f4[0] = '" + tlocal.f4[0] + "' tlocal.f1[2] = '" + tlocal.f1[2] + 
                          "' tlocal.f4[2] = '" + tlocal.f4[2] + "'.");
		RETURN FALSE;

	ELSE
		RETURN TRUE;
     ELSE IF (tc == 5)
	IF (exchg_rows(tlocal, 0, NULL) != FALSE)
		RETURN FALSE;
	ELSE
		RETURN TRUE;
     ELSE IF (tc == 6)
	IF (exchg_rows(tlocal, NULL, 1) != FALSE)
		RETURN FALSE;
	ELSE
		RETURN TRUE;

     RETURN NULL;
ENDPROC

PROCEDURE test_whais_api_sort_table(tc UINT8) RETURN BOOL
DO
    VAR tlocal TABLE (f1 REAL, f2 UINT32, f3 DATE);
    VAR tlocal2 TABLE;
    VAR cols UINT16 ARRAY;
    VAR desc BOOL ARRAY;

    tlocal.f1[0] = 0.01; tlocal.f2[0] = 0;
    tlocal.f1[1] = 1.01; tlocal.f2[1] = NULL; tlocal.f3[1] = '2017/11/01';
    tlocal.f1[2] = 1.01; tlocal.f2[2] = 3; tlocal.f3[2] = '2017-10-03';
    tlocal.f1[3] = -3.01; tlocal.f2[3] = 3; tlocal.f3[3] = '2017-10-25';
    tlocal.f1[4] = 0.01; tlocal.f2[4] = 6; tlocal.f3[4] = '2016-10-25';
    tlocal.f1[5] = 5.01; tlocal.f2[5] = 2; tlocal.f3[5] = '2016-10-25';
    tlocal.f1[6] = NULL; tlocal.f2[6] = 1; tlocal.f3[6] = '-2016-10-25';
    tlocal.f1[7] = -2.01; tlocal.f2[7] = 0; tlocal.f3[7] = '1981-10-21';

    cols[0] = 1;
    cols[1] = 2;
    cols[2] = 0;

    desc[0] = TRUE;
    desc[1] = FALSE;
    desc[2] = FALSE;

    IF (tc == 0) 
	IF (sort_table(NULL, cols, desc) == NULL)
		RETURN TRUE;
	ELSE
		RETURN FALSE;

    ELSE IF (tc == 1)
	IF (sort_table(test_global_some_table, cols, desc) == NULL)
		RETURN TRUE;
	ELSE
		RETURN FALSE;

    ELSE IF (tc == 2) DO
	cols[0] = 1231;
	IF (sort_table(tlocal, cols, desc) == FALSE) 
		RETURN TRUE;
	ELSE
		RETURN FALSE;

    ELSE IF (tc == 3) DO
	desc[3] = FALSE;
	IF (sort_table(tlocal, cols, desc) == FALSE)
		RETURN TRUE;
	ELSE
		RETURN FALSE;
    ELSE IF (tc == 4) 
	IF (sort_table(tlocal, cols, desc, -1) == FALSE)
		RETURN TRUE;
	ELSE
		RETURN FALSE;
    ELSE IF (tc == 5) 
	IF (sort_table(tlocal, cols, desc, 1, 90912) == FALSE)
		RETURN TRUE;
	ELSE
		RETURN FALSE;

    ELSE IF (tc == 6) DO
	IF (sort_table(tlocal, cols, desc, 1, 4) != TRUE)
		RETURN FALSE;

	IF ((tlocal.f1[0] != 0.01) || (tlocal.f2[0] != 0) || (tlocal.f3[0] != NULL)) DO
		write_log(_FUNCL_ + ": failed with: " +
			  "tlocal.f1[0] = '" + tlocal.f1[0] + 
			  "' tlocal.f2[0] = '" + tlocal.f2[0] + 
			  "' tlocal.f3[0] = '" + tlocal.f3[0] + "'.");
		RETURN FALSE;
	END

	IF ((tlocal.f1[1] != 0.01) || (tlocal.f2[1] != 6) || (tlocal.f3[1] != '2016-10-25')) DO
		write_log(_FUNCL_ + ": failed with: " +
			  "tlocal.f1[1] = '" + tlocal.f1[1] + 
			  "' tlocal.f2[1] = '" + tlocal.f2[1] + 
			  "' tlocal.f3[1] = '" + tlocal.f3[1] + "'.");
		RETURN FALSE;
	END

	IF ((tlocal.f1[2] != 1.01) || (tlocal.f2[2] != 3) || (tlocal.f3[2] != '2017-10-03')) DO
		write_log(_FUNCL_ + ": failed with: " +
			  "tlocal.f1[2] = '" + tlocal.f1[2] + 
			  "' tlocal.f2[2] = '" + tlocal.f2[2] + 
			  "' tlocal.f3[2] = '" + tlocal.f3[2] + "'.");
		RETURN FALSE;
	END

	IF ((tlocal.f1[3] != -3.01) || (tlocal.f2[3] != 3) || (tlocal.f3[3] != '2017-10-25')) DO
		write_log(_FUNCL_ + ": failed with: " +
			  "tlocal.f1[3] = '" + tlocal.f1[3] + 
			  "' tlocal.f2[3] = '" + tlocal.f2[3] + 
			  "' tlocal.f3[3] = '" + tlocal.f3[3] + "'.");
		RETURN FALSE;
	END

	IF ((tlocal.f1[4] != 1.01) || (tlocal.f2[4] != NULL) || (tlocal.f3[4] != '2017-11-01')) DO
		write_log(_FUNCL_ + ": failed with: " +
			  "tlocal.f1[4] = '" + tlocal.f1[4] + 
			  "' tlocal.f2[4] = '" + tlocal.f2[4] + 
			  "' tlocal.f3[4] = '" + tlocal.f3[4] + "'.");
		RETURN FALSE;
	END


	IF ((tlocal.f1[5] != 5.01) || (tlocal.f2[5] != 2) || (tlocal.f3[5] != '2016-10-25')) DO
		write_log(_FUNCL_ + ": failed with: " +
			  "tlocal.f1[5] = '" + tlocal.f1[5] + 
			  "' tlocal.f2[5] = '" + tlocal.f2[5] + 
			  "' tlocal.f3[5] = '" + tlocal.f3[5] + "'.");
		RETURN FALSE;
	END
	RETURN TRUE;

    ELSE IF (tc == 7) DO
	cols[2] = NULL;
	IF (sort_table(tlocal, cols, NULL, 6, 7) != TRUE)
		RETURN FALSE;

	IF ((tlocal.f1[6] != -2.01) || (tlocal.f2[6] != 0) || (tlocal.f3[6] != '1981-10-21')) DO
		write_log(_FUNCL_ + ": failed with: " +
			  "tlocal.f1[6] = '" + tlocal.f1[6] + 
			  "' tlocal.f2[6] = '" + tlocal.f2[6] + 
			  "' tlocal.f3[6] = '" + tlocal.f3[6] + "'.");
		RETURN FALSE;
	END

	IF ((tlocal.f1[7] != NULL) || (tlocal.f2[7] != 1) || (tlocal.f3[7] != '-2016-10-25')) DO
		write_log(_FUNCL_ + ": failed with: " +
			  "tlocal.f1[7] = '" + tlocal.f1[7] + 
			  "' tlocal.f2[7] = '" + tlocal.f2[7] + 
			  "' tlocal.f3[7] = '" + tlocal.f3[7] + "'.");
		RETURN FALSE;
	END
	RETURN TRUE;
    END
    RETURN NULL;
ENDPROC


