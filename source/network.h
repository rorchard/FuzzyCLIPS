/*  $Header: /dist/CVS/fzclips/src/network.h,v 1.3 2001/08/11 21:07:05 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*                 NETWORK HEADER FILE                 */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_setup
#include "setup.h"
#endif

#if FUZZY_DEFTEMPLATES
/* Due to a circular set of definitions we need to do this
   match.h includes evaluatn.h which includes symbol.h
   which includes fuzzyval.h
   which includes tmpltdef.h which includes ... .h
   which includes network.h => problem!!
   This effectively makes the include of evaluatn.h an
   include of symbol.h to make sure tmpltdef is included
   before evaluatn!
*/
#ifndef _H_symbol
#include "symbol.h"
#endif
#endif

#ifndef _H_network

#define _H_network

struct patternNodeHeader;
struct joinNode;

#ifndef _H_match
#include "match.h"
#endif

struct patternNodeHeader
  {
   struct partialMatch *alphaMemory;
   struct partialMatch *endOfQueue;
   struct joinNode *entryJoin;
   unsigned int singlefieldNode : 1;
   unsigned int multifieldNode : 1;
   unsigned int stopNode : 1;
   unsigned int initialize : 1;
   unsigned int marked : 1;
   unsigned int beginSlot : 1;
   unsigned int endSlot : 1;
  };

#ifndef _H_expressn
#include "expressn.h"
#endif

#ifndef _H_ruledef
#include "ruledef.h"
#endif

struct joinNode
  {
   unsigned int firstJoin : 1;
   unsigned int logicalJoin : 1;
   unsigned int joinFromTheRight : 1;
   unsigned int patternIsNegated : 1;
   unsigned int initialize : 1;
   unsigned int marked : 1;
   unsigned int rhsType : 3;
   unsigned int depth : 7;
   long bsaveID;
   struct partialMatch *beta;
   struct expr *networkTest;
   void *rightSideEntryStructure;
   struct joinNode *nextLevel;
   struct joinNode *lastLevel;
   struct joinNode *rightDriveNode;
   struct joinNode *rightMatchNode;
   struct defrule *ruleToActivate;
  };

#endif




