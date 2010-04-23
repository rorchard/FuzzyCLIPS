   /*******************************************************/
   /*      "C" Language Integrated Production System      */
	/*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*               CLIPS Version 6.00  10/01/92          */
   /*                                                     */
   /*                   DIALOG2 MODULE                    */
   /*******************************************************/

/**************************************************************/
/* Purpose: Contains the callback functions for all the       */
/*       list manager dialog items.                           */
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

/*------------------------+
| CLIPS 6.0 Include Files |
+------------------------*/
#include "setup.h"
#include "dffctdef.h"
#include "commline.h"
#include "rulecom.h"
#include "rulebsc.h"
#include "dffctbsc.h"
#include "router.h"
#include "tmpltdef.h"
#include "tmpltbsc.h"
#include "dffnxfun.h"
#include "defins.h"
#include "genrccom.h"
#include "classcom.h"
#include "classexm.h"
#include "classinf.h"
#include "msgcom.h"
#include "engine.h"
#include "globldef.h"
#include "globlbsc.h"

/*------------------------+
| Interface Include Files |
+------------------------*/
#include "ids.h"
#include "dialog.h"
#include "main.h"

struct symbolMatch *matches;
char DialogName[80];

BOOL FAR PASCAL DefMethodsManager( HWND, unsigned, WORD, LONG );
BOOL FAR PASCAL DefMessageManager( HWND, unsigned, WORD, LONG );
void ShowButtons ( HWND, int, int, int, int, int, int, int, int, int );
void HandleButton (HWND, int, int );
void HandleCkBox  (HWND, int, int );

/************************************************************************
* HandleCkBox: procedure will update the state of a check box
*   in the list manager
************************************************************************/

void HandleCkBox ( 
	HWND hDlg, /* Pointer to the Window */
	int State, /* HIDE, CHECK, UNCHECK OR GRAY */
	int Ident) /* Checkbox ID */

{ switch (State)
   {  case HIDE:
	 ShowWindow(GetDlgItem(hDlg, Ident), SW_HIDE);
	 break;
      case CHECK:
      case UNCHECK:
	 ShowWindow(GetDlgItem(hDlg, Ident), SW_SHOW);
	 EnableWindow(GetDlgItem(hDlg, Ident), TRUE);
	 CheckDlgButton ( hDlg, Ident, State );
	 break;
      case GRAY:
	 ShowWindow(GetDlgItem(hDlg, Ident), SW_SHOW);
	 EnableWindow(GetDlgItem(hDlg, Ident), FALSE);
	 CheckDlgButton ( hDlg, Ident, DISABLE );
	 break;
   }
}

/************************************************************************
* HandleButton: procedure will update the state of a button
*   in the list manager
************************************************************************/

void HandleButton (
   HWND hDlg,  /* Window Pointer */
   int State,  /* HIDE, ENABLE, DISABLE */
   int Ident)  /* Button ID */

{  switch (State)
   {  case HIDE:
	 ShowWindow(GetDlgItem(hDlg, Ident), SW_HIDE); break;
		case ENABLE:
      case DISABLE:
	 EnableWindow(GetDlgItem(hDlg, Ident), State);
   }
}

/*******************************************************************
* ShowButtons: Common procedure to Activate, Deactivate, Hide, and
*   Gray controls within the list manager dialog.
*******************************************************************/

void ShowButtons ( 
   HWND hDlg,
   int PB1, 
   int PB2, 
   int PB3, 
   int PB4, 
   int PB5, 
   int CB1,
   int CB2,
   int CB3,
   int PB6)

{  HandleButton ( hDlg, PB1, IDC_PBUTTON1 );
   HandleButton ( hDlg, PB2, IDC_PBUTTON2 );
	HandleButton ( hDlg, PB3, IDC_PBUTTON3 );
   HandleButton ( hDlg, PB4, IDC_PBUTTON4 );
   HandleButton ( hDlg, PB5, IDC_PBUTTON5 );
   HandleButton ( hDlg, PB6, IDC_PBUTTON6 );
	HandleCkBox  ( hDlg, CB1, IDC_CBOX1 );
   HandleCkBox  ( hDlg, CB2, IDC_CBOX2 );
   HandleCkBox  ( hDlg, CB3, IDC_CBOX3 );
}

/*********************************************************************
* DefFactManager: Callback Function for the DefFact
*   List Box Manager.
*********************************************************************/
#if IBM_TBC
#pragma argsused
#endif
BOOL FAR PASCAL DefFactManager (
   HWND       hDlg,
	unsigned   message,
   WORD       wParam,
	LONG       lParam)

  {
#if DEFFACTS_CONSTRUCT
	HWND hListBox;
                         
	hListBox = GetDlgItem ( hDlg, IDC_LISTBOX);

   switch (message)
	  {
		case WM_INITDIALOG:
      {  void *deffactPtr = NULL;
	 int count=0;
	 StartWaitCursor();
	 while ((deffactPtr = GetNextDeffacts ( deffactPtr )) != NULL)
	 {  count ++;
	    SendMessage ( hListBox, LB_ADDSTRING, 0,(LPARAM)(LPCSTR) GetDeffactsName (deffactPtr));
	    WinRunEvent();
	 }
	 StopWaitCursor();
	 SendMessage ( hListBox, LB_SETCURSEL, 0, 0L);
	 sprintf ( DialogName,"Deffacts Manager - %4d Items",count);
	 SetWindowText ( hDlg, (LPSTR)DialogName );
	 ShowButtons ( hDlg, DISABLE, HIDE, HIDE, DISABLE, HIDE,
	    HIDE, HIDE, HIDE, ENABLE);

	 return (TRUE);
      }

      case WM_COMMAND:
      {  switch (wParam)
	 {  case IDC_PBUTTON6:
		 {  EndDialog( hDlg, IDOK);
	       return (TRUE);
	    }
			       /*--------*/
		 case IDC_PBUTTON1: /* Remove */
	    case IDC_PBUTTON4: /* PPrint */
	    case IDC_LISTBOX:  /*--------*/
		 {  int index;
			 int stringsize;
			 void *deffactPtr;
			 HANDLE hString;
			 char *string;
			 extern SCREEN WinDialog;

			 index = (int) SendMessage ( hListBox, LB_GETCURSEL, 0, 0L );


			 stringsize = (int) SendMessage(hListBox, LB_GETTEXTLEN, index, 0);


			 if (stringsize == LB_ERR)
		  break;

			  /* hString = GlobalAlloc(GMEM_FIXED, (WORD) stringsize);
				*/
			hString = GlobalAlloc(GMEM_FIXED, (WORD) stringsize + 1);
			 if ( hString == NULL)
			 {  EndDialog( hDlg, IDOK);
		  ExitToShell();
			 }

			 string = (char *) GlobalLock ( hString );

			 /* OK to here */
			 if (string == NULL)    /* Added by GDR */
				{
				 EndDialog( hDlg, IDOK);
				 ExitToShell();
				}
					
			 SendMessage(hListBox, LB_GETTEXT, index,(LPARAM)((LPSTR) string) );



			 deffactPtr = FindDeffacts ( string );


	       /*----------------+
	       | Remove Deffacts |
	       +----------------*/
	       if ( wParam == IDC_PBUTTON1 )
			 {   int count;

		  Undeffacts ( deffactPtr );
		  SendMessage (hListBox, LB_DELETESTRING, index, 0L );

		  count = (int)  SendMessage (hListBox, LB_GETCOUNT, 0, 0);
		  sprintf ( DialogName,"Deffacts Manager - %4d Items",count);
		  SetWindowText ( hDlg, (LPSTR)DialogName );

		  PrintCLIPS ("wclips", "(undeffacts ");
		  PrintCLIPS ("wclips", string );
		  PrintCLIPS ("wclips", ")\n");
		  PrintPrompt ();
		  SetFocus(hListBox);
		  InvalidateRect (WinDialog.hWnd, NULL, TRUE );
		  GlobalUnlock ( hString );
		  GlobalFree ( hString );
		  break;
	       }

	       /*----------------+
	       | PPrint Deffacts |
	       +----------------*/
	       if ( wParam == IDC_PBUTTON4 )
	       {  PrintCLIPS ("wclips", "(ppdeffacts ");
		  PrintCLIPS ("wclips", string );
		  PrintCLIPS ("wclips", ")\n");
		  PrintCLIPS ("wclips", GetDeffactsPPForm ( deffactPtr ));
		  PrintPrompt ();
		  InvalidateRect (WinDialog.hWnd, NULL, TRUE );
		  SetFocus(hListBox);
	       }
	       ShowButtons ( hDlg,
		  IsDeffactsDeletable(deffactPtr), HIDE, HIDE,
		  ( GetDeffactsPPForm(deffactPtr) != NULL),
		   HIDE, HIDE, HIDE, HIDE, ENABLE );
	       GlobalUnlock ( hString );
	       GlobalFree ( hString );
	       return (TRUE);
	    }
	 }
	 break;
      }
   }
#endif
   return (FALSE);
}

/************************************************************
* DefTemplateManager: Callback Function for the
*   DefTemplate List Box Manager.
************************************************************/
#if IBM_TBC
#pragma argsused
#endif
BOOL FAR PASCAL DefTemplateManager (
   HWND       hDlg,
   unsigned   message,
   WORD       wParam,
   LONG       lParam)

{
#if DEFTEMPLATE_CONSTRUCT
   HWND hListBox = GetDlgItem ( hDlg, IDC_LISTBOX);

   switch (message)
   {  case WM_INITDIALOG:
      {  void *deftemplatePtr = NULL;
	 int count=0;
	 StartWaitCursor();
	 while ((deftemplatePtr = GetNextDeftemplate ( deftemplatePtr )) != NULL)
	 {  count ++;
	    SendMessage ( hListBox, LB_ADDSTRING, 0,(LPARAM)(LPCSTR) GetDeftemplateName (deftemplatePtr));
	    WinRunEvent();
	 }
	 StopWaitCursor();
	 SendMessage ( hListBox, LB_SETCURSEL, 0, 0L);
	 sprintf ( DialogName,"Deftemplate Manager - %4d Items",count);
	 SetWindowText ( hDlg, (LPSTR)DialogName );
	 ShowButtons ( hDlg, DISABLE, HIDE, HIDE, DISABLE, HIDE,
	    GRAY, HIDE, HIDE, ENABLE);

	 return (TRUE);
      }

		case WM_COMMAND:
      {  switch (wParam)
	 {  case IDC_PBUTTON6:
	    {  EndDialog( hDlg, IDOK);
	       return (TRUE);
	    }
			       /*--------*/
	    case IDC_PBUTTON1: /* Remove */
	    case IDC_PBUTTON4: /* PPrint */
	    case IDC_CBOX1:    /* Trace  */
	    case IDC_LISTBOX:  /*--------*/
	    {  int index = (int) SendMessage ( hListBox, LB_GETCURSEL, 0, 0L );
	       int stringsize = (int) SendMessage(hListBox, LB_GETTEXTLEN, index, 0);
	       void *deftemplatePtr;
			 HANDLE hString;
			 char *string;

			 if (stringsize == LB_ERR)
		  break;

	       hString = GlobalAlloc(GMEM_FIXED, (WORD) stringsize+1);
	       if (hString == NULL)
	       {  EndDialog( hDlg, IDOK);
		  ExitToShell();
	       }
	       string = (char *) GlobalLock ( hString );
	       SendMessage( hListBox, LB_GETTEXT, index,(LPARAM)((LPSTR) string) );
	       deftemplatePtr = FindDeftemplate ( string );

	       /*-------------------+
	       | Remove Deftemplate |
	       +-------------------*/
	       if ( wParam == IDC_PBUTTON1 )
			 {  extern SCREEN WinDialog;
			 int count;


		  Undeftemplate ( deftemplatePtr );
		  SendMessage (hListBox, LB_DELETESTRING, index, 0L );

		  count = (int) SendMessage (hListBox, LB_GETCOUNT, 0, 0);
		  sprintf ( DialogName,"Deftemplate Manager - %4d Items",count);
		  SetWindowText ( hDlg, (LPSTR)DialogName );

		  PrintCLIPS ("wclips", "(undeftemplate ");
		  PrintCLIPS ("wclips", string );
		  PrintCLIPS ("wclips", ")\n");
		  PrintPrompt ();
		  InvalidateRect ( WinDialog.hWnd, NULL, TRUE );
		  SetFocus(hListBox);
		  GlobalUnlock ( hString );
		  GlobalFree ( hString );
		  break;
	       }

	       /*-------------------+
	       | PPrint Deftemplate |
	       +-------------------*/
	       if ( wParam == IDC_PBUTTON4 )
	       {  extern SCREEN WinDialog;
		  PrintCLIPS ("wclips", "(ppdeftemplate ");
		  PrintCLIPS ("wclips", string );
		  PrintCLIPS ("wclips", ")\n");
		  PrintCLIPS ("wclips", GetDeftemplatePPForm(deftemplatePtr));
		  PrintPrompt ();
		  InvalidateRect ( WinDialog.hWnd, NULL, TRUE );
		  SetFocus(hListBox);
	       }

	       /*-----------------+
	       | Set-Remove Trace |
	       +-----------------*/
	       if ( wParam == IDC_CBOX1 )
	       {  SetDeftemplateWatch (!GetDeftemplateWatch(deftemplatePtr), deftemplatePtr);
		  SetFocus (hListBox);
	       }

	       /*-------------------+
	       | Toggle Check Boxes |
	       +-------------------*/
	       GlobalUnlock ( hString );
	       GlobalFree ( hString );
	       ShowButtons ( hDlg,
		  IsDeftemplateDeletable (deftemplatePtr), HIDE, HIDE,
		  (GetDeftemplatePPForm ( deftemplatePtr ) != NULL), HIDE,
		  GetDeftemplateWatch(deftemplatePtr), HIDE, HIDE, ENABLE);
	       return (TRUE);
	    }
	 }
	 break;
      }
   }
#endif
   return (FALSE);
}

/************************************************************
* DefFunctionManager: Callback Function for the
*   DefFunction List Box Manager.
************************************************************/
#if IBM_TBC
#pragma argsused
#endif
BOOL FAR PASCAL DefFunctionManager (
   HWND       hDlg,
   unsigned   message,
   WORD       wParam,
   LONG       lParam)

{
#if DEFFUNCTION_CONSTRUCT
   HWND hListBox = GetDlgItem ( hDlg, IDC_LISTBOX);

   switch (message)
   {  case WM_INITDIALOG:
      {  void *DeffunctionPtr = NULL;
	 int count=0;
	 StartWaitCursor();
	 while ((DeffunctionPtr = GetNextDeffunction ( DeffunctionPtr )) != NULL)
	 {  count ++;
	    WinRunEvent();
	    SendMessage ( hListBox, LB_ADDSTRING, 0,(LPARAM)(LPCSTR) GetDeffunctionName (DeffunctionPtr));
	 }
	 StopWaitCursor();
	 SendMessage ( hListBox, LB_SETCURSEL, 0, 0L);
	 sprintf ( DialogName,"Deffunction Manager - %4d Items",count);
	 SetWindowText ( hDlg, (LPSTR)DialogName );
	 ShowButtons ( hDlg, DISABLE, HIDE, HIDE, DISABLE, HIDE,
	    GRAY, HIDE, HIDE, ENABLE);
	 return (TRUE);
      }

      case WM_COMMAND:
      {  switch (wParam)
	 {  case IDC_PBUTTON6:
	    {  EndDialog( hDlg, IDOK);
	       return (TRUE);
	    }
			       /*--------*/
	    case IDC_CBOX1:    /* Trace  */
	    case IDC_PBUTTON1: /* Remove */
	    case IDC_PBUTTON4: /* PPrint */
	    case IDC_LISTBOX:  /*--------*/
	    {  int index = (int) SendMessage ( hListBox, LB_GETCURSEL, 0, 0L );
	       int stringsize = (int) SendMessage(hListBox, LB_GETTEXTLEN, index, 0);
	       void *DeffunctionPtr;
			 HANDLE hString;
			 char *string;

			 if (stringsize == LB_ERR)
		  break;

	       hString = GlobalAlloc(GMEM_FIXED, (WORD) stringsize+1);
	       if ( hString == NULL)
	       {  EndDialog( hDlg, IDOK);
		  ExitToShell();
	       }

	       string = (char *) GlobalLock ( hString );
	       SendMessage( hListBox, LB_GETTEXT, index,(LPARAM)((LPSTR) string) );
	       DeffunctionPtr = FindDeffunction ( string );

	       /*-------------------+
	       | Remove Deffunction |
	       +-------------------*/
	       if ( wParam == IDC_PBUTTON1 )
			 {   int count;
		  extern SCREEN WinDialog;

		  Undeffunction ( DeffunctionPtr );
		  SendMessage (hListBox, LB_DELETESTRING, index, 0L );

		  count = (int) SendMessage (hListBox, LB_GETCOUNT, 0, 0);
		  sprintf ( DialogName,"Deffunction Manager - %4d Items",count);
		  SetWindowText ( hDlg, (LPSTR)DialogName );

		  PrintCLIPS ("wclips", "(undeffunction ");
		  PrintCLIPS ("wclips", string );
		  PrintCLIPS ("wclips", ")\n");
		  PrintPrompt ();
		  InvalidateRect ( WinDialog.hWnd, NULL, TRUE );
		  SetFocus ( hListBox );
		  GlobalUnlock ( hString );
		  GlobalFree ( hString );
		  break;
	       }

	       /*-------------------+
	       | PPrint Deffunction |
	       +-------------------*/
	       if ( wParam == IDC_PBUTTON4 )
	       {  extern SCREEN WinDialog;
		  PrintCLIPS ("wclips", "(ppdeffunction ");
		  PrintCLIPS ("wclips", string );
		  PrintCLIPS ("wclips", ")\n");
		  PrintCLIPS ("wclips", GetDeffunctionPPForm ( DeffunctionPtr ));
		  PrintPrompt ();
		  InvalidateRect (WinDialog.hWnd, NULL, TRUE);
		  SetFocus ( hListBox );
	       }

	       /*-----------------+
	       | Set-Remove Trace |
	       +-----------------*/
	       if ( wParam == IDC_CBOX1 )
	       {  SetDeffunctionWatch (!GetDeffunctionWatch(DeffunctionPtr), DeffunctionPtr);
		  SetFocus (hListBox);
	       }

	       /*-------------------+
	       | Toggle Check Boxes |
	       +-------------------*/
	       ShowButtons ( hDlg,
		   IsDeffunctionDeletable( DeffunctionPtr), HIDE, HIDE,
		   (GetDeffunctionPPForm ( DeffunctionPtr ) != NULL), HIDE,
		   GetDeffunctionWatch(DeffunctionPtr), HIDE, HIDE, ENABLE );
	       GlobalUnlock ( hString );
	       GlobalFree ( hString );
	       return (TRUE);
	    }
	 }
	 break;
      }
   }
#endif
   return (FALSE);
}

/************************************************************
* DefMethodsManager: Callback Function for the
*   DefMethods List Box Manager.
************************************************************/
BOOL FAR PASCAL DefMethodsManager (
   HWND       hDlg,
   unsigned   message,
   WORD       wParam,
   LONG       lParam)

{
#if OBJECT_SYSTEM
   HWND hListBox = GetDlgItem ( hDlg, IDC_LISTBOX);
   static void *defgenericPtr;

   switch (message)
   {  case WM_INITDIALOG:
      {  unsigned theMethod = 0 ;
	 int count=0;
	 char buffer[100];
	 defgenericPtr = (void *)lParam;
	 SetWindowText(GetDlgItem (hDlg, IDC_TEXT), "Methods for - ");
	 StartWaitCursor();
	 while ((theMethod = GetNextDefmethod ( defgenericPtr, theMethod )) != 0)
	 {  char Name[100];
	    int index;
	    GetDefmethodDescription ( Name,50,defgenericPtr,theMethod );
	    index = (int) SendMessage ( hListBox, LB_ADDSTRING, 0,(LPARAM)(LPCSTR) Name);
	    SendMessage ( hListBox,LB_SETITEMDATA,index,theMethod);
	    WinRunEvent();
	    count ++;
	 }
	 StopWaitCursor();
	 SendMessage ( hListBox, LB_SETCURSEL, 0, 0L);
	 sprintf ( DialogName,"Defmethod-Handler Manager - %4d Items (in precedence order)",count);
	 SetWindowText ( hDlg, (LPSTR)DialogName );

	 sprintf ( buffer,"Methods for - <%s>", GetDefgenericName (defgenericPtr));
	 SetWindowText(GetDlgItem (hDlg, IDC_TEXT), buffer);


	 ShowButtons (hDlg, DISABLE, HIDE, HIDE, DISABLE, HIDE,
	    DISABLE, HIDE, HIDE, ENABLE);

	 return (TRUE);
      }

      case WM_COMMAND:
      {  switch (wParam)
	 {  /*------*/
	    /* Done */
	    /*------*/
	    case IDC_PBUTTON6:
	    {  EndDialog( hDlg, IDOK);
	       return (TRUE);
	    }
			       /*-------------------*/
	    case IDC_PBUTTON1: /* Remove Defmethod */
	    case IDC_PBUTTON4: /* PPrint Defmethod */
	    case IDC_CBOX1:    /* Trace Defmethod  */
	    case IDC_LISTBOX:  /*-------------------*/

	    {  int index = (int) SendMessage ( hListBox, LB_GETCURSEL, 0, 0L );
	       unsigned theMethod;
	       extern SCREEN WinDialog;
	       int count = (int) SendMessage(hListBox, LB_GETCOUNT, 0, 0);

	       if (index == LB_ERR )
		  break;

	       theMethod = (unsigned) SendMessage ( hListBox,LB_GETITEMDATA,index ,0);

	       /*-------------------*/
	       /* Remove Defmethod */
	       /*-------------------*/
	       if ( wParam == IDC_PBUTTON1 )
	       {  count--;
		  sprintf ( DialogName,"Defmethod-Handler Manager - %4d Items",count);
		  SetWindowText ( hDlg, (LPSTR)DialogName );
		
		  SendMessage (hListBox, LB_DELETESTRING, index, 0L );
		  FlushCommandString();
		  PrintCLIPS ("wclips", "(undefmethod ");
		  PrintCLIPS ("wclips", GetDefgenericName(defgenericPtr));
		  PrintCLIPS ("wclips"," ");
		  PrintLongInteger ("wclips", (long int) theMethod);
		  PrintCLIPS ("wclips", ")\n");
		  PrintPrompt ();
		  Undefmethod (defgenericPtr, theMethod);
		  InvalidateRect ( WinDialog.hWnd, NULL, TRUE );
		  SetFocus (hListBox);
		  break;
	       }

	       /*-------------------*/
	       /* Pprint Defmethod */
	       /*-------------------*/
	       if ( wParam == IDC_PBUTTON4 )
	       {  FlushCommandString();
		  PrintCLIPS ("wclips", "(ppdefmethod ");
		  PrintCLIPS ("wclips", GetDefgenericName(defgenericPtr));
		  PrintCLIPS ("wclips"," ");
		  PrintLongInteger ("wclips", (long int) theMethod);
		  PrintCLIPS ("wclips", ")\n");
		  PrintCLIPS ("wclips", GetDefmethodPPForm ( defgenericPtr, theMethod ));
		  PrintPrompt ();
		  InvalidateRect ( WinDialog.hWnd, NULL, TRUE );
		  SetFocus (hListBox);
	       }

	       /*----------------------------+
	       | Set-Remove Defmethod Trace |
	       +----------------------------*/
	       if ( wParam == IDC_CBOX1 )
	       {  SetDefmethodWatch(
		     !GetDefmethodWatch(defgenericPtr, theMethod),
		     defgenericPtr, theMethod);
		  SetFocus (hListBox);
	       }

	       ShowButtons (hDlg,
		  IsDefmethodDeletable(defgenericPtr, theMethod),
		  HIDE, HIDE,
		  (GetDefmethodPPForm(defgenericPtr, theMethod)!= NULL),
		  HIDE,
		  GetDefmethodWatch(defgenericPtr, theMethod),
		  HIDE, HIDE, ENABLE);
	       return (TRUE);
	    }
	 }
	 break;
      }
   }
#endif
   return (FALSE);
}

/************************************************************
* DefInstanceManager: Callback Function for the
*   DefInstance List Box Manager.
************************************************************/
#if IBM_TBC
#pragma argsused
#endif
BOOL FAR PASCAL DefInstanceManager (
   HWND       hDlg,
   unsigned   message,
   WORD       wParam,
   LONG       lParam)

{
#if OBJECT_SYSTEM
   HWND hListBox = GetDlgItem ( hDlg, IDC_LISTBOX);

   switch (message)
   {  case WM_INITDIALOG:
      {  void *definstancesPtr = NULL;
	 int count=0;
	 StartWaitCursor();
	 while ((definstancesPtr = GetNextDefinstances ( definstancesPtr )) != NULL)
	 {  count ++;
	    WinRunEvent();
	    SendMessage ( hListBox, LB_ADDSTRING, 0,(LPARAM)(LPCSTR) GetDefinstancesName (definstancesPtr));
	 }
	 StopWaitCursor();
	 SendMessage ( hListBox, LB_SETCURSEL, 0, 0L);
	 sprintf ( DialogName,"Definstances Manager - %4d Items",count);
	 SetWindowText ( hDlg, (LPSTR)DialogName );

	 ShowButtons ( hDlg, DISABLE, HIDE, HIDE, DISABLE, HIDE,
	    HIDE, HIDE, HIDE, ENABLE );
	 return (TRUE);
      }

      case WM_COMMAND:
      {  switch (wParam)
	 {  case IDC_PBUTTON6:
	    {  EndDialog( hDlg, IDOK);
	       return (TRUE);
	    }
			       /*--------*/
	    case IDC_PBUTTON1: /* Remove */
	    case IDC_PBUTTON4: /* PPrint */
	    case IDC_LISTBOX:  /*--------*/
	    {  int index = (int) SendMessage ( hListBox, LB_GETCURSEL, 0, 0L );
	       int stringsize = (int) SendMessage(hListBox, LB_GETTEXTLEN, index, 0);
	       void *definstancesPtr;
			 char *string;
			 HANDLE hString;

	       if (stringsize == LB_ERR)
		  break;

	       hString = GlobalAlloc(GMEM_FIXED, (WORD) stringsize+1);
	       if ( hString == NULL )
	       {  EndDialog( hDlg, IDOK);
		  ExitToShell();
	       }

	       string = (char *) GlobalLock ( hString );

	       SendMessage( hListBox, LB_GETTEXT, index,(LPARAM)((LPSTR) string) );
	       definstancesPtr = FindDefinstances ( string );

	       /*--------------------+
	       | Remove Definstances |
	       +--------------------*/
	       if ( wParam == IDC_PBUTTON1 )
			 {           int count;
		  extern SCREEN WinDialog;

		  Undefinstances ( definstancesPtr );
		  SendMessage (hListBox, LB_DELETESTRING, index, 0L );

		  count = (int) SendMessage (hListBox, LB_GETCOUNT, 0, 0);
		  sprintf ( DialogName,"Definstances Manager - %4d Items",count);
		  SetWindowText ( hDlg, (LPSTR)DialogName );

		  PrintCLIPS ("wclips", "(undefinstances ");
		  PrintCLIPS ("wclips", string );
		  PrintCLIPS ("wclips", ")\n");
		  PrintPrompt ();
		  InvalidateRect ( WinDialog.hWnd, NULL, TRUE );
		  GlobalUnlock ( hString );
		  GlobalFree ( hString );
		  SetFocus ( hListBox );
		  break;
	       }

	       /*----------------+
	       | PPrint Deffacts |
	       +----------------*/
	       if ( wParam == IDC_PBUTTON4 )
	       {  extern SCREEN WinDialog;
		  PrintCLIPS ("wclips", "(ppdefinstances ");
		  PrintCLIPS ("wclips", string );
		  PrintCLIPS ("wclips", ")\n");
		  PrintCLIPS ("wclips", GetDefinstancesPPForm ( definstancesPtr ));
		  PrintPrompt ();
		  InvalidateRect ( WinDialog.hWnd, NULL, TRUE );
		  SetFocus ( hListBox );
	       }

	       GlobalUnlock ( hString );
	       GlobalFree ( hString );
	       ShowButtons ( hDlg,
		  IsDefinstancesDeletable(definstancesPtr), HIDE, HIDE,
		  (GetDefinstancesPPForm (definstancesPtr)!= NULL),
		   HIDE,
		   HIDE, HIDE, HIDE, ENABLE );
	       return (TRUE);
	    }
	 }
	 break;
      }
   }
#endif
   return (FALSE);
}

/************************************************************
* DefClassManager: Callback Function for the DefClass
*   List Box Manager.
************************************************************/
#if IBM_TBC
#pragma argsused
#endif
BOOL FAR PASCAL DefClassManager (
   HWND       hDlg,
   unsigned   message,
   WORD       wParam,
   LONG       lParam)

{
#if OBJECT_SYSTEM
   HWND hListBox = GetDlgItem ( hDlg, IDC_LISTBOX);

   switch (message)
   {  case WM_INITDIALOG:
      {  void *defclassPtr = NULL;
	 int count=0;
	 StartWaitCursor();
	 while ((defclassPtr = GetNextDefclass ( defclassPtr )) != NULL)
	 {  count ++;
	    WinRunEvent();
	    SendMessage ( hListBox, LB_ADDSTRING, 0,(LPARAM)(LPCSTR) GetDefclassName (defclassPtr));
	 }
	 StopWaitCursor();
	 SendMessage ( hListBox, LB_SETCURSEL, 0, 0L);
	 sprintf ( DialogName,"Defclass Manager - %4d Items",count);
	 SetWindowText ( hDlg, (LPSTR)DialogName );
	 ShowButtons ( hDlg, DISABLE, DISABLE ,DISABLE , DISABLE, DISABLE,
	    HIDE, GRAY, GRAY, ENABLE);
	 SetWindowText(GetDlgItem (hDlg, IDC_PBUTTON2), "&Describe");
	 SetWindowText(GetDlgItem (hDlg, IDC_PBUTTON3), "&Browse");
	 SetWindowText ( GetDlgItem ( hDlg, IDC_CBOX2 ), "Watch &Instances" );
	 return (TRUE);
      }

      case WM_COMMAND:
      {  switch (wParam)
	 {  /*-----+
	    | Done |
	    +-----*/
	    case IDC_PBUTTON6:
	    {  EndDialog( hDlg, IDOK);
	       return (TRUE);
	    }
			       /*--------------------*/
	    case IDC_CBOX2:    /* Trace Instances    */
	    case IDC_CBOX3:    /* Trace Slot Changes */
	    case IDC_PBUTTON1: /* Remove             */
	    case IDC_PBUTTON2: /* Describe           */
	    case IDC_PBUTTON3: /* Browse             */
	    case IDC_PBUTTON4: /* PPrint             */
	    case IDC_PBUTTON5: /* Message Handlers   */
	    case IDC_LISTBOX:  /*--------------------*/
	    {  int index = (int) SendMessage ( hListBox, LB_GETCURSEL, 0, 0L );
	       int stringsize = (int) SendMessage(hListBox, LB_GETTEXTLEN, index, 0);
	       void *defclassPtr;
			 char *string;
			 HANDLE hString;

	       if (stringsize == LB_ERR)
		  break;

	       hString = GlobalAlloc(GMEM_FIXED, (WORD) stringsize+1);
	       if (hString == NULL)
	       {  EndDialog( hDlg, IDOK);
		  ExitToShell();
	       }

	       string = (char *) GlobalLock ( hString );
	       SendMessage( hListBox, LB_GETTEXT, index,(LPARAM)((LPSTR) string));
	       defclassPtr = FindDefclass ( string );

	       /*------------------+
	       | Remove Defclasses |
	       +------------------*/
	       if ( wParam == IDC_PBUTTON1 )
			 {       int count;
		  extern SCREEN WinDialog;

		  Undefclass ( defclassPtr );
		  SendMessage (hListBox, LB_DELETESTRING, index, 0L );

		  count = (int) SendMessage (hListBox, LB_GETCOUNT, 0, 0);
		  sprintf ( DialogName,"Defclasss Manager - %4d Items",count);
		  SetWindowText ( hDlg, (LPSTR)DialogName );

		  PrintCLIPS ("wclips", "(undefclass ");
		  PrintCLIPS ("wclips", string );
		  PrintCLIPS ("wclips", ")\n");
		  PrintPrompt ();
		  InvalidateRect ( WinDialog.hWnd, NULL, TRUE );
		  SetFocus ( hListBox );
		  GlobalUnlock ( hString );
		  GlobalFree ( hString );
		  break;
	       }

	       /*--------------------+
	       | Describe Defclasses |
	       +--------------------*/
	       if ( wParam == IDC_PBUTTON2 )
	       {  extern SCREEN WinDialog;

		  PrintCLIPS ("wclips", "(describe-class ");
		  PrintCLIPS ("wclips", string );
		  PrintCLIPS ("wclips", ")\n");
		  DescribeClass ("wclips", defclassPtr );
		  PrintPrompt ();
		  InvalidateRect ( WinDialog.hWnd, NULL, TRUE );
	       }

	       /*------------------+
	       | Browse Defclasses |
	       +------------------*/
	       if ( wParam == IDC_PBUTTON3 )
	       {  extern SCREEN WinDialog;

		  PrintCLIPS ("wclips", "(browse-classes ");
		  PrintCLIPS ("wclips", string );
		  PrintCLIPS ("wclips", ")\n");
		  BrowseClasses ("wclips" ,defclassPtr );
		  PrintPrompt ();
		  InvalidateRect ( WinDialog.hWnd, NULL, TRUE );
	       }

	       /*------------------+
	       | PPrint Defclasses |
	       +------------------*/
	       if ( wParam == IDC_PBUTTON4 )
	       {  extern SCREEN WinDialog;
		  PrintCLIPS ("wclips", "(ppdefclass ");
		  PrintCLIPS ("wclips", string );
		  PrintCLIPS ("wclips", ")\n");
		  PrintCLIPS ("wclips", GetDefclassPPForm ( defclassPtr ));
		  PrintPrompt ();
		  InvalidateRect ( WinDialog.hWnd, NULL, TRUE );
	       }

	       /*----------------+
	       | Message Handler |
	       +----------------*/
	       if ( wParam == IDC_PBUTTON5 )
	       {  DLGPROC lpProc;
		  extern HANDLE hInst;

		  lpProc = MakeProcInstance((DLGPROC) DefMessageManager, hInst);
		  DialogBoxParam((HINSTANCE) hInst,"List_Manager", hDlg, lpProc, (LPARAM)defclassPtr);
		  FreeProcInstance(lpProc);
	       }

	       /*---------------------------+
	       | Set-Remove Trace Instances |
	       +---------------------------*/
	       if ( wParam == IDC_CBOX2 )
	       {  SetDefclassWatchInstances(!GetDefclassWatchInstances(defclassPtr), defclassPtr);
		  SetFocus (hListBox);
	       }

	       /*------------------------------+
	       | Set-Remove Trace Slot Changes |
	       +------------------------------*/
	       if ( wParam == IDC_CBOX3 )
	       {  SetDefclassWatchSlots (!GetDefclassWatchSlots(defclassPtr), defclassPtr);
		  SetFocus (hListBox);
	       }

	       /*-------------------+
	       | Update Check Boxes |
	       +-------------------*/
	       {  int Intstances = GRAY;
		  int Slots = GRAY;

		  if ( !(ClassAbstractP(defclassPtr)))
		  {  Intstances = GetDefclassWatchInstances(defclassPtr);
		     Slots = GetDefclassWatchSlots(defclassPtr);
		  }
		  ShowButtons ( hDlg,
		     IsDefclassDeletable(defclassPtr), ENABLE, ENABLE,
		     ( GetDefclassPPForm(defclassPtr)!= NULL),
		     ( GetNextDefmessageHandler(defclassPtr,0) != 0),
		     HIDE,
		     Intstances,
		     Slots,
		     ENABLE);
	       }
	       GlobalUnlock ( hString );
	       GlobalFree ( hString );
	       return ( TRUE );
	    }
	 }
	 break;
      }
   }
#endif
   return (FALSE);
}

/************************************************************
* DefMessageManager: Callback Function for the DefMessage
*   List Box Manager.
************************************************************/
BOOL FAR PASCAL DefMessageManager (
   HWND       hDlg,
   unsigned   message,
   WORD       wParam,
   LONG       lParam)

{
#if OBJECT_SYSTEM
   HWND hListBox = GetDlgItem ( hDlg, IDC_LISTBOX);
   static void *defclassPtr;

   switch (message)
   {  case WM_INITDIALOG:
      {  unsigned handlerIndex = 0 ;
	 int count=0;
	 char buffer[100];
	 defclassPtr = (void *)lParam;
	 StartWaitCursor();
	 while ((handlerIndex = GetNextDefmessageHandler ( defclassPtr, handlerIndex )) != 0)
	 {  char Name[100];
	    int index;
	    sprintf (Name,"%s (%s)",
	       GetDefmessageHandlerName (defclassPtr, handlerIndex),
	       GetDefmessageHandlerType ( defclassPtr, handlerIndex));
	    index = (int) SendMessage ( hListBox, LB_ADDSTRING, 0,(LPARAM)(LPCSTR) Name);
	    SendMessage ( hListBox,LB_SETITEMDATA,index,handlerIndex);
	    WinRunEvent();
	    count ++;
	 }
	 StopWaitCursor();

	 SendMessage ( hListBox, LB_SETCURSEL, 0, 0L);
	 sprintf ( DialogName,"Defmessage-Handler Manager - %4d Items",count);

	 sprintf ( buffer,"Message Handler for - <%s>",GetDefclassName(defclassPtr));
	 SetWindowText(GetDlgItem (hDlg, IDC_TEXT), buffer);

	 SetWindowText ( hDlg, (LPSTR)DialogName );
	 ShowButtons (hDlg, DISABLE, HIDE, HIDE, DISABLE, HIDE,
	    DISABLE, HIDE, HIDE, ENABLE);

	 return (TRUE);
      }

      case WM_COMMAND:
      {  switch (wParam)
	 {  /*------*/
	    /* Done */
	    /*------*/
	    case IDC_PBUTTON6:
	    {  EndDialog( hDlg, IDOK);
	       return (TRUE);
	    }
			       /*-------------------*/
	    case IDC_PBUTTON1: /* Remove Defmessage */
	    case IDC_PBUTTON4: /* PPrint Defmessage */
	    case IDC_CBOX1:    /* Trace Defmessage  */
	    case IDC_LISTBOX:  /*-------------------*/

	    {  int index =(int) SendMessage ( hListBox, LB_GETCURSEL, 0, 0L );
	       unsigned handlerIndex;
	       extern SCREEN WinDialog;
	       int count = (int) SendMessage(hListBox, LB_GETCOUNT, 0, 0);

	       if (index == LB_ERR )
		  break;

	       handlerIndex = (unsigned) SendMessage ( hListBox,LB_GETITEMDATA,index ,0);

	       /*-------------------*/
	       /* Remove Defmessage */
	       /*-------------------*/
	       if ( wParam == IDC_PBUTTON1 )
			 {
		  count--;
		  sprintf ( DialogName,"Defmessage-Handler Manager - %4d Items",count);
		  SetWindowText ( hDlg, (LPSTR)DialogName );

		  SendMessage (hListBox, LB_DELETESTRING, index, 0L );
		  UndefmessageHandler ( defclassPtr, handlerIndex);
		  PrintPrompt ();
		  InvalidateRect ( WinDialog.hWnd, NULL, TRUE );
		  SetFocus (hListBox);
		  break;  

	       }

	       /*-------------------*/
	       /* Pprint Defmessage */
	       /*-------------------*/
	       if ( wParam == IDC_PBUTTON4 )
	       {  PrintCLIPS ("wclips", "(ppdefmessage-handler ");
		  PrintCLIPS ("wclips", GetDefclassName(defclassPtr));
		  PrintCLIPS ("wclips"," ");
		  PrintCLIPS ("wclips", GetDefmessageHandlerName(defclassPtr, handlerIndex));
		  PrintCLIPS ("wclips"," ");
		  PrintCLIPS ("wclips", GetDefmessageHandlerType(defclassPtr, handlerIndex));
		  PrintCLIPS ("wclips", ")\n");
		  PrintCLIPS ("wclips", GetDefmessageHandlerPPForm ( defclassPtr, handlerIndex ));
		  PrintPrompt ();
		  InvalidateRect ( WinDialog.hWnd, NULL, TRUE );
		  SetFocus (hListBox);
	       }

	       /*----------------------------+
	       | Set-Remove Defmessage Trace |
	       +----------------------------*/
	       if ( wParam == IDC_CBOX1 )
	       {  SetDefmessageHandlerWatch(
		     !GetDefmessageHandlerWatch(defclassPtr, handlerIndex),
		     defclassPtr, handlerIndex);
		  SetFocus (hListBox);
	       }

	       ShowButtons (hDlg,
		  IsDefmessageHandlerDeletable(defclassPtr, handlerIndex),
		  HIDE, HIDE,
		  (GetDefmessageHandlerPPForm(defclassPtr, handlerIndex)!= NULL),
		  HIDE,
		  GetDefmessageHandlerWatch(defclassPtr, handlerIndex),
		  HIDE, HIDE, ENABLE);
	       return (TRUE);
	    }
	 }
	 break;
      }
   }
#endif
   return (FALSE);
}

/************************************************************
* AgendaManager: Callback Function for the Agenda
*   List Box Manager.
************************************************************/
#if IBM_TBC
#pragma argsused
#endif
BOOL FAR PASCAL AgendaManager (
   HWND       hDlg,
   unsigned   message,
   WORD       wParam,
   LONG       lParam)

{
#if DEFRULE_CONSTRUCT
   HWND hListBox = GetDlgItem ( hDlg, IDC_LISTBOX);

   switch (message)
   {  case WM_INITDIALOG:
      {  void *activationPtr = NULL;
	 int count=0;

	 ShowButtons   ( hDlg, DISABLE, HIDE, HIDE, DISABLE, HIDE,
	    HIDE, HIDE, HIDE, ENABLE);
	 SetWindowText ( GetDlgItem ( hDlg, IDC_PBUTTON4 ), "&Fire" );

	 StartWaitCursor();
	 while((activationPtr=GetNextActivation(activationPtr)) != NULL)
	 {  char Buffer[200];
	    int index;

	    GetActivationPPForm (Buffer, 199 ,activationPtr);
	    index = (int) SendMessage ( hListBox, LB_ADDSTRING, 0,(LPARAM)(LPCSTR) Buffer);
	    SendMessage ( hListBox, LB_SETITEMDATA, (WPARAM)index, (LPARAM)activationPtr );
	    WinRunEvent();
	    count++;
	 }
	 StopWaitCursor();
	 SendMessage ( hListBox, LB_SETCURSEL, 0, 0L);
	 sprintf ( DialogName,"Agenda Manager - %4d Items",count);
	 SetWindowText ( hDlg, (LPSTR)DialogName );

	 return (TRUE);
      }

      case WM_COMMAND:
      {  switch (wParam)
	 {  /*-----+
	    | Done |
	    +-----*/
	    case IDC_PBUTTON6:
	    {  EndDialog( hDlg, IDOK);
	       return (TRUE);
	    }
			       /*-------------------*/
	    case IDC_PBUTTON1: /* Remove Activation */
	    case IDC_PBUTTON4: /* Fire Activation   */
	    case IDC_LISTBOX:  /*-------------------*/
	    {  int index = (int) SendMessage ( hListBox, LB_GETCURSEL, 0, 0L );
	       extern SCREEN WinAgenda;
	       void *activationPtr;
	       int count;

	       if (index == LB_ERR ) break;

	       activationPtr = (void *)SendMessage (hListBox, LB_GETITEMDATA, (WPARAM)index, 0L);

	       /*------------------+
	       | Remove Activation |
	       +------------------*/
	       if ( wParam == IDC_PBUTTON1 )
	       {  SendMessage (hListBox, LB_DELETESTRING, (WPARAM)index, 0L );
		  DeleteActivation ( activationPtr );

		  count = (int) SendMessage(hListBox, LB_GETCOUNT, 0, 0);
		  sprintf ( DialogName,"Agenda Manager - %4d Items",count);
		  SetWindowText ( hDlg, (LPSTR)DialogName );

		  InvalidateRect (WinAgenda.hWnd, NULL, TRUE );
		  SetFocus ( hListBox );
	       }

	       /*-----+
	       | Fire |
	       +-----*/
	       if ( wParam == IDC_PBUTTON4 )
	       {  MoveActivationToTop ( activationPtr );
		  InvalidateRect ( WinAgenda.hWnd, NULL, TRUE );
		  SetAgendaChanged (FALSE);
		  FlushCommandString();
		  SetCommandString ("(run 1)\n");
		  PrintCLIPS ("stdout","(run 1)\n");
		  SendMessage ( hDlg, WM_COMMAND, IDC_PBUTTON6, 0L );
		  break;
	       }

	       ShowButtons ( hDlg,
		  ENABLE, HIDE, HIDE,
		  ENABLE, HIDE,
		  HIDE, HIDE, HIDE,
		  ENABLE);
	       return (TRUE);
	    }
	 }
	 break;
      }
   }
#endif
   return (FALSE);
}

/************************************************************
* DefGlobalManager: Callback Function for the DefGeneric List
*   Box Manager.
************************************************************/
#if IBM_TBC
#pragma argsused
#endif
BOOL FAR PASCAL DefGlobalManager (
   HWND       hDlg,
   unsigned   message,
   WORD       wParam,
   LONG       lParam)

{
#if DEFGLOBAL_CONSTRUCT
   HWND hListBox = GetDlgItem ( hDlg, IDC_LISTBOX);

   switch (message)
   {  case WM_INITDIALOG:
      {  int count = 0;
	 void *Ptr = NULL;

	 StartWaitCursor();
	 while ((Ptr = GetNextDefglobal ( Ptr )) != NULL)
	 {  count ++;
	    SendMessage ( hListBox, LB_ADDSTRING, 0,(LPARAM)(LPCSTR) GetDefglobalName (Ptr));
	    WinRunEvent();
	 }
	 StopWaitCursor();
	 SendMessage ( hListBox, LB_SETCURSEL, 0, 0L );
	 sprintf ( DialogName,"Defglobal Manager - %4d Items",count);
	 SetWindowText( hDlg, (LPSTR)DialogName );
	 ShowButtons ( hDlg, DISABLE, HIDE, HIDE, DISABLE, HIDE,
	    HIDE, HIDE, HIDE, ENABLE);
	 return (TRUE);
      }

      case WM_COMMAND:
      {  switch (wParam)
	 {  case IDC_PBUTTON6: /* Done */
	    {  EndDialog( hDlg, IDOK);
	       return (TRUE);
	    }
			       /*------------------------------*/
	    case IDC_PBUTTON1: /* Remove                       */
	    case IDC_PBUTTON4: /* PPrint                       */
	    case IDC_LISTBOX:  /*------------------------------*/

	    {  int index = (int) SendMessage ( hListBox, LB_GETCURSEL, 0, 0L );
	       int stringsize = (int) SendMessage(hListBox, LB_GETTEXTLEN, index, 0);
	       void *Ptr;
	       HANDLE hString;
			 char *string;
			 extern SCREEN WinDialog;

	       if (stringsize == LB_ERR)
		  break;
				
	       hString = GlobalAlloc(GMEM_FIXED, (WORD) stringsize+1);
	       if ( hString == NULL)
	       {  EndDialog( hDlg, IDOK);
		  ExitToShell();
	       }

	       string =  (char *) GlobalLock ( hString );
	       SendMessage( hListBox, LB_GETTEXT, index,(LPARAM)((LPSTR) string) );
	       Ptr = FindDefglobal ( string );

	       if (Ptr == NULL) break;
	       /*-----------------+
	       | Remove Defglobal |
	       +-----------------*/
	       if ( wParam == IDC_PBUTTON1 )
			 {     int count;

		  Undefglobal ( Ptr );
		  SendMessage (hListBox, LB_DELETESTRING, index, 0L );

		  count = (int)  SendMessage (hListBox, LB_GETCOUNT, 0, 0);
		  sprintf ( DialogName,"Defglobal Manager - %4d Items",count);
		  SetWindowText ( hDlg, (LPSTR)DialogName );

		  PrintCLIPS ("wclips", "(undefglobal ");
		  PrintCLIPS ("wclips", string );
		  PrintCLIPS ("wclips", ")\n");
		  PrintPrompt ();
		  InvalidateRect (WinDialog.hWnd, NULL, TRUE );
		  SetFocus (hListBox);
		  GlobalUnlock ( hString );
		  GlobalFree ( hString );
		  break;
	       }

	       /*-----------------+
	       | PPrint Defglobal |
	       +-----------------*/
	       if ( wParam == IDC_PBUTTON4 )
	       {  PrintCLIPS ("wclips", "(ppdefglobal ");
		  PrintCLIPS ("wclips", string );
		  PrintCLIPS ("wclips", ")\n");
		  PrintCLIPS ("wclips", GetDefglobalPPForm ( Ptr ));
		  PrintPrompt ();
		  InvalidateRect (WinDialog.hWnd, NULL, TRUE );
		  SetFocus (hListBox);
	       }

	       /*-------------------+
	       | Update Check Boxes |
	       +-------------------*/
	       ShowButtons ( hDlg,
		  IsDefglobalDeletable (Ptr), HIDE, HIDE,
		  (GetDefglobalPPForm (Ptr) != NULL),
		  HIDE, HIDE, HIDE, HIDE, ENABLE);
	       GlobalUnlock ( hString );
	       GlobalFree ( hString );
	       return (TRUE);
	    }
	 }
	 break;
      }
   }
#endif
   return (FALSE);
}

/************************************************************
* DefRuleManager: Callback Function for the DefRule List
*   Box Manager.
************************************************************/
#if IBM_TBC
#pragma argsused
#endif
BOOL FAR PASCAL DefRuleManager (
   HWND       hDlg,
   unsigned   message,
   WORD       wParam,
   LONG       lParam)

{
#if DEFRULE_CONSTRUCT
   HWND hListBox = GetDlgItem ( hDlg, IDC_LISTBOX);

   switch (message)
   {  case WM_INITDIALOG:
      {  int count = 0;
	 void *defrulePtr = NULL;

	 StartWaitCursor();
	 while ((defrulePtr = GetNextDefrule ( defrulePtr )) != NULL)
	 {  count ++;
	    SendMessage ( hListBox, LB_ADDSTRING, 0,(LPARAM)(LPCSTR) GetDefruleName (defrulePtr));
	    WinRunEvent();
	 }
	 StopWaitCursor();
	 SendMessage ( hListBox, LB_SETCURSEL, 0, 0L );
	 sprintf ( DialogName,"Defrule Manager - %4d Items",count);
	 SetWindowText( hDlg, (LPSTR)DialogName );
	 ShowButtons  ( hDlg, DISABLE, DISABLE, DISABLE, DISABLE, HIDE,
	    GRAY, GRAY, GRAY, ENABLE );
	 SetWindowText ( GetDlgItem ( hDlg, IDC_CBOX1 ), "&Breakpoint" );
	 SetWindowText ( GetDlgItem ( hDlg, IDC_CBOX3 ), "Watch &Firings" );
	 return (TRUE);
      }

      case WM_COMMAND:
      {  switch (wParam)
	 {  case IDC_PBUTTON6:
	    {  EndDialog( hDlg, IDOK);
	       return (TRUE);
	    }
			       /*------------------------------*/
	    case IDC_PBUTTON1: /* Remove                       */
	    case IDC_PBUTTON2: /* Refresh                      */
	    case IDC_PBUTTON3: /* Matches                      */
	    case IDC_PBUTTON4: /* PPrint                       */ 
	    case IDC_CBOX1:    /* Remove-Set Break             */
	    case IDC_CBOX2:    /* Remove-Set Trace Activations */
	    case IDC_CBOX3:    /* Remove-Set Trace Firings     */
	    case IDC_LISTBOX:  /*------------------------------*/

	    {  int index = (int) SendMessage ( hListBox, LB_GETCURSEL, 0, 0L );
	       int stringsize = (int) SendMessage(hListBox, LB_GETTEXTLEN, index, 0);
	       void *defrulePtr;
	       HANDLE hString;
			 char *string;
			 extern SCREEN WinDialog;

	       if (stringsize == LB_ERR)
		  break;
				
	       hString = GlobalAlloc(GMEM_FIXED, (WORD) stringsize+1);
	       if ( hString == NULL)
	       {  EndDialog( hDlg, IDOK);
		  ExitToShell();
	       }

	       string =  (char *) GlobalLock ( hString );
	       SendMessage( hListBox, LB_GETTEXT, index,(LPARAM)((LPSTR) string) );
	       defrulePtr = FindDefrule ( string );

	       if (defrulePtr == NULL) break;
	       /*---------------+
	       | Remove Defrule |
	       +---------------*/
	       if ( wParam == IDC_PBUTTON1 )
			 {  int count;

		  Undefrule ( defrulePtr );
		  SendMessage (hListBox, LB_DELETESTRING, index, 0L );

		  count = (int)  SendMessage (hListBox, LB_GETCOUNT, 0, 0);
		  sprintf ( DialogName,"Defrule Manager - %4d Items",count);
		  SetWindowText ( hDlg, (LPSTR)DialogName );

		  PrintCLIPS ("wclips", "(undefrule ");
		  PrintCLIPS ("wclips", string );
		  PrintCLIPS ("wclips", ")\n");
		  PrintPrompt ();
		  InvalidateRect (WinDialog.hWnd, NULL, TRUE );
		  SetFocus (hListBox);
		  GlobalUnlock ( hString );
		  GlobalFree ( hString );
		  break;
	       }

	       /*----------------+
	       | Refresh Defrule |
	       +----------------*/
	       if ( wParam == IDC_PBUTTON2 )
	       {  Refresh ( defrulePtr );
		  PrintCLIPS ("wclips", "(refresh ");
		  PrintCLIPS ("wclips", string );
		  PrintCLIPS ("wclips", ")\n");
		  PrintPrompt ();
		  InvalidateRect (WinDialog.hWnd, NULL, TRUE );
		  SetFocus (hListBox);
	       }

	       /*--------------+
	       | Match Defrule |
	       +--------------*/
	       if ( wParam == IDC_PBUTTON3 )
	       {  PrintCLIPS ("wclips", "(matches ");
		  PrintCLIPS ("wclips", string );
		  PrintCLIPS ("wclips", ")\n");
		  Matches ( defrulePtr );
		  PrintPrompt ();
		  InvalidateRect (WinDialog.hWnd, NULL, TRUE );
		  SetFocus ( hListBox );
	       }

	       /*---------------+
	       | PPrint Defrule |
	       +---------------*/
	       if ( wParam == IDC_PBUTTON4 )
	       {  PrintCLIPS ("wclips", "(ppdefrule ");
		  PrintCLIPS ("wclips", string );
		  PrintCLIPS ("wclips", ")\n");
		  PrintCLIPS ("wclips", GetDefrulePPForm ( defrulePtr ));
		  PrintPrompt ();
		  InvalidateRect (WinDialog.hWnd, NULL, TRUE );
		  SetFocus (hListBox);
	       }

	       /*----------------------------+
	       | Set-Remove Break on Defrule |
	       +----------------------------*/
	       if ( wParam == IDC_CBOX1 )
	       {  if ( RemoveBreak(defrulePtr))
		  {  RemoveBreak(defrulePtr);
		     PrintCLIPS ("wclips", "(remove-break ");
		  }
		  else
		  {  SetBreak(defrulePtr);
		     PrintCLIPS ("wclips", "(set-break ");
		  }
		  PrintCLIPS ("wclips", string );
		  PrintCLIPS ("wclips", ")\n");
		  PrintPrompt ();
		  SetFocus (hListBox);
	       }

	       /*----------------------------+
	       | Set-Remove Trace Activation |
	       +----------------------------*/
	       if ( wParam == IDC_CBOX2 )
	       {  SetDefruleWatchActivations (!GetDefruleWatchActivations(defrulePtr), defrulePtr);
		  SetFocus (hListBox);
	       }

	       /*-------------------------+
	       | Set-Remove Trace Firings |
	       +-------------------------*/
	       if ( wParam == IDC_CBOX3 )
	       {  SetDefruleWatchFirings (!GetDefruleWatchFirings(defrulePtr), defrulePtr);
		  SetFocus (hListBox);
	       }

	       /*-------------------+
	       | Update Check Boxes |
	       +-------------------*/
	       ShowButtons ( hDlg,
		  IsDefruleDeletable (defrulePtr), ENABLE, ENABLE,
		  (GetDefrulePPForm ( defrulePtr ) != NULL),
		  HIDE,
		  DefruleHasBreakpoint(defrulePtr),
		  GetDefruleWatchActivations(defrulePtr),
		  GetDefruleWatchFirings(defrulePtr),
		  ENABLE);
	       GlobalUnlock ( hString );
	       GlobalFree ( hString );
	       return (TRUE);
	    }
	 }
	 break;
      }
   }
#endif
   return (FALSE);
}

/************************************************************
* DefGenericManager: Callback Function for the DefGeneric
*   List Box Manager.
************************************************************/
#if IBM_TBC
#pragma argsused
#endif
BOOL FAR PASCAL DefGenericManager (
   HWND       hDlg,
   unsigned   message,
   WORD       wParam,
   LONG       lParam)

{
#if DEFGENERIC_CONSTRUCT
   HWND hListBox = GetDlgItem ( hDlg, IDC_LISTBOX);

   switch (message)
   {  case WM_INITDIALOG:
      {  void *defPtr = NULL;
	 int count=0;

	 StartWaitCursor();
	 while ((defPtr = GetNextDefgeneric ( defPtr )) != NULL)
	 {  count ++;
	    WinRunEvent();
	    SendMessage ( hListBox, LB_ADDSTRING, 0,(LPARAM)(LPCSTR) GetDefgenericName (defPtr));
	 }
	 StopWaitCursor();
	 SendMessage ( hListBox, LB_SETCURSEL, 0, 0L);
	 sprintf ( DialogName,"Defgeneric Manager - %4d Items",count);
	 SetWindowText ( hDlg, (LPSTR)DialogName );
	 ShowButtons ( hDlg, DISABLE, HIDE ,HIDE , DISABLE, DISABLE,
	    GRAY, HIDE, HIDE, ENABLE);
	 SetWindowText(GetDlgItem (hDlg, IDC_PBUTTON5), "&Methods");
	 return (TRUE);
      }

      case WM_COMMAND:
      {  switch (wParam)
	 {  /*-----+
	    | Done |
	    +-----*/
	    case IDC_PBUTTON6:
	    {  EndDialog( hDlg, IDOK);
	       return (TRUE);
	    }
			       /*---------*/
	    case IDC_CBOX1:    /* Trace   */
	    case IDC_PBUTTON1: /* Remove  */
	    case IDC_PBUTTON4: /* PPrint  */
	    case IDC_PBUTTON5: /* Methods */
	    case IDC_LISTBOX:  /*---------*/
	    {  int index = (int) SendMessage ( hListBox, LB_GETCURSEL, 0, 0L );
	       int stringsize = (int) SendMessage(hListBox, LB_GETTEXTLEN, index, 0);
	       void *defPtr;
			 char *string;
			 HANDLE hString;

	       if (stringsize == LB_ERR)
		  break;

			 hString = GlobalAlloc(GMEM_FIXED, (WORD) stringsize+1);
	       if ( hString == NULL )
	       {  EndDialog( hDlg, IDOK);
		  ExitToShell();
	       }

	       string = (char *) GlobalLock ( hString );
	       SendMessage( hListBox, LB_GETTEXT, index,(LPARAM)((LPSTR) string));
	       defPtr = FindDefgeneric ( string );

	       /*------------------+
	       | Remove Defgeneric |
	       +------------------*/
	       if ( wParam == IDC_PBUTTON1 )
			 { int count;
		  extern SCREEN WinDialog;

		  Undefgeneric ( defPtr );
		  SendMessage (hListBox, LB_DELETESTRING, index, 0L );

		  count = (int) SendMessage (hListBox, LB_GETCOUNT, 0, 0);
		  sprintf ( DialogName,"Defgenerics Manager - %4d Items",count);
		  SetWindowText ( hDlg, (LPSTR)DialogName );

		  PrintCLIPS ("wclips", "(undefgeneric ");
		  PrintCLIPS ("wclips", string );
		  PrintCLIPS ("wclips", ")\n");
		  PrintPrompt ();
		  InvalidateRect ( WinDialog.hWnd, NULL, TRUE );
		  SetFocus ( hListBox );
		  GlobalUnlock ( hString );
		  GlobalFree ( hString );
		  break;
	       }

	       /*------------------+
	       | PPrint Defgeneric |
	       +------------------*/
	       if ( wParam == IDC_PBUTTON4 )
	       {  extern SCREEN WinDialog;
		  PrintCLIPS ("wclips", "(ppdefgeneric ");
		  PrintCLIPS ("wclips", string );
		  PrintCLIPS ("wclips", ")\n");
		  PrintCLIPS ("wclips", GetDefgenericPPForm ( defPtr ));
		  PrintPrompt ();
		  InvalidateRect ( WinDialog.hWnd, NULL, TRUE );
	       }

	       /*----------------+
	       | Methods Handler |
	       +----------------*/
	       if ( wParam == IDC_PBUTTON5 )
	       {  DLGPROC lpProc;
		  extern HANDLE hInst;

		  lpProc = MakeProcInstance((DLGPROC) DefMethodsManager, hInst);
		  DialogBoxParam((HINSTANCE) hInst,"List_Manager", hDlg, lpProc, (LPARAM)defPtr);
		  FreeProcInstance(lpProc);
	       }

	       /*---------------------------+
	       | Set-Remove Trace Instances |
	       +---------------------------*/
	       if ( wParam == IDC_CBOX1 )
	       {  SetDefgenericWatch (!GetDefgenericWatch(defPtr), defPtr);
		  SetFocus (hListBox);
	       }

	       /*-------------------+
	       | Update Check Boxes |
	       +-------------------*/
	       ShowButtons ( hDlg,
		  IsDefgenericDeletable(defPtr), HIDE, HIDE,
		  (GetDefgenericPPForm(defPtr)!= NULL),
		  (GetNextDefmethod(defPtr,0) != 0),
		  GetDefgenericWatch(defPtr),
		  HIDE,
		  HIDE,
		  ENABLE);
	       GlobalUnlock ( hString );
	       GlobalFree ( hString );
	       return (TRUE);
	    }
	 }
      }
   }
#endif
   return (FALSE);
}


/*****************************************************
* CommandComplete: Callback Function for the Command
*   Complete Dialog.
******************************************************/

BOOL FAR PASCAL CommandComplete (
   HWND       hDlg,
   unsigned   message,
   WORD       wParam,
   LONG       lParam)

{  HWND hListBox = GetDlgItem ( hDlg, IDC_LISTBOX);

   switch (message)
   {  case WM_INITDIALOG:
      {  int count = 0;
	 void *symbolPtr;
	 extern char CompleteString[255];
	 extern struct symbolMatch *matches;
	 CompleteString[0] = '\0';

	 symbolPtr = matches;
	 StartWaitCursor();
	 while ( symbolPtr != NULL)
	 {  LPSTR foo;
	    count ++;
	    foo = (LPSTR)ValueToString (((struct symbolMatch *)symbolPtr)->match);
	    SendMessage ( hListBox, LB_ADDSTRING, 0, (LPARAM)foo);
	    WinRunEvent();
	    symbolPtr = (((struct symbolMatch *)symbolPtr)->next);
	 }
	 StopWaitCursor();
	 SendMessage ( hListBox, LB_SETCURSEL,0, 0L);
	 sprintf ( DialogName,"Command Completion for \"%s\" - %4d Items",(LPSTR)lParam, count);
	 SetWindowText( hDlg, (LPSTR)DialogName );
	 ShowButtons  ( hDlg, ENABLE, HIDE, HIDE, ENABLE, HIDE,
	    HIDE, HIDE, HIDE, ENABLE );
	 SetWindowText ( GetDlgItem ( hDlg, IDC_PBUTTON6 ), "&OK" );
	 SetWindowText ( GetDlgItem ( hDlg, IDC_PBUTTON4 ), "&Cancel" );
	 SetWindowText ( GetDlgItem ( hDlg, IDC_PBUTTON1 ), "&Help" );

	 return (TRUE);
      }

      case WM_COMMAND:
      {  switch (wParam)
	 {  /*-------+
	    | Cancel |
	    +-------*/
	    case IDC_PBUTTON4:
	    {  extern char CompleteString[255];
	       CompleteString[0] = '\0';
	       EndDialog( hDlg, IDC_CANCEL);

	       return (TRUE);
	    }

	    /*------------+
	    | Help Button |
	    +------------*/
	    case IDC_PBUTTON1:
		 {  extern char CompleteString[255];
			 int index = (int) SendMessage ( hListBox, LB_GETCURSEL, 0, 0L);
			 SendMessage(hListBox,LB_GETTEXT,index,(LPARAM)(CompleteString));
	       WinHelp (hDlg, "CLIPS6.HLP", HELP_KEY, (DWORD) CompleteString);
	       return (TRUE);
	    }

	    case IDC_LISTBOX:
	       if ( HIWORD(lParam) != LBN_DBLCLK )
	       break;

	    /*----------+
	    | OK Button |
	    +----------*/
	    case IDC_PBUTTON6:
	    {  extern char CompleteString[255];
	       int index = (int) SendMessage ( hListBox, LB_GETCURSEL, 0, 0L );
	       SendMessage(hListBox,LB_GETTEXT,index,(LPARAM)(CompleteString));
	       EndDialog  (hDlg, IDC_OK);
	       return (TRUE);
	    }
	 }
	 break;
      }
   }
   return (FALSE);
}

