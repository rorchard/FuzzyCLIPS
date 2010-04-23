/*  $Header: /dist/CVS/fzclips/src/utility.h,v 1.3 2001/08/11 21:08:22 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*                 UTILITY HEADER FILE                 */
   /*******************************************************/

/*************************************************************/
/* Purpose: Provides a set of utility functions useful to    */
/*   other modules. Primarily these are the functions for    */
/*   handling periodic garbage collection and appending      */
/*   string data.                                            */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_utility
#define _H_utility

#ifdef LOCALE
#undef LOCALE
#endif

struct callFunctionItem
  {
   char *name;
   void (*func)(void);
   int priority;
   struct callFunctionItem *next;
  };

#ifdef _UTILITY_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE void                           PeriodicCleanup(BOOLEAN,BOOLEAN);
   LOCALE BOOLEAN                        AddCleanupFunction(char *,void (*)(void),int);
   LOCALE BOOLEAN                        AddPeriodicFunction(char *,void (*)(void),int);
   LOCALE BOOLEAN                        RemoveCleanupFunction(char *);
   LOCALE BOOLEAN                        RemovePeriodicFunction(char *);
   LOCALE char                          *AppendStrings(char *,char *);
   LOCALE char                          *StringPrintForm(char *);
   LOCALE char                          *AppendToString(char *,char *,int *,int *);
   LOCALE char                          *AppendNToString(char *,char *,int,int *,int *);
   LOCALE char                          *ExpandStringWithChar(int,char *,int *,int *,int);
   LOCALE struct callFunctionItem       *AddFunctionToCallList(char *,int,void (*)(void),
                                                               struct callFunctionItem *);
   LOCALE struct callFunctionItem       *RemoveFunctionFromCallList(char *,
                                                             struct callFunctionItem *,
                                                             int *);
   LOCALE int                            ItemHashValue(int,void *,int);
   LOCALE void                           YieldTime(void);

#ifndef _UTILITY_SOURCE_
   extern unsigned long               EphemeralItemCount;
   extern unsigned long               EphemeralItemSize;
   extern void                      (*YieldTimeFunction)(void);
#endif

#endif




