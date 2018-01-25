#@include whais_std.wh


VAR glb_array, glb_array_n DATE ARRAY;
VAR glb_array_u, glb_array_u_n ARRAY;
VAR g_table_array, g_table_array_n  TABLE (u16 ARRAY UINT16, b BOOL ARRAY, ht ARRAY HIRESTIME);

PROCEDURE dummy_array_value_bool() RETURN BOOL
DO
	RETURN NULL;
ENDPROC

PROCEDURE dummy_array_value_real() RETURN REAL
DO
	RETURN NULL;
ENDPROC


PROCEDURE test_whais_api_count(tc UINT8) RETURN BOOL
DO
	VAR l_table TABLE (u16 ARRAY UINT16, b BOOL ARRAY, ht ARRAY HIRESTIME);
	VAR local_array REAL ARRAY;
	VAR local_array_u ARRAY;
	

	IF (tc == 0) DO
		IF (count(NULL) != 0) DO
			write_log(_FUNCL_ + ": result should be 0.");
			RETURN FALSE;
		ELSE IF (count(glb_array) != 0) DO
			write_log(_FUNCL_ + ": result should be 0.");
			RETURN FALSE;
		ELSE IF (count(glb_array_u_n) != 0) DO
			write_log(_FUNCL_ + ": result should be 0.");
			RETURN FALSE;
		ELSE IF (count(local_array_u) != 0) DO
			write_log(_FUNCL_ + ": result should be 0.");
			RETURN FALSE;
		ELSE IF (count(g_table_array_n.u16[0]) != 0) DO
			write_log(_FUNCL_ + ": result should be 0.");
			RETURN FALSE;
		ELSE IF (count(l_table.b[0]) != 0) DO
			write_log(_FUNCL_ + ": result should be 0.");
			RETURN FALSE;
		END
		RETURN TRUE;
	END

	g_table_array.u16[0] = {1, 2, 3, 4, 6, 6} UINT16;
	l_table.b[0] = {FALSE, TRUE, dummy_array_value_bool(), FALSE};
	local_array = {1.1, -2.2, -3.3, dummy_array_value_real(), 4.4} REAL;

	IF (tc == 1) DO 
		IF (count(g_table_array.b[0]) != 0) DO
			write_log(_FUNCL_ + ": result should be 0.");
			RETURN FALSE;
		ELSE IF (count(l_table.u16[0]) != 0) DO
			write_log(_FUNCL_ + ": result should be 0.");
			RETURN FALSE;
		ELSE IF (count(g_table_array.u16[0]) != 6) DO
			write_log(_FUNCL_ + ": result should be 6.");
			RETURN FALSE;
		ELSE IF (count(l_table.b[0]) != 3) DO
			write_log(_FUNCL_ + ": result should be 3.");
			RETURN FALSE;
		ELSE IF (count(local_array) != 4) DO
			write_log(_FUNCL_ + ": result should be 4.");
			RETURN FALSE;
		END
		RETURN TRUE;
	END

	g_table_array.u16[0][3] = NULL;
	l_table.b[0][1] = NULL;
	glb_array_u = local_array;
	local_array[0] = NULL;

	IF (tc == 2) DO
		IF (count(g_table_array.u16[0]) != 5) DO
			write_log(_FUNCL_ + ": result should be 6.");
			RETURN FALSE;
		ELSE IF (count(l_table.b[0]) != 2) DO
			write_log(_FUNCL_ + ": result should be 2.");
			RETURN FALSE;
		ELSE IF (count(local_array) != 3) DO
			write_log(_FUNCL_ + ": result should be 3.");
			RETURN FALSE;
		ELSE IF (count(glb_array_u) != 4) DO
			write_log(_FUNCL_ + ": result should be 4.");
			RETURN FALSE;
		END

		RETURN TRUE;

	ELSE IF (tc == 3) DO
		IF (g_table_array.u16[0][0] != 1) DO
			write_log(_FUNCL_ + ": unexpected array value."); 
			RETURN FALSE;
		ELSE IF (g_table_array.u16[0][1] != 2) DO
			write_log(_FUNCL_ + ": unexpected array value."); 
			RETURN FALSE;
		ELSE IF (g_table_array.u16[0][2] != 3) DO
			write_log(_FUNCL_ + ": unexpected array value."); 
			RETURN FALSE;
		ELSE IF (g_table_array.u16[0][3] != 6) DO
			write_log(_FUNCL_ + ": unexpected array value."); 
			RETURN FALSE;
		ELSE IF (g_table_array.u16[0][4] != 6) DO
			write_log(_FUNCL_ + ": unexpected array value."); 
			RETURN FALSE;

		ELSE IF (l_table.b[0][0] != FALSE) DO
			write_log(_FUNCL_ + ": unexpected array value."); 
			RETURN FALSE;
		ELSE IF (l_table.b[0][1] != FALSE) DO
			write_log(_FUNCL_ + ": unexpected array value."); 
			RETURN FALSE;

		ELSE IF (local_array[0] != -2.2) DO
			write_log(_FUNCL_ + ": unexpected array value."); 
			RETURN FALSE;
		ELSE IF (local_array[1] != -3.3) DO
			write_log(_FUNCL_ + ": unexpected array value."); 
			RETURN FALSE;
		ELSE IF (local_array[2] != 4.4) DO
			write_log(_FUNCL_ + ": unexpected array value."); 
			RETURN FALSE;
		END
		
		RETURN TRUE;
	END
	RETURN NULL;
ENDPROC

PROCEDURE test_whais_api_sort(tc UINT8) RETURN BOOL
DO
	VAR l_table TABLE (u16 ARRAY UINT16, b BOOL ARRAY, ht ARRAY HIRESTIME);
	VAR local_array REAL ARRAY;
	VAR local_array_u ARRAY;

	IF (tc == 0) DO
		IF (sort(NULL, NULL, NULL) != 0) DO
			write_log(_FUNCL_ + ": result should be 0.");
			RETURN FALSE;
		ELSE IF (sort(local_array) != 0) DO
			write_log(_FUNCL_ + ": result should be 0.");
			RETURN FALSE;
		ELSE IF(sort(glb_array_u_n) != 0) DO
			write_log(_FUNCL_ + ": result should be 0.");
			RETURN FALSE;
		ELSE IF (sort(l_table.ht[0]) != 0) DO
			write_log(_FUNCL_ + ": result should be 0.");
			RETURN FALSE;
		ELSE IF (sort(g_table_array_n.ht[0]) != 0) DO
			write_log(_FUNCL_ + ": result should be 0.");
			RETURN FALSE;
		END
		RETURN TRUE;
	END

	local_array = {1.1, -1.2, -1.0, 0.1, -0.1, 0.3, 0.1} REAL;
	l_table.u16[0] = {1, 3, 1, 2, 4, 5} UINT16;
	g_table_array.ht[0] = {'2000-1-1', '2001-2-2', '2000-1-1', '-1000-1-1'} HIRESTIME;
	IF (tc == 1) DO
		IF (sort(local_array) != 7) DO
			IF (count(local_array) != 7) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (local_array[0] != -1.2) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (local_array[1] != -1.0) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (local_array[2] != -0.1) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (local_array[3] != 0.1) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (local_array[4] != 0.1) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (local_array[5] != 0.3) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (local_array[6] != 1.1) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			END
		END

		IF (sort(l_table.u16[0], FALSE) != 6) DO
			IF (count(l_table.u16[0]) != 7) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (l_table.u16[0][0] != 1) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (l_table.u16[0][1] != 1) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (l_table.u16[0][2] != 2) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (l_table.u16[0][3] != 3) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (l_table.u16[0][4] != 4) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (l_table.u16[0][5] != 5) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;
			END
		END

		IF (sort(g_table_array.ht[0], TRUE) != 4) DO
			IF (count(g_table_array.ht[0]) != 4) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (g_table_array.ht[0][0] != '2001-2-2') DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;
			ELSE IF (g_table_array.ht[0][1] != '2000-1-1') DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;
			ELSE IF (g_table_array.ht[0][2] != '2000-1-1') DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;
			ELSE IF (g_table_array.ht[0][3] != '-1000-1-1') DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;
			END
		END
		RETURN TRUE;
	ELSE IF (tc == 2) DO
		IF (sort(local_array, NULL, NULL) != 7) DO
			IF (count(local_array) != 7) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (local_array[6] != -1.2) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (local_array[5] != -1.0) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (local_array[4] != -0.1) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (local_array[3] != 0.1) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (local_array[2] != 0.1) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (local_array[1] != 0.3) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (local_array[0] != 1.1) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			END
		END

		IF (sort(l_table.u16[0], TRUE, TRUE) != 5) DO
			IF (count(l_table.u16[0]) != 5) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (l_table.u16[0][4] != 1) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (l_table.u16[0][3] != 2) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (l_table.u16[0][2] != 3) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (l_table.u16[0][1] != 4) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (l_table.u16[0][0] != 5) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;
			END
		END

		IF (sort(g_table_array.ht[0], FALSE, FALSE) != 4) DO
			IF (count(g_table_array.ht[0]) != 4) DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;	
			ELSE IF (g_table_array.ht[0][3] != '2001-2-2') DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;
			ELSE IF (g_table_array.ht[0][2] != '2000-1-1') DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;
			ELSE IF (g_table_array.ht[0][1] != '2000-1-1') DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;
			ELSE IF (g_table_array.ht[0][0] != '-1000-1-1') DO
				write_log(_FUNCL_ + ": unexpected array count after sort.");
				RETURN FALSE;
			END
		END
		RETURN TRUE;
	END
	RETURN NULL;
ENDPROC



PROCEDURE test_whais_api_get_min(tc UINT8) RETURN BOOL
DO
	VAR l_a_date ARRAY DATE;
	VAR l_a_undef ARRAY;

	IF (tc == 0) DO
		IF (get_min(NULL) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		ELSE IF (get_min(l_a_date) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		ELSE IF (get_min(glb_array_n) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		ELSE IF (get_min(l_a_undef) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		ELSE IF (get_min(g_table_array_n.ht[0]) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		END
		RETURN TRUE;
	END

	l_a_date = {'2016-1-21', '2017-04-03', '2017-01-02', '2011-02-03'};
	g_table_array.u16[0] = {3, 4, 0, 2, 1, 5, 1 } UINT16;

	IF (tc == 1) DO
		IF (get_min(l_a_date) != 3) DO
			write_log(_FUNCL_ + ": result should be 3 but I got: " + get_min(l_a_date));
			RETURN FALSE;
		ELSE IF (get_min(g_table_array.u16[0]) != 2) DO
			write_log(_FUNCL_ + ": result should be 2 but I got: " + get_min(g_table_array.u16[0]));
			RETURN FALSE;
		END

		l_a_undef = l_a_date;
		result = get_min(l_a_undef, '2012-1-1 10:39');
		IF (result != 0) DO
			write_log(_FUNCL_ + ": result should be 0 but I got: " + result);
			RETURN FALSE;
		END

		l_a_undef = g_table_array.u16[0];
		result = get_min(l_a_undef); 

		IF (result != 2) DO
			write_log(_FUNCL_ + ": result should be 2 but I got: " + get_min(g_table_array.u16[0]));
			RETURN FALSE;
		END

		RETURN TRUE;
	END


	IF (tc == 2) DO
		IF (get_min(l_a_date, NULL, count(l_a_date)) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
	        ELSE IF (get_min(g_table_array.u16[0], 2, 7) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		END

		result = get_min(g_table_array.u16[0], 2, 4);
		IF (result != 5) DO
			write_log(_FUNCL_ + ": result index should be 5 but I got: " + result);
			RETURN FALSE;
		END

		result = get_min(g_table_array.u16[0], 1);
		IF (result != 4) DO
			write_log(_FUNCL_ + ": result index should be 4 but I got: " + result);
			RETURN FALSE;
		END

		result = get_min(g_table_array.u16[0], 1, 5);
		IF (result != 6) DO
			write_log(_FUNCL_ + ": result index should be 6 but I got: " + result);
			RETURN FALSE;
		END

		g_table_array.u16[0] -= 1;
		result = get_min(g_table_array.u16[0], 1);
		IF (count(g_table_array.u16[0]) != 5) DO
			write_log(_FUNCL_ + ": the new array was supposed to have 5 elements but I got: " + count(g_table_array.u16[0]));
			RETURN FALSE;
		
		ELSE IF (result != 3) DO
			write_log(_FUNCL_ + ": result index should be 3 but I got: " + result);
			RETURN FALSE;
		END

		RETURN TRUE;
	END

	RETURN NULL;
ENDPROC


PROCEDURE test_whais_api_get_max(tc UINT8) RETURN BOOL
DO
	VAR l_a_date ARRAY DATE;
	VAR l_a_undef ARRAY;

	IF (tc == 0) DO
		IF (get_max(NULL) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		ELSE IF (get_max(l_a_date) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		ELSE IF (get_max(glb_array_n) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		ELSE IF (get_max(l_a_undef) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		ELSE IF (get_max(g_table_array_n.ht[0]) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		END
		RETURN TRUE;
	END

	l_a_date = {'2016-1-21', '2017-04-03', '2017-01-02', '2011-02-03'};
	g_table_array.u16[0] = {3, 5, 0, 2, 4, 5, 65535, 1 } UINT16;

	IF (tc == 1) DO
		IF (get_max(l_a_date) != 1) DO
			write_log(_FUNCL_ + ": result should be 1 but I got: " + get_max(l_a_date));
			RETURN FALSE;
		ELSE IF (get_max(g_table_array.u16[0]) != 6) DO
			write_log(_FUNCL_ + ": result should be 6 but I got: " + get_max(g_table_array.u16[0]));
			RETURN FALSE;
		END

		l_a_undef = l_a_date;
		result = get_max(l_a_undef, '2017-4-1 10:39:12.89');
		IF (result != 2) DO
			write_log(_FUNCL_ + ": result should be 2 but I got: " + result);
			RETURN FALSE;
		END

		l_a_undef = g_table_array.u16[0];
		result = get_max(l_a_undef); 

		IF (result != 6) DO
			write_log(_FUNCL_ + ": result should be 6 but I got: " + get_max(g_table_array.u16[0]));
			RETURN FALSE;
		END

		RETURN TRUE;
	END


	IF (tc == 2) DO
		IF (get_max(l_a_date, NULL, count(l_a_date)) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
	        ELSE IF (get_max(g_table_array.u16[0], 10000, 8) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		END

		result = get_max(g_table_array.u16[0], 10000, 4);
		IF (result != 5) DO
			write_log(_FUNCL_ + ": result index should be 5 but I got: " + result);
			RETURN FALSE;
		END

		result = get_max(g_table_array.u16[0], 5);
		IF (result != 1) DO
			write_log(_FUNCL_ + ": result index should be 1 but I got: " + result);
			RETURN FALSE;
		END

		result = get_max(g_table_array.u16[0], 5, 2);
		IF (result != 5) DO
			write_log(_FUNCL_ + ": result index should be 5 but I got: " + result);
			RETURN FALSE;
		END

		g_table_array.u16[0] -= 5;
		result = get_max(g_table_array.u16[0], 5);
		IF (count(g_table_array.u16[0]) != 6) DO
			write_log(_FUNCL_ + ": the new array was supposed to have 5 elements but I got: " + count(g_table_array.u16[0]));
			RETURN FALSE;
		
		ELSE IF (result != 3) DO
			write_log(_FUNCL_ + ": result index should be 3 but I got: " + result);
			RETURN FALSE;
		END

		RETURN TRUE;
	END

	RETURN NULL;
ENDPROC


PROCEDURE test_whais_api_truncate(tc UINT8) RETURN BOOL
DO
	VAR l_a_date ARRAY BOOL;
	VAR l_a_undef ARRAY;

	IF (tc == 0) DO
		IF (truncate(NULL) != 0) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		ELSE IF (truncate(l_a_date) != 0) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		ELSE IF (truncate(glb_array_n) != 0) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		ELSE IF (truncate(l_a_undef) != 0) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		ELSE IF (truncate(g_table_array_n.ht[0]) != 0) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		END
		RETURN TRUE;
	END

	l_a_date = {TRUE, FALSE, FALSE, TRUE};
	g_table_array.ht[0] = {'2013-11-10', '2012-10-11', '2012-12-1'} HIRESTIME;

	IF (tc == 1) DO
		IF (truncate(l_a_date, 200) != 4) DO 
			write_log(_FUNCL_ + ": result should be 4.");
			RETURN FALSE;
		ELSE IF (truncate(g_table_array.ht[0], 3) != 3) DO
			write_log(_FUNCL_ + ": result should be 4.");
			RETURN FALSE;
		END

		l_a_undef = g_table_array.ht[0];
		IF (truncate(l_a_undef, 2001) != 3) DO
			write_log(_FUNCL_ + ": result should be 3.");
			RETURN FALSE;
		END

		RETURN TRUE;
	ELSE IF (tc == 2) DO
		IF (truncate(l_a_date, 2) != 2) DO
			write_log(_FUNCL_ + ": result should be 2.");
			RETURN FALSE;
		ELSE IF (count(l_a_date) != 2) DO
			write_log(_FUNCL_ + ": result should be 2.");
			RETURN FALSE;
		END


		l_a_undef = g_table_array.ht[0];
		IF (count(l_a_undef) != 3) DO
			write_log(_FUNCL_ + ": result should be 3.");
			RETURN FALSE;

		ELSE IF (truncate(l_a_undef, 2) != 2) DO
			write_log(_FUNCL_ + ": result should be 2.");
			RETURN FALSE;

		ELSE IF (count(l_a_undef) != 2) DO
			write_log(_FUNCL_ + ": result should be 2.");
			RETURN FALSE;

		ELSE IF (count(g_table_array.ht[0]) != 3) DO
			write_log(_FUNCL_ + ": result should be 3.");
			RETURN FALSE;

		ELSE IF (truncate(g_table_array.ht[0], 1) != 1) DO
			write_log(_FUNCL_ + ": result should be 1.");
			RETURN FALSE;

		ELSE IF (count(g_table_array.ht[0]) != 1) DO
			write_log(_FUNCL_ + ": result should be 1.");
			RETURN FALSE;
		END

		RETURN TRUE;
	ELSE IF (tc == 3) DO
		IF (truncate(l_a_date) != 0) DO
			write_log(_FUNCL_ + ": result should be 0.");
			RETURN FALSE;
		ELSE IF (count(l_a_date) != 0) DO
			write_log(_FUNCL_ + ": result should be 0.");
			RETURN FALSE;
		END


		l_a_undef = g_table_array.ht[0];
		IF (count(l_a_undef) != 3) DO
			write_log(_FUNCL_ + ": result should be 3.");
			RETURN FALSE;
		ELSE IF (truncate(l_a_undef) != 0) DO
			write_log(_FUNCL_ + ": result should be 0.");
			RETURN FALSE;
		ELSE IF (count(l_a_undef) != 0) DO
			write_log(_FUNCL_ + ": result should be 0.");
			RETURN FALSE;
		ELSE IF (count(g_table_array.ht[0]) != 3) DO
			write_log(_FUNCL_ + ": result should be 3.");
			RETURN FALSE;
		ELSE IF (truncate(g_table_array.ht[0]) != 0) DO
			write_log(_FUNCL_ + ": result should be 0.");
			RETURN FALSE;
		ELSE IF (count(g_table_array.ht[0]) != 0) DO
			write_log(_FUNCL_ + ": result should be 0.");
			RETURN FALSE;
		END

		RETURN TRUE;
	END

	RETURN NULL;
ENDPROC

PROCEDURE test_whais_api_hash_array(tc UINT8) RETURN BOOL
DO
	VAR l_a_date ARRAY BOOL;
	VAR l_a_undef ARRAY;

	IF (tc == 0) DO
		IF (hash_array(NULL) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		ELSE IF (hash_array(l_a_date) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		ELSE IF (hash_array(glb_array_n) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		ELSE IF (hash_array(l_a_undef) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		ELSE IF (hash_array(g_table_array_n.ht[0]) != NULL) DO
			write_log(_FUNCL_ + ": result should be NULL.");
			RETURN FALSE;
		END
		RETURN TRUE;
	END

	l_a_date = {TRUE, FALSE, FALSE, TRUE};
	g_table_array.ht[0] = {'2013-11-10', '2012-10-11', '2012-12-1'} HIRESTIME;

	IF (tc == 1) DO
		hash1 = hash_array(l_a_date);
		IF (hash1 != 5314914667041155913) DO
			write_log(_FUNCL_ + ": result should be '5314914667041155913'  but is: " + hash1);
			RETURN FALSE;
		END

		l_a_date += FALSE;
		hash2 = hash_array(l_a_date);
		IF (hash2 != 6055991905435834999) DO
			write_log(_FUNCL_ + ": result should be '6055991905435834999'  but is: " + hash2);
			RETURN FALSE;
		END

		l_a_date[0] = NOT l_a_date[0];
		hash3 = hash_array(l_a_date);
		IF (hash3 != 4799112516819064005) DO
			write_log(_FUNCL_ + ": result should be '4799112516819064005'  but is: " + hash3);
			RETURN FALSE;
		END

		l_a_undef = l_a_date;
		IF (hash3 != hash_array(l_a_undef)) DO
			write_log(_FUNCL_ + ": These two hashes should be equal.");
			RETURN FALSE;
		END


		hash1 = hash_array(g_table_array.ht[0]);
		IF (hash1 != 10163460572765437508) DO
			write_log(_FUNCL_ + ": result should be '10163460572765437508'  but is: " + hash1);
			RETURN FALSE;
		END

		g_table_array.ht[0] += '2011-12-10';
		hash2 = hash_array(g_table_array.ht[0]);
		IF (hash2 != 2769608366229369836) DO
			write_log(_FUNCL_ + ": result should be '2769608366229369836'  but is: " + hash2);
			RETURN FALSE;
		END

		g_table_array.ht[0][0] = prev_htime(g_table_array.ht[0][0]);
		hash3 = hash_array(g_table_array.ht[0]);
		IF (hash3 != 4385893506082889302) DO
			write_log(_FUNCL_ + ": result should be '4385893506082889302'  but is: " + hash3);
			RETURN FALSE;
		END

		l_a_undef = g_table_array.ht[0];
		IF (hash3 != hash_array(l_a_undef)) DO
			write_log(_FUNCL_ + ": These two hashes should be equal.");
			RETURN FALSE;
		END

		RETURN TRUE;
	END

	RETURN NULL;
ENDPROC
