/*  $Header: /dist/CVS/fzclips/src/factmngr.h,v 1.3 2001/08/11 21:05:41 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*              FACTS MANAGER HEADER FILE              */
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

#ifndef _H_factmngr

#define _H_factmngr

struct fact;

#ifndef _H_pattern
#include "pattern.h"
#endif
#include "multifld.h"
#ifndef _H_evaluatn
#include "evaluatn.h"
#endif
#ifndef _H_tmpltdef
#include "tmpltdef.h"
#endif

struct fact
  {
   struct patternEntity factHeader;
   struct deftemplate *whichDeftemplate;
   void *list;
   long int factIndex;
   unsigned int depth : 15;
   unsigned int garbage : 1;
#if CERTAINTY_FACTORS
   double factCF;
#endif
   struct fact *previousFact;
   struct fact *nextFact;
   struct multifield theProposition;
  };

#ifdef LOCALE
#undef LOCALE
#endif
#ifdef _FACTMNGR_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE void                           PrintFactWithIdentifier(char *,struct fact *);
   LOCALE void                           PrintFact(char *,struct fact *);
   LOCALE void                           PrintFactIdentifierInLongForm(char *,void *);
   LOCALE BOOLEAN                        Retract(void *);
   LOCALE void                          *Assert(void *);
   LOCALE void                           RemoveAllFacts(void);
   LOCALE struct fact                   *CreateFactBySize(int);
   LOCALE void                           FactInstall(struct fact *);
   LOCALE void                           FactDeinstall(struct fact *);
   LOCALE void                          *GetNextFact(void *);
   LOCALE void                          *GetNextFactInScope(void *);
   LOCALE void                           GetFactPPForm(char *,int,void *);
   LOCALE long int                       FactIndex(void *);
   LOCALE void                          *AssertString(char *);
   LOCALE int                            GetFactListChanged(void);
   LOCALE void                           SetFactListChanged(int);
   LOCALE long int                       GetNumberOfFacts(void);
   LOCALE void                           InitializeFacts(void);
   LOCALE struct fact                   *FindIndexedFact(long);
   LOCALE void                           IncrementFactCount(void *);
   LOCALE void                           DecrementFactCount(void *);
   LOCALE void                           PrintFactIdentifier(char *,void *);
   LOCALE void                           DecrementFactBasisCount(void *);
   LOCALE void                           IncrementFactBasisCount(void *);
   LOCALE void                           ReturnFact(struct fact *);
   LOCALE void                           MatchFactFunction(void *);
   LOCALE struct fact                   *CreateFact(void *);
   LOCALE BOOLEAN                        PutFactSlot(void *,char *,DATA_OBJECT *);
   LOCALE BOOLEAN                        GetFactSlot(void *,char *,DATA_OBJECT *);
   LOCALE BOOLEAN                        AssignFactSlotDefaults(void *);
   LOCALE BOOLEAN                        CopyFactSlotValues(void *,void *);

#ifndef _FACTMNGR_SOURCE_
   extern int                            ChangeToFactList;
   extern struct fact                    DummyFact;
#if DEBUGGING_FUNCTIONS
   extern int                            WatchFacts;
#endif
#endif

#endif





