/*  $Header: /dist/CVS/fzclips/src/xmenu.h,v 1.3 2001/08/11 21:08:32 dave Exp $  */

/*********************************** xmenu.h *********************************/
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*        Authors:  BeBe Ly - NASA/Johnson Space Center                      */
/*                  Daniel J. McCoy - University of Houston-Downtown         */
/*                                  & Loral Space Information Systems        */
/*                                                                           */
/*****************************************************************************/

void DialogReturn(Widget,XEvent *,String *,Cardinal *);
void CreatePullDownMenus(Widget);
void CancelPopupSelect(Widget,XtPointer,XtPointer);
void PopdownSelect(Widget,XtPointer,XtPointer);
void MenuFunc(Widget,XtPointer,XtPointer);

