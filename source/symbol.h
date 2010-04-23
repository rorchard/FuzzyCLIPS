/*  $Header: /dist/CVS/fzclips/src/symbol.h,v 1.3 2001/08/11 21:08:05 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*                 SYMBOL HEADER FILE                  */
   /*******************************************************/

/*************************************************************/
/* Purpose: Manages the atomic data value hash tables for    */
/*   storing symbols, integers, floats, and bit maps.        */
/*   Contains routines for adding entries, examining the     */
/*   hash tables, and performing garbage collection to       */
/*   remove entries no longer in use.                        */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*      Bob Orchard (NRCC - Nat'l Research Council of Canada)*/
/*                  (Fuzzy reasoning extensions)             */
/*                  (certainty factors for facts and rules)  */
/*                  (extensions to run command)              */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_symbol
#define _H_symbol

struct symbolHashNode;
struct floatHashNode;
struct integerHashNode;
struct bitMapHashNode;
#if FUZZY_DEFTEMPLATES  
struct fuzzyValueHashNode;
#endif
struct genericHashNode;
struct symbolMatch;

#if FUZZY_DEFTEMPLATES
typedef struct symbolHashNode SYMBOL_HN;
typedef struct floatHashNode FLOAT_HN;
typedef struct integerHashNode INTEGER_HN;
typedef struct bitMapHashNode BITMAP_HN;
typedef struct fuzzyValueHashNode FUZZY_VALUE_HN;
typedef struct genericHashNode GENERIC_HN;

#ifndef _H_fuzzyval
#include "fuzzyval.h"
#endif

#endif /* FUZZY_DEFTEMPLATES */


#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _SYMBOL_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

#ifndef SYMBOL_HASH_SIZE
#define SYMBOL_HASH_SIZE        1013
#endif

#ifndef FLOAT_HASH_SIZE
#define FLOAT_HASH_SIZE          503
#endif

#ifndef INTEGER_HASH_SIZE
#define INTEGER_HASH_SIZE        167
#endif

#ifndef BITMAP_HASH_SIZE
#define BITMAP_HASH_SIZE         167
#endif

#if FUZZY_DEFTEMPLATES
#define FUZZY_VALUE_HASH_SIZE	 167
#endif

#define CLIPSTrueSymbol TrueSymbol
#define CLIPSFalseSymbol FalseSymbol

/************************************************************/
/* symbolHashNode STRUCTURE:                                */
/************************************************************/
struct symbolHashNode
  {
   struct symbolHashNode *next;
   long count;
   int depth;
   unsigned int markedEphemeral : 1;
   unsigned int neededSymbol : 1;
   unsigned int bucket : 30;
   char *contents;
  };

/************************************************************/
/* floatHashNode STRUCTURE:                                  */
/************************************************************/
struct floatHashNode
  {
   struct floatHashNode *next;
   long count;
   int depth;
   unsigned int markedEphemeral : 1;
   unsigned int neededFloat : 1;
   unsigned int bucket : 30;
   double contents;
  };

/************************************************************/
/* integerHashNode STRUCTURE:                               */
/************************************************************/
struct integerHashNode
  {
   struct integerHashNode *next;
   long count;
   int depth;
   unsigned int markedEphemeral : 1;
   unsigned int neededInteger : 1;
   unsigned int bucket : 30;
   long int contents;
  };

/************************************************************/
/* bitMapHashNode STRUCTURE:                                */
/************************************************************/
struct bitMapHashNode
  {
   struct bitMapHashNode *next;
   long count;
   int depth;
   unsigned int markedEphemeral : 1;
   unsigned int neededBitMap : 1;
   unsigned int bucket : 30;
   char *contents;
   unsigned short size;
  };

#if FUZZY_DEFTEMPLATES    

/************************************************************/
/* fuzzyValueHashNode STRUCTURE:                            */
/************************************************************/
struct fuzzyValueHashNode
  {
   struct fuzzyValueHashNode *next;
   long count;
   int depth;
   unsigned int markedEphemeral : 1;
   unsigned int neededFuzzyValue : 1;
   unsigned int bucket : 30;
   struct fuzzy_value *contents;
  };

#endif

/************************************************************/
/* genericHashNode STRUCTURE:                               */
/************************************************************/
struct genericHashNode
  {
   struct genericHashNode *next;
   long count;
   int depth;
   unsigned int markedEphemeral : 1;
   unsigned int needed : 1;
   unsigned int bucket : 30;
  };

/************************************************************/
/* symbolMatch STRUCTURE:                               */
/************************************************************/
struct symbolMatch
  {
   struct symbolHashNode *match;
   struct symbolMatch *next;
  };

#if ! FUZZY_DEFTEMPLATES
typedef struct symbolHashNode SYMBOL_HN;
typedef struct floatHashNode FLOAT_HN;
typedef struct integerHashNode INTEGER_HN;
typedef struct bitMapHashNode BITMAP_HN;
typedef struct genericHashNode GENERIC_HN;
#endif

#define ValueToString(target) (((struct symbolHashNode *) (target))->contents)
#define ValueToDouble(target) (((struct floatHashNode *) (target))->contents)
#define ValueToLong(target) (((struct integerHashNode *) (target))->contents)
#define ValueToInteger(target) ((int) (((struct integerHashNode *) (target))->contents))
#define ValueToBitMap(target) ((void *) ((struct bitMapHashNode *) (target))->contents)
#if FUZZY_DEFTEMPLATES   
#define ValueToFuzzyValue(target) (((struct fuzzyValueHashNode *) (target))->contents)
#endif

#define IncrementSymbolCount(theValue) (((SYMBOL_HN *) theValue)->count++)
#define IncrementFloatCount(theValue) (((FLOAT_HN *) theValue)->count++)
#define IncrementIntegerCount(theValue) (((INTEGER_HN *) theValue)->count++)
#define IncrementBitMapCount(theValue) (((BITMAP_HN *) theValue)->count++)
#if FUZZY_DEFTEMPLATES   
#define IncrementFuzzyValueCount(theValue) (((FUZZY_VALUE_HN *) theValue)->count++)
#endif

/*****************************************************/
/* The FindSymbol function is remapped under certain */
/* conditions because it conflicts with a Metroworks */
/* Code Warrior library function.                    */
/*****************************************************/
#if MAC_MCW
#define FindSymbol MCWFindSymbol
#endif

   LOCALE void                          *AddSymbol(char *);
   LOCALE SYMBOL_HN                     *FindSymbol(char *);
   LOCALE void                          *AddDouble(double);
   LOCALE void                          *AddLong(long int);
   LOCALE void                          *AddBitMap(void *,int);
   LOCALE INTEGER_HN                    *FindLong(long int);
   LOCALE void                           InitializeAtomTables(void);
   LOCALE int                            HashSymbol(char *,int);
   LOCALE int                            HashFloat(double,int);
   LOCALE int                            HashInteger(long int,int);
   LOCALE int                            HashBitMap(char *,int,int);
   LOCALE void                           DecrementSymbolCount(struct symbolHashNode *);
   LOCALE void                           DecrementFloatCount(struct floatHashNode *);
   LOCALE void                           DecrementIntegerCount(struct integerHashNode *);
   LOCALE void                           DecrementBitMapCount(struct bitMapHashNode *);
   LOCALE void                           RemoveEphemeralAtoms(void);
   LOCALE struct symbolHashNode        **GetSymbolTable(void);
   LOCALE void                           SetSymbolTable(struct symbolHashNode **);
   LOCALE struct floatHashNode          **GetFloatTable(void);
   LOCALE void                           SetFloatTable(struct floatHashNode **);
   LOCALE struct integerHashNode       **GetIntegerTable(void);
   LOCALE void                           SetIntegerTable(struct integerHashNode **);
   LOCALE struct bitMapHashNode        **GetBitMapTable(void);
   LOCALE void                           SetBitMapTable(struct bitMapHashNode **);
   LOCALE void                           RefreshSpecialSymbols(void);
   LOCALE struct symbolMatch            *FindSymbolMatches(char *,int *,int *);
   LOCALE void                           ReturnSymbolMatches(struct symbolMatch *);
   LOCALE SYMBOL_HN                     *GetNextSymbolMatch(char *,int,SYMBOL_HN *,int,int *);
   LOCALE void                           ClearBitString(void *,int);
   LOCALE void                           SetAtomicValueIndices(int);
   LOCALE void                           RestoreAtomicValueBuckets(void);
#if FUZZY_DEFTEMPLATES     
   LOCALE void                          *AddFuzzyValue(struct fuzzy_value *);
   LOCALE int                            HashFuzzyValue(struct fuzzy_value *,int);
   LOCALE void                           DecrementFuzzyValueCount(struct fuzzyValueHashNode *);
   LOCALE struct fuzzyValueHashNode     **GetFuzzyValueTable(void);
   LOCALE void                           SetFuzzyValueTable(struct fuzzyValueHashNode **);
#endif


#ifndef _SYMBOL_SOURCE
   extern void                   *TrueSymbol;
   extern void                   *FalseSymbol;
   extern void                   *NegativeInfinity;
   extern void                   *PositiveInfinity;
   extern void                   *Zero;
#endif

#endif



