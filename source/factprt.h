/*  $Header: /dist/CVS/fzclips/src/factprt.h,v 1.3 2001/08/11 21:05:44 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*         FACT RETE PRINT FUNCTIONS HEADER FILE       */
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

#ifndef _H_factprt

#define _H_factprt

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _FACTPRT_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE void                           PrintFactJNCompVars1(char *,void *);
   LOCALE void                           PrintFactJNCompVars2(char *,void *);
   LOCALE void                           PrintFactPNCompVars1(char *,void *);
   LOCALE void                           PrintFactJNGetVar1(char *,void *);
   LOCALE void                           PrintFactJNGetVar2(char *,void *);
   LOCALE void                           PrintFactJNGetVar3(char *,void *);
   LOCALE void                           PrintFactPNGetVar1(char *,void *);
   LOCALE void                           PrintFactPNGetVar2(char *,void *);
   LOCALE void                           PrintFactPNGetVar3(char *,void *);
   LOCALE void                           PrintFactSlotLength(char *,void *);
   LOCALE void                           PrintFactPNConstant1(char *,void *);
   LOCALE void                           PrintFactPNConstant2(char *,void *);
#if FUZZY_DEFTEMPLATES
   LOCALE void                           PrintPNFUZZY_VALUE(char *,void *);
#endif


#endif






