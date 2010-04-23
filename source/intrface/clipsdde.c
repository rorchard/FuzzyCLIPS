   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*               CLIPS Version 6.00  01/01/93          */
   /*                                                     */
   /*                     DDE MODULE                      */
   /*******************************************************/

/**************************************************************/
/* Purpose: For Starting, Maintaining and Stopping a Dymanic  */
/*       Data Exchange (DDE) conversation with a client.      */
/*                                                            */
/* Principal Programmer(s):                                   */
/*      Christopher J. Ortiz                                  */
/*                                                            */
/* Contributing Programmer(s):                                */
/*                                                            */
/* Revision History:                                          */
/*                                                            */
/**************************************************************/

/*-------------------------------+
| Windows & System Include Files |
+-------------------------------*/
#include <windows.h>
#undef CopyMemory
#include <string.h>

/*------------------------+
| CLIPS 6.0 Include Files |
+------------------------*/
#include "setup.h"
#include "clips.h"
#include "router.h"
#include "commline.h"
#include "memalloc.h"
#include "filecom.h"
#include "strngrtr.h"
#include "cstrcpsr.h"

/*------------------------+
| Interface Include Files |
+------------------------*/
#include "ids.h"
#include "main.h"
#include "menucmds.h"
#include "clipsdde.h"

/*----------------------+
| DDE Prep for a Server |
+---------------------*/
HSZ   hszService  = NULL;
HSZ   hszComplete = NULL;
HSZ   hszBatch    = NULL; 
HSZ   hszLoad     = NULL;
HSZ   hszItem     = NULL;
HCONV hConvApp    = NULL;
DWORD idInst      = 0L;
FARPROC lpDdeProc = NULL;

/*************************************************************
* DDECallBack : This function is an application-defined dynamic
*    data exchange (DDE) callback function that processes DDE
*    transactions sent to the function as a result of DDE
*    Management Library (DDEML) calls by other applications.
*************************************************************/
#if IBM_TBC
#pragma argsused
#endif
HDDEDATA EXPENTRY  DDECallBack
(  WORD wType,
   WORD wFmt,
   HCONV hConv,
   HSZ hsz1,
   HSZ hsz2,
   HDDEDATA hData,
   DWORD dwData1,
   DWORD dwData2
)
{  extern HSZ hszService;
   extern SCREEN WinDialog;

   switch (wType)
   {  case XTYP_CONNECT:
      {  if ( hsz2 != hszService )
	    return ((HDDEDATA) FALSE);
	 break;
      }

      /*--------------------------------------------------+
      | Get completed command and return result to client |
      +--------------------------------------------------*/
      case XTYP_REQUEST:
      {  extern char CompleteString[255];
	 extern HSZ hszItem;
	 extern DWORD idInst;
	 hData = NULL;
	 if ( hsz1 == hszComplete )
		 hData = DdeCreateDataHandle (idInst,
			 (unsigned char *) CompleteString,
			 strlen(CompleteString)+1,
			 0L, hszItem,
			 CF_TEXT,0);
	 return ( hData );
      }

      case XTYP_POKE:
      {  extern HSZ hszComplete, hszBatch, hszLoad;
	 extern SCREEN WinDialog;

	 /*------------------------+
	 | Editor Complete Command |
	 +------------------------*/
	 if ( hsz1 == hszComplete )
	 {  char *buffer;
	    BYTE far *Data = DdeAccessData ( hData, NULL );
	    buffer = (char *) GetCommandCompletionString ((char*)Data, strlen((char*)Data) );
	    DdeUnaccessData(hData);

	    if ( buffer == NULL)
	    {  extern char CompleteString[255];
	       MessageBeep(0);
	       CompleteString[0] = '\0';
	    }
	    else
	    {  DoCommandCompletion ( buffer, 0 );
	    }
	    return ((HDDEDATA) DDE_FACK);
	 }

	 SetFocus ( WinDialog.hWnd );
	 /*-----------------------+
	 | Editor Batch Selection |
	 +-----------------------*/
	 if (( hsz1 == hszBatch) || (hsz1 == hszLoad ))
	 {
		 DWORD size;
		 HWND hWnd;
		 char *TheData;
		 char *Data;

		 if( EvaluatingTopLevelCommand || BatchActive() )
			 return ((HDDEDATA) DDE_FBUSY  );

		 Data = (char *) DdeAccessData ( hData, NULL);
		 size = strlen((char *) Data) + 1;

		 TheData = (char *) genalloc ( (unsigned) size );
	    DdeGetData ( hData, (LPBYTE)TheData, size, 0L );

	    if ( hsz1 == hszBatch )
	    {  OpenStringBatch ("BatchSelection", TheData, FALSE );
	       DdeUnaccessData(hData);
	    }
	    else
	    {  PrintCLIPS ("wclips","Loading Selection...\n");
	       FlushCommandString();
	       OpenStringSource ("clipread", TheData, 0);
	       SetPrintWhileLoading(TRUE);
	       LoadConstructsFromLogicalName ("clipread");
	       SetPrintWhileLoading(FALSE);
	       CloseStringSource("clipread");
	       PrintCLIPS ("stdout", "CLIPS> ");
	       DdeUnaccessData(hData);
			 genfree (TheData, (unsigned) size);
	    }

	    InvalidateRect ( WinDialog.hWnd, NULL, TRUE );
	    hWnd = FindWindow("ClipsEditWClass", NULL);
	    SetFocus (hWnd);
	    return ((HDDEDATA) DDE_FACK);
	 }

	 return ((HDDEDATA) NULL);
      }
   }
   return ( (HDDEDATA) TRUE );
}

/***************************************************************
* StartUpDDE: The function registers an application with the DDEML,
*   creates all strings, registers the service names that a DDE
*   server supports. 
***************************************************************/

BOOL StartUpDDE ( void )
{  extern HSZ hszService, hszItem, hszComplete, hszBatch, hszLoad;
   extern HANDLE hInst;
   extern FARPROC lpDdeProc;
   extern DWORD idInst;

   lpDdeProc = MakeProcInstance ( (FARPROC) DDECallBack, hInst );
   if (DdeInitialize((LPDWORD) &idInst, (PFNCALLBACK)lpDdeProc, APPCLASS_STANDARD, 0L))
      return ( FALSE );
   hszService  = DdeCreateStringHandle ( idInst, "CLIPS", CP_WINANSI );
   hszComplete = DdeCreateStringHandle ( idInst, "COMPLETE", CP_WINANSI );
   hszLoad     = DdeCreateStringHandle ( idInst, "LOAD", CP_WINANSI );
   hszBatch    = DdeCreateStringHandle ( idInst, "BATCH", CP_WINANSI );
   hszItem     = DdeCreateStringHandle ( idInst, "DDEData", CP_WINANSI );
   DdeNameService (idInst, hszService, (HSZ)NULL, DNS_REGISTER );
   return ( TRUE );
}

/************************************************************
* ShutDownDDE: This function terminates a conversation,
*    invalidates the given conversation handle, and frees
*    string handles in the calling application.
*************************************************************/

void ShutDownDDE ( void )
{  extern HSZ hszService, hszItem, hszComplete, hszBatch, hszLoad;
   extern HCONV hConvApp;
   extern DWORD idInst;

   if ( hConvApp != NULL)
   {  DdeDisconnect ( hConvApp );
      hConvApp = NULL;
   }
   DdeNameService (idInst, hszService, (HSZ)NULL, DNS_UNREGISTER );
   DdeFreeStringHandle ( idInst, hszService );
   DdeFreeStringHandle ( idInst, hszComplete);
   DdeFreeStringHandle ( idInst, hszBatch   );
   DdeFreeStringHandle ( idInst, hszLoad    );
   DdeFreeStringHandle ( idInst, hszItem    );
}

/***************************************************************
* QuitDDE: This function frees all DDEML resources associated
*   with the calling application and frees the specified function
*   from the data segment bound to it by the StartUpDDE function.
***************************************************************/
#if IBM_TBC
#pragma argsused
#endif
void QuitDDE ( void )
{  extern DWORD idInst;
   extern FARPROC lpDdeProc;
   FreeProcInstance(lpDdeProc);
   DdeUninitialize (idInst);
}
