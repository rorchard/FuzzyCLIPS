/*  $Header: /dist/CVS/fzclips/src/sortfun.h,v 1.3 2001/08/11 21:07:55 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  05/23/96            */
   /*                                                     */
   /*            SORT FUNCTIONS HEADER MODULE             */
   /*******************************************************/

/*************************************************************/
/* Purpose: Contains the code for sorting functions.         */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_sortfun

#define _H_sortfun

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _SORTFUN_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE void                           SortFunctionDefinitions(void);
   LOCALE void                           MergeSort(long,DATA_OBJECT *,
                                                   int (*)(DATA_OBJECT *,DATA_OBJECT *));
   LOCALE void                           SortFunction(DATA_OBJECT *);

#endif






