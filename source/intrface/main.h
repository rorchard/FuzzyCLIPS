#define DIALOG_SIZE 800

void UpdateCursor ( int );
void WinRunEvent ( void );
void ExitToShell( void );
void StartWaitCursor ( void );
void StopWaitCursor ( void );

typedef struct
{  int NoLines;            // Number of that can be viewed in window
   int LastLine;           // Pointer to the last line of the dialog box
   HWND hWnd;              // Handle to the Window
   int LineSize;           // Number of Pixels per Line of Text
}  SCREEN;

