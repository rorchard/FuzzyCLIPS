/*  $Header: /dist/CVS/fzclips/src/tmpltlhs.h,v 1.3 2001/08/11 21:08:16 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*              DEFTEMPLATE LHS HEADER FILE            */
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

#ifndef _H_tmpltlhs

#define _H_tmpltlhs

#ifndef _H_symbol
#include "symbol.h"
#endif
#ifndef _H_tmpltdef
#include "tmpltdef.h"
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _TMPLTLHS_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE struct lhsParseNode           *DeftemplateLHSParse(char *,struct deftemplate *);

#if FUZZY_DEFTEMPLATES

#ifndef _TMPLTLHS_SOURCE_

#include "tmpltdef.h"

   extern struct deftemplate *FuzzyDeftemplate;
#endif

#endif

#endif






