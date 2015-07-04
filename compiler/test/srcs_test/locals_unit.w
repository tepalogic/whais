#Test file to check local variables are properly defined and
#if the same output is resulted, no matter of the usedcompiler's
#machine architectures.

PROCEDURE test_procedure (p1 AS ARRAY, p2 as FIELD, p3 AS TABLE) return FIELD
DO
	VAR local0 AS BOOL;
	VAR local1 AS UINT;
	VAR local2 AS DATE;
	VAR local3 AS DATETIME;
	VAR local4 AS HIRESTIME;
	VAR local5 AS INT8;
	VAR local6 AS INT16;
	VAR local7 AS INT32;
	VAR local8 AS INT64;
	VAR local9 AS UINT8;
	VAR local10 AS UINT16;
	VAR local11 AS UINT32;
	VAR local12 AS UINT64;
	VAR local13 AS REAL;
	VAR local14 AS RICHREAL;
	VAR local15 AS TEXT;

	VAR local16 AS ARRAY OF BOOL;
	VAR local17 AS ARRAY OF UINT;
	VAR local18 AS ARRAY OF DATE;
	VAR local19 AS ARRAY OF DATETIME;
	VAR local20 AS ARRAY OF HIRESTIME;
	VAR local21 AS ARRAY OF INT8;
	VAR local22 AS ARRAY OF INT16;
	VAR local23 AS ARRAY OF INT32;
	VAR local24 AS ARRAY OF INT64;
	VAR local25 AS ARRAY OF UINT8;
	VAR local26 AS ARRAY OF UINT16;
	VAR local27 AS ARRAY OF UINT32;
	VAR local28 AS ARRAY OF UINT64;
	VAR local29 AS ARRAY OF REAL;
	VAR local30 AS ARRAY OF RICHREAL;

	VAR local31 AS FIELD OF BOOL;
	VAR local32 AS FIELD OF UINT;
	VAR local33 AS FIELD OF DATE;
	VAR local34 AS FIELD OF DATETIME;
	VAR local35 AS FIELD OF HIRESTIME;
	VAR local36 AS FIELD OF INT8;
	VAR local37 AS FIELD OF INT16;
	VAR local38 AS FIELD OF INT32;
	VAR local39 AS FIELD OF INT64;
	VAR local40 AS FIELD OF UINT8;
	VAR local41 AS FIELD OF UINT16;
	VAR local42 AS FIELD OF UINT32;
	VAR local43 AS FIELD OF UINT64;
	VAR local44 AS FIELD OF REAL;
	VAR local45 AS FIELD OF RICHREAL;
	VAR local46 AS FIELD OF TEXT;

	VAR local47 AS FIELD OF ARRAY OF BOOL;
	VAR local48 AS FIELD OF ARRAY OF UINT;
	VAR local49 AS FIELD OF ARRAY OF DATE;
	VAR local65 AS FIELD OF ARRAY OF DATETIME;
	VAR local51 AS FIELD OF ARRAY OF HIRESTIME;
	VAR local52 AS FIELD OF ARRAY OF INT8;
	VAR local53 AS FIELD OF ARRAY OF INT16;
	VAR local54 AS FIELD OF ARRAY OF INT32;
	VAR local55 AS FIELD OF ARRAY OF INT64;
	VAR local56 AS FIELD OF ARRAY OF UINT8;
	VAR local57 AS FIELD OF ARRAY OF UINT16;
	VAR local58 AS FIELD OF ARRAY OF UINT32;
	VAR local59 AS FIELD OF ARRAY OF UINT64;
	VAR local60 AS FIELD OF ARRAY OF REAL;
	VAR local61 AS FIELD OF ARRAY OF RICHREAL;

	VAR local62 AS TABLE OF (field1 AS DATE, field2 AS ARRAY OF RICHREAL, field3 AS TEXT);

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

