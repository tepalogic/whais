#Test file to check the procedure call statements.
#The same output shoould be resulted no matter what compiler machine
#archintecture is used.


PROCEDURE proc_1 () return BOOL
DO
	RETURN FALSE;
ENDPROC

EXTERN PROCEDURE ext_proc_1 () RETURN TEXT;
EXTERN PROCEDURE ext_proc_2 () RETURN DATE;

PROCEDURE proc_2 (p1 AS TABLE) RETURN INT8
DO
	RETURN 1 + 2;
ENDPROC

PROCEDURE proc_3 (p1 AS ARRAY) RETURN REAL
DO
	RETURN 1 - 2;
ENDPROC


PROCEDURE proc_all () RETURN DATETIME
DO
	IF proc_1 () == TRUE THEN
		proc_2 (NULL);

	ELSEIF (proc_3 (NULL) != -1.0) THEN
		ext_proc_1 ();
		proc_2 (NULL);

	END
	
	RETURN NULL;
ENDPROC

