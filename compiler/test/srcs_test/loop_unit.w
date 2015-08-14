#Test file to check the loop statements are properly definied and
#that the same output is resulted, no matter what compiler
#architecture is used.

PROCEDURE while_test () RETURN BOOL
DO
	VAR a, b BOOL;
	
	WHILE a DO
		b = NULL;
	END

	RETURN b;
ENDPROC


PROCEDURE while_cont_test () RETURN BOOL
DO
	VAR a, b BOOL;
	
	WHILE a DO
		b = NULL;
		CONTINUE;
	END

	RETURN b;

ENDPROC

PROCEDURE while_break_test () RETURN BOOL
DO
	VAR a, b BOOL;
	
	WHILE a DO
		b = NULL;
		BREAK;
	END

	RETURN b;

ENDPROC

PROCEDURE while_break_cont_test () RETURN BOOL
DO
	VAR a, b BOOL;
	
	WHILE a DO
		if b == NULL THEN
			BREAK;
		ELSE
			b = TRUE;
			CONTINUE;
		END
		b = FALSE;
	END

	RETURN b;
ENDPROC


PROCEDURE until_stmt () RETURN BOOL
DO
	VAR a, b BOOL;

	DO
		b = FALSE;
	UNTIL a;

	RETURN b;
ENDPROC

PROCEDURE until_break_stmt () RETURN BOOL
DO
	VAR a, b BOOL;

	DO
		IF b == TRUE THEN
			BREAK;
		END

		b = FALSE;

	UNTIL a;

	RETURN b;
ENDPROC

PROCEDURE until_cont_stmt () RETURN BOOL
DO
	VAR a, b BOOL;

	DO
		IF b == TRUE THEN
			CONTINUE;
		END

		b = FALSE;

	UNTIL a;

	RETURN b;
ENDPROC

PROCEDURE until_break_cont_stmt () RETURN BOOL
DO
	VAR a, b, c BOOL;

	DO
		IF b == TRUE THEN
			CONTINUE;
		ELSE
			c = TRUE;
			BREAK;
		END
		b = FALSE;

	UNTIL a;

	RETURN b;
ENDPROC

