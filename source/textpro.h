/*  $Header: /dist/CVS/fzclips/src/textpro.h,v 1.3 2001/08/11 21:08:08 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*             TEXT PROCESSING HEADER FILE             */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Brian L. Donnell                                     */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_textpro

#define _H_textpro

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _TEXTPRO_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

#if TEXTPRO_FUNCTIONS
   LOCALE void                           FetchCommand(DATA_OBJECT *);
   LOCALE int                            PrintRegionCommand(void);
   int                                   TossCommand(void);
#endif

#if HELP_FUNCTIONS
   LOCALE void                           HelpFunction(void);
   LOCALE void                           HelpPathFunction(void);
#endif

   LOCALE void                           HelpFunctionDefinitions(void);
#endif






