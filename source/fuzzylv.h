/*  $Header: /dist/CVS/fzclips/src/fuzzylv.h,v 1.3 2001/08/11 21:05:55 dave Exp $  */

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



#ifndef _H_fuzzylv
#define _H_fuzzylv


#if DEFFUNCTION_CONSTRUCT

#ifndef _H_dffnxfun
#include "dffnxfun.h"
#endif

#endif

#ifndef _H_extnfunc
#include "extnfunc.h"
#endif


struct primary_term;



/******************************************************************/
/* LINGUISTIC VARIABLE STRUCTURE:                                 */
/******************************************************************/
struct fuzzyLv   
  {
   /* Universe of Discourse */
   double from;
   double to;
   struct symbolHashNode *units;
   /* Primary Terms allowed */
   struct primary_term *primary_term_list;
  };

/******************************************************************/
/* PRIMARY_TERM STRUCTURE:                                        */
/******************************************************************/
struct primary_term
  {
   FUZZY_VALUE_HN *fuzzy_value_description;
   struct primary_term *next;
  };



#endif
