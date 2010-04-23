/*  $Header: /dist/CVS/fzclips/src/insfile.h,v 1.3 2001/08/11 21:06:29 dave Exp $  */

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

#ifndef _H_insfile
#define _H_insfile

#ifndef _H_expressn
#include "expressn.h"
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _INSFILE_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

#if (! RUN_TIME)
LOCALE void SetupInstanceFileCommands(void);
#endif

LOCALE long SaveInstancesCommand(void);
LOCALE long LoadInstancesCommand(void);
LOCALE long RestoreInstancesCommand(void);
LOCALE long SaveInstances(char *,int,EXPRESSION *,BOOLEAN);

#if BSAVE_INSTANCES
LOCALE long BinarySaveInstancesCommand(void);
LOCALE long BinarySaveInstances(char *,int,EXPRESSION *,BOOLEAN);
#endif

#if BLOAD_INSTANCES
LOCALE long BinaryLoadInstancesCommand(void);
LOCALE long BinaryLoadInstances(char *);
#endif

LOCALE long LoadInstances(char *);
LOCALE long LoadInstancesFromString(char *,int);
LOCALE long RestoreInstances(char *);
LOCALE long RestoreInstancesFromString(char *,int);

#ifndef _INSFILE_SOURCE_
#endif

#endif





