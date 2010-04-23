static char rcsid[] = "$Header: /dist/CVS/fzclips/src/xmenu_watch.c,v 1.3 2001/08/11 21:08:38 dave Exp $" ;

/********************************* xmenu_watch.c ******************************/
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*        Author:  BeBe Ly - NASA/Johnson Space Center                        */
/*                 Daniel J. McCoy - University of Houston - Downtown         */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>
#include "xsetup.h"
#include "xclips.h"
#include "setup.h"
#include "constant.h"
#include "watch.h"
#include "xmenu.h"

#include "xmenu_watch.h"

/********** external variables **********/
extern Widget dialog_text;
extern Arg args[10];
extern Pixmap checker;
extern Boolean quit_get_event;
extern XEvent event;      /* Current event */


/******* Global variables ***********/
char *WatchName[MAX_WATCH] = {"compilations","facts","rules","statistics","activations",
                              "focus","globals","deffunctions","generic-functions","methods",
                              "instances","slots","message-handlers","messages"};
Widget watchShell = NULL,watchForm = NULL;
Widget watch_widgets[MAX_WATCH];

/*******************************************************************************
          Name:        WatchWindow
          Description: Creates Watch menu
          arguments: 
          Returns:     None
*******************************************************************************/
void WatchWindow(
  Widget w,
  XtPointer client_data,
  XtPointer call_data)

{
  int n = 0,i;

  Widget Okay,cancel,All,none;
  static char *WidgetName[MAX_WATCH] = {"Compilations","Facts","Rules","Statistics","Activations",
                                        "Focus","Globals","Deffunctions","Generic Functions","Methods",
                                        "Instances","Slots","Messages-handlers","Messages"};

  /* ====================================================== */
  /*  If the watch shell have already existed pop it up,    */
  /*  else create it.                                       */
  /* ====================================================== */

  if(watchShell != NULL)
    {
       for(i = 0; i < MAX_WATCH; i++)
         {
           n = 0;
           if (GetWatchItem(WatchName[i]))
             XtSetArg( args[n],XtNstate,True);
           else
             XtSetArg( args[n],XtNstate,False);
           n++;
           XtSetValues(watch_widgets[i],args,n);
         }
       XtPopup(watchShell,XtGrabExclusive);
       return;
    }
  /* ========================================= */
  /*   Create the watch toggle menu            */
  /* ========================================= */

  XtSetArg( args[n], XtNwidth,250);n++;
  XtSetArg( args[n], XtNheight,400);n++;
  watchShell = XtCreatePopupShell("Watch Menu",topLevelShellWidgetClass,XtParent(w),NULL,0);
  n = 0;
  watchForm = XtCreateManagedWidget( "watch_form", formWidgetClass,
                                        watchShell, args,n);
  n = 0;
  XtSetArg(args[n],XtNwidth,150);n++;
  XtSetArg(args[n],XtNhorizDistance,15);n++;
  for(i = 0; i < 7;i++)
   {
     if (GetWatchItem(WatchName[i]))
        XtSetArg( args[n],XtNstate,True);
     else
        XtSetArg( args[n],XtNstate,False);
     n++;
     watch_widgets[i] = XtCreateManagedWidget(WidgetName[i],
                                      toggleWidgetClass,
                                      watchForm,
                                      args, n);
     n =  2;
     XtSetArg(args[n],XtNfromVert,watch_widgets[i]);n++;
  }
 n = 1;
 XtSetArg(args[n],XtNfromHoriz,watch_widgets[0]);n++;
 for(; i < MAX_WATCH ; i++)
  {
      if (GetWatchItem(WatchName[i]))
        XtSetArg( args[n],XtNstate,True);
      else
        XtSetArg( args[n],XtNstate,False);
      n++;
      watch_widgets[i] = XtCreateManagedWidget(WidgetName[i],
                                      toggleWidgetClass,
                                      watchForm,
                                      args, n);
      n = 1;
      XtSetArg(args[n],XtNfromVert,watch_widgets[i]);n++;
      XtSetArg(args[n],XtNfromHoriz,watch_widgets[i - 7]);n++;
  }
  /* ======================= */
  /* Create the "All" button */
  /* ======================= */

  n = 0;
  XtSetArg(args[n],XtNcornerRoundPercent,40);n++;
  XtSetArg(args[n],XtNshapeStyle,XmuShapeRoundedRectangle);n++;
  XtSetArg(args[n],XtNwidth,150);n++;
  XtSetArg(args[n],XtNfromVert,watch_widgets[6]);n++;
  XtSetArg(args[n],XtNvertDistance,31);n++;
  XtSetArg(args[n],XtNlabel,"All");n++;
  All = XtCreateManagedWidget("watchButton",
                                       commandWidgetClass,
                                        watchForm,
                                        args, n);
  XtAddCallback(All, XtNcallback, WatchAllCallback, NULL);

  /* ============================= */
  /* Create the "None" button      */
  /* ============================= */

  n = 3;
  XtSetArg(args[n],XtNfromHoriz,All);n++;
  XtSetArg(args[n],XtNfromVert,watch_widgets[6]);n++;
  XtSetArg(args[n],XtNvertDistance,31);n++;
  XtSetArg(args[n],XtNhorizDistance,30);n++;
  XtSetArg(args[n],XtNlabel,"None");n++;
  none = XtCreateManagedWidget("watchButton",
                                       commandWidgetClass,
                                        watchForm,args,n);
  XtAddCallback(none, XtNcallback, WatchNoneCallback, NULL);

  /* ================================ */
  /* Create the "Okay" button         */
  /* ================================ */

  n = 3;
  XtSetArg(args[n],XtNfromVert,All);n++;
  XtSetArg(args[n],XtNvertDistance,15);n++;
  XtSetArg(args[n],XtNlabel,"Okay");n++;
  Okay = XtCreateManagedWidget("watchButton",
                                       commandWidgetClass,
                                        watchForm,
                                        args, n);
  XtAddCallback(Okay,XtNcallback,OkWatchCallback,(XtPointer)watchForm);

  /* ================================ */
  /*   Create the "Cancel" button     */
  /* ================================ */

  n = 3;
  XtSetArg(args[n],XtNfromVert,none);n++;
  XtSetArg(args[n],XtNvertDistance,15);n++;
  XtSetArg(args[n],XtNfromHoriz,Okay);n++;
  XtSetArg(args[n],XtNlabel,"Cancel");n++;
  XtSetArg(args[n],XtNhorizDistance,30);n++;
  cancel = XtCreateManagedWidget("watchButton",
                                       commandWidgetClass,
                                        watchForm,
                                        args, n);
  XtAddCallback(cancel,XtNcallback,PopdownSelect,(XtPointer)watchForm);
  XtPopup(watchShell,XtGrabExclusive);
  }


/**************************************************************************
   OkWatchCallback
   Description:  This function will reset the watch flags to the new
                 values and remove the watch window from the screen.
   Arguments:    w - widget that event was activated
                 client_data - NULL
                 call_data - Unused
   Return:       None
 **************************************************************************/
void OkWatchCallback(
  Widget w,
  XtPointer client_data,
  XtPointer call_data)
{
   int i,n;
   Boolean OnOff = False;
   
   for(i = 0; i< MAX_WATCH; i++)
    {
       n = 0;
       XtSetArg(args[n],XtNstate,&OnOff);n++;
       XtGetValues(watch_widgets[i],args,n);
     /*----------------------------------------------------------------*/
     /* I have to do this because I am not sure if True and False in X */
     /* are defined the same as TRUE and FALSE             */
     /*----------------------------------------------------------------*/
       if((OnOff == True)&&(GetWatchItem(WatchName[i])!= TRUE))
        {
          SetWatchItem(WatchName[i], ON,NULL);
        }
       else if((OnOff == False) && (GetWatchItem(WatchName[i]) == TRUE))
        {
          SetWatchItem(WatchName[i], OFF,NULL);
        }
    }
   XtPopdown(XtParent(XtParent(w)));
   quit_get_event = True;
}

/*******************************************************************************
          Name:        WatchAllCallback
          Description: Called when Watch All button is activated.
                       It turns all the watch toggle buttons to ON, and
                       the watch flags' values will be reset only when 
                       the Okay button is pressed.
          Arguments:  w - menu item that was selected
                       client_data - entry number on menu
                       call_data - not used
          Returns:     None
*******************************************************************************/
void WatchAllCallback(
  Widget w,
  XtPointer client_data, 
  XtPointer call_data)
  {
     int i, n = 0;

     XtSetArg(args[n],XtNstate,True);n++;
     for(i = 0;i < MAX_WATCH; i++)
      {
         XtSetValues(watch_widgets[i],args,n);
      }
   quit_get_event = True;
  }

/*******************************************************************************
          Name:        WatchNoneCallback
          Description: Called when Watch None is selected from Watch menu.
                       It turns all the watch toggle buttons to OFF, and
                       the values of the watch flags are resset only when 
                       the Okay button is pressed.
          Arguments:  w - menu item that was selected
                       client_data - entry number on menu
                       call_data - not used
          Returns:     None
*******************************************************************************/
void WatchNoneCallback(
  Widget w,
  XtPointer client_data,
  XtPointer call_data)
  {
     int i, n = 0;

     XtSetArg(args[n],XtNstate,False);n++;
     for(i = 0;i < MAX_WATCH ; i++)
      {
         XtSetValues(watch_widgets[i],args,n);
      }
    quit_get_event = True;
  }

