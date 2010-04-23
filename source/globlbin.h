/*  $Header: /dist/CVS/fzclips/src/globlbin.h,v 1.3 2001/08/11 21:06:13 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*             DEFGLOBAL BINARY HEADER FILE            */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*      Brian L. Donnell                                     */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_globlbin

#define _H_globlbin

#include "modulbin.h"
#include "cstrcbin.h"
#include "globldef.h"

struct bsaveDefglobal
  {
   struct bsaveConstructHeader header;
   long initial;
  };

struct bsaveDefglobalModule
  {
   struct bsaveDefmoduleItemHeader header;
  };

#define DefglobalPointer(i) ((struct defglobal *) (&DefglobalArray[i]))

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _GLOBLBIN_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE void                           DefglobalBinarySetup(void);
   LOCALE void                          *BloadDefglobalModuleReference(int);

#ifndef _GLOBLBIN_SOURCE_
   extern struct defglobal *DefglobalArray;
#endif
#endif






