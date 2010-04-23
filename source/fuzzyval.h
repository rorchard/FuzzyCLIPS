/*  $Header: /dist/CVS/fzclips/src/fuzzyval.h,v 1.3 2001/08/11 21:06:02 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*             CLIPS Version 6.00  05/12/93            */
   /*                                                     */
   /*             FUZZY REASONING HEADER FILE             */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*      Brian L. Donnell                                     */
/*      Bob Orchard (NRCC - Nat'l Research Council of Canada)*/
/*                  (Fuzzy reasoning extensions)             */
/*                  (certainty factors for facts and rules)  */
/*                  (extensions to run command)              */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/



#ifndef _H_fuzzyval
#define _H_fuzzyval


#ifndef _H_symbol
#include "symbol.h"
#endif

#ifndef _H_tmpltdef
#include "tmpltdef.h"
#endif




/*********************************************************************/
/* FUZZY_VALUE STRUCTURE:                                            */
/*  pointer to the deftemplate (fuzzy) associated with fuzzy value   */
/*  name of the fuzzy value (linguistic expression pointer)          */
/*  pointer to the deftemplate (fuzzy) associated with fuzzy value   */
/*  maxn  - size of x and y arrays                                   */
/*  n     - number of elements in x and y in use                     */
/*  x,y   - the membership values                                    */
/*                                                                   */
/*  NOTE: at some time FuzzyValues should become atomic types. We    */
/*        go most of the way by making them hashed etc.              */
/*                                                                   */
/*********************************************************************/
struct fuzzy_value
  {
    struct deftemplate *whichDeftemplate; /* the template (fuzzy) */
    char *name;      /* the fuzzy value linguistic */
                     /* expression eg. "very cold" */
    int maxn;
    int n;
    double *x;
    double *y;
  };
  
  


#endif
