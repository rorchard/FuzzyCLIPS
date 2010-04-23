static char rcsid[] = "$Header: /dist/CVS/fzclips/src/fuzzyrhs.c,v 1.3 2001/08/11 21:05:58 dave Exp $" ;

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*             CLIPS Version 6.00  05/12/93            */
   /*                                                     */
   /*             FUZZY RHS PARSER MODULE                 */
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
    
    functions to parse fuzzy facts asserted on rhs
    allocation and de-allocation of arrays from CLIPS memory

    The calling of functions has the folowing hierarchy:

     ParseAssertFuzzyFact (used in deffacts and asserts)
               |
               |
               |---> assertParseFuzzySet ---|
               |                            |
               |                            |
               |---> ParseLinguisticExpr    |---> assertParseSingletonSet
                     (used by fuzzylhs.c    |
                      to parse patterns     |---> assertParseStandardSet
                      and in fuzzypsr.c to
                      parse terms in fuzzy
                      deftemplates)

 ******************************************************************/
 
#define _FUZZYRHS_SOURCE_


#include "setup.h"


#include <stdio.h>
#define _CLIPS_STDIO_

#include <math.h>
#include <string.h>

#if FUZZY_DEFTEMPLATES
#include "fuzzyrhs.h"
#include "fuzzyval.h"
#include "fuzzylv.h"
#include "fuzzyutl.h"
#include "fuzzypsr.h"
#include "fuzzymod.h"
#endif

#if FUZZY_DEFTEMPLATES || CERTAINTY_FACTORS
#include "symbol.h"
#include "expressn.h"
#include "evaluatn.h"
#include "exprnops.h"
#include "exprnpsr.h"
#include "extnfunc.h"
#include "tmpltdef.h"
#include "pprint.h"
#include "scanner.h"
#include "memalloc.h"
#include "constant.h"
#include "prntutil.h"
#include "router.h"
#endif



#if FUZZY_DEFTEMPLATES




/******************************************************************
    Global Internal Function Declarations
    
    Defined in fuzzyrhs.h
 ******************************************************************/



/******************************************************************
    Local Internal Function Declarations
 ******************************************************************/
 
  static struct fuzzy_value   *ParseLExpr(char *readSource,
                                          struct token *tempToken,
                                          struct fuzzyLv *lvp,
                                          int *error);
  static struct fuzzy_value   *ParseLTerm(char *readSource,
                                          struct token *tempToken,
                                          struct fuzzyLv *lvp,
                                          int *error);
  static struct fuzzy_value   *ParseModExpr(char *readSource,
                                            struct token *tempToken,
                                            struct fuzzyLv *lvp,
                                            int *error);
  static struct fuzzy_value   *PrimaryTerm(char *readSource,
                                           struct token *tempToken,
                                           struct fuzzyLv *lvp,
                                           int *error);
  static char                 *modifyName(char *str1, char *str2);
  static struct primary_term  *FindPrimaryTerm(struct fuzzyLv *lvp, void *pt_name);
  static struct expr          *assertParseFuzzySet(char *readSource,
                                                   struct token *tempToken,
                                                   int  *error,
                                                   struct deftemplate *t,
                                                   int constantsOnly,
                                                   int *onlyConstantsFound);
  static struct expr          *assertParseStandardSet(char *readSource,
                                                      struct token *tempToken,
                                                      int  *error,
                                                      struct deftemplate *t,
                                                      int constantsOnly,
                                                      int *onlyConstantsFound,
                                                      int function_type);
  static struct expr          *assertParseSingletonSet(char *readSource,
                                                      struct token *tempToken,
                                                      int  *error,
                                                      struct deftemplate *t,
                                                      int constantsOnly,
                                                      int *onlyConstantsFound);
  static struct fuzzy_value   *convertStandardSet(struct expr *top, int *error);
  static struct fuzzy_value   *convertSingletonSet(struct expr *top, int *error);
  static void                  expressionToFloat(struct expr *test_ptr,
                                                 double *result,
                                                 int *error);
  static void                  expressionToInteger(struct expr *test_ptr,
                                                  int *result,
                                                  int *error);

   
/******************************************************************
    Global External Variable Declarations
 ******************************************************************/



/******************************************************************
    Global Internal Variable Declarations
 ******************************************************************/



/************************************************************************
    FUNCTIONS that Copy and Compact Fuzzy Values

 ************************************************************************/
 

/*************************************************************/ 
/* CopyFuzzyValue:                                           */
/* copies a Fuzzy Value structure                            */
/*************************************************************/
globle struct fuzzy_value *CopyFuzzyValue(fv)
  struct fuzzy_value *fv;
{
    int i, num;
    struct fuzzy_value *new_fv;
   
    new_fv = get_struct(fuzzy_value);
    new_fv->name = gm2(strlen(fv->name)+1);
    strcpy(new_fv->name, fv->name);
    new_fv->whichDeftemplate = fv->whichDeftemplate;
    num = fv->n;
    
    new_fv->n = num;
    new_fv->maxn = num;
    new_fv->x = FgetArray(num);
    new_fv->y = FgetArray(num);
    
    for ( i = 0; i < num; i++)
     {
        new_fv->x[i] = fv->x[i];
        new_fv->y[i] = fv->y[i];
     }
    return (new_fv);
}



/*************************************************************/ 
/* CompactFuzzyValue:                                        */
/* compacts the membership values of a Fuzzy Value structure */
/* so that n and maxn are the same                           */
/*************************************************************/
globle void CompactFuzzyValue(fv)
  struct fuzzy_value *fv;
{
    int i, num, maxnum;
    double *xptr, *yptr;   
    
    num = fv->n;
    maxnum = fv->maxn;
    
    if (maxnum == num) return;
    
    fv->maxn = num;
    xptr = FgetArray(num);
    yptr = FgetArray(num);
    
    for ( i = 0; i < num; i++)
     {
        xptr[i] = fv->x[i];
        yptr[i] = fv->y[i];
     }
    
    FrtnArray(fv->x, maxnum);
    FrtnArray(fv->y, maxnum);
    
    fv->x = xptr;
    fv->y = yptr;
}




/************************************************************************
    FUNCTIONS THAT PARSE ASSERT ARGUMENTS for FUZZY FACTS

 ************************************************************************/
 
 
 
/***************************************************************/
/* ParseAssertFuzzyFact:                                       */
/* Returns assert argument in expr format.                     */ 
/* Error flag is set to true if an error occurs.               */
/*                                                             */
/* NOTE: don't use endType arg since it appears that it is     */
/*       ALWAYS RPAREN -- if this changes then should change   */
/*       this code to use endType rather than RPAREN           */
/***************************************************************/

#if IBM_TBC
#pragma argsused
#endif

globle struct expr *ParseAssertFuzzyFact(readSource,tempToken,
                                         error,endType,
                                         constantsOnly,theDeftemplate,
                                         variablesAllowed)
  char *readSource;
  struct token *tempToken;
  int *error;
  int endType;
  int constantsOnly; /* TRUE if only Constants allowed in fuzzy set descriptions */
  struct deftemplate *theDeftemplate;
  int variablesAllowed;
{
#if MAC_MPW
#pragma unused(endType)
#endif
    struct fuzzyLv *lvp = theDeftemplate->fuzzyTemplate;
    struct expr *next_one, *temp;
    struct fuzzy_value *fv;
    int onlyConstantsFound; /* TRUE if find only Constants in fuzzy set description */
    
    *error = FALSE;

    /*=======================================*/
    /* Put a space between the template name */
    /* and the fuzzy set definition.         */
    /*=======================================*/

    SavePPBuffer(" ");
    GetToken(readSource,tempToken);
    
    if (tempToken->type == SF_VARIABLE || tempToken->type == GBL_VARIABLE)
      {
        struct token dummyToken;
		
        /* single field variables are allowed BUT they must be of type
           FUZZY_VALUE when finally evaluated AND must have same
           fuzzy deftemplate as required by the slot
        */
        if (constantsOnly || !variablesAllowed)
          {
            *error = TRUE;
            SyntaxErrorMessage("deftemplate pattern (Variables not allowed)");
            return( NULL );
          }
        
        GetToken(readSource,&dummyToken);
        if (dummyToken.type != RPAREN)
          {
            *error = TRUE;
            SyntaxErrorMessage("Fuzzy Expression (expecting ')' to terminate)");
            return( NULL );
          }
        else
            return( GenConstant(tempToken->type, tempToken->value) );
      }
    else if ((tempToken->type == LPAREN) ||
             ((tempToken->type == SYMBOL) &&
              (strcmp(ValueToString(tempToken->value),"#") == 0)
             )
            )
       {
         /* fuzzy set specified - # is optional so expect  '# (' or  '('  */
         if (tempToken->type == SYMBOL)
           {  
             SavePPBuffer(" ");
             /* assertParseFuzzySet is expecting to see a LPAREN as current token */
             GetToken(readSource,tempToken);
           }

         next_one = assertParseFuzzySet(readSource, tempToken, error,
                                         theDeftemplate,
                                         constantsOnly, &onlyConstantsFound);
         if (*error == TRUE)
           {
            return(NULL);
           }
         else
           {            
            if (onlyConstantsFound)
              {
                fv = getConstantFuzzyValue (next_one, error);
                if (*error == TRUE)
                  {
                   ReturnExpression(next_one);
                   return(NULL);
                  }
                temp = get_struct(expr);
                temp->argList = NULL;
                temp->nextArg = NULL;
                temp->type = FUZZY_VALUE;
                temp->value = (void *)AddFuzzyValue(fv);
                /* AddFuzzyValue makes a copy of fv .. so return it */
                rtnFuzzyValue(fv);
                ReturnExpression(next_one);
                  return(temp);
              }
            else
              {
                return(next_one);
              }
           }        
       }
    else if ((fv = ParseLinguisticExpr(readSource,tempToken,lvp,error))
              == NULL)
       {
         *error = TRUE;
         return(NULL);
       }
    else
       {
         next_one = get_struct(expr);
         next_one->argList = NULL;
         next_one->nextArg = NULL;
         next_one->type = FUZZY_VALUE;
         next_one->value = (void *)AddFuzzyValue(fv);
         /* AddFuzzyValue makes a copy of fv .. so return it */
         rtnFuzzyValue(fv);
         return(next_one);
       } 
}    

/***************************************************************/
/* ParseLinguisticExpr:                                        */
/*                                                             */
/* Parses fuzzy expression with fuzzy terms, modifiers, AND,   */
/* OR and brackets ([ and ]) ONLY                              */
/*       eg.  very cold                                        */
/* Returns fuzzy value structure.                              */
/* Error flag is set to true if an error occurs.               */
/*                                                             */
/* The BNF of the linguistic expressions is:                   */
/*                                                             */
/*  <LExpr> ::= <LTerm> | <LTerm> OR <LExpr>                   */
/*                                                             */
/*  <LTerm> ::= <modExpr> | <LTerm> AND <modExpr>              */
/*                                                             */
/*  <modExpr> ::= MODIFIER <modExpr> | <element>               */
/*                                                             */
/*  <element> ::= PRIMARY-TERM | [ <LExpr> ]                   */
/*                                                             */
/* Note: This gives AND higher precedence than OR.             */
/*       MODIFIER is a valid modifier (not, very, etc.)        */
/*       PRIMARY-TERM is a term defined in a fuzzy deftemplate */
/*                                                             */
/***************************************************************/

globle struct fuzzy_value *ParseLinguisticExpr(readSource,tempToken,lvp,error)
  char *readSource;
  struct token *tempToken;
  struct fuzzyLv *lvp;
  int *error;
{
   struct fuzzy_value *fvptr;

   fvptr = ParseLExpr(readSource,tempToken,lvp,error);

   if (*error == TRUE)
      return( NULL );

   if (tempToken->type != RPAREN)
     {
       *error = TRUE;
       SyntaxErrorMessage("Fuzzy Expression (expecting ')' to terminate)");
       rtnFuzzyValue(fvptr);
       return( NULL );
     }

   PPBackup(); PPBackup(); /* remove extra space after last token */
   SavePPBuffer(")");

   return( fvptr );
}



/***************************************************************/
/* ParseLExpr:                                                 */
/*                                                             */
/* Parses a linguistic expression (see BNF in                  */
/* ParseLinguisticExpr routine above).                         */
/*                                                             */
/***************************************************************/
static struct fuzzy_value *ParseLExpr(readSource,tempToken,lvp,error)
  char *readSource;
  struct token *tempToken;
  struct fuzzyLv *lvp;
  int *error;
{
   struct fuzzy_value *fvLeft, *fvRight, *fvTemp;
   char *tmpstr, *tmpstr2;
   
   fvLeft = ParseLTerm(readSource,tempToken,lvp,error);

   if (*error)
     return( NULL );

   if ((tempToken->type == SYMBOL) && 
       ((strcmp(ValueToString(tempToken->value), "OR") == 0) ||
        (strcmp(ValueToString(tempToken->value), "or") == 0)
      ))
     {
       SavePPBuffer(" "); /* space after the OR */
       GetToken(readSource, tempToken);
       fvRight = ParseLExpr(readSource,tempToken,lvp,error);
       
       if (*error)
         {
           rtnFuzzyValue(fvLeft);
           return(NULL);
         }

       /* perform the OR (union) operation of the 2 fuzzy values */
       fvTemp = funion(fvLeft, fvRight);

       tmpstr = modifyName("OR",fvRight->name);
       tmpstr2 = modifyName(fvLeft->name, tmpstr);
       if (fvTemp->name != NULL) rm(fvTemp->name, strlen(fvTemp->name)+1);
       fvTemp->name = tmpstr2;
       rm(tmpstr, strlen(tmpstr)+1);
       rtnFuzzyValue(fvLeft);
       rtnFuzzyValue(fvRight);
       fvLeft = fvTemp;
     }
   return(fvLeft);
}


/***************************************************************/
/* ParseLTerm:                                                 */
/*                                                             */
/* Parses terms in a linguistic expression (see BNF in         */
/* ParseLinguisticExpr routine above).                         */
/*                                                             */
/***************************************************************/
static struct fuzzy_value *ParseLTerm(readSource,tempToken,lvp,error)
  char *readSource;
  struct token *tempToken;
  struct fuzzyLv *lvp;
  int *error;
{
   struct fuzzy_value *fvLeft, *fvRight, *fvTemp;
   char *tmpstr, *tmpstr2;
   
   fvLeft = ParseModExpr(readSource,tempToken,lvp,error);

   if (*error)
     return( NULL );

   if ((tempToken->type == SYMBOL) && 
       ((strcmp(ValueToString(tempToken->value), "AND") == 0) ||
        (strcmp(ValueToString(tempToken->value), "and") == 0)
      ))
     {
       SavePPBuffer(" "); /* space after the AND */
       GetToken(readSource, tempToken);
       fvRight = ParseModExpr(readSource,tempToken,lvp,error);
       
       if (*error)
         {
           rtnFuzzyValue(fvLeft);
           return(NULL);
         }

       /* perform the AND (intersection) operation of the 2 fuzzy values */
       fvTemp = fintersect(fvLeft, fvRight);

       tmpstr = modifyName("AND",fvRight->name);
       tmpstr2 = modifyName(fvLeft->name, tmpstr);
       if (fvTemp->name != NULL) rm(fvTemp->name, strlen(fvTemp->name)+1);
       fvTemp->name = tmpstr2;
       rm(tmpstr, strlen(tmpstr)+1);
       rtnFuzzyValue(fvLeft);
       rtnFuzzyValue(fvRight);
       fvLeft = fvTemp;
     }

   return(fvLeft);
}



/***************************************************************/
/* ParseLModExpr:                                              */
/*                                                             */
/* Parses modifier expressions in a linguistic expression (see */
/* ParseLinguisticExpr routine above).                         */
/*                                                             */
/***************************************************************/
static struct fuzzy_value *ParseModExpr(readSource,tempToken,lvp,error)
  char *readSource;
  struct token *tempToken;
  struct fuzzyLv *lvp;
  int *error;
{
   struct modifierListItem *mptr;
   struct fuzzy_value *fvptr;
   char *tmpstr, *tmpstr2;

   /* next token must be a symbol -- modifier or primary term or [ */
   if (tempToken->type != SYMBOL)
     {
       *error = TRUE;
       SyntaxErrorMessage("Fuzzy Expression (expecting modifier, primary term or '[' )");
       return( NULL );
     }

   /* next token could be a [ to bracket the expressions */
   if (tempToken->type == SYMBOL &&
       strcmp(ValueToString(tempToken->value), "[") == 0
      )
     {
       SavePPBuffer(" "); /* space after the [ */
       GetToken(readSource, tempToken);

       fvptr = ParseLExpr(readSource,tempToken,lvp,error);

       if (*error == TRUE)
           return( NULL );

       /* next token must now be the closing ']' */
       if (tempToken->type == SYMBOL &&
	   strcmp(ValueToString(tempToken->value), "]") == 0
	  )
         {
           SavePPBuffer(" "); /* space after the ] */
           GetToken(readSource, tempToken);
          
           /* save the [ and ] in the fv name */
           tmpstr = modifyName("[",fvptr->name);
           tmpstr2 = modifyName(tmpstr, "]");
           if (fvptr->name != NULL) rm(fvptr->name, strlen(fvptr->name)+1);
           fvptr->name = tmpstr2;
           rm(tmpstr, strlen(tmpstr)+1);

           return( fvptr );
         }

       *error = TRUE;
       SyntaxErrorMessage("Fuzzy Expression (expecting ']' )");
       rtnFuzzyValue(fvptr);
       return( NULL );
     }

   /* next token could be a modifier */
   mptr = FindModifier(ValueToString(tempToken->value));
   if (mptr != NULL)
     {
      SavePPBuffer(" "); /* space after the modifier */
      GetToken(readSource,tempToken);

      fvptr = ParseModExpr(readSource,tempToken,lvp,error);
      if (*error == TRUE) 
          return(NULL);
        
      /* perform the modifier function of the current fuzzy value */
      ModifyFuzzyValue(mptr,fvptr);
      return(fvptr);
     }

   /* if not a [ ... ] or a modifier it had better be a primary term */
   return( PrimaryTerm(readSource,tempToken,lvp,error) );
}




/***************************************************************/
/* PrimaryTerm:                                                */
/* Returns assert argument in fuzzy_value format.              */
/* Error flag is set to true if an error occurs.               */
/***************************************************************/
static struct fuzzy_value *PrimaryTerm(readSource, tempToken, lvp, error)
  char *readSource;
  struct token *tempToken;
  struct fuzzyLv *lvp;
  int *error;
{
   struct primary_term *pt;
   struct fuzzy_value *fv;

   if ((pt = FindPrimaryTerm(lvp,tempToken->value)) == NULL)
     {
       *error = TRUE;
       SyntaxErrorMessage("Fuzzy Expression (expecting a Primary Term or Modifier)");
       return(NULL);
     }
   else
     {
       fv = CopyFuzzyValue((struct fuzzy_value *) ValueToFuzzyValue(pt->fuzzy_value_description));
       SavePPBuffer(" ");
       GetToken(readSource,tempToken);
       return(fv);
     }
}




/***************************************************************/
/* ModifyFuzzyValue:                                           */
/*                                                             */
/* Takes a fuzzy value and modifies it according by operating  */
/* on each value of the fuzzy set with the function identified */
/* by the modifier                                             */
/*                                                             */
/* Modifies fuzzy_value without making a new one.              */
/***************************************************************/
globle void ModifyFuzzyValue(mptr, fv)
  struct modifierListItem *mptr;
  struct fuzzy_value *fv;
{
   char *tmpstr;

   if (fv == NULL)  /*should never happen!! */
     {  return; }

   /* modify the name eg. cold --> very cold */
   tmpstr = modifyName(mptr->name,fv->name);
   if (fv->name != NULL) rm(fv->name, strlen(fv->name)+1);
   fv->name = tmpstr;

   /* modify the fuzzy set */
   executeModifyFunction(fv,mptr);

}



/*************************************************************/
/* modifyName:                                              */
/*************************************************************/
static char *modifyName(str1,str2)
  char *str1, *str2;
{
   char *temp;
   int str1len = strlen(str1);

   temp = gm2(sizeof(char) * (str1len + strlen(str2) + 2));
   strcpy(temp,str1);
   temp[str1len]   = ' ';
   temp[str1len+1] = '\0';
   strcpy(temp+str1len+1, str2);
   return(temp);
}
   


/*************************************************************/
/* FindPrimaryTerm: Searches for primary term.               */
/* Returns a pointer to the primary term if found,           */
/* otherwise NULL.                                           */
/*************************************************************/
static struct primary_term *FindPrimaryTerm(lvp,pt_name)
  struct fuzzyLv *lvp;
  void *pt_name;
{
   struct primary_term *ptptr;

   ptptr = lvp->primary_term_list;
   while (ptptr != NULL)
     {
      struct fuzzy_value *fvptr = (struct fuzzy_value *)
                                  ValueToFuzzyValue(ptptr->fuzzy_value_description);
      if (strcmp(fvptr->name, ((SYMBOL_HN *)pt_name)->contents) == 0)
        {  return(ptptr);  }

      ptptr = ptptr->next;
     }
   return(NULL);
}
  
  
  

/******************************************************************
    assertParseFuzzySet()

    Returns fuzzy set in expanded expr format for assert
    command.  Parses constant, variable, and function type 
    fuzzy set parameters.
    
    Parses assert of fuzzy sets of the form
    
        (fuzzyvar # (1 0) (5 1) (7 0) )  -- a singleton set OR
        (fuzzyvar # (z 4 8) )            -- a standard set
        
        NOTE: the # is optional !!! This routine expects that the
              current token should be a LPAREN [ '(' ]
        
        numeric values may be expressions if constantsOnly is FALSE
        
        sets onlyConstantsFound to TRUE if no expressions used
        
 ******************************************************************/

static struct expr *assertParseFuzzySet(readSource,tempToken,error,
                       theDeftemplate,constantsOnly,onlyConstantsFound)
  char *readSource;
  struct token *tempToken;
  int  *error;
  struct deftemplate *theDeftemplate;
  int constantsOnly;
  int *onlyConstantsFound;
{
      struct expr *parse_result;
      int function_type = -1;
      
      if (tempToken->type == LPAREN)
        {
          GetToken(readSource,tempToken);
          if (tempToken->type == SYMBOL) /* check for the s, z, or PI functions */
            {
              char *tokenStr = ValueToString(tempToken->value);
              
              if (strcmp(tokenStr,"S") == 0 ||
                  strcmp(tokenStr,"s") == 0)
                {
                 function_type = S_FUNCTION;
                }
              else if (strcmp(tokenStr,"Z") == 0 ||
                       strcmp(tokenStr,"z") == 0)
                {
                 function_type = Z_FUNCTION;
                }
              else if (strcmp(tokenStr,"PI") == 0 ||
                       strcmp(tokenStr,"pi") == 0 ||
                       strcmp(tokenStr,"Pi") == 0)
                {
                 function_type = PI_FUNCTION;
                }
             }
          if (function_type != -1)
            {
              parse_result = assertParseStandardSet(readSource, tempToken, error,
                                                    theDeftemplate, constantsOnly,
                                                    onlyConstantsFound, function_type);
            }
          else
            {
              parse_result = assertParseSingletonSet(readSource, tempToken, error,
                                                     theDeftemplate,
                                                     constantsOnly, onlyConstantsFound );
            }
         }  
      else
        {
          *error = TRUE;
          SyntaxErrorMessage("Fuzzy Term (expecting Fuzzy Set description)");
        }
      if (*error == TRUE)
        {
         return(NULL);
        }
        
      return(parse_result);
}


/**********************************************************************
    assertParseStandardSet()
    
    Parses fuzzy sets of the form   (fuzzyvar (S 4 8))
    
    Function_type has aleady been determined as one of PI_FUNCTION, 
    S_FUNCTION, or Z_FUNCTION
 **********************************************************************/
static struct expr *assertParseStandardSet(readSource,tempToken,error,
                                           theDeftemplate,constantsOnly,
                                           onlyConstantsFound,function_type)
  char *readSource;
  struct token *tempToken;
  int  *error;
  struct deftemplate *theDeftemplate;
  int constantsOnly;
  int *onlyConstantsFound;
  int function_type;
{
    struct expr *top, *deft, *arg1, *arg2;
    *onlyConstantsFound = FALSE;
        
    SavePPBuffer(" ");

    top = get_struct(expr);
    top->value  = NULL;
    top->type = function_type; /* standard fuzzy function */
    top->nextArg = NULL;
        
    deft = GenConstant(DEFTEMPLATE_PTR, (void *)theDeftemplate);
    top->argList = deft;

    /* get first parameter */ 
    GetToken(readSource, tempToken);
    SavePPBuffer(" ");
    
    arg1 = tokenToFloatExpression ( readSource, tempToken, error, constantsOnly);
    if ( *error == TRUE)
      {
        ReturnExpression(top);
        return(NULL);
      }
    else
      {
        deft->nextArg = arg1;    
      }

    /* get 2nd parameter */
    GetToken(readSource, tempToken);   
    arg2 = tokenToFloatExpression ( readSource, tempToken, error, constantsOnly);
    if ( *error == TRUE)
      {
        ReturnExpression(top);
        return(NULL);
      }
    else
      {
        arg1->nextArg = arg2;
      }
        
    GetToken(readSource,tempToken);
    if (tempToken->type == RPAREN)
      {    
        if (arg1->type == FLOAT && arg2->type == FLOAT )
          { 
            *onlyConstantsFound = TRUE; 
          }
        GetToken(readSource,tempToken);
        return(top);
      }
    else
      {
        *error = TRUE;
        SyntaxErrorMessage("standard set (expecting ')' )");
        ReturnExpression(top);
        return(NULL);
      }
}

/**********************************************************************
    assertParseSingletonSet()

    Parses fuzzy sets of form   (fuzzyvar (0 0) (5 .5) (7 1) (12 0))
    
 **********************************************************************/
static struct expr *assertParseSingletonSet(readSource,tempToken,error,
                                            theDeftemplate,
                                            constantsOnly,onlyConstantsFound)
  char *readSource;
  struct token *tempToken;
  int  *error;
  struct deftemplate *theDeftemplate;
  int constantsOnly;
  int *onlyConstantsFound;
{
   
    int  count;    /* number of (x,y) pairs input */
    struct expr *top, *first, *next, *deft, *countExpr;

    *onlyConstantsFound = TRUE;
    
    /********************************************************
     Initialize start of linked list and assign first element.
     Token should be x coordinate.
     ********************************************************/
    first  = tokenToFloatExpression (readSource, tempToken, error, constantsOnly);
    if (*error == TRUE)
    {
        SyntaxErrorMessage("Singleton specification (Error in parsing Fuzzy Set x coordinate)");
        return(NULL);
    }
    next = first;
    if (next->type != FLOAT)
        *onlyConstantsFound = FALSE;

    count = 0;

    while (TRUE)
      {
        /*************************************************
        Get the next token, which should be y coordinate
        *************************************************/
        SavePPBuffer(" ");
        GetToken(readSource,tempToken);
        next->next_arg = tokenToFloatExpression(readSource, tempToken, error, constantsOnly);
        if (*error == TRUE)
          {
            SyntaxErrorMessage("Singleton specification (Error in parsing Fuzzy Set y coordinate)");
            ReturnExpression(first);
            return(NULL);
          }
        next = next->nextArg;
        if (next->type != FLOAT)
            *onlyConstantsFound = FALSE;
 
        /*********************************************************************
        Get the next token, which should be closing bracket for the(x,y) pair
        ********************************************************************/
        GetToken(readSource,tempToken);

        if (tempToken->type == RPAREN)
          {
            count++;
            SavePPBuffer(" ");
          }     
        else
          {
             *error = TRUE;
             SyntaxErrorMessage("Singleton specification (Expected ')' )");
            ReturnExpression(first);
            return(NULL);
          }
        /**************************************************************
        Get the next token, which should be either a closing bracket
        indicating the end of the set, or an opening bracket indicating
        the start of another (x,y) pair.
        *************************************************************/
        GetToken(readSource,tempToken);
        if ((tempToken->type == RPAREN) || (tempToken->type == STOP))
          {
            top = get_struct(expr);
            top->type = SINGLETON_EXPRESSION;
            top->value = NULL;
            top->nextArg = NULL;

            deft = GenConstant(DEFTEMPLATE_PTR, (void *)theDeftemplate);
            top->argList = deft;

            /* put the count of the x,y pairs as the 2nd arg in list of args */
            countExpr = GenConstant(INTEGER,(void *)AddLong((long)count));
            deft->nextArg = countExpr;
            countExpr->nextArg = first;
        
               return(top);
          }
          else if (tempToken->type != LPAREN)
          {
             *error = TRUE;
             SyntaxErrorMessage("Singleton specification (Expected '(' )");
            ReturnExpression(first);
             return(NULL);
             }
            
        /************************************************
        Get next token, which should be x coordinate.
        ************************************************/
        SavePPBuffer(" ");
        GetToken(readSource, tempToken);
        next->next_arg = tokenToFloatExpression (readSource, tempToken, error, constantsOnly);
        if (*error)
          {
            SyntaxErrorMessage("Singleton specification (Error in parsing Fuzzy Set x coordinate)");
            ReturnExpression(first);
            return(NULL);
          }
        
        next = next->nextArg;
        if (next->type != FLOAT)
                *onlyConstantsFound = FALSE;

    } /* end of while (TRUE)  */
}


/**********************************************************************
    getConstantFuzzyValue()

    Given a singleton or standard fuzzy value in expanded expr format,
    this function evaluates the parameter expressions and returns 
    a fuzzy value structure.
 **********************************************************************/
globle struct fuzzy_value *getConstantFuzzyValue( top, error )
  struct expr *top;
  int *error;
{
    struct fuzzy_value *new_fv = NULL;
    
    if (top->type == PI_FUNCTION ||
        top->type == Z_FUNCTION  ||
        top->type == S_FUNCTION)
      {
        new_fv = convertStandardSet ( top, error );
        if (*error)
            return (NULL);
      }
    else if (top->type == SINGLETON_EXPRESSION)
      {
        new_fv = convertSingletonSet ( top, error );
        if (*error == TRUE)
            return (NULL);
        
      }
    return(new_fv);
}

/**********************************************************************
    convertStandardSet()

    Evaluates parameters of a standard fuzzy value and returns a 
    fuzzy value structure.
    
    Assumes that top->type is one of PI_FUNCTION, S_FUNCTION, or
    Z_FUNCTION and that top->argList points to 4 arguments
    
       arg 1    ptr to the deftemplate associated with the FV (DEFTEMPLATE_PTR)
       arg 2    alpha value
       arg 3    value depends on function type

    NOTE: this function is similar to the function parseStandardFuzzyValue
          in fuzzypsr.c -- if this is changed consider changes to that
          routine as well

 **********************************************************************/
static struct fuzzy_value *convertStandardSet( top, error )
  struct expr *top;
  int *error;
{
    struct expr *next;
    struct fuzzy_value *fv;
    double xtolerance;
    struct deftemplate *deftPtr;
    struct fuzzyLv *fzLv;
    
    double from, to, alfa, beta, gamma;
    int function_type = top->type;

    /* get the 1st parameter  - deftemplate ptr (fuzzy) */
    next = top->argList;
    deftPtr = (struct deftemplate *)next->value;
    fzLv = deftPtr->fuzzyTemplate;
    if (fzLv == NULL) /* should never happen */
      { *error = TRUE;
        PrintRouter(WERROR,
               "Standard Function (PI, S or Z) has no Fuzzy Deftemplate (possible internal error");
        return(NULL);
      }
    from = fzLv->from;
    to = fzLv->to;

    /* get the 2rd parameter  - alpha */
    next = next->nextArg;    
    expressionToFloat(next, &alfa, error);
    if (*error == TRUE)
        return(NULL);
    
    /* We want to allow specifications for Standard functions and singleton sets
       to NOT be thrown out with an error message due to a floating point
       representation problem so we will allow the points to be considered
       within the universe of discourse (range) if very close.

       FUZZY_TOLERANCE is defined in constant.h
    */
    xtolerance = ((to - from) >= 1.0) ? FUZZY_TOLERANCE : (to-from) * FUZZY_TOLERANCE;

    if (function_type == PI_FUNCTION)
      {
        if (alfa < 0.0)
          {
            *error = TRUE;
            PrintRouter(WERROR,"PI function 1st parameter must be >= 0\n");
            return(NULL);
          }
        else 
          { 
            beta = alfa; 
          }
      }
    else if (alfa < from)
      {
        if (from-alfa > xtolerance)
          {
            *error = TRUE;
            PrintRouter(WERROR,"S or Z function 1st parameter out of range (too small)\n");
            return(NULL);
          }
        alfa = from;
      }
     else if (alfa > to)
      {
        if (alfa-to < xtolerance)
          {
            *error = TRUE;
            PrintRouter(WERROR,"S or Z function 1st parameter out of range (too large)\n");
            return(NULL);
          }
        alfa = to;
      }
       
    /* get the 3rd parameter */
    next = next->next_arg;
    
    expressionToFloat(next, &gamma, error);
    if (*error == TRUE)
        return(NULL);

    if  (function_type == PI_FUNCTION)
      { 
        if ((gamma > to) || (gamma < from))
           {
             *error = TRUE;
             PrintRouter(WERROR,"PI function produces x values out of range\n");
             return(NULL);
           }    
        else if ((gamma - beta) < from)
           {
             if (from - (gamma - beta) > xtolerance)
               {
                 *error = TRUE;
                 PrintRouter(WERROR,"PI function produces x values too small\n");
                 return(NULL);
               }
              beta = gamma - from;
           }    
        else if ((gamma + beta) > to)
           {
             if (gamma + beta - to > xtolerance)
               {
                 *error = TRUE;
                 PrintRouter(WERROR,"PI function produces x values too large\n");
                 return(NULL);
               }
              beta = to - gamma;
           }    
      }
    else if (gamma < alfa)
      {
        *error = TRUE;
        PrintRouter(WERROR,"S or Z function 2nd parameter must be >= 1st parameter\n");
        return(NULL);
      }
    else if (gamma > to)
      {
        if (gamma-to > xtolerance)
          {
            *error = TRUE;
            PrintRouter(WERROR,"S or Z function 2nd parameter out of range (too large)\n");
            return(NULL);
          }
        gamma = to;
      }
    
    /* Construct the fuzzy value -- Get_S_Z_or_PI_FuzzyValue in fuzzypsr.c */

    fv = Get_S_Z_or_PI_FuzzyValue(alfa, beta, gamma, function_type);
    fv->whichDeftemplate = deftPtr;
           
    return (fv);
}


/**********************************************************************
    convertSingletonSet()

    Evaluates parameters of a singleton fuzzy set and returns a 
    fuzzy set structure.
    Assumes that top points to an argList that has:
    
        1st arg is ptr to the deftemplate associated with the FV (DEFTEMPLATE_PTR)
        2nd arg is n    (number of fuzzy set pairs) (will be a constant integer)
        3rd, 4th args are 1st x,y pair
        5th, 6th args are 2nd x,y pair
        etc.

    NOTE: this function is similar to the function parseSingletonFuzzyValue
          in fuzzypsr.c -- if this is changed consider changes to that
          routine as well

 **********************************************************************/
static struct fuzzy_value *convertSingletonSet( top, error )
  struct expr *top;
  int *error;
{
    struct expr *next;
    struct fuzzy_value *fv;
    int num, i, numpairs_retrieved;
    double newx, newy, previous;
    double from, to;
    double xtolerance;
    struct deftemplate *deftPtr;
    struct fuzzyLv *fzLv;
    
    /* get the 1st parameter  - deftemplate ptr (fuzzy) */
    next = top->argList;
    deftPtr = (struct deftemplate *)next->value;
    fzLv = deftPtr->fuzzyTemplate;
    if (fzLv == NULL) /* should never happen */
      { *error = TRUE;
        PrintRouter(WERROR,
               "Standard Function (PI, S or Z) has no Fuzzy Deftemplate (possible internal error");
        return(NULL);
      }
    from = fzLv->from;
    to = fzLv->to;

    /**************************************************************
     Initialize the fuzzy set - 2nd parameter gives required size
     **************************************************************/
    next = next->nextArg;
    expressionToInteger(next, &num, error);
    if (*error == TRUE)   /* should never happen */
        return(NULL);

    fv = get_struct(fuzzy_value);
    fv->name = gm2(4);
    strcpy(fv->name, "???");
    fv->whichDeftemplate = deftPtr;

    fv->n = num;
    fv->maxn = num;
    fv->x = FgetArray(num);
    fv->y = FgetArray(num);

    previous = from - 1.0;
    next = next->nextArg;
    
    /* We want to allow specifications for Standard functions and singleton sets
       to NOT be thrown out with an error message due to a floating point
       representation problem so we will allow the points to be considered
       within the universe of discourse (range) if very close.

       FUZZY_TOLERANCE is defined in constant.h
    */
    xtolerance = ((to - from) >= 1.0) ? FUZZY_TOLERANCE : (to-from) * FUZZY_TOLERANCE;

    i = 0; /* index into the array for storing values */
    numpairs_retrieved = 0; /* count of the pairs obtained from the expression */
    
    while ( numpairs_retrieved < num && next != NULL )
     {
        /************************************************
         Next Expression should be x coordinate.
         ************************************************/
        expressionToFloat(next, &newx, error);
        if (*error == FALSE)
          {
            if (newx > to)
              {
                if (newx-to > xtolerance)
                  {
                    *error = TRUE;
                    PrintRouter(WERROR,"X coordinate of Fuzzy Set out of range (too large)\n");
                  }
                newx = to;
              }
            else if (newx < from)
              {
                if (from-newx > xtolerance)
                  {
                    *error = TRUE;
                    PrintRouter(WERROR,"X coordinate of Fuzzy Set out of range (too small)\n");
                  }
                newx = from;
              }
 
            /* cannot have x value less than a previous one (if close we
               can consider it the same as previous -- rounding errors
               may have occurred)
            */
            if (newx < previous)
              {
               if (previous - newx > FUZZY_TOLERANCE)
                 {
                   *error = TRUE;
                   PrintRouter(WERROR,"(x,y) pairs should be in increasing x order in Fuzzy Set\n");
                   PrintRouter(WERROR,"      and successive x values must not be the same\n");
                 }
               else
                 newx = previous;
              }
          }
        if (*error == TRUE)
          {
            rtnFuzzyValue(fv);
            return(NULL);
          }
        fv->x[i] = newx;
          previous = newx;
        
        /***********************************************
         Next Expression should be y coordinate.
         ***********************************************/
        next = next->nextArg;
        if ( next == NULL )
        {    
            *error = TRUE;
            PrintRouter(WERROR,"Y coordinate of fuzzy set missing (possible internal error)\n");
            rtnFuzzyValue(fv);
            return(NULL);
        }
        expressionToFloat(next, &newy, error);
        if (*error == FALSE)
          {
              if (newy < 0.0)
              {
                if (newy < -FUZZY_TOLERANCE)
                  {
                    *error = TRUE;
                    PrintRouter(WERROR,"Fuzzy membership value (y coordinate) must be >= 0.0\n");
                  }
                newy = 0.0;
              }
              if (newy > 1.0)
              {
                if (newy-1.0 > FUZZY_TOLERANCE)
                  {
                    *error = TRUE;
                    PrintRouter(WERROR,"Fuzzy membership value (y coordinate) must be <= 1.0\n");
                  }
                newy = 1.0;
              }
          }
        if (*error == TRUE)
          {
            rtnFuzzyValue(fv);
            return(NULL);
          }
          
        /* if this point same as last don't store it */
        if (i==0 || !FZ_EQUAL(newx, fv->x[i-1]) || !FZ_EQUAL(newy,fv->y[i-1]))
          {
           /* when adding a y value make sure it is NOT the same as 1st value if
              it is the second one in the array OR that it is not same as previous 2
              if it is past the 3rd pair (ie. i > 2)
           */
           if ( (i == 1 && newy == fv->y[0]) ||
                (i > 2 && newy == fv->y[i-1] && newy == fv->y[i-2]) )
             {
               i--;
               fv->x[i] = newx;
               if (i == 0)
                  previous = from - 1.0;
             }
           
           fv->y[i] = newy;
           i++;
          }

          /* if last 3 points all have same x values and all y values
             are in increasing order or all in decreasing order, then
             replace 2nd one with the last one. If last 4 are the same
             replace 2nd last one with last one.
          */
          if (i >2 && newx == fv->x[i-2] && newx == fv->x[i-3])
            {
             if ((newy > fv->y[i-2] && fv->y[i-2] > fv->y[i-3]) ||
                 (newy < fv->y[i-2] && fv->y[i-2] < fv->y[i-3]) ||
                 (i > 3 && newx == fv->x[i-4])
                )
               { i--; fv->y[i-1] = fv->y[i]; }
            }
        next = next->nextArg;
        numpairs_retrieved++;
      }
    if ( numpairs_retrieved != num || next != NULL)
      {
        /* impossible error ??? */
        *error = TRUE;
        PrintRouter(WERROR,"Fuzzy set - incorrect number of (x,y) pairs - internal error\n");
        rtnFuzzyValue(fv);
          return(NULL);
      }

    /* finally if more than 2 pairs check that the last 2 are not the same  -- if so
       just discard the last one */
    if (i > 2 && fv->y[i-1] == fv->y[i-2])
       i--;
      
    fv->n = i;

    if ((num - i) > 5)
       CompactFuzzyValue( fv );
    
    return(fv);
}



/**********************************************************************
    expressionToFloat()

    Given an expr structure which has been formed by tokenToFloatExpression(),
    this function evaluates the expression and returns the float value.
 **********************************************************************/
static void expressionToFloat(exprPtr, result, error)
  struct expr *exprPtr;
  double *result;
  int *error;
{

  DATA_OBJECT exprValue;
  int type;
  
  EvaluateExpression( exprPtr, &exprValue );
  type = exprValue.type;
  
  if (type == INTEGER)
     *result = (double) DOToLong(exprValue);
     
  else if (type == FLOAT)
     *result = DOToDouble(exprValue);
     
  else 
    {
      *error = TRUE;
      PrintRouter(WERROR,"Fuzzy set value (expecting FLOAT value)\n");
    }
}





/**********************************************************************
    expressionToInteger()

    Given an expr structure which has been formed by tokenToFloatExpression(),
    this function evaluates the expression and returns the integer value.
 **********************************************************************/
static void expressionToInteger(exprPtr, result, error)
  struct expr *exprPtr;
  int *result;
  int *error;
{

  DATA_OBJECT exprValue;
  int type;
  
  EvaluateExpression( exprPtr, &exprValue );
  type = exprValue.type;
  
  if (type == INTEGER)
     *result = (int) DOToLong(exprValue);
     
  else 
    {
      *error = TRUE;
      PrintRouter(WERROR,"Fuzzy set internal evaluation (expecting int value)\n");
    }
}




/************************************************************************
    FUNCTIONS FOR ALLOCATING AND DE-ALLOCATING MEMORY 
************************************************************************/

/************************************************************************
    FgetArray(length)
 
    Allocates memory for a floating point array of size "length" and
    returns a pointer to the allocated array.
 ************************************************************************/
globle double *FgetArray ( length )
  int length;
{
    double *p;

    /* Use CLIPS memory management function gm1() */
    p = (double *) gm1 ( length * (sizeof(double)) );
    return (p);
}

/************************************************************************
    FrtnArray(p, length )
    
    Deallocates memory of floating point array p of size "length".
 ************************************************************************/
globle void FrtnArray ( p, length )
  double *p;
  int length;
{
    rm ( p, length * (sizeof(double)) );    /* a CLIPS memory function */
}

/************************************************************************
    IgetArray(length)
 
    Allocates memory for an integer array of size "length" and
    returns a pointer to the allocated array.
 ************************************************************************/
globle int *IgetArray ( length )
  int length;
{
    int *p;

    /* Use CLIPS memory management function gm1() */
    p = (int *) gm1 ( length * (sizeof(int)) );
    return (p);
}

/************************************************************************
    IrtnArray(p, length )
    
    Deallocates memory of integer array p of size "length".
 ************************************************************************/
globle void IrtnArray ( p, length )
  int *p;
  int length;
{
    rm ( p, length * (sizeof(int)) );    /* a CLIPS memory function */
}

/************************************************************************
    CgetArray(length)
 ************************************************************************/
globle char *CgetArray ( length )
  int length;
{
    char *p = NULL;
    p = (char *) gm1 ( length * (sizeof (char)) );
    return (p);
}

/************************************************************************
    CrtnArray ( p, length )
 ************************************************************************/
globle void CrtnArray ( p, length )
char *p;
int length;
{
    rm ( p, length * (sizeof (char) ) );
}



#endif  /* FUZZY_DEFTEMPLATES */


/***********************************************

 Following routine used by FUZZY and CERTAINTY 
 FACTOR code

***********************************************/

#if FUZZY_DEFTEMPLATES || CERTAINTY_FACTORS



/****************************************************************
    tokenToFloatExpression()

    Given the next token, this function
    parses constants, variables and functions and returns
    appropriate expr structures
 ****************************************************************/
globle struct expr *tokenToFloatExpression ( readSource, tempToken, error, constantsOnly )
  char *readSource;
  struct token *tempToken;
  int  *error;
  int constantsOnly;
{
    struct expr *result=NULL;
    int exprType;

    if (tempToken->type == FLOAT || tempToken->type == INTEGER)    
      {
       /******************************************************
        Constant - FLOAT or INTEGER allowed
        ******************************************************/
           if (tempToken->type == INTEGER)
            result = GenConstant(FLOAT,
                                 (void *) AddDouble((double) ValueToLong(tempToken->value)));
         else
            result = GenConstant(FLOAT,tempToken->value);
            
         return(result);

         }
        
   /*=============================================================*/
   /* If an equal sign or left parenthesis was parsed, then parse */
   /* a function which is to be evaluated to determine the        */
   /* value. The equal sign corresponds to the return value       */
   /* constraint which can be used in LHS fact patterns. The      */
   /* equal sign is no longer necessary on either the LHS or RHS  */
   /* of a rule to indicate that a function is being evaluated to */
   /* determine its value either for assignment or pattern        */
   /* matching.                                                   */
   /*=============================================================*/
   
   if ((tempToken->type == SYMBOL) ? 
       (strcmp(ValueToString(tempToken->value),"=") == 0) : 
       (tempToken->type == LPAREN))
     {
      if (constantsOnly)
        {
         SyntaxErrorMessage("numeric expression (Constants Only Allowed)");
         *error = TRUE;
         return(NULL);
        }

#if ! RUN_TIME
      if (tempToken->type == LPAREN) 
         result = Function1Parse(readSource);
      else 
         result = Function0Parse(readSource);
#endif
      if (result == NULL)
        {
         *error = TRUE;
        }
      else
        {
          exprType = ExpressionFunctionType(result);
          if (GetStaticConstraintChecking() == TRUE)
            if ((result->type == FCALL) && 
                 exprType != 'd' &&
                  exprType != 'f' &&
                  exprType != 'n' &&
                 exprType != 'i' 
               )
              {
               SyntaxErrorMessage("numeric expression (Expected numeric result from function)");
               *error = TRUE;
               ReturnExpression(result);
               return( NULL );
              }
         }
       
      return(result);
     }

   /*========================================*/
   /* Variables are also allowed as RHS      */
   /* values under some circumstances.       */
   /*========================================*/
   
   if ((tempToken->type == SF_VARIABLE) 
#if DEFGLOBAL_CONSTRUCT
       || (tempToken->type == GBL_VARIABLE) 
#endif
      )
     {
      if (constantsOnly)
        {
         *error = TRUE;
         return(NULL);
        }

      return(GenConstant(tempToken->type,tempToken->value));
     }

   /*==========================================================*/
   /* If none of the other case have been satisfied, then the  */
   /* token parsed is not appropriate for a numeric expression */
   /*==========================================================*/
   
   SyntaxErrorMessage("singleton or standard fuzzy set (Numeric expression expected)");
   *error = TRUE;
   return(NULL);

}



#endif  /*  FUZZY_DEFTEMPLATES || CERTAINTY_FACTORS  */
