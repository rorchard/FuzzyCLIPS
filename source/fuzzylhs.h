/*  $Header: /dist/CVS/fzclips/src/fuzzylhs.h,v 1.3 2001/08/11 21:05:54 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*             CLIPS Version 6.00  05/12/93            */
   /*                                                     */
   /*             FUZZY LHS PARSE HEADER FILE             */
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



#ifndef _H_fuzzylhs
#define _H_fuzzylhs




#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _FUZZYLHS_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

    LOCALE struct fuzzy_value *GetFuzzyLHSPattern(char *readSource,struct token *theToken,
                                    struct fuzzyLv *fzTemplate,int *error);


#endif

