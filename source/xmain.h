/*  $Header: /dist/CVS/fzclips/src/xmain.h,v 1.3 2001/08/11 21:08:29 dave Exp $  */

/********************************** xmain.h **********************************/
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*        Authors:  BeBe Ly - NASA/Johnson Space Center                      */
/*                  Daniel J. McCoy - University of Houston - Downtown       */
/*                                  & Loral Space Information Systems        */
/*                                                                           */
/*****************************************************************************/

String fallback_resources[] =
  {
  "*allowHoriz:                 True",
  "*allowVert:                  True",
  "*borderWidth:                4",
  "*lineWidth:                  4",
  "*defaultColumns:             1",
  "*forceColumns:               True",
  "*showGrip:                   off",

  "*MenuButton.width:           75",
  "*MenuButton3D.width:           75",
  "*watchButton.width:          75",
  "*Form.file_dialog*translations:   #override \\n <Key>Return: DialogReturn(client_data)",
  "*Form.match_dialog*translations:  #override \\n <Key>Return: MatchDialogReturnD(client_data)",
  "*Form.match_editor*translations:  #override \\n <Key>Return: MatchDialogReturnE(client_data)",
  "*manager_viewport.height:    300",
  "*manager_viewport.width:     300",
  "*manager_form.Command.width: 150",
  "*Paned*internalBorderWidth:  0",

  NULL,
  };

char *xclips_translation1 =
"\
Ctrl<Key>A:     no-op() \n\
Ctrl<Key>B:     no-op() \n\
Ctrl<Key>C:     complete-construct-dialog() \n\
Ctrl<Key>D:     dribble() \n\
Ctrl<Key>E:     reset() \n\
Ctrl<Key>F:     no-op() \n\
Ctrl<Key>G:     no-op() \n\
Ctrl<Key>H:     stop-execution() \n\
Ctrl<Key>I:     no-op() \n\
Ctrl<Key>J:     no-op() \n\
Ctrl<Key>K:     clear-clips() \n\
Ctrl<Key>L:     load-constructs() \n\
Ctrl<Key>M:     no-op() \n\
Ctrl<Key>N:     clear-screen() \n\
Ctrl<Key>O:     no-op() \n\
Ctrl<Key>P:     no-op() \n\
Ctrl<Key>Q:     quit() \n\
Ctrl<Key>R:     run() \n\
Ctrl<Key>S:     save-rules() \n\
Ctrl<Key>T:     step() \n\
Ctrl<Key>U:     no-op() \n\
Ctrl<Key>V:     edit() \n\
Ctrl<Key>W:     no-op() \n\
Ctrl<Key>X:     no-op() \n\
Ctrl<Key>Y:     no-op() \n\
Ctrl<Key>Z:     command-line-clips() \n\
Meta<Key>I:     no-op() \n\
Meta<Key>K:     no-op() \n\
Meta<Key>Q:     no-op() \n\
:Meta<Key>d:    no-op() \n\
:Meta<Key>D:    no-op() \n\
:Meta<Key>h:    no-op() \n\
:Meta<Key>H:    no-op() \n\
:Meta<Key>]:    no-op() \n\
:Meta<Key>[:    no-op() \n\
~Shift Meta<Key>Delete:         no-op() \n\
 Shift Meta<Key>Delete:         no-op() \n\
~Shift Meta<Key>BackSpace:      no-op() \n\
 Shift Meta<Key>BackSpace:      no-op() \n\
<Key>Return:    Clipsnewline() \n\
<Key>Linefeed:  Clipsnewline() \n\
<Key>Delete:    delete-clips-previous-character() \n\
<Key>BackSpace: delete-clips-previous-character() \n\
<Key>:          insert-clips-char() \n\
<Btn2Down>:     insert-clips-selection(PRIMARY, CUT_BUFFER0) \n\
";

#define clips_logo_width 30
#define clips_logo_height 27
static char clips_logo_bits[] = {
   0x80, 0xff, 0x01, 0x00, 0xe0, 0xff, 0x07, 0x00, 0x70, 0x20, 0x1d, 0x00,
   0x38, 0xb0, 0x3c, 0x00, 0x1c, 0x48, 0x72, 0x00, 0x8c, 0x2f, 0xf9, 0x00,
   0x8e, 0xba, 0xc4, 0x00, 0x66, 0xf8, 0xc6, 0x01, 0x37, 0x29, 0x61, 0x03,
   0xb3, 0xb6, 0x31, 0x03, 0x4f, 0x88, 0x18, 0x06, 0x23, 0x6c, 0x0c, 0x0f,
   0x13, 0x22, 0x86, 0x0c, 0x1f, 0xbf, 0xc3, 0x1c, 0x86, 0x08, 0x60, 0x32,
   0x86, 0x04, 0x10, 0x31, 0x7c, 0x02, 0xd8, 0x31, 0x18, 0x05, 0xc4, 0x1d,
   0xb8, 0x8a, 0x36, 0x0e, 0x70, 0x93, 0x11, 0x06, 0xe0, 0xee, 0x11, 0x06,
   0xc0, 0xa9, 0x09, 0x06, 0x80, 0x73, 0x04, 0x06, 0x00, 0x23, 0x02, 0x06,
   0x00, 0x1b, 0xf9, 0x07, 0x00, 0x8b, 0xfc, 0x03, 0x00, 0x87, 0x0c, 0x00};



