/*  $Header: /dist/CVS/fzclips/src/rulepsr.h,v 1.3 2001/08/11 21:07:51 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*               RULE PARSING HEADER FILE              */
   /*******************************************************/

/*************************************************************/
/* Purpose: Coordinates parsing of a rule.                   */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_rulepsr
#define _H_rulepsr

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _RULEPSR_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE int                            ParseDefrule(char *);
   LOCALE struct lhsParseNode           *FindVariable(struct symbolHashNode *,
                                                      struct lhsParseNode *);

#endif




