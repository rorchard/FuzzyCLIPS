static char rcsid[] = "$Header: /dist/CVS/fzclips/src/xmenu_opt.c,v 1.3 2001/08/11 21:08:37 dave Exp $" ;

/******************************** xmenu_opt.c ********************************/
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*        Author:  BeBe Ly - NASA/Johnson Space Center                       */
/*                 Daniel J. McCoy - University of Houston - Downtown        */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>
#include "xsetup.h"
#include "xclips.h"
#include "xmenu.h"
#include "xmenu_opt.h"
#include "xclipstext.h"
#include "setup.h"
#include "engine.h"
#include "crstrtgy.h"
#include "commline.h"
#include "router.h"
#include "globlcom.h"
#include "facthsh.h"
#include "exprnpsr.h"
#include "bmathfun.h"
#include "incrrset.h"

/********** Global variables **********/
extern Widget dialog_text;         /* the standard IO (main CLIPS window) */
extern Widget toplevel;

extern XEvent event;              /* Current event */
extern Boolean quit_get_event;    /* Flag to break out of the get event loop*/
extern Pixmap checker;
extern Arg args[10];



Widget optionsShell = NULL,optionsForm = NULL;
Widget option_widgets[7];
Widget strategy_widgets[7];
Widget sal_opt_widgets[3];
int optionFlags[2] = {DEPTH_STRATEGY,WHEN_DEFINED};

static char strategy_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0xe0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x30, 0x1b, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0xfe, 0xff, 0x01, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x18, 0x00, 0x00,
   0x03, 0x00, 0x00, 0x00, 0xfe, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x30, 0x78, 0x7c, 0x3c, 0x0f, 0x8f, 0x6b, 0x06,
   0xfe, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0xe0, 0x19, 0xcc, 0x60, 0x83, 0xd9, 0x6c, 0x06, 0xfc, 0xff, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x0c, 0x7c,
   0x83, 0xdf, 0x6c, 0x06, 0xf8, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x0c, 0x66, 0x83, 0x81, 0x67, 0x06,
   0xf0, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x30, 0x9b, 0x0d, 0x66, 0xb3, 0xd9, 0xc0, 0x07, 0xe0, 0x1f, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xf1, 0x0c, 0x7c,
   0x1e, 0x8f, 0x07, 0x06, 0xc0, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x6c, 0x06,
   0x80, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc7, 0x03, 0x00, 0x03, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static char salience_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x07, 0x03,
   0x00, 0x00, 0x00, 0xc0, 0x0f, 0x00, 0x70, 0x00, 0x00, 0x00, 0x06, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x66, 0x00, 0x06, 0x03, 0x00, 0x00, 0x00, 0xc0,
   0x00, 0x00, 0x60, 0x00, 0x00, 0x03, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x06, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x60, 0x00,
   0x00, 0x03, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x06, 0x1e, 0x86, 0xc3,
   0xf3, 0xf1, 0x78, 0xc0, 0x60, 0xe6, 0x61, 0xcc, 0x3c, 0x0f, 0x87, 0xe7,
   0x03, 0xff, 0xff, 0x00, 0x3c, 0x30, 0x06, 0x63, 0x36, 0x9b, 0xcd, 0xc0,
   0x67, 0x06, 0x63, 0xcc, 0x60, 0x03, 0xc6, 0x6c, 0x06, 0xff, 0xff, 0x00,
   0x60, 0x3e, 0x06, 0xe3, 0x37, 0x1b, 0xfc, 0xc0, 0x60, 0xe6, 0x63, 0xcc,
   0x7c, 0x03, 0xc6, 0x6c, 0x06, 0xfe, 0x7f, 0x00, 0x60, 0x33, 0x06, 0x63,
   0x30, 0x1b, 0x0c, 0xc0, 0xc0, 0x33, 0x63, 0xcc, 0x66, 0x03, 0xc6, 0x6c,
   0x06, 0xfc, 0x3f, 0x00, 0x66, 0x33, 0x06, 0x63, 0x36, 0x9b, 0xcd, 0xc0,
   0xc0, 0x33, 0x63, 0xcc, 0x66, 0x33, 0xc6, 0x6c, 0x06, 0xf8, 0x1f, 0x00,
   0x3c, 0xbe, 0xdf, 0xcf, 0x33, 0xf3, 0x78, 0xc0, 0x8f, 0xe1, 0xfb, 0xf9,
   0x7c, 0x9e, 0x9f, 0x67, 0x06, 0xf0, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0xe0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x03, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        
/*******************************************************************************
          Name:        OptionsWindow
          Description: Creates the Options menu
          arguements:  w - Widget that invokes the callbaack function
                       client,data - unused
          Returns:     None
*******************************************************************************/
void OptionsWindow(
  Widget w,
  XtPointer client,
  XtPointer data)
  {
    Widget Cancel,Okay,menu,entry,salienceEval,strategy;
    int n,i;
    static char *optionList1[7] = {"Static Constraint Checking",
                                   "Dynamic Constraint Checking",
                                   "Reset Global Variables",
                                   "Sequence Expansion Operator Recognition",
                                   "Incremental Reset",
                                   "Auto-Float Dividend",
                                   "Fact Duplication"};

    static char *optionList2[7] = {"Depth","Breadth","LEX","MEA","Complexity","Simplicity","Random"};
    static char *optionList3[3] = {"When Defined","When Activated","Every Cycle"};
    Pixmap SalienceBM,StrategyBM;

  /* =============================================== */
  /*    If optionsShell exists pop it up             */
  /* =============================================== */

    if(optionsShell != NULL)
     {
       UpdateOptionsMenu();
       XtPopup(optionsShell,XtGrabNonexclusive);
       return;
     }
  /* ================================================ */
  /*  If optionShell not available create one.        */
  /*  The followings are the widget classes used in   */
  /*  building the option menu                        */
  /*     topLevelShellWidgetClass                     */
  /*        formWidgetClass                           */
  /*          menuButtonWidgetClass                   */
  /*            simpleMenuWidgetClass                 */
  /*             smeBSBOjectClass                     */
  /* ================================================ */
    n = 0;
    XtSetArg( args[n], XtNwidth,250);n++;
    XtSetArg( args[n], XtNheight,400);n++;
    optionsShell = XtCreatePopupShell("Execution Options",topLevelShellWidgetClass,XtParent(
w),NULL,0);

  /* ================================================ */
  /*  Create the outside form for the option menu     */
  /* ================================================ */

    optionsForm = XtCreateManagedWidget( "watch_form", formWidgetClass,
                                        optionsShell, args,n);
    SalienceBM = XCreateBitmapFromData(XtDisplay(toplevel),
                                  RootWindowOfScreen(XtScreen(toplevel)),
                                  salience_bits,
                                  salience_width,
                                  salience_height);

  /* ================================================ */
  /*   Create the salience Evaluation setting menu    */
  /* ================================================ */

    n = 0;
    XtSetArg(args[n],XtNbitmap,SalienceBM);n++;
    XtSetArg(args[n],XtNwidth,180);n++;
    salienceEval = XtCreateManagedWidget("salienceEvaluation",
                                 menuButtonWidgetClass,
                                 optionsForm,
                                 args, n);
    menu = XtCreatePopupShell("menu",
                                simpleMenuWidgetClass,
                                salienceEval,
                                NULL,0);
    n = 0;
    XtSetArg(args[n],XtNleftMargin,15);n++;
    XtSetArg(args[n],XtNleftBitmap,checker);n++;
    for(i = 0; i < 3 ; i++)
      {
       sal_opt_widgets[i] = XtCreateManagedWidget(optionList3[i],
                                smeBSBObjectClass,
                                menu,
                                args, n);
       XtAddCallback(sal_opt_widgets[i],XtNcallback,SetSalienceCallback,(XtPointer)i);
       n = 1;
     }

  /* ================================================= */
  /*   Create the strategy setting menu                */
  /* ================================================= */

    StrategyBM = XCreateBitmapFromData(XtDisplay(toplevel),
                                  RootWindowOfScreen(XtScreen(toplevel)),
                                  strategy_bits,
                                  strategy_width,
                                  strategy_height);
    n = 0;
    XtSetArg(args[n],XtNhorizDistance,10);n++;
    XtSetArg(args[n],XtNfromHoriz,salienceEval);n++;
    XtSetArg(args[n],XtNbitmap,StrategyBM);n++;
    XtSetArg(args[n],XtNwidth,150);n++;
    strategy = XtCreateManagedWidget("strategy",
                                 menuButtonWidgetClass,
                                 optionsForm,
                                 args, n);
    menu = XtCreatePopupShell("menu",
                            simpleMenuWidgetClass,
                            strategy,
                            NULL, 0);
    n = 0;
    XtSetArg(args[n],XtNleftMargin, 15);n++;
    XtSetArg(args[n], XtNleftBitmap, checker);n++;
    for(i = 0; i < 7; i++)
     {
       strategy_widgets[i] = XtCreateManagedWidget(optionList2[i],
                                smeBSBObjectClass,
                                menu,
                                args, n);
       XtAddCallback(strategy_widgets[i],XtNcallback,SetStrategyCallback,(XtPointer)i);
       n = 1;
     }
  /* ========================================== */
  /*   Create the rest of the option menu       */
  /* ========================================== */

    n = 0;
    XtSetArg(args[n],XtNwidth,200);n++;
    XtSetArg(args[n],XtNhorizDistance,80);n++;
    XtSetArg(args[n],XtNvertDistance,5);n++;
    XtSetArg(args[n],XtNfromVert,salienceEval);n++;
    for(i = 0;i < 7; i++)
     {
        if((i == INT_STA_CONSTRAINT_CHK) || ( i == INT_AUTO_FLOAT_DIV) ||
           (i == INT_INCREMENTAL_RESET) || ( i == INT_RESET_GLOBALS))
          {
            XtSetArg(args[n], XtNstate,True);n++;
          }
        option_widgets[i] = XtCreateManagedWidget(optionList1[i],
                                      toggleWidgetClass,
                                      optionsForm,
                                      args, n);
        n = 3;
        XtSetArg(args[n],XtNfromVert,option_widgets[i]);n++;
     }
   /* ====================================== */
   /*  Create the "Cancel" button            */
   /* ====================================== */

    n = 0;
    XtSetArg(args[n],XtNshapeStyle,XmuShapeRoundedRectangle);n++;
    XtSetArg(args[n],XtNwidth,150);n++;
    XtSetArg(args[n],XtNfromVert,option_widgets[6]);n++;
    XtSetArg(args[n],XtNvertDistance,31);n++;
    XtSetArg(args[n],XtNlabel,"Cancel");n++;
    Cancel = XtCreateManagedWidget("watchButton",
                                       commandWidgetClass,
                                       optionsForm,
                                        args, n);
    XtAddCallback(Cancel,XtNcallback,PopdownSelect,(XtPointer)optionsForm);

   /* ====================================== */
   /*  Create the "OKay" button              */
   /* ====================================== */
    n = 4;
    XtSetArg(args[n],XtNfromHoriz,Cancel);n++;
    XtSetArg(args[n],XtNhorizDistance,30);n++;
    XtSetArg(args[n],XtNlabel,"OKay");n++;
    Okay = XtCreateManagedWidget("watchButton",
                                       commandWidgetClass,
                                       optionsForm,
                                        args, n);
    XtAddCallback(Okay,XtNcallback,OkayOptionsCallback,(XtPointer)NULL);
    XtPopup(optionsShell,XtGrabNonexclusive);
  }



/*******************************************************************************
          Name:        SetStrategyCallback
          Description: Called when Depth Strategy is selected from Options menu.
                       It marks the new selection in the Depth Strategy menu,
                       but the value is changed only when the Okay button 
                       is pressed.
          Arguments:   w - menu item that was selected
                       client_data - entry number on menu
                       call_data - not used
          Returns:     None
*******************************************************************************/
void SetStrategyCallback(
  Widget w,
  XtPointer client_data,
  XtPointer call_data)
  {
  int i,n = 0,index = (int)client_data;

  MoveEndOfFile(dialog_text, &event);

  XtSetArg(args[n], XtNleftBitmap, None);n++;
  for(i = 0 ; i <=  RANDOM_STRATEGY;i++)
    XtSetValues(strategy_widgets[i], args, n);
  XtSetArg(args[0], XtNleftBitmap, checker);n++;
  XtSetValues(strategy_widgets[index], args, n);
  optionFlags[STRATEGY_FLAG] = index;
  quit_get_event = True;
  }

/*******************************************************************************
          Name:        SetSalienceCallback
          Description: Called when Evaluate When salience is set
                       It marks the salience evaluation method to
                       the new selected method, but the new value only
                       changes when Okay button is pressed.
          Arguments:  w - menu item that was selected
                       client_data - entry number on menu
                       call_data - not used
          Returns:     None
*******************************************************************************/
void SetSalienceCallback(
  Widget w,
  XtPointer client_data,
  XtPointer call_data)
  {
  int i,n = 0,index = (int)client_data;
  MoveEndOfFile(dialog_text, &event);

  n = 0;
  XtSetArg(args[n], XtNleftBitmap, None);n++;
  for(i = 0; i <= EVERY_CYCLE; i++)
   {
     if(i != index)
      XtSetValues(sal_opt_widgets[i],args,n);
   }
  n = 0;
  XtSetArg(args[n], XtNleftBitmap, checker);n++;
  XtSetValues(sal_opt_widgets[index], args, n);
  optionFlags[SALIENCE_FLAG] = index;
  quit_get_event = True;
  }


/**************************************************************************
 OkayOptionsCallback                                         
 Description : This function reset the option flags to the new values
               and remove the window from the screen.
 Arguments   : w - widget that where the event happened
 Return      : None
**************************************************************************/
void OkayOptionsCallback(
  Widget w,
  XtPointer client,
  XtPointer call)
{
  Boolean OnOff;
  int i,n,flag;
 
   if((GetStrategy()) != optionFlags[STRATEGY_FLAG])
     SetStrategy(optionFlags[STRATEGY_FLAG]);
   if((optionFlags[SALIENCE_FLAG]) != GetSalienceEvaluation())
     SetSalienceEvaluation(optionFlags[SALIENCE_FLAG]);
   n = 0;

/* ========================================================= */

   XtSetArg(args[n],XtNstate,&OnOff);n++;
   XtGetValues(option_widgets[INT_FACT_DUPLICATION],args,n);
   if((OnOff)&&(!GetFactDuplication()))
    {
      SetFactDuplication(TRUE);
    }
   else if ((!OnOff)&&(GetFactDuplication()))
    {
      SetFactDuplication(FALSE);
    }
/* ========================================================= */

   XtGetValues(option_widgets[INT_DYN_CONSTRAINT_CHK],args,n);
   if((OnOff)&&(!GetDynamicConstraintChecking()))
    {
      SetDynamicConstraintChecking(TRUE);
    }
   else if((!OnOff) &&(GetDynamicConstraintChecking()))
    {
      SetDynamicConstraintChecking(FALSE);
    }

/* ========================================================= */

   XtGetValues(option_widgets[INT_STA_CONSTRAINT_CHK],args,n);
   if((OnOff)&&(!GetStaticConstraintChecking()))
    {
      SetStaticConstraintChecking(TRUE);
    }
   else if((!OnOff) &&(GetStaticConstraintChecking()))  
    { 
      SetStaticConstraintChecking(FALSE);
    }
/* ========================================================= */

   XtGetValues(option_widgets[INT_SEQUENCE_OPT_REG],args,n);
   if((OnOff)&&(!GetSequenceOperatorRecognition()))
    {
      SetSequenceOperatorRecognition(TRUE);
    }
   else if((!OnOff) &&(GetSequenceOperatorRecognition()))
    {
      SetSequenceOperatorRecognition(FALSE);
    }

/* ========================================================= */

   XtGetValues(option_widgets[INT_AUTO_FLOAT_DIV],args,n);
   if((OnOff)&&(!GetAutoFloatDividend()))
    {
      SetAutoFloatDividend(TRUE);
    }
   else if((!OnOff) &&(GetAutoFloatDividend()))
    {
      SetAutoFloatDividend(FALSE);
    }
/* ========================================================= */

   XtGetValues(option_widgets[INT_INCREMENTAL_RESET],args,n);
   if((OnOff)&&(!GetIncrementalReset()))
    {
      SetIncrementalReset(TRUE);
    }
   else if((!OnOff) &&(GetIncrementalReset()))
    {
      SetIncrementalReset(FALSE);
    }
/* ========================================================= */

   XtGetValues(option_widgets[INT_RESET_GLOBALS],args,n);
   if((OnOff)&&(!GetResetGlobals()))
    {
      SetResetGlobals(TRUE);
    }
   else if((!OnOff) &&(GetResetGlobals()))
    {
      SetResetGlobals(FALSE);
    }
   XtPopdown(XtParent(XtParent(w)));
  quit_get_event = True;
}

/**************************************************************************/

/**************************************************************************/



