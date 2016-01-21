//---------------------------------------------------------------//
// Concept messages list                                         //
//---------------------------------------------------------------//
#define CC_BUTTONS_YES_NO_CANCEL          6
#define CC_BUTTONS_SAVE_DISCARD_CANCEL    7

#define MSG_FLUSH_UI                      -12
#define MSG_RAISE_ERROR                   -11
#define MSG_DEBUGGER_TRAPPED              -10
#define MSG_MESSAGE_REQUESTINPUT          -8
#define MSG_MESSAGE_LOGIN                 -7
#define MSG_MESSAGE_BOX_GENERIC           -6
#define MSG_MESSAGE_BOX_YESNO             -5
#define MSG_NON_CRITICAL                  -4
#define MSG_CONSOLE                       -3
#define MSG_MESSAGE_BOX                   -2
#define MSG_RAW_OUTPUT                    -1

#define MSG_CHOOSE_COLOR                  0x90
#define MSG_CHOOSE_FONT                   0x91
#define MSG_SET_PROPERTY_BY_NAME          0x92


#define MSG_CREATE                        0x100
#define MSG_SET_PROPERTY                  0x110
#define MSG_PUT_STREAM                    0x111
#define MSG_PUT_SECONDARY_STREAM          0x112
#define MSG_POST_OBJECT                   0x113
#define MSG_REMOTE_METHOD                 0x114
#define MSG_GET_PROPERTY                  0x115
#define MSG_POST_STRING                   0x116
#define MSG_SEND_COOKIE                   0x117
#define MSG_OS_COMMAND                    0x118
#define MSG_REORDER_CHILD                 0x119

#define MSG_CUSTOM_MESSAGE1               0x120
#define MSG_CUSTOM_MESSAGE2               0x121
#define MSG_CUSTOM_MESSAGE3               0x122
#define MSG_CUSTOM_MESSAGE4               0x123
#define MSG_CUSTOM_MESSAGE5               0x124
#define MSG_CUSTOM_MESSAGE6               0x125
#define MSG_CUSTOM_MESSAGE7               0x126
#define MSG_CUSTOM_MESSAGE8               0x127
#define MSG_CUSTOM_MESSAGE9               0x128
#define MSG_CUSTOM_MESSAGE10              0x129
#define MSG_CUSTOM_MESSAGE11              0x12A
#define MSG_CUSTOM_MESSAGE12              0x12B
#define MSG_CUSTOM_MESSAGE13              0x12C
#define MSG_CUSTOM_MESSAGE14              0x12D
#define MSG_CUSTOM_MESSAGE15              0x12E
#define MSG_CUSTOM_MESSAGE16              0x12F
#define MSG_REPAINT                       0x130

#define MSG_REQUEST_FOR_FILE              0x131
#define MSG_SAVE_FILE                     0x132
#define MSG_SET_CLIPBOARD                 0x133
#define MSG_GET_CLIPBOARD                 0x134
#define MSG_STATIC_QUERY                  0x135
#define MSG_STATIC_RESPONSE               0x136
#define MSG_SAVE_FILE2                    0x137
#define MSG_REQUEST_FOR_CHUNK             0x138
#define MSG_SAVE_CHUNK                    0x139
#define MSG_CHUNK                         0x13A
#define MSG_SEND_COOKIE_CHUNK             0x13B

#define MSG_D_PRIMITIVE_LINE              0x140
#define MSG_D_PRIMITIVE_ARC               0x141
#define MSG_D_PRIMITIVE_FILL_ARC          0x142
#define MSG_D_PRIMITIVE_RECT              0x143
#define MSG_D_PRIMITIVE_FILL_RECT         0x144
#define MSG_D_PRIMITIVE_POINT             0x145
#define MSG_D_PRIMITIVE_COLOR             0x146

#define MSG_SEND_ZCOOKIE                  0x147

#define MSG_SET_COOKIE                    0x148
#define MSG_GET_COOKIE                    0x149

#define MSG_GET_DNDFILE                   0x14A

#define MSG_CLIENT_ENVIRONMENT            0x14B
#define MSG_OS_COMMAND_CLOSE              0x14C
#define MSG_OS_COMMAND_QUEUE_CLEAR        0x14D

#ifdef _WIN32
 #define MSG_CREATE_ACTIVEX               0x420
 #define MSG_AX_SET_PROPERTY              0x421
 #define MSG_AX_ADD_PARAMETER             0x422
 #define MSG_AX_CALL_METHOD               0x423
 #define MSG_AX_ADD_FILEPARAMETER         0x424
 #define MSG_AX_GET_PROPERTY              0x425
 #define MSG_AX_GET_RESULT                0x426
 #define MSG_AX_ADD_EVENT                 0x427
 #define MSG_AX_AKNOWLEDGE_EVENT          0x428
 #define MSG_AX_CLEAR_EVENTS_DATA         0x429
 #define MSG_AX_GET_EVENT_PARAM           0x430
#endif

#define MSG_APPLICATION_RUN               0x800
#define MSG_APPLICATION_QUIT              0x500
#define MSG_MESSAGING_SYSTEM              0x501
#define MSG_CLIENT_QUERY                  0x503
#define MSG_RUN_APPLICATION               0x504
#define MSG_DEBUG_APPLICATION             0x505

#define MSG_SET_EVENT                     0x1000
#define MSG_EVENT_FIRED                   0x1001
#define MSG_CONFIRM_EVENT                 0x1002

// default events
#define EVENT_ON_SHOW                     3
#define EVENT_ON_HIDE                     4
#define EVENT_ON_MAP                      5
#define EVENT_ON_UNMAP                    6
#define EVENT_ON_REALIZE                  7
#define EVENT_ON_UNREALIZE                8
#define EVENT_ON_SIZEREQUEST              9
#define EVENT_ON_SIZEALLOCATE             10
#define EVENT_ON_STATECHANGED             11
#define EVENT_ON_PARENTCHANGED            12
#define EVENT_ON_HIERARCHYCHANGED         13
#define EVENT_ON_STYLECHANGED             14
#define EVENT_ON_DIRECTIONCHANGED         15
#define EVENT_ON_GRABNOTIFY               16
#define EVENT_ON_CHILDNOTIFY              17
#define EVENT_ON_MNEMONICACTIVATE         18
#define EVENT_ON_GRABFOCUS                19
#define EVENT_ON_FOCUS                    20
#define EVENT_ON_EVENT                    21
#define EVENT_ON_BUTTONPRESS              22
#define EVENT_ON_BUTTONRELEASE            23
#define EVENT_ON_SCROLL                   24
#define EVENT_ON_MOTIONNOTIFY             25
#define EVENT_ON_DELETEEVENT              26
#define EVENT_ON_EXPOSEEVENT              27
#define EVENT_ON_KEYPRESS                 28
#define EVENT_ON_KEYRELEASE               29
#define EVENT_ON_ENTERNOTIFY              30
#define EVENT_ON_LEAVENOTIFY              31
#define EVENT_ON_CONFIGURE                32
#define EVENT_ON_FOCUSIN                  33
#define EVENT_ON_FOCUSOUT                 34
#define EVENT_ON_MAPEVENT                 35
#define EVENT_ON_UNMAPEVENT               36
#define EVENT_ON_PROPERTY                 37
#define EVENT_ON_SELECTIONCLEAR           38
#define EVENT_ON_SELECTIONREQUEST         39
#define EVENT_ON_SELECTIONNOTIFY          40
#define EVENT_ON_PROXIMITYIN              41
#define EVENT_ON_PROXIMITYOUT             42
#define EVENT_ON_VISIBILITY               43
#define EVENT_ON_CLIENT                   44
#define EVENT_ON_NOEXPOSE                 45
#define EVENT_ON_WINDOWSTATE              46
#define EVENT_ON_SELECTIONGET             47
#define EVENT_ON_SELECTIONRECEIVED        48
#define EVENT_ON_DRAGBEGIN                49
#define EVENT_ON_DRAGEND                  50
#define EVENT_ON_DRAGDATAGET              51
#define EVENT_ON_DRAGDATADELETE           52
#define EVENT_ON_DRAGLEAVE                53
#define EVENT_ON_DRAGMOTION               54
#define EVENT_ON_DRAGDROP                 55
#define EVENT_ON_DRAGDATARECEIVED         56
#define EVENT_ON_SCREENCHANGED            57

// radio&check buttons
#define EVENT_ON_TOGGLED                  2
#define EVENT_ON_GROUPCHANGED             58

// button events
#define EVENT_ON_CLICKED                  1
#define EVENT_ON_ACTIVATE                 59
#define EVENT_ON_PRESSED                  60
#define EVENT_ON_RELEASED                 61
#define EVENT_ON_ENTER                    62
#define EVENT_ON_LEAVE                    63

// entry events
#define EVENT_ON_INSERTATCURSOR           64
#define EVENT_ON_POPULATEPOPUP            65

// textview events
#define EVENT_ON_SETANCHOR                66
#define EVENT_ON_SETSCROLLADJUSTMENTS     67

// spinbutton events
#define EVENT_ON_INPUT                    68
#define EVENT_ON_OUTPUT                   69
#define EVENT_ON_VALUECHANGED             70

// window events
#define EVENT_ON_FRAMEEVENT               71
#define EVENT_ON_SETFOCUS                 72

// notebook events
#define EVENT_ON_SWITCHPAGE               73

// adjustment events
#define EVENT_ON_CHANGED                  74

// item events
#define EVENT_ON_SELECT                   75
#define EVENT_ON_DESELECT                 76

// menuitem
#define EVENT_ON_ACTIVATEITEM             77

// handlebox
#define EVENT_ON_CHILDATTACHED            78
#define EVENT_ON_CHILDDETACHED            79

// toolbar
#define EVENT_ON_ORIENTATIONCHANGED       80
#define EVENT_ON_POPUPCONTEXTMENU         81
#define EVENT_ON_TOOLBARSTYLECHANGED      82

// toolbutton
#define EVENT_ON_CREATEMENUPROXY          83
#define EVENT_ON_TOOLBARRECONFIGURED      84

// menutoolbutton
#define EVENT_ON_SHOWMENU                 85

// treeview
#define EVENT_ON_COLUMNSCHANGED           86
#define EVENT_ON_CURSORCHANGED            87
#define EVENT_ON_ROWACTIVATED             88
#define EVENT_ON_ROWCOLLAPSED             89
#define EVENT_ON_ROWEXPANDED              90

// timer
#define EVENT_ON_TIMER                    91

// calendar
#define EVENT_ON_DAYSELECTED              92
#define EVENT_ON_DAYSELECTEDDBCLICK       93
#define EVENT_ON_MONTHCHANGED             94
#define EVENT_ON_NEXTMONTH                95
#define EVENT_ON_NEXTYEAR                 96
#define EVENT_ON_PREVMONTH                97
#define EVENT_ON_PREVYEAR                 98

// treeview columns
#define EVENT_ON_STARTEDITING             99
#define EVENT_ON_ENDEDITING               100
#define EVENT_ON_CANCELEDITING            101

// ActiveX events
#define EVENT_ACTIVEX_EVENT               102

// range derived
#define EVENT_ON_ADJUSTBOUNDS             103
#define EVENT_ON_CHANGEVALUE              104
#define EVENT_ON_MOVESLIDER               105

// treeview columns again
#define EVENT_ON_HEADERCLICKED            106
#define EVENT_ON_COLUMNRESIZED            107

#define CLASS_FORM                        1
#define CLASS_BUTTON                      2
#define CLASS_HBOX                        3
#define CLASS_VBOX                        4
#define CLASS_HBUTTONBOX                  5
#define CLASS_VBUTTONBOX                  6
#define CLASS_EDIT                        7
#define CLASS_CHECKBUTTON                 8
#define CLASS_RADIOBUTTON                 9
#define CLASS_HSEPARATOR                  10
#define CLASS_VSEPARATOR                  11
#define CLASS_LABEL                       12
#define CLASS_FRAME                       13
#define CLASS_PROGRESSBAR                 14
#define CLASS_HPANED                      15
#define CLASS_VPANED                      16
#define CLASS_SCROLLEDWINDOW              17
#define CLASS_IMAGE                       18
#define CLASS_ADJUSTMENT                  19
#define CLASS_VSCALE                      20
#define CLASS_HSCALE                      21
#define CLASS_VSCROLLBAR                  22
#define CLASS_HSCROLLBAR                  23
#define CLASS_SPINBUTTON                  24
#define CLASS_TEXTVIEW                    25
#define CLASS_ALIGNMENT                   26
#define CLASS_NOTEBOOK                    27
#define CLASS_ASPECTFRAME                 28
#define CLASS_MENUITEM                    29
#define CLASS_MENU                        30
#define CLASS_MENUBAR                     31
#define CLASS_SEPARATORMENUITEM           32
#define CLASS_IMAGEMENUITEM               33
#define CLASS_TEAROFFMENUITEM             34
#define CLASS_CHECKMENUITEM               35
#define CLASS_EVENTBOX                    36
#define CLASS_EXPANDER                    37
#define CLASS_VIEWPORT                    38
#define CLASS_HANDLEBOX                   39
#define CLASS_TOOLBAR                     40
#define CLASS_TOOLBUTTON                  41
#define CLASS_TOOLSEPARATOR               42
#define CLASS_MENUTOOLBUTTON              43
#define CLASS_TOGGLETOOLBUTTON            44
#define CLASS_RADIOMENUITEM               45
#define CLASS_RADIOTOOLBUTTON             46
#define CLASS_TREEVIEW                    47
#define CLASS_COMBOBOX                    48
#define CLASS_COMBOBOXTEXT                49
#define CLASS_STATUSBAR                   50
#define CLASS_FIXED                       51
#define CLASS_CALENDAR                    52
#define CLASS_HTML                        53
#define CLASS_TEXTTAG                     54
#define CLASS_TABLE                       55
#define CLASS_PROPERTIESBOX               56
#define CLASS_ACTIVEX                     57
#define CLASS_PRINTER                     58
#define CLASS_STOCKMENUITEM               59 // is IMAGEMENUITEM !
#define CLASS_ICONVIEW                    60
#define CLASS_MIRRORBIN                   61
#define CLASS_SEARCHBAR                   62
#define CLASS_CALENDARTIME                63
#define CLASS_TOGGLEBUTTON                64
#define CLASS_IMAGEBUTTON                 65

// general (window) messages
#define P_CAPTION                         100
#define P_RESIZABLE                       101
#define P_HEIGHT                          102
#define P_WIDTH                           103
#define P_LEFT                            104
#define P_TOP                             105
#define P_MODAL                           106
#define P_MAXIMIZED                       107
#define P_MINIMIZED                       108
#define P_FULLSCREEN                      109

#define P_NORMAL_BG_COLOR                 110
#define P_INACTIVE_BG_COLOR               111
#define P_SELECTED_BG_COLOR               112

#define P_NORMAL_FG_COLOR                 113
#define P_INACTIVE_FG_COLOR               114
#define P_SELECTED_FG_COLOR               115

#define P_TITLEBAR                        116
#define P_DEFAULTCONTROL                  117
#define P_BORDERWIDTH                     118

#define P_VISIBLE                         119
#define P_TOOLTIP                         120
#define P_POPUPMENU                       121

#define P_ENABLED                         122
#define P_DECORATIONS                     123
#define P_SKIPTASKBAR                     124
#define P_KEEPABOVE                       125
#define P_KEEPBELOW                       126
#define P_SKIPPAGER                       127
#define P_PACKTYPE                        128
#define P_FOCUS                           129
// special message:
#define P_FIXED                           130
#define P_DISPOSE                         131
#define P_PACKING                         132
#define P_PARENT                          133
#define P_MOUSECURSOR                     134
#define P_MOUSECURSORIMAGE                135
// window
#define P_CLOSEABLE                       136
#define P_URGENT                          137
#define P_TYPE                            138
#define P_OPACITY                         139

#define P_TOOLTIPMARKUP                   140
#define P_TOOLTIPWINDOW                   141
#define P_SHOWTOOLTIP                     142
#define P_FORMMASK                        143

#define P_PRESENTYOURSELF                 160
#define P_SCREENSHOT                      161

#define P_BACKGROUNDIMAGE                 162
#define P_CLIENTHANDLE                    163

#define P_SCREEN                          164
#define P_COOLSYSTEMWINDOW                165

// button messages
#define P_RELIEFSTYLE                     200
#define P_IMAGE                           201
#define P_LAYOUT                          202
#define P_USESTOCK                        203
#define P_IMAGEPOSITION                   204

//box
#define P_SPACING                         300
#define P_HOMOGENEOUS                     301
#define P_XPADDING                        302
#define P_YPADDING                        303
#define P_MINHEIGHT                       304
#define P_MINWIDTH                        305
#define P_STYLE                           306

//edit
#define P_MASKED                          400
#define P_MASKEDCHAR                      401
#define P_MAXLEN                          402
#define P_READONLY                        403
#define P_ACTIVATEDEFAULT                 404
#define P_BORDER                          405
#define P_SUGESTION                       406
#define P_SUGESTIONMODEL                  407
#define P_CAPSWARNING                     408
#define P_PRIMARYICONNAME                 409
#define P_SECONDARYICONNAME               410
#define P_PRIMARYICON                     411
#define P_SECONDARYICON                   412
#define P_PRIMARYACTIVABLE                413
#define P_SECONDARYACTIVABLE              414
#define P_PRIMARYTOOLTIP                  415
#define P_SECONDARYTOOLTIP                416
// same ID
#define P_ABSOLUTE                        417
#define P_FORMAT                          418


// radio & check
#define P_CHECKED              500
#define P_INCONSISTENT         501
#define P_GROUP                502

// label
#define P_ANGLE                600
#define P_SELECTABLE           601
#define P_WRAP                 602
#define P_JUSTIFY              603
#define P_PATTERN              604
#define P_MARKUP               605
#define P_USEUNDERLINE         606

// frame
#define P_SHADOWTYPE           700
#define P_VTEXTALIGN           701
#define P_HTEXTALIGN           702

// progress bar
#define P_FRACTION             800
#define P_ORIENTATION          801
#define P_STEP                 802

// paned
#define P_POSITION             803

// Adjustment
#define P_LOWER                900
#define P_UPPER                901
#define P_VALUE                902
#define P_INCREMENT            903
#define P_PAGEINCREMENT        904
#define P_PAGESIZE             905

// v/h scale
#define P_DIGITS               1001
#define P_DRAWVALUE            1002
#define P_VALUEPOS             1003

//scrollbar
#define P_INVERTED             1100

//spinbutton
#define P_SNAPTOTICKS          1200
#define P_NUMERIC              1201

//textview
#define P_ACCEPTTAB            1300
#define P_CURSORVISIBLE        1301
#define P_INDENT               1302
#define P_LEFTMARGIN           1303
#define P_OVERWRITE            1304
#define P_PIXELSABOVE          1305
#define P_PIXELSBELOW          1306
#define P_PIXELSINSIDE         1307
#define P_RIGHTMARGIN          1308
#define P_ADDTEXT              1309
#define P_PRINT                1310
#define P_SELECTIONSTART       1311
#define P_SELECTIONLENGTH      1312

//alignment
#define P_XSCALE               1401
#define P_YSCALE               1402
#define P_XALIGN               1403
#define P_YALIGN               1404
#define P_TOPPADDING           1405
#define P_LEFTPADDING          1406
#define P_RIGHTPADDING         1407
#define P_BOTTOMPADDING        1408

//notebook
#define P_PAGE                 1500
#define P_SCROLLABLE           1501
#define P_SHOWBORDER           1502
#define P_SHOWTABS             1503
#define P_TABBORDER            1504
#define P_TABPOS               1505
#define P_VERTICALBORDER       1506
#define P_HORIZONTALBORDER     1507
#define P_ACTIONWIDGETSTART    1508
#define P_ACTIONWIDGETEND      1509

//scrolled window
#define P_HADJUSTMENT          1600
#define P_VADJUSTMENT          1601
#define P_PLACEMENT            1602
#define P_HSCROLLBARPOLICY     1603
#define P_VSCROLLBARPOLICY     1604

//aspect frame
#define P_OBEYCHILD            1700
#define P_RATIO                1701

//drag & drop
#define P_DRAGOBJECT           1801
#define P_DROPSITE             1802
#define P_DRAGDATA             1803
#define P_DRAGICON             1804

// item properties
#define P_SELECTED             1900
#define P_TOGGLE               1901

// menuitem
#define P_SUBMENU              2000
#define P_RIGHTJUSTIFIED       2001
#define P_ACCELKEY             2002
#define P_MENUOBJECT           2003

// checked menuitem
#define P_DRAWASRADIO          2100

// expaneder
#define P_EXPANDED             2200

// handlebox
#define P_INDRAG               2300
#define P_ISDETACHED           2301
#define P_HANDLEPOSITION       2302
#define P_SNAPEDGE             2303

// toolbar
#define P_ICONSIZE             2400
#define P_SHOWARROW            2401
#define P_TOOLBARSTYLE         2402
#define P_TOOLTIPS             2403

// toobutton
#define P_ISIMPORTANT          2500
#define P_VISIBLEVERTICAL      2501
#define P_VISIBLEHORIZONTAL    2502
#define P_ICON                 2503
#define P_EXPAND               2504

// menutoolbar
#define P_MENU                 2600

// treeview !!!
#define P_ENABLESEARCH         2701
#define P_EXPANDERCOLUMN       2702
#define P_FIXEDHEIGHTMODE      2703
#define P_HEADERSCLICKABLE     2704
#define P_HEADERSVISIBLE       2705
#define P_HOVEREXPAND          2706
#define P_HOVERSELECTION       2707
#define P_MODEL                2708
#define P_REORDABLE            2709
#define P_RULESHINT            2710
#define P_SEARCHCOLUMN         2711
#define P_SEARCHENTRY          2712
#define P_CURSOR               2713
#define P_GRIDLINES            2714
#define P_TREELINES            2715
#define P_LEVELIDENTATION      2716
#define P_SHOWEXPANDERS        2717
#define P_MULTIPLESELECTION    2718
#define P_SELECTEDCOLUMN       2719
#define P_SCROLLTOCOLUMN       2720
#define P_SCROLLTOROW          2721
#define P_SELECT               2722
#define P_UNSELECT             2733

// combobox
//P_CURSOR
#define P_ADDTEAROFFS      2800
#define P_FOCUSONCLICK     2801
#define P_WRAPWIDTH        2802
#define P_ROWSPANCOLUMN    2803
//P_HASFRAME

// comboboxentry
#define P_TEXTCOLUMN           2900

// font
#define P_FONTFAMILY           3000
#define P_FONTSTYLE            3001
#define P_FONTVARIANT          3002
#define P_FONTWEIGHT           3003
#define P_FONTSTRETCH          3004
#define P_FONTSIZE             3005
#define P_FONTNAME             3006

// calendar
#define P_DAY                  3100
#define P_MONTH                3101
#define P_YEAR                 3102

// text tag
#define P_PRIORITY             3201
#define P_UNDERLINE            3202
#define P_STRIKEOUT            3203
#define P_STRETCH              3204
#define P_RISE                 3205
#define P_LANGUAGE             3206
#define P_INVISIBLE            3207
#define P_BACKGROUNDFULL       3208
#define P_SCALE                3209
#define P_FORECOLOR            3210
#define P_BACKCOLOR            3211
#define P_DIRECTION            3212

// tree column
#define P_ALIGNMENT            3301
#define P_CLICKABLE            3302
#define P_FIXEDWIDTH           3303
#define P_MAXWIDTH             3304
#define P_REORDERABLE          3305
#define P_SIZING               3306
#define P_SORTINDICATOR        3307
#define P_SORTINDICATORTYPE    3308
#define P_WIDGET               3309
#define P_SENSITIVE            3310

// table
#define P_COLUMNS              3400
#define P_ROWS                 3401
#define P_ROWSPACING           3402
#define P_COLUMNSPACING        3403
#define P_ATTLEFT              3404
#define P_ATTRIGHT             3405
#define P_ATTTOP               3406
#define P_ATTBOTTOM            3407
#define P_ATTHOPT              3408
#define P_ATTVOPT              3409
#define P_ATTHSPACING          3410
#define P_ATTVSPACING          3411

// properties box
#define P_PROPERTIES           3500
#define P_CHANGEDPROPERTIES    3501

// IDE look properties !!!
#define P_USERRESIZABLE        3600
#define P_DOTTED               3601


// printer
#define P_UNIT                   3700
#define P_BOTTOMMARGIN           3701
//		LeftMargin	%P_LEFTMARGIN
//		RightMargin	%P_RIGHTMARGIN
#define P_TOPMARGIN              3702
#define P_PAGEHEIGHT             3703
#define P_PAGEWIDTH              3704
#define P_PAPERHEIGHT            3705
#define P_PAPERWIDTH             3706
#define P_PAPERSIZE              3707
//#define P_ORIENTATION           3708

#define P_COLLATE                3709
#define P_DITHER                 3710
#define P_DUPLEX                 3711
#define P_FINISHINGS             3712
#define P_MEDIATYPE              3713
#define P_COPIES                 3714
#define P_NUMBERUP               3715
#define P_ORIENTATION_2          3716
#define P_OUTPUTBIN              3717
#define P_PAGERANGES             3718
#define P_PAGESET                3719
#define P_PAPERHEIGHT_2          3720
#define P_PAPERSIZE_2            3721
#define P_PAPERWIDTH_2           3722
#define P_PRINTPAGES             3723
#define P_PRINTER                3724
#define P_QUALITY                3725
#define P_RESOLUTION             3726
#define P_REVERSE                3727
//#define P_SCALE                 3728
#define P_USECOLOR               3729

#define P_CURRENTPAGE            3730
#define P_CAPABILITES            3731

// IconView
#define P_IMAGECOLUMN            3800
#define P_MARKUPCOLUMN           3801
#define P_ITEMWIDTH              3803
#define P_TOOLTIPCOLUMN          3804

// mobile properties
#define P_FILLVERTICALLY         8001
#define P_FILLHORIZONTICALLY     8002
#define P_WRAPVERTICALLY         8003
#define P_WRAPHORIZONTAICALLY    8004
#define P_GRADIENTCOLOR          8005

#define P_SAMPLERATE             1000
#define P_NUMCHANNELS            1001
#define P_RECORD                 1002
#define P_ADDBUFFER              1003
#define P_BEGINPLAYBACK          1004
#define P_QUALITY2               1005
#define P_USECOMPRESSION         1006
#define P_ADDBUFFER2             1007
