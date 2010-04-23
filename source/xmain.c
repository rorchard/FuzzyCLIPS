static char rcsid[] = "$Header: /dist/CVS/fzclips/src/xmain.c,v 1.3 2001/08/11 21:08:29 dave Exp $" ;


/*********************************** xmain.c **********************************/
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*        Authors:  BeBe Ly - NASA/Johnson Space Center                       */
/*                  Daniel J. McCoy - University of Houston - Downtown        */
/*                  Gary D. Riley - NASA/Johnson Space Center                 */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>

#include "setup.h"
#include "sysdep.h"
#include "commline.h"
#include "symbol.h"

#include <X11/bitmaps/xlogo11>
#include "xsetup.h"
#include "xmain.h"
#include "xmenu_file.h"
#include "xmenu.h"
#include "xclips.h"


Boolean Dribble_status;
                                     
Boolean Browse_status[WINDOW_NUM] = {False,False,False,False,False};
                          /* [0] : agenda_window    *
                           * [1] : fact_window      *
                           * [2] : instances_window *
                           * [3] : globals_window   *
                           * [4] : focus_number     */
extern XtActionsRec ClipsTxtActsTable[];
extern Cardinal ClipsTxtActsTableCount;

/********** Global variables **********/
Widget toplevel = NULL, dialog = NULL;
Widget dialog_form = NULL, dialog_text = NULL;
Arg args[10];
XtAppContext app_con;
XEvent event;
KeySym keysym;              /* Key code id event is a keypress */
XComposeStatus compose_status;
Pixmap checker, clips_logo;
char *clips_sel_buf;
Boolean quit_get_event = False;
unsigned long clips_count = 0;   /* This variable is for the interface */
unsigned long clips_last_pos = 0;
Boolean send_to_clips = False;

XtActionsRec actionTable[] =
  {
    {"DialogReturn",      DialogReturn},
    {"MatchDialogReturnD",MatchDialogReturnD},
    {"MatchDialogReturnE",MatchDialogReturnE},
  };

/*******************************************************************************
          Name:        main
          Description: main function - Creates the interface for CLIPS
          Arguments:   argc - number of arguments
                       argv - arguments
          Returns:     0 on exit
*******************************************************************************/
int main(
  int argc,
  char **argv)
  {
  /* Create top level shell widget */

  toplevel = XtAppInitialize(&app_con,
                             "Xclips",
                             NULL, 0,
                             &argc, argv,
                             fallback_resources,
                             NULL, 0);
  /* Add the new action table */

  XtAppAddActions(app_con, actionTable, XtNumber(actionTable));
  XtAppAddActions(app_con, ClipsTxtActsTable, ClipsTxtActsTableCount);
  
  /* Create the checker icon */

  checker = XCreateBitmapFromData(XtDisplay(toplevel),
                                  RootWindowOfScreen(XtScreen(toplevel)),
                                  xlogo11_bits,
                                  xlogo11_width,
                                  xlogo11_height);
  /* Create clips logo icon */

  clips_logo = XCreateBitmapFromData(XtDisplay(toplevel),
                                     RootWindowOfScreen(XtScreen(toplevel)),
                                     clips_logo_bits,
                                     clips_logo_width,
                                     clips_logo_height);

  /* Create the frame for the main IO dialog window */

  dialog = XtCreateManagedWidget("dialog",
                                 panedWidgetClass,
                                 toplevel,
                                 NULL, 0);

  /* Create the Pull down menus */

  CreatePullDownMenus(dialog);

  XtSetArg(args[0], XtNlabel, "Xclips - NASA/JSC Software Technology Branch");
  (void) XtCreateManagedWidget("menu",
                               labelWidgetClass,
                               dialog,
                               args, 1);

  /* Create the main IO dialog window */
  
  dialog_form = XtCreateManagedWidget("dialog_form",
                                      formWidgetClass,
                                      dialog,
                                      NULL, 0);

  dialog_text = XtVaCreateManagedWidget("dialog_text",
                                        asciiTextWidgetClass,
                                        dialog_form,
                                        XtNheight, 500,
                                        XtNwidth, 600,
                                        XtNeditType, XawtextEdit,
                                        XtNtype, XawAsciiString,
                                        XtNscrollHorizontal, XawtextScrollNever,
                                        XtNscrollVertical, XawtextScrollWhenNeeded,
                                        XtNwrap, XawtextWrapWord,
                                        XtNresize, XawtextResizeNever,
                                        NULL);

  XtOverrideTranslations(dialog_text,
                         XtParseTranslationTable(xclips_translation1));

  XtSetKeyboardFocus(dialog_form, dialog_text);

  XtRealizeWidget(toplevel); 

  InitializeCLIPS();

  InitializeInterface();

  CommandLoop();

  return(-1);
  }

/*******************************************************************************
USRFUNCS:  The function which informs CLIPS of any user defined functions.  In
the default case, there are no user defined functions.  To define functions,
either this function must be replaced by a function with the same name within
this file, or this function can be deleted from this file and included in
another file.  User defined functions may be included in this file or other
files.                                            
Example of redefined UserFunctions:                     
  UserFunctions()                                       
    {                                                   
    DefineFunction("fun1",'i',fun1,"fun1");            
    DefineFunction("other",'f',other,"other");         
    }                                                   
*******************************************************************************/
VOID UserFunctions()
  {
  }

