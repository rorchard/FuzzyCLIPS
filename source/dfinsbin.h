/*  $Header: /dist/CVS/fzclips/src/dfinsbin.h,v 1.3 2001/08/11 21:05:04 dave Exp $  */

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

#ifndef _H_dfinsbin
#define _H_dfinsbin

#if DEFINSTANCES_CONSTRUCT && (BLOAD || BLOAD_ONLY || BLOAD_AND_BSAVE)

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _DFINSBIN_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

LOCALE void SetupDefinstancesBload(void);
LOCALE void *BloadDefinstancesModuleRef(int);

#ifndef _DFINSBIN_SOURCE_
#endif

#endif

#endif




