/*  $Header: /dist/CVS/fzclips/src/genrcbin.h,v 1.3 2001/08/11 21:06:04 dave Exp $  */

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

#ifndef _H_genrcbin
#define _H_genrcbin

#include "genrcfun.h"

#define GenericPointer(i) (((i) == -1L) ? NULL : (DEFGENERIC *) &defgenericArray[i])

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _GENRCBIN_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

LOCALE void SetupGenericsBload(void);
LOCALE void *BloadDefgenericModuleReference(int);

#ifndef _GENRCBIN_SOURCE_
extern DEFGENERIC *defgenericArray;
#endif

#endif





