   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*               CLIPS Version 6.00  10/01/92          */
   /*                                                     */
   /*                  MENU COMMANDS MODULE               */
   /*******************************************************/

/**************************************************************/
/* Purpose: Execute File/IO commands from the main menu as    */
/*   well as the command completion procedures.               */
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
#include "commdlg.h"
#include <string.h>

/*------------------------+
| CLIPS 6.0 Include Files |
+------------------------*/
#include "setup.h"
#include "commline.h"
#include "filecom.h"
#include "strngrtr.h"
#include "router.h"
#include "engine.h"
#include "bsave.h"
#include "bload.h"
#include "cstrcpsr.h"

/*------------------------+
| Interface Include Files |
+------------------------*/
#include "menucmds.h"
#include "ids.h"
#include "main.h"
#include "dialog.h"

/*******************************************************************
* DoLoad: Function will display the common file dialog and will load
*    a clips rule file, clips binary file, or a clips batch file.
*******************************************************************/

long DoLoad ( HWND hMain, WORD LoadType)

{  OPENFILENAME ofn;
   extern HANDLE hInst;
   char File[256], FileTitle[256], Filter[256];
   UINT i, cbString;
   char Replace;

   File[0] = '\0';
   memset ( &ofn,0, sizeof (OPENFILENAME));

   /*---------------------------------------------+
   | Initialize Common Dialog based on File Type. |
   +---------------------------------------------*/
   switch ( LoadType )
   {  case IDM_FILE_LOAD:
      {  ofn.lpstrTitle = "Load CLIPS Constructs";
         if ((cbString = LoadString ( (HINSTANCE) hInst, IDS_RULES, Filter, sizeof (Filter))) == 0 )
            return 0L;
	 break;
      }

      case IDM_FILE_LBATCH:
      {  ofn.lpstrTitle = "Load CLIPS Batch File";
         if ((cbString = LoadString ( (HINSTANCE) hInst, IDS_BATCH, Filter, sizeof (Filter))) == 0 )
	    return 0L;
	 break;
      }

      case IDM_FILE_LBINARY:
      {  ofn.lpstrTitle = "Load CLIPS Binary File";
         if ((cbString = LoadString ( (HINSTANCE) hInst, IDS_BINARY, Filter, sizeof (Filter))) == 0 )
	    return 0L;
         break;
      }
   }

   Replace = Filter[cbString-1];

   for (i=0; Filter[i] != '\0'; i++)
      if ( Filter[i] == Replace)
         Filter[i] = '\0';

   ofn.lStructSize = sizeof ( OPENFILENAME );
   ofn.hwndOwner = hMain;
   ofn.lpstrFilter = Filter;
   ofn.nFilterIndex = 1;
   ofn.lpstrFile = File;
   ofn.nMaxFile = sizeof (File );
   ofn.lpstrFileTitle = FileTitle;
   ofn.nMaxFileTitle = sizeof (FileTitle);
   ofn.lpstrInitialDir = NULL;
   ofn.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;


   /*--------------------+
   | Issue CLIPS Command |
   +--------------------*/
   if ( GetOpenFileName ( &ofn ) )
   {  int x, size;

      UpdateCursor(WAIT_CURSOR);
      /*-----------------+
      | Adjust Path Info |
      +-----------------*/
      size = strlen(ofn.lpstrFile );
      for ( x=0; x<size ; x++)
         if ( ofn.lpstrFile [x] == '\\' )
            ofn.lpstrFile [x] = '/';

      switch ( LoadType )
      {  case IDM_FILE_LOAD:
            SetCommandString("(load \"");  break;
         case IDM_FILE_LBATCH:
            SetCommandString("(batch \""); break;
         case IDM_FILE_LBINARY:
            SetCommandString("(bload \""); break;
      }
      AppendCommandString ( ofn.lpstrFile );
      AppendCommandString ( "\")\n");
      PrintCLIPS ( "stdout", GetCommandString());
      UpdateCursor ( ARROW_CURSOR );
      PrintPrompt();
   }
   return 0L;
}

/*******************************************************************
* DoCommandCompletion: This function will complete a specified string.
*******************************************************************/

BOOL DoCommandCompletion( 
  char *buffer,
  int fromEditor)
  {
   int numberOfMatches = 0;
   extern struct symbolMatch *matches;
   extern int HorizScroll;
   extern char CompleteString[255];
	int RtnValue = TRUE;
	int commonPrefixLength = 0;

   HorizScroll = 1;
	matches = FindSymbolMatches ( buffer, &numberOfMatches,
											&commonPrefixLength );

	switch ( numberOfMatches )
	{  case 0:
			CompleteString[0] = '\0';
	 MessageBeep(0);
         RtnValue = FALSE;
         break;

		case 1:
		{  strncpy ( CompleteString, (matches->match->contents), 255 );
	 break;
      }

      default:
      {  extern SCREEN WinDialog;
	 extern HANDLE hInst;
	 extern BOOL Complete;
	 DLGPROC lpProc;

         /* Option from Preferences */
	 if ( fromEditor && !Complete )
	 {  MessageBeep(0);
	    return (FALSE);
         }

	 lpProc = MakeProcInstance((DLGPROC) CommandComplete, hInst);
	 if (DialogBoxParam((HINSTANCE) hInst,"List_Manager", WinDialog.hWnd, lpProc,(LPARAM)buffer) == IDC_CANCEL)
	    RtnValue = FALSE;
      }
	}

	ReturnSymbolMatches(matches);
   
   return (RtnValue);
}

/*******************************************************************
* OpenDribbleFile: Function will display the common file dialog and
*    will open a dribble file.
******************************************************************/
void OpenDribbleFile ( HWND hMain, WORD wParam )
{  extern HANDLE hInst;
   OPENFILENAME ofn;
   char File[256], FileTitle[256], Filter[256];
   UINT i, cbString;
   char Replace;
   HMENU hMenu = GetMenu(hMain);

   if ( !DribbleActive() )
   {  File[0] = '\0';
      memset ( &ofn,0, sizeof (OPENFILENAME));
      ofn.lpstrTitle = "Select CLIPS Dribble File";
      if ((cbString = LoadString ( (HINSTANCE) hInst, IDS_DRIBBLE, Filter, sizeof (Filter))) == 0 )
         return;

      Replace = Filter[cbString-1];
      for (i=0; Filter[i] != '\0'; i++)
         if ( Filter[i] == Replace)
	    Filter[i] = '\0';

      ofn.lStructSize = sizeof ( OPENFILENAME );
      ofn.hwndOwner = hMain;
      ofn.lpstrFilter = Filter;
      ofn.nFilterIndex = 1;
      ofn.lpstrFile = File;
      ofn.nMaxFile = sizeof (File );
      ofn.lpstrFileTitle = FileTitle;
      ofn.nMaxFileTitle = sizeof (FileTitle);
      ofn.lpstrInitialDir = NULL;
      ofn.Flags = OFN_OVERWRITEPROMPT;

      if ( GetSaveFileName ( &ofn ) )
      {  UpdateCursor ( WAIT_CURSOR );
         ModifyMenu (hMenu, wParam, MF_BYCOMMAND, wParam, "Turn Dribble Off");
         DribbleOn ( ofn.lpstrFile );
      }
   }
   else
   {  UpdateCursor ( WAIT_CURSOR );
      ModifyMenu (hMenu, wParam, MF_BYCOMMAND, wParam, "Turn Dribble On...");
      DribbleOff( );
   }
}

/*******************************************************************
* SaveBinaryFile: Function will display the common file dialog and
*    will save a CLIPS binary file.
******************************************************************/
#if IBM_TBC
#pragma argsused
#endif
void SaveBinaryFile ( HWND hMain, WORD wParam )

{  extern HANDLE hInst;
   char File[256], FileTitle[256], Filter[256];
   UINT i, cbString;
   char Replace;
   OPENFILENAME ofn;

   File[0] = '\0';
   memset ( &ofn,0, sizeof (OPENFILENAME));
   ofn.lpstrTitle = "Save CLIPS File as Binary";
   if ((cbString = LoadString ( (HINSTANCE) hInst, IDS_BINARY, Filter, sizeof (Filter))) == 0 )
      return;

   Replace = Filter[cbString-1];

   for (i=0; Filter[i] != '\0'; i++)
     if ( Filter[i] == Replace)
        Filter[i] = '\0';

   ofn.lStructSize = sizeof ( OPENFILENAME );
   ofn.hwndOwner = hMain;
   ofn.lpstrFilter = Filter;
   ofn.nFilterIndex = 1;
   ofn.lpstrFile = File;
   ofn.nMaxFile = sizeof (File );
   ofn.lpstrFileTitle = FileTitle;
   ofn.nMaxFileTitle = sizeof (FileTitle);
   ofn.lpstrInitialDir = NULL;
   ofn.Flags = OFN_OVERWRITEPROMPT;

   if ( GetSaveFileName(&ofn))
   {  UpdateCursor (WAIT_CURSOR);
      Bsave(ofn.lpstrFile );
      UpdateCursor (ARROW_CURSOR);
   }
}
