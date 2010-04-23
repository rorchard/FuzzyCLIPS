/*  $Header: /dist/CVS/fzclips/src/fuzzycom.h,v 1.3 2001/08/11 21:05:52 dave Exp $  */

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*             CLIPS Version 6.00  05/12/93            */
   /*                                                     */
   /*             FUZZY COMMANDS HEADER FILE              */
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



#ifndef _H_fuzzycom
#define _H_fuzzycom




#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _FUZZYCOM_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

    LOCALE void           DeffuzzyCommands();
    LOCALE void          *getu();
    LOCALE double         getu_from();
    LOCALE double         getu_to();
    LOCALE void          *getu_units();
    LOCALE void          *get_fs();
    LOCALE void          *get_fs_template();
    LOCALE void          *get_fs_lv();
    LOCALE int            get_fs_length(); 
    LOCALE double         get_fs_value();
    LOCALE double         get_fs_x();
    LOCALE double         get_fs_y();
    LOCALE double         moment_defuzzify();
    LOCALE double         maximum_defuzzify();
    LOCALE void           add_fuzzy_modifier();
    LOCALE void           remove_fuzzy_modifier();
    LOCALE void           set_fuzzy_inference_type();
    LOCALE void          *get_fuzzy_inference_type();
    LOCALE void           set_fuzzy_display_precision();
    LOCALE long int       get_fuzzy_display_precision();
    LOCALE void           set_alpha_value();
    LOCALE double         get_alpha_value();
    LOCALE void           plot_fuzzy_value();
    LOCALE struct fuzzy_value *get_fuzzy_slot();
    LOCALE struct fuzzy_value *fuzzy_union();
    LOCALE struct fuzzy_value *fuzzy_intersection();
    LOCALE struct fuzzy_value *fuzzy_modify();
    LOCALE struct fuzzy_value *create_fuzzy_value();

#endif

