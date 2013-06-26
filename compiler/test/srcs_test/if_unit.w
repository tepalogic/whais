#Test file to check the if statements are properly defined and
#that the same output is resulted, no matter what compiler
#architecture is used.


PROCEDURE if_test () RETURN BOOL
DO
	LET a, b, c AS BOOL;

	IF a THEN
		b = TRUE;
	END
	
	RETURN c;
ENDPROC

PROCEDURE if_else_test () RETURN BOOL
DO
	LET a, b, c, d AS BOOL;

	IF a THEN
		b = TRUE;
	ELSE
		c = FALSE;
	END
	
	RETURN d;
ENDPROC

PROCEDURE if_elif_test () RETURN BOOL
DO
	LET a, b, c, d AS BOOL;

	IF a THEN
		b = TRUE;
	ELSEIF b THEN
		c = TRUE;
	END
	
	RETURN d;
ENDPROC

PROCEDURE if_elif_else_test () RETURN BOOL
DO
	LET a, b, c, d AS BOOL;

	IF a THEN
		b = TRUE;
	ELSEIF b THEN
		c = TRUE;
	ELSE
		d = TRUE;
	END
	
	RETURN d;
ENDPROC

PROCEDURE if_elif_else_empty_test () RETURN BOOL
DO
	LET a, b, c, d AS BOOL;

	IF a THEN
		#Empty statement
	ELSEIF b THEN
		#Empty statement
	ELSE
		#Empty statement
	END
	
	RETURN d;
ENDPROC

