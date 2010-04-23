static char rcsid[] = "$Header: /dist/CVS/fzclips/src/fuzzydef.c,v 1.3 2001/08/11 21:05:52 dave Exp $" ;

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*             CLIPS Version 6.00  05/12/93            */
   /*                                                     */
   /*             FUZZY REASONING MODULE                  */
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


/******************************************************************
    Fuzzy Logic Module

    This file contains the following categories of functions:
    
    initialization of fuzzy reasoning functions, structures, etc.
        adds function parsers and fuzzy commands
    parsing of deffuzzy statement
    maintaining and accessing linguistic variables

 ******************************************************************/
 
#define _FUZZYDEF_SOURCE_


#include "setup.h"


#if FUZZY_DEFTEMPLATES
 
#include "fuzzydef.h"
#include "fuzzycom.h"
#include "scanner.h"
#include "fuzzyval.h"
#include "fuzzylv.h"
#include "fuzzypsr.h"
#include "fuzzymod.h"

#include <stdio.h>
#define _CLIPS_STDIO_




/******************************************************************
    Global Internal Function Declarations
    
    Defined in fuzzydef.h
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

   globle int      FuzzyInferenceType;
   globle int      FuzzyFloatPrecision;
   globle double   FuzzyAlphaValue; 


/******************************************************************
    FUNCTIONS THAT INITIALIZE FUZZY LOGIC CONSTRUCT 
******************************************************************/


globle void InitializeFuzzy()
{
  DeffuzzyCommands();       /* in fuzzycom.c */
  Init_S_Z_PI_yvalues();    /* in fuzzypsr.c */
  initFuzzyModifierList();  /* in fuzzymod.c */

  FuzzyInferenceType = MAXMIN;  /* inference type 'max-min' by default */
  FuzzyFloatPrecision = 4;      /* default precision for printing fuzzy set values */
  FuzzyAlphaValue = 0.0;        /* default alpha cut for pattern matching */
}





#endif /* FUZZY_DEFTEMPLATES */
