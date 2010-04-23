/*  $Header: /dist/CVS/fzclips/src/rulecmp.h,v 1.3 2001/08/11 21:07:43 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*        DEFRULE CONSTRUCT COMPILER HEADER FILE       */
   /*******************************************************/

/*************************************************************/
/* Purpose: Implements the constructs-to-c feature for the   */
/*    defrule construct.                                     */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*      Bob Orchard (NRCC - Nat'l Research Council of Canada)*/
/*                  (Fuzzy reasoning extensions)             */
/*                  (certainty factors for facts and rules)  */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_rulecmp
#define _H_rulecmp

#include "conscomp.h"
#ifndef _H_extnfunc
#include "extnfunc.h"
#endif

#define JoinPrefix() ArbitraryPrefix(DefruleCodeItem,2)

#if FUZZY_DEFTEMPLATES  
#define PatternFvPrefix() ArbitraryPrefix(DefruleCodeItem,3)
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _RULECMP_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE void                     DefruleCompilerSetup(void);
   LOCALE void                     DefruleCModuleReference(FILE *,int,int,int);

#ifndef _RULECMP_SOURCE_
extern struct CodeGeneratorItem *DefruleCodeItem;
#endif

#endif




