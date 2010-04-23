/*  $Header: /dist/CVS/fzclips/src/dffctpsr.h,v 1.3 2001/08/11 21:04:56 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*             DEFFACTS PARSER HEADER FILE             */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
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

#ifndef _H_dffctpsr
#define _H_dffctpsr

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _DFFCTPSR_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE int                            ParseDeffacts(char *);

#endif


