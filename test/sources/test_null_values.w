################################################################################
# Test module used to verify the correct handling of local values.             #
#                                                                              #
# All the test from this unit should be executed twice at least, and should    #
# return the same result always if not otherwise specified, to validated the   #
# test.                                                                        #
# Multiple execution is needed to prevent contamination of the procedure       #
# default null value.                                                          #
################################################################################

#@include whais_std.wh

#TEST: null_test_1
#Call this function twice in a row and make sure it returns the same non 
#null result.
PROCEDURE null_test_1 ()
RETURN TABLE
DO
    LET row AS UINT64;
    LET result AS TABLE OF (field1 AS BOOL,
                            field2 AS CHAR,
                            field3 AS DATE,
                            field4 AS DATETIME,
                            field5 AS HIRESTIME,
                            field6 AS INT8,
                            field7 AS INT16,
                            field8 AS INT32,
                            field9 AS INT64,
                            field10 AS UINT8,
                            field11 AS UINT16,
                            field12 AS UINT32,
                            field13 AS UINT64,
                            field14 AS REAL,
                            field15 AS RICHREAL,
                            field16 AS TEXT);

    write_log ("Start of null_test_1");
    IF (row != NULL) DO
        write_log ("... FAIL: 'row' is not null by default");
    END

    IF (result != NULL) DO
        write_log ("... FAIL: 'result' is not null by default.");
    END

    write_log ("... Local values are null by default.");

    row = table_reusable_row (result);
    result.field1[row] = TRUE;
    result.field2[row] = 'C';
    result.field3[row] = '1800/10/14';
    result.field4[row] = '-1/10/14 10:31:12';
    result.field5[row] = '1/10/14 10:31:12.111222';
    result.field6[row] = -8;
    result.field7[row] = -16;
    result.field8[row] = -32;
    result.field9[row] = -64;
    result.field10[row] = 8;
    result.field11[row] = 16;
    result.field12[row] = 32;
    result.field13[row] = 64;
    result.field14[row] = -1.1;
    result.field15[row] = 2.12;
    result.field16[row] = "This should be a text";

    write_log ("End of null_test_1");

    RETURN result;
ENDPROC

#TEST: null_test_1_1
#Call this function twice in a row and make sure it returns TRUE 
PROCEDURE null_test_1_1 ()
RETURN BOOL
DO
    LET row AS UINT64;
    LET result AS TABLE OF (field1 AS BOOL,
                            field2 AS CHAR,
                            field3 AS DATE,
                            field4 AS DATETIME,
                            field5 AS HIRESTIME,
                            field6 AS INT8,
                            field7 AS INT16,
                            field8 AS INT32,
                            field9 AS INT64,
                            field10 AS UINT8,
                            field11 AS UINT16,
                            field12 AS UINT32,
                            field13 AS UINT64,
                            field14 AS REAL,
                            field15 AS RICHREAL,
                            field16 AS TEXT);

    write_log ("Start of null_test_1_1");
    IF (row != NULL) DO
        write_log ("... FAIL: 'row' is not null by default");
        RETURN FALSE;
    END

    IF (result != NULL) DO
        write_log ("... FAIL: 'result' is not null by default.");
        RETURN FALSE;
    END

    write_log ("... Local values are null by default.");

    row = table_reusable_row (result);
    
    IF (result.field1[row] != NULL) DO
        write_log ("... FAIL: Field1 is not null");
        RETURN FALSE;
    END
    
    IF (result.field2[row] != NULL) DO
        write_log ("... FAIL: Field2 is not null");
        RETURN FALSE;
    END
    
    IF (result.field3[row] != NULL) DO
        write_log ("... FAIL: Field3 is not null");
        RETURN FALSE;
    END
    
    IF (result.field4[row] != NULL) DO
        write_log ("... FAIL: Field4 is not null");
        RETURN FALSE;
    END
    
    IF (result.field5[row] != NULL) DO
        write_log ("... FAIL: Field5 is not null");
        RETURN FALSE;
    END
    
    IF (result.field6[row] != NULL) DO
        write_log ("... FAIL: Field6 is not null");
        RETURN FALSE;
    END
    
    IF (result.field6[row] != NULL) DO
        write_log ("... FAIL: Field7 is not null");
        RETURN FALSE;
    END
    
    IF (result.field7[row] != NULL) DO
        write_log ("... FAIL: Field7 is not null");
        RETURN FALSE;
    END
       
    IF (result.field8[row] != NULL) DO
        write_log ("... FAIL: Field8 is not null");
        RETURN FALSE;
    END
    
    IF (result.field9[row] != NULL) DO
        write_log ("... FAIL: Field9 is not null");
        RETURN FALSE;
    END
    
    IF (result.field10[row] != NULL) DO
        write_log ("... FAIL: Field10 is not null");
        RETURN FALSE;
    END
    
    IF (result.field11[row] != NULL) DO
        write_log ("... FAIL: Field11 is not null");
        RETURN FALSE;
    END
    
    IF (result.field12[row] != NULL) DO
        write_log ("... FAIL: Field12 is not null");
        RETURN FALSE;
    END
    
    IF (result.field13[row] != NULL) DO
        write_log ("... FAIL: Field13 is not null");
        RETURN FALSE;
    END
    
    IF (result.field14[row] != NULL) DO
        write_log ("... FAIL: Field14 is not null");
        RETURN FALSE;
    END
    
    IF (result.field15[row] != NULL) DO
        write_log ("... FAIL: Field15 is not null");
        RETURN FALSE;
    END
    
    IF (result.field16[row] != NULL) DO
        write_log ("... FAIL: Field16 is not null");
        RETURN FALSE;
    END
    
    write_log ("... Step 2");

    result.field1[row] = TRUE;
    result.field2[row] = 'C';
    result.field3[row] = '1800/10/14';
    result.field4[row] = '-1/10/14 10:31:12';
    result.field5[row] = '1/10/14 10:31:12.111222';
    result.field6[row] = -8;
    result.field7[row] = -16;
    result.field8[row] = -32;
    result.field9[row] = -64;
    result.field10[row] = 8;
    result.field11[row] = 16;
    result.field12[row] = 32;
    result.field13[row] = 64;
    result.field14[row] = -1.1;
    result.field15[row] = 2.12;
    result.field16[row] = "This should be a text";
    
    IF (result.field1[row] == NULL) DO
        write_log ("... FAIL: Field1 is null");
        RETURN FALSE;
    END
    
    IF (result.field2[row] == NULL) DO
        write_log ("... FAIL: Field2 is null");
        RETURN FALSE;
    END
    
    IF (result.field3[row] == NULL) DO
        write_log ("... FAIL: Field3 is null");
        RETURN FALSE;
    END
    
    IF (result.field4[row] == NULL) DO
        write_log ("... FAIL: Field4 is null");
        RETURN FALSE;
    END
    
    IF (result.field5[row] == NULL) DO
        write_log ("... FAIL: Field5 is null");
        RETURN FALSE;
    END
    
    IF (result.field6[row] == NULL) DO
        write_log ("... FAIL: Field6 is null");
        RETURN FALSE;
    END
    
    IF (result.field6[row] == NULL) DO
        write_log ("... FAIL: Field7 is null");
        RETURN FALSE;
    END
    
    IF (result.field7[row] == NULL) DO
        write_log ("... FAIL: Field7 is null");
        RETURN FALSE;
    END
       
    IF (result.field8[row] == NULL) DO
        write_log ("... FAIL: Field8 is null");
        RETURN FALSE;
    END
    
    IF (result.field9[row] == NULL) DO
        write_log ("... FAIL: Field9 is null");
        RETURN FALSE;
    END
    
    IF (result.field10[row] == NULL) DO
        write_log ("... FAIL: Field10 is null");
        RETURN FALSE;
    END
    
    IF (result.field11[row] == NULL) DO
        write_log ("... FAIL: Field11 is null");
        RETURN FALSE;
    END
    
    IF (result.field12[row] == NULL) DO
        write_log ("... FAIL: Field12 is null");
        RETURN FALSE;
    END
    
    IF (result.field13[row] == NULL) DO
        write_log ("... FAIL: Field13 is null");
        RETURN FALSE;
    END
    
    IF (result.field14[row] == NULL) DO
        write_log ("... FAIL: Field14 is null");
        RETURN FALSE;
    END
    
    IF (result.field15[row] == NULL) DO
        write_log ("... FAIL: Field15 is null");
        RETURN FALSE;
    END
    
    IF (result.field16[row] == NULL) DO
        write_log ("... FAIL: Field16 is null");
        RETURN FALSE;
    END    
    
    write_log ("... Step 3");

    result.field1[row] = NULL;
    result.field2[row] = NULL;
    result.field3[row] = NULL;
    result.field4[row] = NULL;
    result.field5[row] = NULL;
    result.field6[row] = NULL;
    result.field7[row] = NULL;
    result.field8[row] = NULL;
    result.field9[row] = NULL;
    result.field10[row] = NULL;
    result.field11[row] = NULL;
    result.field12[row] = NULL;
    result.field13[row] = NULL;
    result.field14[row] = NULL;
    result.field15[row] = NULL;
    result.field16[row] = NULL;
    
    IF (result.field1[row] != NULL) DO
        write_log ("... FAIL: Field1 is not null");
        RETURN FALSE;
    END
    
    IF (result.field2[row] != NULL) DO
        write_log ("... FAIL: Field2 is not null");
        RETURN FALSE;
    END
    
    IF (result.field3[row] != NULL) DO
        write_log ("... FAIL: Field3 is not null");
        RETURN FALSE;
    END
    
    IF (result.field4[row] != NULL) DO
        write_log ("... FAIL: Field4 is not null");
        RETURN FALSE;
    END
    
    IF (result.field5[row] != NULL) DO
        write_log ("... FAIL: Field5 is not null");
        RETURN FALSE;
    END
    
    IF (result.field6[row] != NULL) DO
        write_log ("... FAIL: Field6 is not null");
        RETURN FALSE;
    END
    
    IF (result.field6[row] != NULL) DO
        write_log ("... FAIL: Field7 is not null");
        RETURN FALSE;
    END
    
    IF (result.field7[row] != NULL) DO
        write_log ("... FAIL: Field7 is not null");
        RETURN FALSE;
    END
       
    IF (result.field8[row] != NULL) DO
        write_log ("... FAIL: Field8 is not null");
        RETURN FALSE;
    END
    
    IF (result.field9[row] != NULL) DO
        write_log ("... FAIL: Field9 is not null");
        RETURN FALSE;
    END
    
    IF (result.field10[row] != NULL) DO
        write_log ("... FAIL: Field10 is not null");
        RETURN FALSE;
    END
    
    IF (result.field11[row] != NULL) DO
        write_log ("... FAIL: Field11 is not null");
        RETURN FALSE;
    END
    
    IF (result.field12[row] != NULL) DO
        write_log ("... FAIL: Field12 is not null");
        RETURN FALSE;
    END
    
    IF (result.field13[row] != NULL) DO
        write_log ("... FAIL: Field13 is not null");
        RETURN FALSE;
    END
    
    IF (result.field14[row] != NULL) DO
        write_log ("... FAIL: Field14 is not null");
        RETURN FALSE;
    END
    
    IF (result.field15[row] != NULL) DO
        write_log ("... FAIL: Field15 is not null");
        RETURN FALSE;
    END
    
    IF (result.field16[row] != NULL) DO
        write_log ("... FAIL: Field16 is not null");
        RETURN FALSE;
    END

    write_log ("Start of null_test_1_1");

    RETURN TRUE;
ENDPROC


#TEST: null_test_1_2
#Call this function twice in a row and make sure it returns the same non 
#null result.
PROCEDURE null_test_1_2 ()
RETURN TABLE
DO
    LET row AS UINT64;
    LET result AS TABLE OF (field1 AS ARRAY OF BOOL,
                            field2 AS ARRAY OF CHAR,
                            field3 AS ARRAY OF DATE,
                            field4 AS ARRAY OF DATETIME,
                            field5 AS ARRAY OF HIRESTIME,
                            field6 AS ARRAY OF INT8,
                            field7 AS ARRAY OF INT16,
                            field8 AS ARRAY OF INT32,
                            field9 AS ARRAY OF INT64,
                            field10 AS ARRAY OF UINT8,
                            field11 AS ARRAY OF UINT16,
                            field12 AS ARRAY OF UINT32,
                            field13 AS ARRAY OF UINT64,
                            field14 AS ARRAY OF REAL,
                            field15 AS ARRAY OF RICHREAL,
                            field16 AS TEXT);

    write_log ("Start of null_test_1_2");
    IF (row != NULL) DO
        write_log ("... FAIL: 'row' is not null by default");
    END

    IF (result != NULL) DO
        write_log ("... FAIL: 'result' is not null by default.");
    END

    write_log ("... Local values are null by default.");

    row = table_reusable_row (result);
    
    array_pushback(result.field1[row], TRUE);
    array_pushback(result.field1[row], FALSE);
    array_pushback(result.field2[row], 'C');
    array_pushback(result.field2[row], 'D');
    array_pushback(result.field3[row], '1800/10/14');
    array_pushback(result.field3[row], '1801/11/13');

    array_pushback(result.field4[row], '-1/10/14 10:31:12');
    array_pushback(result.field5[row], '1/10/14 10:31:12.111222');
    array_pushback(result.field6[row], -8);
    array_pushback(result.field7[row], -16);
    array_pushback(result.field8[row], -32);
    array_pushback(result.field9[row], -64);
    array_pushback(result.field10[row], 8);
    array_pushback(result.field11[row], 16);
    array_pushback(result.field12[row], 32);
    array_pushback(result.field13[row], 64);
    array_pushback(result.field14[row], -1.1);
    array_pushback(result.field15[row], 2.12);
    result.field16[row] = "Text field";
    
    write_log ("End of null_test_1_2");

    RETURN result;
ENDPROC


#TEST: null_test_1_3
#Call this function twice in a row and make sure it returns TRUE 
PROCEDURE null_test_1_3 ()
RETURN BOOL
DO
    LET result AS TABLE OF (field1 AS ARRAY OF BOOL,
                            field2 AS ARRAY OF CHAR,
                            field3 AS ARRAY OF DATE,
                            field4 AS ARRAY OF DATETIME,
                            field5 AS ARRAY OF HIRESTIME,
                            field6 AS ARRAY OF INT8,
                            field7 AS ARRAY OF INT16,
                            field8 AS ARRAY OF INT32,
                            field9 AS ARRAY OF INT64,
                            field10 AS ARRAY OF UINT8,
                            field11 AS ARRAY OF UINT16,
                            field12 AS ARRAY OF UINT32,
                            field13 AS ARRAY OF UINT64,
                            field14 AS ARRAY OF REAL,
                            field15 AS ARRAY OF RICHREAL,
                            field16 AS TEXT);
    LET row AS UINT64;
    
    write_log ("Start of null_test_1_3");
    IF (row != NULL) DO
        write_log ("... FAIL: 'row' is not null by default");
    END

    IF (result != NULL) DO
        write_log ("... FAIL: 'result' is not null by default.");
    END

    write_log ("... Local values are null by default.");
    
    row = table_reusable_row (result);
    
    IF (result.field1[row] != NULL) DO
        write_log ("... FAIL: Field1 is not null");
        RETURN FALSE;
    END
    
    IF (result.field2[row] != NULL) DO
        write_log ("... FAIL: Field2 is not null");
        RETURN FALSE;
    END
    
    IF (result.field3[row] != NULL) DO
        write_log ("... FAIL: Field3 is not null");
        RETURN FALSE;
    END
    
    IF (result.field4[row] != NULL) DO
        write_log ("... FAIL: Field4 is not null");
        RETURN FALSE;
    END
    
    IF (result.field5[row] != NULL) DO
        write_log ("... FAIL: Field5 is not null");
        RETURN FALSE;
    END
    
    IF (result.field6[row] != NULL) DO
        write_log ("... FAIL: Field6 is not null");
        RETURN FALSE;
    END
    
    IF (result.field6[row] != NULL) DO
        write_log ("... FAIL: Field7 is not null");
        RETURN FALSE;
    END
    
    IF (result.field7[row] != NULL) DO
        write_log ("... FAIL: Field7 is not null");
        RETURN FALSE;
    END
       
    IF (result.field8[row] != NULL) DO
        write_log ("... FAIL: Field8 is not null");
        RETURN FALSE;
    END
    
    IF (result.field9[row] != NULL) DO
        write_log ("... FAIL: Field9 is not null");
        RETURN FALSE;
    END
    
    IF (result.field10[row] != NULL) DO
        write_log ("... FAIL: Field10 is not null");
        RETURN FALSE;
    END
    
    IF (result.field11[row] != NULL) DO
        write_log ("... FAIL: Field11 is not null");
        RETURN FALSE;
    END
    
    IF (result.field12[row] != NULL) DO
        write_log ("... FAIL: Field12 is not null");
        RETURN FALSE;
    END
    
    IF (result.field13[row] != NULL) DO
        write_log ("... FAIL: Field13 is not null");
        RETURN FALSE;
    END
    
    IF (result.field14[row] != NULL) DO
        write_log ("... FAIL: Field14 is not null");
        RETURN FALSE;
    END
    
    IF (result.field15[row] != NULL) DO
        write_log ("... FAIL: Field15 is not null");
        RETURN FALSE;
    END
    
    IF (result.field16[row] != NULL) DO
        write_log ("... FAIL: Field16 is not null");
        RETURN FALSE;
    END
    
    write_log ("...Step 2");
    
    array_pushback(result.field1[row], TRUE);
    array_pushback(result.field1[row], FALSE);
    array_pushback(result.field2[row], 'C');
    array_pushback(result.field2[row], 'D');
    array_pushback(result.field3[row], '1800/10/14');
    array_pushback(result.field3[row], '1801/11/13');
    array_pushback(result.field4[row], '-1/10/14 10:31:12');
    array_pushback(result.field5[row], '1/10/14 10:31:12.111222');
    array_pushback(result.field6[row], -8);
    array_pushback(result.field7[row], -16);
    array_pushback(result.field8[row], -32);
    array_pushback(result.field9[row], -64);
    array_pushback(result.field10[row], 8);
    array_pushback(result.field11[row], 16);
    array_pushback(result.field12[row], 32);
    array_pushback(result.field13[row], 64);
    array_pushback(result.field14[row], -1.1);
    array_pushback(result.field15[row], 2.12);
    result.field16[row][0] = 'T';
    
    IF (result.field1[row] == NULL) DO
        write_log ("... FAIL: Field1 is null");
        RETURN FALSE;
    END
    
    IF (result.field2[row] == NULL) DO
        write_log ("... FAIL: Field2 is null");
        RETURN FALSE;
    END
    
    IF (result.field3[row] == NULL) DO
        write_log ("... FAIL: Field3 is null");
        RETURN FALSE;
    END
    
    IF (result.field4[row] == NULL) DO
        write_log ("... FAIL: Field4 is null");
        RETURN FALSE;
    END
    
    IF (result.field5[row] == NULL) DO
        write_log ("... FAIL: Field5 is null");
        RETURN FALSE;
    END
    
    IF (result.field6[row] == NULL) DO
        write_log ("... FAIL: Field6 is null");
        RETURN FALSE;
    END
    
    IF (result.field6[row] == NULL) DO
        write_log ("... FAIL: Field7 is null");
        RETURN FALSE;
    END
    
    IF (result.field7[row] == NULL) DO
        write_log ("... FAIL: Field7 is null");
        RETURN FALSE;
    END
       
    IF (result.field8[row] == NULL) DO
        write_log ("... FAIL: Field8 is null");
        RETURN FALSE;
    END
    
    IF (result.field9[row] == NULL) DO
        write_log ("... FAIL: Field9 is null");
        RETURN FALSE;
    END
    
    IF (result.field10[row] == NULL) DO
        write_log ("... FAIL: Field10 is null");
        RETURN FALSE;
    END
    
    IF (result.field11[row] == NULL) DO
        write_log ("... FAIL: Field11 is null");
        RETURN FALSE;
    END
    
    IF (result.field12[row] == NULL) DO
        write_log ("... FAIL: Field12 is null");
        RETURN FALSE;
    END
    
    IF (result.field13[row] == NULL) DO
        write_log ("... FAIL: Field13 is null");
        RETURN FALSE;
    END
    
    IF (result.field14[row] == NULL) DO
        write_log ("... FAIL: Field14 is null");
        RETURN FALSE;
    END
    
    IF (result.field15[row] == NULL) DO
        write_log ("... FAIL: Field15 is null");
        RETURN FALSE;
    END
    
    IF (result.field16[row] == NULL) DO
        write_log ("... FAIL: Field16 is null");
        RETURN FALSE;
    END 

    write_log ("...Step 3");

    result.field1[row] = NULL;
    result.field2[row] = NULL;
    result.field3[row] = NULL;
    result.field4[row] = NULL;
    result.field5[row] = NULL;
    result.field6[row] = NULL;
    result.field7[row] = NULL;
    result.field8[row] = NULL;
    result.field9[row] = NULL;
    result.field10[row] = NULL;
    result.field11[row] = NULL;
    result.field12[row] = NULL;
    result.field13[row] = NULL;
    result.field14[row] = NULL;
    result.field15[row] = NULL;
    result.field16[row] = NULL;
    
    IF (result.field1[row] != NULL) DO
        write_log ("... FAIL: Field1 is not null");
        RETURN FALSE;
    END
    
    IF (result.field2[row] != NULL) DO
        write_log ("... FAIL: Field2 is not null");
        RETURN FALSE;
    END
    
    IF (result.field3[row] != NULL) DO
        write_log ("... FAIL: Field3 is not null");
        RETURN FALSE;
    END
    
    IF (result.field4[row] != NULL) DO
        write_log ("... FAIL: Field4 is not null");
        RETURN FALSE;
    END
    
    IF (result.field5[row] != NULL) DO
        write_log ("... FAIL: Field5 is not null");
        RETURN FALSE;
    END
    
    IF (result.field6[row] != NULL) DO
        write_log ("... FAIL: Field6 is not null");
        RETURN FALSE;
    END
    
    IF (result.field6[row] != NULL) DO
        write_log ("... FAIL: Field7 is not null");
        RETURN FALSE;
    END
    
    IF (result.field7[row] != NULL) DO
        write_log ("... FAIL: Field7 is not null");
        RETURN FALSE;
    END
       
    IF (result.field8[row] != NULL) DO
        write_log ("... FAIL: Field8 is not null");
        RETURN FALSE;
    END
    
    IF (result.field9[row] != NULL) DO
        write_log ("... FAIL: Field9 is not null");
        RETURN FALSE;
    END
    
    IF (result.field10[row] != NULL) DO
        write_log ("... FAIL: Field10 is not null");
        RETURN FALSE;
    END
    
    IF (result.field11[row] != NULL) DO
        write_log ("... FAIL: Field11 is not null");
        RETURN FALSE;
    END
    
    IF (result.field12[row] != NULL) DO
        write_log ("... FAIL: Field12 is not null");
        RETURN FALSE;
    END
    
    IF (result.field13[row] != NULL) DO
        write_log ("... FAIL: Field13 is not null");
        RETURN FALSE;
    END
    
    IF (result.field14[row] != NULL) DO
        write_log ("... FAIL: Field14 is not null");
        RETURN FALSE;
    END
    
    IF (result.field15[row] != NULL) DO
        write_log ("... FAIL: Field15 is not null");
        RETURN FALSE;
    END
    
    IF (result.field16[row] != NULL) DO
        write_log ("... FAIL: Field16 is not null");
        RETURN FALSE;
    END
    
    write_log ("...Step 4");

    array_pushback(result.field1[row], TRUE);
    array_pushback(result.field2[row], 'C');
    array_pushback(result.field3[row], '1800/10/14');
    array_pushback(result.field4[row], '-1/10/14 10:31:12');
    array_pushback(result.field5[row], '1/10/14 10:31:12.111222');
    array_pushback(result.field6[row], -8);
    array_pushback(result.field7[row], -16);
    array_pushback(result.field8[row], -32);
    array_pushback(result.field9[row], -64);
    array_pushback(result.field10[row], 8);
    array_pushback(result.field11[row], 16);
    array_pushback(result.field12[row], 32);
    array_pushback(result.field13[row], 64);
    array_pushback(result.field14[row], -1.1);
    array_pushback(result.field15[row], 2.12);
    
    result.field1[row][0] = TRUE;
    result.field2[row][0] = 'C';
    result.field3[row][0] = '1800/10/14';
    result.field4[row][0] = '-1/10/14 10:31:12';
    result.field5[row][0] = '1/10/14 10:31:12.111222';
    result.field6[row][0] = -8;
    result.field7[row][0] = -16;
    result.field8[row][0] = -32;
    result.field9[row][0] = -64;
    result.field10[row][0] = 8;
    result.field11[row][0] = 16;
    result.field12[row][0] = 32;
    result.field13[row][0] = 64;
    result.field14[row][0] = -1.1;
    result.field15[row][0] = 2.12;
    result.field16[row][0] = 'r';
    
    IF (result.field1[row][0] == NULL) DO
        write_log ("... FAIL: Field1 array is null");
        RETURN FALSE;
    END
    
    IF (result.field2[row][0] == NULL) DO
        write_log ("... FAIL: Field2 array is null");
        RETURN FALSE;
    END
    
    IF (result.field3[row][0] == NULL) DO
        write_log ("... FAIL: Field3 array is null");
        RETURN FALSE;
    END
    
    IF (result.field4[row][0] == NULL) DO
        write_log ("... FAIL: Field4 array is null");
        RETURN FALSE;
    END
    
    IF (result.field5[row][0] == NULL) DO
        write_log ("... FAIL: Field5 array is null");
        RETURN FALSE;
    END
    
    IF (result.field6[row][0] == NULL) DO
        write_log ("... FAIL: Field6 array is null");
        RETURN FALSE;
    END
    
    IF (result.field6[row][0] == NULL) DO
        write_log ("... FAIL: Field7 array is null");
        RETURN FALSE;
    END
    
    IF (result.field7[row][0] == NULL) DO
        write_log ("... FAIL: Field7 array is null");
        RETURN FALSE;
    END
       
    IF (result.field8[row][0] == NULL) DO
        write_log ("... FAIL: Field8 array is null");
        RETURN FALSE;
    END
    
    IF (result.field9[row][0] == NULL) DO
        write_log ("... FAIL: Field9 array is null");
        RETURN FALSE;
    END
    
    IF (result.field10[row][0] == NULL) DO
        write_log ("... FAIL: Field10 array is null");
        RETURN FALSE;
    END
    
    IF (result.field11[row][0] == NULL) DO
        write_log ("... FAIL: Field11 array is null");
        RETURN FALSE;
    END
    
    IF (result.field12[row][0] == NULL) DO
        write_log ("... FAIL: Field12 array is null");
        RETURN FALSE;
    END
    
    IF (result.field13[row][0] == NULL) DO
        write_log ("... FAIL: Field13 array is null");
        RETURN FALSE;
    END
    
    IF (result.field14[row][0] == NULL) DO
        write_log ("... FAIL: Field14 array is null");
        RETURN FALSE;
    END
    
    IF (result.field15[row][0] == NULL) DO
        write_log ("... FAIL: Field15 array is null");
        RETURN FALSE;
    END
    
    IF (result.field16[row][0] == NULL) DO
        write_log ("... FAIL: Field16 array is null");
        RETURN FALSE;
    END
    
    write_log ("...Step 5");

    result.field1[row][0] = NULL;
    result.field2[row][0] = NULL;
    result.field3[row][0] = NULL;
    result.field4[row][0] = NULL;
    result.field5[row][0] = NULL;
    result.field6[row][0] = NULL;
    result.field7[row][0] = NULL;
    result.field8[row][0] = NULL;
    result.field9[row][0] = NULL;
    result.field10[row][0] = NULL;
    result.field11[row][0] = NULL;
    result.field12[row][0] = NULL;
    result.field13[row][0] = NULL;
    result.field14[row][0] = NULL;
    result.field15[row][0] = NULL;
    result.field16[row][0] = NULL;
    
    IF (result.field1[row] != NULL) DO
        write_log ("... FAIL: Field1 array is not null");
        RETURN FALSE;
    END
    
    IF (result.field2[row] != NULL) DO
        write_log ("... FAIL: Field2 array is not null");
        RETURN FALSE;
    END
    
    IF (result.field3[row] != NULL) DO
        write_log ("... FAIL: Field3 array is not null");
        RETURN FALSE;
    END
    
    IF (result.field4[row] != NULL) DO
        write_log ("... FAIL: Field4 array is not null");
        RETURN FALSE;
    END
    
    IF (result.field5[row] != NULL) DO
        write_log ("... FAIL: Field5 array is not null");
        RETURN FALSE;
    END
    
    IF (result.field6[row] != NULL) DO
        write_log ("... FAIL: Field6 array is not null");
        RETURN FALSE;
    END
    
    IF (result.field6[row] != NULL) DO
        write_log ("... FAIL: Field7 array is not null");
        RETURN FALSE;
    END
    
    IF (result.field7[row] != NULL) DO
        write_log ("... FAIL: Field7 array is not null");
        RETURN FALSE;
    END
       
    IF (result.field8[row] != NULL) DO
        write_log ("... FAIL: Field8 array is not null");
        RETURN FALSE;
    END
    
    IF (result.field9[row] != NULL) DO
        write_log ("... FAIL: Field9 array is not null");
        RETURN FALSE;
    END
    
    IF (result.field10[row] != NULL) DO
        write_log ("... FAIL: Field10 array is not null");
        RETURN FALSE;
    END
    
    IF (result.field11[row] != NULL) DO
        write_log ("... FAIL: Field11 array is not null");
        RETURN FALSE;
    END
    
    IF (result.field12[row] != NULL) DO
        write_log ("... FAIL: Field12 array is not null");
        RETURN FALSE;
    END
    
    IF (result.field13[row] != NULL) DO
        write_log ("... FAIL: Field13 array is not null");
        RETURN FALSE;
    END

    IF (result.field14[row] != NULL) DO
        write_log ("... FAIL: Field14 array is not null");
        RETURN FALSE;
    END

    IF (result.field15[row] != NULL) DO
        write_log ("... FAIL: Field15 array is not null");
        RETURN FALSE;
    END

    IF (result.field16[row] != NULL) DO
        write_log ("... FAIL: Field16 array is not null");
        RETURN FALSE;
    END

    write_log ("End of null_test_1_3");

    RETURN TRUE;
ENDPROC


#TEST: null_test_2
#Call this function twice in and make sure it returns the same result.
PROCEDURE null_test_2 ()
RETURN ARRAY
DO
    LET result AS ARRAY OF INT32;
    
    write_log ("Start of null_test_2");
    IF (result != NULL) DO
        write_log ("... FAIL: 'result' is not null by default.");
    END

    write_log ("... Local values are null by default.");

    array_pushback (result, -1000);
    array_pushback (result, 20);

    write_log ("End of null_test_2");

    RETURN result;
ENDPROC



#TEST: null_test_2_1
#Call this function and it should return TRUE for the test to pass.
PROCEDURE null_test_2_1 ()
RETURN BOOL
DO

    LET array1 AS ARRAY OF BOOL;
    LET array2 AS ARRAY OF CHAR;
    LET array3 AS ARRAY OF DATE;
    LET array4 AS ARRAY OF DATETIME;
    LET array5 AS ARRAY OF HIRESTIME;
    LET array6 AS ARRAY OF INT8;
    LET array7 AS ARRAY OF INT16;
    LET array8 AS ARRAY OF INT32;
    LET array9 AS ARRAY OF INT64;
    LET array10 AS ARRAY OF UINT8;
    LET array11 AS ARRAY OF UINT16;
    LET array12 AS ARRAY OF UINT32;
    LET array13 AS ARRAY OF UINT64;
    LET array14 AS ARRAY OF REAL;
    LET array15 AS ARRAY OF RICHREAL;
    LET array16 AS TEXT;

    write_log ("Start of null_test_2_1");
        
    IF (array1 != NULL) DO
        write_log ("... FAIL: Array1 is not null");
        RETURN FALSE;
    END
    
    IF (array2 != NULL) DO
        write_log ("... FAIL: Array2 is not null");
        RETURN FALSE;
    END
    
    IF (array3 != NULL) DO
        write_log ("... FAIL: Array3 is not null");
        RETURN FALSE;
    END
    
    IF (array4 != NULL) DO
        write_log ("... FAIL: Array4 is not null");
        RETURN FALSE;
    END
    
    IF (array5 != NULL) DO
        write_log ("... FAIL: Array5 is not null");
        RETURN FALSE;
    END
    
    IF (array6 != NULL) DO
        write_log ("... FAIL: Array6 is not null");
        RETURN FALSE;
    END
    
    IF (array6 != NULL) DO
        write_log ("... FAIL: Array7 is not null");
        RETURN FALSE;
    END
    
    IF (array7 != NULL) DO
        write_log ("... FAIL: Array7 is not null");
        RETURN FALSE;
    END
       
    IF (array8 != NULL) DO
        write_log ("... FAIL: Array8 is not null");
        RETURN FALSE;
    END
    
    IF (array9 != NULL) DO
        write_log ("... FAIL: Array9 is not null");
        RETURN FALSE;
    END
    
    IF (array10 != NULL) DO
        write_log ("... FAIL: Array10 is not null");
        RETURN FALSE;
    END
    
    IF (array11 != NULL) DO
        write_log ("... FAIL: Array11 is not null");
        RETURN FALSE;
    END
    
    IF (array12 != NULL) DO
        write_log ("... FAIL: Array12 is not null");
        RETURN FALSE;
    END
    
    IF (array13 != NULL) DO
        write_log ("... FAIL: Array13 is not null");
        RETURN FALSE;
    END
    
    IF (array14 != NULL) DO
        write_log ("... FAIL: Array14 is not null");
        RETURN FALSE;
    END
    
    IF (array15 != NULL) DO
        write_log ("... FAIL: Array15 is not null");
        RETURN FALSE;
    END
    
    IF (array16 != NULL) DO
        write_log ("... FAIL: Array16 is not null");
        RETURN FALSE;
    END

    write_log ("... Step 2");
    
    array_pushback(array1, TRUE);
    array_pushback(array1, FALSE);
    array_pushback(array2, 'C');
    array_pushback(array2, 'D');
    array_pushback(array3, '1800/10/14');
    array_pushback(array3, '1801/11/13');
    array_pushback(array4, '-1/10/14 10:31:12');
    array_pushback(array5, '1/10/14 10:31:12.111222');
    array_pushback(array6, -8);
    array_pushback(array7, -16);
    array_pushback(array8, -32);
    array_pushback(array9, -64);
    array_pushback(array10, 8);
    array_pushback(array11, 16);
    array_pushback(array12, 32);
    array_pushback(array13, 64);
    array_pushback(array14, -1.1);
    array_pushback(array15, 2.12);
    array16[0] = 'T';
    
    IF (array1 == NULL) DO
        write_log ("... FAIL: Array1 is null");
        RETURN FALSE;
    END
    
    IF (array2 == NULL) DO
        write_log ("... FAIL: Array2 is null");
        RETURN FALSE;
    END
    
    IF (array3 == NULL) DO
        write_log ("... FAIL: Array3 is null");
        RETURN FALSE;
    END
    
    IF (array4 == NULL) DO
        write_log ("... FAIL: Array4 is null");
        RETURN FALSE;
    END
    
    IF (array5 == NULL) DO
        write_log ("... FAIL: Array5 is null");
        RETURN FALSE;
    END
    
    IF (array6 == NULL) DO
        write_log ("... FAIL: Array6 is null");
        RETURN FALSE;
    END
    
    IF (array6 == NULL) DO
        write_log ("... FAIL: Array7 is null");
        RETURN FALSE;
    END
    
    IF (array7 == NULL) DO
        write_log ("... FAIL: Array7 is null");
        RETURN FALSE;
    END
       
    IF (array8 == NULL) DO
        write_log ("... FAIL: Array8 is null");
        RETURN FALSE;
    END
    
    IF (array9 == NULL) DO
        write_log ("... FAIL: Array9 is null");
        RETURN FALSE;
    END
    
    IF (array10 == NULL) DO
        write_log ("... FAIL: Array10 is null");
        RETURN FALSE;
    END
    
    IF (array11 == NULL) DO
        write_log ("... FAIL: Array11 is null");
        RETURN FALSE;
    END
    
    IF (array12 == NULL) DO
        write_log ("... FAIL: Array12 is null");
        RETURN FALSE;
    END
    
    IF (array13 == NULL) DO
        write_log ("... FAIL: Array13 is null");
        RETURN FALSE;
    END
    
    IF (array14 == NULL) DO
        write_log ("... FAIL: Array14 is null");
        RETURN FALSE;
    END
    
    IF (array15 == NULL) DO
        write_log ("... FAIL: Array15 is null");
        RETURN FALSE;
    END
    
    IF (array16 == NULL) DO
        write_log ("... FAIL: Array16 is null");
        RETURN FALSE;
    END
    
    write_log ("... Step 3");

    array1 = NULL;
    array2 = NULL;
    array3 = NULL;
    array4 = NULL;
    array5 = NULL;
    array6 = NULL;
    array7 = NULL;
    array8 = NULL;
    array9 = NULL;
    array10 = NULL;
    array11 = NULL;
    array12 = NULL;
    array13 = NULL;
    array14 = NULL;
    array15 = NULL;
    array16 = NULL;
    
    IF (array1 != NULL) DO
        write_log ("... FAIL: Array1 is not null");
        RETURN FALSE;
    END
    
    IF (array2 != NULL) DO
        write_log ("... FAIL: Array2 is not null");
        RETURN FALSE;
    END
    
    IF (array3 != NULL) DO
        write_log ("... FAIL: Array3 is not null");
        RETURN FALSE;
    END
    
    IF (array4 != NULL) DO
        write_log ("... FAIL: Array4 is not null");
        RETURN FALSE;
    END
    
    IF (array5 != NULL) DO
        write_log ("... FAIL: Array5 is not null");
        RETURN FALSE;
    END
    
    IF (array6 != NULL) DO
        write_log ("... FAIL: Array6 is not null");
        RETURN FALSE;
    END
    
    IF (array6 != NULL) DO
        write_log ("... FAIL: Array7 is not null");
        RETURN FALSE;
    END
    
    IF (array7 != NULL) DO
        write_log ("... FAIL: Array7 is not null");
        RETURN FALSE;
    END
       
    IF (array8 != NULL) DO
        write_log ("... FAIL: Array8 is not null");
        RETURN FALSE;
    END
    
    IF (array9 != NULL) DO
        write_log ("... FAIL: Array9 is not null");
        RETURN FALSE;
    END
    
    IF (array10 != NULL) DO
        write_log ("... FAIL: Array10 is not null");
        RETURN FALSE;
    END
    
    IF (array11 != NULL) DO
        write_log ("... FAIL: Array11 is not null");
        RETURN FALSE;
    END
    
    IF (array12 != NULL) DO
        write_log ("... FAIL: Array12 is not null");
        RETURN FALSE;
    END
    
    IF (array13 != NULL) DO
        write_log ("... FAIL: Array13 is not null");
        RETURN FALSE;
    END
    
    IF (array14 != NULL) DO
        write_log ("... FAIL: Array14 is not null");
        RETURN FALSE;
    END
    
    IF (array15 != NULL) DO
        write_log ("... FAIL: Array15 is not null");
        RETURN FALSE;
    END
    
    IF (array16 != NULL) DO
        write_log ("... FAIL: Array16 is not null");
        RETURN FALSE;
    END
    
    write_log ("... Step 4");
    
    array_pushback(array1, TRUE);
    array_pushback(array2, 'C');
    array_pushback(array3, '1800/10/14');
    array_pushback(array4, '-1/10/14 10:31:12');
    array_pushback(array5, '1/10/14 10:31:12.111222');
    array_pushback(array6, -8);
    array_pushback(array7, -16);
    array_pushback(array8, -32);
    array_pushback(array9, -64);
    array_pushback(array10, 8);
    array_pushback(array11, 16);
    array_pushback(array12, 32);
    array_pushback(array13, 64);
    array_pushback(array14, -1.1);
    array_pushback(array15, 2.12);
    array16[0] = 'T';
    
    array1[0] = TRUE;
    array2[0] = 'C';
    array3[0] = '1800/10/14';
    array4[0] = '-1/10/14 10:31:12';
    array5[0] = '1/10/14 10:31:12.111222';
    array6[0] = -8;
    array7[0] = -16;
    array8[0] = -32;
    array9[0] = -64;
    array10[0] = 8;
    array11[0] = 16;
    array12[0] = 32;
    array13[0] = 64;
    array14[0] = -1.1;
    array15[0] = 2.12;
    array16[0] = 'r';
    
        IF (array1 == NULL) DO
        write_log ("... FAIL: Array1 is null");
        RETURN FALSE;
    END
    
    IF (array2 == NULL) DO
        write_log ("... FAIL: Array2 is null");
        RETURN FALSE;
    END
    
    IF (array3 == NULL) DO
        write_log ("... FAIL: Array3 is null");
        RETURN FALSE;
    END
    
    IF (array4 == NULL) DO
        write_log ("... FAIL: Array4 is null");
        RETURN FALSE;
    END
    
    IF (array5 == NULL) DO
        write_log ("... FAIL: Array5 is null");
        RETURN FALSE;
    END
    
    IF (array6 == NULL) DO
        write_log ("... FAIL: Array6 is null");
        RETURN FALSE;
    END
    
    IF (array6 == NULL) DO
        write_log ("... FAIL: Array7 is null");
        RETURN FALSE;
    END
    
    IF (array7 == NULL) DO
        write_log ("... FAIL: Array7 is null");
        RETURN FALSE;
    END
       
    IF (array8 == NULL) DO
        write_log ("... FAIL: Array8 is null");
        RETURN FALSE;
    END
    
    IF (array9 == NULL) DO
        write_log ("... FAIL: Array9 is null");
        RETURN FALSE;
    END
    
    IF (array10 == NULL) DO
        write_log ("... FAIL: Array10 is null");
        RETURN FALSE;
    END
    
    IF (array11 == NULL) DO
        write_log ("... FAIL: Array11 is null");
        RETURN FALSE;
    END
    
    IF (array12 == NULL) DO
        write_log ("... FAIL: Array12 is null");
        RETURN FALSE;
    END
    
    IF (array13 == NULL) DO
        write_log ("... FAIL: Array13 is null");
        RETURN FALSE;
    END
    
    IF (array14 == NULL) DO
        write_log ("... FAIL: Array14 is null");
        RETURN FALSE;
    END
    
    IF (array15 == NULL) DO
        write_log ("... FAIL: Array15 is null");
        RETURN FALSE;
    END
    
    IF (array16 == NULL) DO
        write_log ("... FAIL: Array16 is null");
        RETURN FALSE;
    END
    
    write_log ("... Step 5");

    array1[0] = NULL;
    array2[0] = NULL;
    array3[0] = NULL;
    array4[0] = NULL;
    array5[0] = NULL;
    array6[0] = NULL;
    array7[0] = NULL;
    array8[0] = NULL;
    array9[0] = NULL;
    array10[0] = NULL;
    array11[0] = NULL;
    array12[0] = NULL;
    array13[0] = NULL;
    array14[0] = NULL;
    array15[0] = NULL;
    array16[0] = NULL;
    
    
    IF (array1 != NULL) DO
        write_log ("... FAIL: Array1 is not null");
        RETURN FALSE;
    END
    
    IF (array2 != NULL) DO
        write_log ("... FAIL: Array2 is not null");
        RETURN FALSE;
    END
    
    IF (array3 != NULL) DO
        write_log ("... FAIL: Array3 is not null");
        RETURN FALSE;
    END
    
    IF (array4 != NULL) DO
        write_log ("... FAIL: Array4 is not null");
        RETURN FALSE;
    END
    
    IF (array5 != NULL) DO
        write_log ("... FAIL: Array5 is not null");
        RETURN FALSE;
    END
    
    IF (array6 != NULL) DO
        write_log ("... FAIL: Array6 is not null");
        RETURN FALSE;
    END
    
    IF (array6 != NULL) DO
        write_log ("... FAIL: Array7 is not null");
        RETURN FALSE;
    END
    
    IF (array7 != NULL) DO
        write_log ("... FAIL: Array7 is not null");
        RETURN FALSE;
    END
       
    IF (array8 != NULL) DO
        write_log ("... FAIL: Array8 is not null");
        RETURN FALSE;
    END
    
    IF (array9 != NULL) DO
        write_log ("... FAIL: Array9 is not null");
        RETURN FALSE;
    END
    
    IF (array10 != NULL) DO
        write_log ("... FAIL: Array10 is not null");
        RETURN FALSE;
    END
    
    IF (array11 != NULL) DO
        write_log ("... FAIL: Array11 is not null");
        RETURN FALSE;
    END
    
    IF (array12 != NULL) DO
        write_log ("... FAIL: Array12 is not null");
        RETURN FALSE;
    END
    
    IF (array13 != NULL) DO
        write_log ("... FAIL: Array13 is not null");
        RETURN FALSE;
    END
    
    IF (array14 != NULL) DO
        write_log ("... FAIL: Array14 is not null");
        RETURN FALSE;
    END
    
    IF (array15 != NULL) DO
        write_log ("... FAIL: Array15 is not null");
        RETURN FALSE;
    END
    
    IF (array16 != NULL) DO
        write_log ("... FAIL: Array16 is not null");
        RETURN FALSE;
    END

    write_log ("End of null_test_2_1");
    
    RETURN TRUE;

ENDPROC


#TEST: null_test_2_2
#Call this function twice in and make sure it returns the same result.
PROCEDURE null_test_2_2 ()
RETURN ARRAY
DO
    LET result AS ARRAY OF INT8;

    write_log ("Start of null_test_2_2");
    IF (result != NULL) DO
        write_log ("... FAIL: 'result' is not null by default.");
    END
    
    IF (result[0] != NULL) DO
        write_log ("... FAIL: 'result[0]' is not null.");
    END

    result[0] = -10;

    IF (result == NULL) DO
        write_log ("... FAIL: 'result' is null.");
    END
    
    IF (result[0] != NULL) DO
        write_log ("... FAIL: 'result[0]' is null.");
    END

    IF (result[1] != NULL) DO
        write_log ("... FAIL: 'result[1]' is not null.");
    END
    
    IF (result[2] != NULL) DO
        write_log ("... FAIL: 'result[2]' is not null.");
    END

    write_log ("End of null_test_2");

    RETURN result;
ENDPROC


#TEST: null_test_2_3
#Call this function twice in and make sure it returns the same result.
PROCEDURE null_test_2_3 ()
RETURN BOOL
DO
    LET tab AS TABLE OF (field1 AS ARRAY OF DATE);

    write_log ("Started test null_test_2_3");

    IF (tab != NULL) DO
        write_log ("... FAIL: Table is not null");
        RETURN FALSE;
    END

    IF (tab.field1[0] != NULL) DO
        write_log ("... FAIL: The array field is not null.");
        RETURN FALSE;    
    END

    IF (tab.field1[0][0] != NULL) DO
        write_log ("... FAIL: The first element of field is not null.");
        RETURN FALSE;
    END

    tab.field1[0][0] = '1/1/1';

    IF (tab == NULL) DO
        write_log ("... FAIL: The table is null.");
        RETURN FALSE;
    END

    IF (tab.field1[0] == NULL) DO
        write_log ("... FAIL: The array field is null.");
        RETURN FALSE;
    END

    IF (tab.field1[0][0] == NULL) DO
        write_log ("... FAIL: The first element of array field is null");
        RETURN FALSE;
    END
    
    IF (tab.field1[0][1] != NULL) DO
        write_log ("... FAIL: The second element of array field is not null.");
        RETURN FALSE;
    END
    
    IF (tab.field1[0][2] != NULL) DO
        write_log ("... FAIL: The third element of array field is not null.");
        RETURN FALSE;
    END

    write_log ("End of null_test_2_3");

    RETURN TRUE;
ENDPROC



#TEST: null_test_3
#Call this function twice and make sure it returns TRUE in both cases.
PROCEDURE null_test_3 () RETURN BOOL
DO
    LET b AS BOOL;
    LET c AS CHAR;
    LET d AS DATE;
    LET dt AS DATETIME;
    LET ht AS HIRESTIME;
    LET i8 AS INT8;
    LET i16 AS INT16;
    LET i32 AS INT32;
    LET i64 AS INT64;
    LET u8 AS  UINT8;
    LET u16 AS UINT16;
    LET u32 AS UINT32;
    LET u64 AS UINT64;
    LET r AS REAL;
    LET rr AS RICHREAL;
    LET t AS TEXT;
    
    write_log ("Started test null_test_3");
    
    IF (b != NULL) DO
        write_log ("... FAIL: b is not null");
        RETURN FALSE;
    END
    
    b = FALSE;
    
    IF (c != NULL) DO
        write_log ("... FAIL: c is not null");
        RETURN FALSE;
    END
    
    c = 'a';

    IF (d != NULL) DO
        write_log ("... FAIL: d is not null");
        RETURN FALSE;
    END
    
    d = '1800/10/14';
    
    IF (dt != NULL) DO
        write_log ("... FAIL: dt is not null");
        RETURN FALSE;
    END
    
    dt = '-1/10/14 10:31:12';
    
    IF (ht != NULL) DO
        write_log ("... FAIL: ht is not null");
        RETURN FALSE;
    END
    
    ht = '1/10/14 10:31:12.111222';
    
    IF (i8 != NULL) DO
        write_log ("... FAIL: i8 is not null");
        RETURN FALSE;
    END
    
    i8 = -8;
    
    IF (i16 != NULL) DO
        write_log ("... FAIL: i16 is not null");
        RETURN FALSE;
    END
    
    i16 = -1024;
    
    IF (i32 != NULL) DO
        write_log ("... FAIL: i32 is not null");
        RETURN FALSE;
    END
    
    i32 = -6789001;
    
    IF (i64 != NULL) DO
        write_log ("... FAIL: i64 is not null");
        RETURN FALSE;
    END
    
    i64 = -678900100911122;
    
    IF (u8 != NULL) DO
        write_log ("... FAIL: u8 is not null");
        RETURN FALSE;
    END

    u8 = 1;
    
    IF (u16 != NULL) DO
        write_log ("... FAIL: u16 is not null");
        RETURN FALSE;
    END
    
    u16 = 12;
   
    IF (u32 != NULL) DO
        write_log ("... FAIL: u32 is not null");
        RETURN FALSE;
    END
    
    u32 = 10;
    
    IF (u64 != NULL) DO
        write_log ("... FAIL: u64 is not null");
        RETURN FALSE;
    END
    
    u64 = 11;
    
    IF (r != NULL) DO
        write_log ("... FAIL: r is not null");
        RETURN FALSE;
    END
    
    r = -12.11;

    IF (rr != NULL) DO
        write_log ("... FAIL: rr is not null");
        RETURN FALSE;
    END
    
    rr = -111.009990111;

    IF (t != NULL) DO
        write_log ("... FAIL: t is not null");
        RETURN FALSE;
    END
    
    t = "End of null_test_3";
    write_log (t);
    
    RETURN TRUE;
ENDPROC

#TEST: null_test_3_1
#This function should return true
PROCEDURE null_test_3_1 () RETURN BOOL
DO
    LET b AS BOOL;
    LET c AS CHAR;
    LET d AS DATE;
    LET dt AS DATETIME;
    LET ht AS HIRESTIME;
    LET i8 AS INT8;
    LET i16 AS INT16;
    LET i32 AS INT32;
    LET i64 AS INT64;
    LET u8 AS  UINT8;
    LET u16 AS UINT16;
    LET u32 AS UINT32;
    LET u64 AS UINT64;
    LET r AS REAL;
    LET rr AS RICHREAL;
    LET t AS TEXT;
    
    write_log ("Started test null_test_3_1");
    
    b = FALSE;
    c = 'a';
    d = '1800/10/14';
    dt = '-1/10/14 10:31:12';
    ht = '1/10/14 10:31:12.111222';
    i8 = -8;
    i16 = 8;
    i32 = -6789001;
    i64 = -678900100911122;
    u8 = 1;
    u16 = 12;
    u32 = 10;
    u64 = 11;
    r = -12.11;
    rr = -111.009990111;
    t = "Some text.";

    IF (b == NULL) DO
        write_log ("... FAIL: b is null");
        RETURN FALSE;
    END
        
    b = NULL;

    IF (b != NULL) DO
        write_log ("... FAIL: b is not null");
        RETURN FALSE;
    END

    IF (c == NULL) DO
        write_log ("... FAIL: c is null");
        RETURN FALSE;
    END
    
    c = NULL;
        
    IF (c != NULL) DO
        write_log ("... FAIL: c is not null");
        RETURN FALSE;
    END

    IF (d == NULL) DO
        write_log ("... FAIL: d is null");
        RETURN FALSE;
    END
   
    d = NULL;

    IF (d != NULL) DO
        write_log ("... FAIL: d is not null");
        RETURN FALSE;
    END
    
    IF (dt == NULL) DO
        write_log ("... FAIL: dt is null");
        RETURN FALSE;
    END

    dt = NULL;

    IF (dt != NULL) DO
        write_log ("... FAIL: dt is not null");
        RETURN FALSE;
    END

    IF (ht == NULL) DO
        write_log ("... FAIL: ht is null");
        RETURN FALSE;
    END
    
    ht = NULL;    
    
    IF (ht != NULL) DO
        write_log ("... FAIL: ht is not null");
        RETURN FALSE;
    END
    
    IF (i8 == NULL) DO
        write_log ("... FAIL: i8 is null");
        RETURN FALSE;
    END

    i8 = NULL;

    IF (i8 != NULL) DO
        write_log ("... FAIL: i8 is not null");
        RETURN FALSE;
    END
    
    IF (i16 == NULL) DO
        write_log ("... FAIL: i16 is null");
        RETURN FALSE;
    END

    i16 = NULL;

    IF (i16 != NULL) DO
        write_log ("... FAIL: i16 is not null");
        RETURN FALSE;
    END
        
    IF (i32 == NULL) DO
        write_log ("... FAIL: i32 is null");
        RETURN FALSE;
    END

    i32 = NULL;

    IF (i32 != NULL) DO
        write_log ("... FAIL: i32 is not null");
        RETURN FALSE;
    END
    
    IF (i64 == NULL) DO
        write_log ("... FAIL: i64 is null");
        RETURN FALSE;
    END

    i64 = NULL;

    IF (i64 != NULL) DO
        write_log ("... FAIL: i64 is not null");
        RETURN FALSE;
    END
    
    IF (u8 == NULL) DO
        write_log ("... FAIL: u8 is null");
        RETURN FALSE;
    END

    u8 = NULL;

    IF (u8 != NULL) DO
        write_log ("... FAIL: u8 is not null");
        RETURN FALSE;
    END
    
    IF (u16 == NULL) DO
        write_log ("... FAIL: u16 is null");
        RETURN FALSE;
    END

    u16 = NULL;

    IF (u16 != NULL) DO
        write_log ("... FAIL: u16 is not null");
        RETURN FALSE;
    END
        
    IF (u32 == NULL) DO
        write_log ("... FAIL: u32 is null");
        RETURN FALSE;
    END

    u32 = NULL;

    IF (u32 != NULL) DO
        write_log ("... FAIL: u32 is not null");
        RETURN FALSE;
    END
    
    IF (u64 == NULL) DO
        write_log ("... FAIL: u64 is null");
        RETURN FALSE;
    END

    u64 = NULL;

    IF (u64 != NULL) DO
        write_log ("... FAIL: u64 is not null");
        RETURN FALSE;
    END

    IF (r == NULL) DO
        write_log ("... FAIL: r is null");
        RETURN FALSE;
    END
    
    r = NULL;
    
    IF (r != NULL) DO
        write_log ("... FAIL: r is not null");
        RETURN FALSE;
    END

    IF (rr == NULL) DO
        write_log ("... FAIL: rr is  null");
        RETURN FALSE;
    END

    rr = NULL;
 
    IF (rr != NULL) DO
        write_log ("... FAIL: rr is not null");
        RETURN FALSE;
    END

    IF (t == NULL) DO
        write_log ("... FAIL: t is null");
        RETURN FALSE;
    END
    
    t = NULL;
    
    IF (t != NULL) DO
        write_log ("... FAIL: t is not null");
        RETURN FALSE;
    END
    
    write_log ("End of test null_test_3_1");
    
    RETURN TRUE;
ENDPROC


#TEST: null_test_4
#This function should return true
PROCEDURE null_test_4 () RETURN BOOL
DO
    LET t AS TEXT;

    write_log ("Started test null_test_4");

    IF (t != NULL) DO
        write_log ("... FAIL: The text is not null: " + t);
        RETURN FALSE;    
    END

    IF (t[0] != NULL) DO
        write_log ("... FAIL: The first char of text is not null: " + t);
        RETURN FALSE;
    END

    t[0] = 'P';

    IF (t == NULL) DO
        write_log ("... FAIL: The text is null.");
        RETURN FALSE;
    END
    
    IF (t[0] == NULL) DO
        write_log ("... FAIL: The first char of text is null");
        RETURN FALSE;
    END
    
    IF (t[1] != NULL) DO
        write_log ("... FAIL: The second char of text is not null: " + t);
        RETURN FALSE;
    END

    IF (t[2] != NULL) DO
        write_log ("... FAIL: The third char of text is not null: " + t);
        RETURN FALSE;
    END

    write_log ("End of null_test_4");

    RETURN TRUE;
ENDPROC

#TEST: null_test_4_1
#This function should return true
PROCEDURE null_test_4_1 () RETURN BOOL
DO
    LET tab AS TABLE OF (field1 AS TEXT);

    write_log ("Started test null_test_4_1");

    IF (tab != NULL) DO
        write_log ("... FAIL: Table is not null");
        RETURN FALSE;
    END

    IF (tab.field1[0] != NULL) DO
        write_log ("... FAIL: The text field is not null.");
        RETURN FALSE;    
    END

    IF (tab.field1[0] != NULL) DO
        write_log ("... FAIL: The first char of text field is not null.");
        RETURN FALSE;
    END

    tab.field1[0][0] = 'P';

    IF (tab == NULL) DO
        write_log ("... FAIL: The table is null.");
        RETURN FALSE;
    END

    IF (tab.field1[0] == NULL) DO
        write_log ("... FAIL: The text field is null.");
        RETURN FALSE;
    END

    IF (tab.field1[0][0] == NULL) DO
        write_log ("... FAIL: The first char of text field is null");
        RETURN FALSE;
    END
    
    IF (tab.field1[0][1] != NULL) DO
        write_log ("... FAIL: The second char of text field is not null.");
        RETURN FALSE;
    END
    
    IF (tab.field1[0][2] != NULL) DO
        write_log ("... FAIL: The third char of text field is not null.");
        RETURN FALSE;
    END

    write_log ("End of null_test_4_1");

    RETURN TRUE;
ENDPROC


