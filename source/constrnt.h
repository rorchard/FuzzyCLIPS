/*  $Header: /dist/CVS/fzclips/src/constrnt.h,v 1.3 2001/08/11 21:04:28 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.10  04/13/98            */
   /*                                                     */
   /*                CONSTRAINT HEADER FILE               */
   /*******************************************************/

/*************************************************************/
/* Purpose: Provides functions for creating and removing     */
/*   constraint records, adding them to the contraint hash   */
/*   table, and enabling and disabling static and dynamic    */
/*   constraint checking.                                    */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_constrnt
#define _H_constrnt

struct constraintRecord;

#ifndef _H_evaluatn
#include "evaluatn.h"
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _CONSTRNT_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

#include "setup.h"

struct constraintRecord
  {
   unsigned int anyAllowed : 1;
   unsigned int symbolsAllowed : 1;
   unsigned int stringsAllowed : 1;
   unsigned int floatsAllowed : 1;
   unsigned int integersAllowed : 1;
   unsigned int instanceNamesAllowed : 1;
   unsigned int instanceAddressesAllowed : 1;
   unsigned int externalAddressesAllowed : 1;
   unsigned int factAddressesAllowed : 1;
#if FUZZY_DEFTEMPLATES
   unsigned int fuzzyValuesAllowed : 1;
   unsigned int fuzzyValueRestriction : 1;
#endif
   unsigned int voidAllowed : 1;
   unsigned int anyRestriction : 1;
   unsigned int symbolRestriction : 1;
   unsigned int stringRestriction : 1;
   unsigned int floatRestriction : 1;
   unsigned int integerRestriction : 1;
   unsigned int instanceNameRestriction : 1;
   unsigned int multifieldsAllowed : 1;
   unsigned int singlefieldsAllowed : 1;
#if FUZZY_DEFTEMPLATES
   long bsaveIndex;
#else
   unsigned short bsaveIndex;
#endif
   struct expr *restrictionList;
   struct expr *minValue;
   struct expr *maxValue;
   struct expr *minFields;
   struct expr *maxFields;
   struct constraintRecord *multifield;
   struct constraintRecord *next;
   int bucket;
   int count;
  };

typedef struct constraintRecord CONSTRAINT_RECORD;

#define SIZE_CONSTRAINT_HASH  167

   LOCALE void                           InitializeConstraints(void);
   LOCALE int                            GDCCommand(void);
   LOCALE int                            SDCCommand(void);
   LOCALE int                            GSCCommand(void);
   LOCALE int                            SSCCommand(void);
   LOCALE BOOLEAN                        SetDynamicConstraintChecking(int);
   LOCALE BOOLEAN                        GetDynamicConstraintChecking(void);
   LOCALE BOOLEAN                        SetStaticConstraintChecking(int);
   LOCALE BOOLEAN                        GetStaticConstraintChecking(void);
#if (! BLOAD_ONLY) && (! RUN_TIME)
   LOCALE int                            HashConstraint(struct constraintRecord *);
   LOCALE struct constraintRecord       *AddConstraint(struct constraintRecord *);
#endif
#if (! RUN_TIME)
   LOCALE void                           RemoveConstraint(struct constraintRecord *);
#endif

#ifndef _CONSTRNT_SOURCE_
   extern struct constraintRecord   **ConstraintHashtable;
#endif

#endif




