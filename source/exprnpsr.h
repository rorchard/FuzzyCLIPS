/*  $Header: /dist/CVS/fzclips/src/exprnpsr.h,v 1.3 2001/08/11 21:05:25 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*            EXPRESSION PARSER HEADER FILE            */
   /*******************************************************/

/*************************************************************/
/* Purpose: Provides routines for parsing expressions.       */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_exprnpsr

#define _H_exprnpsr

#ifndef _H_extnfunc
#include "extnfunc.h"
#endif
#ifndef _H_scanner
#include "scanner.h"
#endif

#if (! RUN_TIME)

typedef struct saved_contexts
  {
   int rtn;
   int brk;
   struct saved_contexts *nxt;
  } SAVED_CONTEXTS;

#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _EXPRNPSR_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE struct expr                   *Function0Parse(char *);
   LOCALE struct expr                   *Function1Parse(char *);
   LOCALE struct expr                   *Function2Parse(char *,char *);
   LOCALE void                           PushRtnBrkContexts(void);
   LOCALE void                           PopRtnBrkContexts(void);
   LOCALE BOOLEAN                        ReplaceSequenceExpansionOps(struct expr *,struct expr *,
                                                                     void *,void *);
   LOCALE struct expr                   *CollectArguments(struct expr *,char *);
   LOCALE struct expr                   *ArgumentParse(char *,int *);
   LOCALE struct expr                   *ParseAtomOrExpression(char *,struct token *);
   LOCALE EXPRESSION                    *ParseConstantArguments(char *,int *);
   LOCALE BOOLEAN                        SetSequenceOperatorRecognition(int);
   LOCALE BOOLEAN                        GetSequenceOperatorRecognition(void);
   LOCALE struct expr                   *GroupActions(char *,struct token *,int,char *,int);
#if (! RUN_TIME)
   LOCALE int                     CheckExpressionAgainstRestrictions(struct expr *,char *,char *);
#endif

#ifndef _EXPRNPSR_SOURCE_
#if (! RUN_TIME)
extern SAVED_CONTEXTS *svContexts;
extern int ReturnContext,BreakContext;
#endif
extern int SequenceOpMode;
#endif

#endif




