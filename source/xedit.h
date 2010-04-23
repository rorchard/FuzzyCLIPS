/*  $Header: /dist/CVS/fzclips/src/xedit.h,v 1.3 2001/08/11 21:08:28 dave Exp $  */

/********************************** xedit.h **********************************/
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*        Authors:  BeBe Ly - NASA/Johnson Space Center                      */
/*                  Daniel J. McCoy - University of Houston - Downtown       */
/*                                                                           */
/*****************************************************************************/

void EditNewFile(Widget,XtPointer,XtPointer);
void EditorSaveCallback(Widget,XtPointer,XtPointer);
void EditorSaveAsCallback(Widget,XtPointer,XtPointer);
void EditorRevertCallback(Widget,XtPointer,XtPointer);
int FindSelection(char *);
int SelectionGetc(char *);
int  SelectionUngetc(int,char *);
void EditorCompileSelectionCallback(Widget,XtPointer,XtPointer);
int FileFind(char *);
int FileGetc(char *);
int  FileUngetc(int,char *);
void EditorCompileFileCallback(Widget,XtPointer,XtPointer);
int  LoadXFile(char *,char *);
void EditorBatchSelectionCb(Widget,XtPointer,XtPointer);
void EditorExitCallback(Widget,XtPointer,XtPointer);
void EditorCutCallback(Widget,XtPointer,XtPointer);
void EditorPasteCallback(Widget,XtPointer,XtPointer);
void EditorSearchReplaceCallback(Widget,XtPointer,XtPointer);
void FindMatchingParenthesisCallback(Widget,XtPointer,XtPointer);
int SearchForward(Widget,XawTextBlock *,XawTextPosition);
void WarningWindow(char *);
int SearchBackward(Widget,XawTextBlock *,XawTextPosition);
void EditorBeginingOfFileCallback(Widget,XtPointer,XtPointer);
void EditorEndOfFileCallback(Widget,XtPointer,XtPointer);
void EditorHelpSelect(Widget,XtPointer,XtPointer);
void EditorSaveAs(Widget,XtPointer,XtPointer);
void EditorRevert(Widget,XtPointer,XtPointer);



