static char rcsid[] = "$Header: /dist/CVS/fzclips/src/symblcmp.c,v 1.3 2001/08/11 21:08:02 dave Exp $" ;

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*           SYMBOL CONSTRUCT COMPILER MODULE          */
   /*******************************************************/

/*************************************************************/
/* Purpose: Implements the constructs-to-c feature for       */
/*   atomic data values: symbols, integers, floats, and      */
/*   bit maps.                                               */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*      Barry Cameron                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*      Brian L. Donnell                                     */
/*      Bob Orchard (NRCC - Nat'l Research Council of Canada)*/
/*                  (Fuzzy reasoning extensions)             */
/*                  (certainty factors for facts and rules)  */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#define _SYMBLCMP_SOURCE_

#include "setup.h"

#if CONSTRUCT_COMPILER && (! RUN_TIME)

#include <stdio.h>
#define _STDIO_INCLUDED_
#include <string.h>

#include "symbol.h"
#include "memalloc.h"
#include "constant.h"
#include "exprnpsr.h"
#include "cstrccom.h"
#include "constrct.h"
#include "argacces.h"
#include "cstrncmp.h"
#include "router.h"
#include "conscomp.h"
#include "utility.h"

#include "symblcmp.h"

#if FUZZY_DEFTEMPLATES   
#include "fuzzyval.h"
#include "prntutil.h"
#include "tmpltcmp.h"
#endif

/***************************************/
/* LOCAL INTERNAL FUNCTION DEFINITIONS */
/***************************************/

   static int                         SymbolHashNodesToCode(char *,int);
#if FUZZY_DEFTEMPLATES   
   static int                         FuzzyValueHashNodesToCode(char *,int);
   static int                         FuzzyValuesToCode(char *,int);
   static int                         FuzzyValueArraysToCode(char *,int);
#endif
   static int                         BitMapHashNodesToCode(char *,int);
   static int                         BitMapValuesToCode(char *,int);
   static int                         FloatHashNodesToCode(char *,int);
   static int                         IntegerHashNodesToCode(char *,int);
   static int                         HashTablesToCode(char *);
   static void                        PrintCString(FILE *,char *);

/**************************************************************/
/* AtomicValuesToCode: Driver routine for generating the code */
/*  used by the symbol, integer, float, and bit map tables.   */
/**************************************************************/
globle void AtomicValuesToCode(
  char *fileName)
  {
   int version;

   SetAtomicValueIndices(TRUE);

   HashTablesToCode(fileName);

#if FUZZY_DEFTEMPLATES    
   /* would have been easier to change if HashTablesToCode returned
      the next free version to use
   */
   version = SymbolHashNodesToCode(fileName,6);
#else
   version = SymbolHashNodesToCode(fileName,5);
#endif
   version = FloatHashNodesToCode(fileName,version);
   version = IntegerHashNodesToCode(fileName,version);
   version = BitMapHashNodesToCode(fileName,version);

#if FUZZY_DEFTEMPLATES  
   version = BitMapValuesToCode(fileName,version);
   version = FuzzyValueHashNodesToCode(fileName,version);
   version = FuzzyValuesToCode(fileName,version);
   version = FuzzyValueArraysToCode(fileName,version);
#else
   BitMapValuesToCode(fileName,version);
#endif
  }


#if FUZZY_DEFTEMPLATES    

/***********************************************************************/
/* FuzzyValueHashNodesToCode:                                          */
/*                                                                     */
/*   1st write out the fuzzy value hash nodes (V array) putting correct*/
/*   references to the actual fuzzy value array (W array)              */
/***********************************************************************/
static int FuzzyValueHashNodesToCode(fileName,version)
  char *fileName;
  int version;
  {
   int i, j;
   struct fuzzyValueHashNode *hashPtr;
   int count;
   int numberOfEntries;
   struct fuzzyValueHashNode **fuzzyValueTable;
   int newHeader = TRUE;
   int arrayVersion = 1;
   FILE *fp;

   /*====================================*/
   /* Count the total number of entries. */
   /*====================================*/

   fuzzyValueTable = GetFuzzyValueTable();
   count = numberOfEntries = 0;

   for (i = 0; i < FUZZY_VALUE_HASH_SIZE; i++)
     {
      hashPtr = fuzzyValueTable[i];
      while (hashPtr != NULL)
        {
         numberOfEntries++;
         hashPtr = hashPtr->next;
        }
     }

   if (numberOfEntries == 0) return(version);

   for (i = 1; i <= (numberOfEntries / MaxIndices) + 1 ; i++)
     { fprintf(HeaderFP,"extern struct fuzzyValueHashNode V%d_%d[];\n",ImageID,i); }

   /*==================*/
   /* Create the file. */
   /*==================*/
   if ((fp = NewCFile(fileName,1,version,FALSE)) == NULL) return(-1);

   /*===================*/
   /* List the entries. */
   /*===================*/

   j = 0;

   for (i = 0; i < FUZZY_VALUE_HASH_SIZE; i++)
     {
      hashPtr = fuzzyValueTable[i];
      while (hashPtr != NULL)
        {
         if (newHeader)
           {
            fprintf(fp,"struct fuzzyValueHashNode V%d_%d[] = {\n",ImageID,arrayVersion);
            newHeader = FALSE;
           }

         /* fill in 'next' slot in fuzzyValueHashNode struct */
         if (hashPtr->next == NULL)
           { fprintf(fp,"{NULL,"); }
         else
           {
            if ((j + 1) >= MaxIndices)
              { fprintf(fp,"{&V%d_%d[%d],",ImageID,arrayVersion + 1,0); }
            else
              { fprintf(fp,"{&V%d_%d[%d],",ImageID,arrayVersion,j + 1); }
           }

         /* fill in other slots in fuzzyValueHashNode struct */
         fprintf(fp,"%d,0,0,0,%d,(struct fuzzy_value *) &W%d_%d[%d]",
                     hashPtr->count + 1,i,
                     ImageID,arrayVersion,j);

         count++;
         j++;

         if ((count == numberOfEntries) || (j >= MaxIndices))
           {
            fprintf(fp,"}};\n");
            fclose(fp);
            j = 0;
            arrayVersion++;
            version++;
            if (count < numberOfEntries)
              {
               if ((fp = NewCFile(fileName,1,version,FALSE)) == NULL) return(0);
               newHeader = TRUE;
              }
           }
         else
           { fprintf(fp,"},\n"); }
         hashPtr = hashPtr->next;
        }
     }

   return(version);
  }


/***********************************************************************/
/* FuzzyValuesToCode:                                                  */
/*                                                                     */
/*   2nd write out the fuzzy value array (W array) putting correct     */
/*   references to the x,y double arrays (X array)                      */
/***********************************************************************/
static int FuzzyValuesToCode(fileName,version)
  char *fileName;
  int version;
  {
   int i, j;
   struct fuzzyValueHashNode *hashPtr;
   int count;
   int numberOfEntries;
   struct fuzzyValueHashNode **fuzzyValueTable;
   int newHeader = TRUE;
   int arrayVersion = 1;
   FILE *fp;
   int arraySize;
   int xyArrayPartition = 1, xyArrayPartitionCount = 0;

   /*====================================*/
   /* Count the total number of entries. */
   /*====================================*/

   fuzzyValueTable = GetFuzzyValueTable();
   count = numberOfEntries = 0;

   for (i = 0; i < FUZZY_VALUE_HASH_SIZE; i++)
     {
      hashPtr = fuzzyValueTable[i];
      while (hashPtr != NULL)
        {
         numberOfEntries++;
         hashPtr = hashPtr->next;
        }
     }

   if (numberOfEntries == 0) return(version);

   for (i = 1; i <= (numberOfEntries / MaxIndices) + 1 ; i++)
     { fprintf(HeaderFP,"extern struct fuzzy_value W%d_%d[];\n",ImageID,i); }

   /*==================*/
   /* Create the file. */
   /*==================*/

   if ((fp = NewCFile(fileName,1,version,FALSE)) == NULL) return(-1);

   /*=====================*/
   /* Fillin the entries. */
   /*=====================*/

   j = 0;

   for (i = 0; i < FUZZY_VALUE_HASH_SIZE; i++)
     {
      hashPtr = fuzzyValueTable[i];
      while (hashPtr != NULL)
        {
         if (newHeader)
           {
            fprintf(fp,"struct fuzzy_value W%d_%d[] = {\n",ImageID,arrayVersion);
            newHeader = FALSE;
           }

         /* fill in the entries of the fuzzy value struct:
               whichDeftemplate - ptr to deftemplate structure
               name  - char *
               maxn  - int
               n     - int
               x     - ptr to double (will be index into X array)
               y     - ptr to double (will be index into X array)
         */

         fprintf(fp,"{");
         DeftemplateCConstructReference(fp, hashPtr->contents->whichDeftemplate,
                                        ImageID, MaxIndices);
         fprintf(fp,",");
                 PrintCString(fp,hashPtr->contents->name);
         /* NOTE: arraySize twice in next fprintf line is correct!
                  We don't want to store maxn value since we pack the arrays
                  when writing X array out to c file
         */
         arraySize = hashPtr->contents->n;
         fprintf(fp,",%d,%d", arraySize, arraySize);
         fprintf(fp,",(double *)&X%d_%d[%d], (double *)&X%d_%d[%d]",
                 ImageID, xyArrayPartition, xyArrayPartitionCount,
                 ImageID, xyArrayPartition, xyArrayPartitionCount+arraySize);

          xyArrayPartitionCount += arraySize + arraySize;
          if (xyArrayPartitionCount >= MaxIndices)
            {
              xyArrayPartitionCount = 0;
              xyArrayPartition++;
            }

         count++;
         j++;

         if ((count == numberOfEntries) || (j >= MaxIndices))
           {
            fprintf(fp,"}};\n");
            fclose(fp);
            j = 0;
            arrayVersion++;
            version++;
            if (count < numberOfEntries)
              {
               if ((fp = NewCFile(fileName,1,version,FALSE)) == NULL) return(0);
               newHeader = TRUE;
              }
           }
         else
           { fprintf(fp,"},\n"); }

         hashPtr = hashPtr->next;
        }
     }

   return(version);
  }


/***********************************************************************/
/* FuzzyValueArraysToCode:                                             */
/*                                                                     */
/*   finally write out the fuzzy value xy array (X array) putting      */
/*   x values followed by y values                                     */
/***********************************************************************/
static int FuzzyValueArraysToCode(fileName,version)
  char *fileName;
  int version;
  {
   int i, j, k;
   struct fuzzyValueHashNode *hashPtr;
   int count;
   int numberOfEntries;
   struct fuzzyValueHashNode **fuzzyValueTable;
   int newHeader = TRUE;
   int arrayVersion = 1;
   FILE *fp;
   int arraySize;

   /*====================================*/
   /* Count the total number of entries. */
   /*  i.e. total x and y values to be   */
   /*       written                      */
   /*====================================*/

   fuzzyValueTable = GetFuzzyValueTable();
   count = numberOfEntries = 0;

   for (i = 0; i < FUZZY_VALUE_HASH_SIZE; i++)
     {
      hashPtr = fuzzyValueTable[i];
      while (hashPtr != NULL)
        {
                 arraySize = hashPtr->contents->n;
         numberOfEntries += arraySize + arraySize;
         hashPtr = hashPtr->next;
        }
     }

   if (numberOfEntries == 0) return(version);

   for (i = 1; i <= (numberOfEntries / MaxIndices) + 1 ; i++)
     { fprintf(HeaderFP,"extern double X%d_%d[];\n",ImageID,i); }

   /*==================*/
   /* Create the file. */
   /*==================*/

   if ((fp = NewCFile(fileName,1,version,FALSE)) == NULL) return(-1);

   /*=====================*/
   /* Fillin the entries. */
   /*=====================*/

   j = 0;

   for (i = 0; i < FUZZY_VALUE_HASH_SIZE; i++)
     {
      hashPtr = fuzzyValueTable[i];
      while (hashPtr != NULL)
        {
         if (newHeader)
           {
            fprintf(fp,"double X%d_%d[] = {\n",ImageID,arrayVersion);
            newHeader = FALSE;
           }

         /* write out the x values then y values for each fuzzy value */

                 arraySize = hashPtr->contents->n;
                 for (k=0; k<arraySize-1; k++)
            fprintf(fp,"%s,", FloatToString(hashPtr->contents->x[k]));
         fprintf(fp,"%s,\n", FloatToString(hashPtr->contents->x[arraySize-1]));
                 for (k=0; k<arraySize-1; k++)
            fprintf(fp,"%s,", FloatToString(hashPtr->contents->y[k]));
         fprintf(fp,"%s", FloatToString(hashPtr->contents->y[arraySize-1]));

         count += arraySize + arraySize;
                 j += arraySize + arraySize;

         if ((count == numberOfEntries) || (j >= MaxIndices))
           {
            fprintf(fp,"};\n");
            fclose(fp);
            j = 0;
            arrayVersion++;
            version++;
            if (count < numberOfEntries)
              {
               if ((fp = NewCFile(fileName,1,version,FALSE)) == NULL) return(0);
               newHeader = TRUE;
              }
           }
         else
           { fprintf(fp,",\n"); }

         hashPtr = hashPtr->next;
        }
     }

   return(version);
  }

#endif /* #if FUZZY_DEFTEMPLATES */


/*****************************************************/
/* SymbolHashNodesToCode: Produces the code for the  */
/*   symbol hash table entries for a run-time module */
/*   created using the constructs-to-c function.     */
/*****************************************************/
static int SymbolHashNodesToCode(
  char *fileName,
  int version)
  {
   int i, j;
   struct symbolHashNode *hashPtr;
   int count;
   int numberOfEntries;
   struct symbolHashNode **symbolTable;
   int newHeader = TRUE;
   int arrayVersion = 1;
   FILE *fp;

   /*====================================*/
   /* Count the total number of entries. */
   /*====================================*/

   symbolTable = GetSymbolTable();
   count = numberOfEntries = 0;

   for (i = 0; i < SYMBOL_HASH_SIZE; i++)
     {
      for (hashPtr = symbolTable[i];
           hashPtr != NULL;
           hashPtr = hashPtr->next)
        { numberOfEntries++; }
     }

   if (numberOfEntries == 0) return(version);

   for (i = 1; i <= (numberOfEntries / MaxIndices) + 1 ; i++)
     { fprintf(HeaderFP,"extern struct symbolHashNode S%d_%d[];\n",ImageID,i); }

   /*==================*/
   /* Create the file. */
   /*==================*/

   if ((fp = NewCFile(fileName,1,version,FALSE)) == NULL) return(-1);

   /*===================*/
   /* List the entries. */
   /*===================*/

   j = 0;

   for (i = 0; i < SYMBOL_HASH_SIZE; i++)
     {
      for (hashPtr = symbolTable[i];
           hashPtr != NULL;
           hashPtr = hashPtr->next)
        {
         if (newHeader)
           {
            fprintf(fp,"struct symbolHashNode S%d_%d[] = {\n",ImageID,arrayVersion);
            newHeader = FALSE;
           }

         if (hashPtr->next == NULL)
           { fprintf(fp,"{NULL,"); }
         else
           {
            if ((j + 1) >= MaxIndices)
              { fprintf(fp,"{&S%d_%d[%d],",ImageID,arrayVersion + 1,0); }
            else
              { fprintf(fp,"{&S%d_%d[%d],",ImageID,arrayVersion,j + 1); }
           }

         fprintf(fp,"%ld,0,0,0,%d,",hashPtr->count + 1,i);
         PrintCString(fp,hashPtr->contents);

         count++;
         j++;

         if ((count == numberOfEntries) || (j >= MaxIndices))
           {
            fprintf(fp,"}};\n");
            fclose(fp);
            j = 0;
            arrayVersion++;
            version++;
            if (count < numberOfEntries)
              {
               if ((fp = NewCFile(fileName,1,version,FALSE)) == NULL) return(0);
               newHeader = TRUE;
              }
           }
         else
           { fprintf(fp,"},\n"); }
        }
     }

   return(version);
  }

/******************************************************/
/* BitMapHashNodesToCode: Produces the code for the   */
/*   bit map hash table entries for a run-time module */
/*   created using the constructs-to-c function.      */
/******************************************************/
static int BitMapHashNodesToCode(
  char *fileName,
  int version)
  {
   int i, j;
   struct bitMapHashNode *hashPtr;
   int count;
   int numberOfEntries;
   struct bitMapHashNode **bitMapTable;
   int newHeader = TRUE;
   int arrayVersion = 1;
   FILE *fp;
   int longsReqdPartition = 1,longsReqdPartitionCount = 0;

   /*====================================*/
   /* Count the total number of entries. */
   /*====================================*/

   bitMapTable = GetBitMapTable();
   count = numberOfEntries = 0;

   for (i = 0; i < BITMAP_HASH_SIZE; i++)
     {
      for (hashPtr = bitMapTable[i];
           hashPtr != NULL;
           hashPtr = hashPtr->next)
        { numberOfEntries++; }
     }

   if (numberOfEntries == 0) return(version);

   for (i = 1; i <= (numberOfEntries / MaxIndices) + 1 ; i++)
     { fprintf(HeaderFP,"extern struct bitMapHashNode B%d_%d[];\n",ImageID,i); }

   /*==================*/
   /* Create the file. */
   /*==================*/

   if ((fp = NewCFile(fileName,1,version,FALSE)) == NULL) return(-1);

   /*===================*/
   /* List the entries. */
   /*===================*/

   j = 0;

   for (i = 0; i < BITMAP_HASH_SIZE; i++)
     {
      for (hashPtr = bitMapTable[i];
           hashPtr != NULL;
           hashPtr = hashPtr->next)
        {
         if (newHeader)
           {
            fprintf(fp,"struct bitMapHashNode B%d_%d[] = {\n",ImageID,arrayVersion);
            newHeader = FALSE;
           }

         if (hashPtr->next == NULL)
           { fprintf(fp,"{NULL,"); }
         else
           {
            if ((j + 1) >= MaxIndices)
              { fprintf(fp,"{&B%d_%d[%d],",ImageID,arrayVersion + 1,0); }
            else
              { fprintf(fp,"{&B%d_%d[%d],",ImageID,arrayVersion,j + 1); }
           }

         fprintf(fp,"%ld,0,0,0,%d,(char *) &L%d_%d[%d],%d",
                     hashPtr->count + 1,i,
                     ImageID,longsReqdPartition,longsReqdPartitionCount,
                     hashPtr->size);

         longsReqdPartitionCount += (int) (hashPtr->size / sizeof(unsigned long));
         if ((hashPtr->size % sizeof(unsigned long)) != 0)
           longsReqdPartitionCount++;
         if (longsReqdPartitionCount >= MaxIndices)
           {
            longsReqdPartitionCount = 0;
            longsReqdPartition++;
           }

         count++;
         j++;

         if ((count == numberOfEntries) || (j >= MaxIndices))
           {
            fprintf(fp,"}};\n");
            fclose(fp);
            j = 0;
            arrayVersion++;
            version++;
            if (count < numberOfEntries)
              {
               if ((fp = NewCFile(fileName,1,version,FALSE)) == NULL) return(0);
               newHeader = TRUE;
              }
           }
         else
           { fprintf(fp,"},\n"); }
        }
     }

   return(version);
  }

/*****************************************************/
/* BitMapValuesToCode: Produces the code for the bit */
/*   map strings for a run-time module created using */
/*   the constructs-to-c function.                   */
/*****************************************************/
static int BitMapValuesToCode(
  char *fileName,
  int version)
  {
   int i, j, k, l;
   struct bitMapHashNode *hashPtr;
   int count;
   int numberOfEntries;
   struct bitMapHashNode **bitMapTable;
   int newHeader = TRUE;
   int arrayVersion = 1;
   FILE *fp;
   unsigned long tmpLong;
   int longsReqd;

   /*====================================*/
   /* Count the total number of entries. */
   /*====================================*/

   bitMapTable = GetBitMapTable();
   count = numberOfEntries = 0;

   for (i = 0; i < BITMAP_HASH_SIZE; i++)
     {
      for (hashPtr = bitMapTable[i];
           hashPtr != NULL;
           hashPtr = hashPtr->next)
        {
         numberOfEntries += (int) (hashPtr->size / sizeof(unsigned long));
         if ((hashPtr->size % sizeof(unsigned long)) != 0)
           { numberOfEntries++; }
        }
     }

   if (numberOfEntries == 0) return(version);

   for (i = 1; i <= (numberOfEntries / MaxIndices) + 1 ; i++)
     { fprintf(HeaderFP,"extern unsigned long L%d_%d[];\n",ImageID,i); }

   /*==================*/
   /* Create the file. */
   /*==================*/

   if ((fp = NewCFile(fileName,1,version,FALSE)) == NULL) return(-1);

   /*===================*/
   /* List the entries. */
   /*===================*/

   j = 0;

   for (i = 0; i < BITMAP_HASH_SIZE; i++)
     {
      for (hashPtr = bitMapTable[i];
           hashPtr != NULL;
           hashPtr = hashPtr->next)
        {
         if (newHeader)
           {
            fprintf(fp,"unsigned long L%d_%d[] = {\n",ImageID,arrayVersion);
            newHeader = FALSE;
           }

         longsReqd = (int) (hashPtr->size / sizeof(unsigned long));
         if ((hashPtr->size % sizeof(unsigned long)) != 0)
           longsReqd++;

         for (k = 0 ; k < longsReqd ; k++)
           {
            if (k > 0)
              fprintf(fp,",");
            tmpLong = 0L;
            for (l = 0 ;
                 ((l < sizeof(unsigned long)) &&
                 (((k * sizeof(unsigned long)) + l) < hashPtr->size)) ;
                 l++)
              ((char *) &tmpLong)[l] = hashPtr->contents[(k * sizeof(unsigned long)) + l];
            fprintf(fp,"0x%lxL",tmpLong);
           }

         count += longsReqd;
         j += longsReqd;

         if ((count == numberOfEntries) || (j >= MaxIndices))
           {
            fprintf(fp,"};\n");
            fclose(fp);
            j = 0;
            arrayVersion++;
            version++;
            if (count < numberOfEntries)
              {
               if ((fp = NewCFile(fileName,1,version,FALSE)) == NULL) return(0);
               newHeader = TRUE;
              }
           }
         else
           { fprintf(fp,",\n"); }
        }
     }

   return(version);
  }

/****************************************************/
/* FloatHashNodesToCode: Produces the code for the  */
/*   float hash table entries for a run-time module */
/*   created using the constructs-to-c function.    */
/****************************************************/
static int FloatHashNodesToCode(
  char *fileName,
  int version)
  {
   int i, j;
   struct floatHashNode *hashPtr;
   int count;
   int numberOfEntries;
   struct floatHashNode **floatTable;
   int newHeader = TRUE;
   FILE *fp;
   int arrayVersion = 1;

   /*====================================*/
   /* Count the total number of entries. */
   /*====================================*/

   floatTable = GetFloatTable();
   count = numberOfEntries = 0;

   for (i = 0; i < FLOAT_HASH_SIZE; i++)
     {
      for (hashPtr = floatTable[i];
           hashPtr != NULL;
           hashPtr = hashPtr->next)
        { numberOfEntries++; }
     }

   if (numberOfEntries == 0) return(version);

   for (i = 1; i <= (numberOfEntries / MaxIndices) + 1 ; i++)
     { fprintf(HeaderFP,"extern struct floatHashNode F%d_%d[];\n",ImageID,i); }

   /*==================*/
   /* Create the file. */
   /*==================*/

   if ((fp = NewCFile(fileName,1,version,FALSE)) == NULL) return(-1);

   /*===================*/
   /* List the entries. */
   /*===================*/

   j = 0;

   for (i = 0; i < FLOAT_HASH_SIZE; i++)
     {
      for (hashPtr = floatTable[i];
           hashPtr != NULL;
           hashPtr = hashPtr->next)
        {
         if (newHeader)
           {
            fprintf(fp,"struct floatHashNode F%d_%d[] = {\n",ImageID,arrayVersion);
            newHeader = FALSE;
           }

         if (hashPtr->next == NULL)
           { fprintf(fp,"{NULL,"); }
         else
           {
            if ((j + 1) >= MaxIndices)
              { fprintf(fp,"{&F%d_%d[%d],",ImageID,arrayVersion + 1,0); }
            else
              { fprintf(fp,"{&F%d_%d[%d],",ImageID,arrayVersion,j + 1); }
           }

         fprintf(fp,"%ld,0,0,0,%d,",hashPtr->count + 1,i);
         fprintf(fp,"%s",FloatToString(hashPtr->contents));

         count++;
         j++;

         if ((count == numberOfEntries) || (j >= MaxIndices))
           {
            fprintf(fp,"}};\n");
            fclose(fp);
            j = 0;
            version++;
            arrayVersion++;
            if (count < numberOfEntries)
              {
               if ((fp = NewCFile(fileName,1,version,FALSE)) == NULL) return(0);
               newHeader = TRUE;
              }
           }
         else
           { fprintf(fp,"},\n"); }
        }
     }

   return(version);
  }

/******************************************************/
/* IntegerHashNodesToCode: Produces the code for the  */
/*   integer hash table entries for a run-time module */
/*   created using the constructs-to-c function.      */
/******************************************************/
static int IntegerHashNodesToCode(
  char *fileName,
  int version)
  {
   int i, j;
   struct integerHashNode *hashPtr;
   int count;
   int numberOfEntries;
   struct integerHashNode **integerTable;
   int newHeader = TRUE;
   FILE *fp;
   int arrayVersion = 1;

   /*====================================*/
   /* Count the total number of entries. */
   /*====================================*/

   integerTable = GetIntegerTable();
   count = numberOfEntries = 0;

   for (i = 0; i < INTEGER_HASH_SIZE; i++)
     {
      for (hashPtr = integerTable[i];
           hashPtr != NULL;
           hashPtr = hashPtr->next)
        { numberOfEntries++; }
     }

   if (numberOfEntries == 0) return(version);

   for (i = 1; i <= (numberOfEntries / MaxIndices) + 1 ; i++)
     { fprintf(HeaderFP,"extern struct integerHashNode I%d_%d[];\n",ImageID,i); }

   /*==================*/
   /* Create the file. */
   /*==================*/

   if ((fp = NewCFile(fileName,1,version,FALSE)) == NULL) return(-1);

   /*===================*/
   /* List the entries. */
   /*===================*/

   j = 0;

   for (i = 0; i < INTEGER_HASH_SIZE; i++)
     {
      for (hashPtr = integerTable[i];
           hashPtr != NULL;
           hashPtr = hashPtr->next)
        {
         if (newHeader)
           {
            fprintf(fp,"struct integerHashNode I%d_%d[] = {\n",ImageID,arrayVersion);
            newHeader = FALSE;
           }

         if (hashPtr->next == NULL)
           { fprintf(fp,"{NULL,"); }
         else
           {
            if ((j + 1) >= MaxIndices)
              { fprintf(fp,"{&I%d_%d[%d],",ImageID,arrayVersion + 1,0); }
            else
              { fprintf(fp,"{&I%d_%d[%d],",ImageID,arrayVersion,j + 1); }
           }

         fprintf(fp,"%ld,0,0,0,%d,",hashPtr->count + 1,i);
         fprintf(fp,"%ld",hashPtr->contents);

         count++;
         j++;

         if ((count == numberOfEntries) || (j >= MaxIndices))
           {
            fprintf(fp,"}};\n");
            fclose(fp);
            j = 0;
            version++;
            arrayVersion++;
            if (count < numberOfEntries)
              {
               if ((fp = NewCFile(fileName,1,version,FALSE)) == NULL) return(0);
               newHeader = TRUE;
              }
           }
         else
           { fprintf(fp,"},\n"); }
        }
     }

   return(version);
  }

/****************************************************************/
/* HashTablesToCode: Produces the code for the symbol, integer, */
/*   float, and bitmap hash tables for a run-time module        */
/*   created using the constructs-to-c function.                */
/****************************************************************/
static int HashTablesToCode(
  char *fileName)
  {
   int i;
   FILE *fp;
   struct symbolHashNode **symbolTable;
   struct floatHashNode **floatTable;
   struct integerHashNode **integerTable;
   struct bitMapHashNode **bitMapTable;
#if FUZZY_DEFTEMPLATES    
   struct fuzzyValueHashNode **fuzzyValueTable;
#endif

   /*======================================*/
   /* Write the code for the symbol table. */
   /*======================================*/

   symbolTable = GetSymbolTable();

   if ((fp = NewCFile(fileName,1,1,FALSE)) == NULL) return(0);

   fprintf(HeaderFP,"extern struct symbolHashNode *sht%d[];\n",ImageID);
   fprintf(fp,"struct symbolHashNode *sht%d[%d] = {\n",ImageID,SYMBOL_HASH_SIZE);

   for (i = 0; i < SYMBOL_HASH_SIZE; i++)
      {
       PrintSymbolReference(fp,symbolTable[i]);

       if (i + 1 != SYMBOL_HASH_SIZE) fprintf(fp,",\n");
      }

    fprintf(fp,"};\n");

    fclose(fp);

   /*=====================================*/
   /* Write the code for the float table. */
   /*=====================================*/

   floatTable = GetFloatTable();

   if ((fp = NewCFile(fileName,1,2,FALSE)) == NULL) return(0);

   fprintf(HeaderFP,"extern struct floatHashNode *fht%d[];\n",ImageID);
   fprintf(fp,"struct floatHashNode *fht%d[%d] = {\n",ImageID,FLOAT_HASH_SIZE);

   for (i = 0; i < FLOAT_HASH_SIZE; i++)
      {
       if (floatTable[i] == NULL) { fprintf(fp,"NULL"); }
       else PrintFloatReference(fp,floatTable[i]);

       if (i + 1 != FLOAT_HASH_SIZE) fprintf(fp,",\n");
      }

    fprintf(fp,"};\n");

    fclose(fp);

   /*=======================================*/
   /* Write the code for the integer table. */
   /*=======================================*/

   integerTable = GetIntegerTable();

   if ((fp = NewCFile(fileName,1,3,FALSE)) == NULL) return(0);

   fprintf(HeaderFP,"extern struct integerHashNode *iht%d[];\n",ImageID);
   fprintf(fp,"struct integerHashNode *iht%d[%d] = {\n",ImageID,INTEGER_HASH_SIZE);

   for (i = 0; i < INTEGER_HASH_SIZE; i++)
      {
       if (integerTable[i] == NULL) { fprintf(fp,"NULL"); }
       else PrintIntegerReference(fp,integerTable[i]);

       if (i + 1 != INTEGER_HASH_SIZE) fprintf(fp,",\n");
      }

    fprintf(fp,"};\n");

    fclose(fp);

   /*======================================*/
   /* Write the code for the bitmap table. */
   /*======================================*/

   bitMapTable = GetBitMapTable();

   if ((fp = NewCFile(fileName,1,4,FALSE)) == NULL) return(0);

   fprintf(HeaderFP,"extern struct bitMapHashNode *bmht%d[];\n",ImageID);
   fprintf(fp,"struct bitMapHashNode *bmht%d[%d] = {\n",ImageID,BITMAP_HASH_SIZE);

   for (i = 0; i < BITMAP_HASH_SIZE; i++)
      {
       PrintBitMapReference(fp,bitMapTable[i]);

       if (i + 1 != BITMAP_HASH_SIZE) fprintf(fp,",\n");
      }

    fprintf(fp,"};\n");

    fclose(fp);

#if FUZZY_DEFTEMPLATES   

   /*=============================*/
   /* Dump the fuzzy value table. */
   /*=============================*/

   fuzzyValueTable = GetFuzzyValueTable();

   if ((fp = NewCFile(fileName,1,5,FALSE)) == NULL) return(0);

   fprintf(HeaderFP,"extern struct fuzzyValueHashNode *fvht%d[];\n",ImageID);
   fprintf(fp,"struct fuzzyValueHashNode *fvht%d[%d] = {\n",ImageID,FUZZY_VALUE_HASH_SIZE);

   for (i = 0; i < FUZZY_VALUE_HASH_SIZE; i++)
      {
       PrintFuzzyValueReference(fp,fuzzyValueTable[i]);

       if (i + 1 != FUZZY_VALUE_HASH_SIZE) fprintf(fp,",\n");
      }

    fprintf(fp,"};\n");

    fclose(fp);

#endif /* FUZZY_DEFTEMPLATES */

    return(1);
   }

#if FUZZY_DEFTEMPLATES    

/************************************************************/
/* PrintFuzzyValueReference:                                */
/************************************************************/
globle VOID PrintFuzzyValueReference(fp,fvPtr)
  FILE *fp;
  struct fuzzyValueHashNode *fvPtr;
  {
   if (fvPtr == NULL) fprintf(fp,"NULL");
   else fprintf(fp,"&V%d_%d[%d]",ImageID,
                                 ((unsigned int)fvPtr->bucket / MaxIndices) + 1,
                                  (unsigned int)fvPtr->bucket % MaxIndices);
  }

#endif  /* FUZZY_DEFTEMPLATES */

/*****************************************************/
/* PrintSymbolReference: Prints the C code reference */
/*   address to the specified symbol (also used for  */
/*   strings and instance names).                    */
/*****************************************************/
globle void PrintSymbolReference(
  FILE *theFile,
  struct symbolHashNode *theSymbol)
  {
   if (theSymbol == NULL) fprintf(theFile,"NULL");
   else fprintf(theFile,"&S%d_%d[%d]",
                        ImageID,
                        ((int) theSymbol->bucket / MaxIndices) + 1,
                        (int) theSymbol->bucket % MaxIndices);
  }

/****************************************************/
/* PrintFloatReference: Prints the C code reference */
/*   address to the specified float.                */
/****************************************************/
globle void PrintFloatReference(
  FILE *theFile,
  struct floatHashNode *theFloat)
  {
   fprintf(theFile,"&F%d_%d[%d]",
                   ImageID,
                   ((int) theFloat->bucket / MaxIndices) + 1,
                   (int) theFloat->bucket % MaxIndices);
  }

/******************************************************/
/* PrintIntegerReference: Prints the C code reference */
/*   address to the specified integer.                */
/******************************************************/
globle void PrintIntegerReference(
  FILE *theFile,
  struct integerHashNode *theInteger)
  {
   fprintf(theFile,"&I%d_%d[%d]",
                   ImageID,
                   ((int) theInteger->bucket / MaxIndices) + 1,
                   (int) theInteger->bucket % MaxIndices);
  }

/*****************************************************/
/* PrintBitMapReference: Prints the C code reference */
/*   address to the specified bit map.               */
/*****************************************************/
globle void PrintBitMapReference(
  FILE *theFile,
  struct bitMapHashNode *theBitMap)
  {
   if (theBitMap == NULL) fprintf(theFile,"NULL");
   else fprintf(theFile,"&B%d_%d[%d]",
                        ImageID,
                        ((int) theBitMap->bucket / MaxIndices) + 1,
                        (int) theBitMap->bucket % MaxIndices);
  }

/*********************************************************/
/* PrintCString: Prints KB strings in the appropriate    */
/*   format for C (the " and \ characters are preceeded  */
/*   by a \ and carriage returns are replaced with \n).  */
/*********************************************************/
static void PrintCString(
  FILE *theFile,
  char *str)
  {
   int i, slen;

   /*============================================*/
   /* Print the string's opening quotation mark. */
   /*============================================*/

   fprintf(theFile,"\"");

   /*===============================================*/
   /* Convert and write each character to the file. */
   /*===============================================*/

   slen = strlen(str);
   for (i = 0 ; i < slen ; i++)
     {
      /*====================================*/
      /* Preceed " and \ with the \ escape. */
      /*====================================*/

      if ((str[i] == '"') || (str[i] == '\\'))
        {
         fputc('\\',theFile);
         fputc(str[i],theFile);
        }

      /*====================================*/
      /* Replace a carriage return with \n. */
      /*====================================*/

      else if (str[i] == '\n')
        {
         fputc('\\',theFile);
         fputc('n',theFile);
        }

      /*===============================*/
      /* All other characters can be   */
      /* printed without modification. */
      /*===============================*/

      else
        { fputc(str[i],theFile); }
     }

   /*============================================*/
   /* Print the string's closing quotation mark. */
   /*============================================*/

   fprintf(theFile,"\"");
  }

#endif /* CONSTRUCT_COMPILER && (! RUN_TIME) */
