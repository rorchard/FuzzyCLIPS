static char rcsid[] = "$Header: /dist/CVS/fzclips/src/tmpltbin.c,v 1.3 2001/08/11 21:08:09 dave Exp $" ;

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*            DEFTEMPLATE BSAVE/BLOAD MODULE           */
   /*******************************************************/

/*************************************************************/
/* Purpose: Implements the binary save/load feature for the  */
/*    deftemplate construct.                                 */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*      Brian L. Donnell                                     */
/*      Bob Orchard (NRCC - Nat'l Research Council of Canada)*/
/*                  (Fuzzy reasoning extensions)             */
/*                  (certainty factors for facts and rules)  */
/*                  (extensions to run command)              */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#define  _TMPLTBIN_SOURCE_

#include "setup.h"

#if DEFTEMPLATE_CONSTRUCT && (BLOAD || BLOAD_ONLY || BLOAD_AND_BSAVE) && (! RUN_TIME)

#include <stdio.h>
#define _STDIO_INCLUDED_

#include "memalloc.h"
#include "bload.h"
#include "bsave.h"
#include "factbin.h"
#include "cstrnbin.h"
#include "factmngr.h"
#include "tmpltpsr.h"
#include "tmpltdef.h"
#include "tmpltutl.h"

#include "tmpltbin.h"

#if FUZZY_DEFTEMPLATES 
#include "fuzzylv.h"
#include "fuzzyval.h"
#include "dffnxbin.h"
#endif

/****************************************/
/* GLOBAL INTERNAL VARIABLE DEFINITIONS */
/****************************************/

   globle struct deftemplate                  *DeftemplateArray;

/***************************************/
/* LOCAL INTERNAL VARIABLE DEFINITIONS */
/***************************************/

   static long                                 NumberOfDeftemplates;
   static long                                 NumberOfTemplateSlots;
   static long                                 NumberOfTemplateModules;
#if FUZZY_DEFTEMPLATES  
   static long                                 NumberOfFuzzyTemplates;
   static long                                 NumberOfFuzzyPrimaryTerms;
   static long                                 NumberOfFuzzyModifiers;
#endif
   static struct templateSlot                 *SlotArray;
   static struct deftemplateModule            *ModuleArray;

#if FUZZY_DEFTEMPLATES   
   static struct fuzzyLv                      *LvPlusUniverseArray;
   static struct primary_term                 *PrimaryTermArray;
#endif

/***************************************/
/* LOCAL INTERNAL FUNCTION DEFINITIONS */
/***************************************/

#if BLOAD_AND_BSAVE
   static void                    BsaveFind(void);
   static void                    BsaveStorage(FILE *);
   static void                    BsaveBinaryItem(FILE *);
#endif
   static void                    BloadStorage(void);
   static void                    BloadBinaryItem(void);
   static void                    UpdateDeftemplateModule(void *,long);
   static void                    UpdateDeftemplate(void *,long);
   static void                    UpdateDeftemplateSlot(void *,long);
#if FUZZY_DEFTEMPLATES   
   static void                    UpdateLvPlusUniverse(void *,long);
   static void                    UpdateFuzzyPrimaryTerms(void *,long);
#endif
   static void                    ClearBload(void);

/***********************************************/
/* DeftemplateBinarySetup: Installs the binary */
/*   save/load feature for deftemplates.       */
/***********************************************/
globle void DeftemplateBinarySetup()
  {
#if BLOAD_AND_BSAVE
   AddBinaryItem("deftemplate",0,BsaveFind,NULL,
                             BsaveStorage,BsaveBinaryItem,
                             BloadStorage,BloadBinaryItem,
                             ClearBload);
#endif
#if (BLOAD || BLOAD_ONLY)
   AddBinaryItem("deftemplate",0,NULL,NULL,NULL,NULL,
                             BloadStorage,BloadBinaryItem,
                             ClearBload);
#endif
  }

#if BLOAD_AND_BSAVE

/**************************************************************/
/* BsaveFind: Counts the number of data structures which must */
/*   be saved in the binary image for the deftemplates in the */
/*   current environment.                                     */
/**************************************************************/
static void BsaveFind()
  {
   struct deftemplate *theDeftemplate;
   struct templateSlot *theSlot;
   struct defmodule *theModule;

   /*=======================================================*/
   /* If a binary image is already loaded, then temporarily */
   /* save the count values since these will be overwritten */
   /* in the process of saving the binary image.            */
   /*=======================================================*/

   if (Bloaded())
     {
      SaveBloadCount(NumberOfDeftemplates);
      SaveBloadCount(NumberOfTemplateSlots);
      SaveBloadCount(NumberOfTemplateModules);
#if FUZZY_DEFTEMPLATES 
      SaveBloadCount(NumberOfFuzzyTemplates);
      SaveBloadCount(NumberOfFuzzyPrimaryTerms);
#endif
     }

   /*==================================================*/
   /* Set the count of deftemplates, deftemplate slots */
   /* and deftemplate module data structures to zero.  */
   /*==================================================*/

   NumberOfDeftemplates = 0;
   NumberOfTemplateSlots = 0;
   NumberOfTemplateModules = 0;

#if FUZZY_DEFTEMPLATES   
   NumberOfFuzzyTemplates = 0;
   NumberOfFuzzyPrimaryTerms = 0;
#endif

   /*===========================*/
   /* Loop through each module. */
   /*===========================*/

   for (theModule = (struct defmodule *) GetNextDefmodule(NULL);
        theModule != NULL;
        theModule = (struct defmodule *) GetNextDefmodule(theModule))
     {
      /*============================================*/
      /* Set the current module to the module being */
      /* examined and increment the number of       */
      /* deftemplate modules encountered.           */
      /*============================================*/

      SetCurrentModule((void *) theModule);
      NumberOfTemplateModules++;

      /*======================================================*/
      /* Loop through each deftemplate in the current module. */
      /*======================================================*/

      for (theDeftemplate = (struct deftemplate *) GetNextDeftemplate(NULL);
           theDeftemplate != NULL;
           theDeftemplate = (struct deftemplate *) GetNextDeftemplate(theDeftemplate))
        {
         /*======================================================*/
         /* Initialize the construct header for the binary save. */
         /*======================================================*/

         MarkConstructHeaderNeededItems(&theDeftemplate->header,
                                        NumberOfDeftemplates++);

#if FUZZY_DEFTEMPLATES  
         if (theDeftemplate->fuzzyTemplate != NULL)
           {
             struct primary_term *ptptr;

             /* fuzzy template -- deal with the fuzzyLv (linguistic value)
                which contains the universe of discourse, and the list of
                primary terms
                Count the number of fuzzy lv's and the number of primary terms
                ... also mark the fuzzy values and symbols (fv names, unit names)
                as 'needed' for bsave
             */
             NumberOfFuzzyTemplates++;

             if (theDeftemplate->fuzzyTemplate->units != NULL)
                theDeftemplate->fuzzyTemplate->units->neededSymbol = TRUE;

             ptptr = theDeftemplate->fuzzyTemplate->primary_term_list;
             while (ptptr != NULL)
                {
                  NumberOfFuzzyPrimaryTerms++;
                  ptptr->fuzzy_value_description->neededFuzzyValue = TRUE;
                  ptptr = ptptr->next;
                }
            }
#endif
         /*=============================================================*/
         /* Loop through each slot in the deftemplate, incrementing the */
         /* slot count and marking the slot names as needed symbols.    */
         /*=============================================================*/

         for (theSlot = theDeftemplate->slotList;
              theSlot != NULL;
              theSlot = theSlot->next)
           {
            NumberOfTemplateSlots++;
            theSlot->slotName->neededSymbol = TRUE;
           }
        }

     }
  }

/*********************************************************/
/* BsaveStorage: Writes out the storage requirements for */
/*    all deftemplate structures to the binary file.     */
/*********************************************************/
static void BsaveStorage(
  FILE *fp)
  {
   unsigned long space;

   /*========================================================================*/
   /* Three data structures are saved as part of a deftemplate binary image: */
   /* the deftemplate data structure, the deftemplateModule data structure,  */
   /* and the templateSlot data structure. The data structures associated    */
   /* with default values and constraints are not save with the deftemplate  */
   /* portion of the binary image.                                           */
   /*========================================================================*/

#if ! FUZZY_DEFTEMPLATES
   space = sizeof(long) * 3;
#else
   space = sizeof(long) * 5;
#endif
   GenWrite(&space,(unsigned long) sizeof(long int),fp);
   GenWrite(&NumberOfDeftemplates,(unsigned long) sizeof(long int),fp);
   GenWrite(&NumberOfTemplateSlots,(unsigned long) sizeof(long int),fp);
   GenWrite(&NumberOfTemplateModules,(unsigned long) sizeof(long int),fp);
#if FUZZY_DEFTEMPLATES   
   GenWrite(&NumberOfFuzzyTemplates,(unsigned long) sizeof(long int),fp);
   GenWrite(&NumberOfFuzzyPrimaryTerms,(unsigned long) sizeof(long int),fp);
#endif
  }

/***********************************************/
/* BsaveBinaryItem: Writes out all deftemplate */
/*   structures to the binary file.            */
/***********************************************/
static void BsaveBinaryItem(
  FILE *fp)
  {
   unsigned long space;
   struct deftemplate *theDeftemplate;
   struct bsaveDeftemplate tempDeftemplate;
   struct templateSlot *theSlot;
   struct bsaveTemplateSlot tempTemplateSlot;
   struct bsaveDeftemplateModule tempTemplateModule;
   struct defmodule *theModule;
   struct deftemplateModule *theModuleItem;
#if FUZZY_DEFTEMPLATES  
   long lastNumberOfFuzzyPrimaryTerms;
#endif
   /*============================================================*/
   /* Write out the amount of space taken up by the deftemplate, */
   /* deftemplateModule, and templateSlot data structures in the */
   /* binary image.                                              */
   /*============================================================*/

   space = (NumberOfDeftemplates * sizeof(struct bsaveDeftemplate)) +
           (NumberOfTemplateSlots * sizeof(struct bsaveTemplateSlot)) +
#if FUZZY_DEFTEMPLATES  
           (NumberOfFuzzyTemplates * sizeof(struct bsaveLvPlusUniverse)) +
           (NumberOfFuzzyPrimaryTerms * sizeof(struct bsaveFuzzyPrimaryTerm)) +
#endif
           (NumberOfTemplateModules * sizeof(struct bsaveDeftemplateModule));
   GenWrite(&space,(unsigned long) sizeof(unsigned long int),fp);

   /*===================================================*/
   /* Write out each deftemplate module data structure. */
   /*===================================================*/

   NumberOfDeftemplates = 0;
#if FUZZY_DEFTEMPLATES   
   NumberOfFuzzyTemplates = 0;
   NumberOfFuzzyPrimaryTerms = 0;
   lastNumberOfFuzzyPrimaryTerms = 0;
#endif
   for (theModule = (struct defmodule *) GetNextDefmodule(NULL);
        theModule != NULL;
        theModule = (struct defmodule *) GetNextDefmodule(theModule))
     {
      SetCurrentModule((void *) theModule);

      theModuleItem = (struct deftemplateModule *)
                      GetModuleItem(NULL,FindModuleItem("deftemplate")->moduleIndex);
      AssignBsaveDefmdlItemHdrVals(&tempTemplateModule.header,
                                           &theModuleItem->header);
      GenWrite(&tempTemplateModule,(unsigned long) sizeof(struct bsaveDeftemplateModule),fp);
     }

   /*============================================*/
   /* Write out each deftemplate data structure. */
   /*============================================*/

   NumberOfTemplateSlots = 0;
   for (theModule = (struct defmodule *) GetNextDefmodule(NULL);
        theModule != NULL;
        theModule = (struct defmodule *) GetNextDefmodule(theModule))
     {
      SetCurrentModule((void *) theModule);

      for (theDeftemplate = (struct deftemplate *) GetNextDeftemplate(NULL);
           theDeftemplate != NULL;
           theDeftemplate = (struct deftemplate *) GetNextDeftemplate(theDeftemplate))
        {
         AssignBsaveConstructHeaderVals(&tempDeftemplate.header,
                                          &theDeftemplate->header);
         tempDeftemplate.implied = theDeftemplate->implied;
         tempDeftemplate.numberOfSlots = theDeftemplate->numberOfSlots;
         tempDeftemplate.patternNetwork = BsaveFactPatternIndex(theDeftemplate->patternNetwork);

#if FUZZY_DEFTEMPLATES   
         /* a -1 in fuzzyTemplateList slot of tempDeftemplate indicates
                    NOT a fuzzy deftemplate -- other values give index into the
                        LvPlusUniverseArray for resolving ptr when read back in
             */
         if (theDeftemplate->fuzzyTemplate == NULL)
             tempDeftemplate.fuzzyTemplateList = -1L;
         else
                     tempDeftemplate.fuzzyTemplateList = NumberOfFuzzyTemplates;
         tempDeftemplate.hasFuzzySlots = theDeftemplate->hasFuzzySlots;
#endif

         if (theDeftemplate->slotList != NULL)
           { tempDeftemplate.slotList = NumberOfTemplateSlots; }
         else tempDeftemplate.slotList = -1L;

         GenWrite(&tempDeftemplate,(unsigned long) sizeof(struct bsaveDeftemplate),fp);

#if FUZZY_DEFTEMPLATES    
         if (theDeftemplate->fuzzyTemplate != NULL)
                    NumberOfFuzzyTemplates++;
#endif

         NumberOfTemplateSlots += theDeftemplate->numberOfSlots;
        }
     }

#if FUZZY_DEFTEMPLATES   
   /* At this point the bsave templates are written out .. now append
      to that for any fuzzy deftemplates

             1. write out the Lv with universe of discourse
             2. write out the list of primary terms

          all fuzzyLv structs written, then all primary terms
   */

   if (NumberOfFuzzyTemplates > 0)
    {
     theModule = (struct defmodule *) GetNextDefmodule(NULL);
     while (theModule != NULL)
       {
        SetCurrentModule((void *) theModule);

        theDeftemplate = (struct deftemplate *) GetNextDeftemplate(NULL);
        while (theDeftemplate != NULL)
          {
           if (theDeftemplate->fuzzyTemplate != NULL)
             {
               struct bsaveLvPlusUniverse bLVFU;
               struct fuzzyLv *flvptr = theDeftemplate->fuzzyTemplate;
               struct primary_term *ptptr;

               ptptr = flvptr->primary_term_list;
               while (ptptr != NULL)
                  { /* count the primary terms */
                    NumberOfFuzzyPrimaryTerms++;
                    ptptr = ptptr->next;
                  }

               /* -1 means no primary terms or no modifiers */
               bLVFU.ptPtr = (NumberOfFuzzyPrimaryTerms == lastNumberOfFuzzyPrimaryTerms) ?
                              -1L : lastNumberOfFuzzyPrimaryTerms;
               bLVFU.from = flvptr->from;
               bLVFU.to = flvptr->to;
               if (flvptr->units == NULL) /* may not be a unit specifier */
                  bLVFU.unitsName = -1L;
               else
                  bLVFU.unitsName = (long)flvptr->units->bucket;
               GenWrite(&bLVFU,(unsigned long) sizeof(struct bsaveLvPlusUniverse),fp);

               lastNumberOfFuzzyPrimaryTerms = NumberOfFuzzyPrimaryTerms;
             }

           theDeftemplate = (struct deftemplate *) GetNextDeftemplate(theDeftemplate);
          }

        theModule = (struct defmodule *) GetNextDefmodule(theModule);
       }

     theModule = (struct defmodule *) GetNextDefmodule(NULL);
     while (theModule != NULL)
       {
        SetCurrentModule((void *) theModule);

        theDeftemplate = (struct deftemplate *) GetNextDeftemplate(NULL);
        while (theDeftemplate != NULL)
          {
           if (theDeftemplate->fuzzyTemplate != NULL)
             {
               struct fuzzyLv *flvptr = theDeftemplate->fuzzyTemplate;
               struct primary_term *ptptr;
               struct bsaveFuzzyPrimaryTerm bFPT;

               ptptr = flvptr->primary_term_list;
               while (ptptr != NULL)
                 { /* write out the primary terms */
                   bFPT.fuzzyValue = (long)ptptr->fuzzy_value_description->bucket;
                   if (ptptr->next != NULL)
                      bFPT.next = 0L;
                   else
                      bFPT.next = -1L;

                   GenWrite(&bFPT,(unsigned long) sizeof(struct bsaveFuzzyPrimaryTerm),fp);
                                   ptptr = ptptr->next;
                  }
              }

           theDeftemplate = (struct deftemplate *) GetNextDeftemplate(theDeftemplate);
          }

        theModule = (struct defmodule *) GetNextDefmodule(theModule);
       }

     }
#endif


   /*=============================================*/
   /* Write out each templateSlot data structure. */
   /*=============================================*/

   for (theModule = (struct defmodule *) GetNextDefmodule(NULL);
        theModule != NULL;
        theModule = (struct defmodule *) GetNextDefmodule(theModule))
     {
      SetCurrentModule((void *) theModule);

      for (theDeftemplate = (struct deftemplate *) GetNextDeftemplate(NULL);
           theDeftemplate != NULL;
           theDeftemplate = (struct deftemplate *) GetNextDeftemplate(theDeftemplate))
        {
         for (theSlot = theDeftemplate->slotList;
              theSlot != NULL;
              theSlot = theSlot->next)
           {
            tempTemplateSlot.constraints = ConstraintIndex(theSlot->constraints);
            tempTemplateSlot.slotName = (long) theSlot->slotName->bucket;
            tempTemplateSlot.multislot = theSlot->multislot;
            tempTemplateSlot.noDefault = theSlot->noDefault;
            tempTemplateSlot.defaultPresent = theSlot->defaultPresent;
            tempTemplateSlot.defaultDynamic = theSlot->defaultDynamic;
            tempTemplateSlot.defaultList = HashedExpressionIndex(theSlot->defaultList);

            if (theSlot->next != NULL) tempTemplateSlot.next = 0L;
            else tempTemplateSlot.next = -1L;

            GenWrite(&tempTemplateSlot,(unsigned long) sizeof(struct bsaveTemplateSlot),fp);
           }
        }
     }

   /*=============================================================*/
   /* If a binary image was already loaded when the bsave command */
   /* was issued, then restore the counts indicating the number   */
   /* of deftemplates, deftemplate modules, and deftemplate slots */
   /* in the binary image (these were overwritten by the binary   */
   /* save).                                                      */
   /*=============================================================*/

   if (Bloaded())
     {
      RestoreBloadCount(&NumberOfDeftemplates);
      RestoreBloadCount(&NumberOfTemplateSlots);
      RestoreBloadCount(&NumberOfTemplateModules);
#if FUZZY_DEFTEMPLATES    
      RestoreBloadCount(&NumberOfFuzzyTemplates);
      RestoreBloadCount(&NumberOfFuzzyPrimaryTerms);
#endif
     }
  }

#endif /* BLOAD_AND_BSAVE */

/****************************************************/
/* BloadStorage: Allocates storage requirements for */
/*   the deftemplates used by this binary image.    */
/****************************************************/
static void BloadStorage()
  {
   unsigned long int space;

   /*=========================================================*/
   /* Determine the number of deftemplate, deftemplateModule, */
   /* and templateSlot data structures to be read.            */
   /*=========================================================*/

   GenRead(&space,(unsigned long) sizeof(unsigned long int));
   GenRead(&NumberOfDeftemplates,(unsigned long) sizeof(long int));
   GenRead(&NumberOfTemplateSlots,(unsigned long) sizeof(long int));
   GenRead(&NumberOfTemplateModules,(unsigned long) sizeof(long int));
#if FUZZY_DEFTEMPLATES   
   GenRead(&NumberOfFuzzyTemplates,(unsigned long) sizeof(long int));
   GenRead(&NumberOfFuzzyPrimaryTerms,(unsigned long) sizeof(long int));
#endif

   /*====================================*/
   /* Allocate the space needed for the  */
   /* deftemplateModule data structures. */
   /*====================================*/

   if (NumberOfTemplateModules == 0)
     {
      DeftemplateArray = NULL;
      SlotArray = NULL;
      ModuleArray = NULL;
#if FUZZY_DEFTEMPLATES
      LvPlusUniverseArray = NULL;
      PrimaryTermArray = NULL;
#endif
      return;
     }

   space = NumberOfTemplateModules * sizeof(struct deftemplateModule);
   ModuleArray = (struct deftemplateModule *) genlongalloc(space);

   /*===================================*/
   /* Allocate the space needed for the */
   /* deftemplate data structures.      */
   /*===================================*/

   if (NumberOfDeftemplates == 0)
     {
      DeftemplateArray = NULL;
      SlotArray = NULL;
#if FUZZY_DEFTEMPLATES   
      LvPlusUniverseArray = NULL;
      PrimaryTermArray = NULL;
#endif
      return;
     }

   space = NumberOfDeftemplates * sizeof(struct deftemplate);
   DeftemplateArray = (struct deftemplate *) genlongalloc(space);

#if FUZZY_DEFTEMPLATES   
   /*==============================================*/
   /* Get space for the LvPlusUniverse structures. */
   /*==============================================*/

   if (NumberOfFuzzyTemplates == 0)
     {
      LvPlusUniverseArray = NULL;
     }
   else
     {
      space = NumberOfFuzzyTemplates * sizeof(struct fuzzyLv);

      LvPlusUniverseArray = (struct fuzzyLv *) genlongalloc(space);
     }

   /*============================================*/
   /* Get space for the primary term structures. */
   /*============================================*/

   if (NumberOfFuzzyPrimaryTerms == 0)
     {
      PrimaryTermArray = NULL;
     }
   else
     {
      space = NumberOfFuzzyPrimaryTerms * sizeof(struct primary_term);

      PrimaryTermArray = (struct primary_term *) genlongalloc(space);
     }

#endif  /* FUZZY_DEFTEMPLATES */


   /*===================================*/
   /* Allocate the space needed for the */
   /* templateSlot data structures.     */
   /*===================================*/

   if (NumberOfTemplateSlots == 0)
     {
      SlotArray = NULL;
      return;
     }

   space =  NumberOfTemplateSlots * sizeof(struct templateSlot);
   SlotArray = (struct templateSlot *) genlongalloc(space);
  }

/********************************************************/
/* BloadBinaryItem: Loads and refreshes the deftemplate */
/*   constructs used by this binary image.              */
/********************************************************/
static void BloadBinaryItem()
  {
   unsigned long int space;

   /*======================================================*/
   /* Read in the amount of space used by the binary image */
   /* (this is used to skip the construct in the event it  */
   /* is not available in the version being run).          */
   /*======================================================*/

   GenRead(&space,(unsigned long) sizeof(unsigned long int));

   /*===============================================*/
   /* Read in the deftemplateModule data structures */
   /* and refresh the pointers.                     */
   /*===============================================*/

   BloadandRefresh(NumberOfTemplateModules,(unsigned) sizeof(struct bsaveDeftemplateModule),
                   UpdateDeftemplateModule);

   /*===============================================*/
   /* Read in the deftemplateModule data structures */
   /* and refresh the pointers.                     */
   /*===============================================*/

   BloadandRefresh(NumberOfDeftemplates,(unsigned) sizeof(struct bsaveDeftemplate),
                   UpdateDeftemplate);

#if FUZZY_DEFTEMPLATES   
   BloadandRefresh(NumberOfFuzzyTemplates,(unsigned) sizeof(struct bsaveLvPlusUniverse),
                   UpdateLvPlusUniverse);
   BloadandRefresh(NumberOfFuzzyPrimaryTerms,(unsigned) sizeof(struct bsaveFuzzyPrimaryTerm),
                   UpdateFuzzyPrimaryTerms);
#endif  /* FUZZY_DEFTEMPLATES */


   /*==========================================*/
   /* Read in the templateSlot data structures */
   /* and refresh the pointers.                */
   /*==========================================*/

   BloadandRefresh(NumberOfTemplateSlots,(unsigned) sizeof(struct bsaveTemplateSlot),
                   UpdateDeftemplateSlot);
  }

/**************************************************/
/* UpdateDeftemplateModule: Bload refresh routine */
/*   for deftemplateModule data structures.       */
/**************************************************/
static void UpdateDeftemplateModule(
  void *buf,
  long obji)
  {
   struct bsaveDeftemplateModule *bdmPtr;

   bdmPtr = (struct bsaveDeftemplateModule *) buf;
   UpdateDefmoduleItemHeader(&bdmPtr->header,&ModuleArray[obji].header,
                             (int) sizeof(struct deftemplate),
                             (void *) DeftemplateArray);
  }

/********************************************/
/* UpdateDeftemplate: Bload refresh routine */
/*   for deftemplate data structures.       */
/********************************************/
static void UpdateDeftemplate(
  void *buf,
  long obji)
  {
   struct deftemplate *theDeftemplate;
   struct bsaveDeftemplate *bdtPtr;

   bdtPtr = (struct bsaveDeftemplate *) buf;
   theDeftemplate = (struct deftemplate *) &DeftemplateArray[obji];

   UpdateConstructHeader(&bdtPtr->header,&theDeftemplate->header,
                         (int) sizeof(struct deftemplateModule),(void *) ModuleArray,
                         (int) sizeof(struct deftemplate),(void *) DeftemplateArray);

   if (bdtPtr->slotList != -1L)
     { theDeftemplate->slotList = (struct templateSlot *) &SlotArray[bdtPtr->slotList]; }
   else
     { theDeftemplate->slotList = NULL; }

#if FUZZY_DEFTEMPLATES   
   if (bdtPtr->fuzzyTemplateList != -1L)
     { theDeftemplate->fuzzyTemplate = (struct fuzzyLv *) &LvPlusUniverseArray[bdtPtr->fuzzyTemplateList]; }
   else
     { theDeftemplate->fuzzyTemplate = NULL; }

   theDeftemplate->hasFuzzySlots = bdtPtr->hasFuzzySlots;
#endif

   if (bdtPtr->patternNetwork != -1L)
     { theDeftemplate->patternNetwork = (struct factPatternNode *) BloadFactPatternPointer(bdtPtr->patternNetwork); }
   else
     { theDeftemplate->patternNetwork = NULL; }

   theDeftemplate->implied = bdtPtr->implied;
#if DEBUGGING_FUNCTIONS
   theDeftemplate->watch = WatchFacts;
#endif
   theDeftemplate->inScope = FALSE;
   theDeftemplate->numberOfSlots = bdtPtr->numberOfSlots;
  }

#if FUZZY_DEFTEMPLATES   
/******************************************************/
/* UpdateLvPlusUniverse: Updates pointers in bloaded  */
/*   LvPlusUniverse slot structures.                  */
/******************************************************/
static void UpdateLvPlusUniverse(buf,obji)
  void *buf;
  long obji;
  {
    struct fuzzyLv *lvpuPtr;
    struct bsaveLvPlusUniverse *blvpuPtr;

    blvpuPtr = (struct bsaveLvPlusUniverse *) buf;
    lvpuPtr = (struct fuzzyLv *) &LvPlusUniverseArray[obji];

    if (blvpuPtr->ptPtr != -1L)
       lvpuPtr->primary_term_list = (struct primary_term *) &PrimaryTermArray[blvpuPtr->ptPtr];
    else
       lvpuPtr->primary_term_list = NULL;

    lvpuPtr->from = blvpuPtr->from;
    lvpuPtr->to = blvpuPtr->to;
    if (blvpuPtr->unitsName == -1L)
       lvpuPtr->units = NULL;
    else
       {
         lvpuPtr->units = SymbolArray[blvpuPtr->unitsName];
         IncrementSymbolCount(lvpuPtr->units);
       }
  }



/*********************************************************/
/* UpdateFuzzyPrimaryTerms: Updates pointers in bloaded  */
/*   primary term data structures.                       */
/*********************************************************/
static void UpdateFuzzyPrimaryTerms(buf,obji)
  void *buf;
  long obji;
  {
    struct primary_term *ptPtr;
    struct bsaveFuzzyPrimaryTerm *bptPtr;

        bptPtr = (struct bsaveFuzzyPrimaryTerm *) buf;
        ptPtr = (struct primary_term *) &PrimaryTermArray[obji];

    ptPtr->fuzzy_value_description = (FUZZY_VALUE_HN *) FuzzyValueArray[bptPtr->fuzzyValue];
        IncrementFuzzyValueCount(ptPtr->fuzzy_value_description);

        if (bptPtr->next == -1L)
          ptPtr->next = NULL;
        else
          ptPtr->next = (struct primary_term *) &PrimaryTermArray[obji+1];
  }



#endif /* FUZZY_DEFTEMPLATES */


/************************************************/
/* UpdateDeftemplateSlot: Bload refresh routine */
/*   for templateSlot data structures.          */
/************************************************/
static void UpdateDeftemplateSlot(
  void *buf,
  long obji)
  {
   struct templateSlot *theSlot;
   struct bsaveTemplateSlot *btsPtr;

   btsPtr = (struct bsaveTemplateSlot *) buf;
   theSlot = (struct templateSlot *) &SlotArray[obji];

   theSlot->slotName = SymbolPointer(btsPtr->slotName);
   IncrementSymbolCount(theSlot->slotName);
   theSlot->defaultList = HashedExpressionPointer(btsPtr->defaultList);
   theSlot->constraints = ConstraintPointer(btsPtr->constraints);

   theSlot->multislot = btsPtr->multislot;
   theSlot->noDefault = btsPtr->noDefault;
   theSlot->defaultPresent = btsPtr->defaultPresent;
   theSlot->defaultDynamic = btsPtr->defaultDynamic;

   if (btsPtr->next != -1L)
     { theSlot->next = (struct templateSlot *) &SlotArray[obji + 1]; }
   else
     { theSlot->next = NULL; }
  }

/*****************************************/
/* ClearBload: Deftemplate clear routine */
/*   when a binary load is in effect.    */
/*****************************************/
static void ClearBload()
  {
   unsigned long int space;
   int i;

   /*=============================================*/
   /* Decrement in use counters for atomic values */
   /* contained in the construct headers.         */
   /*=============================================*/

   for (i = 0; i < NumberOfDeftemplates; i++)
     { UnmarkConstructHeader(&DeftemplateArray[i].header); }

   /*=======================================*/
   /* Decrement in use counters for symbols */
   /* used as slot names.                   */
   /*=======================================*/

   for (i = 0; i < NumberOfTemplateSlots; i++)
     { DecrementSymbolCount(SlotArray[i].slotName); }

#if FUZZY_DEFTEMPLATES   
   for (i = 0; i < NumberOfFuzzyTemplates; i++)
     {
       if (LvPlusUniverseArray[i].units != NULL)
         DecrementSymbolCount(LvPlusUniverseArray[i].units);
     }

   for (i = 0; i < NumberOfFuzzyPrimaryTerms; i++)
     {
       DecrementFuzzyValueCount(PrimaryTermArray[i].fuzzy_value_description);
     }
#endif
   /*======================================================================*/
   /* Deallocate the space used for the deftemplateModule data structures. */
   /*======================================================================*/

   space =  NumberOfTemplateModules * sizeof(struct deftemplateModule);
   if (space != 0) genlongfree((void *) ModuleArray,space);

   /*================================================================*/
   /* Deallocate the space used for the deftemplate data structures. */
   /*================================================================*/

   space = NumberOfDeftemplates * sizeof(struct deftemplate);
   if (space != 0) genlongfree((void *) DeftemplateArray,space);

#if FUZZY_DEFTEMPLATES  
   space = NumberOfFuzzyTemplates * sizeof(struct fuzzyLv);
   if (space != 0) genlongfree((void *) LvPlusUniverseArray,space);

   space = NumberOfFuzzyPrimaryTerms * sizeof(struct primary_term);
   if (space != 0) genlongfree((void *) PrimaryTermArray,space);
#endif

   /*=================================================================*/
   /* Deallocate the space used for the templateSlot data structures. */
   /*=================================================================*/

   space =  NumberOfTemplateSlots * sizeof(struct templateSlot);
   if (space != 0) genlongfree((void *) SlotArray,space);

   /*======================================*/
   /* Create the initial-fact deftemplate. */
   /*======================================*/

#if (! BLOAD_ONLY)
   CreateImpliedDeftemplate((SYMBOL_HN *) AddSymbol("initial-fact"),FALSE);
#endif
  }

/************************************************************/
/* BloadDeftemplateModuleReference: Returns the deftemplate */
/*   module pointer for use with the bload function.        */
/************************************************************/
globle void *BloadDeftemplateModuleReference(
  int index)
  {
   return ((void *) &ModuleArray[index]);
  }

#endif /* DEFTEMPLATE_CONSTRUCT && (BLOAD || BLOAD_ONLY || BLOAD_AND_BSAVE) && (! RUN_TIME) */


