/*  $Header: /dist/CVS/fzclips/src/xclipstext.h,v 1.3 2001/08/11 21:08:27 dave Exp $  */

void UnKill(TextWidget,XEvent *);
void Stuff(TextWidget,XEvent *);
void MoveBeginningOfFile(Widget,XEvent *);
void MoveEndOfFile(Widget,XEvent *);
void DeleteCurrentSelection(Widget,XEvent *);
int LocalClipsInsertNewLine(TextWidget,XEvent *);
void InsertClipsString(Widget,XEvent *,String *,Cardinal *);
