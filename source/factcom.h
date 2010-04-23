/*  $Header: /dist/CVS/fzclips/src/factcom.h,v 1.3 2001/08/11 21:05:32 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*               FACT COMMANDS HEADER FILE             */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_factcom
#define _H_factcom

#ifndef _H_evaluatn
#include "evaluatn.h"
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _FACTCOM_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE void                           FactCommandDefinitions(void);
   LOCALE void                           AssertCommand(DATA_OBJECT_PTR);
   LOCALE void                           RetractCommand(void);
   LOCALE void                           AssertStringFunction(DATA_OBJECT_PTR);
   LOCALE void                           FactsCommand(void);
   LOCALE void                           Facts(char *,void *,long,long,long);
   LOCALE int                            SetFactDuplicationCommand(void);
   LOCALE int                            GetFactDuplicationCommand(void);
   LOCALE int                            SaveFactsCommand(void);
   LOCALE int                            LoadFactsCommand(void);
   LOCALE int                            SaveFacts(char *,int,struct expr *);
   LOCALE int                            LoadFacts(char *);
   LOCALE int                            LoadFactsFromString(char *,int);
   LOCALE long int                       FactIndexFunction(void);

#endif


