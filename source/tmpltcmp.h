/*  $Header: /dist/CVS/fzclips/src/tmpltcmp.h,v 1.3 2001/08/11 21:08:12 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*      DEFTEMPLATE CONSTRUCT COMPILER HEADER FILE     */
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

#ifndef _H_tmpltcmp

#define _H_tmpltcmp

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _TMPLTCMP_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE void                           DeftemplateCompilerSetup(void);
   LOCALE void                           DeftemplateCModuleReference(FILE *,int,int,int);
   LOCALE void                           DeftemplateCConstructReference(FILE *,void *,int,int);

#endif
