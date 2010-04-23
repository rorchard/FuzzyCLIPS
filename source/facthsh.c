static char rcsid[] = "$Header: /dist/CVS/fzclips/src/facthsh.c,v 1.3 2001/08/11 21:05:35 dave Exp $" ;

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*                 FACT HASHING MODULE                 */
   /*******************************************************/

/*************************************************************/
/* Purpose: Provides routines for maintaining a fact hash    */
/*   table so that duplication of facts can quickly be       */
/*   determined.                                             */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*      Bob Orchard (NRCC - Nat'l Research Council of Canada)*/
/*                  (Fuzzy reasoning extensions)             */
/*                  (certainty factors for facts and rules)  */
/*                  (extensions to run command)              */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#define _FACTHSH_SOURCE_

#include <stdio.h>
#define _STDIO_INCLUDED_
#include <stdlib.h>

#include "setup.h"

#if DEFTEMPLATE_CONSTRUCT

#if FUZZY_DEFTEMPLATES    
#include "symbol.h"
#include "fuzzyutl.h"
#endif

#include "constant.h"
#include "memalloc.h"
#include "router.h"

#if DEFRULE_CONSTRUCT
#include "lgcldpnd.h"
#endif

#include "facthsh.h"

#if CERTAINTY_FACTORS    
#include "cfdef.h"
#endif


/***************************************/
/* LOCAL INTERNAL FUNCTION DEFINITIONS */
/***************************************/

   static struct fact            *FactExists(struct fact *,int);

/***************************************/
/* LOCAL INTERNAL VARIABLE DEFINITIONS */
/***************************************/

   static struct factHashEntry  **FactHashTable;
   static BOOLEAN                 FactDuplication = FALSE;

/************************************************/
/* HashFact: Returns the hash value for a fact. */
/************************************************/
int HashFact(
  struct fact *theFact)
  {
   int count = 0;
   int hashValue;

   /*============================================*/
   /* Get a hash value for the deftemplate name. */
   /*============================================*/

   count += HashSymbol(ValueToString(theFact->whichDeftemplate->header.name),
                       SIZE_FACT_HASH);

   /*=================================================*/
   /* Add in the hash value for the rest of the fact. */
   /*=================================================*/

   count += (int) HashMultifield(&theFact->theProposition,SIZE_FACT_HASH);

   /*================================*/
   /* Make sure the hash value falls */
   /* in the appropriate range.      */
   /*================================*/

   hashValue = (int) (count % SIZE_FACT_HASH);
   if (hashValue < 0) hashValue = - hashValue;

   /*========================*/
   /* Return the hash value. */
   /*========================*/

   return(hashValue);
  }

/**********************************************/
/* FactExists: Determines if a specified fact */
/*   already exists in the fact hash table.   */
/**********************************************/
static struct fact *FactExists(
  struct fact *theFact,
  int hashValue)
  {
   struct factHashEntry *theFactHash;

   for (theFactHash = FactHashTable[hashValue];
        theFactHash != NULL;
        theFactHash = theFactHash->next)
     {
      if ((theFact->whichDeftemplate == theFactHash->theFact->whichDeftemplate) ?
          MultifieldsEqual(&theFact->theProposition,
                           &theFactHash->theFact->theProposition) : FALSE)
#if CERTAINTY_FACTORS 
          /* standard facts -- same templates means same fact --
             perform global contribution calculation for CF and return
             ptr to existing fact indicating fact exists
          */
        { changeCFofExistingFact(theFact,theFactHash->theFact);
          return(theFactHash->theFact);
        }
#else
        { return(theFactHash->theFact); }
#endif
     }

   return(NULL);
  }

/************************************************************/
/* AddHashedFact: Adds a fact entry to the fact hash table. */
/************************************************************/
globle void AddHashedFact(
  struct fact *theFact,
  int hashValue)
  {
   struct factHashEntry *newhash, *temp;

   newhash = get_struct(factHashEntry);
   newhash->theFact = theFact;

   temp = FactHashTable[hashValue];
   FactHashTable[hashValue] = newhash;
   newhash->next = temp;
  }

/******************************************/
/* RemoveHashedFact: Removes a fact entry */
/*   from the fact hash table.            */
/******************************************/
globle BOOLEAN RemoveHashedFact(
  struct fact *theFact)
  {
   int hashValue;
   struct factHashEntry *hptr, *prev;

   hashValue = HashFact(theFact);

   for (hptr = FactHashTable[hashValue], prev = NULL;
        hptr != NULL;
        hptr = hptr->next)
     {
      if (hptr->theFact == theFact)
        {
         if (prev == NULL)
           {
            FactHashTable[hashValue] = hptr->next;
            rtn_struct(factHashEntry,hptr);
            return(1);
           }
         else
           {
            prev->next = hptr->next;
            rtn_struct(factHashEntry,hptr);
            return(1);
           }
        }
      prev = hptr;
     }

   return(0);
  }


#if FUZZY_DEFTEMPLATES
/*******************************************************/
/* HandleExistingFuzzyFact: Determines if new fact to  */
/*   be added to the fact-list is existing fuzzy fact  */
/*   and if so modifies it accordingly.  Will also     */
/*   retract the existing fact if it finds one         */
/*                                                     */
/*   Returns hashvalue for the NEW fact                */
/*                                                     */
/* NOTE: a fact with fuzzy slots exists already if the */
/*       non-fuzzy slots are identical in value and    */
/*       the fuzzy slots all have the same FUZZY_VALUE */
/*       type -- i.e. both temperature deftemplates    */
/*******************************************************/
globle int HandleExistingFuzzyFact(theFact)
  VOID **theFact;
  {
   struct fact *tempFact;
   struct factHashEntry *theFactHash;
   int hashValue;
   struct fact *theFactPtr = (struct fact *)*theFact;

   hashValue = HashFact((VOID *)theFactPtr);

   /* Fuzzy facts never get duplicated ... they just get modified if they
      already exist ... always allow duplication for them...
      do the required modification to the fact if it already exists
   */

   theFactHash = FactHashTable[hashValue];
   tempFact = NULL;

   while (theFactHash != NULL)
     {
      if (theFactPtr->whichDeftemplate == theFactHash->theFact->whichDeftemplate)
        { /* same template definitions for fact */
          if (MultifieldsEqual(&theFactPtr->theProposition,
                               &theFactHash->theFact->theProposition))
            { /* MultiFieldsEqual will compare fuzzy fields to be equal
                 if they are both of the same FUZZY_VALUE type
              */
              tempFact = theFactHash->theFact;
#if CERTAINTY_FACTORS
              /* If facts are the same we need to perform
                 global contribution calculation for CF
              */
              changeCFofNewVsExistingFact(theFactPtr, tempFact);
#endif

              /* fuzzy facts (ie. any fuzzy slots) perform global
                 contribution calculation(s) for each fuzzy slot
              */
              changeValueOfFuzzySlots(tempFact, theFactPtr);
                          break;
            }
        }
      theFactHash = theFactHash->next;
     }

   if (tempFact != NULL) /* existing fact! retract it before new one gets asserted */
       Retract(tempFact);

   return(hashValue);
  }

#endif

/*****************************************************/
/* HandleFactDuplication: Determines if a fact to be */
/*   added to the fact-list is a duplicate entry and */
/*   takes appropriate action based on the current   */
/*   setting of the fact-duplication flag.           */
/*****************************************************/
globle int HandleFactDuplication(
  void *theFact)
  {
   struct fact *tempPtr;
   int hashValue;

   hashValue = HashFact((struct fact *) theFact);

   if (FactDuplication) return(hashValue);

   tempPtr = FactExists((struct fact *) theFact,hashValue);
   if (tempPtr == NULL) return(hashValue);

   ReturnFact((struct fact *) theFact);
#if LOGICAL_DEPENDENCIES && DEFRULE_CONSTRUCT
   AddLogicalDependencies((struct patternEntity *) tempPtr,TRUE);
#endif
   return(-1);
  }

/********************************************/
/* GetFactDuplication: C access routine for */
/*   the get-fact-duplication command.      */
/********************************************/
globle BOOLEAN GetFactDuplication()
  { return(FactDuplication); }

/********************************************/
/* SetFactDuplication: C access routine for */
/*   the set-fact-duplication command.      */
/********************************************/
globle BOOLEAN SetFactDuplication(
  int value)
  {
   int ov;

   ov = FactDuplication;
   FactDuplication = value;
   return(ov);
  }

/**************************************************/
/* InitializeFactHashTable: Initializes the table */
/*   entries in the fact hash table to NULL.      */
/**************************************************/
globle void InitializeFactHashTable()
   {
    int i;

    FactHashTable = (struct factHashEntry **)
                    gm2((int) sizeof (struct factHashEntry *) * SIZE_FACT_HASH);

    if (FactHashTable == NULL) ExitRouter(EXIT_FAILURE);

    for (i = 0; i < SIZE_FACT_HASH; i++) FactHashTable[i] = NULL;
   }

#if DEVELOPER

/*****************************************************/
/* ShowFactHashTable: Displays the number of entries */
/*   in each slot of the fact hash table.            */
/*****************************************************/
globle void ShowFactHashTable()
   {
    int i, count;
    struct factHashEntry *theEntry;
    char buffer[20];

    for (i = 0; i < SIZE_FACT_HASH; i++)
      {
       for (theEntry =  FactHashTable[i], count = 0;
            theEntry != NULL;
            theEntry = theEntry->next)
         { count++; }

       if (count != 0)
         {
          sprintf(buffer,"%4d: %4d\n",i,count);
          PrintRouter(WDISPLAY,buffer);
         }
      }
   }

#endif /* DEVELOPER */

#endif /* DEFTEMPLATE_CONSTRUCT */

