#Test file to check if the different constants are properly handled.

LET glb0 AS BOOL;
LET glb1 AS UINT;
LET glb2 AS DATE;
LET glb3 AS DATETIME;
LET glb4 AS HIRESTIME;
LET glb5 AS INT64;
LET glb6 AS REAL;
LET glb7 AS RICHREAL;
LET glb8 AS TEXT;

PROCEDURE test () RETURN BOOL
DO
	glb0 = TRUE;
	glb0 = FALSE;
	glb0 = NULL;

	glb1 = 'A';
	glb1 = '-';
	glb1 = '+';
	glb1 = '\0xA9DEC7';   #11132615
	glb1 = '\0xa9dec7';   #11132615
	glb1 = '\11132615';   #0xA9DEC7
	glb1 = NULL;

	glb2 = '2013/06/09';
	glb2 = NULL;

	glb3 = '1987/04/01 13:14:15';
	glb3 = NULL;

	glb4 = '1/03/21 01:10:59.91234';
	glb4 = NULL;

	glb5 = -128;
	glb5 = -127;
	glb5 = 127;
	glb5 = 126;
	glb5 = 125;

	glb5 = 65535;
	glb5 = 65536;
	glb5 = -32768;
	glb5 = -32767;
	glb5 = 32767;
	glb5 = 32766;

	glb5 = 0xFFFFFFFF;
	glb5 = 0x100000000;
	glb5 = -0x100000000;
	glb5 = -2147483648;
	glb5 = -2147483647;
	glb5 = 2147483647;
	glb5 = 2147483646;

	glb5 = 0xFFFFFFFFFFFFFFFF;
	glb5 = 0x10AA00000000;
	glb5 = -0xFFFFFFFFFFFFFFFF;
	glb5 = -0x10AA00000000;

	glb5 = 0;
	glb5 = NULL;


	glb6 = 5.3;
	glb6 = 1.0;
	glb6 = -1.0;
	glb6 = 10.3;
	glb6 = -78129934.001233007;


	glb8 = "My name is Iulian Popa and there is nothing you can do to stop me!-->>\0xAABBCCDD<<--";

ENDPROC


