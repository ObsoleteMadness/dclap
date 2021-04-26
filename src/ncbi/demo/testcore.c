
/*****************************************************************************
*
*   testcore.c
*
*****************************************************************************/
#include <ncbi.h>

#define TEST (CTX_RESERVED+1)

                            /*** our prototyped functions ***/
void BuildBS PROTO((ByteStorePtr bsp));

void TestMessages PROTO((void));
void TestErrors PROTO((void));
void TestSettings PROTO((void));
void TestMemory PROTO((void));
void TestByteStores PROTO((void));
void TestStrings PROTO((void));
void TestMisc PROTO((void));

                            /*** our arguments ***/
#define NUMARGS 8

Args testargs[8] = {
	{ "test Boolean", "T", NULL, NULL, FALSE, 'b', ARG_BOOLEAN, 0.0, 0, NULL },
	{ "test Integer", "42", "41", "43", TRUE, 'i', ARG_INT, 0.0, 0, NULL },
	{ "test Float", "3.14159", NULL, NULL, FALSE, 'f', ARG_FLOAT, 0.0, 0, NULL },
	{ "test String", NULL, NULL, NULL, TRUE, 's', ARG_STRING, 0.0, 0, NULL },
	{ "test File-in", NULL, NULL, NULL, TRUE, 'w', ARG_FILE_IN, 0.0, 0, NULL },
	{ "test File-out", NULL, NULL, NULL, TRUE, 'x', ARG_FILE_OUT, 0.0, 0, NULL },
	{ "test Data-in", NULL, "Fake-type", NULL, TRUE, 'y', ARG_DATA_IN, 0.0, 0, NULL },
	{ "test Data-out", NULL, "Fake-out", NULL, TRUE, 'z', ARG_DATA_OUT, 0.0, 0, NULL }};
  
char * tmsg = "Test of %s",
     * fmsg = "Fail on %s",
	 * omsg = "[overwrite at 50]",
	 * imsg = "[inserted at 10]",
	 * stest[4] = { "The","quick","brown","fox" };


							/*** use Int2 Main(), not main(argc, argv)  ***/
Int2 Main (void)

{
	Char lbuf[100];
	Int4 seconds;
	                                      /*** provide opening arguments ***/
	if (! GetArgs("TestCore 1.0", NUMARGS, testargs))
		return 1;			              /*** leave if nothing happens ***/

	seconds = GetSecs();

	TestErrors ();

	TestMessages ();

	TestSettings ();

	TestMemory ();

	TestByteStores ();

	TestMisc ();

	seconds = GetSecs() - seconds;
	DayTimeStr(lbuf, TRUE, TRUE);
	Message(MSG_OK, "Today is: %s.  Test took %ld seconds.", lbuf, seconds);

	return 0;
}


void TestErrors (void)

{
    remove ("testcore.log");
    ErrSetLog ("testcore.log");

    ERRPOST((CTX_DEBUG, 0, "Test post of a debugging message"));
	ErrSetOpts (0, ERR_LOG_OFF);
	ERRPOST((CTX_DEBUG, 1,
		"If you see this message, logging was not disabled as it should be"));
	ErrSetOpts (0, ERR_LOG_ON);
}


void TestMessages (void)

{
	Message(MSG_ERROR, tmsg, "non-fatal error message");
	Message(MSG_OK, tmsg, "OK message");
	if (Message(MSG_RC, tmsg, "retry/cancel message. Hit c.") != ANS_CANCEL)
		ERRPOST((TEST, 1, "Did not get retry"));
	if (Message(MSG_ARI, tmsg, "abort/retry/ignore message. Hit a.") != ANS_ABORT)
		ERRPOST((TEST, 1, "Did not get abort"));
	if (! Message(MSG_YN, tmsg, "yes/no message. Hit y."))
		ERRPOST((TEST, 1, "Did not get yes"));
}

void TestSettings (void)

{
	char buffer[6];

	if ( ! SetAppParam ("junk", "test", "key", "value"))
		ERRPOST((TEST, 1, fmsg, "SetAppParam"));

	GetAppParam ("junk", "test", "key", NULL, buffer, sizeof buffer);
	if (strcmp(buffer, "value"))
		ERRPOST((TEST, 1, fmsg, "GetAppParam"));

	GetAppParam ("junk", "test", "foo", "foo", buffer, sizeof buffer);
	if (strcmp(buffer, "foo"))
		ERRPOST((TEST, 1, "GetAppParam: default value not returned"));

	if ( ! TransientSetAppParam ("junk", "test", "foo", "foo2"))
		ERRPOST((TEST, 1, "TransientSetAppParam"));
	GetAppParam ("junk", "test", "foo", "foo", buffer, sizeof buffer);
	if (strcmp(buffer, "foo2"))
		ERRPOST((TEST, 1, "GetAppParam: TransientSetAppParam value not returned"));
	if ( ! TransientSetAppParam ("junk", "test", NULL, NULL))
		ERRPOST((TEST, 1, "TransientSetAppParam section"));

	GetAppParam ("junk", "test", "key", "foo", buffer, sizeof buffer);
	if (strcmp(buffer, "foo"))
		ERRPOST((TEST, 1, fmsg, "GetAppParam on empty section"));

	if ( ! SetAppParam ("junk", "test", "key", "val2"))
		ERRPOST((TEST, 1, fmsg, "SetAppParam"));

	GetAppParam ("junk", "test", "key", NULL, buffer, sizeof buffer);
	if (strcmp(buffer, "val2"))
		ERRPOST((TEST, 1, fmsg, "GetAppParam on new value"));
}

void TestMemory (void)

{
	CharPtr cpnt;
	Handle hand;
										  /*** test some memory ***/

	if ((hand = HandNew((size_t)60000)) == NULL)
		ERRPOST((TEST, 1, fmsg, "HandNew"));
	if ((cpnt = (CharPtr) HandLock(hand)) == NULL)
		ERRPOST((TEST, 1, fmsg, "HandLock"));
	HandUnlock(hand);
	HandFree(hand);

	if ((cpnt = MemNew((size_t)60000)) == NULL)
		ERRPOST((TEST, 1, fmsg, "MemNew"));

}

void TestByteStores (void)

{
	CharPtr cpnt = NULL;
	ByteStorePtr bsp;
	Char lbuf[100];
	Int2 ctr;

	if ((bsp = BSNew(60000)) == NULL)
		ERRPOST((TEST, 1, fmsg, "BSNew"));

										 /*** test ByteStore ***/
	BuildBS(bsp);                        /*** fill 60k ***/
	if (BSLen(bsp) != 60000)
		Message(MSG_ERROR, fmsg, "BSLen");

	BSSeek(bsp, 31980, SEEK_SET);
	if (BSTell(bsp) != 31980)
		Message(MSG_ERROR, fmsg, "BSTell");
	MemFill(lbuf, '\0', 100);      /*** BSRead does not add '\0' at end */
	BSRead(bsp, lbuf, 50);
	Message(MSG_OK, "BS50  [%s]", lbuf);        /*** show 31980-32029 ***/

	BSSeek(bsp, 31990, SEEK_SET);
	BSDelete(bsp, 20);                          /*** show delete at 31990 **/
	BSSeek(bsp, 31980, SEEK_SET);
	MemFill(lbuf, '\0', 100);      /*** BSRead does not add '\0' at end */
	BSRead(bsp, lbuf, 50);
	Message(MSG_OK, "BSdel [%s]", lbuf);

	BSSeek(bsp, 50, SEEK_SET);                 /*** show overwrite at 50 **/
	BSWrite(bsp, omsg, StringLen(omsg));
	BSSeek(bsp,40, SEEK_SET);
	MemFill(lbuf, '\0', 100);      /*** BSRead does not add '\0' at end */
	BSRead(bsp, lbuf, 50);
	Message(MSG_OK, "BSow [%s]", lbuf);

	BSSeek(bsp, 10, SEEK_SET);                 /*** show insert at 10 **/
	BSInsert(bsp, imsg, StringLen(imsg));
	BSSeek(bsp, 0, SEEK_SET);
	MemFill(lbuf, '\0', 100);      /*** BSRead does not add '\0' at end */
	BSRead(bsp, lbuf, 50);
	Message(MSG_OK, "BS i [%s]", lbuf);

	BSSeek(bsp, 0, SEEK_SET);                  /*** show getbyte ***/
	MemFill(lbuf, '\0', 100);      /*** BSRead does not add '\0' at end */
	for (ctr = 0; ctr < 50; ctr++)
		lbuf[ctr] = (Byte) BSGetByte(bsp);
	Message(MSG_OK, "BS g [%s]", lbuf);

								   /*** check freeing of memory ***/
	cpnt = (CharPtr) MemFree(cpnt);
	cpnt = (CharPtr) BSMerge(bsp, NULL);       /*** check merge ***/
	MemFill(lbuf, '\0', 100);
	MemCopy(lbuf, cpnt, 50);
	Message(MSG_OK, "BS m [%s]", lbuf);
	bsp = BSFree(bsp);             /*** free the ByteStore */
	cpnt = (CharPtr) MemFree(cpnt);

								   /*** check add ***/
	bsp = BSNew(0);                /*** no initial storage allocated ***/
	for (ctr = 0; ctr < 4; ctr++)
		BSWrite(bsp, stest[ctr], StringLen(stest[ctr]));
	MemFill(lbuf, '\0', 100);      /*** merge doesn't add '\0' to previous */
															 /*   storage **/
	BSMerge(bsp, lbuf);            /*** merge to previous storage ***/
	Message(MSG_OK, "BS add [%s]", lbuf);
	bsp = BSFree(bsp);


}

/*****************************************************************************
*
*   BuildBS(bsp);
*
*****************************************************************************/
void BuildBS (ByteStorePtr bsp)

{
	Char lbuf[20], tbuf[20];
	Int4 value;

	StringCpy(lbuf, "....:....|");
	for (value = 0; value < 60000; value += 10)
	{
		sprintf(tbuf, "%ld", value);
		MemCopy(lbuf, tbuf, StringLen(tbuf));
		BSWrite(bsp, lbuf, 10);
	}
	return;
}

void TestStrings (void)

{
	CharPtr cpnt;
	Char lbuf[100];
	Int2 ctr;
								   /*** some string functions ***/
	for (ctr = 0; ctr < 4; ctr++)
	{
		if (StringCmp("brown", stest[ctr]))
		{
			if (ctr == 2)
				Message(MSG_ERROR, fmsg, "StringCmp");
		}
		else if (ctr != 2)
			Message(MSG_ERROR, fmsg, "StringCmp2");
	}

	for (ctr = 0; ctr < 4; ctr++)
	{
		if (StringICmp("Brown", stest[ctr]))
		{
			if (ctr == 2)
				Message(MSG_ERROR, fmsg, "StringICmp");
		}
		else if (ctr != 2)
			Message(MSG_ERROR, fmsg, "StringICmp2");
	}

	for (ctr = 0; ctr < 4; ctr++)
	{
		if (StringNCmp("brow", stest[ctr], 4))
		{
			if (ctr == 2)
				Message(MSG_ERROR, fmsg, "StringNCmp");
		}
		else if (ctr != 2)
			Message(MSG_ERROR, fmsg, "StringNCmp2");
	}

	for (ctr = 0; ctr < 4; ctr++)
	{
		if (StringNICmp("Brow", stest[ctr], 4))
		{
			if (ctr == 2)
				Message(MSG_ERROR, fmsg, "StringNICmp");
		}
		else if (ctr != 2)
			Message(MSG_ERROR, fmsg, "StringNICmp2");
	}

	MemFill(lbuf, '\0', 100);
	for (ctr = 0; ctr < 4; ctr++)
		StringCat(lbuf, stest[ctr]);
	Message(MSG_OK, "StringCat [%s]", lbuf);

	MemFill(lbuf, '\0', 100);
	for (ctr = 0; ctr < 4; ctr++)
		StringNCat(lbuf, stest[ctr], 4);
	Message(MSG_OK, "StringNCat 4 [%s]", lbuf);

	cpnt = StringSave(lbuf);
	StringCpy(lbuf, cpnt);   /* copy to local buf for MSWindows */
	Message(MSG_OK, "StringSave [%s]", lbuf);
	cpnt = (CharPtr) MemFree(cpnt);
}

void TestMisc (void)

{
										  /*** test ncbistd.h ***/
	if (MIN(0.8, 1.0) != 0.8)
		Message(MSG_ERROR, fmsg, "MIN");
	if (MAX(0.8, 1.0) != 1.0)
		Message(MSG_ERROR, fmsg, "MAX");
	if (ABS(-2) != 2)
		Message(MSG_ERROR, fmsg, "ABS");
	if (ROUNDUP(24, 5) != 25)
		Message(MSG_ERROR, fmsg, "ROUNDUP");
}


