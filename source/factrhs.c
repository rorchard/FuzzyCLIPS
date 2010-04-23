static char rcsid[] = "$Header: /dist/CVS/fzclips/src/factrhs.c,v 1.3 2001/08/11 21:05:46 dave Exp $" ;

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*            FACT RHS PATTERN PARSER MODULE           */
   /*******************************************************/

/*************************************************************/
/* Purpose: Provides a number of routines for parsing fact   */
/*   patterns typically found on the RHS of a rule (such as  */
/*   the assert command). Also contains some functions for   */
/*   parsing RHS slot values (used by functions such as      */
/*   assert, modify, and duplicate).                         */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*      Chris Culbert                                        */
/*      Brian L. Donnell                                     */
/*      Bob Orchard (NRCC - Nat'l Research Council of Canada)*/
/*                  (Fuzzy reasoning extensions)             */
/*                  (certainty factors for facts and rules)  */
/*                  (extensions to run command)              */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#define _FACTRHS_SOURCE_

#include <stdio.h>
#define _STDIO_INCLUDED_
#include <string.h>

#include "setup.h"

#if DEFTEMPLATE_CONSTRUCT

#include "constant.h"
#include "prntutil.h"
#include "extnfunc.h"
#include "pattern.h"
#include "modulutl.h"
#include "modulpsr.h"
#include "cstrcpsr.h"

#if BLOAD_AND_BSAVE || BLOAD || BLOAD_ONLY
#include "bload.h"
#endif

#include "tmpltpsr.h"
#include "tmpltrhs.h"
#include "tmpltutl.h"
#include "exprnpsr.h"
#include "strngrtr.h"
#include "router.h"

#include "factrhs.h"

#if FUZZY_DEFTEMPLATES 
#include "rulepsr.h"
#endif

#if CERTAINTY_FACTORS || FUZZY_DEFTEMPLATES 
#include "fuzzyrhs.h"
#endif

#if CERTAINTY_FACTORS 
#include "cfdef.h"
#endif


/***************************************/
/* LOCAL INTERNAL FUNCTION DEFINITIONS */
/***************************************/

#if RUN_TIME || BLOAD_ONLY || BLOAD || BLOAD_AND_BSAVE
   static void                       NoSuchTemplateError(char *);
#endif

#if (! RUN_TIME)

/**********************************************************************/
/* BuildRHSAssert: Parses zero or more RHS fact patterns (the format  */
/*   which is used by the assert command and the deffacts construct). */
/*   Each of the RHS patterns is attached to an assert command and if */
/*   there is more than one assert command, then a progn command is   */
/*   wrapped around all of the assert commands.                       */
/**********************************************************************/
globle struct expr *BuildRHSAssert(
  char *logicalName,
  struct token *theToken,
  int *error,
  int atLeastOne,
  int readFirstParen,
  char *whereParsed)
  {
   struct expr *lastOne, *nextOne, *assertList, *stub;
   *error = FALSE;

   /*===============================================================*/
   /* If the first parenthesis of the RHS fact pattern has not been */
   /* read yet, then get the next token. If a right parenthesis is  */
   /* encountered then exit (however, set the error return value if */
   /* at least one fact was expected).                              */
   /*===============================================================*/

   if (readFirstParen == FALSE)
     {
      if (theToken->type == RPAREN)
        {
         if (atLeastOne)
           {
            *error = TRUE;
            SyntaxErrorMessage(whereParsed);
           }
         return(NULL);
        }
     }

   /*================================================*/
   /* Parse the facts until no more are encountered. */
   /*================================================*/

   lastOne = assertList = NULL;
   while ((nextOne = GetRHSPattern(logicalName,theToken,
                                   error,FALSE,readFirstParen,
                                   TRUE,RPAREN)) != NULL)
     {
      PPCRAndIndent();

      stub = GenConstant(FCALL,(void *) FindFunction("assert"));
      stub->argList = nextOne;
      nextOne = stub;

      if (lastOne == NULL)
        { assertList = nextOne; }
      else
        { lastOne->nextArg = nextOne; }
      lastOne = nextOne;

      readFirstParen = TRUE;
     }

   /*======================================================*/
   /* If an error was detected while parsing, then return. */
   /*======================================================*/

   if (*error)
     {
      ReturnExpression(assertList);
      return(NULL);
     }

   /*======================================*/
   /* Fix the pretty print representation. */
   /*======================================*/

   if (theToken->type == RPAREN)
     {
      PPBackup();
      PPBackup();
      SavePPBuffer(")");
     }

   /*==============================================================*/
   /* If no facts are being asserted then return NULL. In addition */
   /* if at least one fact was required, then signal an error.     */
   /*==============================================================*/

   if (assertList == NULL)
     {
      if (atLeastOne)
        {
         *error = TRUE;
         SyntaxErrorMessage(whereParsed);
        }

      return(NULL);
     }

   /*===============================================*/
   /* If more than one fact is being asserted, then */
   /* wrap the assert commands within a progn call. */
   /*===============================================*/

   if (assertList->nextArg != NULL)
     {
      stub = GenConstant(FCALL,(void *) FindFunction("progn"));
      stub->argList = assertList;
      assertList = stub;
     }

   /*==========================================================*/
   /* Return the expression for asserting the specified facts. */
   /*==========================================================*/

   return(assertList);
  }

#endif

/***************************************************************/
/* GetRHSPattern: Parses a single RHS fact pattern. The return */
/*   value is the fact just parsed (or NULL if the delimiter   */
/*   for no more facts is the first token parsed). If an error */
/*   occurs, then the error flag passed as an argument is set. */
/***************************************************************/
globle struct expr *GetRHSPattern(
  char *readSource,
  struct token *tempToken,
  int *error,
  int constantsOnly,
  int readFirstParen,
  int checkFirstParen,
  int endType)
  {
   struct expr *lastOne = NULL;
   struct expr *nextOne, *firstOne, *argHead = NULL;
   int printError, count;
   struct deftemplate *theDeftemplate;
   struct symbolHashNode *templateName;

   /*=================================================*/
   /* Get the opening parenthesis of the RHS pattern. */
   /*=================================================*/

   *error = FALSE;

   if (readFirstParen) GetToken(readSource,tempToken);

   if (checkFirstParen)
     {
      if (tempToken->type == endType) return(NULL);

      if (tempToken->type != LPAREN)
        {
         SyntaxErrorMessage("RHS patterns");
         *error = TRUE;
         return(NULL);
        }
     }

   /*======================================================*/
   /* The first field of an asserted fact must be a symbol */
   /* (but not = or : which have special significance).    */
   /*======================================================*/

   GetToken(readSource,tempToken);
   if (tempToken->type != SYMBOL)
     {
      SyntaxErrorMessage("first field of a RHS pattern");
      *error = TRUE;
      return(NULL);
     }
   else if ((strcmp(ValueToString(tempToken->value),"=") == 0) ||
            (strcmp(ValueToString(tempToken->value),":") == 0))
     {
      SyntaxErrorMessage("first field of a RHS pattern");
      *error = TRUE;
      return(NULL);
     }

   /*=========================================================*/
   /* Check to see if the relation name is a reserved symbol. */
   /*=========================================================*/

   templateName = (struct symbolHashNode *) tempToken->value;

   if (ReservedPatternSymbol(ValueToString(templateName),NULL))
     {
      ReservedPatternSymbolErrorMsg(ValueToString(templateName),"a relation name");
      *error = TRUE;
      return(NULL);
     }

   /*============================================================*/
   /* A module separator in the name is illegal in this context. */
   /*============================================================*/

   if (FindModuleSeparator(ValueToString(templateName)))
     {
      IllegalModuleSpecifierMessage();

      *error = TRUE;
      return(NULL);
     }

   /*=============================================================*/
   /* Determine if there is an associated deftemplate. If so, let */
   /* the deftemplate parsing functions parse the RHS pattern and */
   /* then return the fact pattern that was parsed.               */
   /*=============================================================*/

   theDeftemplate = (struct deftemplate *)
                    FindImportedConstruct("deftemplate",NULL,ValueToString(templateName),
                                          &count,TRUE,NULL);

   if (count > 1)
     {
      AmbiguousReferenceErrorMessage("deftemplate",ValueToString(templateName));
      *error = TRUE;
      return(NULL);
     }

   /*======================================================*/
   /* If no deftemplate exists with the specified relation */
   /* name, then create an implied deftemplate.            */
   /*======================================================*/

   if (theDeftemplate == NULL)
#if (! BLOAD_ONLY) && (! RUN_TIME)
     {
#if BLOAD || BLOAD_AND_BSAVE
      if ((Bloaded()) && (! CheckSyntaxMode))
        {
         NoSuchTemplateError(ValueToString(templateName));
         *error = TRUE;
         return(NULL);
        }
#endif
#if DEFMODULE_CONSTRUCT
      if (FindImportExportConflict("deftemplate",((struct defmodule *) GetCurrentModule()),ValueToString(templateName)))
        {
         ImportExportConflictMessage("implied deftemplate",ValueToString(templateName),NULL,NULL);
         *error = TRUE;
         return(NULL);
        }
#endif
      if (! CheckSyntaxMode)
        { theDeftemplate = CreateImpliedDeftemplate((SYMBOL_HN *) templateName,TRUE); }
     }
#else
    {
     NoSuchTemplateError(ValueToString(templateName));
     *error = TRUE;
     return(NULL);
    }
#endif

   /*=========================================*/
   /* If an explicit deftemplate exists, then */
   /* parse the fact as a deftemplate fact.   */
   /*=========================================*/

   if ((theDeftemplate != NULL) && (theDeftemplate->implied == FALSE))
     {
      firstOne = GenConstant(DEFTEMPLATE_PTR,theDeftemplate);
#if FUZZY_DEFTEMPLATES 
      if (theDeftemplate->fuzzyTemplate != NULL)
         firstOne->nextArg = ParseAssertFuzzyFact(readSource,tempToken,
                                              error,endType,
                                              constantsOnly,theDeftemplate,
                                              TRUE);
      else
#endif

      firstOne->nextArg = ParseAssertTemplate(readSource,tempToken,
                                              error,endType,
                                              constantsOnly,theDeftemplate);
      if (*error)
        {
         ReturnExpression(firstOne);
         firstOne = NULL;
        }

#if CERTAINTY_FACTORS 
      else
        {
          /* if certaintly factors allowed then the next item after a fact
             specifier COULD be a certainty factor spec --- CF x.xxx
          */
          SavePPBuffer(" ");
          GetToken(readSource,tempToken);
          if ((tempToken->type == SYMBOL) &&
              ((strcmp(ValueToString(tempToken->value),"CF") == 0) ||
               (strcmp(ValueToString(tempToken->value),"cf") == 0))
             )
            {
              struct expr *CFexpr;
              /* expecting a certainty factor (float) expression */
              /* tokenToFloatExpression expect 1st token already read */
              SavePPBuffer(" ");
              GetToken(readSource,tempToken);
              CFexpr = tokenToFloatExpression(readSource,tempToken,error,constantsOnly);

              if (*error)
                {
                  ReturnExpression(firstOne);
                  return( NULL );
                }

              if (CFexpr->type == FLOAT) /* if constant -- check range */
                {
                  double cfval = ValueToDouble(CFexpr->value);
                  if (cfval > 1.0 || cfval < 0.0)
                    {
                      *error = TRUE;
                      ReturnExpression(CFexpr);
                      cfRangeError();
                      ReturnExpression(firstOne);
                      return( NULL );
                    }
                }

             /* store the CF expression in the argList of the DEFTEMPLATE_PTR expr */
             firstOne->argList = CFexpr;
            }
          else
            {
              /* Do an 'UnGetToken' function here to undo the lookahead for a CF.
                 Also need to PPBackup over the space added before reading the
                 potential CF expression -- UnGetToken does one PPBackup over the
                 token which was added to the PP Buffer
              */
              UnGetToken(tempToken);
              PPBackup();
            }
        }
#endif

      return(firstOne);
     }


   /*========================================*/
   /* Parse the fact as an ordered RHS fact. */
   /*========================================*/

   firstOne = GenConstant(DEFTEMPLATE_PTR,theDeftemplate);

#if FUZZY_DEFTEMPLATES
   /*=============================================*/
   /* Fuzzy facts parsed differently              */
   /*=============================================*/
   if (theDeftemplate->fuzzyTemplate != NULL)
     {
       firstOne->nextArg = ParseAssertFuzzyFact(readSource,tempToken,
                                                error,endType,
                                                constantsOnly,theDeftemplate,
                                                TRUE);
       if (*error)
         {
          ReturnExpression(firstOne);
          return(NULL);
         }
     }
   else
     { /*   --- matches } below with FUZZY_DEFTEMPLATES */

#endif   /* FUZZY_DEFTEMPLATES */


#if (! RUN_TIME) && (! BLOAD_ONLY)
   SavePPBuffer(" ");
#endif

   while ((nextOne = GetAssertArgument(readSource,tempToken,
                                        error,endType,constantsOnly,&printError)) != NULL)
     {
      if (argHead == NULL) argHead = nextOne;
      else lastOne->nextArg = nextOne;
      lastOne = nextOne;
#if (! RUN_TIME) && (! BLOAD_ONLY)
      SavePPBuffer(" ");
#endif
     }

   /*===========================================================*/
   /* If an error occurred, set the error flag and return NULL. */
   /*===========================================================*/

   if (*error)
     {
      if (printError) SyntaxErrorMessage("RHS patterns");
      ReturnExpression(firstOne);
      ReturnExpression(argHead);
      return(NULL);
     }

   /*=====================================*/
   /* Fix the pretty print representation */
   /* of the RHS ordered fact.            */
   /*=====================================*/

#if (! RUN_TIME) && (! BLOAD_ONLY)
   PPBackup();
   PPBackup();
   SavePPBuffer(tempToken->printForm);
#endif

   /*==========================================================*/
   /* Ordered fact assertions are processed by stuffing all of */
   /* the fact's proposition (except the relation name) into a */
   /* single multifield slot.                                  */
   /*==========================================================*/

   firstOne->nextArg = GenConstant(FACT_STORE_MULTIFIELD,AddBitMap("\0",1));
   firstOne->nextArg->argList = argHead;

#if FUZZY_DEFTEMPLATES 
     }  /*    --- matches else { above with FUZZY_DEFTEMPLATES */
#endif

#if CERTAINTY_FACTORS 
   /* if certaintly factors allowed then the next item after a fact
          specifier could be a certainty factor spec --- CF x.xxx
   */
#if (! RUN_TIME) && (! BLOAD_ONLY)
   SavePPBuffer(" ");
#endif
   GetToken(readSource,tempToken);
   if ((tempToken->type == SYMBOL) &&
           ((strcmp(ValueToString(tempToken->value),"CF") == 0) ||
                (strcmp(ValueToString(tempToken->value),"cf") == 0))
          )
         {
            struct expr *CFexpr;

                /* expecting a certainty factor (float) expression */
        /* tokenToFloatExpression expect 1st token already read */
#if (! RUN_TIME) && (! BLOAD_ONLY)
        SavePPBuffer(" ");
#endif
        GetToken(readSource,tempToken);
                CFexpr = tokenToFloatExpression(readSource,tempToken,error,constantsOnly);

            if (*error)
          {
            ReturnExpression(firstOne);
            return( NULL );
          }

            if (CFexpr->type == FLOAT) /* if constant -- check range */
                  {
                    double cfval = ValueToDouble(CFexpr->value);
                        if (cfval > 1.0 || cfval < 0.0)
                          {
                            *error = TRUE;
                ReturnExpression(CFexpr);
                            cfRangeError();
                ReturnExpression(firstOne);
                return( NULL );
                          }
                  }

                /* store the CF expression in the argList of the DEFTEMPLATE_PTR expr */
            firstOne->argList = CFexpr;
          }
        else
      {
            /* Do an 'UnGetToken' function here to undo the lookahead for a CF.
                   Also need to PPBackup over the space added before reading the
                   potential CF expression -- UnGetToken does one PPBackup over the
                   token which was added to the PP Buffer
                */
                UnGetToken(tempToken);
#if (! RUN_TIME) && (! BLOAD_ONLY)
        PPBackup();
#endif
      }

#endif   /* CERTAINTY_FACTORS */


   /*==============================*/
   /* Return the RHS ordered fact. */
   /*==============================*/

   return(firstOne);
  }

/********************************************************************/
/* GetAssertArgument: Parses a single RHS slot value and returns an */
/*   expression representing the value. When parsing a deftemplate  */
/*   slot, the slot name has already been parsed when this function */
/*   is called. NULL is returned if a slot or fact delimiter is     */
/*   encountered. In the event of a parse error, the error flag     */
/*   passed as an argument is set.                                  */
/********************************************************************/
globle struct expr *GetAssertArgument(
  char *logicalName,
  struct token *theToken,
  int *error,
  int endType,
  int constantsOnly,
  int *printError)
  {
   struct expr *nextField;

   /*=================================================*/
   /* Read in the first token of the slot's value. If */
   /* the end delimiter is encountered, then return.  */
   /*=================================================*/

   *printError = TRUE;
   GetToken(logicalName,theToken);
   if (theToken->type == endType) return(NULL);

   /*=============================================================*/
   /* If an equal sign of left parenthesis was parsed, then parse */
   /* a function which is to be evaluated to determine the slot's */
   /* value. The equal sign corresponds to the return value       */
   /* constraint which can be used in LHS fact patterns. The      */
   /* equal sign is no longer necessary on either the LHS or RHS  */
   /* of a rule to indicate that a function is being evaluated to */
   /* determine its value either for assignment or pattern        */
   /* matching.                                                   */
   /*=============================================================*/

   if ((theToken->type == SYMBOL) ?
       (strcmp(ValueToString(theToken->value),"=") == 0) :
       (theToken->type == LPAREN))
     {
      if (constantsOnly)
        {
         *error = TRUE;
         return(NULL);
        }

#if ! RUN_TIME
      if (theToken->type == LPAREN) nextField = Function1Parse(logicalName);
      else nextField = Function0Parse(logicalName);
      if (nextField == NULL)
#endif
        {
         *printError = FALSE;
         *error = TRUE;
        }
#if ! RUN_TIME
      else
        {
         theToken->type= RPAREN;
         theToken->value = (void *) AddSymbol(")");
         theToken->printForm = ")";
        }
#endif

      return(nextField);
     }

   /*==================================================*/
   /* Constants are always allowed as RHS slot values. */
   /*==================================================*/

   if ((theToken->type == SYMBOL) || (theToken->type == STRING) ||
#if OBJECT_SYSTEM
           (theToken->type == INSTANCE_NAME) ||
#endif
           (theToken->type == FLOAT) || (theToken->type == INTEGER))
     { return(GenConstant(theToken->type,theToken->value)); }

   /*========================================*/
   /* Variables are also allowed as RHS slot */
   /* values under some circumstances.       */
   /*========================================*/

   if ((theToken->type == SF_VARIABLE) ||
#if DEFGLOBAL_CONSTRUCT
            (theToken->type == GBL_VARIABLE) ||
            (theToken->type == MF_GBL_VARIABLE) ||
#endif
            (theToken->type == MF_VARIABLE))
     {
      if (constantsOnly)
        {
         *error = TRUE;
         return(NULL);
        }

      return(GenConstant(theToken->type,theToken->value));
     }

   /*==========================================================*/
   /* If none of the other cases have been satisfied, then the */
   /* token parsed is not appropriate for a RHS slot value.    */
   /*==========================================================*/

   *error = TRUE;
   return(NULL);
  }

/****************************************************/
/* StringToFact: Converts the string representation */
/*   of a fact to a fact data structure.            */
/****************************************************/
globle struct fact *StringToFact(
  char *str)
  {
   struct token theToken;
   struct fact *factPtr;
   int numberOfFields = 0, whichField;
   struct expr *assertArgs, *tempPtr;
   int error = FALSE;
   DATA_OBJECT theResult;

   /*=========================================*/
   /* Open a string router and parse the fact */
   /* using the router as an input source.    */
   /*=========================================*/

   OpenStringSource("assert_str",str,0);

   assertArgs = GetRHSPattern("assert_str",&theToken,
                              &error,FALSE,TRUE,
                              TRUE,RPAREN);

   CloseStringSource("assert_str");

#if CERTAINTY_FACTORS 
   /* GetRHSPattern called above may have left a token
      in the lookahead Token (theUnToken)  -- see GetRHSPattern and
          Scanner.c -- clear it since we are closing the string source
          and it should not be read when next token requested
          NOTE: this may not be needed now that am not unGetting STOP tokens?
   */
   ClearTheUnToken();
#endif


   /*===========================================*/
   /* Check for errors or the use of variables. */
   /*===========================================*/

   if (error)
     {
      ReturnExpression(assertArgs);
      return(NULL);
     }

   if (ExpressionContainsVariables(assertArgs,FALSE))
     {
      LocalVariableErrorMessage("the assert-string function");
      SetEvaluationError(TRUE);
      ReturnExpression(assertArgs);
      return(NULL);
     }

   /*=======================================================*/
   /* Count the number of fields needed for the fact and    */
   /* create a fact data structure of the appropriate size. */
   /*=======================================================*/

   for (tempPtr = assertArgs->nextArg; tempPtr != NULL; tempPtr = tempPtr->nextArg)
     { numberOfFields++; }

   factPtr = (struct fact *) CreateFactBySize(numberOfFields);
   factPtr->whichDeftemplate = (struct deftemplate *) assertArgs->value;

#if CERTAINTY_FACTORS 
   /* get the CF from the argList of the DEFTEMPLATE_PTR expr struct currently
      pointed at be assertArgs
   */
   if (assertArgs->argList == NULL)
      factPtr->factCF = 1.0;
   else
      {
        EvaluateExpression(assertArgs->argList,&theResult);
                if (theResult.type != FLOAT && theResult.type != INTEGER)
                  {
            cfNonNumberError();
            factPtr->factCF = 1.0;
                  }
                else
                  factPtr->factCF = (theResult.type == FLOAT) ?
                                         ValueToDouble(theResult.value) :
                                         (double)ValueToLong(theResult.value);
          }
#endif

   /*=============================================*/
   /* Copy the fields to the fact data structure. */
   /*=============================================*/

   ExpressionInstall(assertArgs); /* DR0836 */
   whichField = 0;
   for (tempPtr = assertArgs->nextArg; tempPtr != NULL; tempPtr = tempPtr->nextArg)
     {
#if FUZZY_DEFTEMPLATES  /* 03-07-96 */
    /* NOTE: a fuzzy fact should have been parsed to give a single constant arg
                 of type FUZZY_VALUE
        */
#endif
      EvaluateExpression(tempPtr,&theResult);
      factPtr->theProposition.theFields[whichField].type = (short) theResult.type;
      factPtr->theProposition.theFields[whichField].value = theResult.value;
      whichField++;
     }
   ExpressionDeinstall(assertArgs); /* DR0836 */
   ReturnExpression(assertArgs);

   /*==================*/
   /* Return the fact. */
   /*==================*/

   return(factPtr);
  }

#if RUN_TIME || BLOAD_ONLY || BLOAD || BLOAD_AND_BSAVE

/*********************************************************/
/* NoSuchTemplateError: Prints out an error message      */
/* in a BLOAD_ONLY, RUN_TIME or bload active environment */
/* when an implied deftemplate cannot be created for     */
/* an assert                                             */
/*********************************************************/
static void NoSuchTemplateError(
  char *templateName)
  {
   PrintErrorID("FACTRHS",1,FALSE);
   PrintRouter(WERROR,"Template ");
   PrintRouter(WERROR,templateName);
   PrintRouter(WERROR," does not exist for assert.\n");
  }

#endif /* RUN_TIME || BLOAD_ONLY || BLOAD || BLOAD_AND_BSAVE */

#endif /* DEFTEMPLATE_CONSTRUCT */


