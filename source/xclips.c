static char rcsid[] = "$Header: /dist/CVS/fzclips/src/xclips.c,v 1.3 2001/08/11 21:08:24 dave Exp $" ;

/********************************** xclips.c **********************************/
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*        Authors:  BeBe Ly - NASA/Johnson Space Center                       */
/*                  Daniel J. McCoy - University of Houston-Downtown          */
/*                                  & Loral Space Information Systems         */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "xsetup.h"
#include "xclips.h"
#include "xmenu_wind.h"
#include "xclipstext.h"

#include "setup.h"
#include "agenda.h"
#include "bmathfun.h"
#include "classcom.h"
#include "commline.h"
#include "crstrtgy.h"
#include "defins.h"
#include "dffctdef.h"
#include "dffnxfun.h"
#include "engine.h"
#include "exprnpsr.h"
#include "facthsh.h"
#include "factmngr.h"
#include "filecom.h"
#include "genrccom.h"
#include "globlcom.h"
#include "globldef.h"
#include "incrrset.h"
#include "inscom.h"
#include "insfun.h"
#include "router.h"
#include "utility.h"

/********* Local funtions visible outside this file **********/

static void PeriodicUpdate(void);
static void UpdateWindowsMenu(void);
static void GetEvent(void);
static int InitalizeLogTable(void);
static void ReleaseLogTable(void);
static LogNamePtr LogInsert(char *);
static LogNamePtr LogLookup(char *);
static int GetHashValue(char *,int);

/********* Global variables **********/
extern Widget dialog_text;  /* CLIPS dialog window */
extern Widget watch_widgets[13];
extern Widget strategy_widgets[7];
extern Widget sal_opt_widgets[3];
extern Widget option_widgets[7];
extern Widget defrule_manager, deffact_manager, deftemplate_manager,
              deffunction_manager, defglobal_manager,defgeneric_manager, 
              definstances_manager,
              defclass_manager, facts,globals,instances,agenda,
              agenda_manager;
extern Widget FileItemWidgets[],ExecItemWidgets[];
extern XtAppContext app_con;            /* Application context */
extern XEvent event;                    /* Current event */
extern KeySym keysym;                   /* Key code id event is a keypress */
extern XComposeStatus compose_status;
extern Boolean list_change,list1_change;
extern Boolean quit_get_event;
extern Pixmap checker;
extern Arg args[10];
extern Boolean Browse_status[],Dribble_status;
extern int EvaluatingTopLevelCommand;

char unget_buf[MAX_CHAR_IN_BUF];
int char_counter = 0;
int send_clips_command = True;
Boolean periodicChecking = False;
LogNamePtr log_table[LOG_TABLE_SIZE];
LogNamePtr stdin_log, xclips_log, 
           xagenda_log, xfacts_log,
           xinstances_log, xglobals_log,
           xfocus_log;


/*******************************************************************************
          Name:        InitializeInterface
          Description: initializes the the X window interface
          Arguments:   None
          Returns:     None
*******************************************************************************/
void InitializeInterface()
{
  if(!InitalizeLogTable())
  {
    PrintRouter("werror", "Could not initialize logical name hash table\n");
    XclipsExit(0);
    ExitRouter(0);
  }
  SetDribbleStatusFunction(0);

/* Tell Clips Which GetEvent function to call when an event needed */

  SetEventFunction((int(*)())GetEvent);

/*  SetMemoryStatusFunction((int(*)())ActivateTheMenus());*/

/* Tell Clips which function to call periodically */
  AddPeriodicFunction("PeriodicUpdate",PeriodicUpdate,90);
/* Add a main IO router */
  if(!AddRouter("wclips", 10, XclipsQuery, XclipsPrint, XclipsGetc, XclipsUngetc,
                XclipsExit))
  {
    printf("Could not allocate xclips router!\n");
    XclipsExit(0);
    ExitRouter(0);
  }
  PrintRouter("wclips", "                  XCLIPS for:");
}

/*******************************************************************************
          Name:        XclipsQuery
          Description: This function verify the router's logical name
          Arguments:   log_name - logical name of the router
          Returns:      
*******************************************************************************/
int XclipsQuery(
char *log_name)
{
  return((LogLookup(log_name) != NULL) ? TRUE : FALSE);
}

/*******************************************************************************
          Name:        XclipsPrint
          Description: This function will print a string to the window which 
                       has the same logical name as the argument log_name
          Arguments:   log_name - the logical name
                       str - string to be printed
          Returns:     unused
*******************************************************************************/
int XclipsPrint(
char *log_name, 
char *str)
{
  String str_list[1];
  int num = 1;
  LogNamePtr lptr;
  extern Widget agenda_text, facts_text,instances_text,focus_text,globals_text;

  if((lptr = LogLookup(log_name)) == NULL)
    return(False);
  if(lptr == stdin_log)  /* standard IO : print to the main dialog */
    {
    str_list[0] = str;
    InsertClipsString(dialog_text, &event, str_list,(Cardinal *) &num);
    }
/* ============= */
/* agenda window */
/* ============= */

  else if ((lptr == xagenda_log)&&(agenda_text != NULL))
    {
    str_list[0] = str;
    InsertClipsString(agenda_text, &event, str_list,(Cardinal *) &num);
    }

/* ============= */
/* fact window   */
/* ============= */

  else if ((lptr == xfacts_log)&&(facts_text != NULL))
    {
    str_list[0] = str;
    InsertClipsString(facts_text, &event, str_list,(Cardinal *) &num);
    }
/* ================ */
/* instances window */
/* ================ */

  else if ((lptr == xinstances_log) && (instances_text != NULL))
   {
   str_list[0] = str;
   InsertClipsString(instances_text, &event, str_list,(Cardinal *) &num);  
   }
/* ================= */
/* defglobals window */
/* ================= */

  else if ((lptr == xglobals_log) && (globals_text != NULL))
   {
   str_list[0] = str;
   InsertClipsString(globals_text, &event, str_list,(Cardinal *) &num);
   }
/* ================= */
/* Focus window      */
/* ================= */

  else if ((lptr == xfocus_log) && (focus_text != NULL))
   {
   str_list[0] = str;
   InsertClipsString(focus_text,&event,str_list,(Cardinal *) &num);
   }
/* ==================== */
/* print to main dialog */
/* ==================== */

  else if ((lptr != xagenda_log) && (lptr != xfacts_log) &&
           (lptr != xinstances_log) && (lptr != xglobals_log)&&
           (lptr != stdin_log)) 
    {
    str_list[0] = str;
    InsertClipsString(dialog_text, &event, str_list,(Cardinal *) &num);
    }
return(0);
}

/*******************************************************************************
          Name:        XclipsGetc
          Description: Get a character input from user
          Arguments:   log_name -
          Returns:       
*******************************************************************************/
int XclipsGetc(
  char *log_name)
  {
  int quit = False;
  char ch[2];
  String str_list[1];
  int num = 1;
  
  ch[1] = 0;

/* if unget_buf is not empty return the last character in the buf */

  if(char_counter > 0)
    return(unget_buf[--char_counter]);
  if(LogLookup(log_name) == NULL)
    return(EOS);
  while(!quit)
    {
    /* Get an event and if it is a legalimate (cahracter)key press */ 
    /* event print it to the main window, return the character     */
    /* (out of the while loop).                                    */
    /* else, dispatch the event and get another event              */

    XtAppNextEvent(app_con, &event); /* Get an event */
    XLookupString(&event.xkey, ch, 1, &keysym, &compose_status);
    if((event.type == KeyPress) &&
       (event.xproperty.window == dialog_text->core.window))
     {
      if((keysym >= XK_space) && (keysym <= XK_asciitilde))
        {
        str_list[0] = ch;
        InsertClipsString(dialog_text, &event, str_list,(Cardinal *) &num);
        quit = True;
        }
      else if ((keysym != XK_Linefeed) ? (keysym == XK_Return) : TRUE)
        {
        ch[0] = NEWLINE;
        quit = True;
        XtDispatchEvent(&event);
        }
      else if ((keysym != XK_Delete) ? (keysym == XK_BackSpace) :TRUE)
        {
        if(CommandBufferInputCount != 0)
          {
          ch[0] = BACKSPACE;
          quit = True;
          XtDispatchEvent(&event);
          }
        }
      }
     else
       XtDispatchEvent(&event);
     }
    return((int)ch[0]);
    }

/*******************************************************************************
          Name:        XclipsUngetc
          Description: unget an input
          Arguments:   ch - the character to be save
                       log_name - logical name
          Returns:       
*******************************************************************************/
int XclipsUngetc(
  int ch,
  char *log_name)
  {
  if(char_counter < MAX_CHAR_IN_BUF)
    unget_buf[char_counter++] = ch;

   return(0);
  }

/*******************************************************************************
          Name:        XclipsExit
          Description: exit CLIPS in case of abnormal exit 
          Arguments:   num - unused
          Returns:       
*******************************************************************************/
int XclipsExit(
  int num)
  {
  DribbleOff();
  DeleteRouter("wclips");
  XtDestroyApplicationContext(app_con);
  ReleaseLogTable();
  exit(0);
  }

/*******************************************************************************
          Name:        PrintChangedAgenda
          Description: Update the agenda window
          Arguments:   None
          Returns:      
*******************************************************************************/
int PrintChangedAgenda()
  {
  VOID *rule_ptr;
  extern VOID PrintActivation();
  extern Widget agenda_text,agenda;
  char buffer[MAX_CHAR_IN_BUF];
  char *name, labelBuffer[MAX_CHAR_IN_BUF];
  Window AgendaWin;
  Display *theDisplay;
  struct defmodule* theModule = (struct defmodule *)GetCurrentModule();


 /* Change the name of the window to the current module */

  AgendaWin = XtWindow(agenda);
  theDisplay = XtDisplay(agenda);
  if(theModule  != NULL)
     {
       name = GetDefmoduleName(theModule);
       strcpy(labelBuffer,"Agenda Window(");
       strcat(labelBuffer,name);
       strcat(labelBuffer,")");
     }
    else
     {
       strcpy(labelBuffer,"Agenda Window");
     }
  XStoreName(theDisplay,AgendaWin,labelBuffer);
  /* wipe out the old contents */

  XtSetArg(args[0], XtNstring, "");
  XtSetValues(agenda_text, args, 1);
  XawAsciiSourceFreeString(XawTextGetSource(agenda_text));

  /* Print the new agenda list */

  rule_ptr = GetNextActivation(NULL);
  while (rule_ptr != NULL)
    {
    GetActivationPPForm(buffer,MAX_CHAR_IN_BUF - 1,rule_ptr);
    PrintRouter("xagenda",buffer);
    PrintRouter("xagenda", "\n");
    rule_ptr = GetNextActivation(rule_ptr);
    }
  }

/*******************************************************************************
          Name:        PrintChangedFacts
          Description: Update the fact window
          Arguments:   None
          Returns:      
*******************************************************************************/
int PrintChangedFacts()
  {
  VOID *fact_ptr;
  extern VOID PrintFactWithIdentifier();
  extern Widget facts_text;
  char buffer[MAX_CHAR_IN_BUF];
  char *name,labelBuffer[MAX_CHAR_IN_BUF];
  Window FactWin;
  Display *theDisplay;
  struct defmodule* theModule = (struct defmodule *)GetCurrentModule();


 /* Change the name of the window to the current module */

  FactWin = XtWindow(facts);
  theDisplay = XtDisplay(facts);
  if(theModule  != NULL)
     {
       name = GetDefmoduleName(theModule);
       strcpy(labelBuffer,"Fact Window(");
       strcat(labelBuffer,name);
       strcat(labelBuffer,")");
     }
    else
     {
       strcpy(labelBuffer,"Fact Window");
     }
  XStoreName(theDisplay,FactWin,labelBuffer);


  /* Clear the old contents */

  XtSetArg(args[0], XtNstring, "");
  XtSetValues(facts_text, args, 1);
  XawAsciiSourceFreeString(XawTextGetSource(facts_text));

  /* Print the new fact list */

  fact_ptr = GetNextFact(NULL);
  while (fact_ptr != NULL)
    {
    GetFactPPForm(buffer,MAX_CHAR_IN_BUF - 1,fact_ptr);
    PrintRouter("xfacts",buffer);
    PrintRouter("xfacts", "\n");
    fact_ptr = GetNextFact(fact_ptr);
    }
  }

/*******************************************************************************
          Name:        PrintChangedInstances
          Description: Update the instances window
          Arguments:   None
          Returns:
*******************************************************************************/
int PrintChangedInstances()
{
   int   n = 0;
   VOID *instancePtr;
   extern Widget instances_text;
   char buffer[MAX_CHAR_IN_BUF];
   char *name,labelBuffer[MAX_CHAR_IN_BUF];
   Window InstanceWin;
   Display *theDisplay;
   struct defmodule* theModule = (struct defmodule *)GetCurrentModule();


 /* Change the name of the window to the current module */

    InstanceWin = XtWindow(instances);
    theDisplay = XtDisplay(instances);
    if(theModule  != NULL)
     {
       name = GetDefmoduleName(theModule);
       strcpy(labelBuffer,"Instances Window(");
       strcat(labelBuffer,name);
       strcat(labelBuffer,")");
     }
     else
     {
       strcpy(labelBuffer,"Instances Window");
     }
    XStoreName(theDisplay,InstanceWin,labelBuffer);


   /* Clear the old contents */

   XtSetArg(args[n],XtNstring,"");n++;
   XtSetValues(instances_text,args,n);
   XawAsciiSourceFreeString(XawTextGetSource(instances_text));
   /* Print the new instance list */
   instancePtr = (VOID*)GetNextInstance(NULL);
   while(instancePtr != NULL)
    {
       GetInstancePPForm(buffer,MAX_CHAR_IN_BUF - 1,instancePtr);
       PrintRouter("xinstances",buffer);
       PrintRouter("xinstances","\n");
       instancePtr = (VOID*)GetNextInstance(instancePtr);
    }
}

/*******************************************************************************
          Name:        PrintChangedGlobals
          Description: Update the global window
          Arguments:   None
          Returns:
*******************************************************************************/
int PrintChangedGlobals()
{
   VOID *dgPtr;
   int n;
   extern Widget globals_text;
   char *buffer;
   char *name,labelBuffer[MAX_CHAR_IN_BUF];
   Window GlobalWin;
   Display *theDisplay;
   struct defmodule* theModule = (struct defmodule *)GetCurrentModule();


 /* Change the name of the window to the current module */

    GlobalWin = XtWindow(globals);
    theDisplay = XtDisplay(globals);
    if(theModule  != NULL)
     {
       name = GetDefmoduleName(theModule);
       strcpy(labelBuffer,"Globals Window(");
       strcat(labelBuffer,name);
       strcat(labelBuffer,")");
     }
     else
     {
       strcpy(labelBuffer,"Globals Window");
     }
    XStoreName(theDisplay,GlobalWin,labelBuffer);

   /* Clear the old contents */

   n = 0;
   XtSetArg(args[n],XtNstring,"");n++;
   XtSetValues(globals_text,args,n);
   XawAsciiSourceFreeString(XawTextGetSource(globals_text));

   /* Print the new defglobal list */

   dgPtr = GetNextDefglobal(NULL);
   while(dgPtr != NULL)
    {
      buffer = (char *) GetDefglobalPPForm((struct constructHeader *) dgPtr);
      PrintRouter("xglobals",buffer);
      PrintRouter("xglobals","\n");
      dgPtr = GetNextDefglobal(dgPtr);
    }

}

/*******************************************************************************
          Name:        PrintChangedFocus
          Description: Update the Focus window
          Arguments:   None
          Returns:
*******************************************************************************/
int PrintChangedFocus()
{
   VOID *FocusPtr;
   int n;
   extern Widget focus_text;
   char *buffer;

   /* Clear the old contents */
   n = 0;
   XtSetArg(args[n],XtNstring,"");n++;
   XtSetValues(focus_text,args,n);
   XawAsciiSourceFreeString(XawTextGetSource(focus_text));

   /* Print the new focus list */

   FocusPtr = GetNextFocus(NULL);
   while(FocusPtr != NULL)
    {
      buffer = GetDefmoduleName(((struct focus*)FocusPtr)->theModule);
      PrintRouter("xfocus",buffer);
      PrintRouter("xfocus","\n");
      FocusPtr = GetNextFocus(FocusPtr);
    }

}


/*******************************************************************************
          Name:        UpdateMenus
          Description: Updates the windows if necessary
          Arguments:   None 
          Returns:     None
*******************************************************************************/
void UpdateMenus()
  {
  static long LastModuleIndex = -1; 
  Boolean UpdateAll = False;
  extern long ModuleChangeIndex;

  UpdateWindowsMenu();
  if(LastModuleIndex != ModuleChangeIndex)
   {
     UpdateAll = True;
     LastModuleIndex = ModuleChangeIndex;
   }
#if DEFRULE_CONSTRUCT
  if((Browse_status[AGENDA_WIN] == True ) ? (UpdateAll || (GetAgendaChanged() == TRUE)) : FALSE)
      {
      SetAgendaChanged(FALSE);
      PrintChangedAgenda();
      }

 if((Browse_status[FOCUS_WIN] == True) ? ( UpdateAll || (GetFocusChanged() == TRUE)) : FALSE)
     {
     SetFocusChanged(FALSE);
     PrintChangedFocus();
     }

#endif

#if DEFTEMPLATE_CONSTRUCT
  if((Browse_status[FACT_WIN] == True) ? ( UpdateAll || (GetFactListChanged() == TRUE)) : FALSE)
      {
      SetFactListChanged(FALSE);
      PrintChangedFacts();
      }
#endif

#if OBJECT_SYSTEM
 if((Browse_status[INSTANCE_WIN]) ? (UpdateAll || (GetInstancesChanged() == TRUE)) : FALSE)
      {
      SetInstancesChanged(FALSE);
      PrintChangedInstances();
      }
#endif

#ifdef DEFGLOBAL_CONSTRUCT
 if((Browse_status[GLOBAL_WIN] == True) ? (UpdateAll || (GetGlobalsChanged() == TRUE)) : FALSE)
      {
      SetGlobalsChanged(FALSE);
      PrintChangedGlobals();
      }

#endif
  }

/*******************************************************************************
          Name:        UpdateOptionsMenu
          Description: Set menu item mark on options selected
          Arguments:    None
          Returns:     None
*******************************************************************************/
void UpdateOptionsMenu()
  {
  int i;
  unsigned n =  0;

  XtSetArg(args[n], XtNleftBitmap, None);n++;
  for(i = 0; i <= RANDOM_STRATEGY;i++) 
    XtSetValues(strategy_widgets[i], args, n);
  for(i = 0; i <= EVERY_CYCLE;i++)
    XtSetValues(sal_opt_widgets[i],args,n);
  n = 0;
  XtSetArg(args[n], XtNleftBitmap, checker);n++;
  XtSetValues(strategy_widgets[GetStrategy()],args,n);
  XtSetValues(sal_opt_widgets[GetSalienceEvaluation()],args,n);
  n = 0;
  if (GetFactDuplication())
    {
    XtSetArg(args[n], XtNstate,True);n++;
    XtSetValues(option_widgets[INT_FACT_DUPLICATION], args, n);
    }
  else
    {
    XtSetArg(args[n], XtNstate,False);n++;
    XtSetValues(option_widgets[INT_FACT_DUPLICATION], args, n);
    }
  n = 0;
  if (GetDynamicConstraintChecking())
    {
    XtSetArg(args[n], XtNstate,True);n++;
    XtSetValues(option_widgets[INT_DYN_CONSTRAINT_CHK], args, n);
    }
  else
    {
    XtSetArg(args[n], XtNstate,False);n++;
    XtSetValues(option_widgets[INT_DYN_CONSTRAINT_CHK], args, n);
    }
  n = 0;
  if (GetStaticConstraintChecking())
    {
    XtSetArg(args[n], XtNstate,True);n++;
    XtSetValues(option_widgets[INT_STA_CONSTRAINT_CHK], args, n);
    }
  else
    {
    XtSetArg(args[n], XtNstate,False);n++;
    XtSetValues(option_widgets[INT_STA_CONSTRAINT_CHK], args, n);
    }
  n = 0;
  if (GetSequenceOperatorRecognition())
    {
    XtSetArg(args[n], XtNstate,True);n++;
    XtSetValues(option_widgets[INT_SEQUENCE_OPT_REG], args, n);
    }
  else
    {
    XtSetArg(args[n], XtNstate,False);n++;
    XtSetValues(option_widgets[INT_SEQUENCE_OPT_REG], args, n);
    }

  n = 0;
  if (GetAutoFloatDividend())
    {
    XtSetArg(args[n], XtNstate,True);n++;
    XtSetValues(option_widgets[INT_AUTO_FLOAT_DIV], args, n);
    }
  else
    {
    XtSetArg(args[n], XtNstate,False);n++;
    XtSetValues(option_widgets[INT_AUTO_FLOAT_DIV], args, n);
    }
  n =  0;
  if (GetIncrementalReset())
    {
    XtSetArg(args[n], XtNstate,True);n++;
    XtSetValues(option_widgets[INT_INCREMENTAL_RESET], args, n);
    }
  else
    {
    XtSetArg(args[n], XtNstate,False);n++;
    XtSetValues(option_widgets[INT_INCREMENTAL_RESET], args, n);
    }
  n =  0;
  if (GetResetGlobals())
    {
    XtSetArg(args[n], XtNstate,True);n++;
    XtSetValues(option_widgets[INT_RESET_GLOBALS], args, n);
    }
  else
    {
    XtSetArg(args[n], XtNstate,False);n++;
    XtSetValues(option_widgets[INT_RESET_GLOBALS], args, n);
    }
  }

/*******************************************************************************
          Name:        UpdateWindowsMenu
          Description: Sets manager menu items to sensitive  or unsensitive
          Arguments:   None
          Returns:     None
*******************************************************************************/
static void UpdateWindowsMenu()
  {
  /* ==================================================== */
  /*   Refresh the manager window if nessessary           */
  /* ==================================================== */
  if(list_change || list1_change )
      RefreshMngrList();

  /* =================================================================== */
  /*  Set the sensitive state to defrule manager item in the browse menu */
  /* =================================================================== */
  if(GetNextDefrule(NULL))
    {
    XtSetArg(args[0], XtNsensitive, True);
    XtSetValues(defrule_manager, args, 1);
    }
  else
    {
    XtSetArg(args[0], XtNsensitive, False);
    XtSetValues(defrule_manager, args, 1);
    }

  /* ===================================-================================ */
  /*  Set the sensitive state to deffacts manager item in the browse menu */
  /* ==================================================================== */
  if(GetNextDeffacts(NULL))
    {
    XtSetArg(args[0], XtNsensitive, True);
    XtSetValues(deffact_manager, args, 1);
    }
  else
    {
    XtSetArg(args[0], XtNsensitive, False);
    XtSetValues(deffact_manager, args, 1);
     }

  /* ======================================================================= */
  /*  Set the sensitive state to deftemplate manager item in the browse menu */
  /* ======================================================================= */
  if(GetNextDeftemplate(NULL))
    {
    XtSetArg(args[0], XtNsensitive, True);
    XtSetValues(deftemplate_manager, args, 1);
    }
  else
    {
    XtSetArg(args[0], XtNsensitive, False);
    XtSetValues(deftemplate_manager, args, 1);
    }

  /* ======================================================================= */
  /*  Set the sensitive state to deffunction manager item in the browse menu */
  /* ======================================================================= */
  if(GetNextDeffunction(NULL))
    {
    XtSetArg(args[0], XtNsensitive, True);
    XtSetValues(deffunction_manager, args, 1);
    }
  else
    {
    XtSetArg(args[0], XtNsensitive, False);
    XtSetValues(deffunction_manager, args, 1);
    }

  /* ===================================================================== */
  /*  Set the sensitive state to defglobal manager item in the browse menu */
  /* ===================================================================== */

  if(GetNextDefglobal(NULL))
   {
    XtSetArg(args[0], XtNsensitive, True);
    XtSetValues(defglobal_manager,args,1);
   }
  else
   {
    XtSetArg(args[0], XtNsensitive, False);
    XtSetValues(defglobal_manager,args,1);
   }

  /* ====================================================================== */
  /*  Set the sensitive state to defgeneric manager item in the browse menu */
  /* ====================================================================== */

  if(GetNextDefgeneric(NULL))
    {
    XtSetArg(args[0], XtNsensitive, True);
    XtSetValues(defgeneric_manager, args, 1);
    }
  else
    {
    XtSetArg(args[0], XtNsensitive, False);
    XtSetValues(defgeneric_manager, args, 1);
    }

  /* ======================================================================== */
  /*  Set the sensitive state to definstances manager item in the browse menu */
  /* ======================================================================== */

  if(GetNextDefinstances(NULL))
    {
    XtSetArg(args[0], XtNsensitive, True);
    XtSetValues(definstances_manager, args, 1);
    }
  else
    {
    XtSetArg(args[0], XtNsensitive, False);
    XtSetValues(definstances_manager, args, 1);
    }

  /* ==================================================================== */
  /*  Set the sensitive state to defclass manager item in the browse menu */
  /* ==================================================================== */

  if(GetNextDefclass(NULL))
    {
    XtSetArg(args[0], XtNsensitive, True);
    XtSetValues(defclass_manager, args, 1);
    }
  else
    {
    XtSetArg(args[0], XtNsensitive, False);
    XtSetValues(defclass_manager, args, 1);
    }

  /* =================================================================== */
  /*  Set the sensitive state to agenda manager item in the browse menu */
  /* =================================================================== */

  if(GetNextActivation(NULL))
    {
    XtSetArg(args[0], XtNsensitive, True);
    XtSetValues(agenda_manager, args, 1);
    }
  else
    {
    XtSetArg(args[0], XtNsensitive, False);
    XtSetValues(agenda_manager, args, 1);
    }
  }

/*******************************************************************************
          Name:        GetEvent
          Description: This function will get the events and dispatch 
                       them to the the appropriate event handler.
          Arguments:   None 
          Returns:       
*******************************************************************************/
static void GetEvent()
  {
  char ch[2];
  int i;
  extern Boolean send_to_clips;

  ch[1] = 0;
  quit_get_event = False;

 /* Set the menu items to sensitive before getting events */

  XtSetArg(args[0],XtNsensitive,True);
  for( i  = 0; i < 7; i++)
     XtSetValues(FileItemWidgets[i],args,1);
  for(i = 0; i < 5; i++)
     XtSetValues(ExecItemWidgets[i],args,1);

 /* Refresh the windows */
  UpdateMenus(); 

  while(!quit_get_event)
    {

/* There are two types of event (1)middle button press         */
/* (paste event) inside  the  dialog window,(2)others.         */
/* For (1), set the send_to_clips flag to true before dispatch.*/
/* This flag notify the event handler that the paste event was */
/* inside the main dialog,so send the pasted string to CLIPS   */

    XtAppNextEvent(app_con, &event);
    if(event.type == ButtonPress)
      {
      if((event.xproperty.window == dialog_text->core.window) &&
         (!GetManagerList())&& (event.xbutton.button == Button2))
        {
         send_to_clips = True;
        }
      XtDispatchEvent(&event);
      }
    else 
      {
      XtDispatchEvent(&event);
      set_clips_command(True);
      }
    }

/* Deactivate some of the menu items while CLIPS processes a command */

    XtSetArg(args[0],XtNsensitive,False);
    for( i  = 0; i < 7; i++)
     XtSetValues(FileItemWidgets[i],args,1);
    for(i = 0; i < 5; i++)
     XtSetValues(ExecItemWidgets[i],args,1);
    XtSetValues(defrule_manager,args,1);
    XtSetValues(deffact_manager,args,1);
    XtSetValues(deftemplate_manager,args,1);
    XtSetValues(deffunction_manager,args,1);
    XtSetValues(defgeneric_manager,args,1);
    XtSetValues(definstances_manager,args,1);
    XtSetValues(defclass_manager,args,1);
    XtSetValues(agenda_manager,args,1);
  }


/*******************************************************************************
          Name:        InitalizeLogTable
          Description:  
          Arguments:   None 
          Returns:       
*******************************************************************************/
static int InitalizeLogTable()
  {
  register int i;
    
  for (i = 0; i < LOG_TABLE_SIZE; i++)
    log_table[i] = NULL;

  if((xclips_log = LogInsert("wclips")) == NULL)
    return(FALSE);
  if(LogInsert("wdialog") == NULL)
    return(FALSE);
  if(LogInsert("wdisplay") == NULL)
    return(FALSE);
  if(LogInsert("wwarning") == NULL)
    return(FALSE);
  if(LogInsert("werror") == NULL)
    return(FALSE);
  if(LogInsert("wtrace") == NULL)
    return(FALSE);
  if(LogInsert("wagenda") == NULL)
    return(FALSE);
  if((stdin_log = LogInsert("stdin")) == NULL)
    return(FALSE);
  if(LogInsert("stdout") == NULL)
    return(FALSE);
  if((xagenda_log = LogInsert("xagenda")) == NULL)
    return(FALSE);
  if((xfacts_log = LogInsert("xfacts")) == NULL)
    return(FALSE);
  if((xinstances_log =  LogInsert("xinstances")) == NULL)
    return(FALSE);
  if((xglobals_log =  LogInsert("xglobals")) == NULL)
    return(FALSE);
  if((xfocus_log =  LogInsert("xfocus")) == NULL)
    return(FALSE);
  return(TRUE);
  }

/*******************************************************************************
          Name:        ReleaseLogTable
          Description: 
          Arguments:   None 
          Returns:       
*******************************************************************************/
static void ReleaseLogTable()
  {
  register int i;
  LogNamePtr ptr1, ptr2;

  for(i = 0; i < LOG_TABLE_SIZE; i++)
    {
    ptr1 = log_table[i];
    while(ptr1 != NULL)
      {
      ptr2 = ptr1;
      ptr1 = ptr1->next;
      release(ptr2->name);
      release(ptr2);
      } 
    log_table[i] = NULL;
    }
  }

/*******************************************************************************
          Name:        LogInsert
          Description: Insert a new logical name in the log table
          Arguments:   logname -
          Returns:       
*******************************************************************************/
static LogNamePtr LogInsert(
  char *logname)
  {
  register int i;
  LogNamePtr node, ptr;

  i = GetHashValue(logname, LOG_TABLE_SIZE);

  if((node = balloc(1, LogName)) == NULL)
    return(NULL);
  if((node->name = balloc (strlen(logname)+1, char)) == NULL)
    {
    release(node);
    return(NULL);
    }
  strcpy(node->name, logname);
  node->next = NULL;
  if(log_table[i] == NULL)
    log_table[i] = node;
  else
    {
    ptr = log_table[i];
    while(ptr != NULL)
      {
      if(strcmp(ptr->name, logname) == 0)
        {
        release(node->name);
        release(node);
        return(NULL);
        }
      if(ptr->next == NULL)
      break;
      ptr = ptr->next;
      }
    ptr->next = node;
    }
  return(node);
  }

/*******************************************************************************
          Name:        LogLookup
          Description: Lookup in the log table for the recognizable logical name
          Arguments:   logname -
          Returns:       
*******************************************************************************/
static LogNamePtr LogLookup(
  char *logname)
  {
   LogNamePtr ptr;

   ptr = log_table[GetHashValue(logname, LOG_TABLE_SIZE)];
   while (ptr != NULL)
     {
     if(strcmp(logname, ptr->name) == 0)
       return(ptr);
     ptr = ptr->next;
     }
  return(NULL);
 }

/*******************************************************************************
          Name:        GetHashValue
          Description: Hash generates an index into the hash table for a string
                       by the following algorithm:
                            For each character in the string, take its ascii
                            value multiplied by its position in the string, and
                            then add all these values together.  The hash index
                            is this final sum mod the hash table size.
          Arguments:   str
                       size 
          Returns:     hvalue - 
*******************************************************************************/
static int GetHashValue(
  char *str,
  int size)
  {
  register unsigned i;
  unsigned hvalue;

  for(i = 0, hvalue = 0; str[i] != EOS; i++)
    hvalue += ((unsigned)str[i])*(i+1);
  hvalue %= (unsigned)size;
  return((int)hvalue);
  }

/*******************************************************************************
          Name:        set_clips_command
          Description:  
          Arguments:   flag -
          Returns:       
*******************************************************************************/
int set_clips_command(
  int flag)
  {
  send_clips_command = flag;
  }

/*******************************************************************************
          Name:        get_clips_command
          Description:  
          Arguments:   None 
          Returns:       
*******************************************************************************/
int get_clips_command()
  {
  return(send_clips_command);
  }


/******************************************************************************
 * PeriodicUpdate:
 * Description : This function will be called by CLIPS periodically 
 *               while it processes a command. This will allow user
 *               to halt the execution, change options, turn on/off watch,
 *               update the the windows during CLIPS process
 ******************************************************************************
 */
static void PeriodicUpdate()
{
  periodicChecking = True;
  while(XtAppPending(app_con) != 0)
   {
     XtAppNextEvent(app_con, &event);
     XtDispatchEvent(&event);
   }

  periodicChecking = False;

  if((Browse_status[AGENDA_WIN] == True) ? (GetAgendaChanged() == TRUE) : FALSE)
      {
      PrintChangedAgenda();
      SetAgendaChanged(FALSE);
      }
  if((Browse_status[FACT_WIN] == True) ? (GetFactListChanged() == TRUE) : FALSE)
      {
      PrintChangedFacts();
      SetFactListChanged(FALSE);
      }
 if((Browse_status[INSTANCE_WIN] == True) ? (GetInstancesChanged() == TRUE) : FALSE)
      {
      PrintChangedInstances();
      SetInstancesChanged(FALSE);
      }
 if((Browse_status[GLOBAL_WIN] == True) ? (GetGlobalsChanged() == TRUE) : FALSE)
      {
      PrintChangedGlobals();
      SetGlobalsChanged(FALSE);
      }
 if((Browse_status[FOCUS_WIN] == True) ? (GetFocusChanged() == TRUE) :FALSE)
     {
      PrintChangedFocus();
      SetFocusChanged(FALSE);
     }

}

