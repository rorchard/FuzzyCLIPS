static char rcsid[] = "$Header: /dist/CVS/fzclips/src/fuzzycom.c,v 1.3 2001/08/11 21:05:50 dave Exp $" ;
   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*             CLIPS Version 6.00  05/12/93            */
   /*                                                     */
   /*             FUZZY REASONING COMMANDS MODULE         */
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
    
    Fuzzy reasoning related commands (functions)

 ******************************************************************/
 
#define _FUZZYCOM_SOURCE_


#include "setup.h"
#include "math.h"


#if FUZZY_DEFTEMPLATES

#include <stdio.h>
#define _CLIPS_STDIO_

#include <string.h>

#include "extnfunc.h"
#include "evaluatn.h"
#include "router.h"
#include "prntutil.h"
#include "argacces.h"
#include "memalloc.h"
#include "router.h"
#include "factrhs.h"
#include "factmngr.h"
#include "symbol.h"
#include "modulutl.h"

#include "fuzzyval.h"
#include "fuzzylv.h"
#include "fuzzycom.h"
#include "fuzzymod.h"
#include "fuzzydef.h"
#include "fuzzyrhs.h"
#include "fuzzymod.h"
#include "fuzzyutl.h"



#define ONE_THIRD    (1.0/3.0)
#define TWO_THIRDS   (2.0/3.0)



/******************************************************************
    Global Internal Function Declarations
    
    Defined in fuzzycom.h
 ******************************************************************/



/******************************************************************
    Local Internal Function Declarations
 ******************************************************************/
 
    static struct fuzzyLv *getFuzzyUniversePtr(DATA_OBJECT *theResult, char *functionName, int argn);
    static struct fuzzy_value *getFuzzyValuePtr(DATA_OBJECT *theResult, char *functionName, int argn);
    static char          *u_to_string ( struct fuzzyLv *up, int *length );
    static char          *fs_to_string ( struct fact *factPtr, int *length );
    static double         moment_defuzzification( struct fuzzy_value *fvPtr );  
    static VOID           assert_defuzzified_fact( char *name, double value);
    static VOID           get_moment_and_area ( double *moment_ptr, double *area_ptr, double x1,
                                                double y1, double x2, double y2 );
    static double         maximum_defuzzification( struct fuzzy_value *fvPtr );  
    static char          *fv_to_string ( struct fuzzy_value *fv_ptr, int *length_ptr );    
    static struct expr   *CreateFuzzyValueParse( struct expr *top, char *logName );
    globle BOOLEAN        is_defuzzify_value_valid(void); /* Changed from static to global to avoid
								compiler warning DPW 22 May 2001 */

/******************************************************************
    Local Internal Variable Declarations
 ******************************************************************/
   
   /* ALL defuzzify functions must set this variable before exiting. 
      They must set it to TRUE if the defuzzify is valid or
      to FALSE if it is returning a default value when the
      defuzzificztion is undefined (e.g. moment-defuzzify being done
      for a fuzzy set that has zero area)
   */
   static BOOLEAN is_last_defuzzify_valid = TRUE;
   
/******************************************************************
    Global External Variable Declarations
 ******************************************************************/



/******************************************************************
    Global Internal Variable Declarations
 ******************************************************************/


/*************************************************************

        Fuzzy Commands
        
    - initialize the commands related to fuzzy reasoning 

**************************************************************/


globle VOID DeffuzzyCommands()
{

#if ! RUN_TIME
 
 DefineFunction2("moment-defuzzify",'d',PTIF moment_defuzzify, "moment_defuzzify", "11Z");
 DefineFunction2("maximum-defuzzify",'d',PTIF maximum_defuzzify, "maximum_defuzzify", "11Z");
 DefineFunction2("get-u", 'w', PTIF getu, "getu", "11Z");
 DefineFunction2("get-u-from", 'd', PTIF getu_from, "getu_from", "11Z");
 DefineFunction2("get-u-to", 'd', PTIF getu_to, "getu_to", "11Z");
 DefineFunction2("get-u-units", 'w', PTIF getu_units, "getu_units", "11Z");
 DefineFunction2("get-fs", 'w', PTIF get_fs, "get_fs", "11Z");
 DefineFunction2("get-fs-lv", 'w', PTIF get_fs_lv, "get_fs_lv", "11Z");
 DefineFunction2("get-fs-template", 'w', PTIF get_fs_template, "get_fs_template", "11Z");
 DefineFunction2("get-fs-value", 'd', PTIF get_fs_value, "get_fs_value", "22*Zn");
 DefineFunction2("get-fs-length", 'i', PTIF get_fs_length, "get_fs_length", "11Z");
 DefineFunction2("get-fs-x", 'd', PTIF get_fs_x, "get_fs_x", "22*Zi");
 DefineFunction2("get-fs-y", 'd', PTIF get_fs_y, "get_fs_y", "22*Zi");
 DefineFunction2("add-fuzzy-modifier", 'v', PTIF add_fuzzy_modifier, 
                 "add_fuzzy_modifier", "22*ww");
 DefineFunction2("remove-fuzzy-modifier", 'v', PTIF remove_fuzzy_modifier, 
                 "remove_fuzzy_modifier", "11*w");
 DefineFunction2("set-fuzzy-inference-type", 'v', PTIF set_fuzzy_inference_type, 
                 "set_fuzzy_inference_type", "11*w");
 DefineFunction2("get-fuzzy-inference-type", 'w', PTIF get_fuzzy_inference_type, 
                 "get_fuzzy_inference_type", "00");
 DefineFunction2("set-fuzzy-display-precision", 'v', PTIF set_fuzzy_display_precision, 
                 "set_fuzzy_display_precision", "11*l");
 DefineFunction2("get-fuzzy-display-precision", 'l', PTIF get_fuzzy_display_precision, 
                 "get_fuzzy_display_precision", "00");
 DefineFunction2("plot-fuzzy-value", 'v', PTIF plot_fuzzy_value, 
                 "plot_fuzzy_value", "5*ZkkggZ");
 DefineFunction2("set-alpha-value", 'v', PTIF set_alpha_value, "set_alpha_value", "11n");
 DefineFunction2("get-alpha-value", 'd', PTIF get_alpha_value, "get_alpha_value", "00");
 DefineFunction2("get-fuzzy-slot", 'F', PTIF get_fuzzy_slot, "get_fuzzy_slot", "12*zw");
 DefineFunction2("create-fuzzy-value", 'F', PTIF create_fuzzy_value, "create_fuzzy_value", NULL);
 DefineFunction2("fuzzy-union", 'F', PTIF fuzzy_union, "fuzzy_union", "22*FF");
 DefineFunction2("fuzzy-intersection", 'F', PTIF fuzzy_intersection, "fuzzy_intersection", "22*FF");
 DefineFunction2("fuzzy-modify", 'F', PTIF fuzzy_modify, "fuzzy_modify", "22*Fw");
 DefineFunction2("is-defuzzify-value-valid", 'b', is_defuzzify_value_valid, "is_defuzzify_value_valid", "00");


 /* Special parser for the create-fuzzy-value function -- needs to handle
    linguistic expressions and standard or singleton fuzzy set expressions
 */
 AddFunctionParser("create-fuzzy-value", CreateFuzzyValueParse);

#endif

}


/****************************************/
/* is_defuzzify_value_valid:            */
/****************************************/
globle BOOLEAN is_defuzzify_value_valid()
  {
   if (ArgCountCheck("is-defuzzify-value-valid",EXACTLY,0) == -1)
     {
      SetEvaluationError(TRUE);
     }

   return(is_last_defuzzify_valid);
  }





/********************************************************************
    Functions to access the universe of discourse 
 ********************************************************************/                                                                    


/************************************************************
    seeIfFuzzyFact():                                            

    given a ptr to a DATA_OBJECT of type integer or fact-address
    see if it is designates a fuzzy fact or not
    
    returns a ptr to the fuzzy value in the fuzzy fact 
    or NULL if error occurred
************************************************************/
static struct fuzzy_value *seeIfFuzzyFact( theResult, functionName, argn )

  DATA_OBJECT  *theResult;
  char *functionName;
  int argn;
{
   long int factIndex;
   int found_fact;
   struct fact *factPtr;
   char tempBuffer[100];
   struct field *fieldPtr;

   if (theResult->type == INTEGER)
     { 
       factIndex = ValueToLong(theResult->value); 
       if (factIndex < 0)
         {            
           ExpectedTypeError1(functionName,argn,"fact-index must be positive");
           return(NULL);
         }
       found_fact = FALSE;
       factPtr = (struct fact *) GetNextFact(NULL);
       while (factPtr != NULL)
         {
           if (factPtr->factIndex == factIndex)
             {
               found_fact = TRUE;
               break;
             }
           factPtr = factPtr->nextFact;
         }
       
       if (found_fact == FALSE)
         {
           sprintf(tempBuffer,"f-%ld",factIndex);
           CantFindItemErrorMessage("fuzzy fact",tempBuffer);
           return(NULL);
         }
      }
    else
      { /* arg type is fact address */
        factPtr = (struct fact *) theResult->value; 
      }     

    /* must check with (factPtr->whichDeftemplate->fuzzyTemplate != NULL) */
    if (factPtr->whichDeftemplate->fuzzyTemplate != NULL)
      {
       fieldPtr = &(factPtr->theProposition.theFields[0]);
       return( (struct fuzzy_value *)ValueToFuzzyValue(fieldPtr->value) );
      }
    else 
      {
        sprintf(tempBuffer,"f-%ld (or NOT a fuzzy fact)",factPtr->factIndex);
        CantFindItemErrorMessage("fact", tempBuffer);
        return( NULL );
      }
}

/************************************************************
    getFuzzyValuePtr():                                            

    given a ptr to an argument of a function that is expected
    to be a fact address or a fact index get a ptr to the fact
    or a FuzzyValue
    
    returns a ptr to a fact or NULL if error occurred
************************************************************/
static struct fuzzy_value *getFuzzyValuePtr(theResult, functionName, argn)

  DATA_OBJECT  *theResult;
  char *functionName;
  int argn;
{      
   if ((theResult->type == INTEGER) || (theResult->type == FACT_ADDRESS))
     {
       return( seeIfFuzzyFact(theResult, functionName, argn) );
     }
   else if (theResult->type == FUZZY_VALUE)
      return( (struct fuzzy_value *)ValueToFuzzyValue(theResult->value) );
   
   ExpectedTypeError1(functionName,argn,"fact-index/fact-address of fuzzy fact or FUZZY_VALUE");
   return( NULL );
}


/************************************************************
    get_fs_template():                                   

    given a ptr to an argument of a function that is expected
    to be a fact address or a fact index or a fuzzy value
    get the name of the fuzzy deftemplate associated with
    the fuzzy value
    
************************************************************/
globle VOID *get_fs_template()

{
   struct fuzzy_value *fvPtr;
   DATA_OBJECT theArgument;
  
   if (ArgCountCheck("get-u-units",EXACTLY,1) != -1)
     {     
       RtnUnknown(1, &theArgument);

       fvPtr = getFuzzyValuePtr(&theArgument, "get-fs-template", 1);
         
       if (fvPtr != NULL && fvPtr->whichDeftemplate != NULL)
         {
           return( (VOID *)(fvPtr->whichDeftemplate->header.name) );
         }
      }    
        
    SetEvaluationError(TRUE);
    return((VOID *) AddSymbol(""));
}


/************************************************************
    getFuzzyUniversePtr():                                   

    given a ptr to an argument of a function that is expected
    to be a fact address or a fact index or the name of a fuzzy
    deftemplate get a ptr to the universe of discourse (in the
    fuzzyLv structure)
    
    returns a ptr to the fuzzyLv struct  or NULL if an 
    error occurred
************************************************************/
static struct fuzzyLv *getFuzzyUniversePtr(theResult, functionName, argn)

  DATA_OBJECT  *theResult;
  char *functionName;
  int argn;
{
   struct fuzzy_value *fvPtr;
   struct deftemplate *theDeftemplate;
         
   if ((theResult->type == INTEGER) || (theResult->type == FACT_ADDRESS))
     { /* Fuzzy fact being referenced */
       fvPtr = seeIfFuzzyFact(theResult, functionName, argn);
       if (fvPtr == NULL)
         return( NULL );
       else
         return( fvPtr->whichDeftemplate->fuzzyTemplate );
     }
   else if (theResult->type == SYMBOL)
     { /* fuzzy deftemplate being referenced */
       char * theName;
       int count;

       theName = ValueToString(theResult->value);
       if (FindModuleSeparator(theName))
         {
           theDeftemplate = (struct deftemplate *) FindDeftemplate(theName);
         }
       else
         {
           theDeftemplate = (struct deftemplate *)
                 FindImportedConstruct("deftemplate",NULL,theName,
                                        &count,TRUE,NULL);
           if (count > 1)
             {
               AmbiguousReferenceErrorMessage("deftemplate",theName);
               return(NULL);
             }
          }

       if (theDeftemplate != NULL)
         {
           if (theDeftemplate->fuzzyTemplate != NULL)
              return( theDeftemplate->fuzzyTemplate );
           else
	     {
               CantFindItemErrorMessage("fuzzy fact", theName);
               return( NULL );
             }
         }
     }
   else if (theResult->type == FUZZY_VALUE)
     {
       struct fuzzy_value *fvPtr = 
                     (struct fuzzy_value *)ValueToFuzzyValue(theResult->value);
       return( fvPtr->whichDeftemplate->fuzzyTemplate );
     }   

   ExpectedTypeError1(functionName,argn,"fact-index/fact-address of \nfuzzy fact, fuzzy deftemplate name, or FUZZY_VALUE\n(fuzzy deftemplate name may be out of scope)");
   return( NULL );
}




/************************************************************
    getu():                                            

    returns the universe limits and units of a single fact in
    SYMBOL format
************************************************************/
globle VOID *getu()
  {
   struct fuzzyLv *uPtr;
   DATA_OBJECT theArgument;
   VOID *hashresult;
   char *ustring;
   int ulength = 0;

   if (ArgCountCheck("get-u",EXACTLY,1) != -1)
     {     
       RtnUnknown(1, &theArgument);

       uPtr = getFuzzyUniversePtr(&theArgument, "get-u", 1);
       
       if (uPtr != NULL)
         {
           ustring = u_to_string(uPtr, &ulength); 
           hashresult = (VOID *) AddSymbol(ustring);
           rm(ustring, ulength + 1);
           return(hashresult);
         }
      }
        
    SetEvaluationError(TRUE);
    return((VOID *) AddSymbol(""));
  }
  
  

/************************************************************
    u_to_string():                                            

    given a fuzzy universe pointer returns the universe limits 
    and units of a single fact as a string
    
************************************************************/
static char *u_to_string ( up, length_ptr )
struct fuzzyLv *up;
int *length_ptr;
{
    char *string = NULL;
    
    if (up->units != NULL)
    {
        *length_ptr = 36 + strlen(ValueToString(up->units));
        string = gm2( (*length_ptr + 1));
        sprintf( string, "%.2f - %.2f %s", up->from, up->to, ValueToString(up->units));
    }
    else    /* no units specified */
    {
        *length_ptr = 36;
        string = gm2((*length_ptr + 1));
        sprintf( string, "%.2f - %.2f", up->from, up->to);
    }

    return (string);
}


/************************************************************
    getu_from():                                            

    returns the universe minimum limit of a single fact in
    double format
************************************************************/
globle double getu_from()
  {
   struct fuzzyLv *uPtr;
   DATA_OBJECT theArgument;

   if (ArgCountCheck("get-u-from",EXACTLY,1) != -1)
     {     
       RtnUnknown(1, &theArgument);

       uPtr = getFuzzyUniversePtr(&theArgument, "get-u-from", 1);
       
       if (uPtr != NULL)
         {
           return(uPtr->from);            
         }
      }
        
    SetEvaluationError(TRUE);
    return(0.0);
}



 /************************************************************
    getu_to():                                            

    returns the universe maximum limit of a single fact in
    double format
************************************************************/
globle double getu_to()
  {
   struct fuzzyLv *uPtr;
   DATA_OBJECT theArgument;

   if (ArgCountCheck("get-u-to",EXACTLY,1) != -1)
     {     
       RtnUnknown(1, &theArgument);

       uPtr = getFuzzyUniversePtr(&theArgument, "get-u-to", 1);
       
       if (uPtr != NULL)
         {
           return(uPtr->to);            
         }
      }
        
    SetEvaluationError(TRUE);
    return(0.0);
}



/************************************************************
    getu_units():                                            

    returns the universe units of a single fact in
    word format

    returns "" if no units specified in deffuzzy
************************************************************/
globle VOID *getu_units()
  {
   struct fuzzyLv *uPtr;
   DATA_OBJECT theArgument;
  
   if (ArgCountCheck("get-u-units",EXACTLY,1) != -1)
     {     
       RtnUnknown(1, &theArgument);

       uPtr = getFuzzyUniversePtr(&theArgument, "get-u-units", 1);
    
       if (uPtr != NULL)
         {
           if (uPtr->units != NULL)
              return( (VOID *)uPtr->units );
           else
              return((VOID *) AddSymbol(""));
         }
      }    
        
    SetEvaluationError(TRUE);
    return((VOID *) AddSymbol(""));
  }
  
  


/**********************************************************************
    FUNCTIONS TO ACCESS FUZZY SET ELEMENTS
 **********************************************************************/
 
 /************************************************************
    get_fs():                                            

    returns the fuzzy set array of a fuzzy fact or a fuzzy
    value in string format

    if the fact is not fuzzy, a warning is printed and 
    the word "" is returned.
************************************************************/
globle VOID *get_fs()
  {
   struct fuzzy_value *fvPtr;
   DATA_OBJECT theArgument;
   VOID *hashresult;
   char *ustring;
   int ulength = 0;

   if (ArgCountCheck("get-fs",EXACTLY,1) != -1)
     {     
       RtnUnknown(1, &theArgument);

       fvPtr = getFuzzyValuePtr(&theArgument, "get-fs", 1);
       
       if (fvPtr != NULL)
         {
           ustring = fv_to_string(fvPtr, &ulength); 
           hashresult = (VOID *) AddSymbol(ustring);
           rm(ustring, ulength + 1);
           return(hashresult);
         }
      }
        
    SetEvaluationError(TRUE);
    return((VOID *) AddSymbol(""));
}

 /************************************************************
    get_fs_lv():                                            

    returns the fuzzy set's linguistic value
    value in string format

    if the fact is not fuzzy, a warning is printed and 
    the word "" is returned.
************************************************************/
globle VOID *get_fs_lv()
  {
   struct fuzzy_value *fvPtr;
   DATA_OBJECT theArgument;

   if (ArgCountCheck("get-fs-lv",EXACTLY,1) != -1)
     {     
       RtnUnknown(1, &theArgument);

       fvPtr = getFuzzyValuePtr(&theArgument, "get-fs-lv", 1);
       
       if (fvPtr != NULL && fvPtr->name != NULL)
         {
           return( (VOID *)AddSymbol(fvPtr->name) );
         }
      }
        
    SetEvaluationError(TRUE);
    return((VOID *) AddSymbol(""));
}



/************************************************************
    fv_to_string():                                            

    given a fuzzy fact pointer returns the fuzzy set of 
    a single fact as a string
    
************************************************************/

static char *fv_to_string ( fv_ptr, length_ptr )    
struct fuzzy_value *fv_ptr;
int *length_ptr;
{
    int i, num;
    char *string, *strindex;

    num = fv_ptr->n;
    
    /* NOTE: We should really use a MAXDIGITS that tells max float digits */
    /* allows for 10 digits in front of decimal point plus space for ... to
       indicate that space exceeded and not all numbers printed */
       
    *length_ptr = num*33 + 50;  

    string = gm2(*length_ptr + 1);
    strindex = string;
    
    for (i=0; i < num; i++)    
         {
           if (strindex-string > *length_ptr-50)
             { /* we are in the string safety zone -- stop before overrun space */
               /* perhaps we should do this in a better way -- get each pair and
                  append them to each other as needed 
               */
               strcpy(strindex, " ... ");
               PrintRouter(WERROR,"Internal Problem *****\n");
               PrintRouter(WERROR,"String space exceeded in fs_to_string (FUZZYCOM.C)\n");
               break;
             }
             
           sprintf(strindex, "(%.3f, %.3f) ", fv_ptr->x[i], fv_ptr->y[i]);
           strindex = string + strlen(string);
         }
         
   return(string);
}



 /************************************************************
    get_fs_length():                                            

    returns the size of a fuzzy set array in
    INTEGER format

    if the fact is not fuzzy, a warning is printed and 
    a value of 0 is returned.
************************************************************/
globle int get_fs_length()
  {
   DATA_OBJECT theArgument;
   struct fuzzy_value *fv_ptr;

   if (ArgCountCheck("get-fs-length",EXACTLY,1) != -1)
     {     
       RtnUnknown(1, &theArgument);

       fv_ptr = getFuzzyValuePtr(&theArgument, "get-fs-length", 1);
       
       if (fv_ptr != NULL)
         {
           return(fv_ptr->n);            
         }
      }
        
    SetEvaluationError(TRUE);
    return(0);
}



/************************************************************************
    get_fs_value()

    syntax: (get-fs-value <?var> <numeric expression>)

    Returns the y value at the x values specified by the numeric expr.
    If the expression evaluates to a number which is out of range of the
    universe of discourse, the value 0.0 is returned.

 ************************************************************************/
globle double get_fs_value()
{
   DATA_OBJECT theArgument;
   struct fuzzy_value *fv_ptr;
   DATA_OBJECT result;
   double xvalue, yvalue;
   double from, to;

   if (ArgCountCheck("get-fs-x",EXACTLY,2) != -1)
     {     
       RtnUnknown(1, &theArgument);

       fv_ptr = getFuzzyValuePtr(&theArgument, "(get-fs-value", 1);
       
       if (fv_ptr != NULL)
         {
           from = fv_ptr->whichDeftemplate->fuzzyTemplate->from;
           to = fv_ptr->whichDeftemplate->fuzzyTemplate->to;

           if (ArgTypeCheck("get-fs-x",2,INTEGER_OR_FLOAT,&result) == TRUE)
             {
               if (result.type == INTEGER)
                 { xvalue = (double) ValueToInteger(result.value); }
               else
                 { xvalue = ValueToDouble(result.value); }
                 
               if (xvalue < from || xvalue > to)
                 { /* error - out of range */
                   PrintRouter(WERROR,"Function get-fs-value, x value out of range\n");
                 }
               else
                 {
                   int n, i;
                   double lastx, lasty, currentx, currenty;

                   /* find the y value that corresponds to the given x value */
                   n = fv_ptr->n;

                   if (n == 1) 
                          yvalue = fv_ptr->y[0];
                   else
                     {
                       /* find the correct x points that span the required x position */

                       if (xvalue < fv_ptr->x[0]) /* before 1st point */
                          yvalue = fv_ptr->y[0];
                       else if (xvalue > fv_ptr->x[n-1]) /* after last point */
                          yvalue = fv_ptr->y[n-1];
                       else
                         {
                           i = 1;
                       
                           while (i < n)
                             {
                               lastx = fv_ptr->x[i-1];
                               currentx = fv_ptr->x[i];
                               if (xvalue >= lastx && xvalue <= currentx)
                                  break;
                               i++;
                             }
                            
                           if (i == n)  /* major problem -- didn't find it! */
                             {
                              PrintRouter(WERROR,"Function get-fs-value, internal problem, see system manager\n");
                              SetEvaluationError(TRUE);
                              return( 0.0 );
                             }

                           /* if either x value is the same as the one we are looking
                              for then scan from that point on looking for the
                              possibility of multiple points with the same x value
                              and return the max of these values
                           */
                           if (lastx == xvalue || currentx == xvalue)
                             {
                               int k;
                               
                               if (lastx == xvalue) 
                                  k = i-1;
                               else
                                  k = i;
                               yvalue = fv_ptr->y[k++];
                               while (k < n && fv_ptr->x[k] == xvalue)
                                 {
                                   double tmp = fv_ptr->y[k];
                                   if (tmp > yvalue) 
                                      yvalue = tmp;
                                   k++;
                                   
                                 }
                             }
                           else
                             {
                              lasty = fv_ptr->y[i-1];
                              currenty = fv_ptr->y[i];
                              if (lastx == currentx)
                                 yvalue = lasty >= currenty ? lasty : currenty;
                              else
                                 yvalue = lasty+(currenty-lasty)*(xvalue-lastx)/(currentx-lastx);
			     }
                         }
                     }

                   return( yvalue );
                 }
              }
         }
      }
        
    SetEvaluationError(TRUE);
    return(0.0);
    
}




/************************************************************************
    get_fs_x()

    syntax: (get-fs-x <?var> <integer expression>)

    Returns the x value of the ith point in the fuzzy set array.
    If the expression evaluates to a number which is not an integer
    value, the expression result is truncated to an integer.
 ************************************************************************/
globle double get_fs_x()
{
   DATA_OBJECT theArgument;
   struct fuzzy_value *fv_ptr;
   DATA_OBJECT result;
   int index;

   if (ArgCountCheck("get-fs-x",EXACTLY,2) != -1)
     {     
       RtnUnknown(1, &theArgument);

       fv_ptr = getFuzzyValuePtr(&theArgument, "get-fs-x", 1);
       
       if (fv_ptr != NULL)
         {
           if (ArgTypeCheck("get-fs-x",2,INTEGER_OR_FLOAT,&result) == TRUE)
              {
                if (result.type == INTEGER)
                  { index = ValueToInteger(result.value); }
                else
                  { index = (int) ValueToDouble(result.value); }
                      
                if (index < 0 || index >= fv_ptr->n)
                  { /* error - out of range */
                    PrintRouter(WERROR,"Function get-fs-x, index out of range\n");
                  }
                else
                  {
                    return( (double)(fv_ptr->x[index]) );
                  }
              }
         }
      }
        
    SetEvaluationError(TRUE);
    return(0.0);
    
}




/************************************************************************
    get_fs_y()

    syntax: (get-fs-y <?var> <integer expression>)

    Returns the y value of the ith point in the fuzzy set array.
    If the expression evaluates to a number which is not an integer
    value, the expression result is truncated to an integer.
 ************************************************************************/
globle double get_fs_y()
{
   DATA_OBJECT theArgument;
   struct fuzzy_value *fv_ptr;
   DATA_OBJECT result;
   int index;

   if (ArgCountCheck("get-fs-y",EXACTLY,2) != -1)
     {     
       RtnUnknown(1, &theArgument);

       fv_ptr = getFuzzyValuePtr(&theArgument, "get-fs-y", 1);
       
       if (fv_ptr != NULL)
         {
           if (ArgTypeCheck("get-fs-y",2,INTEGER_OR_FLOAT,&result) == TRUE)
              {
                if (result.type == INTEGER)
                  { index = ValueToInteger(result.value); }
                else
                  { index = (int) ValueToDouble(result.value); }
                      
                if (index < 0 || index >= fv_ptr->n)
                  { /* error - out of range */
                    PrintRouter(WERROR,"Function get-fs-y, index out of range\n");
                  }
                else
                  {
                    return( (double)(fv_ptr->y[index]) );
                  }
              }
         }
      }
        
    SetEvaluationError(TRUE);
    return(0.0);
    
}





/***********************************************************************
    DEFUZZIFYING FUNCTIONS
 ***********************************************************************/

/************************************************************/
/* moment-defuzzify function:                               */
/************************************************************/
globle double moment_defuzzify()
{
   struct fuzzy_value *fv_ptr;
   DATA_OBJECT theArgument;

   if (ArgCountCheck("moment-defuzzify",EXACTLY,1) != -1)
     {     
       RtnUnknown(1, &theArgument);

       fv_ptr = getFuzzyValuePtr(&theArgument, "moment-defuzzify", 1);
       
       if (fv_ptr != NULL)
         {
           return(moment_defuzzification(fv_ptr));
         }
      }
        
    SetEvaluationError(TRUE);
    return(0.0);
}



/**********************************************************************
    moment_defuzzification()

    Calculates the first moment of area of a fuzzy set about the
    y axis.  The set is subdivided into different shapes by partitioning
    vertically at each point in the set, resulting in rectangles, 
    triangles, and trapezoids.  The centre of gravity and area of
    each subdivision is calculated using the appropriate formulas
    for each shape.  The first moment of area of the whole set is
    then:

        sum of ( moment(i) * area(i) )          <-- top
        ------------------------------
            sum of (area(i))                    <-- bottom

    If the total area is 0 then use the mid-point of the universe.
    This is not too appropriate but we have to return something ... 
    could have had an error message display since it is like a
    division by zero.

 **********************************************************************/   
static double moment_defuzzification(fv)
  struct fuzzy_value *fv;  
{
    int i,  num;
       
    double result, local_moment, local_area, xmin, xmax;
    double currentx, currenty, nextx, nexty;
    double top = 0.0, bottom = 0.0;
    
    double *fsx, *fsy; /* ptrs to fuzzy set x and y arrays */

    /**********************************************
     Start calculating the c.o.g.
     **********************************************/

    xmin = fv->whichDeftemplate->fuzzyTemplate->from;
    xmax = fv->whichDeftemplate->fuzzyTemplate->to;
    
    is_last_defuzzify_valid = TRUE;

    fsx = fv->x;
    fsy = fv->y;
    num = fv->n;

    if ( num <= 1 )  /* single point is constant fuzzy set over universe */
      {
        result = 0.5 * (xmax - xmin) + xmin;
        /* if no points in the set (should NEVER happen)
           or the y-value of the single point is 0
           then the area is zero -- invalid moment
        */
        if (num < 1 || fsy[0] == 0.0)
           is_last_defuzzify_valid = FALSE;
      }
    else
      {
        currentx = fsx[0];
        currenty = fsy[0];

        /* Check for open-ended set & add initial rectangle if needed */
        if ( currenty != 0.0 && currentx != xmin )    
          {
            local_moment = 0.5 * (currentx+xmin);
            local_area = (currentx - xmin) * currenty;
            top += local_moment * local_area;
            bottom += local_area;    
          }

        for ( i = 1; i < num; i++ )
          {
            nextx = fsx[i];
            nexty = fsy[i];

            get_moment_and_area ( &local_moment, &local_area, currentx, currenty, nextx, nexty );
            top += local_moment * local_area;
            bottom += local_area;

            currentx = nextx;
            currenty = nexty;
          }
    
        /* Check for open-ended set & add final rectangle if needed */
        if ( currenty != 0.0 && currentx < xmax )
          {
            local_moment = 0.5 * (currentx + xmax);
            local_area = (xmax - currentx) * currenty;
            top += local_moment * local_area;
            bottom += local_area;        
          }
      
        /*************************************************************
        Calculate the final result but check for zero area set.
        **************************************************************/
        if ( bottom == 0.0 )
            {  result = 0.5 * ( xmax - xmin ) + xmin;
               /* return a default but indicate that it was an invalid moment */
               is_last_defuzzify_valid = FALSE;
            }
        else
            result = top/bottom;
      }

   return( result );

}




/***********************************************************************
    get_moment_and_area(moment_ptr, area_ptr, x1, y1, x2, y2)

    Given a polygon defined by the vertices (x1, 0), (x1, y1),
    (x2, y2), (x2, 0), the first moment of area of the polygon is
    returned in moment_ptr, and the area of the polygon in area_ptr.

    Conditions: x2 > x1.
 ***********************************************************************/
static VOID get_moment_and_area ( moment_ptr, area_ptr, x1, y1, x2, y2 )
double *moment_ptr, *area_ptr, x1, y1, x2, y2;
{
    /* rectangle of zero height or zero width? */
    if (( y1 == 0.0 && y2 == 0.0 ) ||
        ( x1 == x2)
       )
      {
        *moment_ptr = 0.0;
        *area_ptr = 0.0;
      }
    else if ( y1 == y2 )            /* rectangle */
      {
        *moment_ptr = 0.5 * ( x1 + x2 );
        *area_ptr = ( x2 - x1 ) * y1;
      }
    else if ( y1 == 0.0 && y2 != 0.0 )    /* triangle, height y2 */
      {
        *moment_ptr = TWO_THIRDS * ( x2 - x1 ) + x1;
        *area_ptr = 0.5 * ( x2 - x1 ) * y2;
      }
    else if ( y2 == 0.0 && y1 != 0.0 )    /* triangle, height y1 */
      {
        *moment_ptr = ONE_THIRD * ( x2 - x1 ) + x1;
        *area_ptr = 0.5 * (x2 - x1 ) * y1;
      }
    else                     /* trapezoid */
      {
        *moment_ptr = ( TWO_THIRDS * (x2-x1) * (y2+0.5*y1))/(y1+y2) + x1;
        *area_ptr = 0.5 * ( x2 - x1 ) * ( y1 + y2 );
      }
}


/************************************************************/
/* maximum-defuzzify function:                              */
/************************************************************/
globle double maximum_defuzzify()
{
   struct fuzzy_value *fvPtr;
   DATA_OBJECT theArgument;

   if (ArgCountCheck("maximum-defuzzify",EXACTLY,1) != -1)
     {     
       RtnUnknown(1, &theArgument);

       fvPtr = getFuzzyValuePtr(&theArgument, "maximum-defuzzify", 1);
       
       if (fvPtr != NULL)
         {
           return( maximum_defuzzification(fvPtr) );
         }
      }
        
    SetEvaluationError(TRUE);
    return(0.0);
}
  
  
  
/********************************************************************
    maximum_defuzzification()

    finds the mean of maxima of a fuzzy set
    
    NOTE: This really doesn't work well because there can be x ranges
          where the y value is constant at the max value and other 
          places where the max is only reached for a single x value.
          When this happens the single value gets too much of a say
          in the defuzzified value.
          
              /------\                   /\
             /        \                 /  \
          --/          \---------------/    \-------------
                  ^       ^
                  |       |
                  |       | gives this as the mean of maximum
                  | this is more reasonable???
                  
         Centre of gravity is likely more useful
 ********************************************************************/
 
static double maximum_defuzzification(fv)
  struct fuzzy_value *fv;  
{
    int i,  num, count;
       
    double result,  xmin, xmax;
    double maxy, sum;

    double *fsx, *fsy; /* ptrs to fuzzy set x and y arrays */

    /* always valid result for the maximun defuzzify */
    is_last_defuzzify_valid = TRUE; 

    /******************************************
        Find Mean of Maxima
     ******************************************/

    maxy = 0.0;
    num = fv->n;
    
    fsx = fv->x;
    fsy = fv->y;
    
    for (i=0; i < num; i++ )
      {
        if ( fsy[i] > maxy )
            maxy = fsy[i];
      }

    xmin = fv->whichDeftemplate->fuzzyTemplate->from;
    xmax = fv->whichDeftemplate->fuzzyTemplate->to;
    
    count = 0;
    sum = 0.0;

    /* Check for a zero max value or a single point in the set */
    if (maxy == 0.0 || num == 1)
        result = (xmax - xmin) * 0.5 + xmin;
        
    else /* Set has at least two points */
      {    
        /* check for maximum at beginning of open-ended set */
        if ( fsy[0] == maxy)
          {
            sum += xmin;
            count++;
            if ( fsx[0] != xmin && fsy[1] != maxy )
              {
                sum += fsx[0];
                count++;
              }
          }
          
        for ( i = 1; i < num - 1; i++ )
          {
            if ( fsy[i] == maxy )
              {
                if ( fsy[i-1] != maxy || fsy[i+1] != maxy )
                  {
                    sum += fsx[i];
                    count++;
                  }
              }
          }
          
        /* check for maximum at end of open-ended set */
        if ( fsy[num-1] == maxy )
          {
            if ( fsx[num-1] != xmax && fsy[num-2] != maxy )
              {
                sum += fsx[num-1];
                count++;
              }
            sum += xmax;
            count++;
          }
        
        result = sum/count;
      }
    
   return( result );
}






/**********************************************************************
    FUNCTIONS TO ADD/REMOVE FUZZY MODIFIERS (HEDGES)
 **********************************************************************/
 
 /************************************************************
    add_fuzzy_modifier():                                            

    adds a new fuzzy modifier to the list of modifiers

************************************************************/
globle VOID add_fuzzy_modifier()
  {
   char *modname, *modfuncname;
   struct FunctionDefinition *fun = NULL;
   DEFFUNCTION *deffun = NULL;
   
   if (ArgCountCheck("add-fuzzy-modifier",EXACTLY,2) != -1)
     {     
       modname = RtnLexeme(1);     /* get name of the modifier */
       modfuncname = RtnLexeme(2); /* get name of the modifier function */
       if (modname == NULL || modfuncname == NULL)
          return;

#if DEFFUNCTION_CONSTRUCT
      deffun = (DEFFUNCTION *)LookupDeffunctionInScope(modfuncname);
      
      if (deffun == NULL)
        {
#endif
         /* should be name of a function of 1 arg of type float, returns float */
         fun = FindFunction(modfuncname);
          if (fun == NULL)
            {
              PrintRouter(WERROR,"Function add-fuzzy-modifier (undefined modifier function)\n");
              return;
            }
          if ((strcmp(fun->restrictions,"11f") != 0 &&
               strcmp(fun->restrictions,"11d") != 0 &&
               strcmp(fun->restrictions,"11n") != 0) 
               ||
              (fun->returnValueType != 'f' && fun->returnValueType != 'd')) 
             {
               PrintRouter(WERROR,"Function add-fuzzy-modifier (incorrect function type in arg 2)\n");
               return;               
             }
        }
#if DEFFUNCTION_CONSTRUCT
      else
        {    
          struct expr *top, *temp;
          BOOLEAN res;
          DATA_OBJECT result;
          
          if (deffun->minNumberOfParameters != 1 || deffun->maxNumberOfParameters != 1)
             {
               PrintRouter(WERROR,"Function add-fuzzy-modifier (modifier function requires exactly 1 arg)");
               return;               
             }
          /* try out the deffunction and make sure it accepts a float arg and returns a float */
          top = GenConstant(PCALL, (VOID *)deffun);
          top->argList = GenConstant(FLOAT, (VOID *)AddDouble(0.0));
          temp = CurrentExpression;
          CurrentExpression = top;
          res = (*PrimitivesArray[PCALL]->evaluateFunction)(deffun,&result);
          CurrentExpression = temp;
          ReturnExpression(top);
          if (!res || (result.type != FLOAT))
             {
               PrintRouter(WERROR,"Function add-fuzzy-modifier (modifier function must accept and return FLOAT)");
               return;               
             }
        }
#endif
 
       if (AddFuzzyModifier(modname, NULL, fun, deffun) == TRUE)


           return;
     }
        
    SetEvaluationError(TRUE);
  }





 /************************************************************
    remove_fuzzy_modifier():                                            

    removes a fuzzy modifier from the list of modifiers
    (note: cannot remove the system provided ones)

************************************************************/
globle VOID remove_fuzzy_modifier()
  {
   char *modname;
   
   
   if (ArgCountCheck("remove-fuzzy-modifier",EXACTLY,1) != -1)
     {     
       modname = RtnLexeme(1);     /* get name of the modifier */
       
       if (modname != NULL)
          RemoveFuzzyModifier(modname);
     }
 
  }



/**********************************************************************
    FUNCTIONS TO CONTROL INFERENCING & PRINTING
 **********************************************************************/
 


 /************************************************************
    set_fuzzy_inference_type():                                            

    sets the type of fuzzy inferencing to 1 of:
 
       max-min     - default
       max-prod

************************************************************/
globle VOID set_fuzzy_inference_type()
  {
   char *inference_type;
   
   
   if (ArgCountCheck("set-fuzzy-inference-type",EXACTLY,1) != -1)
     {     
       inference_type = RtnLexeme(1);     /* get type of inferencing */
       if (inference_type == NULL)
          return;
       
       if (strcmp(inference_type,"max-min") == 0 ||
           strcmp(inference_type,"MAX-MIN") == 0 
          )
          FuzzyInferenceType = MAXMIN;
       else if  (strcmp(inference_type,"max-prod") == 0 ||
                 strcmp(inference_type,"MAX-PROD") == 0 
                )
          FuzzyInferenceType = MAXPROD;
       else
          {
            PrintRouter(WERROR,"Function set-fuzzy-inference-type (argument must be max-min or max-prod)\n");
          }
     }
 
  }



 /************************************************************
    get_fuzzy_inference_type():                                            

    gets the type of fuzzy inferencing as:
 
       "max-min"  OR   "max-prod"

************************************************************/
globle VOID *get_fuzzy_inference_type()
  {
    if (FuzzyInferenceType == MAXMIN)
      return( (VOID *) AddSymbol("max-min") );
    else 
      return( (VOID *) AddSymbol("max-prod") );
  
  }







/************************************************************
    set_fuzzy_display_precision():                                            

    sets the precision used when displaying fuzzy set values
 
************************************************************/
globle VOID set_fuzzy_display_precision()
  {
   DATA_OBJECT arg;
   long int precision;   
   
   if (ArgCountCheck("set-fuzzy-display-precision",EXACTLY,1) != -1)
     {     
       if (ArgTypeCheck("set-fuzzy-display-precision",1,INTEGER,&arg) == FALSE) 
          return;	    
      
       precision = DOToLong(arg);

       if (precision > 16) precision = 16;
       else if (precision <= 1) precision = 2;

       FuzzyFloatPrecision = precision;
     }
 
  }



/************************************************************
    get_fuzzy_display_precision():                                            

    gets the precision used when displaying fuzzy set values  

************************************************************/
globle long  get_fuzzy_display_precision()
  {
    return( (long int)FuzzyFloatPrecision ); 
  }





#ifndef nint

static int nint( double );

/* Define a rounding function if not already available */

static int nint(f)
 
   double f;
{
  return( (int)floor(f + 0.5) );
}

#endif



#define NROWS 20          /* number of rows in the plot */
#define NCOLS 50          /* number of columns in the plot */
#define NTICKS 5          /* number of ticks per division in x axis */

 /************************************************************
    plot_fuzzy_value():                                            

    plots the fuzzy value in an ascii format


   (plot-fuzzy-value logname plotchar(s) low high fuzzyvalue [ fuzzyvalue... ])

   where
         logname     - router name
         plotchar(s) - character(s) to use for plotting
         low         - low limit in universe for graph (if symbol use from)
         high        - high limit in universe for graph (if symbol use to)
         fuzzyvalue  - fuzzy value to plot 

   NOTE: There may be multiple fuzzy values plotted on the same graph.
         The plotchar is selected form the list of chars provided. When
         more fuzzy values than plotchars the last is used over again.
 
************************************************************/
globle VOID plot_fuzzy_value()
  {
   char *dummyid;
   int i, j, k, m, lastk, previousLastk, numArgs, n;
   DATA_OBJECT theArgument;
   char theSymbol, *theString;
   struct fuzzy_value *fv_ptr, *fv_ptr_first;
   char PlotSpace[NROWS+1][NCOLS+2];
   double from, to, range, deltax;
   double fromArg, toArg;
   double *fvx, *fvy, y;
   double thisx, thisy, lastx, lasty;
   double nextx, nexty, previousThisy;
   double miny, maxy;
   char str[200];
   int numSymbols, fvNum;

   /*=====================================================*/
   /* Get the logical name to which output is to be sent. */
   /*=====================================================*/
 
   if ((numArgs = ArgCountCheck("plot-fuzzy-value",AT_LEAST,5)) == -1) return;

   dummyid = GetLogicalName(1,"stdout");
   if (dummyid == NULL)
     {
       PrintErrorID("IOFUN",1,FALSE);
       PrintRouter(WERROR,"Illegal logical name used for plot-fuzzy-value function.\n");
       SetHaltExecution(TRUE);
       SetEvaluationError(TRUE);
       return;
     }

   /*============================================================*/
   /* Determine if any router recognizes the output destination. */
   /*============================================================*/

   if (QueryRouters(dummyid) == FALSE)
     {
      UnrecognizedRouterMessage(dummyid);
      return;
     }

   /*============================================================*/
   /* Get the symbol(s) for plotting                             */
   /*============================================================*/

   theString = RtnLexeme(2);

   if (theString == NULL) return;
   if ((unsigned int)strlen(theString) < 1)
     {
       PrintRouter(WERROR,"ERROR: Function plot_fuzzy_value (argument 3 must be a character)\n");
       return;
     }

    numSymbols = strlen(theString);

   /*============================================================*/
   /* Get the limit arguments                                    */
   /*============================================================*/

   RtnUnknown(5, &theArgument); /* need deftemplate of 1st fuzzy value */

   fv_ptr = getFuzzyValuePtr(&theArgument, "plot-fuzzy-value", 5);
   if (fv_ptr == NULL)  return;

   from = fv_ptr->whichDeftemplate->fuzzyTemplate->from;
   to = fv_ptr->whichDeftemplate->fuzzyTemplate->to;

   fv_ptr_first = fv_ptr;

   RtnUnknown(3, &theArgument);

   if ((theArgument.type == INTEGER) || (theArgument.type == FLOAT))
     {
       fromArg = (theArgument.type == INTEGER) ? (double)DOToLong(theArgument) : 
                                                  DOToDouble(theArgument);
       if (fromArg > from) from = fromArg;
     }

   RtnUnknown(4, &theArgument);

   if ((theArgument.type == INTEGER) || (theArgument.type == FLOAT))
     {
       toArg = (theArgument.type == INTEGER) ? (double)DOToLong(theArgument) : 
                                               DOToDouble(theArgument);
       if (toArg < to) to = toArg;
     }

   if (from >= to)
     {
       PrintRouter(WERROR,"ERROR: Function plot_fuzzy_value (low limit for plot >= high limit)\n");
       return;
     }

   /*============================================================*/
   /* Set up to Do the plotting                                  */
   /*============================================================*/

   for (i=0; i<=NROWS; i++)
      { for (j=0; j<NCOLS+1; j++)
           PlotSpace[i][j] = ' ';

        PlotSpace[i][j] = '\0';
      }

   range = to - from;
   deltax = range/NCOLS;

   PrintRouter(dummyid, "\nFuzzy Value: ");
   PrintRouter(dummyid, ValueToString(fv_ptr->whichDeftemplate->header.name));
   PrintRouter(dummyid, "\nLinguistic Value: ");

   /*============================================================*/
   /* Get the fuzzy value(s)  and do the plotting                */
   /*============================================================*/

   for (fvNum = 0; fvNum <= numArgs-5; fvNum++)
     {

      if (fvNum == 0)
        fv_ptr = fv_ptr_first;
      else
        {
          RtnUnknown(5+fvNum, &theArgument);

          fv_ptr = getFuzzyValuePtr(&theArgument, "plot-fuzzy-value", 5+fvNum);
        }
       
      if (fv_ptr == NULL)  return;

      if (numSymbols > fvNum) 
         theSymbol = theString[fvNum];

      /* MUST have same deftemplates for all fuzzy values to plot properly */
      if (fv_ptr->whichDeftemplate != fv_ptr_first->whichDeftemplate)
        {
          PrintRouter(WERROR,"ERROR: Function plot_fuzzy_value (all fuzzy values must have same fuzzy deftemplate)\n");
          break;
        }

      { char tmpStr[10];

        if (fvNum > 0) PrintRouter(dummyid, ",  ");
        PrintRouter(dummyid, fv_ptr->name);
        sprintf(tmpStr, " (%c)", theSymbol);
        PrintRouter(dummyid, tmpStr);
      }
 
      /* make arrays with point added to beginning and end to 
         capture the extreme values of the universe of discourse
      */
      n = fv_ptr->n + 2;
      fvx = FgetArray(n);
      fvy = FgetArray(n);
      for (i=1; i<n-1; i++)
        {
         fvx[i] = fv_ptr->x[i-1];  fvy[i] = fv_ptr->y[i-1];
        }
      fvx[0]   = fv_ptr->whichDeftemplate->fuzzyTemplate->from;
      fvy[0]   = fvy[1];
      fvx[n-1] = fv_ptr->whichDeftemplate->fuzzyTemplate->to;
      fvy[n-1] = fvy[n-2];

      /* NOTE:  thisx, thisy  are current points -- at plotting points
                lastx, lasty  is 1st pt of line that crosses thisx 
                nextx, nexty  is end pt of line that crosses thisx 
      */

      thisx = from;
      previousLastk = 0;
      previousThisy = 0.0; 
      k = 0; 

      /* plot the points -- when a point is plotted it is
         necessary to go back over the 'actual' points in the array
         to see if any lie outside the y range of the point just plotted
         and the last one; if they do then plot them as well; if we
         don't do this then some points that 'fall betwen the cracks' will
         not show on the plot
      */
      while (thisx <= to && k <= NCOLS)
       {
        for (i=previousLastk; i<n-1; i++)
          {    
            lastx = fvx[i]; lasty = fvy[i];
            lastk = i;
            nextx = fvx[i+1]; nexty = fvy[i+1];
            if (thisx == lastx)
              { /* print points with same x values -- should have diff y values */
               thisy = fvy[lastk];
               j = nint(thisy*NROWS);
               PlotSpace[NROWS-j][k] = theSymbol;
               if (nextx != thisx) 
                   break;
               else
                { /* we have a vertical line -- plot it */
                  m = nint(nexty*NROWS);
                  if (m > j)
                     while (m > j)
                       { PlotSpace[NROWS-m][k] = theSymbol; m--; }
                  else
                     while (m < j)
                       { PlotSpace[NROWS-m][k] = theSymbol; m++; }
                }
              }
            else if (thisx > lastx && thisx < nextx)
              break;
          }
        if (nextx == lastx) /* same x points -- just find min and max y values */
          {
            miny = thisy; maxy = thisy;
            if (previousThisy < miny) miny = previousThisy;
            if (nexty < miny) miny = nexty;
            if (previousThisy > maxy) previousThisy = maxy;
            if (nexty > maxy) maxy = nexty;
          }
        else
          {
           /* found the points that span this point - calc y value and plot it*/
           thisy = lasty + (thisx - lastx)*(nexty - lasty)/(nextx - lastx);
           j = nint(thisy*NROWS);
           PlotSpace[NROWS-j][k] = theSymbol;
       
           /* now plot any points that between this point and the previous this
              point that lie outside of the y range of these 2 points OR
              are at same x location (vertical line)
           */
           if (thisy < previousThisy)
             { miny = thisy; maxy = previousThisy; }
           else
             { maxy = thisy; miny = previousThisy; }
	 }

        /* this second test (fvx[i] >= from) is included so that we 
           don't wander back before the start of the plot
        */
        for (i = lastk; i > previousLastk  && fvx[i] >= from; i--)
         {
          if (fvy[i] > maxy || fvy[i] < miny)
            {
              y = fvy[i];
              j = nint(y*NROWS);
              m = nint((fvx[i]-from)/deltax);
              PlotSpace[NROWS-j][m] = theSymbol;
            }
          if (fvx[i] == fvx[i-1])
            { /* we have a straight line plot it! */
              int j1, j2;
              m = nint((fvx[i]-from)/deltax);
              j1 = nint(fvy[i]*NROWS);
              j2 = nint(fvy[i-1]*NROWS);
              if (j1 > j2)
                 while (j1 > j2)
                   { PlotSpace[NROWS-j1][m] = theSymbol; j1--; }
              else
                 while (j1 < j2)
                   { PlotSpace[NROWS-j1][m] = theSymbol; j1++; }
            }
         }  

        previousLastk = lastk;
        previousThisy = thisy;
        thisx = thisx + deltax;
        k++;

       }  /* end of   while (thisx <= to && k <= NCOLS)   */

      FrtnArray(fvx, n);
      FrtnArray(fvy, n);

     }  /* end of  for (fvNum = 0 ...    */
  
   PrintRouter(dummyid, "\n\n");

   for (i=0; i<=NROWS; i++)
      { sprintf(str, "%5.2f", 1.0 - ((float)i /(float)NROWS));
        PrintRouter(dummyid, str);
        PrintRouter(dummyid, &PlotSpace[i][0]);
        PrintRouter(dummyid, "\n");
      }
   
   PrintRouter(dummyid, "     ");
   for (i=0; i<NCOLS/NTICKS; i++)
      {
        PrintRouter(dummyid, "|");
        for (j=1; j<NTICKS; j++)
            PrintRouter(dummyid, "-");
      }
   PrintRouter(dummyid, "|");
   PrintRouter(dummyid, "\n ");
   for (i=0; i<=NCOLS; i=i+2*NTICKS)
      {
        sprintf(str, "%7.2f", from+i*deltax);
        PrintRouter(dummyid, str);
        for (j=0; j<NTICKS*2-7; j++) str[j] = ' ';
        str[j] = '\0';
        PrintRouter(dummyid, str);
      }

   sprintf(str, "\n\nUniverse of Discourse:  From %6.2f  to  %6.2f\n\n",
           fv_ptr->whichDeftemplate->fuzzyTemplate->from,
           fv_ptr->whichDeftemplate->fuzzyTemplate->to);
   PrintRouter(dummyid, str);

}

  

/*******************************************************************
    Functions for setting and accessing the alpha-cut for fuzzy
    patter matching
 *******************************************************************/

/*******************************************************************
    set_alpha_value()

    Sets the alpha cut value which determines the minimum amount of 
    match between a pattern and a fuzzy value that is required to
    consider it a match.
 *******************************************************************/
globle VOID set_alpha_value()
  {
    DATA_OBJECT theArgument;
    double theAlphaValue;

    if (ArgCountCheck("set-alpha-value",EXACTLY,1) != -1)
      {
        if (ArgTypeCheck("set-alpha-value",1,INTEGER_OR_FLOAT,&theArgument) != 0)
          {
            if (GetType(theArgument) == INTEGER)
              {
                theAlphaValue = (double)DOToLong(theArgument);
              }
            else
              {
                theAlphaValue = DOToDouble(theArgument);
              }
            if (theAlphaValue < 0.0 || theAlphaValue > 1.0)
              {
                PrintErrorID("Alpha Value ",909,TRUE);
                PrintRouter(WERROR,"Expected Value in Range 0.0 to 1.0");
                PrintRouter(WERROR,".\n");
              }
            else
              {
                FuzzyAlphaValue = theAlphaValue;
                return;
              }
          }
      }
}


/********************************************************************
    get_alpha_value()

    Returns the Alpha value.
 ********************************************************************/
globle double get_alpha_value()
{
    if (ArgCountCheck("get-alpha-value",EXACTLY,0) == -1)
      {
        SetEvaluationError(TRUE);
      }

    return( FuzzyAlphaValue );
}





/*******************************************************************
    Functions to access slots of facts
 *******************************************************************/

/*******************************************************************
    get_fuzzy_slot()

    Get a fuzzy slot from a fact:

      arg 1   - the fact  (integer or fact address)
      arg 2   - the slot name (symbol) or if missing will expect
                arg 1 to be a fuzzy deftemplate fact and will
                get the only slot (fuzzy value)
 *******************************************************************/
globle struct fuzzy_value *get_fuzzy_slot()
  {
    DATA_OBJECT theArgument;
    long int factIndex;
    int found_fact;
    struct fact *factPtr;
    int error = FALSE;
    char *slotName;
    DATA_OBJECT theSlot;
    char tempBuffer[30];
    int numArgs;

   if ((numArgs = ArgCountCheck("get-fuzzy-slot",AT_LEAST,1)) != -1)
     {     
       RtnUnknown(1, &theArgument);

       if (theArgument.type == INTEGER)
        { 
           factIndex = ValueToLong(theArgument.value); 
           if (factIndex >= 0)
             {
               found_fact = FALSE;
               factPtr = (struct fact *) GetNextFact(NULL);
               while (factPtr != NULL)
                 {
                   if (factPtr->factIndex == factIndex)
                     {
                       found_fact = TRUE;
                       break;
                     }
                   factPtr = factPtr->nextFact;
                 }
       
               if (found_fact == FALSE)
                 {
                   error = TRUE;
                   sprintf(tempBuffer,"f-%ld",factIndex);
                   CantFindItemErrorMessage("fact",tempBuffer);
                 }
             }
            else
             {            
               error = TRUE;
               ExpectedTypeError1("get-fuzzy-slot",1,"fact-index must be positive");
             }
          }
        else if (theArgument.type == FACT_ADDRESS)
          { /* arg type is fact address */
            factPtr = (struct fact *) theArgument.value; 
          }  
        else
          {
            ExpectedTypeError1("get-fuzzy-slot",1,"fact-index/fact-address of fuzzy fact");
            error = TRUE;
          }

        if (!error)
          {
             if (numArgs == 1)
                slotName = NULL; /* looking for Fuzzy Deftemplate fact slot */

             if ((numArgs == 1) || ((slotName = RtnLexeme(2)) != NULL))
              {
               if (GetFactSlot((VOID *)factPtr, slotName, &theSlot))
                 {
                   if (theSlot.type == FUZZY_VALUE)
                     {
                      return( CopyFuzzyValue(ValueToFuzzyValue(theSlot.value))  );
                     }
                   else
                     {
                       if (slotName == NULL)
                          CantFindItemErrorMessage("fuzzy slot (not Fuzzy Deftemplate fact)","");
                       else
                         {
                          sprintf(tempBuffer,"'%s'",slotName);
                          CantFindItemErrorMessage("fuzzy slot (not Fuzzy Value)",tempBuffer);
                         }
                     }
                 }
               else
                 {
                   if (slotName == NULL)
                     sprintf(tempBuffer,"(no slot name specified)");
                   else
                     sprintf(tempBuffer,"'%s'",slotName);
                   CantFindItemErrorMessage("fact slot",tempBuffer);
                 }
              }
          }
      }
        
    return( NULL );
  }





/*******************************************************************
    Functions to create and operate on fuzzy values
 *******************************************************************/

/*******************************************************************
    fuzzy_union()

    Perform a fuzzy union of 2 fuzzy values

    arg 1    - fuzzy value
    arg 2    - fuzzy value
 *******************************************************************/
globle struct fuzzy_value *fuzzy_union()
  {  
    DATA_OBJECT theArgument;
    struct fuzzy_value *f1, *f2, *fresult;

   if (ArgCountCheck("fuzzy-union",EXACTLY,2) != -1)
     {     
       if (ArgTypeCheck("fuzzy-union",1,FUZZY_VALUE,&theArgument) != 0)
        { 
          f1 = ValueToFuzzyValue(theArgument.value);

          if (ArgTypeCheck("fuzzy-union",2,FUZZY_VALUE,&theArgument) != 0)
            { 
              int lenf1Name, lenf2Name;
              char *f1Name, *f2Name;
              char *str;

              f2 = ValueToFuzzyValue(theArgument.value);
              if (f2->whichDeftemplate == f1->whichDeftemplate)
		{
                 fresult = funion(f1, f2);
                 f1Name = f1->name;
                 f2Name = f2->name;
                 lenf1Name = strlen(f1Name);
                 lenf2Name = strlen(f2Name);
                 str = gm2(lenf1Name + lenf2Name + 13);
                 strcpy(str, "[ ");
                 strcpy(str+2, f1Name);
                 strcpy(str+2+lenf1Name," ] OR [ ");
                 strcpy(str+lenf1Name+10, f2Name);
                 strcpy(str+lenf1Name+lenf2Name+10, " ]");
                 if (fresult->name != NULL) rm(fresult->name, strlen(fresult->name)+1);
                 fresult->name = str;
                 return( fresult );
                }
              else
                ExpectedTypeError1("fuzzy-union",2,"fuzzy value (with same fuzzy template as arg #1)");
            }
        }
     }    

    SetEvaluationError(TRUE);
    return( NULL );    
  }


/*******************************************************************
    fuzzy_intersection()

    Perform a fuzzy intersection of 2 fuzzy values

    arg 1    - fuzzy value
    arg 2    - fuzzy value
 *******************************************************************/
globle struct fuzzy_value *fuzzy_intersection()
  {  
    DATA_OBJECT theArgument;
    struct fuzzy_value *f1, *f2, *fresult;

   if (ArgCountCheck("fuzzy-intersection",EXACTLY,2) != -1)
     {     
       if (ArgTypeCheck("fuzzy-intersection",1,FUZZY_VALUE,&theArgument) != 0)
        { 
          f1 = ValueToFuzzyValue(theArgument.value);

          if (ArgTypeCheck("fuzzy-intersection",2,FUZZY_VALUE,&theArgument) != 0)
            { 
              int lenf1Name, lenf2Name;
              char *f1Name, *f2Name;
              char *str;

              f2 = ValueToFuzzyValue(theArgument.value);
              if (f2->whichDeftemplate == f1->whichDeftemplate)
		{
                 fresult = fintersect(f1, f2);
                 f1Name = f1->name;
                 f2Name = f2->name;
                 lenf1Name = strlen(f1Name);
                 lenf2Name = strlen(f2Name);
                 str = gm2(lenf1Name + lenf2Name + 14);
                 strcpy(str, "[ ");
                 strcpy(str+2, f1Name);
                 strcpy(str+2+lenf1Name," ] AND [ ");
                 strcpy(str+lenf1Name+11, f2Name);
                 strcpy(str+lenf1Name+lenf2Name+11, " ]");
                 if (fresult->name != NULL) rm(fresult->name, strlen(fresult->name)+1);
                 fresult->name = str;
                 return( fresult );
                }
              else
                ExpectedTypeError1("fuzzy-intersection",2,"fuzzy value (with same fuzzy template as arg #1)");
            }
        }
     }    

    SetEvaluationError(TRUE);
    return( NULL );    
  }



/*******************************************************************
    fuzzy_modify()

    Perform a fuzzy modification of a fuzzy value

    arg 1    - fuzzy value
    arg 2    - fuzzy modifier (eg. not, very, slightly, etc.
 *******************************************************************/
globle struct fuzzy_value *fuzzy_modify()
  {  
    DATA_OBJECT theArgument;
    struct fuzzy_value *f1, *fresult;
    char *modifier;

   if (ArgCountCheck("fuzzy-modify",EXACTLY,2) != -1)
     {     
       if (ArgTypeCheck("fuzzy-modify",1,FUZZY_VALUE,&theArgument) != 0)
        { 
          f1 = ValueToFuzzyValue(theArgument.value);
          modifier = RtnLexeme(2);

          if (modifier != NULL)
            { 
              struct modifierListItem *mptr;

              if ((mptr = FindModifier( modifier )) != NULL)
		{
                 fresult = CopyFuzzyValue( f1 );
                 ModifyFuzzyValue(mptr, fresult);
                 return( fresult );
                }
              else
                ExpectedTypeError1("fuzzy-modify",2,"symbol AND a modifier name");
            }
          else
            ExpectedTypeError1("fuzzy-modify",2,"symbol");
        }
     }    

    SetEvaluationError(TRUE);
    return( NULL );    
  }





/*******************************************************************
    create_fuzzy_value

    Create a fuzzy value

    arg 1    - fuzzy value -- parser creates it!

 *******************************************************************/
globle struct fuzzy_value *create_fuzzy_value()
  {  
    DATA_OBJECT theArgument;

   if (ArgCountCheck("create-fuzzy-value",EXACTLY,1) != -1)
     {     
       RtnUnknown(1, &theArgument);

       if (theArgument.type == FUZZY_VALUE)
         { 
           return( CopyFuzzyValue(ValueToFuzzyValue(theArgument.value)) );
         }
       else
          ExpectedTypeError1("create-fuzzy-value",1,"fuzzy value");
     }    

    SetEvaluationError(TRUE);
    return( NULL );       
  }





/*******************************************************************

 Functions to parse special functions

     create-fuzzy-value

 *******************************************************************/



#if (! RUN_TIME)

/*******************************************************************
  CreateFuzzyValueParse:

  Parses the create-fuzzy-value function

    (create-fuzzy-value fuzzyTemplateName fuzzy-expression)
 *******************************************************************/

static struct expr *CreateFuzzyValueParse( top, logName )
  struct expr *top;
  char *logName;
{
  struct token theToken;
  struct deftemplate *theDeftemplate;
  char * theName;
  int count, error;
  struct expr *fvExpr;
  
  SavePPBuffer(" ");  /* space after the function name */

  /* this is now just like parsing a fuzzy deftemplate fact assert ...
     the template name and then the fuzzy value expression
  */

  /* First we get the deftemplate name -- must be a fuzzy deftemplate */
  GetToken(logName, &theToken);

  if (theToken.type != SYMBOL)
    {
      SyntaxErrorMessage("function create-fuzzy-value (1st argument must be a symbol)");
      ReturnExpression(top);
      return( NULL );
    }

  theName = ValueToString(theToken.value);
  if (FindModuleSeparator(theName))
    {
      theDeftemplate = (struct deftemplate *) FindDeftemplate(theName);
    }
  else
    {
      theDeftemplate = (struct deftemplate *)
            FindImportedConstruct("deftemplate",NULL,theName,
                                   &count,TRUE,NULL);
      if (count > 1)
        {
          AmbiguousReferenceErrorMessage("deftemplate",theName);
          ReturnExpression(top);
          return(NULL);
        }
     }

  if ((theDeftemplate == NULL) || 
      ((theDeftemplate != NULL) &&
       (theDeftemplate->fuzzyTemplate == NULL)
      )
     )
    {
          CantFindItemErrorMessage("fuzzy deftemplate", theName);
          ReturnExpression(top);
          return( NULL );
    }
  
   fvExpr = ParseAssertFuzzyFact(logName, &theToken, &error, RPAREN, 
                                 FALSE, theDeftemplate, FALSE);

   if (error == TRUE)
     {
       ReturnExpression(top);
       return( NULL );
     }

   top->argList = fvExpr;
   return( top );
  
}

#endif  /* !RUN_TIME */


#endif  /* FUZZY_DEFTEMPLATES */

