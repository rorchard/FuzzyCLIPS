static char rcsid[] = "$Header: /dist/CVS/fzclips/src/xmenu_exec.c,v 1.3 2001/08/11 21:08:34 dave Exp $" ;

/********************************* xmenu_exec.c *******************************/
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*        Author:  BeBe Ly - NASA/Johnson Space Center                        */
/*                 Daniel J. McCoy - University of Houston - Downtown         */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>
#include "xsetup.h"

#include "xclipstext.h"
#include "xmenu_exec.h"
#include "xmenu.h"

#include "setup.h"
#include "router.h"
#include "factmngr.h"
#include "commline.h"

/********** local functions not visible outside this file **********/
static void ResetClips(Widget,XtPointer,XtPointer);
static void ClearClips(Widget,XtPointer,XtPointer);

/********** external functions from xmenu.c **********/

extern Widget toplevel, dialog_text;
extern Boolean quit_get_event;
extern Pixmap clips_logo;
extern XEvent event;                      /* Current event */
extern Arg args[10];
extern int EvaluatingTopLevelCommand;
extern Widget ExecItemWidgets[5];

/*******************************************************************************
          Name:        ResetCallback
          Description: Called when Reset is selected form Execution menu
          Arguments:  w - menu item that was selected
                       client_data - entry number on menu
                       call_data - not used
          Returns:     None
*******************************************************************************/
void ResetCallback(
  Widget w,
  XtPointer client_data, 
  XtPointer call_data)
  {
  MoveEndOfFile(dialog_text, &event);
  if(GetNextFact(NULL))
    {
    Widget confirmshell, confirm;

    confirmshell = XtCreatePopupShell("Confirmation",
                                      topLevelShellWidgetClass,
                                      toplevel,
                                      NULL, 0);

    XtSetArg(args[0], XtNlabel, "The fact list\nis not empty!");
    XtSetArg(args[1], XtNicon, clips_logo);
    confirm = XtCreateManagedWidget("confirm",
                                    dialogWidgetClass,
                                    confirmshell, args, 2);

    XawDialogAddButton(confirm, "Reset", ResetClips, (XtPointer) confirm);
    XawDialogAddButton(confirm, "Cancel", CancelPopupSelect,
                       (XtPointer) confirm);

    XtPopup(confirmshell, XtGrabNonexclusive);
    }

  else
    {
    PrintRouter("wclips", "(reset)\n");
    SetCommandString("(reset)\n");

    /* ============================================ */
    /*  Set this flag to True to break out of the   */
    /*  event loop so CLIPS could process the       */
    /*  command.                                    */
    /* ============================================ */
 
    quit_get_event = True;
    }
  }

/*******************************************************************************
          Name:        RunCallback
          Description: Called when Run is selected form Execution menu
          Arguments:  w - menu item that was selected
                       client_data - entry number on menu
                       call_data - not used
          Returns:     None
*******************************************************************************/
void RunCallback(
  Widget w,
  XtPointer client_data, 
  XtPointer call_data)
  {
/*  if(EvaluatingTopLevelCommand)
   return;*/
  MoveEndOfFile(dialog_text, &event);
  PrintRouter("wclips", "(run)\n");
  SetCommandString("(run)\n");
    /* ============================================ */
    /*  Set this flag to True to break out of the   */
    /*  event loop so CLIPS could process the       */ 
    /*  command.                                    */
    /* ============================================ */

  quit_get_event = True;
 
  }

/*******************************************************************************
          Name:        StepCallback
          Description: Called when Step is selected form Execution menu
          Arguments:  w - menu item that was selected
                       client_data - entry number on menu
                       call_data - not used
          Returns:     None
*******************************************************************************/
void StepCallback(
  Widget w,
  XtPointer client_data, 
  XtPointer call_data)
  {
/*  if(EvaluatingTopLevelCommand)
   return;*/
  MoveEndOfFile(dialog_text, &event);
  PrintRouter("wclips", "(run 1)\n");
  SetCommandString("(run 1)\n");
    /* ============================================ */
    /*  Set this flag to True to break out of the   */
    /*  event loop so CLIPS could process the       */ 
    /*  command.                                    */
    /* ============================================ */

   quit_get_event = True;
  }

/*******************************************************************************
          Name:        ClearCLIPSCallback
          Description: Called when Clear CLIPS is selected form Execution menu
          Arguments:  w - menu item that was selected
                       client_data - entry number on menu
                       call_data - not used
          Returns:     None
*******************************************************************************/
void ClearCLIPSCallback(
  Widget w,
  XtPointer client_data, 
  XtPointer call_data)
  {
  Widget confirmshell, confirm;

/*  if(EvaluatingTopLevelCommand)
   return;*/
  confirmshell = XtCreatePopupShell("Confirmation",
                                    topLevelShellWidgetClass,
                                    toplevel,
                                    NULL, 0);

  XtSetArg(args[0], XtNlabel, "Clear CLIPS!\nAre you sure?");
  XtSetArg(args[1], XtNicon, clips_logo);
  confirm = XtCreateManagedWidget("confirm",
                                  dialogWidgetClass,
                                  confirmshell,
                                  args, 2);

  XawDialogAddButton(confirm, "Clear", ClearClips, (XtPointer) confirm);
  XawDialogAddButton(confirm, "Cancel", CancelPopupSelect, (XtPointer) confirm);

  XtPopup(confirmshell, XtGrabNonexclusive);

    /* ============================================ */
    /*  Set this flag to True to break out of the   */
    /*  event loop so CLIPS could process the       */ 
    /*  command.                                    */
    /* ============================================ */

  quit_get_event = True;
  }

/*******************************************************************************
          Name:        ResetClips
          Description: Calls the command `reset' in CLIPS
          Arguments:  w - Not Used
                       client_data - Child of widget to destroy
                       call_data - Not Used
          Returns:     None
*******************************************************************************/
static void ResetClips(
  Widget w,
  XtPointer client_data, 
  XtPointer call_data)
  {
  XtDestroyWidget(XtParent((Widget) client_data));
  PrintRouter("wclips","(reset)\n");
  SetCommandString("(reset)\n");
    /* ============================================ */
    /*  Set this flag to True to break out of the   */
    /*  event loop so CLIPS could process the       */ 
    /*  command.                                    */
    /* ============================================ */

  quit_get_event = True;
  }

/*******************************************************************************
          Name:        ClearClips
          Description: Calls the `clear' command in CLIPS
          Arguments:  w - Not Used
                       client_data - Not Used
                       call_data - Not Used
          Returns:     None
*******************************************************************************/
static void ClearClips(
  Widget w,
  XtPointer client_data, 
  XtPointer call_data)
  {
  XtDestroyWidget(XtParent((Widget) client_data));
  PrintRouter("wclips","(clear)\n");
  SetCommandString("(clear)\n");
    /* ============================================ */
    /*  Set this flag to True to break out of the   */
    /*  event loop so CLIPS could process the       */ 
    /*  command.                                    */
    /* ============================================ */

  quit_get_event = True;
  }

/*******************************************************************************
 *  ClearScreenCallback
 *  Description: is called when a clear screen is requested
 *  Input : unused
 *******************************************************************************/
void ClearScreenCallback(
  Widget w,
  XtPointer client_data, 
  XtPointer call_data)
  {
    int n = 0;
    
    XtSetArg(args[n],XtNstring,"");n++;
    XtSetValues(dialog_text,args,n);
  }

