/*  $Header: /dist/CVS/fzclips/src/xclips.h,v 1.3 2001/08/11 21:08:25 dave Exp $  */

/*********************************** xclips.h *********************************/
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*        Authors:  BeBe Ly - NASA/Johnson Space Center                       */
/*                  Daniel J. McCoy - University of Houston - Downtown        */
/*                                                                            */
/******************************************************************************/

void InitializeInterface(void);
int XclipsQuery(char *);
int XclipsPrint(char *,char *);
int XclipsGetc(char *);
int XclipsUngetc(int,char *);
int XclipsExit(int);
int PrintChangedAgenda(void);
int PrintChangedFacts(void);
int PrintChangedInstances(void);
int PrintChangedGlobals(void);
int PrintChangedFocus(void);
void UpdateMenus(void);
void UpdateOptionsMenu(void);
int set_clips_command(int);
int get_clips_command(void);


#ifndef TRUE
#define TRUE		1
#endif

#ifndef FALSE
#define FALSE 		0
#endif

#ifndef EOS
#define EOS 		'\0'
#endif

#define DELETE   	'\d'
#define BACKSPACE       '\b'
#define NEWLINE		'\n'
#define CR		'\r'
#define FORMFEED	'\f'
#define BLANK		' '
#define TAB		'\t'
#define ESC		'\033'
#define LOW_PRN_ASCII	' '
#define HIGH_PRN_ASCII	'~'
#define LOG_TABLE_SIZE  15
#define MAX_CHAR_IN_BUF 512

#define Facts           0
#define Rules           1
#define Activations     2
#define Compilations    3

#define RULEMNGR        0
#define DEFFACTSMNGR    1
#define DEFTEMPMNGR     2
#define AGENDAMNGR      3

#define EDIT            0
#define LOADBATCH       1
#define LOADBINARY      2
#define LOADFACTS       3
#define LOADRULES       4
#define DRIBBLEON       5
#define SAVEAS          6
#define SAVEBINARY      7
#define SAVEFACTS       8
#define SAVERULES       9
#define REVERT          10

#define INT_STA_CONSTRAINT_CHK    0
#define INT_DYN_CONSTRAINT_CHK    1
#define INT_RESET_GLOBALS        2
#define INT_SEQUENCE_OPT_REG     3
#define INT_INCREMENTAL_RESET    4
#define INT_AUTO_FLOAT_DIV       5
#define INT_FACT_DUPLICATION     6


#define SALIENCE_FLAG 0
#define STRATEGY_FLAG 1

#define MAX_WATCH 14

#define AGENDA_WIN   0
#define FACT_WIN     1
#define INSTANCE_WIN 2
#define GLOBAL_WIN  3
#define FOCUS_WIN   4
#define WINDOW_NUM     5


#define streq(a, b)   (strcmp((a), (b)) == 0)
#define balloc(nm,bk) (bk*)malloc (sizeof(bk)*((unsigned)nm))
#define release(node) free((char *)node)

typedef struct logname
  {
  char *name;
  struct logname *next;
  }LogName, *LogNamePtr;
#define salience_width 153
#define salience_height 18

#define strategy_width 153
#define strategy_height 18



