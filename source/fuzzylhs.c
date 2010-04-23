static char rcsid[] = "$Header: /dist/CVS/fzclips/src/fuzzylhs.c,v 1.3 2001/08/11 21:05:54 dave Exp $" ;

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*             CLIPS Version 6.00  05/12/93            */
   /*                                                     */
   /*             FUZZY LHS PARSE MODULE                  */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
/*                                                           */
/* Principal Programmer(s):                                  */
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


/******************************************************************
    Fuzzy Logic Module

    This file contains the following categories of functions:
    
    routines to parse fuzzy patterns in lhs of rule

 ******************************************************************/
 
#define _FUZZYLHS_SOURCE_


#include "setup.h"


#if FUZZY_DEFTEMPLATES && DEFRULE_CONSTRUCT && (! RUN_TIME) && (! BLOAD_ONLY)


#include <stdio.h>
#define _CLIPS_STDIO_

#include <math.h>


#include "tmpltdef.h"

#include "fuzzylhs.h"
#include "fuzzyrhs.h"

/******************************************************************
    Global Internal Function Declarations
    
    Defined in fuzzylhs.h
 ******************************************************************/



/******************************************************************
    Local Internal Function Declarations
 ******************************************************************/
 


/******************************************************************
    Local Internal Variable Declarations
 ******************************************************************/


   
/******************************************************************
    Global External Variable Declarations
 ******************************************************************/



/******************************************************************
    Global Internal Variable Declarations
 ******************************************************************/




                                                                  
/**********************************************************************
    FUNCTIONS FOR PARSING THE FUZZY PATTERNS ON THE LHS OF A RULE
 **********************************************************************/
 
 
/********************************************************************/
/* GetFuzzyLHSPattern:                                              */
/*                                                                  */
/********************************************************************/

globle struct fuzzy_value *GetFuzzyLHSPattern(readSource,theToken,fzTemplate,error)
  char *readSource;
  struct token *theToken;
  struct  fuzzyLv *fzTemplate;
  int *error;
{   
      
     /* ParseLinguisticExpr is in FUZZYRHS.C 
        -- same syntax in pattern as in assert
    */
    return( ParseLinguisticExpr(readSource,theToken,fzTemplate,error) );
      
}





#endif  /* FUZZY_DEFTEMPLATES */
