################################################################################
# Test module used to verify the correct handling of global values.            #
#                                                                              #                                                          #
################################################################################

#@include whais_std.wh

VAR test_global_value1 INT32;
VAR test_global_value2 INT64 ARRAY;
VAR test_global_value3 TEXT;
VAR test_global_value4 INT32 FIELD;
VAR test_global_value5 INT32 ARRAY FIELD;
VAR test_global_value6 TEXT FIELD;

VAR test_global_value7 TABLE (f1 INT32, 
                                    f2 INT32 ARRAY,
                                    f3 TEXT,
                                    f1_2 INT32,
                                    f2_2 INT32 ARRAY,
                                    f3_2 TEXT);

VAR test_global_values_inited BOOL;


PROCEDURE test_global_value_semantic_init () RETURN BOOL
DO
    write_log ("Initializing the globals values to test theirs semantics.");
    IF (test_global_values_inited == TRUE) DO
        write_log (" ... The initialization was already done!");
        RETURN TRUE;
    ELSE IF (test_global_values_inited == FALSE) DO
	write_log (" ... Restoring values!");
	test_global_value1 = NULL;
	test_global_value2 = NULL;
	test_global_value3 = NULL;
	test_global_value4 = NULL;
	test_global_value5 = NULL;
	test_global_value6 = NULL;
	test_global_value7 = NULL;

    END
    
    IF ((test_global_value1 != NULL)
	OR (test_global_value2 != NULL)
	OR (test_global_value3 != NULL)
	OR (test_global_value4 != NULL)
	OR (test_global_value5 != NULL)
	OR (test_global_value6 != NULL)
	OR (test_global_value7 != NULL)) DO
	
	write_log (" ... One or more value is not NULL as it was expected.!");
	RETURN FALSE;
    END

    test_global_value1 = 1;
    
    test_global_value2 += {-1, -2};

    test_global_value3 = "First text";
    
    test_global_value4 = test_global_value7.f1;
    test_global_value5 = test_global_value7.f2;
    test_global_value6 = test_global_value7.f3;
    
    test_global_value4[0] = 10;
    test_global_value5[0] += -10;
    test_global_value5[0] += -20;
    
    test_global_value6[0] = "First field text";

    write_log (" ... The initialization completed!");
    test_global_values_inited = TRUE;
    
    RETURN TRUE;
ENDPROC


PROCEDURE test_global_value_first_chage_values () RETURN BOOL
DO
    write_log ("... Modifying values first step!");

    test_global_value1 = 2;
    
    test_global_value2 = NULL;
    test_global_value2 += -3;
    test_global_value2 += -5;

    test_global_value3 = "Second text";
    
    test_global_value4[0] = NULL;
    test_global_value5[0] = NULL;
    test_global_value6[0] = NULL;
    
    test_global_value4 = test_global_value7.f1_2;
    test_global_value5 = test_global_value7.f2_2;
    test_global_value6 = test_global_value7.f3_2;


    test_global_value4[0] = -90;

    test_global_value4[0] = -13;
    test_global_value5[0] += {45544321, -67811121} INT32;
    
    test_global_value6[0] = "Second field text";

    write_log (" ... The modification have been done!");
    test_global_values_inited = FALSE;
    
    RETURN TRUE;
ENDPROC

PROCEDURE test_global_value_semantic () RETURN BOOL
DO
    write_log ("Starting the global values semantic test.");
    IF ( NOT test_global_values_inited) DO
        write_log (" ... The initialization of the global values was not done!");
        RETURN FALSE;
    END

    IF (test_global_value1 != 1) DO
        write_log ("First stage global 'test_global_value1' is not correct.");
        RETURN FALSE;
    END
    
    IF ((count (test_global_value2) != 2)
        OR (test_global_value2[0] != -1)
        OR (test_global_value2[1] != -2)) DO
        
        write_log ("First stage global 'test_global_value2' is not correct. ['" +
			count (test_global_value2) + "' '" +
			test_global_value2[0]	         + "' '" +
			test_global_value2[1]	         + "']");
        RETURN FALSE;
    END
    
    IF (test_global_value3 != "First text") DO
        write_log ("First stage global 'test_global_value1' is not correct.");
        RETURN FALSE;
    END
    
    IF ((test_global_value7.f1[0] != test_global_value4[0])
        OR (test_global_value7.f1[0] != 10)) 
    DO
        write_log ("First stage global 'test_global_value4' is not correct.");
        RETURN FALSE;
    END
    
    IF ((count (test_global_value7.f2[0]) != 2)
        OR (count (test_global_value5[0]) != 2)
        OR (test_global_value7.f2[0][0] != -10) OR (test_global_value7.f2[0][0] != test_global_value5[0][0])
        OR (test_global_value7.f2[0][1] != -20) OR (test_global_value7.f2[0][1] != test_global_value5[0][1])) 
    DO
        write_log ("First stage global 'test_global_value5' is not correct.");
        RETURN FALSE;
    END
    
    IF ((test_global_value7.f3[0] != test_global_value6[0])
        OR (test_global_value7.f3[0] != "First field text")) 
    DO
        write_log ("First stage global 'test_global_value6' is not correct.");
        RETURN FALSE;
    END


    IF ( NOT test_global_value_first_chage_values ())
        RETURN FALSE;


    IF (test_global_value1 != 2) DO
        write_log ("Second stage global 'test_global_value1' is not correct.");
        RETURN FALSE;
    END
    
    IF ((count (test_global_value2) != 2)
        OR (test_global_value2[0] != -3)
        OR (test_global_value2[1] != -5))  DO
        
        write_log ("Second stage global 'test_global_value2' is not correct.");
        RETURN FALSE;
    END
    
    IF (test_global_value3 != "Second text") DO
        write_log ("First stage global 'test_global_value1' is not correct.");
        RETURN FALSE;
    END
    
    
    IF ((test_global_value7.f1[0] != NULL)
        OR (test_global_value7.f2[0] != NULL)
        OR (test_global_value7.f2[0] != NULL))
    DO
        write_log ("Second stage global 'test_global_value7' is not correct");
        RETURN FALSE;
    END
    
    IF ((test_global_value7.f1_2[0] != test_global_value4[0])
        OR (test_global_value7.f1_2[0] != -13)) 
    DO
        write_log ("Second stage global 'test_global_value4' is not correct.");
        RETURN FALSE;
    END
    
    IF ((count (test_global_value7.f2_2[0]) != 2)
        OR (count (test_global_value5[0]) != 2)
        OR (test_global_value7.f2_2[0][0] != 45544321) OR (test_global_value7.f2_2[0][0] != test_global_value5[0][0])
        OR (test_global_value7.f2_2[0][1] != -67811121) OR (test_global_value7.f2_2[0][1] != test_global_value5[0][1])) 
     DO
        write_log ("Second stage global 'test_global_value5' is not correct.");
        RETURN FALSE;
    END
    
    IF ((test_global_value7.f3_2[0] != test_global_value6[0])
        OR (test_global_value7.f3_2[0] != "Second field text")) 
    DO
        write_log ("Second stage global 'test_global_value6' is not correct.");
        RETURN FALSE;
    END 

    write_log (" ... Test passed!");
    RETURN TRUE;
ENDPROC
