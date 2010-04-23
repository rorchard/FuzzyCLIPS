/*  $Header: /dist/CVS/fzclips/src/cfdef.h,v 1.3 2001/08/11 21:04:10 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*             CLIPS Version 6.00  05/12/93            */
   /*                                                     */
   /*             CERTAINTY FACTOR HEADER FILE            */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*      Brian L. Donnell                                     */
/*      Bob Orchard (NRCC - Nat'l Research Council of Canada)*/
/*	                (Fuzzy reasoning extensions)             */
/*	                (certainty factors for facts and rules)  */
/*	                (extensions to run command)              */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/


#ifndef _H_cfdef
#define _H_cfdef


#ifndef _H_factmngr
#include "factmngr.h"
#endif


#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _CFDEF_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE double                possibility( struct fuzzy_value *f1, struct fuzzy_value *f2 );
   LOCALE double                necessity( struct fuzzy_value *f1, struct fuzzy_value *f2 );
   LOCALE double                similarity( struct fuzzy_value *f1, struct fuzzy_value *f2 );
   LOCALE void                  InitializeCF();
   LOCALE struct expr           *ParseDeclareUncertainty(char *readSource,
                                                        char *ruleName,
                                                        int *error,
                                                        double *cfVALUE);
   LOCALE double                computeStdConclCF(double theRuleCF,
                                                  struct partialMatch *binds);
#if FUZZY_DEFTEMPLATES
   LOCALE double                computeFuzzyCrispConclCF(struct defrule *theRule,
                                                  struct partialMatch *binds);
#endif
   LOCALE void                  changeCFofNewFact(struct fact *newfact);
   LOCALE void                  changeCFofExistingFact(struct fact *fact1,struct fact *fact2);
   LOCALE void                  changeCFofNewVsExistingFact(struct fact *fact1,struct fact *fact2);
   LOCALE void                  cfInformationError(char *);
   LOCALE void                  cfRangeError();
   LOCALE void                  cfNonNumberError();
   LOCALE void                  printCF(char *logicalName, double cf);
   LOCALE double                getcf();
   LOCALE void                  set_threshold();
   LOCALE void                  unthreshold();
   LOCALE double                get_threshold();
	LOCALE void                  enable_rule_cf_calculation();
	LOCALE void                  disable_rule_cf_calculation();

#ifndef _CFDEF_SOURCE_
   extern double               Threshold_CF;
#endif


#endif


