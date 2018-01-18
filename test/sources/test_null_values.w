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
    VAR row UINT64;
    VAR result TABLE (field1 BOOL,
                            field2 CHAR,
                            field3 DATE,
                            field4 DATETIME,
                            field5 HIRESTIME,
                            field6 INT8,
                            field7 INT16,
                            field8 INT32,
                            field9 INT64,
                            field10 UINT8,
                            field11 UINT16,
                            field12 UINT32,
                            field13 UINT64,
                            field14 REAL,
                            field15 RICHREAL,
                            field16 TEXT);

    write_log ("Start of null_test_1");
    IF (row != NULL) DO
        write_log ("... FAIL: 'row' is not null by default");
        
    END

    IF (result != NULL) DO
        write_log ("... FAIL: 'result' is not null by default.");
    END

    write_log ("... Local values are null by default.");

    row = get_empty (result);
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
    VAR row UINT64;
    VAR result TABLE (field1 BOOL,
                            field2 CHAR,
                            field3 DATE,
                            field4 DATETIME,
                            field5 HIRESTIME,
                            field6 INT8,
                            field7 INT16,
                            field8 INT32,
                            field9 INT64,
                            field10 UINT8,
                            field11 UINT16,
                            field12 UINT32,
                            field13 UINT64,
                            field14 REAL,
                            field15 RICHREAL,
                            field16 TEXT);

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

    row = get_empty (result);
    
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

    write_log ("End of null_test_1_1");

    RETURN TRUE;
ENDPROC


#TEST: null_test_1_2
#Call this function twice in a row and make sure it returns the same non 
#null result.
PROCEDURE null_test_1_2 ()
RETURN TABLE
DO
    VAR row UINT64;
    VAR result TABLE (field1 BOOL ARRAY,
                            field2 CHAR ARRAY,
                            field3 DATE ARRAY,
                            field4 DATETIME ARRAY,
                            field5 HIRESTIME ARRAY,
                            field6 INT8 ARRAY,
                            field7 INT16 ARRAY,
                            field8 INT32 ARRAY,
                            field9 INT64 ARRAY,
                            field10 UINT8 ARRAY,
                            field11 UINT16 ARRAY,
                            field12 UINT32 ARRAY,
                            field13 UINT64 ARRAY,
                            field14 REAL ARRAY,
                            field15 RICHREAL ARRAY,
                            field16 TEXT);

    write_log ("Start of null_test_1_2");
    IF (row != NULL) DO
        write_log ("... FAIL: 'row' is not null by default");
    END

    IF (result != NULL) DO
        write_log ("... FAIL: 'result' is not null by default.");
    END

    write_log ("... Local values are null by default.");

    row = get_empty (result);
    
    result.field1[row] += TRUE;
    result.field1[row] += FALSE;
    result.field2[row] += 'C';
    result.field2[row] += 'D';
    result.field3[row] += '1800/10/14';
    result.field3[row] += '1801/11/13';

    result.field4[row] += '-1/10/14 10:31:12';
    result.field5[row] += '1/10/14 10:31:12.111222';
    result.field6[row] += -8;
    result.field7[row] += -16;
    result.field8[row] += -32;
    result.field9[row] += -64;
    result.field10[row] += 8;
    result.field11[row] += 16;
    result.field12[row] += 32;
    result.field13[row] += 64;
    result.field14[row] += -1.1;
    result.field15[row] += 2.12;
    result.field16[row] = "Text field";
    
    write_log ("End of null_test_1_2");

    RETURN result;
ENDPROC


#TEST: null_test_1_3
#Call this function twice in a row and make sure it returns TRUE 
PROCEDURE null_test_1_3 ()
RETURN BOOL
DO
    VAR result TABLE (field1 BOOL ARRAY,
                            field2 CHAR ARRAY,
                            field3 DATE ARRAY,
                            field4 DATETIME ARRAY,
                            field5 HIRESTIME ARRAY,
                            field6 INT8 ARRAY,
                            field7 INT16 ARRAY,
                            field8 INT32 ARRAY,
                            field9 INT64 ARRAY,
                            field10 UINT8 ARRAY,
                            field11 UINT16 ARRAY,
                            field12 UINT32 ARRAY,
                            field13 UINT64 ARRAY,
                            field14 REAL ARRAY,
                            field15 RICHREAL ARRAY,
                            field16 TEXT);
    VAR row UINT64;
    
    write_log ("Start of null_test_1_3");
    IF (row != NULL) DO
        write_log ("... FAIL: 'row' is not null by default");
    END

    IF (result != NULL) DO
        write_log ("... FAIL: 'result' is not null by default.");
    END

    write_log ("... Local values are null by default.");
    
    row = get_empty (result);
    
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
    
    result.field1[row] += TRUE;
    result.field1[row] += FALSE;
    result.field2[row] += 'C';
    result.field2[row] += 'D';
    result.field3[row] += '1800/10/14';
    result.field3[row] += '1801/11/13';
    result.field4[row] += '-1/10/14 10:31:12';
    result.field5[row] += '1/10/14 10:31:12.111222';
    result.field6[row] += -8;
    result.field7[row] += -16;
    result.field8[row] += -32;
    result.field9[row] += -64;
    result.field10[row] += 8;
    result.field11[row] += 16;
    result.field12[row] += 32;
    result.field13[row] += 64;
    result.field14[row] += -1.1;
    result.field15[row] += 2.12;
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

    result.field1[row] += TRUE;
    result.field2[row] += 'C';
    result.field3[row] += '1800/10/14';
    result.field4[row] += '-1/10/14 10:31:12';
    result.field5[row] += '1/10/14 10:31:12.111222';
    result.field6[row] += -8;
    result.field7[row] += -16;
    result.field8[row] += -32;
    result.field9[row] += -64;
    result.field10[row] += 8;
    result.field11[row] += 16;
    result.field12[row] += 32;
    result.field13[row] += 64;
    result.field14[row] += -1.1;
    result.field15[row] += 2.12;
    
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



#TEST: null_test_1_4
#Call this function twice a row and make sure it returns TRUE each time.
PROCEDURE null_test_1_4 ()
RETURN BOOL
DO
    VAR row UINT64;
    VAR tab TABLE (field1 DATE);

    write_log ("Start of null_test_1_4");
    IF (row != NULL) DO
        write_log ("... FAIL: 'row' is not null by default");
        RETURN FALSE;
    END

    IF (tab != NULL) DO
        write_log ("... FAIL: 'result' is not null by default.");
        RETURN FALSE;
    END

    write_log ("... Local values are null by default.");

    row = 0;
    tab.field1[row] = '1800/10/14';
    IF (tab == NULL) DO
        write_log ("Table is null though it should not.");
        RETURN FALSE;
    END

    tab = NULL;

    IF (tab != NULL) DO
        write_log ("Table is not null though it should be.");
        RETURN FALSE;
    END

    write_log ("End of null_test_1_4");

    RETURN TRUE;
ENDPROC

#TEST: null_test_1_5
#Call this function twice a row and make sure it returns TRUE each time.
PROCEDURE null_test_1_5 ()
RETURN BOOL
DO
    VAR tab TABLE (field1 DATE);
    VAR fld DATE FIELD;

    write_log ("Start of null_test_1_5");
    IF (tab != NULL) DO
        write_log ("... FAIL: 'result' is not null by default.");
        RETURN FALSE;
    END
    
    IF (fld != NULL) DO
        write_log ("... FAIL: 'fld' is not null by default");
        RETURN FALSE;
    END

    write_log ("... Local values are null by default.");
    
    fld = tab.field1;
    fld[0] = '1800/10/14';
    
    IF (tab == NULL) DO
        write_log ("Table is null though it should not.");
        RETURN FALSE;
    END
    
    IF (fld == NULL) DO
        write_log ("Table field is null though it should not.");
        RETURN FALSE;
    END

    fld = NULL;

    IF (fld != NULL) DO
        write_log ("Table field is not null though it should be.");
        RETURN FALSE;
    END

    write_log ("End of null_test_1_5");

    RETURN TRUE;
ENDPROC


#TEST: null_test_2
#Call this function twice in and make sure it returns the same result.
PROCEDURE null_test_2 ()
RETURN ARRAY
DO
    VAR result INT32 ARRAY;
    
    write_log ("Start of null_test_2");
    IF (result != NULL) DO
        write_log ("... FAIL: 'result' is not null by default.");
	RETURN NULL;
    END

    write_log ("... Local values are null by default.");

    result += { -1000, 20} INT32;

    write_log ("End of null_test_2");

    RETURN result;
ENDPROC



#TEST: null_test_2_1
#Call this function and it should return TRUE for the test to pass.
PROCEDURE null_test_2_1 ()
RETURN BOOL
DO

    VAR array1 BOOL ARRAY;
    VAR array2 CHAR ARRAY;
    VAR array3 DATE ARRAY;
    VAR array4 DATETIME ARRAY;
    VAR array5 HIRESTIME ARRAY;
    VAR array6 INT8 ARRAY;
    VAR array7 INT16 ARRAY;
    VAR array8 INT32 ARRAY;
    VAR array9 INT64 ARRAY;
    VAR array10 UINT8 ARRAY;
    VAR array11 UINT16 ARRAY;
    VAR array12 UINT32 ARRAY;
    VAR array13 UINT64 ARRAY;
    VAR array14 REAL ARRAY;
    VAR array15 RICHREAL ARRAY;
    VAR array16 TEXT;

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
    
    array1 += TRUE;
    array1 += FALSE;
    array2 += 'C';
    array2 += 'D';
    array3 += '1800/10/14';
    array3 += '1801/11/13';
    array4 += '-1/10/14 10:31:12';
    array5 += '1/10/14 10:31:12.111222';
    array6 += -8;
    array7 += -16;
    array8 += -32;
    array9 += -64;
    array10 += 8;
    array11 += 16;
    array12 += 32;
    array13 += 64;
    array14 += -1.1;
    array15 += 2.12;
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
    
    array1 += TRUE;
    array2 += 'C';
    array3 += '1800/10/14';
    array4 += '-1/10/14 10:31:12';
    array5 += '1/10/14 10:31:12.111222';
    array6 += -8;
    array7 += -16;
    array8 += -32;
    array9 += -64;
    array10 += 8;
    array11 += 16;
    array12 += 32;
    array13 += 64;
    array14 += -1.1;
    array15 += 2.12;
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
#Call this function twice in and make sure it returns the same TRUE result.
PROCEDURE null_test_2_2 ()
RETURN BOOL
DO
    VAR result INT8 ARRAY;

    write_log ("Start of null_test_2_2");
    IF (result != NULL) DO
        write_log ("... FAIL: 'result' is not null by default.");
	RETURN FALSE;
    END
    
    IF (result[0] != NULL) DO
        write_log ("... FAIL: 'result[0]' is not null.");
	RETURN FALSE;
    END

    result[0] = -10;

    IF (result == NULL) DO
        write_log ("... FAIL: 'result' is null.");
	RETURN FALSE;
    END
    
    IF (result[0] == NULL) DO
        write_log ("... FAIL: 'result[0]' is null.");
	RETURN FALSE;
    END

    IF (result[1] != NULL) DO
        write_log ("... FAIL: 'result[1]' is not null.");
	RETURN FALSE;
    END
    
    IF (result[2] != NULL) DO
        write_log ("... FAIL: 'result[2]' is not null.");
	RETURN FALSE;
    END

    result += 20;

    write_log ("End of null_test_2_2");

    RETURN TRUE;
ENDPROC


#TEST: null_test_2_3
#Call this function twice in and make sure it returns the same result.
PROCEDURE null_test_2_3 ()
RETURN BOOL
DO
    VAR tab TABLE (field1 DATE ARRAY);

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
    VAR b BOOL;
    VAR c CHAR;
    VAR d DATE;
    VAR dt DATETIME;
    VAR ht HIRESTIME;
    VAR i8 INT8;
    VAR i16 INT16;
    VAR i32 INT32;
    VAR i64 INT64;
    VAR u8  UINT8;
    VAR u16 UINT16;
    VAR u32 UINT32;
    VAR u64 UINT64;
    VAR r REAL;
    VAR rr RICHREAL;
    VAR t TEXT;
    
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
    VAR b BOOL;
    VAR c CHAR;
    VAR d DATE;
    VAR dt DATETIME;
    VAR ht HIRESTIME;
    VAR i8 INT8;
    VAR i16 INT16;
    VAR i32 INT32;
    VAR i64 INT64;
    VAR u8  UINT8;
    VAR u16 UINT16;
    VAR u32 UINT32;
    VAR u64 UINT64;
    VAR r REAL;
    VAR rr RICHREAL;
    VAR t TEXT;
    
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
#This function should return true twice when called multiple times.
PROCEDURE null_test_4 () RETURN BOOL
DO
    VAR t TEXT;

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

#TEST: null_test_4_1a
#This function should return the same result when called multiple times.
PROCEDURE null_test_4_1a () RETURN BOOL
DO
    VAR t TEXT;

    write_log ("Started test null_test_4_1a");

    IF (t != NULL) DO
        write_log ("... FAIL: The text is not null: " + t);
        RETURN FALSE;    
    END

    IF (t[0] != NULL) DO
        write_log ("... FAIL: The first char of text is not null: " + t);
        RETURN FALSE;
    END

    t += "Te";

    IF (t == NULL) DO
        write_log ("... FAIL: The text is null.");
        RETURN FALSE;
    END
    
    IF (t[0] != 'T') DO
        write_log ("... FAIL: The first char of text is not expcted");
        RETURN FALSE;
    END
    
    IF (t[1] != 'e') DO
        write_log ("... FAIL: The second char of text is not expected: " + t);
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
    VAR tab TABLE (field1 TEXT);

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

#TEST: null_test_5
#This function should return a null table with the correct signature.
PROCEDURE null_test_5 () RETURN TABLE (field1 TEXT, field2 UINT32)
DO
    write_log ("Started test null_test_5");
    write_log ("End of null_test_5");

    RETURN NULL;
ENDPROC

#TEST: null_test_5_1
#This function should return a null array with the correct signature.
PROCEDURE null_test_5_1 () RETURN INT32 ARRAY
DO
    write_log ("Started test null_test_5_1");
    write_log ("End of null_test_5_1");

    RETURN NULL;
ENDPROC

#TEST: null_test_5_2
#This function should return a null text with the correct signature.
PROCEDURE null_test_5_2 () RETURN TEXT
DO
    write_log ("Started test null_test_5_2");
    write_log ("End of null_test_5_2");

    RETURN NULL;
ENDPROC

#TEST: null_test_5_3
#This function should return a null bool with the correct signature.
PROCEDURE null_test_5_3 () RETURN BOOL
DO
    write_log ("Started test null_test_5_3");
    write_log ("End of null_test_5_3");

    RETURN NULL;
ENDPROC
