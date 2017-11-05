################################################################################
# This file should be kept empty and it should be use punctually to reproduce  #
# different problems to fix.                                                   #
################################################################################

#@include whais_std.wh

VAR gTestTableSync TABLE (entryId UINT32, rndValue UINT64);
VAR gEntryCount, gRequestsCount UINT32;

EXTERN PROCEDURE HandleSyncTestRequest () RETURN UINT32;

PROCEDURE AddTableRow (failure UINT32) RETURN UINT32
DO
	VAR result UINT32;

	SYNC
		IF (gEntryCount == NULL)
			gEntryCount = 0;

		IF (failure == 0)
			RETURN 1;

		result = table_reusable_row (gTestTableSync);
		gTestTableSync.entryId[result]  = gEntryCount;
		gTestTableSync.rndValue[result] = random (gEntryCount + 1);
		gEntryCount += 1;

		IF (failure == 1)
			RETURN 2;

		#On purpose this ... to verify if the
		#section  locks are disabled in case of internal errors.
		IF (failure == 3)
			RETURN failure / (failure - 3);

		sleep (random (5));
	ENDSYNC

	RETURN result;

ENDPROC

PROCEDURE proxyAddTable_1 () RETURN UINT32
DO
	RETURN AddTableRow (random (5));
ENDPROC

PROCEDURE proxyAddTable_2 () RETURN UINT32
DO
	VAR r UINT32;
	r = AddTableRow (random (5));

	RETURN r;
ENDPROC

PROCEDURE proxyAddTable_3 () RETURN UINT32
DO
	VAR r UINT32;

	SYNC
	r = AddTableRow (random (5));
	ENDSYNC

	RETURN r;
ENDPROC

PROCEDURE proxyAddTable_4 () RETURN UINT32
DO
	VAR r UINT32;

	SYNC
		return  AddTableRow (random (5));
	ENDSYNC
ENDPROC

PROCEDURE HandleSyncTestRequest () RETURN UINT32
DO
	VAR c UINT32;

	SYNC
		IF (gRequestsCount == NULL)
			gRequestsCount = 1;
		ELSE
			gRequestsCount += 1;

		c = gRequestsCount;
	ENDSYNC

	c = random (4) + 1;

	IF (c == 1)
		return proxyAddTable_1 ();

	ELSE IF (c == 2)
		return proxyAddTable_2 ();

	ELSE IF (c == 3)
		return proxyAddTable_3 ();

	ELSE IF (c == 4)
		return proxyAddTable_4 ();
	
	RETURN NULL;
ENDPROC

PROCEDURE SyncStatementsTestSetup () RETURN BOOL
DO
	write_log ("Starting test of sync statements. Reseting the control variables...");

	gTestTableSync = NULL;
	gEntryCount    = NULL;
	gRequestsCount = NULL;

	RETURN TRUE;
ENDPROC

PROCEDURE GetSyncTestTable () RETURN TABLE (entryId UINT32, rndValue UINT64)
DO
	RETURN gTestTableSync;
ENDPROC

PROCEDURE GetSyncTestEntryCount () RETURN UINT32
DO
	RETURN gEntryCount;
ENDPROC

PROCEDURE GetSyncTestRequestsCount () RETURN UINT32
DO
	RETURN gRequestsCount;
ENDPROC


