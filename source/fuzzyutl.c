static char rcsid[] = "$Header: /dist/CVS/fzclips/src/fuzzyutl.c,v 1.3 2001/08/11 21:06:00 dave Exp $" ;

   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*             CLIPS Version 6.00  05/12/93            */
   /*                                                     */
   /*             FUZZY UTILITY MODULE                    */
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


/******************************************************************
    Fuzzy Logic Module

    This file contains the following categories of functions:
    
    routines to support fuzzy set operations (compliment,
                   union, intersection, fuzzy_consequent)
    routines to support compositional rule of inference
    routines to support the max of min operation
    
    routine to change value of fuzzy fact due to global contribution

 ******************************************************************/
 
#define _FUZZYUTL_SOURCE_


#include "setup.h"


#if FUZZY_DEFTEMPLATES

#include <stdio.h>
#define _CLIPS_STDIO_

#include <math.h>

#include "symbol.h"
#include "fuzzydef.h"
#include "fuzzyutl.h"
#include "fuzzyrhs.h"
#include "fuzzyval.h"
#include "fuzzypsr.h"
#include "reteutil.h"
#include "memalloc.h"
#include "engine.h"
#include "match.h"
#include "factmngr.h"
#include "router.h"
#include "evaluatn.h"
#include "argacces.h"
#include "prntutil.h"
#include "router.h"

#if CERTAINTY_FACTORS  
#include "cfdef.h"
#endif


/******************************************************************
    Global Internal Function Declarations
    
    Defined in fuzzyutl.h
 ******************************************************************/



/******************************************************************
    Local Internal Function Declarations
 ******************************************************************/
 
   static struct   fuzzy_value *horizontal_union( struct fuzzy_value *fv,
                                           double yvalue );
   static struct   fuzzy_value *horizontal_intersection( struct fuzzy_value *fv,
                                           double yvalue );
   static struct   fuzzy_value *max_prod_scale( struct fuzzy_value *fv,
                                           double yvalue );
   static struct   fuzzy_value *horizontal_union_or_intersection(struct fuzzy_value *fv,
                                 double yvalue, int unionFlg);
   static void     concatenate ( double *set1x, double *set1y, int length1,
                                 double *set2x, double *set2y, int length2,
                                 struct fuzzy_value *newValue );
   static void     standardize ( double *oldsetx, double *oldsety, int oldlength, 
                                 double *newValuex, double *newValuey, int *newlength, 
                                 double min, double max);
   static int      line_segment_intersection( double xP, double yP, double xQ, double yQ,
                                 double xR, double yR, double xS, double yS,
                                 double *x, double *y );
   static struct   fuzzy_value *horizontal_union( struct fuzzy_value *fv, 
                               double yvalue );
   static void     STORE_THE_POINT( struct fuzzy_value *result,
                                    double xval, double yval );
   static void     computeMinOfMaxmins();


/******************************************************************
    Local Internal Variable Declarations
 ******************************************************************/


   
/******************************************************************
    Global External Variable Declarations
 ******************************************************************/




/******************************************************************
    Global Internal Variable Declarations
 ******************************************************************/

   /* saveFactsInProgress is TRUE temporarily during save-facts command */
   globle int   saveFactsInProgress = FALSE;



/******************************************************************
    FUZZY SET OPERATION FUNCTIONS
 ******************************************************************/


/*****************************************************************/
/* FCOMPLIMENT: compliment of fuzzy set (does change in place -  */
/*              ie. no new fuzzy set created)                    */
/*                                                               */
/*   V u (x) = 1 - u (x)                                         */
/*    x f           f                                            */
/*****************************************************************/
globle void fcompliment(fv)
  struct fuzzy_value *fv;
{
    int i;
  
    for ( i=0; i < fv->n; i++ )
      {
        fv->y[i] = 1.0 - fv->y[i];
      }
}




/*********************************************************************
       line_segment_intersection
       
    Calculates the intersection of 2 line segments AB and CD 
    
    Assume that Ax < Bx, Cx < Dx, A != B, C != D
    
    Returns the intersection point thru X and Y args
    
    Returns a value of:
    
        0 if no intersection
        1 if intersection exists
        2 if segments are collinear - may or may not overlap
        
        
    AB and CD can be given by:
    
        AB = A+r(B-A), r in [0,1]
        CD = C+s(D-c), s in [0,1]
        
    If AB & CD intersect, then
    
        A+r(B-A) = C+s(D-C), or
        
        XA+r(XB-XA) = XC+s(XD-XC)
        YA+r(YB-YA) = YC+s(YD-YC)  FOR SOME s,r IN [0,1]
        
    Solving for r and s gives:
    
              (YA-YC) (XD-XC) - (XA-XC) (YD-YC)
        r = ------------------------------------   eqn 1
              (XB-XA) (YD-YC) - (YB-YA) (XD-XC)
              
              (YA-YC) (XB-XA) - (XA-XC) (YB-YA)
        S = ------------------------------------   eqn 2
              (XB-XA) (YD-YC) - (YB-YA) (XD-XC)
              
    Then intersection is
    
            I = A+r(B-A), or
            
            XI = XA+r(XB-XA)
            YI = YA+r(YB-YA)
    
    If 0<=r<=1 and 0<=s<=1 the segments intersect
    If r<0 or r>1 or s<0 or s>1 then no intersection of segments
    
    If denominator in eqn 1 (same as denom in eqn 2) is 0, AB & CB parallel
    If numerator of eqn 1 is also 0, AB and CD are collinear or 
    If numerator of eqn 2 is also 0, AB and CD are collinear

    Note: demoninator is 0 basically means that AB and CD have same slope
          numerator of eqn1 is 0 when CA and CD have same slope
          numerator of eqn2 is 0 when CA and AB have same slope
              
*********************************************************************/

static int line_segment_intersection(Ax, Ay, Bx, By, Cx, Cy, Dx, Dy, X, Y )
  
   double Ax, Ay, Bx, By;
   double Cx, Cy, Dx, Dy;
   double *X, *Y;
{
  int numer2Positive, denomPositive;
  double denominator;
  double numerator1, numerator2;
  double r;
  
  double BymAy = By - Ay;
  double BxmAx = Bx - Ax;
  double DxmCx = Dx - Cx;
  double DymCy = Dy - Cy;
  double AymCy = Ay - Cy;
  double AxmCx = Ax - Cx;
  
  denominator = (BxmAx)*(DymCy) - (BymAy)*(DxmCx);
  numerator1 =  (AymCy)*(DxmCx) - (AxmCx)*(DymCy);
  
  /* Sometimes we get into trouble when lines are very close to being
     parallel and the limits of the fl. pt. precision take over so we
     chose to make a simple test for close to 0 denominator.
  */
  if (fabs(denominator) < 1.0e-12)
     if (numerator1 == 0.0)
        return( 2 );  /* on same lines - collinear */
     else
        return( 0 );  /* no intersection -- parallel lines */
        
  r = numerator1/denominator;
  
  if (r<0 || r>1)
     return( 0 );    /* no intersection of segments */

  numerator2 = (AymCy)*(BxmAx) - (AxmCx)*(BymAy);
  
  /* Following code replaced by the test below -- saves a division and
     gets the same results.

     s = numerator2/denominator;
  
     if (s<0 || s>1)
        return( 0 );

     s<0 is covered by test of signs being different 
         (numer2Positive != denomPositive)
     s>1 is covered by other 2 tests (both positive or both negative)

     Note that denominator is not 0 at this point
  */
  
  if (numerator2 != 0.0)
    {
      numer2Positive = (numerator2 > 0.0);
      denomPositive = (denominator > 0.0);
      if ((numer2Positive != denomPositive) ||
          (numer2Positive && (numerator2 > denominator)) || 
          (!numer2Positive && (numerator2 < denominator))
         )
         return( 0 );    /* no intersection of segments */
    }

  /* We can expect vertical lines or horizontal lines.
     In that case avoid a mult and subtract and add and likely
     some rounding errors by checking if x vals (y vals) equal for
     either line segment. Note we could do other special checks for 
     speed considerations BUT this has relevance in this application 
     since horizontal lines appear often.
  */

   if (BxmAx == 0.0)
      *X = Ax;
   else if (DxmCx == 0.0)
      *X = Cx;
   else
      *X = Ax + r*(BxmAx);
  

  if (Cy == Dy)
     *Y = Cy;
  else if (By == Ay)
     *Y = Ay;
  else
     *Y = Ay + r*(BymAy);
  
  return( 1);  /* intersection obtained */

}

/*******************************************************************

 FZ_EQUAL tests two float values for equality with a fine tolerance

 FUZZY_TOLERANCE is defined in constant.h

*******************************************************************/


globle int FZ_EQUAL( f1, f2 )
   double f1, f2;
{
  return( fabs(f1-f2) < FUZZY_TOLERANCE );
}

/*********************************************************************

 STORE_THE_POINT below used by funion & maxmin_intersect functions to 
 insert points into the array that is being created as the 
 union/intersection of 2 fuzzy sets 

 *********************************************************************/

static void STORE_THE_POINT(result, xval, yval)

  struct fuzzy_value *result;
  double xval, yval;
{  
  int i = result->n;
  double *resultX = result->x;
  double *resultY = result->y;

  if (i >= result->maxn)
    { /* exceeding space allocated for the array of points */
      SystemError("FUZZYUTL - STORE_THE_POINT routine", 903);
      ExitCLIPS(2);
    }
    
  /* if yvalue is > 1.0 or < 0.0 could be a rounding problem -- set to 0 or 1 */
  if (yval > 1.0) 
      yval = 1.0;
  else if (yval < 0.0) 
      yval = 0.0;

  /* Could get an intersection calculation that shows the x value to be less
     the previous x value (NOT SURE IF THIS REALLY COULD HAPPEN?). If so
     and the x value is very close to last one then set the x value to be the
     same as the last x value, otherwise we have a major problem that should 
     never happen (print warning and do not insert the point).
  */
  if ((i != 0) && (xval < resultX[i-1]))
     {
       if ( (resultX[i-1] - xval) > FUZZY_TOLERANCE )
         {
           PrintRouter(WERROR,"[FUZZYUTL] Attempt to insert point with smaller x value in 'STORE_THE_POINT'\n");
           PrintRouter(WERROR,"           WARNING: Possible internal error\n");
           return;  /* do NOT insert */
         }

       xval = resultX[i-1];
     }

  /* If this point same as last one then do not insert */
  if ((i > 0) && FZ_EQUAL(xval, resultX[i-1]) && FZ_EQUAL(yval, resultY[i-1]))
     return;

  /* If inserting any point and the X value is the same as the
     last 2 (ie. 3 pts with same x value in a row) and all 3 point's
     y values are increasing or all 3 are decreasing then replace
     the 2nd point of the 3 with the 3rd point. OR if we have 4 points
     in a row with same x value then replace the 3rd one with the
     last one (you need to draw some pictures to appreciate this --
     the 1st 3 of the 4 wil not be all increasing or all decreasing
     becuase of the above test and removal).
  */
  if ((i > 2) && (xval == resultX[i-1]) && (xval == resultX[i-2]))
     {  /* last 3 pts have same x value */
        if ((yval < resultY[i-1] && resultY[i-1] < resultY[i-2]) ||
            (yval > resultY[i-1] && resultY[i-1] > resultY[i-2]) ||
            (i>3 && xval == resultX[i-4])
           )
          {
	    resultY[i-1] = yval;
            return;
          }
     }

  /* If inserting 2nd point (i == 1) and the y value is the same as the 1st
     point's y value then just change the 1st point's x value to be same as
     this point's x value
  */
  if ((i == 1) && (yval == resultY[0]))
    {
      resultX[0] = xval;
      return;
    }
  
  /* if inserting a point beyond the 3rd point (i > 2) and the last 2
     points have same yval as this one to be inserted then just replace
     last x value with this x value
  */
  if ((i > 2) && (yval == resultY[i-1]) && (yval == resultY[i-2]))
    {
      resultX[i-1] = xval;
      return;
    }

  /* otherwise just add the point and increment the number of points
     in the result array
  */
  resultX[i] = xval;
  resultY[i] = yval;
  result->n = ++i;
}



/*********************************************************************
    max_of_min()

    returns the maximum y value of the intersection of two fuzzy sets

 *********************************************************************/
globle double max_of_min(f1,f2)
  struct fuzzy_value *f1,*f2;
{
  return( maxmin_intersect(f1, f2, FALSE, NULL) );
}


/*********************************************************************
    fintersect()

    returns the intersection of two fuzzy sets

 *********************************************************************/
globle struct fuzzy_value *fintersect(f1,f2)
  struct fuzzy_value *f1,*f2;
{
  struct fuzzy_value *intersectSet;

  maxmin_intersect(f1, f2, TRUE, &intersectSet);
  return( intersectSet );

}

/********************************************************************
    findOverlapMaxYvalue(ay1, ay2, by1, by2)

    Checks that the two vertical line segments represented by the 
    y values ay1, ay2 and by1, by2 have opposite directions
    and if so finds the maximum y value of the overlapping part 
    of these lines.
    If there is no overlapping part or the segments have the same 
    direction (both up or both down) then return -1.0

    Note: assumption -- all y values between 0.0 and 1.0 

             |        <-- max ay
             |
             |  |     <-- max by
             |  |
             |  | 
             |  |     <-- min ay
                |
                |     <-- min by

   The maximum of the overlap is the lesser of the two max values
   of the line segments

********************************************************************/
static double findOverlapMaxYvalue(ay1, ay2, by1, by2)
  double ay1, ay2, by1, by2;
{
  double maxa, mina, maxb, minb;

  /* opposite directions? */
  if (((ay2 < ay1) && (by2 < by1)) ||
      ((ay2 > ay1) && (by2 > by1))
     )
    return( -1.0 ); /* same direction -- do not find overlap */

  /* find overlap if any */
  if (ay2 < ay1) 
	{ maxa = ay1; mina = ay2;}
  else
   { maxa = ay2; mina = ay1;}

  if (by2 < by1) 
   { maxb = by1; minb = by2;}
  else
	{ maxb = by2; minb = by1;}

  if ((mina > maxb) || (minb > maxa))
    return( -1.0 ); /* no overlap */

  /* find max of overlap */
  if (maxa > maxb)
	 return( maxa );
  else
	 return( maxb );
}

/*********************************************************************
    maxmin_intersect()

    returns the maximum y value and if the argument DoIntersect is 
    TRUE then also returns the intersection set address via the 
    argument intersectSet. If DoIntersect is FALSE then no intersection
    set is created.

    Assume that Alength and Blength are both >= 1

    Expects that both f1 anf f2 fuzzy values are associated with the 
    same fuzzy deftemplate -- if not shouldn't be doing the intersection
    (like adding apples and oranges)

    User of this routine is expected to maintain the correct value
    in the "name" field of the resulting fuzzy value

 *********************************************************************/
globle double maxmin_intersect(f1, f2, DoIntersect, intersectSet)
  struct fuzzy_value *f1,*f2;
  int DoIntersect;
  struct fuzzy_value **intersectSet;
{
  double *Ax, *Ay;
  double *Bx, *By;
  int Alength,Blength;
  double lastAx, lastAy, lastBx, lastBy;
  double currentAx, currentBx;
  double currentAy, currentBy;
  double max, maxmin, Xmax;
  double X, Y;
  int Aindex, Bindex;
  int intersectFlag;
  int i, size;
  struct fuzzy_value *result;
  
  /* extract the sets from the Fuzzy values */
  Ax = f1->x;
  Ay = f1->y;
  Alength = f1->n;
  
  Bx = f2->x;
  By = f2->y;
  Blength = f2->n;

  /* If both sets have only 1 member then they are constants fuzzy sets
     so just pick the min of the y values for the max of mins and return
     that set as the intersection.
  */
  if (Alength ==1 && Blength == 1)
    {
     if ( Ay[0] < By[0] ) 
        { 
          if (DoIntersect) *intersectSet = CopyFuzzyValue( f1 );
          return( Ay[0]  );
        }
     else
        {
          if (DoIntersect) *intersectSet = CopyFuzzyValue( f2 );
          return( By[0] );
        }
    }
     
  /* if only 1 of the 2 sets has 1 element then compare that y value to the
     max y of the other set. If it is greater return the max of the set since
     it lies completely below this single point constant set (and the 
     intersection set is the multi point set), otherwise return
     the y value of the single point since it intersects the other set
     horizontally.
  */
  if (Alength == 1)
    {
      max = By[0];
      for (i = 1; i<Blength; i++)
         if (By[i] > max) max = By[i];

      if ( max < Ay[0] )  
         { 
           if (DoIntersect) *intersectSet = CopyFuzzyValue( f2 );
         }
      else
         {
           max = Ay[0];
           if (DoIntersect) *intersectSet = horizontal_intersection( f2, max );
         }

      return( max );
    }
    
  if (Blength == 1)
    {
      max = Ay[0];
      for (i = 1; i<Alength; i++)
         if (Ay[i] > max) max = Ay[i];

      if ( max < By[0] )  
         { 
           if (DoIntersect) *intersectSet = CopyFuzzyValue( f1 );
         }
      else
         {
           max = By[0];
           if (DoIntersect) *intersectSet = horizontal_intersection( f1, max );
         }

      return( max );
    }
    
  /* If the two sets do not intersect then maxmin is 0.0.
     Note: would not have to do this if we knew that they intersected

     The returned fuzzy set will be a single point set with membership
     value of 0.0 
  */
  if (nonintersectiontest(Ax, Ay, Bx, By, Alength, Blength))
    {
     if (DoIntersect) 
       {
        result = get_struct(fuzzy_value);
  
        result->name = NULL;
        result->whichDeftemplate = f1->whichDeftemplate;
        result->x = FgetArray( 1 );
        result->y = FgetArray( 1 );
        result->n = 1;
        result->maxn = 1;

        result->x[0] = Ax[0];
        result->y[0] = 0.0;

        *intersectSet = result;
       }
     return( 0.0 );
    }

  /* Create the fuzzy value to return to the caller and the sets that will
     be stored in the fuzzy value. Note we allocate an initial x and y
     array to hold the union. The initial size of each is set to
     
       (Alength*3-1)/2 + (Blength*3-1)/2
       
     This value was determined to be the worst case size -- I think it is correct
     or 1 or 2 too large?!!
  */
  if (DoIntersect) 
    {
     result = get_struct(fuzzy_value);
  
     size = (Alength*3-1)/2 + (Blength*3-1)/2 + 1;  /* 1 extra for safety */
     result->x = FgetArray( size );
     result->y = FgetArray( size );
     result->n = 0;
     result->maxn = size;
     result->name = NULL;
     result->whichDeftemplate = f1->whichDeftemplate;
    }
  
     
  /* Set up to find the max of mins (ie. the max y value in the intersection
     of the 2 fuzzy sets) and to find the intersection set if required.
     Pick the starting point by determining which of the 1st points of 
     either set in inside (part of the intersection) and set the 
     starting x position to be the same for each set.
  */
  lastAy = Ay[0];
  lastBy = By[0];
  
  if (Ax[0] == Bx[0])  /* Set A and B same 1st position */
    {
      lastAx = Ax[0]; /* set same x values */
      lastBx = Bx[0];
      
      Aindex = 1;
      Bindex = 1;
    }
  else if (Ax[0] < Bx[0]) /* Set A has 1st x position */
    {
      lastAx = Ax[0]; /* set same x values */
      lastBx = Ax[0];
      
      Aindex = 1;
      Bindex = 0;
    }
  else /*  Ax[0] > Bx[0] -- Set B has 1st x position */
    {
		lastBx = Bx[0]; /* set same x values */
      lastAx = Bx[0];
      
      Aindex = 0;
      Bindex = 1;
    } 
      
  if (lastAy <= lastBy) /* A set has 1st point inside */
    {
      maxmin = lastAy;
      if (DoIntersect)
         STORE_THE_POINT(result, lastAx, lastAy);
    }
  else /* B set has 1st point inside */
    {
      maxmin = lastBy;
      if (DoIntersect)
         STORE_THE_POINT(result, lastBx, lastBy);
    }
    
  /* store the largest X value in either of the 2 sets */
  Xmax = (Ax[Alength -1] >= Bx[Blength-1]) ? Ax[Alength -1] : Bx[Blength-1];
    
  /* remember the current positions of the A and B sets -- just need to
     keep the x values, the y values can be picked up using Aindex, Bindex
     when required but the x values may have to be taken from an extended 
     x value (last point of the other set) on reaching its last x value
  */
  currentAx = Ax[Aindex];
  currentBx = Bx[Bindex];
  currentAy = Ay[Aindex];
  currentBy = By[Bindex];
    
  /* now we are ready to walk through the 2 sets looking for points inside
     the intersection of the 2 sets (actually points on the intersection
     boundary)

    The loop below terminates when certain end conditions met:

    A. If we have hit the end of both A and B sets (in while condition)
    B. If hit the end of one of A or B AND that last point of the 
       set is inside -- this point defines a horizontal line from
       its x position to the max value of the universe of discourse
       and therefore NO other pt after this on the intersection
       boundary can be above this -- so any current max will remain
       the max! NOTE: if we are finding the intersection set as well as
       the max we cannot stop here (ie. when DoIntersect flag is TRUE)

  */
  
  while (Bindex < Blength || Aindex < Alength) 
    {
      /* if the current max_of_min (maxmin) ever hits 1.0 just return since
         it can't go any higher than that! Unless we are calculating the
         intersection set as well
      */
		if (!DoIntersect && maxmin == 1.0)
         return(1.0);
         
      /* if the last points being considered for the 2 sets are in fact the
         same points then by comparing the slopes of the 2 line segments
         we can tell which current point is inside (smaller slope)
      */
      if (lastAx == lastBx && lastAy == lastBy)
        {
          /* if the current x values of the 2 sets is the same then the
             checking of the slopes is easy since the minimum of the 
             current y values is the minimum slope and that one is inside
          */
          if (currentAx == currentBx)
            { /* move last points of A and B to same x position */
              if (DoIntersect)
                {
                 if (currentAy > currentBy) 
                  {
                   /* current B is inside or both at same point */
                   STORE_THE_POINT(result, currentBx, currentBy);
                  }
                 else
                  {
                   /* current A is inside */
                   STORE_THE_POINT(result, currentAx, currentAy);
                  }
                }

              Aindex++; Bindex++;

              if (currentAy == currentBy) 
                {
                  /* A and B same line segments ! */
                  if (currentAy > maxmin)
                     maxmin = currentAy;
                  if (!DoIntersect && (Aindex >= Alength || Bindex >= Blength))
                     /* hit end of A or B and inside -- done */
                     return( maxmin );
                }
              else if (currentAy < currentBy) 
                {
                  /* current A is inside */
                  if (currentAy > maxmin)
                     maxmin = currentAy;
                  if (!DoIntersect && (Aindex >= Alength))
                     /* hit end & A inside -- done */
                     return( maxmin );
                }
              else
                {
                  /* current B is inside */
                  if (currentBy > maxmin)
                     maxmin = currentBy;
                  if (!DoIntersect && (Bindex >= Blength))
                     /* hit end & B inside -- done */
                     return( maxmin );
                }

              /* don't move A or B if already at end */
              if (Aindex <= Alength)
                {
                  lastAx = currentAx; lastAy = currentAy;
                  if (Aindex == Alength)
                      currentAx = Xmax;
                  else
                      { currentAx = Ax[Aindex]; currentAy = Ay[Aindex];}
                }

              if (Bindex <= Blength)
                {
                  lastBx = currentBx; lastBy = currentBy;
                  if (Bindex == Blength)
                      currentBx = Xmax;
                  else
                      { currentBx = Bx[Bindex]; currentBy = By[Bindex]; }
                }
            }
          /* if the current x values not the same must do a calc to compare 
             slopes

                         dAy/dAx compared to dBy/dBx

              If dAx = 0 set slopes to 1 (or -1) and 0 
              If dBx = 0 set slopes to 0 and 1 (or -1) 
          */
          else 
            { double dAydBx;  
              double dBydAx;

              if (currentAx == lastAx)
                 { dAydBx = (currentAy > lastAy) ? 1.0 : -1.0; 
                   dBydAx = 0.0; 
                 }
              else if (currentBx == lastBx)
                 { dAydBx = 0.0; 
                   dBydAx = (currentBy > lastBy) ? 1.0 : -1.0;
                 }
              else
                 { dAydBx = (currentAy-lastAy)*(currentBx-lastBx);
                   dBydAx = (currentBy-lastBy)*(currentAx-lastAx);
                 }

              if (currentAx < currentBx)
                {            
                  Aindex++;
                  
                  if (dAydBx == dBydAx)
                    {
                     /* slopes equal -- lines are collinear -- move both
                        points to current A position - A already moved.
                        A is inside
                     */
                     lastBy = currentAy; lastBx = currentAx;
                    }
                  if (dAydBx <= dBydAx)
                    {
                      /* current A is inside */
                      if (currentAy > maxmin)
                         maxmin = currentAy;
                      if (DoIntersect) 
                         STORE_THE_POINT(result, currentAx, currentAy);
                      else if (Aindex >= Alength) 
                         return( maxmin );   /* A reaches end and is inside -- done */
                    }

                  lastAy = currentAy; lastAx = currentAx;
                  if (Aindex >= Alength)
                     currentAx = Xmax;
                  else
                    { currentAx = Ax[Aindex]; currentAy = Ay[Aindex]; }

                }
              else  /* currentBx < currentAx */
                {
                  Bindex++;

                  if (dAydBx == dBydAx)
                    {
                     /* slopes equal -- lines are collinear -- move both
                        points to current B position - B already moved.
                        B is inside
                     */
                     lastAy = currentBy; lastAx = currentBx;
                    }
                  if (dBydAx <= dAydBx)
                    {
                      /* B is inside */
                      if (currentBy > maxmin)
                         maxmin = currentBy;
                      if (DoIntersect) 
                         STORE_THE_POINT(result, currentBx, currentBy);
                      else if (Bindex >= Blength)
                         return( maxmin );   /* B reaches end and is inside -- done */
                    }

                  lastBy = currentBy; lastBx = currentBx;
                  if (Bindex >= Blength)
                    currentBx = Xmax;
                  else
                    { currentBx = Bx[Bindex]; currentBy = By[Bindex]; }
                }  /* end of else  currentBx < currentAx */
             }
        
        } /* end of (lastAx == lastBx && lastAy == lastBy) */
        
      else  /* -- not same last points */
        {
          /* current line segments being looked at are separate and may intersect
             so do intersection and see
             
             Note: Intersection returns 0 if no intersection
                                        1 if intersection
                                        2 if collinear line segments (may overlap)
          */
          
          if (currentAx == currentBx && currentAy == currentBy)
            {
              /* current points at same place - intersect does occur and must
                 move BOTH A and B last points to current point and proceed
                 from there -- both points are inside
              */
              Aindex++; Bindex++;
              
              if (currentAy > maxmin)
                  maxmin = currentAy;

              if (DoIntersect) 
                  STORE_THE_POINT(result, currentBx, currentBy);
              else if (Aindex >= Alength || Bindex >= Blength)
                  /* hit end of A or B and inside -- done */
                  return( maxmin );
                  
              /* don't move if A or B already at end */
              if (Aindex <= Alength)
                 {
                   lastAx = currentAx; lastAy = currentAy;
                   if (Aindex == Alength)
                      currentAx = Xmax;
                   else
                      { currentAx = Ax[Aindex]; currentAy = Ay[Aindex]; }
                 }
              if (Bindex <= Blength)
                 {
                   lastBx = currentBx; lastBy = currentBy;
                   if (Bindex == Blength)
                      currentBx = Xmax;
                   else
                      { currentBx = Bx[Bindex]; currentBy = By[Bindex]; }
                 }
            }
          else
            {
              intersectFlag = line_segment_intersection(
                                lastAx, lastAy, currentAx, currentAy,
                                lastBx, lastBy, currentBx, currentBy,
                                &X, &Y );
                          
              /* If intersection occurs then we  
                 test the intersection point against maxmin since it is
                 an inside point and move the last positions of both A and B
                 to this intersection point
              */
              if (intersectFlag == 1) /* intersection exists */
                {
                  if (Y > maxmin)
                     maxmin = Y;
                     
                  lastAx = X; lastAy = Y;
                  lastBx = X; lastBy = Y;
                  
                  /* if the intersection point is one of the current points of
                     A or B then we must move the current point of that point
                     to it's next position
                  */
                  if (currentAx == X &&  currentAy == Y)
                    {
                      Aindex++;
                      if (!DoIntersect && (Aindex >= Alength))
                         return( maxmin );  /* current A is inside and hit end -- done */
                      
                      if (Aindex <= Alength)
                        {
                          if (Aindex == Alength)
                             currentAx = Xmax;
                          else
                            { currentAx = Ax[Aindex]; currentAy = Ay[Aindex]; }
                        }
                    }    
                  if (currentBx == X && currentBy == Y)
                    {
                      Bindex++;
                      if (!DoIntersect && (Bindex >= Blength))
                         return( maxmin );  /* current B is inside and hit end -- done */
                      
                      if (Bindex <= Blength)
                        {
                          if (Bindex == Blength)
                             currentBx = Xmax;
                          else
                             { currentBx = Bx[Bindex]; currentBy = By[Bindex]; }
                        }
                    }  
                  
                  if (DoIntersect) 
                     STORE_THE_POINT(result, X, Y);
                }
              else /* NO intersection or collinear line segments */
                {
                  /* In the case when x values of current positions are same
                     we can tell which point is inside; if not same x values
                     then we need to check to see if current point with smaller x
                     value is inside or not
                  */
						if (currentAx == currentBx)
                     { /* move last points of A and B to same x position */

                       /* if the line segments are collinear and they start and 
								  end at the same x values, then they must be vertical lines
                          (in fact if they are collinear they must be vertical since 
                           current x's are same and we've taken care of both current  
                           points at same location)
                          With vetical lines we have to add an extra point if the
                          line segments overlap and one is pointing up and the other down.
                          This is the tricky case with vertical line overlap. The 
                          extra point is the current x position and the max y of the 
                          overlap.
                       */
                       if (intersectFlag == 2) /* collinear line segments */
                          {
									 double overlapY = findOverlapMaxYvalue(lastAy, currentAy, lastBy, currentBy);
									 if (overlapY >= 0.0)
                              {
                                if (DoIntersect) 
                                   STORE_THE_POINT(result, currentAx, overlapY);
                                if (overlapY > maxmin)
                                   maxmin = overlapY;
                              }
                          }

                       if (DoIntersect)
                         {
                          if (currentAy > currentBy) 
                            {
                             /* current B is inside or both at same point */
                             STORE_THE_POINT(result, currentBx, currentBy);
                            }
                          else
                            {
                             /* current A is inside */
                             STORE_THE_POINT(result, currentAx, currentAy);
                            }
                          }

                       Aindex++; Bindex++;

                      if (currentAy == currentBy) 
                        {
                          /* A and B same end points ! */
                          if (currentAy > maxmin)
                             maxmin = currentAy;
                          if (!DoIntersect && (Aindex >= Alength || Bindex >= Blength))
                             /* hit end of A or B and inside -- done */
                             return( maxmin );
                        }
                      else if (currentAy < currentBy) 
                        {
                          /* current A is inside */
                          if (currentAy > maxmin)
                             maxmin = currentAy;
                          if (!DoIntersect && (Aindex >= Alength))
                             /* hit end & A inside -- done */
                             return( maxmin );
                        }
                      else
                        {
                          /* current B is inside */
                          if (currentBy > maxmin)
                             maxmin = currentBy;
                          if (!DoIntersect && (Bindex >= Blength))
                             /* hit end & B inside -- done */
                             return( maxmin );
                        }

                       /* if both already at end don't move */
                       if (Bindex <= Blength)
                         {
                           lastBx = currentBx; lastBy = currentBy;
                           if (Bindex == Blength)
                               currentBx = Xmax;
                           else
                              { currentBx = Bx[Bindex];  currentBy = By[Bindex]; }
                         }
                       if (Aindex <= Alength)
                         {
                           lastAx = currentAx; lastAy = currentAy;
                           if (Aindex == Alength)
                               currentAx = Xmax;
                           else
                              { currentAx = Ax[Aindex];  currentAy = Ay[Aindex]; }
                         }
                     }
                  else if (currentAx < currentBx)
                     {
                       double dABydBx;
                       double dBydABx;

                       /* if intersetcFlag is 2 (collinear) slopes same */
                       if (intersectFlag == 2)
                         {
                           dABydBx = 0.0;
                           dBydABx = 0.0;
                         }
                       else
                         {
                           dABydBx = (currentAy-lastBy)*(currentBx-lastBx);
                           dBydABx = (currentBy-lastBy)*(currentAx-lastBx);
                         }

                       Aindex++;

                       if (dABydBx <= dBydABx)
                          {
                            /* current A is inside */
                            if (currentAy > maxmin)
                               maxmin = currentAy;
                            if (DoIntersect)
                               STORE_THE_POINT(result, currentAx, currentAy);
                            else if (Aindex >= Alength) 
                               return( maxmin );   /* A reaches end and is inside -- done */

                            if (dABydBx == dBydABx)
                              {
                               /* if equal then there was actually an intersection!!
                                  even though the line intersection test says no intersection..
                                  this is a floating point accuracy problem --  
                                  move last point of B to the current point of A 
                               */
                               lastBy = currentAy; lastBx = currentAx;
                              }
                          }
                       
                       lastAy = currentAy; lastAx = currentAx;
                       if (Aindex >= Alength)
                          currentAx = Xmax;
                       else
                         { currentAx = Ax[Aindex]; currentAy = Ay[Aindex]; }
                      }
                   else /* currentAx > currentBx */
                      {
                        double dBAydAx;
                        double dAydBAx;

                        /* if intersetcFlag is 2 (collinear) slopes same */
                        if (intersectFlag == 2)
                          {
                            dBAydAx = 0.0;
                            dAydBAx = 0.0;
                          }
                        else
                          {
                            dBAydAx = (currentBy-lastAy)*(currentAx-lastAx);
                            dAydBAx = (currentAy-lastAy)*(currentBx-lastAx);
                          }

                        Bindex++;

                        if (dBAydAx <= dAydBAx)
                          {
                            /* B is inside */
                            if (currentBy > maxmin)
                               maxmin = currentBy;
                            if (DoIntersect)
                               STORE_THE_POINT(result, currentBx, currentBy);
                            else if (Bindex >= Blength) 
                               return( maxmin );   /* B reaches end and is inside -- done */

                            if (dBAydAx == dAydBAx)
                              {
                               /* if equal then there was actually an intersection!!
                                  even though the line intersection test says no intersection..
                                  this is a floating point accuracy problem --  
                                  move last point of A to the current point of B 
                               */
                               lastAy = currentBy; lastAx = currentBx;
                              }
                          }
                          
                        lastBy = currentBy; lastBx = currentBx;
                        if (Bindex >= Blength)
                           currentBx = Xmax;
                        else
                           { currentBx = Bx[Bindex]; currentBy = By[Bindex]; }
                      }
                } /* end of else NO Intersection */
                                  
            } /* end of if (currentAx == currentBx && Ay[Aindex] == By[Bindex]) ... else ... */
          
        } /* end of else -- not same last points */
    
    } /* end of   while (Bindex < Blength || Aindex < Alength)  loop */
  
  /* one of last points of A or B must be inside and if also > maxmin return
     that point's y value as maxmin
  */
  if (currentAy <= currentBy && currentAy > maxmin)
     maxmin = currentAy;
  else if (currentBy > maxmin)
     maxmin = currentBy;

  /* At this point must add the last point with the smallest y value if 
     it is not already the last y value in the result array. Also if the
     X value is the same as the last 2 then replace the last one.
  */
  if (DoIntersect)
    {
      i = result->n;
  
      if (currentAy < currentBy) 
        { Y = currentAy; X = currentAx; }
      else
        { Y = currentBy; X = currentBx; }

      if (Y != result->y[i-1]) 
        {
          if (i>1 && (X == result->x[i-1] && X == result->x[i-2]))
             i--;
          result->x[i] = X;
          result->y[i] = Y;
          result->n = ++i;
        }
   
      /* Also if the last 2 points are the same remove the last one --
         i.e. decrement result->n
      */
      while ((i > 1) && (result->y[i-1] == result->y[i-2]))
        {
          result->n--;
          i--;
        }
         
      /* return the fuzzy value -- compact if too much wasted space */
      if ((result->maxn - result->n) > 5)
        CompactFuzzyValue( result );

      *intersectSet = result;
     }

  return( maxmin );
    
}  /* end of maxmin_intersect routine */



/**************************************************************
    horizontal_union ()

    Returns the fuzzy set which is the union of a horizontal
    line and a fuzzy set.
 **************************************************************/
static struct fuzzy_value *horizontal_union(fv, yvalue)
  struct fuzzy_value *fv;
  double yvalue;
{
  return( horizontal_union_or_intersection(fv, yvalue, TRUE) );
}



/**************************************************************
    max_prod_scale 

    Returns the fuzzy set which is scaled by:

         yvalue/maxYValueOfSet   

    if maximum y value of set is > yvalue; otherwise the set remains
    the same.
      
 **************************************************************/
static struct fuzzy_value *max_prod_scale(fv, yvalue)
  struct fuzzy_value *fv;
  double yvalue;
{
  double maxYValueOfSet;
  struct fuzzy_value *fvResult;
  double *fvy;
  double scale;
  int i, n;
  
  fvResult = CopyFuzzyValue( fv );

  /* find the max value in the set */
  fvy = fvResult->y;
  n = fvResult->n;

  maxYValueOfSet = fvy[0];

  for (i=1; i<n; i++)
     if (fvy[i] > maxYValueOfSet)
        maxYValueOfSet = fvy[i];

  /* do scaling if needed */

  if (maxYValueOfSet <= yvalue)
     return( fvResult );

  scale = yvalue/maxYValueOfSet;

  for (i=0; i<n; i++)
      fvy[i] = fvy[i] * scale;

  return( fvResult );
  
}



/**************************************************************
    horizontal_intersection ()

    Returns the fuzzy set which is the intersection of a horizontal
    line and a fuzzy set.
 **************************************************************/
static struct fuzzy_value *horizontal_intersection(fv, yvalue)
  struct fuzzy_value *fv;
  double yvalue;
{
  return( horizontal_union_or_intersection(fv, yvalue, FALSE) );
}



/*****************************************************************
    horizontal_union_or_intersection ()

    Returns the fuzzy set which is the union/intersection  of a 
    horizontal line and a fuzzy set.
    
    If unionFlg is true return a union else return an intersection
 *****************************************************************/
static struct fuzzy_value *horizontal_union_or_intersection(fv, yvalue, unionFlg)
  struct fuzzy_value *fv;
  double yvalue;
  int unionFlg;
{
    int i, num, count, newmax;
    struct fuzzy_value *newfv;
    double previousy, currenty, X;
    double *newX, *newY;
    double *fvX, *fvY;
    
    /* if union and the yvalue is 0.0 -- nothing to do really - just return
       a copy of the fv passed to us since 0.0 is min y value possible
       
       if intersection and the yvalue is 1.0 -- nothing to do really - just return
       a copy of the fv passed to us since 1.0 is max y value possible
    */
    
    if ((unionFlg && yvalue == 0.0) ||
        (!unionFlg && yvalue == 1.0))
       return( CopyFuzzyValue(fv) );
       

    newfv = get_struct(fuzzy_value);
    newfv->name = NULL;
    newfv->whichDeftemplate = fv->whichDeftemplate;
    num = fv->n;

    /* Max number of points due to horizontal union is
           
            (3N -1 )/2     
       
       - this is worst case when fuzzy set is a saw tooth above
         and below the line on each alternate point
    */
    newmax = (3*num - 1)/2;    
    newfv->maxn = newmax;
    newfv->x = FgetArray(newmax);
    newfv->y = FgetArray(newmax);
    
    newX = newfv->x;
    newY = newfv->y;
    
    fvX = fv->x;
    fvY = fv->y;

    currenty = fvY[0];
    
    if ( (unionFlg && (currenty >= yvalue))  ||
        (!unionFlg && (currenty <= yvalue)) )
      {
        newX[0] = fvX[0];
        newY[0] = fvY[0];
        count = 1;
      }
    else
        count = 0;
    
    for (i=1; i < num; i++)
      {
        previousy = currenty;
        currenty = fvY[i];
        if ( previousy < yvalue && currenty > yvalue || previousy > yvalue && currenty < yvalue )
          { /* there is an intersection with the horizontal line */
            
            X = fvX[i-1] + (fvX[i] - fvX[i-1]) * (yvalue - fvY[i-1]) / (fvY[i] - fvY[i-1]);
            /* If same point as last one then don't add it */
            if (count == 0 || !FZ_EQUAL(X, newX[count-1]) || !FZ_EQUAL(yvalue, newY[count-1]))
              {
            
               /* If same y value 3 points in a row or in 1st 2 places replace 
                  last point with this one
               */
               if ((count == 1 && newY[0] == yvalue) ||
                   (count > 2 && yvalue == newY[count-1] && yvalue == newY[count-2]) ||
                   (count > 2 && X == newX[count-1] && X == newX[count-2]) 
                  )
                 count--; 
               
               newX[count] = X;
               newY[count] = yvalue;
               count++;

              /* if last 3 points all have same x values and all y values
                 are in increasing order or all in decreasing order, then
                 replace 2nd one with the last one. If last 4 are the same
                 replace 2nd last one with last one.
              */
              if (count > 2 && X == newX[count-2] && X == newX[count-3])
                {
                 if ((yvalue > newY[count-2] && newY[count-2] > newY[count-3]) ||
                     (yvalue < newY[count-2] && newY[count-2] < newY[count-3]) ||
                     (count > 3 && X == newX[count-4])
                    )
                   { count--; newY[count-1] = newY[count]; }
                 }
              }
          }
        if ( (unionFlg && (currenty >= yvalue))  ||
            (!unionFlg && (currenty <= yvalue)) )
          {
            /* as above  -- watch for unecessary points being added */

            /* If same point as last one then don't add it */
            if (count ==  0 || !FZ_EQUAL(fvX[i], newX[count-1]) || !FZ_EQUAL(currenty, newY[count-1]))
              {
               if ((count == 1 && newY[0] == currenty) ||
                   (count > 2 && currenty == newY[count-1] && currenty == newY[count-2]) ||
                   (count > 2 && fvX[i] == newX[count-1] && fvX[i] == newX[count-2]) 
                  )
                  count--; 
               
               newX[count] = fvX[i];
               newY[count] = currenty;
               count++;

               if (count > 2 && fvX[i] == newX[count-2] && fvX[i] == newX[count-3])
                 {
                  if ((currenty > newY[count-2] && newY[count-2] > newY[count-3]) ||
                      (currenty < newY[count-2] && newY[count-2] < newY[count-3]) ||
                      (count > 3 && fvX[i] == newX[count-4])
                     )
                    { count--; newY[count-1] = newY[count]; }
                  }
              }
          }
      }
    
    if (count > 2 && newY[count-1] == newY[count-2])
       count--;  /* remove last point if same y value as 2nd last */
       
    else if (count == 0)        /* All set points are below the line */
      {
        /* NOTE: we return a single point fuzzy set; this is treated
                 as a fuzzy set with a constant value (yvalue) over
                 entire universe of discourse -- NOT a crisp value!!
        */
        newX[0] = fvX[0];
        newY[0] = yvalue;
        count = 1;
      }
    newfv->n = count;
    
    /* use CompactFuzzyValue to make sure we don't have a terribly 
       oversized array
    */
    if (newmax-count > 5) 
         CompactFuzzyValue(newfv);
    
    return (newfv);
}        
            





/*********************************************************************
    funion()

    returns the union of two fuzzy sets

    Expects that both f1 anf f2 fuzzy values are associated with the 
    same fuzzy deftemplate -- if not shouldn't be doing the intersection
    (like adding apples and oranges)

    User of this routine is expected to maintain the correct value
    in the "name" field of the resulting fuzzy value

 *********************************************************************/
globle struct fuzzy_value *funion(f1,f2)
  struct fuzzy_value *f1,*f2;
{
  struct fuzzy_value *result;
  double *Ax, *Ay, *Bx, *By;
  int Alength, Blength;
  double lastAx, lastAy, lastBx, lastBy;
  double currentAx, currentBx;
  double currentAy, currentBy;
  double Xmax;
  double X, Y;
  int Aindex, Bindex;
  int intersectFlag;
  int i, size;
  
  /* extract the sets from the Fuzzy value */
  Ax = f1->x;
  Ay = f1->y;
  Alength = f1->n;
  
  Bx = f2->x;
  By = f2->y;
  Blength = f2->n;

  /* If both sets have only 1 member then they are constant fuzzy sets
     so just pick the max of the sets as the one to return
  */
  if (Alength == 1 && Blength == 1)
     return( Ay[0] < By[0] ? CopyFuzzyValue( f2 ) : CopyFuzzyValue( f1 ) );
     
  /* if only 1 of the 2 sets has 1 element then this is a simpler horizontal
     union since it intersects the other set horizontally.
  */
  if (Alength == 1)
    {
      return( horizontal_union(f2, Ay[0]) );
     }
    
  if (Blength == 1)
    {
      return( horizontal_union(f1, By[0]) );
    }
   
  
  /* The two sets may not intersect. */
  if (nonintersectiontest(Ax, Ay, Bx, By, Alength, Blength))
     {
       /* just return a simple concatenate of the 2 sets in this case...
          BUT recall that nonintersectiontest may return a true result
          if one (or both?) of the sets is all zeros... better check
          that during the concatenate or before
       */

       /* see if one set is all zeros */
       for (i = 0; i<Alength; i++)
          if (Ay[i] != 0.0)
                break;
       if (i == Alength) /* then all y values were zero just return other set */
              return( CopyFuzzyValue(f2) );

       for (i = 0; i<Blength; i++)
          if (By[i] != 0.0)
                break;
       if (i == Blength) /* then all y values were zero just return other set */
              return( CopyFuzzyValue(f1) );

       result = get_struct(fuzzy_value);
  
       size = Alength + Blength;
       result->name = NULL;
       result->whichDeftemplate = f1->whichDeftemplate;
       result->x = FgetArray( size );
       result->y = FgetArray( size );
       result->n = 0;
       result->maxn = size;
  
       if (Ax[Alength-1] <= Bx[0])
          concatenate( Ax, Ay, Alength, Bx, By, Blength, result );
       else
          concatenate( Bx, By, Blength, Ax, Ay, Alength, result );
       return( result ); 
     }
        
  /* Create the fuzzy value to return to the caller and the sets that will
     be stored in the fuzzy value. Note we allocate an initial x and y
     array to hold the union. The initial size of each is set to
     
       (Alength*3-1)/2 + (Blength*3-1)/2
       
     This value was determined to be the worst case size -- I think it is correct
     or 1 or 2 too large?!!
  */
  result = get_struct(fuzzy_value);
  
  size = (Alength*3-1)/2 + (Blength*3-1)/2 + 1;  /* 1 extra for safety */
  result->x = FgetArray( size );
  result->y = FgetArray( size );
  result->n = 0;
  result->maxn = size;
  result->name = NULL;
  result->whichDeftemplate = f1->whichDeftemplate;
  
  /* Set up to find the union of the 2 fuzzy sets. Pick the starting point by
     determining which of the 1st points of either set is inside (part of the
     union) and set the starting x position to be the same for each set.
  */
  lastAy = Ay[0];
  lastBy = By[0];
  
  if (Ax[0] == Bx[0])  /* Set A and B same 1st position */
    {
      lastAx = Ax[0]; /* set same x values */
      lastBx = Bx[0];
      
      Aindex = 1;
      Bindex = 1;
    }
  else if (Ax[0] <= Bx[0]) /* Set A has 1st x position */
    {
      lastAx = Ax[0]; /* set same x values */
      lastBx = Ax[0];
      
      Aindex = 1;
      Bindex = 0;
    }
  else /*  Ax[0] > Bx[0] -- Set B has 1st x position */
    {
      lastBx = Bx[0]; /* set same x values */
      lastAx = Bx[0];
      
      Aindex = 0;
      Bindex = 1;
    } 
      
  if (lastAy >= lastBy) /* A set has 1st point outside */
    {
      STORE_THE_POINT(result, lastAx, lastAy);
    }
  else /* B set has 1st point outside */
    {
      STORE_THE_POINT(result, lastBx, lastBy);
    }
    
  /* store the largest X value in either of the 2 sets */
  Xmax = (Ax[Alength -1] >= Bx[Blength-1]) ? Ax[Alength -1] : Bx[Blength-1];
    
  /* remember the current positions of the A and B sets -- just need to
     keep the x values, the y values can be picked up using Aindex, Bindex
     when required but the x values may have to be taken from an extended 
     x value (last point of the other set) on reaching its last x value
  */
  currentAx = Ax[Aindex];
  currentBx = Bx[Bindex];
  currentAy = Ay[Aindex];
  currentBy = By[Bindex];
    
  /* now we are ready to walk through the 2 sets looking for points outside
     the intersection of the 2 sets (actually points of intersection between lines
     in the 2 sets and those with max value at same x location define the union)
  */
  
  /* terminates when both sets must have reached their end */
  while (Bindex < Blength || Aindex < Alength) 
    {
      /* if the last points being considered for the 2 sets are in fact the
         same points then by comparing the slopes of the 2 line segments
         we can tell which current point is outside (bigger slope)
      */
      if (lastAx == lastBx && lastAy == lastBy)
        {
          /* if the current x values of the 2 sets is the same then the
             checking of the slopes is easy since the maximum of the 
             current y values is the maximum slope and that one is outside
          */
          if (currentAx == currentBx)
            { /* move last points of A and B to same x position */
              if (currentAy <= currentBy) 
                {
                  /* current B is outside or both at same point */
                  STORE_THE_POINT(result, currentBx, currentBy);
                }
              else
                {
                  /* current A is outside */
                  STORE_THE_POINT(result, currentAx, currentAy);
                }
              Aindex++; Bindex++;
              /* don't move A or B if already at end */
              if (Aindex <= Alength)
                 {
                  lastAx = currentAx; lastAy = currentAy;
                  if (Aindex == Alength)
                      currentAx = Xmax;
                  else
                      { currentAx = Ax[Aindex]; currentAy = Ay[Aindex];}
                 }

              if (Bindex <= Blength)
                 {
                  lastBx = currentBx; lastBy = currentBy;
                  if (Bindex == Blength)
                      currentBx = Xmax;
                  else
                      { currentBx = Bx[Bindex]; currentBy = By[Bindex]; }
                 }
             }
          /* if the current x values not the same must do a calc to compare 
             slopes

                         dAy/dAx compared to dBy/dBx

              If dAx = 0 set slopes to 1 and 0 
              If dBx = 0 set slopes to 0 and 1 
          */
          else 
            { double dAydBx;  
              double dBydAx;

              if (currentAx == lastAx)
                 { dAydBx = (currentAy > lastAy) ? 1.0 : -1.0; 
                   dBydAx = 0.0; 
                 }
              else if (currentBx == lastBx)
                 { dAydBx = 0.0; 
                   dBydAx = (currentBy > lastBy) ? 1.0 : -1.0;
                 }
              else
                 { dAydBx = (currentAy-lastAy)*(currentBx-lastBx);
                   dBydAx = (currentBy-lastBy)*(currentAx-lastAx);
                 }

              if (currentAx < currentBx)
                {            
                  if (dAydBx >= dBydAx)
                    {
                      /* B may or may not be inside - only know A is outside */
                      STORE_THE_POINT(result, currentAx, currentAy);
                    }
                    
                  lastAy = currentAy; lastAx = currentAx;
                  Aindex++;
              
                  if (Aindex >= Alength)
                     currentAx = Xmax;
                  else
                     { currentAx = Ax[Aindex];  currentAy = Ay[Aindex]; }

                  if (dAydBx == dBydAx)
                    {
                     /* slopes equal -- lines are collinear -- move both
                        points to current A position - A already moved.
                        A is outside
                     */
                     lastBy = lastAy; lastBx = lastAx;
                    }
                }
              else  /* currentBx < currentAx */
                {
                  if (dBydAx >= dAydBx)
                    {
                      /* A may be inside -- only know B is outside */
                      STORE_THE_POINT(result, currentBx, currentBy);
                    }
                    
                  lastBy = currentBy; lastBx = currentBx;
                  Bindex++;
              
                  if (Bindex >= Blength) 
                     currentBx = Xmax;
                  else
                     { currentBx = Bx[Bindex]; currentBy = By[Bindex]; }

                  if (dBydAx == dAydBx)
                    {
                     /* slopes equal -- lines are collinear -- move both
                        points to current B position - B already moved.
                        B is outside
                     */
                     lastAy = lastBy; lastAx = lastBx;
                    }
                }
            }
        
        } /* end of (lastAx == lastBx && lastAy == lastBy) */
        
      else  /* -- not same last points */
        {
          /* current line segments being looked at are separate and may intersect
             so do intersection and see
             
             Note: Intersection returns 0 if no intersection
                                        1 if intersection
                                        2 if collinear lines -- may or may not overlap
          */
          
          if (currentAx == currentBx && currentAy == currentBy)
            {
              STORE_THE_POINT(result, currentBx, currentBy);
              
              /* current points at same place - intersect does occur and must
                 move BOTH A and B last points to current point and proceed
                 from there -- both points are outside
              */
              Aindex++; Bindex++;
              
              /* don't move A or B if already at end */
              if (Aindex <= Alength)
                {
                  lastAx = currentAx; lastAy = currentAy;
                  if (Aindex == Alength)
                     currentAx = Xmax;
                  else
                     { currentAx = Ax[Aindex]; currentAy = Ay[Aindex]; }
                }
              
              if (Bindex <= Blength)
                {
                  lastBx = currentBx; lastBy = currentBy;
                  if (Bindex == Blength)
                     currentBx = Xmax;
                  else
                     { currentBx = Bx[Bindex]; currentBy = By[Bindex]; }
                }
            }
          else
            {
              intersectFlag = line_segment_intersection(
                                lastAx, lastAy, currentAx, currentAy,
                                lastBx, lastBy, currentBx, currentBy,
                                &X, &Y );
          
                
              /* In the case when x values of current positions are same
                 we can tell which point is outside; if not same x values
                 then we need to check to see if current point with bigger x
                 value is outside or not; if intersection occurs then we  
                 store it and move the last positions of both A and B
                 to this intersection point
              */
              if (intersectFlag == 1) /* intersection exists */
                {
                  lastAx = X; lastAy = Y;
                  lastBx = X; lastBy = Y;
                  
                  /* if the intersection point is one of the current points of
                     A or B then we must move the current point of that point
                     to it's next position
                  */
                  if (currentAx == X && currentAy == Y) 
                    {
                      Aindex++;
                        if (Aindex <= Alength) /* don't move A if already at end */
                        {
                          if (Aindex == Alength)
                             currentAx = Xmax;
                          else 
                             { currentAx = Ax[Aindex]; currentAy = Ay[Aindex]; }
                        }
                    }    
                  if (currentBx == X && currentBy == Y) 
                    {
                      Bindex++;
                      if (Bindex <= Blength) /* don't move B if already at end */
                        {
                          if (Bindex == Blength)
                             currentBx = Xmax;
                          else 
                             { currentBx = Bx[Bindex]; currentBy = By[Bindex]; }
                        }
                    }
                             
                  STORE_THE_POINT(result, X, Y);
                }
              else /* NO intersection */
                {
                  if (currentAx == currentBx)
                     { /* move last points of A and B to same x position */

                       if (currentAy <= currentBy) 
                         {
                           /* current A is inside B is outside */
                           STORE_THE_POINT(result, currentBx, currentBy);
                         }
                       else
                         {
                           /* current B is inside A is outside */
                           STORE_THE_POINT(result, currentAx, currentAy);
                         }

                        Aindex++; Bindex++;
              
                       /* don't move A or B if already at end */
                       if (Aindex <= Alength)
                         {
                           lastAx = currentAx; lastAy = currentAy;
                           if (Aindex == Alength)
                              currentAx = Xmax;
                           else
                              { currentAx = Ax[Aindex]; currentAy = Ay[Aindex]; }
                         }
              
                       if (Bindex <= Blength)
                         {
                           lastBx = currentBx; lastBy = currentBy;
                           if (Bindex == Blength)
                              currentBx = Xmax;
                           else
                              { currentBx = Bx[Bindex]; currentBy = By[Bindex]; }
                         }

                      }
                  else if (currentAx < currentBx)
                     {
                       double dABydBx;
                       double dBydABx;

                       /* if intersectFlag is 2 (collinear) slopes same */
                       if (intersectFlag == 2)
                         {
                           dABydBx = 0.0;
                           dBydABx = 0.0;
                         }
                       else
                         {
                           dABydBx = (currentAy-lastBy)*(currentBx-lastBx);
                           dBydABx = (currentBy-lastBy)*(currentAx-lastBx);
                         }

                       if (dABydBx >= dBydABx)
                         {
                          /* B may or may not be inside - only know A is outside */
                          STORE_THE_POINT(result, currentAx, currentAy);

                          if (dABydBx == dBydABx)
                            {
                             /* if equal then there was actually an intersection!!
                                even though the line intersection test says no intersection..
                                this is a floating point accuracy problem --  
                                move last point of B to the current point of A 
                             */
                             lastBy = currentAy; lastBx = currentAx;
                            }
                         }

                       lastAy = currentAy; lastAx = currentAx;
                       Aindex++;

                       if (Aindex >= Alength)
                          currentAx = Xmax;
                       else
                          { currentAx = Ax[Aindex]; currentAy = Ay[Aindex]; }
                     }
                  else /* currentAx > currentBx */
                     {
                       double dBAydAx;
                       double dAydBAx;

                       /* if intersectFlag is 2 (collinear) slopes same */
                        if (intersectFlag == 2)
                         {
                           dBAydAx = 0.0;
                           dAydBAx = 0.0;
                         }
                       else
                         {
                           dBAydAx = (currentBy-lastAy)*(currentAx-lastAx);
                           dAydBAx = (currentAy-lastAy)*(currentBx-lastAx);
                         }

                       if (dBAydAx >= dAydBAx)
                         {
                           /* A may be inside -- only know B is outside */
                           STORE_THE_POINT(result, currentBx, currentBy);

                           if (dBAydAx == dAydBAx)
                             {
                              /* if equal then there was actually an intersection!!
                                 even though the line intersection test says no intersection..
                                 this is a floating point accuracy problem --  
                                 move last point of A to the current point of B 
                              */
                              lastAy = currentBy; lastAx = currentBx;
                             }
                         }

                       lastBy = currentBy; lastBx = currentBx;
                       Bindex++;
 
                       if (Bindex >= Blength) 
                          currentBx = Xmax;
                       else
                          { currentBx = Bx[Bindex]; currentBy = By[Bindex]; }
                     }
                } /* end of else NO Intersection */
                                  
            } /* end of if (currentAx == currentBx && Ay[Aindex] == By[Bindex]) ... else ... */
          
        } /* end of else -- not same last points */
    
    } /* end of  while (Bindex < Blength || Aindex < Alength)  loop */

  /* At this point must add the last point with the biggest y value if 
     it is not already the last y value in the result array. Also if the
     X value is the same as the last 2 then replace the last one.
  */
  i = result->n;
  
  if (currentAy > currentBy) 
     { Y = currentAy; X = currentAx; }
  else
     { Y = currentBy; X = currentBx; }

  if (Y != result->y[i-1])
     {
       if (i>1 && (X == result->x[i-1] && X == result->x[i-2]))
          i--;
       result->x[i] = X;
       result->y[i] = Y;
       result->n = ++i;
     }

  /* Also if the last 2 points are the same remove the last one --
     i.e. decrement result->n
  */
  while ((i > 1) && (result->y[i-1] == result->y[i-2]))
     {
       result->n--;
       i--;
     }
         
  /* return the fuzzy value -- compact if too much wasted space */
  if ((result->maxn - result->n) > 5)
     CompactFuzzyValue( result );
     
  return( result );
    
}  /* end of funion routine */

            

/************************************************************************
    nonintersectiontest()

    returns TRUE if the intersection set of A and B is NULL set, 
    (i.e. no overlapping region), or if either set A or B have
    all y values equal to zero.

    NOTE: will return true if even 1 point in common
 ************************************************************************/
globle int nonintersectiontest ( Ax, Ay, Bx, By, Asize, Bsize )
  double *Ax, *Ay, *Bx, *By;
  int Asize, Bsize;
{
    int count, allzeros_A, i;
    int firstAisnonzero, firstAiszero, firstBisnonzero, firstBiszero;
    int lastAisnonzero, lastAiszero, lastBisnonzero, lastBiszero;

	 /***********************************
     test for non-overlapping sets

     if either set has only 1 point then
     will overlap unless 1 or both sets
     have zero y values
     ***********************************/
    if (Asize > 1 && Bsize > 1)
      {
        /* This can be a little tricky when we have the possibility of
           several points in a row at the beginning or end of the set
           with the same x values. Basically if the last point of 1 set
           is before or equal to the 1st point of the other and these
           points have a zero y value then they do not intersect. BUT
           if there are several points wth same x value at end or beginning
           we need to consider if the 1st and last of the sequence are both
           zero with non zero points in between -- use this info to decide.
           If 1st and last pts are zero with non-zero in bewteen then this is
           like a zero unless the other set does the same thing and the sets
           just touch one another. 

                 *      *
                 *     * *
                 *    *   *            No intersect here
                 *   *     *
                 *  *       *
                 * *         *
                 *            *

               *           *
              * *         * *
             *   *       *   *            No intersect here
            *     *     *     *
           *       *   *       *
          *         * *         *
         *           *           *

         BUT if both are 'spikes' that start and end with zero yvalues
         at begining and end there is an overlap!

        */
        firstAiszero = (Ay[0] == 0.0);
        lastBiszero  = (By[Bsize-1] == 0.0);
          
        if ((Bx[Bsize-1] < Ax[0]) && firstAiszero && lastBiszero)
           return( TRUE );
          
        if ((Bx[Bsize-1] == Ax[0]) && firstAiszero && lastBiszero)
           /* the beginning of A and end of B touch */
          {
            firstAisnonzero = FALSE;
            lastBisnonzero = FALSE;
            for (i=1; i<Asize; i++)
             {
              if (Ax[i] != Ax[0]) break;
              if (Ay[i] != 0.0)
                {
                  firstAisnonzero = TRUE;
                  break;
                }
             }
            for (i=Bsize-2; i>=0; i--)
             {
              if (Bx[i] != Bx[Bsize-1]) break;
              if (By[i] != 0.0)
                {
                  lastBisnonzero = TRUE;
                  break;
                }
             }
            if (!firstAisnonzero || !lastBisnonzero)
              return( TRUE );
          }

        firstBiszero = (By[0] == 0.0);
        lastAiszero  = (Ay[Asize-1] == 0.0);
         
        if ((Ax[Asize-1] < Bx[0]) && firstBiszero && lastAiszero)
           return( TRUE );
          
        if ((Ax[Asize-1] == Bx[0]) && firstBiszero && lastAiszero)
           /* the beginning of B and end of A touch */
           {
            firstBisnonzero = FALSE;
            lastAisnonzero = FALSE;
            for (i=1; i<Bsize; i++)
             {
              if (Bx[i] != Bx[0]) break;
              if (By[i] != 0.0)
                {
                  firstBisnonzero = TRUE;
                  break;
                }
             }
            for (i=Asize-2; i>=0; i--)
             {
              if (Ax[i] != Ax[Asize-1]) break;
              if (Ay[i] != 0.0)
                {
                  lastAisnonzero = TRUE;
                  break;
                }
             }
            if (!firstBisnonzero || !lastAisnonzero)
              return( TRUE );
          }
      }

    /*******************************************************
    Sets may overlap unless either set contains all zero y
    values -- then no intersection occurs, and return TRUE.
    ********************************************************/
    count = 0;
    allzeros_A = TRUE;
    while ( count < Asize )
      {
        if (Ay[count] > 0.0 )
          {
            allzeros_A = FALSE;
            break;
          }
        count++;
      }
    if (allzeros_A)
        return TRUE; /* set A all zeros -- no intersection */
    
    count = 0;
    while ( count < Bsize )
      {
        if (By[count] > 0.0)
            return FALSE; /* a non-zero -- sets overlap */
        count++;
      }
    return TRUE; /* set B all zeros -- no intersection */
}


/*************************************************************************
    concatenate

    This function will concatenate set1x & set2x and set1y and set2y 
    storing result in fuzzy set newValue.  If the last point
    of set1 is equal to the first point of set2, this common middle
    point is not repeated.
 *************************************************************************/
static void concatenate ( set1x, set1y, length1, set2x, set2y, length2, newValue )
  double *set1x, *set1y;
  int length1;
  double *set2x, *set2y;
  int length2;
  struct fuzzy_value *newValue;
{
    double *x = newValue->x;
    double *y = newValue->y;
    int i;
    int j;
    
    for ( i=0; i < length1; i++ )
      {
        x[i] = set1x[i];
        y[i] = set1y[i];
      }
      
    j = length1;
    
    if ( (set1x[length1-1] != set2x[0] || set1y[length1-1] != set2y[0]) )
      {
        x[j] = set2x[0];
        y[j] = set2y[0];
        j++;
      }
    for ( i=1; i < length2; i++ )
      {
        x[j] = set2x[i];
        y[j] = set2y[i];
        j++;
      }
    newValue->n = j;
}        


/*******************************************************************/
/* computeMinOfMaxmins:                                            */
/*                                                                 */
/* computes the min of max-mins of fuzzy patterns in the antecedent*/
/* of the rule with the facts that they matched                    */
/* !!!!should ONLY do this once for each rule firing               */
/*     -- not for each asserted fuzzy fact!!!                      */
/*     -- and ONLY for FUZZY_FUZZY rules                           */
/*******************************************************************/
static void computeMinOfMaxmins()
  {
    struct genericMatch *antecedent_binds;
    double currenty, minimumy;
    struct fuzzy_value *fact_fv, *antecedent_fv;
    struct fact *tmpFact;
    int i;
    
    /**********************************************************
     Loop through all antecedents and calculate the max-min
     y value for all fuzzy antecedents and their associated 
     facts.  Keep track of the minimum y value of the max-mins.
     The result of the compositional rule for all antecedents
     is then the consequence set cut off by the horizontal line
     y = minimum(max-mins of all antecedent-fact pairs).
    ***********************************************************/
    
    /* GlobalLHSBinds is ptr to partial match for rule */
    antecedent_binds = &(GlobalLHSBinds->binds[0]); 
        
    minimumy = 1.0;
    
    for (i=0; i<(unsigned int)GlobalLHSBinds->bcount; i++)
      {
        int j;

        /* ExecutingRule->pattern_fv_arrayPtr is ptr to array of fuzzy value
           hashNode ptrs of connected to fuzzy patterns in the rule's LHS
           Each of these ptrs points to a fuzzy value connected to a 
           fuzzy pattern and slot within that pattern in the rule's LHS.
           NOTE!! if there are no fuzzy patterns on the LHS of the rule then
           there are NO entries in this array and numberOfPatterns == 0
        */
        if (ExecutingRule->numberOfFuzzySlots > 0)
          {
           /* find fuzzy values of the fact which matched the pattern */
           tmpFact = (struct fact *)(antecedent_binds[i].gm.theMatch->matchingItem);

           if (tmpFact == NULL)  /* may be NULL here if NOT condition for pattern */
              continue;          /* ignore it -- treat as max-of-min of 1.0 */

           /* search for any fuzzy slots in this pattern and do a max-mof-min
              calculation for each one against the matching fact slot -- if 
              less than the current minimumy use that min value
           */
           for (j=0; j<ExecutingRule->numberOfFuzzySlots; j++)
             {
               int slotNum, patternNum;
               struct fzSlotLocator *patFvArrayPtrj = (ExecutingRule->pattern_fv_arrayPtr) + j;

               patternNum = patFvArrayPtrj->patternNum;

               if (patternNum > i) break; /* past FV's for this pattern */

               if (patternNum == i) 
                  {
                    /* this pattern has a fuzzy slot */
                    slotNum = patFvArrayPtrj->slotNum;
                    /* get FV for pattern and the matching FV in the fact */
                    antecedent_fv = (struct fuzzy_value *) 
                            ValueToFuzzyValue(patFvArrayPtrj->fvhnPtr); 
                    
                    fact_fv = (struct fuzzy_value *)
                              ValueToFuzzyValue((tmpFact->theProposition.theFields[slotNum].value));
                   
                    currenty = max_of_min (fact_fv,  antecedent_fv);
 
                    if ( currenty < minimumy )
                         minimumy = currenty;
                  }
          
             } /* end of   for (j=0; ...)  */
          }
      } /* end of   for (i=0; ...)  */

     ExecutingRule->min_of_maxmins = minimumy;
  }
    

/*******************************************************************/
/* computeFuzzyConsequence:                                        */
/*                                                                 */
/* computes the consequent fuzzy set based on the fuzzy set of the */
/* assert in the conclusion and the match of fuzzy facts in the    */
/* antecedent of the rule                                          */
/*   MUST be a FUZZY_FUZZY rule                                    */
/*   We already know RHS is fuzzy or we wouldn't be calling this   */
/*      function - so only need to check the LHS type              */
/*   The new fact has not been installed yet so no need to         */
/*      deinstall it (or the fuzzy value in it)                    */
/*   ALSO always make a copy of the fuzzy value to operate on      */
/*******************************************************************/
globle void computeFuzzyConsequence(new_fact)
  struct fact *new_fact;
  {
    struct fuzzy_value *consequence, *conclusion;
    struct field  *elem;
    int i, length;
   
    /**********************************************************
     check if the fuzzy rhs pattern is being processed rather 
     than an assert from command level -- if from command level
     then there is no lhs fuzzy pattern matches that will 
     influence the value of the conclusion
    **********************************************************/
    if ((ExecutingRule != NULL) &&
        (ExecutingRule->executing) &&
        (ExecutingRule->lhsRuleType == FUZZY_LHS))
      {
        /********************************************************
         Calculate the min_of_maxmins if not done yet 
         (-1.0 if not yet calculated)
        *********************************************************/
        if ( ExecutingRule->min_of_maxmins == -1.0 ) 
           computeMinOfMaxmins();

        /********************************************************
         Calculate conclusion by adjusting the consequence set(s)
         if necessary.  Assign the conclusion set to the new fact
         being asserted, and if the set has been altered, indicate
         the new name as "???"

         Note: a fuzzy fact may have several fuzzy values in it
               and each one must be adjusted.

         Note: if the inferecing type is 'max-min' we cut the
               consequence set (horizontal_intersection) and if
               it is 'max-prod' we scale the consequence set
               (max_prod_scale).
        *********************************************************/
        if ( ExecutingRule->min_of_maxmins != 1.0 ) /* no change necessary if min is 1.0 */
          {
            /* get pointers to the fields of the facts */
            length = (&new_fact->theProposition)->multifieldLength;
            elem   = (&new_fact->theProposition)->theFields;
   
            /* for each slot see if a fuzzy slot and if so compute Fuzzy Consequence  */
 
            for (i=0; i<length; i++)
              {
                if (elem[i].type != FUZZY_VALUE)
                continue; 
       
                consequence = (struct fuzzy_value *) 
                              ValueToFuzzyValue(elem[i].value);
                if (FuzzyInferenceType == MAXMIN)
                    conclusion = horizontal_intersection(consequence, ExecutingRule->min_of_maxmins );
                else
                    conclusion = max_prod_scale(consequence, ExecutingRule->min_of_maxmins );

                if (conclusion->name != NULL)
                    rm(conclusion->name, strlen(conclusion->name)+1);
                conclusion->name = gm2(4);
                strcpy(conclusion->name, "???");
                elem[i].value = (void *)AddFuzzyValue(conclusion);
                /* AddFuzzyValue copies the fuzzy variable so we need to remove 
                   conclusion fuzzy value since it will no longer be pointed to
                */
                rtnFuzzyValue(conclusion);
           
                /* NOTE: the consequence does NOT need to be deinstalled even though we replace
                         it by the conclusion fuzzy value. This is because the conclusion
                         fuzzy value has not yet been installed.
                */
           
                /* NOTE: the new fact has not yet been installed therefore the fuzzy value
                   has not been installed (count incremented) ... no need to
                   deinstall it or return it since hash values are removed as needed
                   when their count hits zero
                */
              }
          }        
      }
  }



/*******************************************************************
    FUNCTION FOR CHANGING FUZZY FACTS DUE TO
    GLOBAL CONTRIBUTION
 *******************************************************************/


/************************************************************/
/* changeValueOfFuzzySlots;                                 */
/*                                                          */
/*  Global contribution !!  Done when adding fact with fuzzy*/
/*  slots that matches existing fact.                       */
/*       SEE HandleExistingFuzzyFact routine in FACTHSH.C   */
/*                                                          */
/*  fact2 is new fact, fact1 is already in hash table       */
/*  fact2 has not yet been installed (FactInstall)          */
/*                                                          */
/*  modify each fuzzy slot in fact2                         */
/*                                                          */
/************************************************************/
globle void changeValueOfFuzzySlots(fact1,fact2)
  struct fact *fact1, *fact2;
  {
   struct field *elem1, *elem2;
   int i, length;
   FUZZY_VALUE_HN *fv1_hn, *fv2_hn; 
   struct fuzzy_value *fv1_ptr, *fv2_ptr, *theUnion;

   /* combine two facts with fuzzy slots -- fact2 is the existing fact in hash tbl */

   /* get pointers to the fields of the facts */
   length = (&fact1->theProposition)->multifieldLength;
   elem1 = (&fact1->theProposition)->theFields;
   elem2 = (&fact2->theProposition)->theFields;
   
   /* for each slot see if a fuzzy slot and if so perform global contribution */

   for (i=0; i<length; i++)
     {
       if (elem1[i].type != FUZZY_VALUE)
         continue; 
       
       /* If the  fuzzy values point to the same hashnode, then no need to do
          a union since they are the same sets
       */
       fv1_hn = (FUZZY_VALUE_HN *)(elem1[i].value);
       fv2_hn = (FUZZY_VALUE_HN *)(elem2[i].value);
       
       if (fv1_hn == fv2_hn)
          continue;
          
       fv1_ptr = (struct fuzzy_value *)ValueToFuzzyValue(fv1_hn);
       fv2_ptr = (struct fuzzy_value *)ValueToFuzzyValue(fv2_hn);
       
       /* if we need to perform the union (global contribution) then we are changing
          the existing fuzzy value associated with fact ... therefore we must 
          deinstall the original fuzzy value and Add the new one to the hash table
          and install it
       */
       
       theUnion = funion(fv1_ptr, fv2_ptr);
       if (theUnion->name != NULL)
           rm(theUnion->name, strlen(theUnion->name)+1);
       theUnion->name = gm2(4);
       strcpy(theUnion->name, "???");
      /* the name should be [ name_of_fv1 ] OR [ name_of_fv2 ] 
          unless one of the names is ??? 
       */
       if (strcmp(fv1_ptr->name, theUnion->name) != 0 && strcmp(fv2_ptr->name, theUnion->name) != 0)
         {
           char *str, *strPtr;
           char *fv1n = fv1_ptr->name;
           char *fv2n = fv2_ptr->name;
           int   fv1nLen = strlen(fv1n);
           int   fv2nLen = strlen(fv2n);

           /* get space for [ name1 ] OR [ name2 ]   */
           str = gm2(sizeof(char) * (fv1nLen + fv2nLen + 13));

           str[0] = '['; str[1] = ' ';
           strcpy(str+2, fv1n);
           strcpy(str+fv1nLen+2, " ] OR [ ");
           strcpy(str+fv1nLen+10, fv2n);
            strPtr = str+fv1nLen+fv2nLen+10;
           *strPtr = ' '; strPtr++;
           *strPtr = ']'; strPtr++;
           *strPtr = '\0';

           if (theUnion->name != NULL) rm(theUnion->name, strlen(theUnion->name)+1);
           theUnion->name = str;
         }
              
       fv2_hn = (FUZZY_VALUE_HN *)AddFuzzyValue(theUnion);
       elem2[i].value = (void *)fv2_hn;

       /* AddFuzzyValue copies the fuzzy variable so we need to remove 
          theUnion fuzzy value since it will no longer be pointed to
       */
       rtnFuzzyValue(theUnion);
            
     } /* end of   for (i=0; ....)   */
  }







/*******************************************************************
    FUNCTION FOR printing  Fuzzy Values
    
    PrintFuzzyTemplateFact
    
    When printing facts normally -- (facts) command or watching facts
    it looks like this:
    
          (fuzzyvar very cold) CF 0.6
          
          ( (x1 y1) (x2 y2) ... (xn yn) )
          
    where xi yi are the pairs that describe the values of the fuzzy set. We
    show both the linguistic expression (very cold) and the x,y pairs. If 
    the linguistic expression is not known then ??? is used.
    
    When printing to a file as a result of a (save-facts) command then we
    can only print the fact in a form that can be parsed properly when
    read back by a (load-facts) command. So it will look like:
    
              (fuzzyvar very cold) CF 0.6
              
    OR if the linguistic expression is ??? then it will have the form
    
              (fuzzyvar (x1 y1) (x2 y2) ... (xn yn)) CF 0.6

     
 *******************************************************************/
 
 
globle    void PrintFuzzyTemplateFact(logName, fv
#if CERTAINTY_FACTORS  
                                    , CF   
#endif
                                   )
 
 char *logName;
 struct fuzzy_value *fv;
#if CERTAINTY_FACTORS  
 double CF;   
#endif
 {
   char *fvname;
   int nameUnknown = FALSE;;

   /* has already printed the Template name at this point */
   PrintRouter(logName," ");
   
   fvname = fv->name;
   
   /* ??? signifies a fv with no linguistic expression (eg. very cold) */
   if (strcmp(fvname, "???") == 0)  
      nameUnknown = TRUE;
      
   /* different format for save-facts command */
   if (!(saveFactsInProgress && nameUnknown)) 
     {
       PrintRouter(logName,fvname);
       PrintRouter(logName,")");
#if CERTAINTY_FACTORS  
       printCF(logName,CF);   
#endif
     }
     
   if (!saveFactsInProgress)
       PrintRouter(logName,"\n\t( ");
   
   if (!saveFactsInProgress || nameUnknown)
      {
        PrintFuzzySet(logName, fv);     
        PrintRouter(logName," )\n");
      }
          
#if CERTAINTY_FACTORS  
   if (saveFactsInProgress && nameUnknown)
       printCF(logName,CF);   
#endif
 }




/*******************************************************************
    FUNCTION FOR printing  Fuzzy Sets
    
    PrintFuzzySet  &  PrintFloatFuzzy
    
          (x1 y1) (x2 y2) ... (xn yn)
          
 *******************************************************************/

/*************************************************************/
/* PrintFloatFuzzy:  Converts number to CLIPS string format. */
/*************************************************************/
static void PrintFloatFuzzy(logName, number)
  char *logName;
  double number;
  {
   char floatString[40];
   char formatString[20];
   int i;
   char x;

   sprintf(formatString, "%%.%dg", FuzzyFloatPrecision);

   sprintf(floatString, formatString, number);

   for (i = 0; (x = floatString[i]) != '\0'; i++)
     {
      if ((x == '.') || (x == 'e'))
        {
         PrintRouter(logName, floatString);
         return;
        }
     }

   strcat(floatString,".0");

   PrintRouter(logName, floatString);
  }

 
 
globle    void PrintFuzzySet(logName, fv)
 
 char *logName;
 struct fuzzy_value *fv;
 {
   int i, j;
 
   for (i=0, j=0; i < fv->n; i++)
      {
        if (j==5)
          {
            PrintRouter(logName, "\n\t  ");
            j = 0;
          }
        PrintRouter(logName, "(");
        PrintFloatFuzzy(logName, fv->x[i]);
        PrintRouter(logName, " ");
        PrintFloatFuzzy(logName, fv->y[i]);
        PrintRouter(logName, ") ");
        j++;
      }
 }
  
 
 

#endif  /* FUZZY_DEFTEMPLATES */
