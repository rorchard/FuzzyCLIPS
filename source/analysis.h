/*  $Header: /dist/CVS/fzclips/src/analysis.h,v 1.3 2001/08/11 21:04:03 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.10  04/13/98            */
   /*                                                     */
   /*                ANALYSIS HEADER FILE                 */
   /*******************************************************/

/*************************************************************/
/* Purpose: Analyzes LHS patterns to check for semantic      */
/*   errors and to determine variable comparisons and other  */
/*   tests which must be performed either in the pattern or  */
/*   join networks.                                          */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_analysis

#define _H_analysis

#ifndef _H_expressn
#include "expressn.h"
#endif
#ifndef _H_reorder
#include "reorder.h"
#endif

#ifdef LOCALE
#undef LOCALE
#endif
#ifdef _ANALYSIS_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

#if FUZZY_DEFTEMPLATES    
   LOCALE unsigned int     FuzzySlotAnalysis(struct lhsParseNode *patternPtr,
                                             int *numFuzzySlotsInNonNotPatterns
                                            );
#endif


   LOCALE BOOLEAN                        VariableAnalysis(struct lhsParseNode *);

#endif

