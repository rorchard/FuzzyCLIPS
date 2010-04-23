/*  $Header: /dist/CVS/fzclips/src/globlpsr.h,v 1.3 2001/08/11 21:06:22 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*             DEFGLOBAL PARSER HEADER FILE            */
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

#ifndef _H_globlpsr

#define _H_globlpsr

#ifdef _DEFGLOBL_SOURCE_
struct defglobal;
#endif

#ifndef _H_expressn
#include "expressn.h"
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _GLOBLPSR_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE BOOLEAN                 ParseDefglobal(char *);
   LOCALE BOOLEAN                 ReplaceGlobalVariable(struct expr *);
   LOCALE void                    GlobalReferenceErrorMessage(char *);

#endif







