   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*               CLIPS Version 6.00  10/01/92          */
   /*                                                     */
   /*                   STATUS MODULE                     */
   /*******************************************************/

/**************************************************************/
/* Purpose: Callback and support for all Status Trace Windows */
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
#include <string.h>

/*------------------------+
| CLIPS 6.0 Include Files |
+------------------------*/
#include "setup.h"
#include "engine.h"
#include "inscom.h"
#include "globldef.h"
#include "insfun.h"
#include "factmngr.h"

/*------------------------+
| Interface Include Files |
+------------------------*/

#include "ids.h"
#include "status.h"
#include "main.h"

/*--------------------------+
| Local Function Prototypes |
+--------------------------*/

void DrawFacts ( void );
void RedrawScreen (SCREEN *,void(*)(char*,int,void*),void*(*)(void *), HDC);
void UpdateWnd ( SCREEN * );
static void GetFocusPPForm ( char *, int, void *);
void UpdateStatusWndTitle ( HWND, char*);
int  CountInstances( void );
int  CountDefglobals( void );
int  CountFocus( void );
int  CountFacts ( void );
int  ProcessScrollMsg (SCREEN *, WORD, LONG, int );

int ChangeToFocus = 1;

/*-----------------------------------+
| Setup Screen Information Variables |
+-----------------------------------*/

SCREEN WinGlobal;
SCREEN WinFact;
SCREEN WinInst;
SCREEN WinAgenda;
SCREEN WinFocus;

/*******************************************************************
* StatusWndProc: Callback Function for Status Windows.
*******************************************************************/

long FAR PASCAL StatusWndProc(
  HWND     hWnd,
  unsigned message,
  WPARAM    wParam,
  LONG     lParam)

{  extern SCREEN WinFact, WinAgenda, WinInst, WinGlobal, WinFocus;
   SCREEN *Window;
   void (*GetPPForm)(char*,int,void*) = NULL;
   void *(*GetNextValue)(void*) = NULL;

   /*---------------------------------------------------+
   | Determine which Window the message is intended for |
   +---------------------------------------------------*/
#if DEFTEMPLATE_CONSTRUCT
   if ( hWnd == WinFact.hWnd )
   {  Window = &WinFact;
      GetNextValue = GetNextFactInScope;
      GetPPForm = GetFactPPForm;
   }
#endif

#if OBJECT_SYSTEM
   if ( hWnd == WinInst.hWnd )
   {  Window = &WinInst;
      GetNextValue = GetNextInstanceInScope;
      GetPPForm = GetInstancePPForm;
   }
#endif

#if DEFGLOBAL_CONSTRUCT
   if ( hWnd == WinGlobal.hWnd )
   {  Window = &WinGlobal;
      GetNextValue = GetNextDefglobalInScope;
      GetPPForm = GetDefglobalValueForm;
   }
#endif

#if DEFRULE_CONSTRUCT
   if ( hWnd == WinAgenda.hWnd )
   {  Window = &WinAgenda;
      GetNextValue = GetNextActivation;
      GetPPForm = GetActivationPPForm;
   }

   if ( hWnd == WinFocus.hWnd )
   {  Window = &WinFocus;
      GetNextValue = GetNextFocus;
      GetPPForm = GetFocusPPForm;
   }
#endif

   switch (message)
   {  case WM_VSCROLL:
	 return ( ProcessScrollMsg ( Window, LOWORD(wParam), lParam, SB_VERT ));

      case WM_HSCROLL:
	 return ( ProcessScrollMsg ( Window, LOWORD(wParam), lParam, SB_HORZ ));

      case WM_SIZE:
      {  /*-----------------------+
	 | Window Changes in Size |
	 +-----------------------*/
	 (*Window).NoLines = HIWORD (lParam) / (*Window).LineSize;
	 UpdateWnd ( Window );
	 break;
      }

      case WM_KEYDOWN:
      {  /*----------------------------------------------------+
	 | Key input was detected in window send to Dialog Wnd |
	 +----------------------------------------------------*/
         extern SCREEN WinDialog;
         PostMessage ( WinDialog.hWnd, message, wParam, lParam);
	 SetFocus ( WinDialog.hWnd );
         break;
      }

      case WM_PAINT:
      {  /*---------------------+
	 | Screen Update Needed |
         +---------------------*/
         PAINTSTRUCT ps;
         HDC  hDC;
 
	 BeginPaint (hWnd, (LPPAINTSTRUCT)&ps);
	    hDC = ps.hdc;
            SetMapMode (hDC, MM_TEXT);
            RedrawScreen ( Window, GetPPForm, GetNextValue, hDC );
	    ValidateRect (hWnd, (LPRECT)NULL);
         EndPaint (hWnd, (LPPAINTSTRUCT)&ps);
	 return (TRUE);
      }

      default:
         return (DefWindowProc(hWnd, message, wParam, lParam));

   }
   return ( TRUE );
}

/*******************************************************************
* ProcessScrollMsg: Generic Procedure to process scroll bar messages.
*******************************************************************/

int ProcessScrollMsg ( 
   SCREEN *Window,
   WORD wParam,
   LONG lParam,
   int Bar)

{  int min, max, cp;
   /*------------------------+
   | Vertical Scroll Message |
   +------------------------*/
   cp = GetScrollPos ( (*Window).hWnd, Bar );
   switch (wParam)
   {  case SB_LINEDOWN:
      {  cp++;
         break;
      }
      case SB_LINEUP:
      {  cp--;
         break;
      }
      case SB_PAGEDOWN:
      {  cp += (*Window).NoLines;
         break;
      }
      case SB_PAGEUP:
      {  cp -= (*Window).NoLines;
         break;
      }
      case SB_THUMBTRACK:
      {  cp = LOWORD(lParam);
         break;
      }
      default:
         return (0);
   }
   GetScrollRange ((*Window).hWnd ,Bar, &min, &max);
   if (cp < min) cp = min;
   if (cp > max) cp = max;
   SetScrollPos ((*Window).hWnd, Bar, cp, TRUE);
   InvalidateRect ( (*Window).hWnd, NULL, TRUE );
   return (1);
}


/*******************************************************************
* RedrawScreen: Function will clear and redraw a status screen.
*******************************************************************/
void RedrawScreen(
  SCREEN *Window,
  void (*GetPPForm)(char*,int,void*),
  void *(*GetNextValue)(void*),
  HDC hDC)

{  void *ValuePtr;
   char Buffer[300];
   RECT Rect;
   int count, pos, sbmax = 0;

   pos = GetScrollPos ( (*Window).hWnd, SB_VERT );
   /*-----------------------------------------------------------+
   |Skip over any facts that fall out of range above the window.|
   +-----------------------------------------------------------*/
   count = 0;
   ValuePtr = (*GetNextValue)(NULL);
   while ( ValuePtr != NULL && count < pos )
   {  ValuePtr = (*GetNextValue)(ValuePtr);
      count++;
   }  

   if ( ValuePtr != NULL )
   {  /*-------------+
      | Clear Window |
      +-------------*/
      GetClientRect ( (*Window).hWnd, &Rect );
      Rectangle ( hDC, Rect.left-2, Rect.top-2, Rect.right+2, Rect.bottom+2 );

      /*-----------------------------------------------------+
      | Display the items that fall within the status window.|
      +-----------------------------------------------------*/
      count = 0;
      do
      {  int bufsize;
         pos = GetScrollPos ( (*Window).hWnd, SB_HORZ );
         (*GetPPForm)(Buffer, 299, ValuePtr);
         bufsize = lstrlen (Buffer);
         if (bufsize > sbmax ) sbmax = bufsize;
         if ( pos < bufsize )
         {  char * buffer = Buffer + pos;
            DrawText(hDC, buffer, lstrlen(buffer), &Rect, DT_LEFT );
         }
         Rect.top = Rect.top + (*Window).LineSize;
         count ++;
      }
      while (((ValuePtr=(*GetNextValue)(ValuePtr)) != NULL) && (count < (*Window).NoLines));
      SetScrollRange ( (*Window).hWnd, SB_HORZ, 0, sbmax-1, TRUE );
   }
}

/*************************************************************
* EnableStatusWindows:  Function is used to initialize status
*    window menu items.
**************************************************************/

void EnableStatusWindows ( void )
{  extern SCREEN WinDialog;
   HMENU hMenu = GetMenu ( WinDialog.hWnd );
#if DEFRULE_CONSTRUCT
   /* Enable Agenda Status Window */
   EnableMenuItem(hMenu,IDM_WIN_AGENDA,MF_BYCOMMAND|MF_ENABLED);

   /* Enable Focus Status Window */
   EnableMenuItem(hMenu,IDM_WIN_FOCUS,MF_BYCOMMAND|MF_ENABLED);
#endif

#if DEFTEMPLATE_CONSTRUCT
   /* Enable Facts Status Window */
   EnableMenuItem(hMenu,IDM_WIN_FACTS,MF_BYCOMMAND|MF_ENABLED);
#endif

#if DEFGLOBAL_CONSTRUCT
   /* Enable Globals Status Window */
   EnableMenuItem(hMenu,IDM_WIN_GLOBALS,MF_BYCOMMAND|MF_ENABLED);
#endif

#if OBJECT_SYSTEM
    /* Enable Instance Status Window */
    EnableMenuItem(hMenu,IDM_WIN_INSTANCE,MF_BYCOMMAND|MF_ENABLED);
#endif
}

/*******************************************************************
* GetFocusPPForm: Returns the Focus Names in printable form.
*******************************************************************/

static void GetFocusPPForm ( 
   char *buffer,
   int   bufferLength,
   void *vTheFocus)

{  struct focus *theFocus = (struct focus *) vTheFocus;
   strncpy (buffer, GetDefmoduleName(theFocus->theModule),bufferLength);
}

/*******************************************************************
* CountFocus: Returns the number of Focus in CLIPS
*******************************************************************/

int CountFocus( void )
{ int count = 0;
  void *Ptr = NULL;
  while (( Ptr = GetNextFocus(Ptr) )!= NULL)
     count ++;
  return count;
}

/*******************************************************************
* CountFacts: Returns the number of Facts in Scope.
*******************************************************************/

int CountFacts( void )
{ int count = 0;
  void *FactPtr = NULL;
  while (( FactPtr = GetNextFactInScope(FactPtr) )!= NULL)
     count ++;
  return count;
}

/*******************************************************************
* CountInstances: Returns the number of instances in Scope.
*******************************************************************/

int CountInstances( void )
{ int count = 0;
  void *InstPtr = NULL;
  while (( InstPtr = GetNextInstanceInScope(InstPtr) )!= NULL)
     count ++;
  return count;
}


/*******************************************************************
* CountDefglobals: Returns the number of DefGlobals in Scope
*******************************************************************/

int CountDefglobals( void )
{ int count = 0;
  void *Ptr = NULL;
  while (( Ptr = GetNextDefglobalInScope(Ptr) )!= NULL)
     count ++;
  return count;
}

/*******************************************************************
* UpdateStatus: Function called while CLIPS is executing to update
*    each visible status window.
*******************************************************************/

void UpdateStatus ( void )
{  extern SCREEN WinFact, WinAgenda, WinGlobal, WinInst, WinDialog;
   static long lastModuleIndex = -1;
   extern long ModuleChangeIndex;

   if ( lastModuleIndex != ModuleChangeIndex )
   {
#if DEFRULE_CONSTRUCT
      SetFocusChanged     (CLIPS_TRUE);
      SetAgendaChanged    (CLIPS_TRUE);
#endif
#if DEFTEMPLATE_CONSTRUCT
      SetFactListChanged  (CLIPS_TRUE);
#endif
#if OBJECT_SYSTEM
      SetInstancesChanged (CLIPS_TRUE);
#endif
#if DEFGLOBAL_CONSTRUCT
      SetGlobalsChanged   (CLIPS_TRUE);
#endif
      lastModuleIndex = ModuleChangeIndex;
   }

#if DEFRULE_CONSTRUCT
   /*------------------------+
   | Update the Focus Window |
   +------------------------*/
   if (IsWindowVisible(WinFocus.hWnd))
   {  if (GetFocusChanged())
      {  SetFocusChanged(CLIPS_FALSE);
	 WinFocus.LastLine = (int) CountFocus();
	 UpdateWnd ( &WinFocus );
      }
   }

   /*-------------------------+
   | Update the Agenda Window |
   +-------------------------*/
   if (IsWindowVisible(WinAgenda.hWnd))
   {  if ( GetAgendaChanged() )
      {  SetAgendaChanged ( CLIPS_FALSE );
	 WinAgenda.LastLine = (int) GetNumberOfActivations();
         UpdateStatusWndTitle(WinAgenda.hWnd, "Agenda");
	 UpdateWnd ( &WinAgenda );
      }
   }
#endif

#if DEFTEMPLATE_CONSTRUCT
   /*-----------------------+
   | Update the Fact Window |
   +-----------------------*/
   if (IsWindowVisible(WinFact.hWnd))
   {  if ( GetFactListChanged())
      {  SetFactListChanged ( CLIPS_FALSE );
	 WinFact.LastLine = CountFacts();
	 UpdateStatusWndTitle(WinFact.hWnd, "Facts");
	 UpdateWnd ( &WinFact );
      }
   }
#endif

#if OBJECT_SYSTEM
   /*---------------------------+
   | Update the Instance Window |
   +---------------------------*/
   if (IsWindowVisible(WinInst.hWnd))
   {  if (GetInstancesChanged())
      {  SetInstancesChanged (CLIPS_FALSE);
	 UpdateStatusWndTitle(WinInst.hWnd, "Instances");
	 WinInst.LastLine = CountInstances();
	 UpdateWnd ( &WinInst );
      }
   }
#endif

#if DEFGLOBAL_CONSTRUCT
   /*--------------------------+
   | Update the Globals Window |
   +--------------------------*/
   if (IsWindowVisible(WinGlobal.hWnd))
   {  if (GetGlobalsChanged())
      {  SetGlobalsChanged(CLIPS_FALSE);
	 UpdateStatusWndTitle(WinGlobal.hWnd, "Globals");
	 WinGlobal.LastLine = CountDefglobals();
	 UpdateWnd ( &WinGlobal );
      }
   }
#endif
}

/*******************************************************************
* UpdateWnd: Calculates the Maximum Value for the Vertical
*    Scrollbar, Current position and Redraws Window.
*******************************************************************/

void UpdateWnd ( 
   SCREEN *Window)

{  /*------------------------------------------------+
   | Calculate the Maximum Value for the Vert Scroll |
   +------------------------------------------------*/
   int max = (int) (*Window).LastLine - (*Window).NoLines;
   if (max < 0 )
      max = 0;
   SetScrollRange ((*Window).hWnd ,SB_VERT,0, max ,FALSE);

   /*-----------------------------------------------+
   | If Agenda Window Then start display at the top |
   +----------------------------------------------*/
   if ( (*Window).hWnd == WinAgenda.hWnd )
      max = 0;
   SetScrollPos   ((*Window).hWnd, SB_VERT, max ,TRUE);
   InvalidateRect ( (*Window).hWnd, NULL, TRUE);
}

/*******************************************************************
* UpdateStatusWndTitle: Function will append the Current Module
*   name to the Window Title Bar.
*******************************************************************/
void UpdateStatusWndTitle(
  HWND hWnd,
  char *prefix)
  {  
   struct defmodule *theModule = (struct defmodule *) GetCurrentModule();
   char buffer[255];
   
   sprintf ( buffer, "%s (%s)",prefix, GetDefmoduleName(theModule));
   SetWindowText (hWnd, buffer);
  }

