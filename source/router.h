/*  $Header: /dist/CVS/fzclips/src/router.h,v 1.3 2001/08/11 21:07:38 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*                 ROUTER HEADER FILE                  */
   /*******************************************************/

/*************************************************************/
/* Purpose: Provides a centralized mechanism for handling    */
/*   input and output requests.                              */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_router
#define _H_router

/* added to avoid too deep include nesting messages on sun system */
#ifndef _H_symbol
#include "symbol.h"
#endif

#ifndef _H_prntutil
#include "prntutil.h"
#endif

#ifndef _STDIO_INCLUDED_
#define _STDIO_INCLUDED_
#include <stdio.h>
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _ROUTER_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

#define PrintCLIPS(x,y) PrintRouter(x,y)
#define GetcCLIPS(x,y) GetcRouter(x,y)
#define UngetcCLIPS(x,y) UngetcRouter(x,y)
#define ExitCLIPS(x) ExitRouter(x)

#define WCLIPS WPROMPT

   LOCALE void                           InitializeDefaultRouters(void);
   LOCALE int                            PrintRouter(char *,char *);
   LOCALE int                            GetcRouter(char *);
   LOCALE int                            UngetcRouter(int,char *);
   LOCALE void                           ExitRouter(int);
   LOCALE void                           AbortExit(void);
   LOCALE BOOLEAN                        AddRouter(char *,int,int (*)(char *),
                                                              int (*)(char *,char *),
                                                              int (*)(char *),
                                                              int (*)(int,char *),
                                                              int (*)(int));
   LOCALE int                            DeleteRouter(char *);
   LOCALE int                            QueryRouters(char *);
   LOCALE int                            DeactivateRouter(char *);
   LOCALE int                            ActivateRouter(char *);
   LOCALE void                           SetFastLoad(FILE *);
   LOCALE void                           SetFastSave(FILE *);
   LOCALE FILE                          *GetFastLoad(void);
   LOCALE FILE                          *GetFastSave(void);
   LOCALE void                           UnrecognizedRouterMessage(char *);
   LOCALE void                           ExitCommand(void);
   LOCALE int                            PrintNRouter(char *,char *,long);

#ifndef _ROUTER_SOURCE_
   extern char                       *WWARNING;
   extern char                       *WERROR;
   extern char                       *WTRACE;
   extern char                       *WDIALOG;
   extern char                       *WPROMPT;
   extern char                       *WDISPLAY;
   extern int                         CommandBufferInputCount;
   extern char                       *LineCountRouter;
   extern char                       *FastCharGetRouter;
   extern char                       *FastCharGetString;
   extern long                        FastCharGetIndex;
#endif

#endif






