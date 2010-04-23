long DoLoad ( HWND, WORD );
char *GetCommandCompletionString (char*, int);
BOOL DoCommandCompletion ( char *, int );
void UpdateModuleMenu ( HMENU );
void UpdateMenu ( void );
void OpenDribbleFile ( HWND, WORD );
void SaveBinaryFile ( HWND, WORD );
long FAR PASCAL DialogWndProc(HWND, unsigned, WPARAM, LONG);