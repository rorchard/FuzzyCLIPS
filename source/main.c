static char rcsid[] = "$Header: /dist/CVS/fzclips/src/main.c,v 1.3 2001/08/11 21:06:42 dave Exp $" ;

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.05  04/09/97            */
   /*                                                     */
   /*                     MAIN MODULE                     */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/* Contributing Programmer(s):                               */
/*      Bob Orchard (NRCC - Nat'l Research Council of Canada)*/
/*                  (Fuzzy reasoning extensions)             */
/*                  (certainty factors for facts and rules)  */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#include <stdio.h>
#include "setup.h"
#include "sysdep.h"
#include "extnfunc.h"
#include "commline.h"

int main(int,char *[]);
void UserFunctions(void);

/****************************************/
/* main: Starts execution of the expert */
/*   system development environment.    */
/****************************************/
int main(
  int argc,
  char *argv[])
  {
   InitializeEnvironment();   
   RerouteStdin(argc,argv);
   CommandLoop();
   return(-1);
  }
  
/*********************************************************/
/* UserFunctions: Informs the expert system environment  */
/*   of any user defined functions. In the default case, */
/*   there are no user defined functions. To define      */
/*   functions, either this function must be replaced by */
/*   a function with the same name within this file, or  */
/*   this function can be deleted from this file and     */
/*   included in another file.                           */
/*********************************************************/
void UserFunctions()
  {
/*
#if FUZZY_DEFTEMPLATES

#define ShowerProblemUI 0

#if ! RUN_TIME
#if ShowerProblemUI
   void initSimulation();
   void SetValvePositions();

   DefineFunction("initSimulation",'v',PTIF initSimulation, "initSimulation");
   DefineFunction("SetValvePositions",'v',PTIF SetValvePositions, "SetValvePositions");
#endif

#endif


#endif  end of #if FUZZY_DEFTEMPLATES
*/

  }

