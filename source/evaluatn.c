static char rcsid[] = "$Header: /dist/CVS/fzclips/src/evaluatn.c,v 1.3 2001/08/11 21:05:19 dave Exp $" ;

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*                  EVALUATION MODULE                  */
   /*******************************************************/

/*************************************************************/
/* Purpose: Provides routines for evaluating expressions.    */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*      Brian L. Donnell                                     */
/*      Bob Orchard (NRCC - Nat'l Research Council of Canada)*/
/*                  (Fuzzy reasoning extensions)             */
/*                  (certainty factors for facts and rules)  */
/*                  (extensions to run command)              */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#define _EVALUATN_SOURCE_

#include <stdio.h>
#define _STDIO_INCLUDED_
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "setup.h"

#if FUZZY_DEFTEMPLATES 
#include "symbol.h"
#include "fuzzyrhs.h"
#include "fuzzypsr.h"
#endif


#include "commline.h"
#include "constant.h"
#include "memalloc.h"
#include "router.h"
#include "extnfunc.h"
#include "prcdrfun.h"
#include "multifld.h"
#include "factmngr.h"
#include "prntutil.h"
#include "exprnpsr.h"
#include "utility.h"
#include "proflfun.h"
#include "sysdep.h"

#if DEFFUNCTION_CONSTRUCT
#include "dffnxfun.h"
#endif

#if DEFGENERIC_CONSTRUCT
#include "genrccom.h"
#endif

#if OBJECT_SYSTEM
#include "object.h"
#endif

#include "evaluatn.h"

/***************************************/
/* LOCAL INTERNAL FUNCTION DEFINITIONS */
/***************************************/

   static void                    PropagateReturnAtom(int,void *);

/****************************************/
/* GLOBAL INTERNAL VARIABLE DEFINITIONS */
/****************************************/

   globle struct expr          *CurrentExpression = NULL;
   globle int                   EvaluationError = FALSE;
   globle int                   HaltExecution = FALSE;
   globle int                   CurrentEvaluationDepth = 0;
   globle struct entityRecord  *PrimitivesArray[MAXIMUM_PRIMITIVES];

/**************************************************************/
/* EvaluateExpression: Evaluates an expression. Returns FALSE */
/*   if no errors occurred during evaluation, otherwise TRUE. */
/**************************************************************/
globle int EvaluateExpression(
  struct expr *problem,
  DATA_OBJECT_PTR returnValue)
  {
   struct expr *oldArgument;
   struct FunctionDefinition *fptr;
#if PROFILING_FUNCTIONS
   struct profileFrameInfo profileFrame;
#endif

   if (problem == NULL)
     {
      returnValue->type = SYMBOL;
      returnValue->value = FalseSymbol;
      return(EvaluationError);
     }

   switch (problem->type)
     {
      case STRING:
      case SYMBOL:
      case FLOAT:
      case INTEGER:
#if OBJECT_SYSTEM
      case INSTANCE_NAME:
      case INSTANCE_ADDRESS:
#endif
#if FUZZY_DEFTEMPLATES 
      case FUZZY_VALUE:
#endif
      case EXTERNAL_ADDRESS:
        returnValue->type = problem->type;
        returnValue->value = problem->value;
        break;

#if FUZZY_DEFTEMPLATES 
      case S_FUNCTION:
      case PI_FUNCTION:
      case Z_FUNCTION:
      case SINGLETON_EXPRESSION:
             /* At some time it may be worthwhile making this into an FCALL
                    but only when we allow user's to create functions that return
                        fuzzy values -- this may not happen
             */
                {
                  struct fuzzy_value *fvptr;
                  fvptr = getConstantFuzzyValue(problem, &EvaluationError);
          returnValue->type = FUZZY_VALUE;
                  if (fvptr != NULL)
                    {
              returnValue->value = (VOID *)AddFuzzyValue(fvptr);
                      /* AddFuzzyValue makes a copy of the fuzzy value -- so remove this one */
              rtnFuzzyValue(fvptr);
                        }
              else
                  {
                   returnValue->type = RVOID;
                   returnValue->value = CLIPSFalseSymbol;
                   SetEvaluationError(TRUE);
                  }
        }
        break;
#endif

      case FCALL:
        {
         fptr = (struct FunctionDefinition *) problem->value;

#if PROFILING_FUNCTIONS   
         StartProfile(&profileFrame,
                      &fptr->usrData,
                      ProfileUserFunctions);
#endif

         oldArgument = CurrentExpression;
         CurrentExpression = problem;

         switch(fptr->returnValueType)
           {
            case 'v' :
              (* (void (*)(void)) fptr->functionPointer)();
              returnValue->type = RVOID;
              returnValue->value = FalseSymbol;
              break;
            case 'b' :
              returnValue->type = SYMBOL;
              if ((* (int (*)(void)) fptr->functionPointer)())
                returnValue->value = TrueSymbol;
              else
                returnValue->value = FalseSymbol;
              break;
            case 'a' :
              returnValue->type = EXTERNAL_ADDRESS;
              returnValue->value =
                             (* (void *(*)(void)) fptr->functionPointer)();
              break;
            case 'i' :
              returnValue->type = INTEGER;
              returnValue->value = (void *)
                AddLong((long) (* (int (*)(void)) fptr->functionPointer)());
              break;
            case 'l' :
              returnValue->type = INTEGER;
              returnValue->value = (void *)
                 AddLong((* (long int (*)(void)) fptr->functionPointer)());
              break;
#if FUZZY_DEFTEMPLATES 
            case 'F' :
              {
                struct fuzzy_value *fvPtr;

                fvPtr = (* (struct fuzzy_value * (*)(VOID_ARG)) fptr->functionPointer)();
                if (fvPtr != NULL)
                  {
                   returnValue->type = FUZZY_VALUE;
                   returnValue->value = (VOID *)AddFuzzyValue( fvPtr );
                   /* AddFuzzyValue makes a copy of fv .. so return it */
                   rtnFuzzyValue( fvPtr );
                  }
                else
                  {
                   returnValue->type = RVOID;
                   returnValue->value = CLIPSFalseSymbol;
                  }
               }
              break;
#endif
            case 'f' :
              returnValue->type = FLOAT;
              returnValue->value = (void *)
                 AddDouble((double) (* (float (*)(void)) fptr->functionPointer)());
              break;
            case 'd' :
              returnValue->type = FLOAT;
              returnValue->value = (void *)
                 AddDouble((* (double (*)(void)) fptr->functionPointer)());
              break;
            case 's' :
              returnValue->type = STRING;
              returnValue->value = (void *)
                (* (SYMBOL_HN *(*)(void)) fptr->functionPointer)();
              break;
            case 'w' :
              returnValue->type = SYMBOL;
              returnValue->value = (void *)
                (* (SYMBOL_HN *(*)(void)) fptr->functionPointer)();
              break;
#if OBJECT_SYSTEM
            case 'x' :
              returnValue->type = INSTANCE_ADDRESS;
              returnValue->value =
                             (* (void *(*)(void)) fptr->functionPointer)();
              break;
            case 'o' :
              returnValue->type = INSTANCE_NAME;
              returnValue->value = (void *)
                (* (SYMBOL_HN *(*)(void)) fptr->functionPointer)();
              break;
#endif
            case 'c' :
              {
               char cbuff[2];

               cbuff[0] = (* (char (*)(void)) fptr->functionPointer)();
               cbuff[1] = EOS;
               returnValue->type = SYMBOL;
               returnValue->value = (void *) AddSymbol(cbuff);
               break;
              }

            case 'j' :
            case 'k' :
            case 'm' :
            case 'n' :
            case 'u' :
              (* (void (*)(DATA_OBJECT_PTR)) fptr->functionPointer)(returnValue);
              break;

            default :
               SystemError("EVALUATN",2);
               ExitRouter(EXIT_FAILURE);
               break;
            }

#if PROFILING_FUNCTIONS 
        EndProfile(&profileFrame);
#endif

        CurrentExpression = oldArgument;
        break;
        }

     case MULTIFIELD:
        returnValue->type = MULTIFIELD;
        returnValue->value = ((DATA_OBJECT_PTR) (problem->value))->value;
        returnValue->begin = ((DATA_OBJECT_PTR) (problem->value))->begin;
        returnValue->end = ((DATA_OBJECT_PTR) (problem->value))->end;
        break;

     case MF_VARIABLE:
     case SF_VARIABLE:
        if (GetBoundVariable(returnValue,(SYMBOL_HN *) problem->value) == FALSE)
          {
           PrintErrorID("EVALUATN",1,FALSE);
           PrintRouter(WERROR,"Variable ");
           PrintRouter(WERROR,ValueToString(problem->value));
           PrintRouter(WERROR," is unbound\n");
           returnValue->type = SYMBOL;
           returnValue->value = FalseSymbol;
           SetEvaluationError(TRUE);
          }
        break;

      default:
        if (PrimitivesArray[problem->type] == NULL)
          {
           SystemError("EVALUATN",3);
           ExitRouter(EXIT_FAILURE);
          }

        if (PrimitivesArray[problem->type]->copyToEvaluate)
          {
           returnValue->type = problem->type;
           returnValue->value = problem->value;
           break;
          }

        if (PrimitivesArray[problem->type]->evaluateFunction == NULL)
          {
           SystemError("EVALUATN",4);
           ExitRouter(EXIT_FAILURE);
          }

        oldArgument = CurrentExpression;
        CurrentExpression = problem;

#if PROFILING_FUNCTIONS 
        StartProfile(&profileFrame,
                     &PrimitivesArray[problem->type]->usrData,
                     ProfileUserFunctions);
#endif

        (*PrimitivesArray[problem->type]->evaluateFunction)(problem->value,returnValue);

#if PROFILING_FUNCTIONS
        EndProfile(&profileFrame);
#endif

        CurrentExpression = oldArgument;
        break;
     }

   PropagateReturnValue(returnValue);
   return(EvaluationError);
  }

/******************************************/
/* InstallPrimitive: Installs a primitive */
/*   data type in the primitives array.   */
/******************************************/
globle void InstallPrimitive(
  struct entityRecord *thePrimitive,
  int whichPosition)
  {
   if (PrimitivesArray[whichPosition] != NULL)
     {
      SystemError("EVALUATN",5);
      ExitRouter(EXIT_FAILURE);
     }

   PrimitivesArray[whichPosition] = thePrimitive;
  }

/******************************************************/
/* SetEvaluationError: Sets the EvaluationError flag. */
/******************************************************/
globle void SetEvaluationError(
  int value)
  {
   EvaluationError = value;
   if (value == TRUE) HaltExecution = TRUE;
  }

/*********************************************************/
/* GetEvaluationError: Returns the EvaluationError flag. */
/*********************************************************/
globle int GetEvaluationError()
  {
   return(EvaluationError);
  }

/**************************************************/
/* SetHaltExecution: Sets the HaltExecution flag. */
/**************************************************/
globle void SetHaltExecution(
  int value)
  { HaltExecution = value; }

/*****************************************************/
/* GetHaltExecution: Returns the HaltExecution flag. */
/*****************************************************/
globle int GetHaltExecution()
  {
   return(HaltExecution);
  }

/******************************************************/
/* ReturnValues: Returns a linked list of DATA_OBJECT */
/*   structures to the pool of free memory.           */
/******************************************************/
globle void ReturnValues(
  DATA_OBJECT_PTR garbagePtr)
  {
   DATA_OBJECT_PTR nextPtr;

   while (garbagePtr != NULL)
     {
      nextPtr = garbagePtr->next;
      ValueDeinstall(garbagePtr);
      rtn_struct(dataObject,garbagePtr);
      garbagePtr = nextPtr;
     }
  }

/***************************************************/
/* PrintDataObject: Prints a DATA_OBJECT structure */
/*   to the specified logical name.                */
/***************************************************/
globle void PrintDataObject(
  char *fileid,
  DATA_OBJECT_PTR argPtr)
  {
   switch(argPtr->type)
     {
      case RVOID:
      case SYMBOL:
      case STRING:
      case INTEGER:
      case FLOAT:
      case EXTERNAL_ADDRESS:
      case FACT_ADDRESS:
#if OBJECT_SYSTEM
      case INSTANCE_NAME:
      case INSTANCE_ADDRESS:
#endif
#if FUZZY_DEFTEMPLATES 
     case FUZZY_VALUE:
#endif
        PrintAtom(fileid,argPtr->type,argPtr->value);
        break;
      case MULTIFIELD:
        PrintMultifield(fileid,(struct multifield *) argPtr->value,
                        argPtr->begin,argPtr->end,TRUE);
        break;

      default:
        if (PrimitivesArray[argPtr->type] != NULL)
          {
           if (PrimitivesArray[argPtr->type]->longPrintFunction)
             {
              (*PrimitivesArray[argPtr->type]->longPrintFunction)(fileid,argPtr->value);
              break;
             }
           else if (PrimitivesArray[argPtr->type]->shortPrintFunction)
             {
              (*PrimitivesArray[argPtr->type]->shortPrintFunction)(fileid,argPtr->value);
              break;
             }
          }

        PrintRouter(fileid,"<UnknownPrintType");
        PrintLongInteger(fileid,(long int) argPtr->type);
        PrintRouter(fileid,">");
        SetHaltExecution(TRUE);
        SetEvaluationError(TRUE);
        break;
     }
  }

/*************************************************/
/* SetMultifieldErrorValue: Creates a multifield */
/*   value of length zero for error returns.     */
/*************************************************/
globle void SetMultifieldErrorValue(
  DATA_OBJECT_PTR returnValue)
  {
   returnValue->type = MULTIFIELD;
   returnValue->value = CreateMultifield(0L);
   returnValue->begin = 1;
   returnValue->end = 0;
  }

/**************************************************/
/* ValueInstall: Increments the appropriate count */
/*   (in use) values for a DATA_OBJECT structure. */
/**************************************************/
globle void ValueInstall(
  DATA_OBJECT *vPtr)
  {
   if (vPtr->type == MULTIFIELD) MultifieldInstall((struct multifield *) vPtr->value);
   else AtomInstall(vPtr->type,vPtr->value);
  }

/****************************************************/
/* ValueDeinstall: Decrements the appropriate count */
/*   (in use) values for a DATA_OBJECT structure.   */
/****************************************************/
globle void ValueDeinstall(
  DATA_OBJECT *vPtr)
  {
   if (vPtr->type == MULTIFIELD) MultifieldDeinstall((struct multifield *) vPtr->value);
   else AtomDeinstall(vPtr->type,vPtr->value);
  }

/*****************************************/
/* AtomInstall: Increments the reference */
/*   count of an atomic data type.       */
/*****************************************/
globle void AtomInstall(
  int type,
  void *vPtr)
  {
   switch (type)
     {
      case SYMBOL:
      case STRING:
#if DEFGLOBAL_CONSTRUCT
      case GBL_VARIABLE:
#endif
#if OBJECT_SYSTEM
      case INSTANCE_NAME:
#endif
        IncrementSymbolCount(vPtr);
        break;

      case FLOAT:
        IncrementFloatCount(vPtr);
        break;

      case INTEGER:
        IncrementIntegerCount(vPtr);
        break;

      case MULTIFIELD:
        MultifieldInstall((struct multifield *) vPtr);
        break;

#if FUZZY_DEFTEMPLATES
      /* fuzzy values have a name which is a symbol */
      case FUZZY_VALUE:
            InstallFuzzyValue(vPtr);
        break;
#endif

      case RVOID:
        break;

      default:
        if (PrimitivesArray[type] == NULL) break;
        if (PrimitivesArray[type]->bitMap) IncrementBitMapCount(vPtr);
        else if (PrimitivesArray[type]->incrementBusyCount)
          { (*PrimitivesArray[type]->incrementBusyCount)(vPtr); }
        break;
     }
  }

/*******************************************/
/* AtomDeinstall: Decrements the reference */
/*   count of an atomic data type.         */
/*******************************************/
globle void AtomDeinstall(
  int type,
  void *vPtr)
  {
   switch (type)
     {
      case SYMBOL:
      case STRING:
#if DEFGLOBAL_CONSTRUCT
      case GBL_VARIABLE:
#endif
#if OBJECT_SYSTEM
      case INSTANCE_NAME:
#endif
        DecrementSymbolCount((SYMBOL_HN *) vPtr);
        break;

      case FLOAT:
        DecrementFloatCount((FLOAT_HN *) vPtr);
        break;

      case INTEGER:
        DecrementIntegerCount((INTEGER_HN *) vPtr);
        break;

#if FUZZY_DEFTEMPLATES
      case FUZZY_VALUE:
            DeinstallFuzzyValue(vPtr);
        break;
#endif

      case MULTIFIELD:
        MultifieldDeinstall((struct multifield *) vPtr);
        break;

      case RVOID:
        break;

      default:
        if (PrimitivesArray[type] == NULL) break;
        if (PrimitivesArray[type]->bitMap) DecrementBitMapCount((BITMAP_HN *) vPtr);
        else if (PrimitivesArray[type]->decrementBusyCount)
          { (*PrimitivesArray[type]->decrementBusyCount)(vPtr); }
     }
  }

/*********************************************************************/
/* PropagateReturnValue: Decrements the associated depth for a value */
/*   stored in a DATA_OBJECT structure. In effect, the values        */
/*   returned by certain evaluations (such as a deffunction call)    */
/*   are passed up to the previous depth of evaluation. The return   */
/*   value's depth is decremented so that it will not be garbage     */
/*   collected along with other items that are no longer needed from */
/*   the evaluation that generated the return value.                 */
/*********************************************************************/
globle void PropagateReturnValue(
  DATA_OBJECT *vPtr)
  {
   long i; /* 6.04 Bug Fix */
   struct multifield *theSegment;
   struct field *theMultifield;

   if (vPtr->type != MULTIFIELD)
     { PropagateReturnAtom(vPtr->type,vPtr->value); }
   else
     {
      theSegment = (struct multifield *) vPtr->value;

      if (theSegment->depth > CurrentEvaluationDepth)
        theSegment->depth = (short) CurrentEvaluationDepth;

      theMultifield = theSegment->theFields;

      for (i = 0; i < theSegment->multifieldLength; i++)
        { PropagateReturnAtom(theMultifield[i].type,theMultifield[i].value); }
     }
  }

/*****************************************/
/* PropagateReturnAtom: Support function */
/*   for PropagateReturnValue.           */
/*****************************************/
static void PropagateReturnAtom(
  int type,
  void *value)
  {
   switch (type)
     {
      case INTEGER         :
      case FLOAT           :
      case SYMBOL          :
      case STRING          :

#if OBJECT_SYSTEM
      case INSTANCE_NAME   :
#endif
        if (((SYMBOL_HN *) value)->depth > CurrentEvaluationDepth)
          { ((SYMBOL_HN *) value)->depth = CurrentEvaluationDepth; }
        break;

#if FUZZY_DEFTEMPLATES
      case FUZZY_VALUE     :
        {
          if (((FUZZY_VALUE_HN *) value)->depth > CurrentEvaluationDepth)
            { ((FUZZY_VALUE_HN *) value)->depth = CurrentEvaluationDepth; }
        }

        break;
#endif

#if OBJECT_SYSTEM
      case INSTANCE_ADDRESS :
        if (((INSTANCE_TYPE *) value)->depth > CurrentEvaluationDepth)
          { ((INSTANCE_TYPE *) value)->depth = CurrentEvaluationDepth; }
        break;
#endif
      case FACT_ADDRESS :
        if (((int) ((struct fact *) value)->depth) > CurrentEvaluationDepth)
          { ((struct fact *) value)->depth = (unsigned) CurrentEvaluationDepth; }
        break;
     }
  }

#if DEFFUNCTION_CONSTRUCT || DEFGENERIC_CONSTRUCT

/********************************************/
/* FunctionCall: Allows Deffunctions and    */
/*   Generic Functions to be called from C. */
/*   Allows only constants as arguments.    */
/********************************************/
globle int FunctionCall(
  char *name,
  char *args,
  DATA_OBJECT *result)
  {
   FUNCTION_REFERENCE theReference;

   /*=======================================*/
   /* Call the function if it can be found. */
   /*=======================================*/

   if (GetFunctionReference(name,&theReference))
     { return(FunctionCall2(&theReference,args,result)); }

   /*=========================================================*/
   /* Otherwise signal an error if a deffunction, defgeneric, */
   /* or user defined function doesn't exist that matches     */
   /* the specified function name.                            */
   /*=========================================================*/

   PrintErrorID("EVALUATN",2,FALSE);
   PrintRouter(WERROR,"No function, generic function or deffunction of name ");
   PrintRouter(WERROR,name);
   PrintRouter(WERROR," exists for external call.\n");
   return(TRUE);
  }

/********************************************/
/* FunctionCall2: Allows Deffunctions and    */
/*   Generic Functions to be called from C. */
/*   Allows only constants as arguments.    */
/********************************************/
globle int FunctionCall2(
  FUNCTION_REFERENCE *theReference,
  char *args,
  DATA_OBJECT *result)
  {
   EXPRESSION *argexps;
   int error = FALSE;

   /*=============================================*/
   /* Force periodic cleanup if the function call */
   /* was executed from an embedded application.  */
   /*=============================================*/

   if ((CurrentEvaluationDepth == 0) && (! EvaluatingTopLevelCommand) &&
       (CurrentExpression == NULL))
     { PeriodicCleanup(TRUE,FALSE); }

   /*========================*/
   /* Reset the error state. */
   /*========================*/

   if (CurrentEvaluationDepth == 0) SetHaltExecution(FALSE);
   EvaluationError = FALSE;

   /*======================================*/
   /* Initialize the default return value. */
   /*======================================*/

   result->type = SYMBOL;
   result->value = FalseSymbol;

   /*============================*/
   /* Parse the argument string. */
   /*============================*/

   argexps = ParseConstantArguments(args,&error);
   if (error == TRUE) return(TRUE);

   /*====================*/
   /* Call the function. */
   /*====================*/

   theReference->argList = argexps;
   error = EvaluateExpression(theReference,result);

   /*========================*/
   /* Return the expression. */
   /*========================*/

   ReturnExpression(argexps);
   theReference->argList = NULL;

   /*==========================*/
   /* Return the error status. */
   /*==========================*/

   return(error);
  }

#endif

/***************************************************/
/* CopyDataObject: Copies the values from a source */
/*   DATA_OBJECT to a destination DATA_OBJECT.     */
/***************************************************/
globle void CopyDataObject(
  DATA_OBJECT *dst,
  DATA_OBJECT *src,
  int garbageMultifield)
  {
   if (src->type != MULTIFIELD)
     {
      dst->type = src->type;
      dst->value = src->value;
     }
   else
     {
      DuplicateMultifield(dst,src);
      if (garbageMultifield)
        { AddToMultifieldList((struct multifield *) dst->value); }
     }
  }

/***********************************************/
/* TransferDataObjectValues: Copies the values */
/*   directly from a source DATA_OBJECT to a   */
/*   destination DATA_OBJECT.                  */
/***********************************************/
globle void TransferDataObjectValues(
  DATA_OBJECT *dst,
  DATA_OBJECT *src)
  {
   dst->type = src->type;
   dst->value = src->value;
   dst->begin = src->begin;
   dst->end = src->end;
   dst->supplementalInfo = src->supplementalInfo;
   dst->next = src->next;
  }

/************************************************************************/
/* ConvertValueToExpression: Converts the value stored in a data object */
/*   into an expression. For multifield values, a chain of expressions  */
/*   is generated and the chain is linked by the nextArg field. For a   */
/*   single field value, a single expression is created.                */
/************************************************************************/
globle struct expr *ConvertValueToExpression(
  DATA_OBJECT *theValue)
  {
   long i;
   struct expr *head = NULL, *last = NULL, *newItem;

   if (GetpType(theValue) != MULTIFIELD)
     { return(GenConstant(GetpType(theValue),GetpValue(theValue))); }

   for (i = GetpDOBegin(theValue); i <= GetpDOEnd(theValue); i++)
     {
      newItem = GenConstant(GetMFType(GetpValue(theValue),i),
                        GetMFValue(GetpValue(theValue),i));
      if (last == NULL) head = newItem;
      else last->nextArg = newItem;
      last = newItem;
     }

   if (head == NULL)
     return(GenConstant(FCALL,(void *) FindFunction("create$")));

   return(head);
  }

/****************************************/
/* GetAtomicHashValue: Returns the hash */
/*   value for an atomic data type.     */
/****************************************/
unsigned int GetAtomicHashValue(
  int type,
  void *value,
  int position)
  {
   unsigned int tvalue;
   union
     {
      double fv;
      unsigned int liv;
     } fis;

   switch (type)
     {
      case FLOAT:
        fis.fv = ValueToDouble(value);
        tvalue = fis.liv;
        break;

      case INTEGER:
        tvalue = (unsigned int) ValueToLong(value);
        break;

      case FACT_ADDRESS:
#if OBJECT_SYSTEM
      case INSTANCE_ADDRESS:
#endif
      case EXTERNAL_ADDRESS:
         tvalue = (unsigned int) value;
         break;

      case STRING:
#if OBJECT_SYSTEM
      case INSTANCE_NAME:
#endif
      case SYMBOL:
        tvalue = ((SYMBOL_HN *) value)->bucket;
        break;

      default:
        tvalue = type;
     }

   if (position < 0) return(tvalue);

   return((unsigned int) (tvalue * (position + 29)));
  }

/***********************************************************/
/* FunctionReferenceExpression: Returns an expression with */
/*   an appropriate expression reference to the specified  */
/*   name if it is the name of a deffunction, defgeneric,  */
/*   or user/system defined function.                      */
/***********************************************************/
globle struct expr *FunctionReferenceExpression(
  char *name)
  {
#if DEFGENERIC_CONSTRUCT
   void *gfunc;
#endif
#if DEFFUNCTION_CONSTRUCT
   void *dptr;
#endif
   struct FunctionDefinition *fptr;

   /*=====================================================*/
   /* Check to see if the function call is a deffunction. */
   /*=====================================================*/

#if DEFFUNCTION_CONSTRUCT
   if ((dptr = (void *) LookupDeffunctionInScope(name)) != NULL)
     { return(GenConstant(PCALL,dptr)); }
#endif

   /*====================================================*/
   /* Check to see if the function call is a defgeneric. */
   /*====================================================*/

#if DEFGENERIC_CONSTRUCT
   if ((gfunc = (void *) LookupDefgenericInScope(name)) != NULL)
     { return(GenConstant(GCALL,gfunc)); }
#endif

   /*======================================*/
   /* Check to see if the function call is */
   /* a system or user defined function.   */
   /*======================================*/

   if ((fptr = FindFunction(name)) != NULL)
     { return(GenConstant(FCALL,fptr)); }

   /*===================================================*/
   /* The specified function name is not a deffunction, */
   /* defgeneric, or user/system defined function.      */
   /*===================================================*/

   return(NULL);
  }

/******************************************************************/
/* GetFunctionReference: Fills an expression with an appropriate  */
/*   expression reference to the specified name if it is the      */
/*   name of a deffunction, defgeneric, or user/system defined    */
/*   function.                                                    */
/******************************************************************/
globle BOOLEAN GetFunctionReference(
  char *name,
  FUNCTION_REFERENCE *theReference)
  {
#if DEFGENERIC_CONSTRUCT
   void *gfunc;
#endif
#if DEFFUNCTION_CONSTRUCT
   void *dptr;
#endif
   struct FunctionDefinition *fptr;

   theReference->nextArg = NULL;
   theReference->argList = NULL;
   theReference->type = RVOID;
   theReference->value = NULL;

   /*=====================================================*/
   /* Check to see if the function call is a deffunction. */
   /*=====================================================*/

#if DEFFUNCTION_CONSTRUCT
   if ((dptr = (void *) LookupDeffunctionInScope(name)) != NULL)
     {
      theReference->type = PCALL;
      theReference->value = dptr;
      return(TRUE);
     }
#endif

   /*====================================================*/
   /* Check to see if the function call is a defgeneric. */
   /*====================================================*/

#if DEFGENERIC_CONSTRUCT
   if ((gfunc = (void *) LookupDefgenericInScope(name)) != NULL)
     {
      theReference->type = GCALL;
      theReference->value = gfunc;
      return(TRUE);
     }
#endif

   /*======================================*/
   /* Check to see if the function call is */
   /* a system or user defined function.   */
   /*======================================*/

   if ((fptr = FindFunction(name)) != NULL)
     {
      theReference->type = FCALL;
      theReference->value = fptr;
      return(TRUE);
     }

   /*===================================================*/
   /* The specified function name is not a deffunction, */
   /* defgeneric, or user/system defined function.      */
   /*===================================================*/

   return(FALSE);
  }

/*******************************************************/
/* DOsEqual: Determines if two DATA_OBJECTS are equal. */
/*******************************************************/
globle BOOLEAN DOsEqual(
  DATA_OBJECT_PTR dobj1,
  DATA_OBJECT_PTR dobj2)
  {
   if (GetpType(dobj1) != GetpType(dobj2))
     { return(FALSE); }

   if (GetpType(dobj1) == MULTIFIELD)
     {
      if (MultifieldDOsEqual(dobj1,dobj2) == FALSE)
        { return(FALSE); }
     }
   else if (GetpValue(dobj1) != GetpValue(dobj2))
     { return(FALSE); }

   return(TRUE);
  }


