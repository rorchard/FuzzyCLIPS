/*  $Header: /dist/CVS/fzclips/src/fuzzypsr.h,v 1.3 2001/08/11 21:05:58 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*             CLIPS Version 6.00  05/12/93            */
   /*                                                     */
   /*             DEFFUZZY PARSER HEADER FILE             */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*      Brian L. Donnell                                     */
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



#ifndef _H_fuzzypsr
#define _H_fuzzypsr




#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _FUZZYPSR_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE struct fuzzyLv        *ParseFuzzyTemplate(char *readSource, 
                                                   struct token *inputToken,
                                                   int *DeftemplateError);
   LOCALE void                  RtnFuzzyTemplate(struct fuzzyLv *lv);
   LOCALE void                  rtnFuzzyValue(struct fuzzy_value *fv);
   LOCALE void                  InstallFuzzyValue(void *fv);
   LOCALE void                  DeinstallFuzzyValue(void *fv);
   LOCALE void                  InstallFuzzyTemplate(struct deftemplate *theDeftemplate);
   LOCALE void                  DeinstallFuzzyTemplate(struct fuzzyLv *fzTemplate);
   LOCALE double                sFunction(double x, double alfa, double beta, double gamma);
   LOCALE void                  Init_S_Z_PI_yvalues( );
   LOCALE struct fuzzy_value    *Get_S_Z_or_PI_FuzzyValue(double alfa, double beta, 
                                                          double gamma, int function_type);
   

/* must be an odd number -- best not to be too big or too small
   Keep in range 5 to 13 -- 9 has been used most of the time
*/
#define ArraySIZE 9



#ifndef _FUZZYPSR_SOURCE_
   extern double                S_array[];
   extern double                Z_array[];
   extern double                PI_array[];
#endif



#endif

