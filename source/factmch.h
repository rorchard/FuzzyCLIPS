/*  $Header: /dist/CVS/fzclips/src/factmch.h,v 1.3 2001/08/11 21:05:39 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*               FACT MATCH HEADER FILE                */
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

#ifndef _H_factmch

#define _H_factmch

#ifndef _H_evaluatn
#include "evaluatn.h"
#endif
#ifndef _H_factmngr
#include "factmngr.h"
#endif
#ifndef _H_factbld
#include "factbld.h"
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _FACTMCH_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE void                           FactPatternMatch(struct fact *,
                                               struct factPatternNode *,int,
                                               struct multifieldMarker *,
                                               struct multifieldMarker *);
   LOCALE void                           MarkFactPatternForIncrementalReset(struct patternNodeHeader *,int);
   LOCALE void                           FactsIncrementalReset(void);

#ifndef _FACTMCH_SOURCE_
   extern struct fact             *CurrentPatternFact;
   extern struct multifieldMarker *CurrentPatternMarks;
#endif

#endif






