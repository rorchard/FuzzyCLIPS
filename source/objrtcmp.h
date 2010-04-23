/*  $Header: /dist/CVS/fzclips/src/objrtcmp.h,v 1.3 2001/08/11 21:07:13 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*               CLIPS Version 6.05  04/09/97          */
   /*                                                     */
   /*                                                     */
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

#ifndef _H_objrtcmp
#define _H_objrtcmp

#if INSTANCE_PATTERN_MATCHING && (! RUN_TIME) && CONSTRUCT_COMPILER

#ifndef _STDIO_INCLUDED_
#include <stdio.h>
#define _STDIO_INCLUDED_
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _OBJRTCMP_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

LOCALE void ObjectPatternsCompilerSetup(void);
LOCALE void ObjectPatternNodeReference(void *,FILE *,int,int);

#endif

#endif






