   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*               CLIPS Version 6.00  10/01/92          */
   /*                                                     */
   /*                    MAIN MODULE                      */
   /*******************************************************/

/**************************************************************/
/* Purpose: Maintains all functions for all I/O in the main   */
/*       Dialog Window.                                       */
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
#include <ctype.h>
#include <string.h>
//#include <alloc.h>

/*------------------------+
| CLIPS 6.0 Include Files |
+------------------------*/
#include "setup.h"
#include "commline.h"
#include "scanner.h"
#include "router.h"
#include "factbld.h"
#include "agenda.h"

/*------------------------+
| Interface Include Files |
+------------------------*/
#include "ids.h"
#include "diag_win.h"
#include "main.h"
#include "clipsdde.h"
#include "menucmds.h"

SCREEN WinDialog;

int OldLine = 0;
int HorizScroll = 0;

char **Terminal;

void SendToScreen( void );
void FreeTerminalText ( void );
void DeleteTerminal ( void );
void GetUserCmd ( WORD,BOOL,int);

/*********************************************************************
* CreateTerminal: Initialize the structure which will hold all
*   the text for the terminal.
*********************************************************************/
void CreateTerminal ( void )

{
	int i;
	Terminal = (char **) malloc (sizeof(char *)*(DIALOG_SIZE+1));
   if ( Terminal == NULL )
      ExitToShell();
   for (i=0; i<=(DIALOG_SIZE); i++)
      Terminal[i] = NULL;
}

/*********************************************
* Free all text associated with the Terminal *
**********************************************/
void FreeTerminalText ( void )
{ 
	int i;
	for (i=0; i<=(DIALOG_SIZE); i++)
	{  if ( Terminal[i] != NULL )
	 free(Terminal[i]);
      Terminal[i] = NULL;
	}
}

/*********************************************
* Free all text associated with the Terminal *
**********************************************/
void DeleteTerminal ( void )
{  
	FreeTerminalText();
   free(Terminal);
}

/*********************************************************************
* GetUserCmd: Function used to filter/display characters typed in
*    from the keyboard.   
*********************************************************************/

void GetUserCmd ( 
  WORD wParam,      /* Key Code */
  BOOL ScreenOnly,  /* Send to Screen and or Command Buffer */
  int  InputSize)   /* Number of characters send to screen only */

  {
	extern int HorizScroll; /* Automatic Scrolling Enabled/Disabled */

	switch (wParam)
	  {
		extern SCREEN WinDialog;
		extern int UserInput;

		/*------------------+
		| Handle Back Space |
		+------------------*/

		case VK_BACK:
		  {
			/*-----------------------------------------------+
			| Init Values when sending to the command buffer |
			+-----------------------------------------------*/
			if (! ScreenOnly)
			  {
				HorizScroll = 1;
				if (GetCommandString() != NULL)
				  { InputSize = strlen(GetCommandString()); }
				else
				  { InputSize = 0; }
			  }

	 if (InputSize > 0 )
	 {
		 int size;

		 if (Terminal[WinDialog.LastLine] != NULL)
			{ size = strlen(Terminal[WinDialog.LastLine]); }
		 else
			{ size = 0; }

		 if (!ScreenOnly)
			 ExpandCommandString ('\b');

		 if (size > 0)
			 Terminal[WinDialog.LastLine][size - 1] = '\0';
		 else
		 {  int min, max;
			 extern int OldLine;

			 if ( Terminal[WinDialog.LastLine] != NULL )
			 {
		free(Terminal[WinDialog.LastLine]);
		Terminal[WinDialog.LastLine] = NULL;
	       }

			 WinDialog.LastLine --;
	       OldLine --;

	       if ( WinDialog.LastLine < 0 )
		  WinDialog.LastLine = DIALOG_SIZE;

	       GetScrollRange (WinDialog.hWnd, SB_VERT, &min, &max);
	       if ( WinDialog.NoLines < max && max < DIALOG_SIZE )
	       {  SetScrollRange ( WinDialog.hWnd, SB_VERT, WinDialog.NoLines, max-1, FALSE );
		  SetScrollPos   ( WinDialog.hWnd, SB_VERT, max-1, FALSE );
	       }
	       InvalidateRect ( WinDialog.hWnd, NULL, TRUE );
		 }
		 SendToScreen();
	 }
	 break;
      }

      /*--------------------------+
      | Remove Special Keys (ALT) |
      +--------------------------*/
      case '\f':
      case VK_MENU:
	 break;


      /*----------------+
      | Handle Tab Keys |
      +----------------*/
      case '\t':
		{  if ( !ScreenOnly)
	 {  if ( GetCommandString() == NULL)
	       SetCommandString("   ");
	    else
	       AppendCommandString("   ");
	 }
	 PrintCLIPS ("stdout", "   " );
	 break;
      }


      /*---------------------+
      | Return / Newline Key |
      +---------------------*/
      case '\r':
      case '\n':
      {  wParam = (int)'\n';
	 if (GetScrollPos (WinDialog.hWnd, SB_HORZ) != 0)
	 {  SetScrollPos ( WinDialog.hWnd, SB_HORZ, 0, TRUE );
	    InvalidateRect(WinDialog.hWnd, NULL, FALSE);
	    SendMessage ( WinDialog.hWnd, WM_PAINT, 0, 0 );
	 }
      }

      /*---------------------+
      | All other characters |
      +---------------------*/
      default:
      {  char text[2];

	 text[0] = (char) wParam;
	 text[1] = '\0';

	 if(isprint ( text[0]) || isspace (text[0]))
	 {  /*----------------------------+
	    | Add to CLIPS Command Buffer |
		 +----------------------------*/

	    if (!ScreenOnly)
	    {  if ( GetCommandString() == NULL)
		  SetCommandString(text);
	       else
		  AppendCommandString(text);
		 }

		 PrintCLIPS ("stdout", text );
	 }
	 break;
		}
	}
	HorizScroll = 0;
}

/****************************************************************
* DoWindowMessage: Callback function for dialog main window.
*    WM_COMMAND has been filtered.
****************************************************************/

long FAR PASCAL DoWindowMessage ( 
   HWND hMain,
   unsigned message,
   WPARAM wParam,
   LONG lParam)

{  switch ( message )
   {  case WM_CREATE:
	 return (StartUpDDE());

      case WM_KEYDOWN:
		{  if ( LOWORD(wParam) == VK_SHIFT )
	 {  extern int ShiftKeyDown;
	    ShiftKeyDown = TRUE;
	 }
	 break;
      }

      case WM_KEYUP:
      {  if ( LOWORD(wParam) == VK_SHIFT )
	 {  extern int ShiftKeyDown;
	    ShiftKeyDown = FALSE;
	 }
	 break;
      }

      case WM_CHAR:
      {  GetUserCmd ( LOWORD(wParam), FALSE, 0 );
	 break;
      }

      case WM_ACTIVATEAPP:
		{   extern int ApplicationFocus;
	  ApplicationFocus = (int) LOWORD(wParam);
	  if (!ApplicationFocus )
		  UpdateCursor( ARROW_CURSOR );
	  break;
      }

      case WM_TIMER:
      {  static int value = 0;
	 extern HCURSOR NowCursor, WAIT[];
	 extern int ApplicationFocus;
	 value ++;
	 NowCursor = WAIT[value];
	 if (ApplicationFocus )
	    SetCursor ( NowCursor );
	 if (value > 7 )
	   value = 0;
	 break;
      }

      case WM_SETCURSOR:
      {  extern HCURSOR NowCursor;
	 SetCursor ( NowCursor );
	 break;
		}

      case WM_SETFOCUS:
		{  extern SCREEN WinDialog;
	 CreateCaret ( hMain, NULL, 1, WinDialog.LineSize);
	 ShowCaret ( hMain );
	 break;
      }

      case WM_KILLFOCUS:
      {  HideCaret ( hMain );
	 DestroyCaret();
	 break;
      }

		case WM_PAINT:
      {  PAINTSTRUCT ps;
	 HDC  hDC;
 
	 BeginPaint (hMain, (LPPAINTSTRUCT)&ps);
	    hDC = ps.hdc;
	    SetMapMode (hDC, MM_TEXT);
	    RedrawTerminal(hDC);
	    ValidateRect (hMain, (LPRECT)NULL);
	 EndPaint (hMain, (LPPAINTSTRUCT)&ps);
	 break;
      }

      case WM_SIZE:
      {  extern SCREEN WinDialog;
	 int min, max;

	 GetScrollRange (hMain,SB_VERT,&min, &max);
	 WinDialog.NoLines = (HIWORD (lParam) / WinDialog.LineSize) - 1;
	 SetScrollRange (hMain, SB_VERT, WinDialog.NoLines, max, TRUE );

	 InvalidateRect ( hMain, NULL, TRUE );
	 switch ( LOWORD(wParam) )
	 {  case SIZE_MINIMIZED:
			 PostMessage ( hMain, WM_COMMAND, IDC_WIN_MINIMIZE, 0);
	       break;

	    case SIZE_RESTORED:
	       PostMessage ( hMain, WM_COMMAND, IDC_WIN_MAXIMIZE, 0);
	       break;
	 }
	 break;
      }

      case WM_HSCROLL:
      {  extern SCREEN WinDialog;
	 int min, max, cp;
	 GetScrollRange (hMain,SB_HORZ,&min, &max);
	 cp = GetScrollPos ( hMain, SB_HORZ );
	 switch (LOWORD(wParam))
	 {   case SB_LINEDOWN:
	     {  cp++;
		break;
	     }
	     case SB_LINEUP:
		  {  cp--;
		break;
	     }
		  case SB_PAGEDOWN:
	     {  cp += WinDialog.NoLines;
		break;
	     }
	     case SB_PAGEUP:
	     {  cp -= WinDialog.NoLines;
		break;
	     }
	     case SB_THUMBTRACK:
		  {  cp = LOWORD(lParam);
		break;
	     }
		  default:
	     {   return (0);
	     }
	 }

	 if (cp > max) cp = max;
	 if (cp < min) cp = min;

	 SetScrollPos   (hMain, SB_HORZ, cp, TRUE );
	 InvalidateRect (hMain, NULL, TRUE );
	 break;
      }

      case WM_VSCROLL:
      {  extern SCREEN WinDialog;
	 int min, max, cp;
	 static int CaretON = TRUE;

	 GetScrollRange (hMain,SB_VERT,&min, &max);
	 cp = GetScrollPos ( hMain, SB_VERT );
	 switch (LOWORD(wParam))
	 {   case SB_LINEDOWN:
	     {  cp++;
		break;
		  }
	     case SB_LINEUP:
	     {  cp--;
		break;
	     }
	     case SB_PAGEDOWN:
	     {  cp += WinDialog.NoLines;
		break;
	     }
		  case SB_PAGEUP:
	     {  cp -= WinDialog.NoLines;
		break;
		  }
	     case SB_THUMBTRACK:
	     {  cp = LOWORD(lParam);
		break;
	     }
	     default:
	     {   return (0);
	     }
	 }

	 if (cp >= max)
	 {  cp = max;
		 ShowCaret ( hMain );
	    CaretON = TRUE;
	 }
	 else if ( CaretON )
	 {  HideCaret ( hMain );
	    CaretON = FALSE;
	 }

	 if (cp < WinDialog.NoLines)
		 cp = WinDialog.NoLines;

	 SetScrollPos   (hMain, SB_VERT, cp, TRUE );
	 InvalidateRect (hMain, NULL, TRUE );
	 break;
      }

      case WM_DESTROY:
		{  

	 /* Shut Down DDE Server. */
	 ShutDownDDE ( );

	 /* Free Dialog Window */
	 DeleteTerminal();

	 /* Quit Windows */
	 PostQuitMessage(0);
	 return 0;
      }

      /*-------------------------------+
      | Used to Update the Module Menu |
      +-------------------------------*/
		case WM_INITMENUPOPUP:
      {  int MenuID = GetMenuItemID((HMENU) wParam, 0);
	 if ( MenuID == IDM_MODULE_ONE )
		 UpdateModuleMenu ((HMENU)wParam);
	 break;
      }

      case WM_CLOSE:
	 PostMessage ( hMain, WM_COMMAND, IDM_FILE_QUIT, 0);
	 break;

      default:
	 return (DefWindowProc(hMain, message, wParam, lParam));
   }
   return (TRUE);
}

/******************************************************************
* RedrawTerminal: Draw dialog window based on the min, max
*    and current position of the thumbnail in the horizontal
*    & vertical scroll bars.
******************************************************************/

void RedrawTerminal ( 
	HDC hDC)

{  extern SCREEN WinDialog;
	int x, max, min, pos, start, sbmax = 0;
	RECT Rect;
	char *buffer;
   
   /*----------------------------------------------+
   | Get current value of the vertical scroll bar. |
   +----------------------------------------------*/
   GetScrollRange ( WinDialog.hWnd, SB_VERT, &min, &max);
	pos = GetScrollPos (WinDialog.hWnd, SB_VERT );

   /*-----------------------------------------+
	| Locate the first visible line to display |
   +-----------------------------------------*/
   start = WinDialog.LastLine - ( max - pos ) - WinDialog.NoLines;

   if ( start == WinDialog.LastLine )
      start ++;

   /*----------------------------------------------+
   | If the start value is negative then check if  |
	| at a 'hard' top or if a wrap around is needed |
   +----------------------------------------------*/
   if ( start < 0 )
	{  if ( max == DIALOG_SIZE )
	 start = max + start + 1;
      else
	 start = 0;
   }

   /*------------------------------------------+
   | Get Position of the horizontal scroll bar |
   +------------------------------------------*/
	pos = GetScrollPos ( WinDialog.hWnd, SB_HORZ);

   /*----------------------------------------------------------+
	| Loop to display text in the visible portion of the screen |
   +----------------------------------------------------------*/
   for ( x = 0; x<= WinDialog.NoLines && Terminal[start] != NULL; x++ )
	{
	 int bufsize;

      GetClientRect (WinDialog.hWnd, &Rect);
      Rect.top = x * WinDialog.LineSize;

      /*------------------------------------+
      | Calculate Horizontal Scroll bar Max | 
		| based on what is displayed.         |
      +------------------------------------*/
      
		if (Terminal[start] != NULL)
	{ bufsize = strlen(Terminal[start]); }
      else
	{ bufsize = 0; }

      if ( bufsize > sbmax )  sbmax = bufsize;

      /*-----------------------------------+
      | Display each line of text adjusted |
		| for the horizontal scroll bar.     |
      +-----------------------------------*/
      buffer = NULL;
		if (( pos <= bufsize ) && (Terminal[start] != NULL))
      {  buffer = Terminal[start]+pos;
	 DrawText ( hDC, buffer, -1, &Rect, DT_LEFT|DT_NOCLIP | DT_NOPREFIX );
      }

      /*-------------------------------+
      | Check if wrap around is needed |
      +-------------------------------*/
      start ++;
		if ( start > DIALOG_SIZE )
	 start = 0;
   }
	/*-------------------------------------+
   | Calculate and display caret adjusted |
   |    for horizontal scroll bar.        |
   +-------------------------------------*/
   if ( buffer == NULL )
      buffer = "";
	DrawText ( hDC, buffer, -1, &Rect,
	          DT_LEFT|DT_NOCLIP|DT_CALCRECT );
	SetCaretPos (Rect.right, Rect.top);
	SetScrollRange ( WinDialog.hWnd, SB_HORZ, 0, sbmax-1, TRUE );
}

/**********************************************************************
* PrintFunction: Function will allocate memory used to store strings
*   sent to the Dialog window.
**********************************************************************/
#if IBM_TBC
#pragma argsused
#endif
int PrintFunction ( char* logicalname, char* buffer )

{  extern SCREEN WinDialog;       /* Data Structure for the Dialog Window */
	int Loc = 0;                /* Number of characters until line break */
	char *str;                  /* Subset of the buffer up to the line break */
	extern int HorizScroll;
   int oldsize, size;
	/*------------------------------------------------------------------------+
   | WinDialog.NoLines = Number of text lines that can be displayed per screen. |
   | WinDialog.LineSize = Number of pixels in height for each line of text.     |
   | WinDialog.LastLine = Virtual location for the bottom of the text area.     |
   +------------------------------------------------------------------------*/

   /*------------------------------------------+
	| Allocate room for the buffer to be parsed |
   +------------------------------------------*/
	str = (char *)malloc(strlen(buffer)+1);
   if (str == NULL)
      ExitToShell();



   /*-----------------------------------------------+
   | Loop through each 'part' of the buffer string  |
   | Note: 'part' is text between carriage returns. |
   +-----------------------------------------------*/
   while (Loc < strlen(buffer))
   {
     str[0] = '\0';
     
     /*----------------------------------+
		| Caputre text up to the line break |
      +----------------------------------*/
      sscanf(&(buffer[Loc]), "%[^\n]",str );


		/*----------------------------------------+
		| Allocate new memory if new line of text |
		| or reallocate if appending line of text |
		+----------------------------------------*/

		if (Terminal[WinDialog.LastLine] != NULL)
	{ oldsize = strlen( Terminal[WinDialog.LastLine] ); }
		else
	{ oldsize = 0; }

		size = oldsize + strlen(str)+1;
		if (Terminal[WinDialog.LastLine] != NULL)
	 Terminal[WinDialog.LastLine] = (char *) realloc(Terminal[WinDialog.LastLine], size);
		else
	 Terminal[WinDialog.LastLine] = (char *) malloc(size);
		if (Terminal[WinDialog.LastLine] == NULL)
	 ExitToShell();

		/*--------------------------------+
		|Copy String to the dialog window |
		+--------------------------------*/
		Terminal[WinDialog.LastLine][oldsize] = '\0';
		strcat (Terminal[WinDialog.LastLine], str);
		Loc += strlen( str );


		if (buffer[Loc] == '\n')
		{  int min, max;
	 /*------------------------------+
	 | Display line before advancing |
	 +------------------------------*/
	 SendToScreen ();
	 WinDialog.LastLine ++;
	 if (WinDialog.LastLine > DIALOG_SIZE ) WinDialog.LastLine = 0;

	 /*-----------------------+
	 | Free next line of text |
	 +-----------------------*/
	 if (Terminal[WinDialog.LastLine] != NULL)
	 {
	  free(Terminal[WinDialog.LastLine]);
		 Terminal[ WinDialog.LastLine ] = NULL;
	 }

	 /*-------------------+
	 | Update Scroll Bars |
	 +-------------------*/
	 GetScrollRange ( WinDialog.hWnd, SB_VERT, &min, &max );
	 if ( max < DIALOG_SIZE ) max = WinDialog.LastLine;
	 else max = DIALOG_SIZE;
	 if ( max > WinDialog.NoLines )
		 SetScrollRange ( WinDialog.hWnd, SB_VERT, WinDialog.NoLines, max, FALSE );
	 else
		 SetScrollRange ( WinDialog.hWnd, SB_VERT, 0,1 , FALSE );
	 SetScrollPos   ( WinDialog.hWnd, SB_VERT, max, TRUE );

	 GetScrollRange ( WinDialog.hWnd, SB_HORZ, &min, &max);
	 if ( max < size ) max = size;
	 SetScrollRange ( WinDialog.hWnd, SB_HORZ, 0, max, FALSE);
	 SetScrollPos   ( WinDialog.hWnd, SB_HORZ, 0, TRUE);
		}

		SendToScreen ();

		Loc ++;
	}
	free(str);
	return (TRUE);
}

/********************************************************************
* SendToScreen: Displays the current text line in the Dialog Window.
*********************************************************************/
void SendToScreen ( void )

{  extern SCREEN WinDialog;           /* D.S. Holding info about screen */
	RECT DRect;                     /* Client Area of Dialog Window */
	RECT Rect;                      /* Adjusted Area for scrolling */
	HDC hDC = GetDC ( WinDialog.hWnd); /* Handle to the device context */
	extern int OldLine;             /* Previous Line Printed */
	int min, max, pos;              /* Scroll Bar Values */
	int Scroll = 0;                 /* Scrolling Needed? */
	GetClientRect ( WinDialog.hWnd, &Rect );
	GetClientRect ( WinDialog.hWnd, &DRect );

	HideCaret ( WinDialog.hWnd );
	/*------------------------------------------------------+
	| Move to the bottom of the screen if not already there |
	+------------------------------------------------------*/
	GetScrollRange ( WinDialog.hWnd, SB_VERT, &min, &max );
	pos = GetScrollPos (WinDialog.hWnd, SB_VERT );
	if ( pos != max )
	{  InvalidateRect ( WinDialog.hWnd, NULL, TRUE );
		SetScrollPos (WinDialog.hWnd, SB_VERT, max, FALSE );
		SendMessage ( WinDialog.hWnd, WM_PAINT, 0, 0);
	}

	/*--------------------------------------------------------+
	| Determine if the screen is full and scrolling is needed |
	+--------------------------------------------------------*/
	if ( max > WinDialog.NoLines ) Scroll = 1;

	/*-------------------------------------------------------------+
	| Scroll Window if newline and text will not fit on the screen |
	+-------------------------------------------------------------*/
	if (Scroll && WinDialog.LastLine != OldLine)
	{  OldLine = WinDialog.LastLine;
		ScrollDC ( hDC, 0, -WinDialog.LineSize, &DRect, &DRect, NULL, &Rect );
	}

	/*----------------------------------------+
	| Calculate when text is to be displayed  |
	+----------------------------------------*/
	if ( !Scroll)
		Rect.top = (WinDialog.LastLine) * WinDialog.LineSize;
	else
		Rect.top = (WinDialog.NoLines) * WinDialog.LineSize;

	/*----------------------------+
	| Clear Line to be displayed  |
	+----------------------------*/
	{  HANDLE OldObject;
		OldObject = SelectObject (hDC, GetStockObject ( WHITE_PEN ) );
		Rectangle (hDC, Rect.left, Rect.top, Rect.right, Rect.bottom);
		SelectObject (hDC, OldObject);
	}

	/*--------------------------+
	| Display Text Adjusting    |
	| for the Horizontal scroll |
	+--------------------------*/
	pos = GetScrollPos (WinDialog.hWnd, SB_HORZ );

	if (Terminal[WinDialog.LastLine] == NULL)
	  { /* Do Nothing */ }
	else if ( pos < strlen (Terminal[WinDialog.LastLine]) )
	{
	 DrawText ( hDC, Terminal[WinDialog.LastLine]+pos, -1, &Rect,
			DT_LEFT | DT_NOCLIP | DT_SINGLELINE | DT_NOPREFIX );
	 DrawText ( hDC, Terminal[WinDialog.LastLine]+pos, -1, &Rect,
			DT_LEFT | DT_NOCLIP | DT_SINGLELINE | DT_CALCRECT | DT_NOPREFIX);
	}
	else
		Rect.right = 2;

	/*-----------+
	| Show Caret |
	+-----------*/
	SetCaretPos (Rect.right, Rect.top);
	ShowCaret ( WinDialog.hWnd );

	/*--------------------------------+
	| Automatic Horizontal Scrolling  |
	+--------------------------------*/

	if (Terminal[WinDialog.LastLine] != NULL)
	  {
		DrawText ( hDC, Terminal[WinDialog.LastLine]+pos, -1, &Rect,
		 DT_LEFT | DT_NOCLIP | DT_SINGLELINE | DT_NOPREFIX );
	  }
	if ( Rect.right > DRect.right && HorizScroll )
	{  GetScrollRange ( WinDialog.hWnd, SB_HORZ, &min, &max );
		pos ++;
		if ( max < pos) max = pos;
		SetScrollRange ( WinDialog.hWnd, SB_HORZ, 0, max, FALSE );
		SetScrollPos ( WinDialog.hWnd, SB_HORZ, pos, TRUE );
		InvalidateRect (WinDialog.hWnd, NULL, TRUE);
	}

	OldLine = WinDialog.LastLine;
	ReleaseDC ( WinDialog.hWnd, hDC);
}

/**************************************************************
* ClearDialogWnd: Procedure will clear all text from the dialog
*    window and free storage in the terminal data structure.
**************************************************************/

void ClearDialogWnd ( void )
{  extern int OldLine;
   extern SCREEN WinDialog;

   /*-----------------------------------------+
   | Free all data associated with the screen |
   +-----------------------------------------*/
   FreeTerminalText();

   /*-----------------------------+
   | Information about the Screen |
   +-----------------------------*/
   WinDialog.LastLine = 0;
   OldLine = 0;

   /*-------------------+
   | Update Scroll Bars |
   +-------------------*/
   SetScrollRange ( WinDialog.hWnd, SB_VERT, WinDialog.NoLines-1, WinDialog.NoLines, FALSE);
   SetScrollPos   ( WinDialog.hWnd, SB_VERT, WinDialog.NoLines, TRUE);
   SetScrollRange ( WinDialog.hWnd, SB_HORZ, 0, 1, FALSE);
   SetScrollPos   ( WinDialog.hWnd, SB_HORZ, 0, TRUE);
   InvalidateRect ( WinDialog.hWnd, NULL, TRUE );
   FlushCommandString();
}
