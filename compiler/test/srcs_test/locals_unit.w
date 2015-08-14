#Test file to check local variables are properly defined and
#if the same output is resulted, no matter of the usedcompiler's
#machine architectures.

PROCEDURE test_procedure (p1 ARRAY, p2 as FIELD, p3 TABLE) return FIELD
DO
	VAR local0 BOOL;
	VAR local1 UINT;
	VAR local2 DATE;
	VAR local3 DATETIME;
	VAR local4 HIRESTIME;
	VAR local5 INT8;
	VAR local6 INT16;
	VAR local7 INT32;
	VAR local8 INT64;
	VAR local9 UINT8;
	VAR local10 UINT16;
	VAR local11 UINT32;
	VAR local12 UINT64;
	VAR local13 REAL;
	VAR local14 RICHREAL;
	VAR local15 TEXT;

	VAR local16 BOOL ARRAY;
	VAR local17 UINT ARRAY;
	VAR local18 DATE ARRAY;
	VAR local19 DATETIME ARRAY;
	VAR local20 HIRESTIME ARRAY;
	VAR local21 INT8 ARRAY;
	VAR local22 INT16 ARRAY;
	VAR local23 INT32 ARRAY;
	VAR local24 INT64 ARRAY;
	VAR local25 UINT8 ARRAY;
	VAR local26 UINT16 ARRAY;
	VAR local27 UINT32 ARRAY;
	VAR local28 UINT64 ARRAY;
	VAR local29 REAL ARRAY;
	VAR local30 RICHREAL ARRAY;

	VAR local31 BOOL FIELD;
	VAR local32 UINT FIELD;
	VAR local33 DATE FIELD;
	VAR local34 DATETIME FIELD;
	VAR local35 HIRESTIME FIELD;
	VAR local36 INT8 FIELD;
	VAR local37 INT16 FIELD;
	VAR local38 INT32 FIELD;
	VAR local39 INT64 FIELD;
	VAR local40 UINT8 FIELD;
	VAR local41 UINT16 FIELD;
	VAR local42 UINT32 FIELD;
	VAR local43 UINT64 FIELD;
	VAR local44 REAL FIELD;
	VAR local45 RICHREAL FIELD;
	VAR local46 TEXT FIELD;

	VAR local47 BOOL ARRAY FIELD;
	VAR local48 UINT ARRAY FIELD;
	VAR local49 DATE ARRAY FIELD;
	VAR local65 DATETIME ARRAY FIELD;
	VAR local51 HIRESTIME ARRAY FIELD;
	VAR local52 INT8 ARRAY FIELD;
	VAR local53 INT16 ARRAY FIELD;
	VAR local54 INT32 ARRAY FIELD;
	VAR local55 INT64 ARRAY FIELD;
	VAR local56 UINT8 ARRAY FIELD;
	VAR local57 UINT16 ARRAY FIELD;
	VAR local58 UINT32 ARRAY FIELD;
	VAR local59 UINT64 ARRAY FIELD;
	VAR local60 REAL ARRAY FIELD;
	VAR local61 RICHREAL ARRAY FIELD;

	VAR local62 TABLE OF (field1 DATE, field2 RICHREAL ARRAY, field3 TEXT);

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

