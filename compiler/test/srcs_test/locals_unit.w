#Test file to check local variables are properly defined and
#if the same output is resulted, no matter of the usedcompiler's
#machine architectures.

PROCEDURE test_procedure (p1 AS ARRAY, p2 as FIELD, p3 AS TABLE) return FIELD
DO
	LET local0 AS BOOL;
	LET local1 AS CHARACTER;
	LET local2 AS DATE;
	LET local3 AS DATETIME;
	LET local4 AS HIRESTIME;
	LET local5 AS INT8;
	LET local6 AS INT16;
	LET local7 AS INT32;
	LET local8 AS INT64;
	LET local9 AS UNSIGNED INT8;
	LET local10 AS UNSIGNED INT16;
	LET local11 AS UNSIGNED INT32;
	LET local12 AS UNSIGNED INT64;
	LET local13 AS REAL;
	LET local14 AS RICHREAL;
	LET local15 AS TEXT;

	LET local16 AS ARRAY OF BOOL;
	LET local17 AS ARRAY OF CHARACTER;
	LET local18 AS ARRAY OF DATE;
	LET local19 AS ARRAY OF DATETIME;
	LET local20 AS ARRAY OF HIRESTIME;
	LET local21 AS ARRAY OF INT8;
	LET local22 AS ARRAY OF INT16;
	LET local23 AS ARRAY OF INT32;
	LET local24 AS ARRAY OF INT64;
	LET local25 AS ARRAY OF UNSIGNED INT8;
	LET local26 AS ARRAY OF UNSIGNED INT16;
	LET local27 AS ARRAY OF UNSIGNED INT32;
	LET local28 AS ARRAY OF UNSIGNED INT64;
	LET local29 AS ARRAY OF REAL;
	LET local30 AS ARRAY OF RICHREAL;

	LET local31 AS FIELD OF BOOL;
	LET local32 AS FIELD OF CHARACTER;
	LET local33 AS FIELD OF DATE;
	LET local34 AS FIELD OF DATETIME;
	LET local35 AS FIELD OF HIRESTIME;
	LET local36 AS FIELD OF INT8;
	LET local37 AS FIELD OF INT16;
	LET local38 AS FIELD OF INT32;
	LET local39 AS FIELD OF INT64;
	LET local40 AS FIELD OF UNSIGNED INT8;
	LET local41 AS FIELD OF UNSIGNED INT16;
	LET local42 AS FIELD OF UNSIGNED INT32;
	LET local43 AS FIELD OF UNSIGNED INT64;
	LET local44 AS FIELD OF REAL;
	LET local45 AS FIELD OF RICHREAL;
	LET local46 AS FIELD OF TEXT;

	LET local47 AS FIELD OF ARRAY OF BOOL;
	LET local48 AS FIELD OF ARRAY OF CHARACTER;
	LET local49 AS FIELD OF ARRAY OF DATE;
	LET local65 AS FIELD OF ARRAY OF DATETIME;
	LET local51 AS FIELD OF ARRAY OF HIRESTIME;
	LET local52 AS FIELD OF ARRAY OF INT8;
	LET local53 AS FIELD OF ARRAY OF INT16;
	LET local54 AS FIELD OF ARRAY OF INT32;
	LET local55 AS FIELD OF ARRAY OF INT64;
	LET local56 AS FIELD OF ARRAY OF UNSIGNED INT8;
	LET local57 AS FIELD OF ARRAY OF UNSIGNED INT16;
	LET local58 AS FIELD OF ARRAY OF UNSIGNED INT32;
	LET local59 AS FIELD OF ARRAY OF UNSIGNED INT64;
	LET local60 AS FIELD OF ARRAY OF REAL;
	LET local61 AS FIELD OF ARRAY OF RICHREAL;

	LET local62 AS TABLE OF (field1 AS DATE, field2 AS ARRAY OF RICHREAL, field3 AS TEXT);

	local0 = NULL;
	local1 = NULL;
	local2 = NULL;
	local3 = NULL;
	local4 = NULL;
	local5 = NULL;
	local6 = NULL;
	local7 = NULL;
	local8 = NULL;
	local9 = NULL;
	local10 = NULL;
	local11 = NULL;
	local12 = NULL;
	local13 = NULL;
	local14 = NULL;
	local15 = NULL;

	local16 = NULL;
	local17 = NULL;
	local18 = NULL;
	local19 = NULL;
	local20 = NULL;
	local21 = NULL;
	local22 = NULL;
	local23 = NULL;
	local24 = NULL;
	local25 = NULL;
	local26 = NULL;
	local27 = NULL;
	local28 = NULL;
	local29 = NULL;
	local30 = NULL;

	local31 = NULL;
	local32 = NULL;
	local33 = NULL;
	local34 = NULL;
	local35 = NULL;
	local36 = NULL;
	local37 = NULL;
	local38 = NULL;
	local39 = NULL;
	local40 = NULL;
	local41 = NULL;
	local42 = NULL;
	local43 = NULL;
	local44 = NULL;
	local45 = NULL;
	local46 = NULL;

	local47 = NULL;
	local48 = NULL;
	local49 = NULL;
	local65 = NULL;
	local51 = NULL;
	local52 = NULL;
	local53 = NULL;
	local54 = NULL;
	local55 = NULL;
	local56 = NULL;
	local57 = NULL;
	local58 = NULL;
	local59 = NULL;
	local60 = NULL;
	local61 = NULL;

	local62 = NULL;

	local62.field1[1] = NULL;
	local62.field2[2] = NULL;
	local62.field3[3] = NULL;

	RETURN NULL;
ENDPROC

