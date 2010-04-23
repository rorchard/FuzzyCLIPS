/*  $Header: /dist/CVS/fzclips/src/objbin.h,v 1.3 2001/08/11 21:07:07 dave Exp $  */

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

#ifndef _H_objbin
#define _H_objbin

#define DefclassPointer(i) (((i) == -1L) ? NULL : (DEFCLASS *) &defclassArray[i])
#define DefclassIndex(cls) (((cls) == NULL) ? -1 : ((struct constructHeader *) cls)->bsaveID)

#ifndef _H_object
#include "object.h"
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _OBJBIN_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

LOCALE void SetupObjectsBload(void);
LOCALE void *BloadDefclassModuleReference(int);

#ifndef _OBJBIN_SOURCE_
extern DEFCLASS *defclassArray;
#endif

#endif





