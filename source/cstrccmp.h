/*  $Header: /dist/CVS/fzclips/src/cstrccmp.h,v 1.3 2001/08/11 21:04:31 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*           CONSTRUCT CONSTRUCTS-TO-C HEADER          */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
/*      Support functions for the constructs-to-c of         */
/*      construct headers and related items.                 */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_cstrccmp
#define _H_cstrccmp

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _CSTRCCMP_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

#ifndef _STDIO_INCLUDED_
#define _STDIO_INCLUDED_
#include <stdio.h>
#endif

#if ANSI_COMPILER  
   LOCALE VOID                           MarkConstructHeaders(int);
#else
   LOCALE VOID                           MarkConstructHeaders();
#endif 

#endif




