/*  $Header: /dist/CVS/fzclips/src/symblcmp.h,v 1.3 2001/08/11 21:08:03 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*        SYMBOL CONSTRUCT COMPILER HEADER FILE        */
   /*******************************************************/

/*************************************************************/
/* Purpose: Implements the constructs-to-c feature for       */
/*   atomic data values: symbols, integers, floats, and      */
/*   bit maps.                                               */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*      Brian L. Donnell                                     */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_symblcmp
#define _H_symblcmp

#ifndef _STDIO_INCLUDED_
#define _STDIO_INCLUDED_
#include <stdio.h>
#endif

#ifndef _H_symbol
#include "symbol.h"
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _SYMBLCMP_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE void                     PrintSymbolReference(FILE *,SYMBOL_HN *);
   LOCALE void                     PrintFloatReference(FILE *,FLOAT_HN *);
   LOCALE void                     PrintIntegerReference(FILE *,INTEGER_HN *);
   LOCALE void                     PrintBitMapReference(FILE *,BITMAP_HN *);
#if FUZZY_DEFTEMPLATES  
   LOCALE void                     PrintFuzzyValueReference(FILE *,FUZZY_VALUE_HN *);
#endif
   LOCALE void                     AtomicValuesToCode(char *);

#endif







