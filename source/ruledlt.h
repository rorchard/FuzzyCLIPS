/*  $Header: /dist/CVS/fzclips/src/ruledlt.h,v 1.3 2001/08/11 21:07:48 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*          RULE DELETION MODULE HEADER FILE           */
   /*******************************************************/

/*************************************************************/
/* Purpose: Provides routines for deleting a rule including  */
/*   freeing the defrule data structures and removing the    */
/*   appropriate joins from the join network.                */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_ruledlt

#define _H_ruledlt

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _RULEDLT_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE void                           ReturnDefrule(void *);

#ifndef _RULEDLT_SOURCE_
#if (! RUN_TIME) && (! BLOAD_ONLY) && DEBUGGING_FUNCTIONS
   extern int                            DeletedRuleDebugFlags;
#endif
#endif

#endif






