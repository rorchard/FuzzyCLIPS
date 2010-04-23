/*  $Header: /dist/CVS/fzclips/src/insmult.h,v 1.3 2001/08/11 21:06:34 dave Exp $  */

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

#ifndef _H_insmult
#define _H_insmult

#ifndef _H_evaluatn
#include "evaluatn.h"
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _INSMULT_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

#if (! RUN_TIME)
LOCALE void SetupInstanceMultifieldCommands(void);
#endif

LOCALE void MVSlotReplaceCommand(DATA_OBJECT *);
LOCALE void MVSlotInsertCommand(DATA_OBJECT *);
LOCALE void MVSlotDeleteCommand(DATA_OBJECT *);
LOCALE BOOLEAN DirectMVReplaceCommand(void);
LOCALE BOOLEAN DirectMVInsertCommand(void);
LOCALE BOOLEAN DirectMVDeleteCommand(void);

#ifndef _INSMULT_SOURCE_
#endif

#endif





