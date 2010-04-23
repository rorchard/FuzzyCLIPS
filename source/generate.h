/*  $Header: /dist/CVS/fzclips/src/generate.h,v 1.3 2001/08/11 21:06:03 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*                GENERATE HEADER FILE                 */
   /*******************************************************/

/*************************************************************/
/* Purpose: Provides routines for converting field           */
/*   constraints to expressions which can be used            */
/*   in the pattern and join networks.                       */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_generate

#define _H_generate

#ifndef _H_expressn
#include "expressn.h"
#endif
#ifndef _H_reorder
#include "reorder.h"
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _GENERATE_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE void                           FieldConversion(struct lhsParseNode *,struct lhsParseNode *);
   LOCALE struct expr                   *GetvarReplace(struct lhsParseNode *);

#endif






