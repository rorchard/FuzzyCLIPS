/*  $Header: /dist/CVS/fzclips/src/sysdep.h,v 1.3 2001/08/11 21:08:06 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*            SYSTEM DEPENDENT HEADER FILE             */
   /*******************************************************/

/*************************************************************/
/* Purpose: Isolation of system dependent routines.          */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_sysdep
#define _H_sysdep

#if IBM_TBC || IBM_MSC || IBM_ICB
#include <dos.h>
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _SYSDEP_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE void                        InitializeEnvironment(void);
   LOCALE void                        SetRedrawFunction(void (*)(void));
   LOCALE void                        SetPauseEnvFunction(void (*)(void));
   LOCALE void                        SetContinueEnvFunction(void (*)(int));
   LOCALE void                        RerouteStdin(int,char *[]);
   LOCALE double                      gentime(void);
   LOCALE void                        gensystem(void);
   LOCALE void                        VMSSystem(char *);
   LOCALE int                         GenOpen(char *,char *);
   LOCALE void                        GenSeek(long);
   LOCALE void                        GenClose(void);
   LOCALE void                        GenRead(void *,unsigned long);
   LOCALE void                        genexit(int);
   LOCALE int                         genrand(void);
   LOCALE void                        genseed(int);
   LOCALE int                         genremove(char *);
   LOCALE int                         genrename(char *,char *);

#ifndef _SYSDEP_SOURCE_
   extern void                    (*RedrawScreenFunction)(void);
   extern void                    (*PauseEnvFunction)(void);
   extern void                    (*ContinueEnvFunction)(int);
#endif

#define InitializeCLIPS() InitializeEnvironment()

#endif





