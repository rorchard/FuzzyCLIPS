/*  $Header: /dist/CVS/fzclips/src/prntutil.h,v 1.3 2001/08/11 21:07:31 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*              PRINT UTILITY HEADER FILE              */
   /*******************************************************/

/*************************************************************/
/* Purpose: Utility routines for printing various items      */
/*   and messages.                                           */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_prntutil
#define _H_prntutil

#ifndef _H_moduldef
#include "moduldef.h"
#endif

#ifndef _STDIO_INCLUDED_
#define _STDIO_INCLUDED_
#include <stdio.h>
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _PRNTUTIL_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE void                           PrintInChunks(char *,char *);
   LOCALE void                           PrintFloat(char *,double);
   LOCALE void                           PrintLongInteger(char *,long);
   LOCALE void                           PrintAtom(char *,int,void *);
   LOCALE void                           PrintTally(char *,long,char *,char *);
   LOCALE char                          *FloatToString(double);
   LOCALE char                          *LongIntegerToString(long);
   LOCALE void                           SyntaxErrorMessage(char *);
   LOCALE void                           SystemError(char *,int);
   LOCALE void                           PrintErrorID(char *,int,int);
   LOCALE void                           PrintWarningID(char *,int,int);
   LOCALE void                           CantFindItemErrorMessage(char *,char *);
   LOCALE void                           CantDeleteItemErrorMessage(char *,char *);
   LOCALE void                           AlreadyParsedErrorMessage(char *,char *);
   LOCALE void                           LocalVariableErrorMessage(char *);
   LOCALE void                           DivideByZeroErrorMessage(char *);
   LOCALE void                           SalienceInformationError(char *,char *);
   LOCALE void                           SalienceRangeError(int,int);
   LOCALE void                           SalienceNonIntegerError(void);

#ifndef _PRNTUTIL_SOURCE_
   extern BOOLEAN                     PreserveEscapedCharacters;
   extern BOOLEAN                     AddressesToStrings;
   extern BOOLEAN                     InstanceAddressesToNames;
#endif

#endif






