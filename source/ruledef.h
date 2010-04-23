/*  $Header: /dist/CVS/fzclips/src/ruledef.h,v 1.3 2001/08/11 21:07:47 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*                 DEFRULE HEADER FILE                 */
   /*******************************************************/

/*************************************************************/
/* Purpose: Defines basic defrule primitive functions such   */
/*   as allocating and deallocating, traversing, and finding */
/*   defrule data structures.                                */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*      Brian L. Donnell                                     */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_ruledef
#define _H_ruledef

#define GetDisjunctIndex(r) ((struct constructHeader *) r)->bsaveID

struct defrule;
struct defruleModule;

#ifndef _H_symbol
#include "symbol.h"
#endif
#ifndef _H_expressn
#include "expressn.h"
#endif
#ifndef _H_evaluatn
#include "evaluatn.h"
#endif
#ifndef _H_constrct
#include "constrct.h"
#endif
#ifndef _H_moduldef
#include "moduldef.h"
#endif
#ifndef _H_constrnt
#include "constrnt.h"
#endif
#ifndef _H_cstrccom
#include "cstrccom.h"
#endif
#ifndef _H_agenda
#include "agenda.h"
#endif
#ifndef _H_network
#include "network.h"
#endif

#if FUZZY_DEFTEMPLATES 

/* These structs are added at the end of rule structs
   that have patterns that have FUZZY slot references.
   They record for each fuzzy slot the pattern number
   in the rule and the slot number within the pattern
   as well as the ptr to that fuzzy values hash node.

   The patterns and slots are indexed from 0 (i.e. the 1st
   pattern has patternNum = 0)
*/
struct fzSlotLocator
  {
    unsigned int patternNum;
    unsigned int slotNum : 13;
    FUZZY_VALUE_HN *fvhnPtr;
  };

#endif


struct defrule
  {
   struct constructHeader header;
   int salience;
   int localVarCnt;
   unsigned int complexity      : 11;
   unsigned int afterBreakpoint :  1;
   unsigned int watchActivation :  1;
   unsigned int watchFiring     :  1;
   unsigned int autoFocus       :  1;
   unsigned int executing       :  1;
#if DYNAMIC_SALIENCE
   struct expr *dynamicSalience;
#endif
   struct expr *actions;
#if LOGICAL_DEPENDENCIES
   struct joinNode *logicalJoin;
#endif
   struct joinNode *lastJoin;
   struct defrule *disjunct;
#if CERTAINTY_FACTORS     /* added 03-12-96 */
   double CF;
   struct expr *dynamicCF;
#endif
#if FUZZY_DEFTEMPLATES    /* added 03-12-96 */
   double       min_of_maxmins;
   unsigned int lhsRuleType;
   unsigned int numberOfFuzzySlots;
   struct fzSlotLocator *pattern_fv_arrayPtr;
#endif
  };

struct defruleModule
  {
   struct defmoduleItemHeader header;
   struct activation *agenda;
  };

#define GetDefruleName(x) GetConstructNameString((struct constructHeader *) x)
#define GetDefrulePPForm(x) GetConstructPPForm((struct constructHeader *) x)
#define DefruleModule(x) GetConstructModuleName((struct constructHeader *) x)

#define GetPreviousJoin(theJoin) \
   (((theJoin)->joinFromTheRight) ? \
    ((struct joinNode *) (theJoin)->rightSideEntryStructure) : \
    ((theJoin)->lastLevel))
#define GetPatternForJoin(theJoin) \
   (((theJoin)->joinFromTheRight) ? \
    NULL : \
    ((theJoin)->rightSideEntryStructure))

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _RULEDEF_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE void                           InitializeDefrules(void);
   LOCALE void                          *FindDefrule(char *);
   LOCALE void                          *GetNextDefrule(void *);
   LOCALE struct defruleModule          *GetDefruleModuleItem(struct defmodule *);
   LOCALE BOOLEAN                        IsDefruleDeletable(void *);

#ifndef _RULEDEF_SOURCE_
   extern struct construct              *DefruleConstruct;
   extern int                            DefruleModuleIndex;
   extern long                           CurrentEntityTimeTag;
#endif

#endif


