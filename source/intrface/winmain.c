   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
	/*                  A                                */
	/*                   Of The                   */
	/*             Software Technology Branch              */
	/*             NASA - Johnson Space Center             */
	/*                                                     */
	/*               CLIPS Version 6.00  10/01/92          */
	/*                                                     */
   /*                    MAIN MODULE                      */
	/*******************************************************/

/***********************************************************
* Purpose: Initalizing and Maintaining the Interface windows 
*       and main menu items during execution.
*                                                            
* Principal Programmer(s):                                   
*      Christopher J. Ortiz                                  
*                                                            
* Contributing Programmer(s):                                
*      Gary Riley
*                                                      
* Revision History:                                          
*
************************************************************/

/*-------------------------------+
| Windows & System Include Files |
+-------------------------------*/
#include <windows.h>
#undef CopyMemory
#include <stdlib.h>  
#include <ctype.h> // Remove later
#include <stdio.h>
#include <string.h>
#include <time.h> // Remove later

/*------------------------+
| CLIPS 6.0 Include Files |
+------------------------*/
#include "setup.h"
#include "commline.h"
#include "extnfunc.h"
#include "sysdep.h"
#include "utility.h"
#include "engine.h"
#include "memalloc.h"
#include "argacces.h"
#include "router.h"
//#include "memory.h"

/*------------------------+
| Interface Include Files |
+------------------------*/
#include "ids.h"
#include "dialog.h"
#include "commdlg.h"
#include "status.h"
//#include "memory.h"
#include "main.h"
#include "diag_win.h"
#include "menucmds.h"

HCURSOR NowCursor;   /* Current Cursor */
HANDLE hAccTable;
HANDLE hInst= NULL;
HCURSOR ARROW, QUERY;
HCURSOR WAIT[10];
int ShiftKeyDown;
int ApplicationFocus = 0;

/*-----------------+
| Local Procedures |
+-----------------*/
BOOL InitApplication (HANDLE);
BOOL InitInstance( HANDLE, int);
void SetUpRouters ( void );
int  QueryFunction (char*);
int  GetCFunction(char*);
int  SetJoeWarningsCommand ( void );
int  WinWaitEvent ( void );
static void InitializeKeywords( void );
void DefineWinFunctions ( void );
int  ExitFunction ( int );
int  SetCompletionDialogCommand( void );
int  WinOutOfMemoryFunction( unsigned long int );
int PASCAL WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
void WinHelp2 ( void );
void UserFunctions(void);

/***************************************************************
* WinMAIN: Start execution of CLIPS.  This function must be
*   redefined in order to embed CLIPS within another program. 
*   Example of redefined main:
*     main()
*       {                                                     
*        init_clips();                                        
*            .                                                
*            .                                                
*            .                                                
*        process_data();                                      
*        RunCLIPS(-1);
*        evaluate_data();                                     
*            .                                                
*            .                                                
*            .                                                
*        final_results();                                     
*       }                                                     
***************************************************************/
#if IBM_TBC
#pragma argsused
#endif
int PASCAL WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR  lpCmdLine,
	int    nCmdShow)

{  extern HWND hDialogWnd;
	extern HANDLE hAccTable;

	/*----------------------------------+
	| Check Windows Version Information |
	+---------------------------------*/
	{  int Version = LOBYTE(LOWORD( GetVersion()))*100 +
	 HIBYTE(LOWORD( GetVersion()));

		if ( Version < 310 )
		{  MessageBeep (0);
	 MessageBox ( NULL, "CLIPS Needs Windows 3.1 or Higher "," Version Error", MB_ICONHAND | MB_OK );
	 return ( TRUE );
		}
	}

	InitializeCLIPS();

	/*-------------------------------------------------+
   | Load Accelerator table and Initalize Application |
   +-------------------------------------------------*/
   hAccTable = LoadAccelerators (hInstance, "Main_Accelerator");
   if (!hPrevInstance)
      if (!InitApplication(hInstance))
	 return (FALSE);

   /*-------------------+
   | Initalize Instance |
	+-------------------*/
   if (!InitInstance(hInstance, nCmdShow))
      return (FALSE);

   /*--------------------------------+
   | setup routers for the interface |
   +--------------------------------*/
   SetUpRouters ();

   /*-------------------------------+
   | Create Terminal Data Structure |
   +-------------------------------*/
   CreateTerminal ();

   /*----------------------------------+
	| Set up hook between CLIPS command |
   | loop and interface event loop.    |
   +----------------------------------*/
   SetEventFunction((int(*)())WinWaitEvent);

   /*------------------------------------+
   | Add execution functions to update   |
   | the interface between rule firings. |
   +------------------------------------*/
   AddPeriodicFunction("status_wnd", WinRunEvent ,0);
#if DEFRULE_CONSTRUCT
   AddRunFunction ("run_function", WinRunEvent, 0);
#endif

   /*------------------------------------------------------------+
	| Change the functions to be called when CLIPS is out of      |
   | memory, CLIPS preforms a reset, and CLIPS preforms a clear. |
   +------------------------------------------------------------*/
   SetOutOfMemoryFunction ( WinOutOfMemoryFunction );

   /*---------------------------------+
   / Add Interface specific functions |
   +---------------------------------*/
   InitializeKeywords();
   DefineWinFunctions();
   EnableStatusWindows();
   /*------------------------------------------------------+
   | Get message and dispatch it to the appropriate window |
	+------------------------------------------------------*/

	CommandLoop();
	return (TRUE);
}

/*******************************************************************
* InitApplication: function creates & registers window classes for
*    subsequent use in calls to the CreateWindow function.
*    Performs any initialization necessary only for the first
*    executing instance of the application.
*******************************************************************/

BOOL InitApplication(
   HANDLE hInstance)

{  WNDCLASS dialog;
   WNDCLASS status;

   dialog.style = CS_CLASSDC | CS_VREDRAW | CS_HREDRAW;
   dialog.lpfnWndProc = (WNDPROC) DialogWndProc;
   dialog.cbClsExtra = 0;
   dialog.cbWndExtra = 0;
   dialog.hInstance = (HINSTANCE) hInstance;
   dialog.hIcon = LoadIcon((HINSTANCE) hInstance, "Clips_Icon");
   dialog.hCursor = LoadCursor ( NULL, IDC_ARROW);
   dialog.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
   dialog.lpszMenuName ="MainMenu";
   dialog.lpszClassName = "DialogWClass";

   if (!(RegisterClass(&dialog)))
      return (FALSE);

   status.style = CS_CLASSDC | CS_HREDRAW | CS_VREDRAW;
   status.lpfnWndProc = (WNDPROC) StatusWndProc;
   status.cbClsExtra = 0;
   status.cbWndExtra = 0;
   status.hInstance = (HINSTANCE) hInstance;
   status.hIcon = LoadIcon((HINSTANCE) hInstance, "Clips_Icon");
   status.hCursor = NULL;
   status.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
   status.lpszMenuName =  NULL;
   status.lpszClassName = "StatusWClass";

   return (RegisterClass(&status));
}

/***************************************************************
* InitInstance: Creates & Displays all the necessary windows
*   as well as loading all cursor resources. 
*   Performs any initialization necessary for every executing
*   instance of the application.
****************************************************************/

BOOL InitInstance(
   HANDLE hInstance,
   int    nCmdShow)

{  extern HANDLE hInst;        /* Handle to instance data*/
   extern HWND   hInstWnd;     /* Handle to Instance window */
   TEXTMETRIC tm;
   extern SCREEN WinDialog, WinFact;
   extern SCREEN WinAgenda;
   extern SCREEN WinGlobal, WinInst, WinFocus;
   HDC    hDC;
   int width =  GetSystemMetrics ( SM_CXSCREEN );
   int height = GetSystemMetrics ( SM_CYSCREEN );
   RECT ClientRect;

   hInst = hInstance;

#if CLP_HELP
   /*-----------------------+
   | Find the path for Help |
   +-----------------------*/
   {  extern char *help_file;
      char HelpPath [255];
      int size,x ;
      if ( help_file != NULL )
	 genfree ( help_file, strlen (help_file) );
      GetModuleFileName(hInst,(LPSTR) &HelpPath, 254);
      HelpPath [strlen ( HelpPath ) - 12] = '\0';
      strcat ( HelpPath, "CLIPS.HLP\0");
      for ( x=0; x< strlen(HelpPath); x++)
	 if ( HelpPath[x] == '\\' )
	    HelpPath[x] = '/';
      size = strlen( HelpPath ) + 1;
      help_file = (char *) genalloc ( size );
      strncpy ( help_file, HelpPath, size );
   }
#endif

   /*--------------+
   | Dialog Window |
   +--------------*/

   WinDialog.hWnd = CreateWindow( "DialogWClass", "CLIPS 6.10",
      WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CAPTION |  WS_VSCROLL | WS_HSCROLL,
      0,0, (width * .66)-5,(height * .66)-5,
      NULL, NULL, (HINSTANCE) hInstance, (LPSTR) NULL );

   if (!WinDialog.hWnd)
      return (FALSE);

   hDC = GetDC ( WinDialog.hWnd );
   SelectObject (hDC, GetStockObject(SYSTEM_FIXED_FONT));
   GetTextMetrics ( hDC, &tm );
   ReleaseDC ( WinDialog.hWnd, hDC );

   GetClientRect  ( WinDialog.hWnd, (LPRECT) &ClientRect );
   WinDialog.LastLine = 0;
   WinDialog.LineSize = tm.tmHeight+ tm.tmExternalLeading;
   WinDialog.NoLines  = (ClientRect.bottom / WinDialog.LineSize)-1;

   ShowWindow(WinDialog.hWnd, nCmdShow );

#if DEFTEMPLATE_CONSTRUCT
   /*------------+
   | Fact Window |
   +------------*/
   WinFact.hWnd = CreateWindow( "StatusWClass", NULL,
      WS_THICKFRAME | WS_MAXIMIZEBOX | WS_CAPTION |  WS_VSCROLL | WS_HSCROLL,
      0, 0, 0, 0, NULL, NULL, (HINSTANCE) hInstance, NULL);

   if (!WinFact.hWnd)
     return (FALSE);

   hDC = GetDC ( WinFact.hWnd);
   SelectObject (hDC, GetStockObject(SYSTEM_FIXED_FONT));
   GetTextMetrics(hDC, &tm);
   ReleaseDC ( WinFact.hWnd, hDC );
   WinFact.LineSize = tm.tmHeight+ tm.tmExternalLeading;
   WinFact.NoLines = 0;
   ShowWindow(WinFact.hWnd,   SW_HIDE );
#endif

#if OBJECT_SYSTEM
   /*----------------+
   | Instance Window |
   +----------------*/
   WinInst.hWnd = CreateWindow( "StatusWClass", NULL,
      WS_THICKFRAME | WS_MAXIMIZEBOX | WS_CAPTION | WS_VSCROLL | WS_HSCROLL,
      0, 0, 0, 0, NULL, NULL, (HINSTANCE) hInstance, NULL);

   if (!WinInst.hWnd)
      return (FALSE);

   hDC = GetDC ( WinInst.hWnd);
   SelectObject (hDC, GetStockObject(SYSTEM_FIXED_FONT));
   GetTextMetrics(hDC, &tm);
   ReleaseDC ( WinInst.hWnd, hDC );
   WinInst.LineSize = ( tm.tmHeight + tm.tmExternalLeading );
   WinInst.NoLines = 0;
   ShowWindow(WinInst.hWnd,   SW_HIDE );
#endif

#if DEFGLOBAL_CONSTRUCT
   /*---------------+
   | Globals Window |
   +---------------*/
   WinGlobal.hWnd = CreateWindow( "StatusWClass", NULL,
      WS_THICKFRAME | WS_MAXIMIZEBOX | WS_CAPTION | WS_VSCROLL | WS_HSCROLL,
      0, 0, 0, 0, NULL, NULL, (HINSTANCE) hInstance, NULL);

   if (!WinGlobal.hWnd)
      return (FALSE);

   hDC = GetDC ( WinGlobal.hWnd);
   SelectObject (hDC, GetStockObject(SYSTEM_FIXED_FONT));
   GetTextMetrics(hDC, &tm);
   ReleaseDC ( WinGlobal.hWnd, hDC );
   WinGlobal.LineSize = ( tm.tmHeight + tm.tmExternalLeading );
   WinGlobal.NoLines = 0;
   ShowWindow(WinGlobal.hWnd, SW_HIDE );
#endif

#if DEFRULE_CONSTRUCT
   /*--------------+
   | Agenda Window |
   +--------------*/
    WinAgenda.hWnd = CreateWindow( "StatusWClass", NULL,
      WS_THICKFRAME | WS_MAXIMIZEBOX | WS_CAPTION | WS_VSCROLL | WS_HSCROLL,
      0, 0, 0, 0, NULL, NULL, (HINSTANCE) hInstance, NULL);

   if (!WinAgenda.hWnd)
      return (FALSE);

   hDC = GetDC ( WinAgenda.hWnd);
   SelectObject (hDC, GetStockObject(SYSTEM_FIXED_FONT));
   GetTextMetrics(hDC, &tm);
   ReleaseDC ( WinAgenda.hWnd, hDC );
   WinAgenda.LineSize = ( tm.tmHeight + tm.tmExternalLeading );
   WinAgenda.NoLines = 0;
   ShowWindow(WinAgenda.hWnd, SW_HIDE );

   /*-------------+
   | Focus Window |
   +-------------*/
    WinFocus.hWnd = CreateWindow( "StatusWClass", "Focus",
      WS_THICKFRAME | WS_MAXIMIZEBOX | WS_CAPTION | WS_VSCROLL | WS_HSCROLL,
      0, 0, 0, 0, NULL, NULL, (HINSTANCE) hInstance, NULL);

   if (!WinFocus.hWnd)
      return (FALSE);

   hDC = GetDC ( WinFocus.hWnd);
   SelectObject (hDC, GetStockObject(SYSTEM_FIXED_FONT));
   GetTextMetrics(hDC, &tm);
   ReleaseDC ( WinFocus.hWnd, hDC );
   WinFocus.LineSize = ( tm.tmHeight + tm.tmExternalLeading );
   WinFocus.NoLines = 0;
   ShowWindow(WinFocus.hWnd,  SW_HIDE );
#endif

   /*-----+
   | Misc |
   +-----*/
   SetScrollRange ( WinDialog.hWnd, SB_VERT, WinDialog.NoLines-1, WinDialog.NoLines, TRUE );

   WAIT[0] = LoadCursor ( (HINSTANCE) hInst, "CURSOR0" );
   WAIT[1] = LoadCursor ( (HINSTANCE) hInst, "CURSOR1" );
   WAIT[2] = LoadCursor ( (HINSTANCE) hInst, "CURSOR2" );
   WAIT[3] = LoadCursor ( (HINSTANCE) hInst, "CURSOR3" );
   WAIT[4] = LoadCursor ( (HINSTANCE) hInst, "CURSOR4" );
   WAIT[5] = LoadCursor ( (HINSTANCE) hInst, "CURSOR5" );
   WAIT[6] = LoadCursor ( (HINSTANCE) hInst, "CURSOR6" );
   WAIT[7] = LoadCursor ( (HINSTANCE) hInst, "CURSOR7" );
   WAIT[8] = LoadCursor ( (HINSTANCE) hInst, "CURSOR8" );
   WAIT[9] = LoadCursor ( (HINSTANCE) hInst, IDC_WAIT );

   QUERY = LoadCursor ((HINSTANCE) hInst, "CLIPS_QUERY");
   ARROW = LoadCursor ( NULL, IDC_ARROW );
   UpdateCursor ( ARROW_CURSOR );

   return (TRUE);
}

/****************************************************************
* WinOutOfMemoryFunction ( size ): Function which is called when
*   CLIPS runs out of memory.  Clips will not attempt to allocate
*   the memory again and will exit.
*
* size - size of memory request that could not be satisfied.
****************************************************************/
#if IBM_TBC
#pragma argsused
#endif
int WinOutOfMemoryFunction( unsigned long int size )

{    extern SCREEN WinDialog;
     MessageBeep(0);
     MessageBox ( WinDialog.hWnd, "CLIPS is out of memory!!", "-WARNING-", MB_ICONHAND | MB_OK );
     return (1);
}

/****************************************************************
* ExitToShell:  Quit Clips and return to OS*
****************************************************************/

void ExitToShell( void )

{  extern SCREEN WinDialog;
   MessageBeep(0);
   MessageBox (WinDialog.hWnd, "CLIPS is out of memory!!", "-ERROR-", MB_ICONHAND | MB_OK );
   PostMessage (WinDialog.hWnd, WM_COMMAND, IDM_FILE_QUIT, 0);
}

/********************************************************
* SetCompletionDialogCommand: Turns on/off the option for
*   using the Completion WinDialog.
*********************************************************/

int SetCompletionDialogCommand( void )

{  extern BOOL Complete;
   int oldValue;
   DATA_OBJECT argPtr;

   oldValue = Complete;
   if ( ArgCountCheck ("set-completion-dialog", EXACTLY, 1 ) == -1 )
      return ( oldValue );

   RtnUnknown (1,&argPtr );

   if ((argPtr.value == CLIPSFalseSymbol ) && (argPtr.type == SYMBOL))
      Complete = FALSE;
   else
      Complete = TRUE;

   return (oldValue);
}

/********************************************************
* SetJoeWarningsCommand: Turns on/off the option for
*   using the Yes/No Joe warning Dialog.
*********************************************************/

int SetJoeWarningsCommand()
{  extern int warnings;
   int oldvalue;
   DATA_OBJECT ArgPtr;

   oldvalue = warnings;
   if (ArgCountCheck ("set-joe-warnings",EXACTLY,1) == -1)
      return (oldvalue);

   RtnUnknown(1,&ArgPtr);

   if((ArgPtr.value == (void *) CLIPSFalseSymbol) && (ArgPtr.type == SYMBOL))
      warnings = CLIPS_FALSE;
   else
      warnings = CLIPS_TRUE;

   return ( warnings );
}

/*******************************************************************
* DefineWinFunctions: Creates CLIPS command line functions
*   for use of interface options.
*******************************************************************/

void DefineWinFunctions ( void )

{  DefineFunction2 ("clear-window", 'v', PTIF ClearDialogWnd, "ClearDialogWnd", "00" );
   DefineFunction2 ("set-completion-dialog",'b', SetCompletionDialogCommand, "SetCompletionDialogCommand", "11");
   DefineFunction2 ("set-joe-warnings", 'b', SetJoeWarningsCommand, "SetJoeWarningsCommand", "11");
   DefineFunction2 ("winhelp", 'v', PTIF WinHelp2, "WinHelp2", NULL);
}

/***************************************************************
* UserFunctions:  The function which informs CLIPS of any   
*   user defined functions.  In the default case, there are 
*   no user defined functions.  To define functions, either 
*   this function must be replaced by a function with the   
*   same name within this file, or this function can be     
*   deleted from this file and included in another file.    
*   User defined functions may be included in this file or  
*   other files.                                            
*   Example of redefined UserFunctions:                     
*     UserFunctions()                                       
*       {                                                   
*        DefineFunction("fun1",'i',fun1,"fun1");            
*        DefineFunction("other",'f',other,"other");         
*       }                                                   
*
****************************************************************/

void UserFunctions()
{
}

/*******************************************************************
* WinWaitEvent: Executes one pass of the main program loop.
*******************************************************************/

int WinWaitEvent ( void )
{  extern SCREEN WinDialog;
   extern HANDLE hAccTable;
   MSG msg;

   UpdateCursor ( ARROW_CURSOR );
   /*------------------------------------+
   | Update Status window and Menu Items |
   +------------------------------------*/
   UpdateStatus();
   UpdateMenu();

   /*------------------+
   | Handle Next Event |
   +------------------*/
   GetMessage (&msg, NULL, 0, 0 );
   if (!TranslateAccelerator(WinDialog.hWnd, (HACCEL) hAccTable, &msg))
   {  TranslateMessage (&msg);
      DispatchMessage (&msg);
   }

   return (TRUE);
}

/*******************************************************************
* WinRunEvent: Execution function which is called between rule
*    firings by CLIPS to update the interface.
*******************************************************************/

void WinRunEvent ( void )
{  MSG msg;
   extern SCREEN WinDialog;
   extern HANDLE hAccTable;
   extern int ApplicationFocus;

   UpdateMenu();
   UpdateStatus();

   while (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
   {  if (!TranslateAccelerator(WinDialog.hWnd, (HACCEL) hAccTable, &msg))
      {  TranslateMessage (&msg);
	 DispatchMessage (&msg);
      }
   }
}

/****************************************************************/
/* InitializeKeywords: Adds CLIPS key words to the symbol table */
/*   so that they are available for command completion.         */
/****************************************************************/
static void InitializeKeywords()
{
#if ! RUN_TIME
   void *ts;
   /*====================*/
   /* Interface keywords */
   /*====================*/

   ts = AddSymbol("set-completion-dialog");
   IncrementSymbolCount(ts);
   ts = AddSymbol("set-joe-warnings");
   IncrementSymbolCount(ts);
   ts = AddSymbol("clear-window");
   IncrementSymbolCount(ts);
#endif
}

/*******************************************************************
* SetUpRouters:  Creates and Renames I/O routers for use
*    with the Windows Interface
*******************************************************************/

void SetUpRouters ( void )

{  
   AddRouter ("stdin",   20, QueryFunction, PrintFunction, GetCFunction, NULL, ExitFunction);
}

/*******************************************************************
* QueryFunction: Call back function used to recognize logical names
*    for interface defined routers.
*******************************************************************/
int QueryFunction ( char * logicalname )

{  if ( strcmp (logicalname, "wclips")==0)
      return 1;
   else if ( strcmp (logicalname, "stdout")==0)
      return 1;
   else if ( strcmp (logicalname, "stdin")==0)
      return 1;
   else if ( strcmp (logicalname, "wdialog")==0)
      return 1;
   else if ( strcmp (logicalname, "wdisplay")==0)
      return 1;
   else if ( strcmp (logicalname, "wtrace")==0)
      return 1;
   else if ( strcmp (logicalname, "werror")==0)
      return 1;
   else if ( strcmp (logicalname, "wwarning")==0)
      return 1;
   return 0;
}

/*******************************************************************
* ExitFunction: Routine to  check an exit from the dialog window
*  to make sure that the user has an opportunity to save files.
*******************************************************************/
#if IBM_TBC
#pragma argsused
#endif
int ExitFunction ( int exitcode )
{  extern SCREEN WinDialog;
   PostMessage ( WinDialog.hWnd, WM_COMMAND, IDM_FILE_QUIT, 0);
   return (0); 
}


/*******************************************************************
* GetCFunction: Function is called whenever clips needs input
*   from the keyboard.  Function will setup an event loop and
*   will process character input.  Return value is the ASCII
*   code for the character received.
*******************************************************************/
#if IBM_TBC
#pragma argsused
#endif
int GetCFunction ( char * logicalname )
{ MSG msg;
  extern HCURSOR QUERY;
  extern HANDLE hAccTable;
  static int count = 0;

  UpdateCursor ( QUESTION_CURSOR );
  GetMessage ( &msg, NULL,0,0);
  TranslateMessage (&msg);

  while (TRUE)
  {  if (msg.message == WM_CHAR )
     {  switch ( msg.wParam )
	{  case VK_BACK:
			 GetUserCmd ( (WORD) msg.wParam, TRUE, count);
	       count --;
	       if ( count < 0 ) count = 0;
	       msg.wParam = '\b';
	       break;

	    case VK_RETURN:
	       GetUserCmd ( msg.wParam, TRUE, count);
	       count = 0;
	       UpdateCursor ( ARROW_CURSOR );
	       msg.wParam = '\n';
	       break;

	    default:
	       count ++;
	       GetUserCmd ( msg.wParam, TRUE, count);
	       break;
	}
	return (msg.wParam);
     }
     DispatchMessage ( &msg );
     UpdateCursor ( QUESTION_CURSOR );
     GetMessage(&msg, NULL, 0, 0);
     TranslateMessage (&msg);
  }
}

/*******************************************************************
* StartWaitCursor: Function is called to set an event timer
*   used to update the animated wait cursor.
*******************************************************************/
void StartWaitCursor ( void )
{  extern SCREEN WinDialog;
   SetTimer(WinDialog.hWnd, IDT_UPDATEWAIT, 500, (TIMERPROC)NULL);
}

/*******************************************************************
* StopWaitCursor: Function is called to stop the event timer
*   used to update the animated wait cursor.
*******************************************************************/
void StopWaitCursor ( void )
{  extern SCREEN WinDialog;
   extern HCURSOR ARROW;
   KillTimer(WinDialog.hWnd, IDT_UPDATEWAIT);
   NowCursor = ARROW;
   SetCursor ( ARROW);
}

/*******************************************************************
* StopWaitCursor: Function is called everytime the event timer
*   activates to update the wait cursor.
*******************************************************************/
void UpdateCursor ( int Cursor )
{  extern HCURSOR WAIT[], ARROW, QUERY, NowCursor;
   switch ( Cursor )
   {  case ARROW_CURSOR:    NowCursor = ARROW;   break;
      case QUESTION_CURSOR: NowCursor = QUERY;   break;
      case WAIT_CURSOR:     NowCursor = WAIT[0]; break;
   }
   SetCursor ( NowCursor );
}

/*******************************************************************
* WinHelp2: Callback function used to provide a commmand line
*   interface to the Windows Help Facility. 
*******************************************************************/
void WinHelp2 (void)
{  extern SCREEN WinDialog;
   switch ( RtnArgCount () )
   {  case 1:
      {  LPSTR String;
	 String = (LPSTR) RtnLexeme ( 1 );
	 if ( String != NULL )
	 {  WinHelp (WinDialog.hWnd, "CLIPS6.HLP", HELP_KEY, (DWORD) String);
	    break;
	 }
      }
      case 0:
      {  WinHelp (WinDialog.hWnd, "CLIPS6.HLP", HELP_INDEX, 0);
	 break;
      }
      default:
      {  MessageBeep(0);
	 break;
      }
   }
}
