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


PROCEDURE same_bool (a BOOL) RETURN BOOL
DO 
	RETURN a;
ENDPROC

PROCEDURE same_char (a CHAR) RETURN CHAR
DO 
	RETURN a;
ENDPROC

PROCEDURE same_date (a DATE) RETURN DATE
DO 
	RETURN a;
ENDPROC

PROCEDURE same_datetime (a DATETIME) RETURN DATETIME
DO 
	RETURN a;
ENDPROC

PROCEDURE same_hirestime (a HIRESTIME) RETURN HIRESTIME
DO 
	RETURN a;
ENDPROC

PROCEDURE same_int8 (a INT8) RETURN INT8
DO 
	RETURN a;
ENDPROC

PROCEDURE same_int16 (a INT16) RETURN INT16
DO 
	RETURN a;
ENDPROC

PROCEDURE same_int32 (a INT32) RETURN INT32
DO 
	RETURN a;
ENDPROC

PROCEDURE same_int64 (a INT64) RETURN INT64
DO 
	RETURN a;
ENDPROC

PROCEDURE same_uint8 (a UINT8) RETURN UINT8
DO 
	RETURN a;
ENDPROC

PROCEDURE same_uint16 (a UINT16) RETURN UINT16
DO 
	RETURN a;
ENDPROC

PROCEDURE same_uint32 (a UINT32) RETURN UINT32
DO 
	RETURN a;
ENDPROC

PROCEDURE same_uint64 (a UINT64) RETURN UINT64
DO 
	RETURN a;
ENDPROC

PROCEDURE same_real (a REAL) RETURN REAL
DO 
	RETURN a;
ENDPROC

PROCEDURE same_richreal (a RICHREAL) RETURN RICHREAL
DO 
	RETURN a;
ENDPROC

PROCEDURE same_text (a TEXT) RETURN TEXT
DO 
	RETURN a;
ENDPROC

PROCEDURE same_array (a ARRAY) RETURN ARRAY
DO 
	RETURN a;
ENDPROC

PROCEDURE same_table (a TABLE) RETURN TABLE
DO 
	RETURN a;
ENDPROC

