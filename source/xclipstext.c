static char rcsid[] = "$Header: /dist/CVS/fzclips/src/xclipstext.c,v 1.3 2001/08/11 21:08:26 dave Exp $" ;

/******************************* ClipsTextAct.c ******************************/
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include <X11/Xatom.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/keysym.h>
#include <X11/Xlib.h>

#include <X11/Xaw/Text.h>

#include <X11/Xmu/Atoms.h>
#include <X11/Xmu/Misc.h>

#ifdef XAW3D
#include <X11/Xaw3d/TextP.h>
#else
#include <X11/Xaw/TextP.h>
#endif

#include "setup.h"

#include "commline.h"
#include "evaluatn.h"
#include "filertr.h"
#include "router.h"

#include "xclips.h"
#include "xedit.h"
#include "xmenu_file.h"
#include "xmenu_exec.h"
#include "xmenu_wind.h"
#include "xclipstext.h"

static void StartAction(TextWidget,XEvent *);
static void EndAction(TextWidget);
static void StuffFromBuffer(TextWidget,int);
static void _SelectionReceived(Widget,caddr_t,Atom *,Atom *,caddr_t,unsigned long *,int *);
static void _ClipsSelectionReceived(Widget,caddr_t,Atom *,Atom *,caddr_t,unsigned long *,int *);
static void GetSelection(Widget,Time,String *,Cardinal);
static void ClipsGetSelection(Widget,Time,String *,Cardinal);
static void InsertSelection(Widget,XEvent *,String *,Cardinal *);
static void InsertClipsSelection(Widget,XEvent *,String *,Cardinal *);
static void Move(TextWidget,XEvent *,XawTextScanDirection,XawTextScanType,Boolean);
static void MoveForwardChar(Widget,XEvent *);
static void MoveBackwardChar(Widget,XEvent *);
static void MoveForwardWord(Widget,XEvent *);
static void MoveBackwardWord(Widget,XEvent *);
static void MoveForwardParagraph(Widget,XEvent *);
static void MoveBackwardParagraph(Widget,XEvent *);
static void MoveToLineEnd(Widget,XEvent *);
static void MoveToLineStart(Widget,XEvent *);
static void MoveLine(TextWidget,XEvent *,XawTextScanDirection);
static void MoveNextLine(Widget,XEvent *);
static void MovePreviousLine(Widget,XEvent *);
static void Scroll(TextWidget,XEvent *,XawTextScanDirection);
static void ScrollOneLineUp(Widget,XEvent *);
static void ScrollOneLineDown(Widget,XEvent *);
static void MovePage(TextWidget,XEvent *,XawTextScanDirection);
static void MoveNextPage(Widget,XEvent *);
static void MovePreviousPage(Widget,XEvent *);
static void _DeleteOrKill(TextWidget,XawTextPosition, XawTextPosition,Boolean);
static void DeleteOrKill(TextWidget,XEvent *,XawTextScanDirection,XawTextScanType,Boolean,Boolean);
static void DeleteForwardChar(Widget,XEvent *);
static void DeleteBackwardChar(Widget,XEvent *);
static void DeleteClipsBackwardChar(Widget,XEvent *);
static void DeleteForwardWord(Widget,XEvent *);
static void DeleteBackwardWord(Widget,XEvent *);
static void KillForwardWord(Widget,XEvent *);
static void KillBackwardWord(Widget,XEvent *);
static void KillToEndOfLine(Widget,XEvent *);
static void KillToEndOfParagraph(Widget,XEvent *);
static void KillCurrentSelection(Widget,XEvent *);
static int InsertNewLineAndBackupInternal(TextWidget);
static void InsertNewLineAndBackup(Widget,XEvent *);
static int LocalInsertNewLine(TextWidget,XEvent *);
static void InsertNewLine(Widget,XEvent *);
static void InsertClipsNewLine(Widget,XEvent *);
static void InsertNewLineAndIndent(Widget,XEvent *);
static void SelectWord(Widget,XEvent *,String *,Cardinal *);
static void SelectAll(Widget,XEvent *,String *,Cardinal *);
static void ModifySelection(TextWidget,XEvent *,XawTextSelectionMode,
                            XawTextSelectionAction,String *,Cardinal *);	
static void SelectStart(Widget,XEvent *,String *,Cardinal *);	
static void SelectAdjust(Widget,XEvent *,String *,Cardinal *);	
static void SelectEnd(Widget,XEvent *,String *,Cardinal *);	
static void ExtendStart(Widget,XEvent *,String *,Cardinal *);	
static void ExtendAdjust(Widget,XEvent *,String *,Cardinal *);	
static void ExtendEnd(TextWidget,XEvent *,String *,Cardinal *);
static void RedrawDisplay(Widget,XEvent *);
static void TextFocusIn (TextWidget,XEvent *);
static void TextFocusOut(TextWidget,XEvent *);
static void AutoFill(TextWidget);
static void InsertChar(Widget,XEvent *);
static void InsertClipsChar(Widget,XEvent *);
static void InsertString(Widget,XEvent *,String *,Cardinal *);
static void DisplayCaret(Widget,XEvent *,String *,Cardinal *);	
static void Multiply(Widget,XEvent *,String *,Cardinal *);
static XawTextPosition StripOutOldCRs(TextWidget,XawTextPosition,XawTextPosition);
static void InsertNewCRs(TextWidget,XawTextPosition,XawTextPosition);  
static void FormRegion(TextWidget,XawTextPosition, XawTextPosition);
static void FormParagraph(Widget,XEvent *,String *,Cardinal *);
static void TransposeCharacters(Widget,XEvent *,String *,Cardinal *);
static void NoOp(Widget,XEvent *,String *,Cardinal *);
static void Clear_CLIPS(Widget,XEvent *);
static void IntReset(Widget,XEvent *);
static void FactsWindow(Widget,XEvent *);
static void AgendaWindow(Widget,XEvent *);
static void LoadRulesProc(Widget,XEvent *);
static void Dribble(Widget,XEvent *);
static void QuitProc(Widget,XEvent *);
static void IntRun(Widget,XEvent *);
static void SaveRules(Widget,XEvent *);
static void Step(Widget,XEvent *);
static void Edit(Widget,XEvent *);
static void CommandLineCLIPS(Widget,XEvent *);
static void FindBalance(Widget,XEvent *);
static void CompleteConstructInDialog(Widget,XEvent *);
static void CompleteConstructInEditor(Widget,XEvent *);
static void ClearScreen(Widget,XEvent *);
static void StopExecution(Widget,XEvent *);


#define SrcScan                XawTextSourceScan
#define FindDist               XawTextSinkFindDistance
#define FindPos                XawTextSinkFindPosition

/*
 *  These are defined in xclips.c
 */


static void StartAction(
  TextWidget ctx,
  XEvent *event)
  {
  _XawTextPrepareToUpdate(ctx);
  if (event != NULL) {
    switch (event->type) {
    case ButtonPress:
    case ButtonRelease:
      ctx->text.time = event->xbutton.time;
      ctx->text.ev_x = event->xbutton.x;
      ctx->text.ev_y = event->xbutton.y;
      break;
    case KeyPress:
    case KeyRelease:
      ctx->text.time = event->xkey.time;
      ctx->text.ev_x = event->xkey.x;
      ctx->text.ev_y = event->xkey.y;
      break;
    case MotionNotify:
      ctx->text.time = event->xmotion.time;
      ctx->text.ev_x = event->xmotion.x;
      ctx->text.ev_y = event->xmotion.y;
      break;
    case EnterNotify:
    case LeaveNotify:
      ctx->text.time = event->xcrossing.time;
      ctx->text.ev_x = event->xcrossing.x;
      ctx->text.ev_y = event->xcrossing.y;
    }
  }
  }

static void EndAction(
  TextWidget ctx)
  {
  _XawTextCheckResize(ctx);
  _XawTextExecuteUpdate(ctx);
  ctx->text.mult = 1;
  }



/*
 * These functions are superceeded by insert-selection.
 */

static void StuffFromBuffer(
  TextWidget ctx,
  int buffer)
  {
  XawTextBlock text;
  text.ptr = XFetchBuffer(XtDisplay(ctx), &(text.length), buffer);
  text.firstPos = 0;
  if (_XawTextReplace(ctx, ctx->text.insertPos, ctx->text.insertPos, &text)) {
    XBell(XtDisplay(ctx), 0);
    return;
  }
  ctx->text.insertPos = SrcScan(ctx->text.source, ctx->text.insertPos,
				XawstPositions, XawsdRight, text.length, TRUE);
  XtFree(text.ptr);
  }

void UnKill(
  TextWidget ctx,
  XEvent *event)
  {
  StartAction(ctx, event);
  StuffFromBuffer(ctx, 1);
  EndAction(ctx);
  }

void Stuff(
  TextWidget ctx,
  XEvent *event)
  {
  StartAction(ctx, event);
  StuffFromBuffer(ctx, 0);
  EndAction(ctx);
  }


struct _SelectionList
  {
  String *params;
  Cardinal count;
  Time time;
  };

/* ARGSUSED */
static void _SelectionReceived(
  Widget w,
  caddr_t client_data,
  Atom *selection,
  Atom *type,
  caddr_t value,
  unsigned long *length,
  int *format)
{
  TextWidget ctx = (TextWidget)w;
  XawTextBlock text;

  if (*type == 0 /*XT_CONVERT_FAIL*/ || *length == 0) {
    struct _SelectionList* list = (struct _SelectionList*)client_data;
    if (list != NULL) {
      GetSelection(w, list->time, list->params, list->count);
      XtFree(client_data);
    }
    return;
  }

  StartAction(ctx, NULL);
  text.ptr = (char*)value;
  text.firstPos = 0;
  text.length = *length;
  text.format = FMT8BIT;
  if (_XawTextReplace(ctx, ctx->text.insertPos, ctx->text.insertPos, &text)) {
    XBell(XtDisplay(ctx), 0);
    return;
  }
  ctx->text.insertPos = SrcScan(ctx->text.source, ctx->text.insertPos,
                                XawstPositions, XawsdRight, text.length, TRUE);

  EndAction(ctx);
  _XawTextSetScrollBars(ctx);
  XtFree(client_data);
  XtFree(value);
}

static void _ClipsSelectionReceived(
  Widget w,
  caddr_t client_data,
  Atom *selection,
  Atom *type,
  caddr_t value,
  unsigned long *length,
  int *format)
  {
  TextWidget ctx = (TextWidget)w;
  XawTextBlock text;
  extern Boolean send_to_clips;
  char *cmdstr;

  if (*type == 0 /*XT_CONVERT_FAIL*/ || *length == 0) {
    struct _SelectionList* list = (struct _SelectionList*)client_data;
    if (list != NULL) {
      ClipsGetSelection(w, list->time, list->params, list->count);
      XtFree(client_data);
    }
      return;
  }
  StartAction(ctx, NULL);
  text.ptr = (char*)value;
  if(send_to_clips)
   {
       cmdstr = GetCommandString();
       if(cmdstr == NULL)
         {
           SetCommandString(text.ptr);
         }
       else
         {
           AppendCommandString(text.ptr);
          }
       send_to_clips = False;
   }
  text.firstPos = 0;
  text.length = *length;
  text.format = FMT8BIT;
  if (_XawTextReplace(ctx, ctx->text.insertPos, ctx->text.insertPos, &text)) {
    XBell(XtDisplay(ctx), 0);
    return;
  }
  ctx->text.insertPos = SrcScan(ctx->text.source, ctx->text.insertPos, 
				XawstPositions, XawsdRight, text.length, TRUE);

  EndAction(ctx);
  _XawTextSetScrollBars(ctx);
  XtFree(client_data);
  XtFree(value);
}

static void GetSelection(
  Widget w,
  Time time,
  String *params,                 /* selections in precedence order */
  Cardinal num_params)
{
    Atom selection;
    int buffer;

    XmuInternStrings(XtDisplay(w), params, (Cardinal)1, &selection);
    switch (selection) {
      case XA_CUT_BUFFER0: buffer = 0; break;
      case XA_CUT_BUFFER1: buffer = 1; break;
      case XA_CUT_BUFFER2: buffer = 2; break;
      case XA_CUT_BUFFER3: buffer = 3; break;
      case XA_CUT_BUFFER4: buffer = 4; break;
      case XA_CUT_BUFFER5: buffer = 5; break;
      case XA_CUT_BUFFER6: buffer = 6; break;
      case XA_CUT_BUFFER7: buffer = 7; break;
      default:         buffer = -1;
    }
    if (buffer >= 0) {
        int nbytes;
        unsigned long length;
        int fmt8 = 8;
        Atom type = XA_STRING;
        char *line = XFetchBuffer(XtDisplay(w), &nbytes, buffer);
        if (length = nbytes)
            _SelectionReceived(w, NULL, &selection, &type, (caddr_t)line,
                               &length, &fmt8);
        else if (num_params > 1)
            GetSelection(w, time, params+1, num_params-1);
    } else {
        struct _SelectionList* list;
        if (--num_params) {
            list = XtNew(struct _SelectionList);
            list->params = params + 1;
            list->count = num_params;
            list->time = time;
        } else list = NULL;
        XtGetSelectionValue(w, selection, XA_STRING,
                            (XtSelectionCallbackProc)_SelectionReceived,
                            (XtPointer)list, time);
    }
}

static void ClipsGetSelection(
  Widget w,
  Time time,
  String *params,			/* selections in precedence order */
  Cardinal num_params)
{
    Atom selection;
    int buffer;
    TextWidget ctx = (TextWidget)w;
   
/*    XmuInternStrings(XtDisplay(w), params, (Cardinal)1, &selection);*/
    selection = XInternAtom(XtDisplay(w), *params, False);
    switch (selection) {
      case XA_CUT_BUFFER0: buffer = 0; break;
      case XA_CUT_BUFFER1: buffer = 1; break;
      case XA_CUT_BUFFER2: buffer = 2; break;
      case XA_CUT_BUFFER3: buffer = 3; break;
      case XA_CUT_BUFFER4: buffer = 4; break;
      case XA_CUT_BUFFER5: buffer = 5; break;
      case XA_CUT_BUFFER6: buffer = 6; break;
      case XA_CUT_BUFFER7: buffer = 7; break;
      default:	       buffer = -1;
    }
    if (buffer >= 0) {
	int nbytes;
	unsigned long length;
	int fmt8 = 8;
	Atom type = XA_STRING;
	char *line = XFetchBuffer(XtDisplay(w), &nbytes, buffer);
	if (length = nbytes)
	    _ClipsSelectionReceived(w, NULL, &selection, &type, (caddr_t)line,
			       &length, &fmt8);
	else if (num_params > 1)
	    ClipsGetSelection(w, time, params+1, num_params-1);
    } else {
	struct _SelectionList* list;

	if (--num_params) {
	    list = (struct _SelectionList*)XtNew(struct _SelectionList);
	    list->params = params + 1;
	    list->count = num_params;
	    list->time = time;
	} else list = NULL;
	XtGetSelectionValue(w, selection, XA_STRING, 
                            (XtSelectionCallbackProc)_ClipsSelectionReceived,
			    (XtPointer)list, time);
    }
}

static void InsertSelection(
  Widget w,
  XEvent *event,
  String *params,
  Cardinal *num_params)
{
  StartAction((TextWidget)w, event); /* Get Time. */
  GetSelection(w, ((TextWidget)w)->text.time, params, *num_params);
  EndAction((TextWidget)w);
}

static void InsertClipsSelection(
  Widget w,
  XEvent *event,
  String *params,		
  Cardinal *num_params)
  {
  extern Widget dialog_text;

  TextWidget ctx = (TextWidget)w;

  MoveEndOfFile(dialog_text,event);
  StartAction((TextWidget)w, event); /* Get Time. */
  ClipsGetSelection(w, ((TextWidget)w)->text.time, params, *num_params);
  EndAction((TextWidget)w);
  }

/************************************************************
 *
 * Routines for Moving Around.
 *
 ************************************************************/

static void Move(
  TextWidget ctx,
  XEvent *event,
  XawTextScanDirection dir,
  XawTextScanType type,
  Boolean include)
  {
  StartAction(ctx, event);
  ctx->text.insertPos = SrcScan(ctx->text.source, ctx->text.insertPos, type, dir, ctx->text.mult, include);
  EndAction(ctx);
  }

static void MoveForwardChar(
  Widget w,
  XEvent *event)
  {
  Move((TextWidget) w, event, XawsdRight, XawstPositions, TRUE);
  }

static void MoveBackwardChar(
  Widget w,
  XEvent *event)
  {
  Move((TextWidget) w, event, XawsdLeft, XawstPositions, TRUE);
  }

static void MoveForwardWord(
  Widget w,
  XEvent *event)
  {
  Move((TextWidget) w, event, XawsdRight, XawstWhiteSpace, FALSE);
  }

static void MoveBackwardWord(
  Widget w,
  XEvent *event)
  {
  Move((TextWidget) w, event, XawsdLeft, XawstWhiteSpace, FALSE);
  }

static void MoveForwardParagraph(
  Widget w,
  XEvent *event)
  {
  Move((TextWidget) w, event, XawsdRight, XawstParagraph, FALSE);
  }

static void MoveBackwardParagraph(
  Widget w,
  XEvent *event)
  {
  Move((TextWidget) w, event, XawsdLeft, XawstParagraph, FALSE);
  }

static void MoveToLineEnd(
  Widget w,
  XEvent *event)
  {
  Move((TextWidget) w, event, XawsdRight, XawstEOL, FALSE);
  }

static void MoveToLineStart(
  Widget w,
  XEvent *event)
  {
  Move((TextWidget) w, event, XawsdLeft, XawstEOL, FALSE);
  }


static void MoveLine(
  TextWidget ctx,
  XEvent *event,
  XawTextScanDirection dir)
  {
  XawTextPosition newP, next_line, junk;
  int from_left, garbage;

  StartAction(ctx, event);

  if (dir == XawsdLeft)
    ctx->text.mult++;

  newP = SrcScan(ctx->text.source, ctx->text.insertPos, XawstEOL, XawsdLeft, 1, FALSE);

  FindDist(ctx->text.sink, newP, ctx->text.margin.left, ctx->text.insertPos, &from_left, &junk, &garbage);

  newP = SrcScan(ctx->text.source, ctx->text.insertPos, XawstEOL, dir, ctx->text.mult, (dir == XawsdRight));

  next_line = SrcScan(ctx->text.source, newP, XawstEOL, XawsdRight, 1, FALSE);

  FindPos(ctx->text.sink, newP, ctx->text.margin.left, from_left, FALSE, &(ctx->text.insertPos), &garbage, &garbage);
  
  if (ctx->text.insertPos > next_line)
    ctx->text.insertPos = next_line;

  EndAction(ctx);
}

static void MoveNextLine(
  Widget w,
  XEvent *event)
  {
  MoveLine( (TextWidget) w, event, XawsdRight);
  }

static void MovePreviousLine(
  Widget w,
  XEvent *event)
  {
  MoveLine( (TextWidget) w, event, XawsdLeft);
  }

void MoveBeginningOfFile(
  Widget w,
  XEvent *event)
  {
  Move((TextWidget) w, event, XawsdLeft, XawstAll, TRUE);
  }

void MoveEndOfFile(
  Widget w,
  XEvent *event)
  {
  Move((TextWidget) w, event, XawsdRight, XawstAll, TRUE);
  }

static void Scroll(
  TextWidget ctx,
  XEvent *event,
  XawTextScanDirection dir)
  {
  StartAction(ctx, event);

  if (dir == XawsdLeft)
    _XawTextVScroll(ctx, ctx->text.mult);
  else
    _XawTextVScroll(ctx, -ctx->text.mult);

  EndAction(ctx);
  }

static void ScrollOneLineUp(
  Widget w,
  XEvent *event)
  {
  Scroll( (TextWidget) w, event, XawsdLeft);
  }

static void ScrollOneLineDown(
  Widget w,
  XEvent *event)
  {
  Scroll( (TextWidget) w, event, XawsdRight);
  }

static void MovePage(
  TextWidget ctx,
  XEvent *event,
  XawTextScanDirection dir)
  {
  int scroll_val = Max(1, ctx->text.lt.lines - 2);

  if (dir == XawsdLeft)
    scroll_val = -scroll_val;

  StartAction(ctx, event);
  _XawTextVScroll(ctx, scroll_val);
  ctx->text.insertPos = ctx->text.lt.top;
  EndAction(ctx);
  }

static void MoveNextPage(
  Widget w,
  XEvent *event)
  {
  MovePage((TextWidget) w, event, XawsdRight);
  }

static void MovePreviousPage(
  Widget w,
  XEvent *event)
  {
  MovePage((TextWidget) w, event, XawsdLeft);
  }

/************************************************************
 *
 * Delete Routines.
 *
 ************************************************************/

static void _DeleteOrKill(
  TextWidget ctx,
  XawTextPosition from, 
  XawTextPosition to,
  Boolean	kill)
  {
  XawTextBlock text;
  char *ptr;
  
  if (kill && from < to)
    {
    ptr = (char *) _XawTextGetText(ctx, from, to);
    XStoreBuffer(XtDisplay(ctx), ptr, strlen(ptr), 1);
    XtFree(ptr);
    }
  text.length = 0;
  text.firstPos = 0;
  if (_XawTextReplace(ctx, from, to, &text))
    {
    XBell(XtDisplay(ctx), 50);
    return;
    }
  ctx->text.insertPos = from;
  ctx->text.showposition = TRUE; 
  }

static void DeleteOrKill(
  TextWidget ctx,
  XEvent *event,
  XawTextScanDirection dir,
  XawTextScanType type,
  Boolean include,
  Boolean kill)
  {
  XawTextPosition from, to;
  
  StartAction(ctx, event);
  to = SrcScan(ctx->text.source, ctx->text.insertPos, type, dir, ctx->text.mult, include);
  
  if (dir == XawsdLeft)
    {
    from = to;
    to = ctx->text.insertPos;
    }
  else 
    from = ctx->text.insertPos;

  _DeleteOrKill(ctx, from, to, kill);
  _XawTextSetScrollBars(ctx);
  EndAction(ctx);
  }

static void DeleteForwardChar(
  Widget w,
  XEvent *event)
  {
  DeleteOrKill((TextWidget) w, event, XawsdRight, XawstPositions, TRUE, FALSE);
  }

static void DeleteBackwardChar(
  Widget w,
  XEvent *event)
{
  DeleteOrKill((TextWidget) w, event, XawsdLeft, XawstPositions, TRUE, FALSE);
}

static void DeleteClipsBackwardChar(
  Widget w,
  XEvent *event)
  {
  TextWidget ctx = (TextWidget)w;
  char *cmdstr,strbuf[2];
  extern Boolean quit_get_event;
  extern char* GetCommandString();

  MoveEndOfFile(w,event);
  strbuf[1] = 0;
  if(CommandBufferInputCount == 0)
    return;
  if((!quit_get_event)&&(get_clips_command())&& (!GetManagerList()))
   {
      cmdstr = GetCommandString();
      if((cmdstr != NULL) ? (cmdstr[0] != EOS) :FALSE)
       {
        strbuf[0] = (char) XK_BackSpace;
        ExpandCommandString(strbuf[0]);
      }
   }
  DeleteOrKill(ctx, event, XawsdLeft, XawstPositions, TRUE, FALSE);
  }

static void DeleteForwardWord(
  Widget w,
  XEvent *event)
  {
  DeleteOrKill((TextWidget) w, event, XawsdRight, XawstWhiteSpace, FALSE, FALSE);
  }

static void DeleteBackwardWord(
  Widget w,
  XEvent *event)
  {
  DeleteOrKill((TextWidget) w, event, XawsdLeft, XawstWhiteSpace, FALSE, FALSE);
  }

static void KillForwardWord(
  Widget w,
  XEvent *event)
  {
  DeleteOrKill((TextWidget) w, event, XawsdRight, XawstWhiteSpace, FALSE, TRUE);
  }

static void KillBackwardWord(
  Widget w,
  XEvent *event)
  {
  DeleteOrKill((TextWidget) w, event, XawsdLeft, XawstWhiteSpace, FALSE, TRUE);
  }

static void KillToEndOfLine(
  Widget w,
  XEvent *event)
  {
  TextWidget ctx = (TextWidget) w;
  XawTextPosition end_of_line;

  StartAction(ctx, event);
  end_of_line = SrcScan(ctx->text.source, ctx->text.insertPos, XawstEOL, XawsdRight, ctx->text.mult, FALSE);
  if (end_of_line == ctx->text.insertPos)
    end_of_line = SrcScan(ctx->text.source, ctx->text.insertPos, XawstEOL, XawsdRight, ctx->text.mult, TRUE);

  _DeleteOrKill(ctx, ctx->text.insertPos, end_of_line, TRUE);
  EndAction(ctx);
  _XawTextSetScrollBars(ctx);
  }

static void KillToEndOfParagraph(
  Widget w,
  XEvent *event)
  {
  DeleteOrKill((TextWidget) w, event, XawsdRight, XawstParagraph, FALSE, TRUE);
  }

static void KillCurrentSelection(
  Widget w,
  XEvent *event)
  {
  _XawTextZapSelection( (TextWidget) w, event, TRUE);
  }

void DeleteCurrentSelection(
  Widget w,
  XEvent *event)
  {
  _XawTextZapSelection( (TextWidget) w, event, FALSE);
  }

/************************************************************
 *
 * Insertion Routines.
 *
 ************************************************************/

static int InsertNewLineAndBackupInternal(
  TextWidget ctx)
  {
  int count, error = XawEditDone;
  XawTextBlock text;
  char *buf, *ptr;

  ptr = buf = XtMalloc(sizeof(char) * ctx->text.mult);
  for (count = 0; count < ctx->text.mult; count++, ptr++)
    ptr[0] = '\n';

  text.length = ctx->text.mult;
  text.ptr = buf;
  text.firstPos = 0;
  text.format = FMT8BIT;

  if (_XawTextReplace(ctx, ctx->text.insertPos, ctx->text.insertPos, &text))
    {
    XBell( XtDisplay(ctx), 50);
    error = XawEditError;
    }
  else 
    ctx->text.showposition = TRUE;

  XtFree(buf);
  return(error);
  }

static void InsertNewLineAndBackup(
  Widget w,
  XEvent *event)
  {
  StartAction( (TextWidget) w, event );
  (void) InsertNewLineAndBackupInternal( (TextWidget) w );
  EndAction( (TextWidget) w );
  _XawTextSetScrollBars( (TextWidget) w);
  }

static int LocalInsertNewLine(
TextWidget ctx,
XEvent *event)
{
  StartAction(ctx, event);
  if (InsertNewLineAndBackupInternal(ctx) == XawEditError)
    return(XawEditError);
  ctx->text.insertPos = SrcScan(ctx->text.source, ctx->text.insertPos,
                             XawstPositions, XawsdRight, ctx->text.mult, TRUE);
  EndAction(ctx);
  _XawTextSetScrollBars(ctx);
  return(XawEditDone);
}

static void InsertNewLine(
  Widget w,
  XEvent *event)
{
  (void) LocalInsertNewLine( (TextWidget) w, event);
}

int LocalClipsInsertNewLine(
  TextWidget ctx,
  XEvent *event)
  {

  StartAction(ctx, event);
  if (InsertNewLineAndBackupInternal(ctx) == XawEditError)
    return(XawEditError);
  ctx->text.insertPos = SrcScan(ctx->text.source, ctx->text.insertPos, XawstPositions, XawsdRight, ctx->text.mult, TRUE);
  EndAction(ctx);
  _XawTextSetScrollBars(ctx);
  return(XawEditDone);
  }

static void InsertClipsNewLine(
  Widget w,
  XEvent *event)
  {
  TextWidget ctx = (TextWidget)w;
  char *cmdstr,strbuf[2];
  extern Boolean quit_get_event;
  strbuf[1] = 0;

  MoveEndOfFile(w,event);
  if((!quit_get_event)&&(get_clips_command())&& (!GetManagerList()))
    {
      strbuf[0] = (char) XK_Linefeed;
      ExpandCommandString(strbuf[0]);
      quit_get_event = True;
    }
  (void)LocalClipsInsertNewLine(ctx,event);
  }

static void InsertNewLineAndIndent(
  Widget w,
  XEvent *event)
  {
  XawTextBlock text;
  XawTextPosition pos1, pos2;
  TextWidget ctx = (TextWidget) w;

  StartAction(ctx, event);
  pos1 = SrcScan(ctx->text.source, ctx->text.insertPos, XawstEOL, XawsdLeft, 1, FALSE);
  pos2 = SrcScan(ctx->text.source, pos1, XawstEOL, XawsdLeft, 1, TRUE);
  pos2 = SrcScan(ctx->text.source, pos2, XawstWhiteSpace, XawsdRight, 1, TRUE);
  text.ptr = (char *) _XawTextGetText(ctx, pos1, pos2);
  text.length = strlen(text.ptr);
  if (LocalInsertNewLine(ctx, event)) return;
  text.firstPos = 0;
  if (_XawTextReplace(ctx,ctx->text.insertPos, ctx->text.insertPos, &text))
    {
    XBell(XtDisplay(ctx), 50);
    EndAction(ctx);
    return;
    }
  ctx->text.insertPos = SrcScan(ctx->text.source, ctx->text.insertPos, XawstPositions, XawsdRight, text.length, TRUE);
  XtFree(text.ptr);
  EndAction(ctx);
  _XawTextSetScrollBars(ctx);
  }

/************************************************************
 *
 * Selection Routines.
 *
 *************************************************************/

static void SelectWord(
  Widget w,
  XEvent *event,
  String *params,
  Cardinal *num_params)
  {
  TextWidget ctx = (TextWidget) w;
  XawTextPosition l, r;

  StartAction(ctx, event);
  l = SrcScan(ctx->text.source, ctx->text.insertPos, XawstWhiteSpace, XawsdLeft, 1, FALSE);
  r = SrcScan(ctx->text.source, l, XawstWhiteSpace, XawsdRight, 1, FALSE);
  _XawTextSetSelection(ctx, l, r, params, *num_params);
  EndAction(ctx);
  }

static void SelectAll(
  Widget w,
  XEvent *event,
  String *params,
  Cardinal *num_params)
  {
  TextWidget ctx = (TextWidget) w;

  StartAction(ctx, event);
  _XawTextSetSelection(ctx,zeroPosition,ctx->text.lastPos,params,*num_params);
  EndAction(ctx);
  }

static void ModifySelection(
  TextWidget ctx,
  XEvent *event,
  XawTextSelectionMode mode,
  XawTextSelectionAction action,
  String *params,		/* unused */
  Cardinal *num_params)	/* unused */
  {
  StartAction(ctx, event);
  _XawTextAlterSelection(ctx, mode, action, params, num_params);
  EndAction(ctx);
  }

static void SelectStart(
  Widget w,
  XEvent *event,
  String *params,		/* unused */
  Cardinal *num_params)	/* unused */
  {
  TextWidget ctx = (TextWidget)w;
  ModifySelection((TextWidget) w, event, XawsmTextSelect, XawactionStart, params, num_params);
  }

static void SelectAdjust(
  Widget w,
  XEvent *event,
  String *params,		/* unused */
  Cardinal *num_params)	/* unused */
  {
  ModifySelection((TextWidget) w, event, XawsmTextSelect, XawactionAdjust, params, num_params);
  }

static void SelectEnd(
  Widget w,
  XEvent *event,
  String *params,		/* unused */
  Cardinal *num_params)	/* unused */
  {
  ModifySelection((TextWidget) w, event, XawsmTextSelect, XawactionEnd, params, num_params);
  }

static void ExtendStart(
  Widget w,
  XEvent *event,
  String *params,		/* unused */
  Cardinal *num_params)	/* unused */
  {
  ModifySelection((TextWidget) w, event, XawsmTextExtend, XawactionStart, params, num_params);
  }

static void ExtendAdjust(
  Widget w,
  XEvent *event,
  String *params,		/* unused */
  Cardinal *num_params)	/* unused */
  {
  ModifySelection((TextWidget) w, event, XawsmTextExtend, XawactionAdjust, params, num_params);
  }

static void ExtendEnd(
  TextWidget w,
  XEvent *event,
  String *params,
  Cardinal *num_params)
  {
  ModifySelection((TextWidget) w, event, XawsmTextExtend, XawactionEnd, params, num_params);
  }

/************************************************************
 *
 * Misc. Routines.
 *
 ************************************************************/

static void RedrawDisplay(
  Widget w,
  XEvent *event)
  {
  StartAction( (TextWidget) w, event);
  _XawTextClearAndCenterDisplay((TextWidget) w);
  EndAction( (TextWidget) w);
  }

/*ARGSUSED*/
static void TextFocusIn (
  TextWidget w,
  XEvent *event)
  {
  TextWidget ctx = (TextWidget) w;

  ctx->text.hasfocus = TRUE; 
  }

/*ARGSUSED*/
static void TextFocusOut(
  TextWidget w,
  XEvent *event)
  {
  TextWidget ctx = (TextWidget) w;

  ctx->text.hasfocus = FALSE;
  }

static XComposeStatus compose_status = {NULL, 0};

/*	Function Name: AutoFill
 *	Description: Breaks the line at the previous word boundry when
 *                   called inside InsertChar.
 *	Arguments: ctx - The text widget.
 *	Returns: none
 */

static void AutoFill(
  TextWidget ctx)
  {
  int width, height, x, line_num, max_width;
  XawTextPosition ret_pos;
  XawTextBlock text;

  if ( !((ctx->text.auto_fill) && (ctx->text.mult == 1)) )
    return;

  for ( line_num = 0; line_num < ctx->text.lt.lines ; line_num++)
    if ( ctx->text.lt.info[line_num].position >= ctx->text.insertPos )
      break;
  line_num--;			/* backup a line. */

  max_width = Max(0, ctx->core.width - HMargins(ctx));

  x = ctx->text.margin.left;
  XawTextSinkFindPosition( ctx->text.sink,ctx->text.lt.info[line_num].position, x, max_width, TRUE, &ret_pos, &width, &height);
  
  if ( ret_pos >= ctx->text.insertPos )
    return;
  
  text.ptr = "\n";
  text.length = 1;
  text.firstPos = 0;
  text.format = FMT8BIT;

  _XawTextReplace(ctx, ret_pos - 1, ret_pos, &text);
  }


static void InsertChar(
Widget w,
XEvent *event)
{
  TextWidget ctx = (TextWidget) w;
  char *ptr, strbuf[BUFSIZ];
  int count, error;
  KeySym keysym;
  XawTextBlock text;

  if ( (text.length = XLookupString (&event->xkey, strbuf, BUFSIZ,
                               &keysym, &compose_status)) == 0) {
    return;
  }

  text.ptr = ptr = XtMalloc(sizeof(char) * text.length * ctx->text.mult);
  for (count = 0 ; count < ctx->text.mult ; count++) {
    strncpy(ptr, strbuf, text.length);
    ptr += text.length;
  }

  text.length = text.length * ctx->text.mult;
  text.firstPos = 0;
  text.format = FMT8BIT;

  StartAction(ctx, event);

  error = _XawTextReplace(ctx, ctx->text.insertPos,ctx->text.insertPos, &text);

  if (error == XawEditDone) {
    ctx->text.insertPos =
      SrcScan(ctx->text.source, ctx->text.insertPos,
              XawstPositions, XawsdRight, text.length, TRUE);
    AutoFill(ctx);
  }
  else
    XBell(XtDisplay(ctx), 50);

  XtFree(text.ptr);
  EndAction(ctx);
  _XawTextSetScrollBars(ctx);
}

static void InsertClipsChar(
  Widget w,
  XEvent *event)
  {
  TextWidget ctx = (TextWidget) w;
  char *ptr, strbuf[BUFSIZ];
  int count, error;
  KeySym keysym;
  XawTextBlock text;
  extern Boolean quit_get_event;
  char *cmdstr;
  
  MoveEndOfFile(w, event);
  if ( (text.length = XLookupString (&event->xkey, strbuf, BUFSIZ, &keysym, &compose_status)) == 0)
    return;
  if((!quit_get_event)&&(get_clips_command())&& (!GetManagerList()))
   {
        strbuf[1] = 0;
        if((keysym>= XK_space) && (keysym<= XK_asciitilde))
          {
          ExpandCommandString(strbuf[0]);
          }
    }
  else
    return;
  text.ptr = ptr = XtMalloc(sizeof(char) * text.length * ctx->text.mult);
  for (count = 0 ; count < ctx->text.mult ; count++)
    {
    strncpy(ptr, strbuf, text.length);
    ptr += text.length;
    }

  text.length = text.length * ctx->text.mult;
  text.firstPos = 0;
  text.format = FMT8BIT;
  
  StartAction(ctx, event);
  
  error = _XawTextReplace(ctx, ctx->text.insertPos,ctx->text.insertPos, &text);

  if (error == XawEditDone)
    {
    ctx->text.insertPos = SrcScan(ctx->text.source, ctx->text.insertPos, XawstPositions, XawsdRight, text.length, TRUE);
    AutoFill(ctx);
    }
  else 
    XBell(XtDisplay(ctx), 50);

  XtFree(text.ptr);
  EndAction(ctx);
  _XawTextSetScrollBars(ctx);
  }

/*ARGSUSED*/
static void InsertString(
  Widget w,
  XEvent *event,
  String *params,
  Cardinal *num_params)
{
  TextWidget ctx = (TextWidget) w;
  XawTextBlock text;
  int      i;

  text.firstPos = 0;
  StartAction(ctx, event);
  for (i = *num_params; i; i--, params++) {
    unsigned char hexval;
    if ((*params)[0] == '0' && (*params)[1] == 'x' && (*params)[2] != '\0') {
      char c, *p;
      hexval = 0;
      for (p = *params+2; (c = *p); p++) {
        hexval *= 16;
        if (c >= '0' && c <= '9')
          hexval += c - '0';
        else if (c >= 'a' && c <= 'f')
          hexval += c - 'a' + 10;
        else if (c >= 'A' && c <= 'F')
          hexval += c - 'A' + 10;
        else break;
      }
      if (c == '\0') {
        text.ptr = (char*)&hexval;
        text.length = 1;
      } else text.length = strlen(text.ptr = *params);
    } else text.length = strlen(text.ptr = *params);
    if (text.length == 0) continue;
    if (_XawTextReplace(ctx, ctx->text.insertPos,
                        ctx->text.insertPos, &text)) {
      XBell(XtDisplay(ctx), 50);
      EndAction(ctx);
      return;
    }
    ctx->text.insertPos =
      SrcScan(ctx->text.source, ctx->text.insertPos,
              XawstPositions, XawsdRight, text.length, TRUE);
  }
  EndAction(ctx);
}

/*ARGSUSED*/
void InsertClipsString(
  Widget w,
  XEvent *event,
  String *params,
  Cardinal *num_params)
  {
  TextWidget ctx = (TextWidget) w;
  XawTextBlock text;
  int	   i;
  
  text.firstPos = 0;
  StartAction(ctx, event);
  for (i = *num_params; i; i--, params++) 
    {
    unsigned char hexval;
    if ((*params)[0] == '0' && (*params)[1] == 'x' && (*params)[2] != '\0') 
      {
      char c, *p;
      hexval = 0;
      for (p = *params+2; (c = *p); p++) 
        {
        hexval *= 16;
        if (c >= '0' && c <= '9')
          hexval += c - '0';
        else if (c >= 'a' && c <= 'f')
          hexval += c - 'a' + 10;
        else if (c >= 'A' && c <= 'F')
          hexval += c - 'A' + 10;
        else break;
        }
      if (c == '\0') 
        {
        text.ptr = (char*)&hexval;
        text.length = 1;
        } 
      else 
        text.length = strlen(text.ptr = *params);
      } 
    else 
      text.length = strlen(text.ptr = *params);
    if (text.length == 0) 
      continue;
    if (_XawTextReplace(ctx, ctx->text.insertPos, ctx->text.insertPos, &text)) 
      {
      XBell(XtDisplay(ctx), 50);
      EndAction(ctx);
      return;
      }
    ctx->text.insertPos = SrcScan(ctx->text.source, ctx->text.insertPos, XawstPositions, XawsdRight, text.length, TRUE);
    }
  EndAction(ctx);

  }

static void DisplayCaret(
  Widget w,
  XEvent *event,		/* CrossingNotify special-cased */
  String *params,		/* Off, False, No, On, True, Yes, etc. */
  Cardinal *num_params)	/* 0, 1 or 2 */
  {
  TextWidget ctx = (TextWidget)w;
  Boolean display_caret = True;

  if (event->type == EnterNotify || event->type == LeaveNotify)
    {
    /* for Crossing events, the default case is to check the focus
     * field and only change the caret when focus==True.  A second
     * argument of "always" will cause the focus field to be ignored.
     */
    Boolean check_focus = True;
    if (*num_params == 2 && strcmp(params[1], "always") == 0)
      check_focus = False;
    if (check_focus && !event->xcrossing.focus)
      return;
    }

  if (*num_params > 0)  /* default arg is "True" */
    {
    XrmValue from, to;
    from.size = strlen(from.addr = params[0]);
    XtConvert(w, XtRString, &from, XtRBoolean, &to);
    if (to.addr != NULL) display_caret = *(Boolean*)to.addr;
    if (ctx->text.display_caret == display_caret) return;
    }
  StartAction(ctx, event);
  ctx->text.display_caret = display_caret;
  EndAction(ctx);
  }

/*	Function Name: Multiply
 *	Description: Multiplies the current action by the number passed.
 *	Arguments: w - the text widget.
 *                 event - ** NOT USED **.
 *                 params, num_params - The parameter list, see below.
 *	Returns: none.
 *
 * Parameter list;
 *  
 * The parameter list may contain either a number or the string 'Reset'.
 * 
 * A number will multiply the current multiplication factor by that number.
 * Many of the text widget actions will will perform n actions, where n is
 * the multiplication factor.
 *
 * The string reset will reset the mutiplication factor to 1.
 * 
 */

static void Multiply(
  Widget w,
  XEvent *event,
  String *params,
  Cardinal *num_params)
  {
  TextWidget ctx = (TextWidget) w;
  int mult;

  if (*num_params != 1)
    {
    XtAppError(XtWidgetToApplicationContext(w), "The multiply action takes exactly one argument.");
    XBell(XtDisplay(w), 0);
    return;
    }

  if ( (params[0][0] == 'r') || (params[0][0] == 'R') )
    {
    XBell(XtDisplay(w), 0);
    ctx->text.mult = 1;
    return;
    }

  if ( (mult = atoi(params[0])) == 0 )
    {
    char buf[BUFSIZ];
    sprintf(buf, "%s %s", "Text Widget: The multiply action's argument", "must be a number greater than zero, or 'Reset'.");
    XtAppError(XtWidgetToApplicationContext(w), buf);
    XBell(XtDisplay(w), 0);
    return;
    }

  ctx->text.mult *= mult;
  }

/*	Function Name: StripOutOldCRs
 *	Description: strips out the old carrige returns.
 *	Arguments: ctx - the text widget.
 *                 from - starting point.
 *                 to - the ending point
 *	Returns: the new ending location (we may add some caracters).
 */

static XawTextPosition StripOutOldCRs(
  TextWidget ctx,
  XawTextPosition from,
  XawTextPosition to)
  {
  XawTextPosition startPos, endPos, eop_begin, eop_end, temp;
  Widget src = ctx->text.source;
  XawTextBlock text;
  char *buf;

  text.ptr= "  ";
  text.firstPos = 0;
  text.format = FMT8BIT;
   
/*
 * Strip out CR's. 
 */

  eop_begin = eop_end = startPos = endPos = from;
  while (TRUE)
    {
    endPos=SrcScan(src, startPos, XawstEOL, XawsdRight, 1, FALSE);

    temp=SrcScan(src, endPos, XawstWhiteSpace, XawsdLeft, 1, FALSE);
    temp=SrcScan(src, temp, XawstWhiteSpace, XawsdRight, 1, FALSE);
    if (temp > startPos)
      endPos = temp;

    if (endPos >= to)
      break;

    if (endPos >= eop_begin)
      {
      startPos = eop_end;
      eop_begin = SrcScan(src, startPos, XawstParagraph, XawsdRight, 1, FALSE);
      eop_end = SrcScan(src, startPos, XawstParagraph, XawsdRight, 1, TRUE);
      }
    else
      {
      XawTextPosition periodPos, next_word;
      int i, len;

      periodPos= SrcScan(src, endPos, XawstPositions, XawsdLeft, 1, TRUE);
      next_word = SrcScan(src, endPos, XawstWhiteSpace, XawsdRight, 1, FALSE);

      len = next_word - periodPos;

      text.length = 1;
      buf = (char *) _XawTextGetText(ctx, periodPos, next_word);
      if ( (periodPos < endPos) && (buf[0] == '.') )
        text.length++;	/* Put in two spaces. */

      /*
       * Remove all extra spaces. 
       */

      for (i = 1 ; i < len; i++) 
        if ( !isspace(buf[i]) || ((periodPos + i) >= to) )
	  break;
      
      XtFree(buf);

      to -= (i - text.length - 1);
      startPos = SrcScan(src, periodPos, XawstPositions, XawsdRight, i, TRUE);
      _XawTextReplace(ctx, endPos, startPos, &text);
      startPos -= i - text.length;
      }
    }
  return(to);
  }

/*	Function Name: InsertNewCRs
 *	Description: Inserts the new Carrige Returns.
 *	Arguments: ctx - the text widget.
 *                 from, to - the ends of the region.
 *	Returns: none
 */

static void InsertNewCRs(
  TextWidget ctx,
  XawTextPosition from,
  XawTextPosition to)
  {
  XawTextPosition startPos, endPos, space, eol;
  XawTextBlock text;
  int i, width, height, len;
  char * buf;

  text.ptr = "\n";
  text.length = 1;
  text.firstPos = 0;
  text.format = FMT8BIT;

  startPos = from;
  while (TRUE)
    {
    XawTextSinkFindPosition( ctx->text.sink, startPos, (int) ctx->text.margin.left, (int) (ctx->core.width - HMargins(ctx)), TRUE, &eol, &width, &height);
    if (eol >= to)
      break;

    eol = SrcScan(ctx->text.source, eol, XawstPositions, XawsdLeft, 1, TRUE);
    space= SrcScan(ctx->text.source, eol, XawstWhiteSpace, XawsdRight, 1,TRUE);
    
    startPos = endPos = eol;
    if (eol == space) 
      return;

    len = (int) (space - eol);
    buf = (char *) _XawTextGetText(ctx, eol, space);
    for ( i = 0 ; i < len ; i++)
      if (!isspace(buf[i]))
	break;

    to -= (i - 1);
    endPos = SrcScan(ctx->text.source, endPos, XawstPositions, XawsdRight, i, TRUE);
    XtFree(buf);
    
    _XawTextReplace(ctx, startPos, endPos, &text);
    startPos = SrcScan(ctx->text.source, startPos, XawstPositions, XawsdRight, 1, TRUE);
    }
  }  
  
/*	Function Name: FormRegion
 *	Description: Forms up the region specified.
 *	Arguments: ctx - the text widget.
 *                 from, to - the ends of the region.
 *	Returns: none.
 */

static void FormRegion(
  TextWidget ctx,
  XawTextPosition from, 
  XawTextPosition to)
  {
  if (from >= to) return;

  to = StripOutOldCRs(ctx, from, to);
  InsertNewCRs(ctx, from, to);
  _XawTextBuildLineTable(ctx, ctx->text.lt.top, TRUE);
  }

/*	Function Name: FromParagraph.
 *	Description: reforms up the current paragraph.
 *	Arguments: w - the text widget.
 *                 event - the X event.
 *                 params, num_params *** NOT USED ***.
 *	Returns: none
 */

static void FormParagraph(
  Widget w,
  XEvent *event,
  String *params,
  Cardinal *num_params)
  {
  TextWidget ctx = (TextWidget) w;
  XawTextPosition from, to;

  StartAction(ctx, event);

  from =  SrcScan(ctx->text.source, ctx->text.insertPos, XawstParagraph, XawsdLeft, 1, FALSE);
  to  =  SrcScan(ctx->text.source, from, XawstParagraph, XawsdRight, 1, FALSE);

  FormRegion(ctx, from, to);
  EndAction(ctx);
  _XawTextSetScrollBars(ctx);
  }

/*	Function Name: TransposeCharacters
 *	Description: Swaps the character to the left of the mark with
 *                   the character to the right of the mark.
 *	Arguments: w - the text widget.
 *                 event - the event that cause this action.
 *                 params, num_params *** NOT USED ***.
 *	Returns: none.
 */
	     
static void TransposeCharacters(
  Widget w,
  XEvent *event,
  String *params,
  Cardinal *num_params)
  {
  TextWidget ctx = (TextWidget) w;
  XawTextPosition start, end;
  XawTextBlock text;
  unsigned char * buf, c;
  int i;

  StartAction(ctx, event);

/*
 * Get bounds. 
 */

  start = SrcScan(ctx->text.source, ctx->text.insertPos, XawstPositions, XawsdLeft, 1, TRUE);
  end = SrcScan(ctx->text.source, ctx->text.insertPos, XawstPositions, XawsdRight, ctx->text.mult, TRUE);

  if ( (start == ctx->text.insertPos) || (end == ctx->text.insertPos) ) 
    XBell(XtDisplay(w), 0);	/* complain. */
  else
    {
    ctx->text.insertPos = end;

    /*
     * Retrieve text and swap the characters. 
     */
    
    buf = (unsigned char *) _XawTextGetText(ctx, start, end);
    text.length = strlen((char *) buf);
    text.firstPos = 0;
    text.format = FMT8BIT;
    
    c = buf[0];
    for (i = 1 ; i < text.length ; i++)
      buf[i - 1] = buf[i];
    buf[i - 1] = c;
    
    /* 
     * Store new text is source.
     */
    
    text.ptr = (char *) buf;
    _XawTextReplace (ctx, start, end, &text);
    
    XtFree((char *) buf);
    }
  EndAction(ctx);
  }

/*	Function Name: NoOp
 *	Description: This action performs no action, and allows
 *                   the user or application programmer to unbind a 
 *                   translation.
 *	Arguments: w - the text widget.
 *                 event - *** UNUSED ***.
 *                 parms and num_params - the action parameters.
 *	Returns: none.
 *
 * Note: If the parameter list contains the string "RingBell" then
 *       this action will ring the bell.
 */

static void NoOp(
  Widget w,
  XEvent *event,
  String *params,
  Cardinal *num_params)
  {
  if (*num_params != 1)
    return;

  switch(params[0][0])
    {
    case 'R':
    case 'r': XBell(XtDisplay(w), 0);
    default: break;
    }
  }
	
/* CLIPS key bound functions for the menus */

/*****************************************************************************
*
        Name:           
        Description:
        Arguments:
        Return:
******************************************************************************
*/
static void Clear_CLIPS(
  Widget w,
  XEvent *event)
  {
  ClearCLIPSCallback(w, (XtPointer)NULL, (XtPointer)NULL);
  }

/*****************************************************************************
*
        Name:           
        Description:
        Arguments:
        Return:
******************************************************************************
*/

static void IntReset(
  Widget w,
  XEvent *event)
  {
  ResetCallback(w, (XtPointer)NULL, (XtPointer)NULL);
  }

/*****************************************************************************
*
        Name:           
        Description:
        Arguments:
        Return:
******************************************************************************
*/

static void FactsWindow(
  Widget w,
  XEvent *event)
  {
  FactsWindowCallback(w, (XtPointer)NULL, (XtPointer)NULL);
  }

/*****************************************************************************
*
        Name:           
        Description:
        Arguments:
        Return:
******************************************************************************
*/

static void AgendaWindow(
  Widget w,
  XEvent *event)
  {
  AgendaWindowCallback(w, (XtPointer)NULL, (XtPointer)NULL);
  }
  
/*****************************************************************************
*
        Name:           
        Description:
        Arguments:
        Return:
******************************************************************************
*/

static void LoadRulesProc(
  Widget w,
  XEvent *event)
  {
  LoadRulesCallback(w, (XtPointer)NULL, (XtPointer)NULL);
  }

/*****************************************************************************
*
        Name:           
        Description:
        Arguments:
        Return:
******************************************************************************
*/

static void Dribble(
  Widget w,
  XEvent *event)
  {
  DribbleCallback(w, (XtPointer)NULL, (XtPointer)NULL);
  }

/*****************************************************************************
*
        Name:           
        Description:
        Arguments:
        Return:
******************************************************************************
*/

static void QuitProc(
  Widget w,
  XEvent *event)
  {
  QuitCallback(w, (XtPointer)NULL, (XtPointer)NULL);
  }

/*****************************************************************************
*
        Name:           
        Description:
        Arguments:
        Return:
******************************************************************************
*/

static void IntRun(
  Widget w,
  XEvent *event)
  {
  RunCallback(w, (XtPointer)NULL, (XtPointer)NULL);
  }

/*****************************************************************************
*
        Name:           
        Description:
        Arguments:
        Return:
******************************************************************************
*/

static void SaveRules(
  Widget w,
  XEvent *event)
  {
  SaveRulesCallback(w, (XtPointer)NULL, (XtPointer)NULL);
  }

/*****************************************************************************
*
        Name:           
        Description:
        Arguments:
        Return:
******************************************************************************
*/

static void Step(
  Widget w,
  XEvent *event)
  {
  StepCallback(w, (XtPointer)NULL, (XtPointer)NULL);
  }

/*****************************************************************************
*
        Name:           
        Description:
        Arguments:
        Return:
******************************************************************************
*/

static void Edit(
  Widget w,
  XEvent *event)
  {
  EditCallback(w, (XtPointer)NULL, (XtPointer)NULL);
  }

/*****************************************************************************
*
        Name:           
        Description:
        Arguments:
        Return:
******************************************************************************
*/

static void CommandLineCLIPS(
  Widget w,
  XEvent *event)
  {
  CommandLineCLIPSCallback(w, (XtPointer)NULL, (XtPointer)NULL);
  }

/*****************************************************************************
*
        Name:           
        Description:
        Arguments:
        Return:
******************************************************************************
*/

static void FindBalance(
  Widget w,
  XEvent *event)
  {
    FindMatchingParenthesisCallback(w,(XtPointer)w,(XtPointer)NULL);
  }

/*****************************************************************************
*
        Name:           
        Description:
        Arguments:
        Return:
******************************************************************************
*/

static void CompleteConstructInDialog(
  Widget w,
  XEvent *event)
  {
       CompletionDialogCallback(w,NULL,(XtPointer)NULL);
  }

/*****************************************************************************
*
        Name:           
        Description:
        Arguments:
        Return:
******************************************************************************
*/

static void CompleteConstructInEditor(
  Widget w,
  XEvent *event)
  {
       CompletionEditCallback(w,(XtPointer)w,(XtPointer)NULL);
  }


/*****************************************************************************
*
        Name:           
        Description:
        Arguments:
        Return:
******************************************************************************
*/

static void ClearScreen(
  Widget w,
  XEvent *event)
  {
    ClearScreenCallback(w,(XtPointer)w,(XtPointer)NULL);
  }

/*****************************************************************************
*
        Name:           
        Description:
        Arguments:
        Return:
******************************************************************************
*/

static void StopExecution(
  Widget w,
  XEvent *event)
  {
    extern Boolean periodicChecking;

    if(periodicChecking)
     {
      SetHaltExecution(TRUE);
      CloseAllFiles();
     }
  }

/*****************************************************************************
*  Action Table
******************************************************************************
*/


XtActionsRec ClipsTxtActsTable[] =
  {
/* delete bindings */
  {"delete-clips-previous-character",  (XtActionProc)DeleteClipsBackwardChar},
/* new line stuff */
  {"Clipsnewline",              (XtActionProc)InsertClipsNewLine},
/* Selection stuff */
  {"insert-clips-selection",    (XtActionProc)InsertClipsSelection},
/* Miscellaneous */
  {"insert-clips-string",       (XtActionProc)InsertClipsString},
  {"insert-clips-char",         (XtActionProc)InsertClipsChar},
/* CLIPS Dialog Window key bindings for menus */
  {"clear-clips",               (XtActionProc)Clear_CLIPS},
  {"reset",                     (XtActionProc)IntReset},
  {"facts-window",              (XtActionProc)FactsWindow},
  {"agenda-window",             (XtActionProc)AgendaWindow},
  {"load-constructs",           (XtActionProc)LoadRulesProc},
  {"dribble",                   (XtActionProc)Dribble},
  {"quit",                      (XtActionProc)QuitProc},
  {"run",                       (XtActionProc)IntRun},
  {"save-rules",                (XtActionProc)SaveRules},
  {"step",                      (XtActionProc)Step},
  {"edit",                      (XtActionProc)Edit},
  {"command-line-clips",        (XtActionProc)CommandLineCLIPS},
  {"balance",                   (XtActionProc)FindBalance},
  {"clear-screen",              (XtActionProc)ClearScreen},
  {"complete-construct-dialog", (XtActionProc)CompleteConstructInDialog},
  {"complete-construct-editor", (XtActionProc)CompleteConstructInEditor},
  {"stop-execution",            (XtActionProc)StopExecution},
  };

Cardinal ClipsTxtActsTableCount = XtNumber(ClipsTxtActsTable);

