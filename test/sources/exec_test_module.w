################################################################################
# Test module used to verify the handling the execution and parsing of
# procedure by the 'wcmd'
# 
# Iulian Popa
#
# Notes:
#  1. It contains a set of special procedure to ensure the execution is triggered
#     and the correct argument value is received.
################################################################################


PROCEDURE same_bool (a AS BOOL) RETURN BOOL
DO 
	RETURN a;
ENDPROC

PROCEDURE same_char (a AS CHAR) RETURN CHAR
DO 
	RETURN a;
ENDPROC

PROCEDURE same_date (a AS DATE) RETURN DATE
DO 
	RETURN a;
ENDPROC

PROCEDURE same_datetime (a AS DATETIME) RETURN DATETIME
DO 
	RETURN a;
ENDPROC

PROCEDURE same_hirestime (a AS HIRESTIME) RETURN HIRESTIME
DO 
	RETURN a;
ENDPROC

PROCEDURE same_int8 (a AS INT8) RETURN INT8
DO 
	RETURN a;
ENDPROC

PROCEDURE same_int16 (a AS INT16) RETURN INT16
DO 
	RETURN a;
ENDPROC

PROCEDURE same_int32 (a AS INT32) RETURN INT32
DO 
	RETURN a;
ENDPROC

PROCEDURE same_int64 (a AS INT64) RETURN INT64
DO 
	RETURN a;
ENDPROC

PROCEDURE same_uint8 (a AS UINT8) RETURN UINT8
DO 
	RETURN a;
ENDPROC

PROCEDURE same_uint16 (a AS UINT16) RETURN UINT16
DO 
	RETURN a;
ENDPROC

PROCEDURE same_uint32 (a AS UINT32) RETURN UINT32
DO 
	RETURN a;
ENDPROC

PROCEDURE same_uint64 (a AS UINT64) RETURN UINT64
DO 
	RETURN a;
ENDPROC

PROCEDURE same_real (a AS REAL) RETURN REAL
DO 
	RETURN a;
ENDPROC

PROCEDURE same_richreal (a AS RICHREAL) RETURN RICHREAL
DO 
	RETURN a;
ENDPROC

PROCEDURE same_text (a AS TEXT) RETURN TEXT
DO 
	RETURN a;
ENDPROC

PROCEDURE same_array (a AS ARRAY) RETURN ARRAY
DO 
	RETURN a;
ENDPROC

PROCEDURE same_table (a AS TABLE) RETURN TABLE
DO 
	RETURN a;
ENDPROC

