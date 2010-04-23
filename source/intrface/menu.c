   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*               CLIPS Version 6.00  10/01/92          */
   /*                                                     */
   /*                     MENU MODULE                     */
   /*******************************************************/

/**************************************************************/
/* Purpose: Contains the callback functions for items on the  */
/*      main menu.                                            */
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
#include <stdlib.h>
#include <string.h>

/*------------------------+
| CLIPS 6.0 Include Files |
+------------------------*/
#include "setup.h"
#include "commline.h"
#include "filecom.h"
#include "router.h"
#include "agenda.h"
#include "evaluatn.h"
#include "dffctdef.h"
#include "genrccom.h"
#include "defins.h"
#include "classcom.h"
#include "engine.h"
#include "dffnxfun.h"
#include "globldef.h"
#include "tmpltdef.h"

/*------------------------+
| Interface Include Files |
+------------------------*/
#include "ids.h"
#include "clipsdde.h"
#include "dialog.h"
#include "main.h"
#include "menucmds.h"
#include "diag_win.h"

/*----------------+
| Local Functions |
+----------------*/
void DoExecutionChoice ( HWND, WORD );
void DoWindowChoice (HWND, WORD);
void DoBrowseChoice (HWND, WORD);
void DoFileChoice   (HWND, WORD);
void DoHelpChoice   (HWND, WORD);
void DoModuleChoice (HMENU, WORD);

char CompleteString[255];

/*******************************************************************
* DialogWndProc: Call back function to handle the main menu.
*******************************************************************/

long FAR PASCAL DialogWndProc(
   HWND     hMain,
   unsigned message,
   WPARAM     wParam,
   LONG     lParam)

{  switch (message)
   {  case WM_COMMAND:
      {  switch ( LOWORD(wParam) )
	 {  extern HMENU hMenu;

	    /*----------+
	    | File Menu |
	    +----------*/
	    case IDM_FILE_LOAD:
	    case IDM_FILE_LBATCH:
	    case IDM_FILE_LBINARY:
	    case IDM_FILE_EDITOR:
	    case IDM_FILE_DRIBBLE:
	    case IDM_FILE_SBINARY:
	    case IDM_FILE_QUIT:
	    case IDM_EDIT_PASTE:
	       DoFileChoice ( hMain, LOWORD(wParam) );
	       break;

	    /*------------+
	    | Browse Menu |
	    +------------*/
	    case IDM_BROWSE_RULE:
	    case IDM_BROWSE_TEMPLATE:
	    case IDM_BROWSE_FACTS:
	    case IDM_BROWSE_FUNCTION:
	    case IDM_BROWSE_GLOBAL:
	    case IDM_BROWSE_GENERIC:
	    case IDM_BROWSE_CLASS:
	    case IDM_BROWSE_INSTANCES:
	    case IDM_BROWSE_AGENDA:
	       DoBrowseChoice ( hMain, LOWORD(wParam) );
	       break;

	    /*------------+
	    | Window Menu |
	    +------------*/
	    case IDM_WIN_FACTS:
	    case IDM_WIN_AGENDA:
	    case IDM_WIN_FOCUS:
	    case IDM_WIN_INSTANCE:
	    case IDM_WIN_GLOBALS:
	    case IDM_WIN_ALL:
	    case IDM_WIN_NONE:
	    case IDM_WIN_CLEAR:
	    case IDC_WIN_MINIMIZE:
	    case IDC_WIN_MAXIMIZE:
	       DoWindowChoice (hMain, LOWORD(wParam));
	       break;

	    /*---------------+
	    | Execution Menu |
	    +---------------*/
	    case IDM_EXE_PREFERENCES:
	    case IDM_EXE_OPTIONS:
	    case IDM_EXE_WATCH:
	    case IDM_EXE_RESET:
	    case IDM_EXE_RUN:
	    case IDM_EXE_HALT:
	    case IDM_EXE_HALT_NOW:
	    case IDM_EXE_STEP:
	    case IDM_EXE_CLEAR:
	       DoExecutionChoice ( hMain, LOWORD(wParam) );
	       break;

	    /*----------+
	    | Help Menu |
	    +----------*/
	    case IDM_HELP_ABOUT:
	    case IDM_HELP_COMPLETE:
	    case IDM_HELP_CLIPS:
	       DoHelpChoice ( hMain, LOWORD(wParam));
	       break;

	    /*------------+
	    | Module Menu |
	    +------------*/
	    default:
	    {  if ( wParam >= IDM_MODULE_ONE )
	       {  HMENU hMenu = GetMenu (hMain);
		  DoModuleChoice ( hMenu, LOWORD(wParam) );
	       }
	       break;
	    }

	 }
	 break;
      } // WM_COMMAND

      default:
	 return DoWindowMessage ( hMain, message, wParam, lParam );
   }
   return ( 0 );
}

/***************************************************
* DoExecutionChoice: Process all menu options from
*    the EXECUTION menu.
****************************************************/

void DoExecutionChoice ( 
   HWND hMain,
   WORD wParam)

{  extern HANDLE hInst;

   switch (wParam)
   {   case IDM_EXE_PREFERENCES:
		{  FARPROC lpOptionDlg;
	 lpOptionDlg = MakeProcInstance ((FARPROC) OptionDlgProc, hInst);
	 DialogBox ( (HINSTANCE) hInst, "OptionDlg", hMain, (DLGPROC) lpOptionDlg);
	 FreeProcInstance (lpOptionDlg);
	 break;
      }

      case IDM_EXE_OPTIONS:
		{  FARPROC lpProcOption;
	 lpProcOption = MakeProcInstance((FARPROC) ExecDlg, hInst);
	 DialogBox((HINSTANCE) hInst,"Exec_Option", hMain, (DLGPROC) lpProcOption);
	 FreeProcInstance(lpProcOption);
	 break;
      }

      case IDM_EXE_WATCH:
		{  FARPROC lpWatchDlg;
	 lpWatchDlg = MakeProcInstance ((FARPROC) WatchDlgProc, hInst);
	 DialogBox ((HINSTANCE) hInst, "WatchDlg", hMain, (DLGPROC) lpWatchDlg);
	 FreeProcInstance (lpWatchDlg);
	 break;
      }

      case IDM_EXE_CLEAR:
      {  if ( JoeWarnings ("OK to clear CLIPS environment?"))
	 {  PrintCLIPS ("wclips","(clear)\n");
	    UpdateCursor ( WAIT_CURSOR );
	    Clear ( );
	    PrintPrompt();
	    UpdateCursor ( ARROW_CURSOR );
	 }
	 break;
      }

      case IDM_EXE_RESET:
      {  if ( GetNextActivation(NULL)!=NULL)
	 {  if (!JoeWarnings ("There are activations on the agenda.  OK to reset the CLIPS environment?"))
		break;
	 }
	 PrintCLIPS ("wclips","(reset)\n");
	 UpdateCursor ( WAIT_CURSOR );
	 Reset ( );
	 PrintPrompt();
	 UpdateCursor ( ARROW_CURSOR );
	 break;
      }

      case IDM_EXE_RUN:
      {  PrintCLIPS ("wclips","(run)\n");
	 SetCommandString(    "(run)\n");
	 break;
      }

      case IDM_EXE_HALT:
      {  extern int ShiftKeyDown;
	 if ( !ShiftKeyDown )
	 {  if ( ExecutingRule != NULL )
	      HaltRules = CLIPS_TRUE;
	    else
	    {  SetHaltExecution(CLIPS_TRUE);
	       CloseAllBatchSources();
	    }
	    break;
	 }
      }

      case IDM_EXE_HALT_NOW:
      {  SetHaltExecution ( CLIPS_TRUE );
	 CloseAllBatchSources();
	 break;
      }

      case IDM_EXE_STEP:
      {  extern int rulestep;
	 char Command[12];
	 sprintf (Command,"(run %3d)\n",rulestep);
	 PrintCLIPS ("wclips",Command);
	 SetCommandString(Command);
	 break;
      }
   }
}

/***************************************************
* DoWindowChoice: Process all menu options from
*    the WINDOW menu.
****************************************************/
void DoWindowChoice (
   HWND hMain,
   WORD wParam)
{  extern SCREEN WinDialog;
   switch (wParam)
   {  case IDM_WIN_FACTS:
      {
#if DEFTEMPLATE_CONSTRUCT
	 extern SCREEN WinFact;
	 HMENU hMenu = GetMenu(hMain);
	 if ( GetMenuState (hMenu, wParam, MF_BYCOMMAND ) == MF_CHECKED )
	 {  ShowWindow ( WinFact.hWnd, SW_HIDE );
	     CheckMenuItem (hMenu, wParam, MF_BYCOMMAND | MF_UNCHECKED );
	 }
	 else
	 {  int width =  GetSystemMetrics ( SM_CXSCREEN );
	    int height = GetSystemMetrics ( SM_CYSCREEN );
	    MoveWindow( WinFact.hWnd, (width * 0.66), 0, (width * 0.33), (height * 0.33 )-5, TRUE);
	    ShowWindow ( WinFact.hWnd, SW_SHOW );
	    CheckMenuItem ( hMenu, wParam, MF_BYCOMMAND | MF_CHECKED );
	 }
#endif
	 break;
      }

      case IDM_WIN_AGENDA:
      {
#if DEFRULE_CONSTRUCT
	 extern SCREEN WinAgenda;
	 HMENU hMenu = GetMenu(hMain);
	 if ( GetMenuState (hMenu, wParam, MF_BYCOMMAND ) == MF_CHECKED )
	 {  ShowWindow ( WinAgenda.hWnd, SW_HIDE );
	    CheckMenuItem (hMenu, wParam, MF_BYCOMMAND | MF_UNCHECKED );
	 }
	 else
	 {  int width =  GetSystemMetrics ( SM_CXSCREEN );
	    int height = GetSystemMetrics ( SM_CYSCREEN );
	    MoveWindow( WinAgenda.hWnd, 0, (height * 0.66),(width * 0.4)-5, (height * 0.33), TRUE);
	    ShowWindow ( WinAgenda.hWnd, SW_SHOW );
	    CheckMenuItem ( hMenu, wParam, MF_BYCOMMAND | MF_CHECKED );
	 }
#endif
	 break;
      }

      case IDM_WIN_INSTANCE:
      {
#if OBJECT_SYSTEM
	 extern SCREEN WinInst;
	 HMENU hMenu = GetMenu(hMain);
	 if ( GetMenuState (hMenu, wParam, MF_BYCOMMAND ) == MF_CHECKED )
	 {  ShowWindow ( WinInst.hWnd, SW_HIDE );
	    CheckMenuItem ( hMenu, wParam, MF_BYCOMMAND | MF_UNCHECKED );
	 }
	 else
	 {  int width =  GetSystemMetrics ( SM_CXSCREEN );
	    int height = GetSystemMetrics ( SM_CYSCREEN );
	    MoveWindow( WinInst.hWnd, (width * 0.66), (height * 0.33),(width * 0.33), (height * 0.33)-5, TRUE);
	    ShowWindow ( WinInst.hWnd, SW_SHOW );
	    CheckMenuItem ( hMenu, wParam, MF_BYCOMMAND | MF_CHECKED );
	 }
#endif
	 break;
      }

      case IDM_WIN_GLOBALS:
      {
#if DEFGLOBAL_CONSTRUCT
	 HMENU hMenu = GetMenu(hMain);
	 extern SCREEN WinGlobal;
	 if ( GetMenuState (hMenu, wParam, MF_BYCOMMAND ) == MF_CHECKED )
	 {  ShowWindow ( WinGlobal.hWnd, SW_HIDE );
	    CheckMenuItem ( hMenu, wParam, MF_BYCOMMAND | MF_UNCHECKED );
	 }
	 else
	 {  int width =  GetSystemMetrics ( SM_CXSCREEN );
	    int height = GetSystemMetrics ( SM_CYSCREEN );
	    MoveWindow( WinGlobal.hWnd, (width * 0.66), (height * 0.66 ), (width * 0.33), (height * 0.33 ), TRUE);
	    ShowWindow ( WinGlobal.hWnd, SW_SHOW );
	    CheckMenuItem ( hMenu, wParam, MF_BYCOMMAND | MF_CHECKED );
	 }
#endif
	 break;
      }

      case IDM_WIN_FOCUS:
      {
#if DEFRULE_CONSTRUCT
	 HMENU hMenu = GetMenu(hMain);
	 extern SCREEN WinFocus;
	 if ( GetMenuState (hMenu, wParam, MF_BYCOMMAND ) == MF_CHECKED )
	 {  ShowWindow ( WinFocus.hWnd, SW_HIDE );
	    CheckMenuItem ( hMenu, wParam, MF_BYCOMMAND | MF_UNCHECKED );
	 }
	 else
	 {  int width =  GetSystemMetrics ( SM_CXSCREEN );
	    int height = GetSystemMetrics ( SM_CYSCREEN );
	    MoveWindow( WinFocus.hWnd, (width * 0.4), (height * 0.66 ), (width * 0.26)-5, (height * 0.33 ), TRUE);
	    ShowWindow ( WinFocus.hWnd, SW_SHOW );
	    CheckMenuItem ( hMenu, wParam, MF_BYCOMMAND | MF_CHECKED );
	 }
#endif
	 break;
      }

      case IDM_WIN_ALL:
      {  HMENU hMenu = GetMenu(hMain);
	 CheckMenuItem ( hMenu, IDM_WIN_GLOBALS,  MF_BYCOMMAND | MF_UNCHECKED );
	 CheckMenuItem ( hMenu, IDM_WIN_FACTS,    MF_BYCOMMAND | MF_UNCHECKED );
	 CheckMenuItem ( hMenu, IDM_WIN_INSTANCE, MF_BYCOMMAND | MF_UNCHECKED );
	 CheckMenuItem ( hMenu, IDM_WIN_AGENDA,   MF_BYCOMMAND | MF_UNCHECKED );
	 CheckMenuItem ( hMenu, IDM_WIN_FOCUS,    MF_BYCOMMAND | MF_UNCHECKED );
	 SendMessage (hMain, WM_COMMAND, IDM_WIN_FACTS, 0);
	 SendMessage (hMain, WM_COMMAND, IDM_WIN_AGENDA, 0);
	 SendMessage (hMain, WM_COMMAND, IDM_WIN_INSTANCE, 0);
	 SendMessage (hMain, WM_COMMAND, IDM_WIN_GLOBALS, 0);
	 SendMessage (hMain, WM_COMMAND, IDM_WIN_FOCUS, 0);
	 break;
      }

      case IDC_WIN_MAXIMIZE:
      {  extern SCREEN WinFact, WinFocus, WinGlobal, WinAgenda, WinInst;
	 HMENU hMenu = GetMenu(hMain);
	 int CMD = MF_BYCOMMAND;
	 int CHK = MF_CHECKED;

	 if( GetMenuState (hMenu, IDM_WIN_GLOBALS, CMD)==CHK)
	    ShowWindow ( WinGlobal.hWnd, SW_SHOW );

	 if( GetMenuState (hMenu, IDM_WIN_FACTS, CMD)==CHK)
	    ShowWindow ( WinFact.hWnd, SW_SHOW );

	 if( GetMenuState (hMenu, IDM_WIN_INSTANCE, CMD)==CHK)
	    ShowWindow ( WinInst.hWnd, SW_SHOW );

	 if( GetMenuState (hMenu, IDM_WIN_AGENDA, CMD)==CHK)
	    ShowWindow ( WinAgenda.hWnd, SW_SHOW );

	 if( GetMenuState (hMenu, IDM_WIN_FOCUS, CMD)==CHK)
	    ShowWindow ( WinFocus.hWnd, SW_SHOW );
	 break;
      }

      case IDC_WIN_MINIMIZE:
      {  extern SCREEN WinFact, WinFocus, WinGlobal, WinAgenda, WinInst;
	 ShowWindow ( WinFact.hWnd, SW_HIDE );
	 ShowWindow ( WinFocus.hWnd, SW_HIDE );
	 ShowWindow ( WinGlobal.hWnd, SW_HIDE );
	 ShowWindow ( WinAgenda.hWnd, SW_HIDE );
	 ShowWindow ( WinInst.hWnd, SW_HIDE );
	 break;
      }

      case IDM_WIN_NONE:
      {  HMENU hMenu = GetMenu(hMain);

	 CheckMenuItem ( hMenu, IDM_WIN_GLOBALS,  MF_BYCOMMAND | MF_CHECKED );
	 CheckMenuItem ( hMenu, IDM_WIN_FACTS,    MF_BYCOMMAND | MF_CHECKED );
	 CheckMenuItem ( hMenu, IDM_WIN_INSTANCE, MF_BYCOMMAND | MF_CHECKED );
	 CheckMenuItem ( hMenu, IDM_WIN_AGENDA,   MF_BYCOMMAND | MF_CHECKED );
	 CheckMenuItem ( hMenu, IDM_WIN_FOCUS,    MF_BYCOMMAND | MF_CHECKED );
	 SendMessage (hMain, WM_COMMAND, IDM_WIN_FOCUS, 0);
	 SendMessage (hMain, WM_COMMAND, IDM_WIN_FACTS, 0);
	 SendMessage (hMain, WM_COMMAND, IDM_WIN_AGENDA, 0);
	 SendMessage (hMain, WM_COMMAND, IDM_WIN_INSTANCE, 0);
	 SendMessage (hMain, WM_COMMAND, IDM_WIN_GLOBALS, 0);
	 break;
      }

      case IDM_WIN_CLEAR:
      {  ClearDialogWnd();
	 PrintPrompt ();
	 break;
      }
   }
   SetFocus(WinDialog.hWnd);
}

/***************************************************
* DoBrowseChoice: Process all menu options from
*    the BROWSE menu.
****************************************************/
void DoBrowseChoice (
   HWND hMain,
   WORD wParam)

{  extern HANDLE hInst;
   switch ( wParam )
   {
#if DEFRULE_CONSTRUCT
      case IDM_BROWSE_RULE:
      {  FARPROC lpProc;

	 lpProc = MakeProcInstance((FARPROC) DefRuleManager, hInst);
	 DialogBox((HINSTANCE) hInst,"List_Manager", hMain, (DLGPROC) lpProc);
	 FreeProcInstance(lpProc);
	 break;
      }

      case IDM_BROWSE_AGENDA:
      {  FARPROC lpProc;
	 lpProc = MakeProcInstance((FARPROC) AgendaManager, hInst);
	 DialogBox((HINSTANCE) hInst,"Agenda_Manager", hMain, (DLGPROC) lpProc);
	 FreeProcInstance(lpProc);
	 break;
      }
#endif

#if DEFGLOBAL_CONSTRUCT
      case IDM_BROWSE_GLOBAL:
      {  FARPROC lpProc;

	 lpProc = MakeProcInstance((FARPROC) DefGlobalManager, hInst);
	 DialogBox((HINSTANCE) hInst,"List_Manager", hMain, (DLGPROC) lpProc);
	 FreeProcInstance(lpProc);
	 break;
      }
#endif

#if DEFTEMPLATE_CONSTRUCT
      case IDM_BROWSE_TEMPLATE:
      {  FARPROC lpProc;

	 lpProc = MakeProcInstance((FARPROC) DefTemplateManager, hInst);
	 DialogBox((HINSTANCE) hInst,"List_Manager", hMain, (DLGPROC) lpProc);
	 FreeProcInstance(lpProc);
	 break;
      }
#endif

#if DEFFACTS_CONSTRUCT
      case IDM_BROWSE_FACTS:
      {  FARPROC lpProc;

	 lpProc = MakeProcInstance((FARPROC) DefFactManager, hInst);
	 DialogBox((HINSTANCE) hInst,"List_Manager", hMain, (DLGPROC) lpProc);
	 FreeProcInstance(lpProc);
	 break;
      }
#endif

#if DEFFUNCTION_CONSTRUCT
      case IDM_BROWSE_FUNCTION:
      {  FARPROC lpProc;
	 lpProc = MakeProcInstance((FARPROC) DefFunctionManager, hInst);
	 DialogBox((HINSTANCE) hInst,"List_Manager", hMain, (DLGPROC) lpProc);
	 FreeProcInstance(lpProc);
	 break;
      }
#endif

#if DEFGENERIC_CONSTRUCT
      case IDM_BROWSE_GENERIC:
      {  FARPROC lpProc;
	 lpProc = MakeProcInstance((FARPROC) DefGenericManager, hInst);
	 DialogBox((HINSTANCE) hInst,"List_Manager", hMain, (DLGPROC) lpProc);
	 FreeProcInstance(lpProc);
	 break;
      }
#endif

#if OBJECT_SYSTEM
      case IDM_BROWSE_CLASS:
      {  FARPROC lpProc;
	 lpProc = MakeProcInstance((FARPROC) DefClassManager, hInst);
	 DialogBox((HINSTANCE) hInst,"List_Manager", hMain, (DLGPROC) lpProc);
	 FreeProcInstance(lpProc);
	 break;
      }

      case IDM_BROWSE_INSTANCES:
      {  FARPROC lpProc;
	 lpProc = MakeProcInstance((FARPROC) DefInstanceManager, hInst);
	 DialogBox((HINSTANCE) hInst,"List_Manager", hMain, (DLGPROC) lpProc);
	 FreeProcInstance(lpProc);
	 break;
      }
#endif
   }
}

/***************************************************
* DoFileChoice: Process all menu options from
*    the FILE menu.
****************************************************/

void DoFileChoice ( 
   HWND hMain,
   WORD wParam)

{  extern HANDLE hInst;

   switch (wParam)
   {  /*----------------------------+
      | Startup Editor DDEML Client |
      +----------------------------*/
      case IDM_FILE_EDITOR:
      {  UINT wReturn;
	 char szMsg[80];

	 {
	  wReturn = WinExec("CLIPSedt.exe", SW_SHOW);
	    if (wReturn < 32)
	    {  switch (wReturn)
	       {  case 0: sprintf (szMsg, "Not Enough Memory."); break;
		  case 1:
		  case 2: sprintf (szMsg, "%s %s","File not Found.",
		       "Make sure the CLIPS directory is in your DOS Path.");
		       break;
		  case 8: sprintf (szMsg, "Not Enough Memory."); break;
		  case 16:
		  {  HWND hWnd = FindWindow("ClipsEditWClass", NULL);
		     SetFocus (hWnd);
		     break;
		  }

		  default:
		      sprintf(szMsg, "WinExec failed; error code = %d", wReturn);
		      break;
	       }
	       if ( wReturn != 16 )
		  MessageBox (NULL, szMsg, "Load Error", MB_ICONHAND | MB_OK );
	    }
	 }
	 break;
      } 

      /*------------------------+
      | Open/Close Dribble File |
      +------------------------*/
      case IDM_FILE_DRIBBLE:
      {  OpenDribbleFile ( hMain, wParam );
	 break;
      }

      /*-------------------------+
      | Save File as Binary File |
      +-------------------------*/
      case IDM_FILE_SBINARY:
      {  SaveBinaryFile ( hMain, wParam );
	 break;
      }

      /*-----------------------------------+
      | Load a file (CLP, Binary or Batch) |
      +-----------------------------------*/
      case IDM_FILE_LOAD:
      case IDM_FILE_LBATCH:
      case IDM_FILE_LBINARY:
      {  DoLoad ( hMain, wParam );
	 break;
      }

      /*---------------+
      | Start Shutdown |
      +---------------*/
      case IDM_FILE_QUIT:
      {  extern SCREEN WinFact, WinGlobal, WinInst, WinAgenda, WinDialog;
	 HWND hEditor = FindWindow("ClipsEditWClass", NULL);

	 if ( hEditor != NULL )
	    PostMessage ( hEditor, WM_COMMAND, wParam, 0 );

	 WinHelp ( WinDialog.hWnd, "CLIPS6.HLP", HELP_QUIT, 0);
	 PrintCLIPS ("wclips","(exit)\n");
	 DestroyWindow ( hMain );
	 DestroyWindow ( WinFact.hWnd );
	 DestroyWindow ( WinGlobal.hWnd);
	 DestroyWindow ( WinInst.hWnd );
	 DestroyWindow ( WinAgenda.hWnd );
	 QuitDDE ( );
	 SetCommandString("(exit)\n");

	 ExitCLIPS(0);
      }

      /*---------------+
      | Start Shutdown |
      +---------------*/
      case IDM_EDIT_PASTE:
      {  extern SCREEN WinDialog;
	 HANDLE hData;
	 LPSTR  pData;
	 int x;

	 OpenClipboard ( WinDialog.hWnd );
	 hData = GetClipboardData (CF_TEXT );
	 pData = (char *) GlobalLock ( hData );

	 for (x=0; x < strlen ( pData ); x++ )
	   if ( pData[x] == '\r' )
	      pData[x] = ' ';
	 SetCommandString(pData);
	 PrintCLIPS ("stdout", pData);

	 GlobalUnlock ( hData );
	 CloseClipboard ( );
	 break;
      }
   }
}

/**************************************************
* DoHelpChoice: Process all menu options from
*    the FILE menu.
**************************************************/

void DoHelpChoice (
   HWND hMain,
   WORD wParam)

{
  switch (wParam)
   {   /*------------------+
       | Show About WinDialog |
       +------------------*/
       case IDM_HELP_ABOUT:
       {  extern HANDLE hInst;
	  FARPROC lpProc;
	  lpProc = MakeProcInstance((FARPROC) AboutDlg, hInst);
	  DialogBox((HINSTANCE) hInst,"AboutBox", hMain, (DLGPROC) lpProc);
	  FreeProcInstance(lpProc);
	  break;
       }

       case IDM_HELP_CLIPS:
       {  extern SCREEN WinDialog;
	  WinHelp ( WinDialog.hWnd, "CLIPS6.HLP", HELP_INDEX, 0);
	  break;
       }

       /*---------------------------+
       | Do Command Complete Dialog |
       +---------------------------*/
      case IDM_HELP_COMPLETE:
		{  char *buffer;
	 int  length;
	 extern char CompleteString[255];

	 buffer = GetCommandString();         

	 if ( buffer == NULL)
	 {  MessageBeep(0);
		 break;
	 }

	 length = strlen (buffer);
	 buffer = GetCommandCompletionString ( buffer, length );

	 if ( buffer == NULL)
	 {  MessageBeep(0);
		 break;
	 }

	 length = strlen ( buffer );

	 if ( DoCommandCompletion ( buffer, 1))
	 {  AppendCommandString (&(CompleteString[length]));
	    AppendCommandString (" ");
	    PrintCLIPS ("wclips",(&(CompleteString[length])));
	    PrintCLIPS ("wclips"," ");
	 }
      }
   }
}

/*******************************************************************
* DoModuleChoice: Sets the current module based on the
*    menu item input.
*******************************************************************/

void DoModuleChoice ( 
   HMENU hMenu,
   WORD wParam)

{  char moduleName[255];
   GetMenuString ( hMenu, wParam, (LPSTR)&moduleName, 255, MF_BYCOMMAND);
   SetCurrentModule (FindDefmodule((char *)moduleName));
}

/***********************************************************************
* DoModuleChoice: Will create the all popup menu items for Module Menu.
***********************************************************************/

void UpdateModuleMenu ( 
   HMENU ModuleMenu)

{  char moduleTitle[255];
   int position;
   struct defmodule *theModule;
   int value = GetMenuItemCount ( ModuleMenu );

   /*-----------------------------------------------+
   | Delete all Menu Items in the Module Popup Menu |
   +-----------------------------------------------*/
   while ( value )
   {  DeleteMenu (ModuleMenu, 0, MF_BYPOSITION);
      value = GetMenuItemCount ( ModuleMenu );
   }

   /*-----------------------------------------------+
   | Create a new menu item for each defined module |
   +-----------------------------------------------*/
   for ( theModule = (struct defmodule *) GetNextDefmodule(NULL), position = IDM_MODULE_ONE ;
      theModule != NULL;
      theModule = (struct defmodule *) GetNextDefmodule(theModule), position ++)
   {  strncpy ((char *)moduleTitle,GetDefmoduleName(theModule),255);
      moduleTitle[255] = '\0';
      InsertMenu (ModuleMenu, -1, MF_BYPOSITION|MF_STRING, position, moduleTitle);
      if (theModule == GetCurrentModule())
	 CheckMenuItem (ModuleMenu, position, MF_BYCOMMAND|MF_CHECKED);
   }
}

/************************************************************
* UpdateMenu: Function which will enable/disable interface
*   menu items based on rule execution and other factors. 
************************************************************/

void UpdateMenu ( )

{  extern SCREEN WinDialog;
   HMENU hMenu = GetMenu(WinDialog.hWnd );
   extern int ShiftKeyDown;
   /*----------------------------------+
   | Value - Flag used to execute code |
   | once for each wait/active state.  |
   +----------------------------------*/
   static int value = 0;

   if (EvaluatingTopLevelCommand || BatchActive() )
   {  if ( value )
      {  value = 0;

	 /*---------------------------------+
	 | Gray File, Execute & Browse Menu |
	 | items when a rule is being fired |
	 +---------------------------------*/
	 EnableMenuItem(hMenu,IDM_FILE_LOAD,   MF_BYCOMMAND|MF_GRAYED);
	 EnableMenuItem(hMenu,IDM_FILE_LBINARY,MF_BYCOMMAND|MF_GRAYED);
	 EnableMenuItem(hMenu,IDM_FILE_SBINARY,MF_BYCOMMAND|MF_GRAYED);
	 EnableMenuItem(hMenu,IDM_FILE_LBATCH, MF_BYCOMMAND|MF_GRAYED);
	 EnableMenuItem(hMenu,IDM_FILE_QUIT,   MF_BYCOMMAND|MF_GRAYED);
	 EnableMenuItem(hMenu,IDM_EDIT_PASTE,  MF_BYCOMMAND|MF_GRAYED);

	 EnableMenuItem(hMenu,IDM_HELP_COMPLETE, MF_BYCOMMAND|MF_GRAYED);
	 EnableMenuItem(hMenu,IDM_WIN_CLEAR,   MF_BYCOMMAND|MF_GRAYED);

	 EnableMenuItem(hMenu,IDM_EXE_RESET,   MF_BYCOMMAND|MF_GRAYED);
	 EnableMenuItem(hMenu,IDM_EXE_STEP,    MF_BYCOMMAND|MF_GRAYED);
	 EnableMenuItem(hMenu,IDM_EXE_CLEAR,   MF_BYCOMMAND|MF_GRAYED);

	 EnableMenuItem(hMenu,IDM_BROWSE_RULE,     MF_BYCOMMAND|MF_GRAYED);
	 EnableMenuItem(hMenu,IDM_BROWSE_FACTS,    MF_BYCOMMAND|MF_GRAYED);
	 EnableMenuItem(hMenu,IDM_BROWSE_TEMPLATE, MF_BYCOMMAND|MF_GRAYED);
	 EnableMenuItem(hMenu,IDM_BROWSE_FUNCTION, MF_BYCOMMAND|MF_GRAYED);
	 EnableMenuItem(hMenu,IDM_BROWSE_GENERIC,  MF_BYCOMMAND|MF_GRAYED);
	 EnableMenuItem(hMenu,IDM_BROWSE_GLOBAL,   MF_BYCOMMAND|MF_GRAYED);
	 EnableMenuItem(hMenu,IDM_BROWSE_CLASS,    MF_BYCOMMAND|MF_GRAYED);
	 EnableMenuItem(hMenu,IDM_BROWSE_INSTANCES,MF_BYCOMMAND|MF_GRAYED);
	 EnableMenuItem(hMenu,IDM_BROWSE_AGENDA,   MF_BYCOMMAND|MF_GRAYED);

	 if (DeleteMenu(hMenu,IDM_EXE_RUN, MF_BYCOMMAND ))
	    InsertMenu (hMenu,IDM_EXE_STEP,MF_BYCOMMAND,IDM_EXE_HALT,"&Halt \tCtrl+C");
	 DrawMenuBar (WinDialog.hWnd);

	 StartWaitCursor();
      }
      else
      {  /*--------------------------------+
	 | Update Halt and Halt Now menu   |
	 | items based on shift key status.|
	 +--------------------------------*/
	 if ( ShiftKeyDown )
	 {  if (DeleteMenu(hMenu,IDM_EXE_HALT, MF_BYCOMMAND ))
	    { InsertMenu (hMenu,IDM_EXE_STEP,MF_BYCOMMAND,IDM_EXE_HALT_NOW,"&Halt Now!!\tCtrl+Shift+C");
	      DrawMenuBar (WinDialog.hWnd);
	    }
	 }
	 else
	 {  if (DeleteMenu(hMenu,IDM_EXE_HALT_NOW, MF_BYCOMMAND ))
	    {  InsertMenu (hMenu,IDM_EXE_STEP,MF_BYCOMMAND,IDM_EXE_HALT,"&Halt");
	       DrawMenuBar (WinDialog.hWnd);
	    }
	 }
      }
   }
   else
   {   if (!value)
       {  value = 1;
	  UpdateCursor ( ARROW_CURSOR );

	  /*-------------------------------------+
	  | Enable File, Execute & Browse Menu   |
	  | items when a rule is not being fired.|
	  +-------------------------------------*/
	  EnableMenuItem(hMenu,IDM_FILE_LOAD,   MF_BYCOMMAND|MF_ENABLED);
	  EnableMenuItem(hMenu,IDM_FILE_LBINARY,MF_BYCOMMAND|MF_ENABLED);
	  EnableMenuItem(hMenu,IDM_FILE_SBINARY,MF_BYCOMMAND|MF_ENABLED);
	  EnableMenuItem(hMenu,IDM_FILE_LBATCH, MF_BYCOMMAND|MF_ENABLED);
	  EnableMenuItem(hMenu,IDM_FILE_QUIT,   MF_BYCOMMAND|MF_ENABLED);

	  EnableMenuItem(hMenu,IDM_HELP_COMPLETE, MF_BYCOMMAND|MF_ENABLED);

	  EnableMenuItem(hMenu,IDM_EXE_RESET,   MF_BYCOMMAND|MF_ENABLED);
	  EnableMenuItem(hMenu,IDM_EXE_STEP,    MF_BYCOMMAND|MF_ENABLED);
	  EnableMenuItem(hMenu,IDM_EXE_CLEAR,   MF_BYCOMMAND|MF_ENABLED);
	  EnableMenuItem(hMenu,IDM_WIN_CLEAR,   MF_BYCOMMAND|MF_ENABLED);

	  if (DeleteMenu(hMenu,IDM_EXE_HALT, MF_BYCOMMAND ))
	    InsertMenu (hMenu, IDM_EXE_STEP, MF_BYCOMMAND, IDM_EXE_RUN, "&Run\tCtrl+R" );
	  else if (DeleteMenu(hMenu,IDM_EXE_HALT_NOW, MF_BYCOMMAND ))
	    InsertMenu (hMenu, IDM_EXE_STEP, MF_BYCOMMAND, IDM_EXE_RUN, "&Run\tCtrl+R" );

	  DrawMenuBar (WinDialog.hWnd);
	  StopWaitCursor ();
       }

       /*-----------------------------------+
       | Update Browse Menu Items if needed |
       +-----------------------------------*/

       /*----------------+
       | PASTE Menu Item |
       +----------------*/
       if ( IsClipboardFormatAvailable (CF_TEXT))
	  EnableMenuItem(hMenu,IDM_EDIT_PASTE, MF_BYCOMMAND|MF_ENABLED);
       else
	  EnableMenuItem(hMenu,IDM_EDIT_PASTE, MF_BYCOMMAND|MF_GRAYED);

       /*-----------------+
       | AGENDA Menu Item |
       +-----------------*/
       if (GetNextActivation(NULL) != NULL )
	  EnableMenuItem(hMenu,IDM_BROWSE_AGENDA,MF_BYCOMMAND|MF_ENABLED);
       else
	  EnableMenuItem(hMenu,IDM_BROWSE_AGENDA,MF_BYCOMMAND|MF_GRAYED);

#if DEFGLOBAL_CONSTRUCT
       /*------------------+
       | DEFFACT Menu Item |
       +------------------*/
       if ( GetNextDefglobal (NULL) != NULL)
	  EnableMenuItem(hMenu,IDM_BROWSE_GLOBAL,MF_BYCOMMAND|MF_ENABLED);
       else
	  EnableMenuItem(hMenu,IDM_BROWSE_GLOBAL,MF_BYCOMMAND|MF_GRAYED);
#endif

#if DEFFACTS_CONSTRUCT
       /*------------------+
       | DEFFACT Menu Item |
       +------------------*/
       if ( GetNextDeffacts (NULL) != NULL)
	  EnableMenuItem(hMenu,IDM_BROWSE_FACTS,MF_BYCOMMAND|MF_ENABLED);
       else
	  EnableMenuItem(hMenu,IDM_BROWSE_FACTS,MF_BYCOMMAND|MF_GRAYED);
#endif

#if DEFRULE_CONSTRUCT
       /*------------------+
       | DEFRULE Menu Item |
       +------------------*/
       if (GetNextDefrule(NULL)!=NULL)
	  EnableMenuItem(hMenu,IDM_BROWSE_RULE,MF_BYCOMMAND|MF_ENABLED);
       else
	  EnableMenuItem(hMenu,IDM_BROWSE_RULE,MF_BYCOMMAND|MF_GRAYED);
#endif

#if DEFTEMPLATE_CONSTRUCT
       /*----------------------+
       | DEFTEMPLATE Menu Item |
       +----------------------*/
       if (GetNextDeftemplate(NULL) != NULL)
	  EnableMenuItem(hMenu,IDM_BROWSE_TEMPLATE, MF_BYCOMMAND|MF_ENABLED);
       else
	  EnableMenuItem(hMenu,IDM_BROWSE_TEMPLATE, MF_BYCOMMAND|MF_GRAYED);
#endif

#if DEFFUNCTION_CONSTRUCT
       /*----------------------+
       | DEFFUNCTION Menu Item |
       +----------------------*/
       if (GetNextDeffunction(NULL) != NULL)
	  EnableMenuItem(hMenu,IDM_BROWSE_FUNCTION, MF_BYCOMMAND|MF_ENABLED);
       else
	  EnableMenuItem(hMenu,IDM_BROWSE_FUNCTION, MF_BYCOMMAND|MF_GRAYED);
#endif

#if DEFGENERIC_CONSTRUCT
       /*---------------------+
       | DEFGENERIC Menu Item |
       +---------------------*/
       if ( GetNextDefgeneric(NULL)!= NULL)
	  EnableMenuItem(hMenu,IDM_BROWSE_GENERIC,  MF_BYCOMMAND|MF_ENABLED);
       else
	  EnableMenuItem(hMenu,IDM_BROWSE_GENERIC,  MF_BYCOMMAND|MF_GRAYED);
#endif

#if OBJECT_SYSTEM
#if DEFINSTANCES_CONSTRUCT
       /*----------------------+
       | DEFINSTANCE Menu Item |
       +----------------------*/
       if (GetNextDefinstances (NULL) != NULL )
	  EnableMenuItem(hMenu,IDM_BROWSE_INSTANCES,MF_BYCOMMAND|MF_ENABLED);
       else
	  EnableMenuItem(hMenu,IDM_BROWSE_INSTANCES,MF_BYCOMMAND|MF_GRAYED);
#endif

       /*-------------------+
       | DEFCLASS Menu Item |
       +-------------------*/
       if ( GetNextDefclass(NULL) != NULL )
	  EnableMenuItem(hMenu,IDM_BROWSE_CLASS,    MF_BYCOMMAND|MF_ENABLED);
       else
	  EnableMenuItem(hMenu,IDM_BROWSE_CLASS,    MF_BYCOMMAND|MF_GRAYED);
#endif
   }
}
