/*  $Header: /dist/CVS/fzclips/src/inscom.h,v 1.3 2001/08/11 21:06:27 dave Exp $  */

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

#ifndef _H_inscom
#define _H_inscom

#ifndef _H_object
#include "object.h"
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _INSCOM_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

LOCALE void SetupInstances(void);
LOCALE BOOLEAN DeleteInstance(void *);
LOCALE BOOLEAN UnmakeInstance(void *);
#if DEBUGGING_FUNCTIONS
LOCALE void InstancesCommand(void);
LOCALE void PPInstanceCommand(void);
LOCALE void Instances(char *,void *,char *,int);
#endif
LOCALE void *MakeInstance(char *);
LOCALE void *CreateRawInstance(void *,char *);
LOCALE void *FindInstance(void *,char *,BOOLEAN);
LOCALE int ValidInstanceAddress(void *);
LOCALE void DirectGetSlot(void *,char *,DATA_OBJECT *);
LOCALE int DirectPutSlot(void *,char *,DATA_OBJECT *);
LOCALE char *GetInstanceName(void *);
LOCALE void *GetInstanceClass(void *);
LOCALE unsigned long GetGlobalNumberOfInstances(void);
LOCALE void *GetNextInstance(void *);
LOCALE void *GetNextInstanceInScope(void *);
LOCALE void *GetNextInstanceInClass(void *,void *);
LOCALE void *GetNextInstanceInClassAndSubclasses(void **,void *,DATA_OBJECT *);
LOCALE void GetInstancePPForm(char *,int,void *);
LOCALE void ClassCommand(DATA_OBJECT *);
LOCALE BOOLEAN DeleteInstanceCommand(void);
LOCALE BOOLEAN UnmakeInstanceCommand(void);
LOCALE void SymbolToInstanceName(DATA_OBJECT *);
LOCALE void *InstanceNameToSymbol(void);
LOCALE void InstanceAddressCommand(DATA_OBJECT *);
LOCALE void InstanceNameCommand(DATA_OBJECT *);
LOCALE BOOLEAN InstanceAddressPCommand(void);
LOCALE BOOLEAN InstanceNamePCommand(void);
LOCALE BOOLEAN InstancePCommand(void);
LOCALE BOOLEAN InstanceExistPCommand(void);

#ifndef _INSCOM_SOURCE_
extern INSTANCE_TYPE DummyInstance;
#endif

#endif





