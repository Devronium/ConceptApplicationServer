//---------------------------------------------------------------//
// Concept messages list                                         //
//---------------------------------------------------------------//
var CC_BUTTONS_YES_NO_CANCEL          = 6;
var CC_BUTTONS_SAVE_DISCARD_CANCEL    = 7;

var PACK_SHRINK                       = 0;
var PACK_EXPAND_PADDING               = 1;
var PACK_EXPAND_WIDGET                = 2;

var MSG_FLUSH_UI                      = -12;
var MSG_RAISE_ERROR                   = -11;
var MSG_DEBUGGER_TRAPPED              = -10;
var MSG_MESSAGE_REQUESTINPUT          = -8;
var MSG_MESSAGE_LOGIN                 = -7;
var MSG_MESSAGE_BOX_GENERIC           = -6;
var MSG_MESSAGE_BOX_YESNO             = -5;
var MSG_NON_CRITICAL                  = -4;
var MSG_CONSOLE                       = -3;
var MSG_MESSAGE_BOX                   = -2;
var MSG_RAW_OUTPUT                    = -1;

var MSG_CHOOSE_COLOR                  = 0x90;
var MSG_CHOOSE_FONT                   = 0x91;
var MSG_SET_PROPERTY_BY_NAME          = 0x92;

var MSG_CREATE                        = 0x100;
var MSG_SET_PROPERTY                  = 0x110;
var MSG_PUT_STREAM                    = 0x111;
var MSG_PUT_SECONDARY_STREAM          = 0x112;
var MSG_POST_OBJECT                   = 0x113;
var MSG_REMOTE_METHOD                 = 0x114;
var MSG_GET_PROPERTY                  = 0x115;
var MSG_POST_STRING                   = 0x116;
var MSG_SEND_COOKIE                   = 0x117;
var MSG_OS_COMMAND                    = 0x118;
var MSG_REORDER_CHILD                 = 0x119;

var MSG_CUSTOM_MESSAGE1               = 0x120;
var MSG_CUSTOM_MESSAGE2               = 0x121;
var MSG_CUSTOM_MESSAGE3               = 0x122;
var MSG_CUSTOM_MESSAGE4               = 0x123;
var MSG_CUSTOM_MESSAGE5               = 0x124;
var MSG_CUSTOM_MESSAGE6               = 0x125;
var MSG_CUSTOM_MESSAGE7               = 0x126;
var MSG_CUSTOM_MESSAGE8               = 0x127;
var MSG_CUSTOM_MESSAGE9               = 0x128;
var MSG_CUSTOM_MESSAGE10              = 0x129;
var MSG_CUSTOM_MESSAGE11              = 0x12A;
var MSG_CUSTOM_MESSAGE12              = 0x12B;
var MSG_CUSTOM_MESSAGE13              = 0x12C;
var MSG_CUSTOM_MESSAGE14              = 0x12D;
var MSG_CUSTOM_MESSAGE15              = 0x12E;
var MSG_CUSTOM_MESSAGE16              = 0x12F;
var MSG_REPAINT                       = 0x130;

var MSG_REQUEST_FOR_FILE              = 0x131;
var MSG_SAVE_FILE                     = 0x132;
var MSG_SET_CLIPBOARD                 = 0x133;
var MSG_GET_CLIPBOARD                 = 0x134;
var MSG_STATIC_QUERY                  = 0x135;
var MSG_STATIC_RESPONSE               = 0x136;
var MSG_SAVE_FILE2                    = 0x137;
var MSG_REQUEST_FOR_CHUNK             = 0x138;
var MSG_SAVE_CHUNK                    = 0x139;
var MSG_CHUNK                         = 0x13A;
var MSG_SEND_COOKIE_CHUNK             = 0x13B;

var MSG_D_PRIMITIVE_LINE              = 0x140;
var MSG_D_PRIMITIVE_ARC               = 0x141;
var MSG_D_PRIMITIVE_FILL_ARC          = 0x142;
var MSG_D_PRIMITIVE_RECT              = 0x143;
var MSG_D_PRIMITIVE_FILL_RECT         = 0x144;
var MSG_D_PRIMITIVE_POINT             = 0x145;
var MSG_D_PRIMITIVE_COLOR             = 0x146;

var MSG_SEND_ZCOOKIE                  = 0x147;

var MSG_SET_COOKIE                    = 0x148;
var MSG_GET_COOKIE                    = 0x149;

var MSG_GET_DNDFILE                   = 0x14A;

var MSG_CLIENT_ENVIRONMENT            = 0x14B;
var MSG_OS_COMMAND_CLOSE              = 0x14C;
var MSG_OS_COMMAND_QUEUE_CLEAR        = 0x14D;

var MSG_CREATE_ACTIVEX                = 0x420;
var MSG_AX_SET_PROPERTY               = 0x421;
var MSG_AX_ADD_PARAMETER              = 0x422;
var MSG_AX_CALL_METHOD                = 0x423;
var MSG_AX_ADD_FILEPARAMETER          = 0x424;
var MSG_AX_GET_PROPERTY               = 0x425;
var MSG_AX_GET_RESULT                 = 0x426;
var MSG_AX_ADD_EVENT                  = 0x427;
var MSG_AX_AKNOWLEDGE_EVENT           = 0x428;
var MSG_AX_CLEAR_EVENTS_DATA          = 0x429;
var MSG_AX_GET_EVENT_PARAM            = 0x430;

var MSG_APPLICATION_RUN               = 0x800;
var MSG_APPLICATION_QUIT              = 0x500;
var MSG_MESSAGING_SYSTEM              = 0x501;
var MSG_CLIENT_QUERY                  = 0x503;
var MSG_RUN_APPLICATION               = 0x504;
var MSG_DEBUG_APPLICATION             = 0x505;

var MSG_SET_EVENT                     = 0x1000;
var MSG_EVENT_FIRED                   = 0x1001;
var MSG_CONFIRM_EVENT                 = 0x1002;

// default events;
var EVENT_ON_SHOW                     = 3;
var EVENT_ON_HIDE                     = 4;
var EVENT_ON_MAP                      = 5;
var EVENT_ON_UNMAP                    = 6;
var EVENT_ON_REALIZE                  = 7;
var EVENT_ON_UNREALIZE                = 8;
var EVENT_ON_SIZEREQUEST              = 9;
var EVENT_ON_SIZEALLOCATE             = 10;
var EVENT_ON_STATECHANGED             = 11;
var EVENT_ON_PARENTCHANGED            = 12;
var EVENT_ON_HIERARCHYCHANGED         = 13;
var EVENT_ON_STYLECHANGED             = 14;
var EVENT_ON_DIRECTIONCHANGED         = 15;
var EVENT_ON_GRABNOTIFY               = 16;
var EVENT_ON_CHILDNOTIFY              = 17;
var EVENT_ON_MNEMONICACTIVATE         = 18;
var EVENT_ON_GRABFOCUS                = 19;
var EVENT_ON_FOCUS                    = 20;
var EVENT_ON_EVENT                    = 21;
var EVENT_ON_BUTTONPRESS              = 22;
var EVENT_ON_BUTTONRELEASE            = 23;
var EVENT_ON_SCROLL                   = 24;
var EVENT_ON_MOTIONNOTIFY             = 25;
var EVENT_ON_DELETEEVENT              = 26;
var EVENT_ON_EXPOSEEVENT              = 27;
var EVENT_ON_KEYPRESS                 = 28;
var EVENT_ON_KEYRELEASE               = 29;
var EVENT_ON_ENTERNOTIFY              = 30;
var EVENT_ON_LEAVENOTIFY              = 31;
var EVENT_ON_CONFIGURE                = 32;
var EVENT_ON_FOCUSIN                  = 33;
var EVENT_ON_FOCUSOUT                 = 34;
var EVENT_ON_MAPEVENT                 = 35;
var EVENT_ON_UNMAPEVENT               = 36;
var EVENT_ON_PROPERTY                 = 37;
var EVENT_ON_SELECTIONCLEAR           = 38;
var EVENT_ON_SELECTIONREQUEST         = 39;
var EVENT_ON_SELECTIONNOTIFY          = 40;
var EVENT_ON_PROXIMITYIN              = 41;
var EVENT_ON_PROXIMITYOUT             = 42;
var EVENT_ON_VISIBILITY               = 43;
var EVENT_ON_CLIENT                   = 44;
var EVENT_ON_NOEXPOSE                 = 45;
var EVENT_ON_WINDOWSTATE              = 46;
var EVENT_ON_SELECTIONGET             = 47;
var EVENT_ON_SELECTIONRECEIVED        = 48;
var EVENT_ON_DRAGBEGIN                = 49;
var EVENT_ON_DRAGEND                  = 50;
var EVENT_ON_DRAGDATAGET              = 51;
var EVENT_ON_DRAGDATADELETE           = 52;
var EVENT_ON_DRAGLEAVE                = 53;
var EVENT_ON_DRAGMOTION               = 54;
var EVENT_ON_DRAGDROP                 = 55;
var EVENT_ON_DRAGDATARECEIVED         = 56;
var EVENT_ON_SCREENCHANGED            = 57;

// radio&check buttons;
var EVENT_ON_TOGGLED                  = 2;
var EVENT_ON_GROUPCHANGED             = 58;

// button events;
var EVENT_ON_CLICKED                  = 1;
var EVENT_ON_ACTIVATE                 = 59;
var EVENT_ON_PRESSED                  = 60;
var EVENT_ON_RELEASED                 = 61;
var EVENT_ON_ENTER                    = 62;
var EVENT_ON_LEAVE                    = 63;

// entry events;
var EVENT_ON_INSERTATCURSOR           = 64;
var EVENT_ON_POPULATEPOPUP            = 65;

// textview events;
var EVENT_ON_SETANCHOR                = 66;
var EVENT_ON_SETSCROLLADJUSTMENTS     = 67;

// spinbutton events;
var EVENT_ON_INPUT                    = 68;
var EVENT_ON_OUTPUT                   = 69;
var EVENT_ON_VALUECHANGED             = 70;

// window events;
var EVENT_ON_FRAMEEVENT               = 71;
var EVENT_ON_SETFOCUS                 = 72;

// notebook events;
var EVENT_ON_SWITCHPAGE               = 73;

// adjustment events;
var EVENT_ON_CHANGED                  = 74;

// item events;
var EVENT_ON_SELECT                   = 75;
var EVENT_ON_DESELECT                 = 76;

// menuitem;
var EVENT_ON_ACTIVATEITEM             = 77;

// handlebox;
var EVENT_ON_CHILDATTACHED            = 78;
var EVENT_ON_CHILDDETACHED            = 79;

// toolbar;
var EVENT_ON_ORIENTATIONCHANGED       = 80;
var EVENT_ON_POPUPCONTEXTMENU         = 81;
var EVENT_ON_TOOLBARSTYLECHANGED      = 82;

// toolbutton;
var EVENT_ON_CREATEMENUPROXY          = 83;
var EVENT_ON_TOOLBARRECONFIGURED      = 84;

// menutoolbutton;
var EVENT_ON_SHOWMENU                 = 85;

// treeview;
var EVENT_ON_COLUMNSCHANGED           = 86;
var EVENT_ON_CURSORCHANGED            = 87;
var EVENT_ON_ROWACTIVATED             = 88;
var EVENT_ON_ROWCOLLAPSED             = 89;
var EVENT_ON_ROWEXPANDED              = 90;

// timer;
var EVENT_ON_TIMER                    = 91;

// calendar;
var EVENT_ON_DAYSELECTED              = 92;
var EVENT_ON_DAYSELECTEDDBCLICK       = 93;
var EVENT_ON_MONTHCHANGED             = 94;
var EVENT_ON_NEXTMONTH                = 95;
var EVENT_ON_NEXTYEAR                 = 96;
var EVENT_ON_PREVMONTH                = 97;
var EVENT_ON_PREVYEAR                 = 98;

// treeview columns;
var EVENT_ON_STARTEDITING             = 99;
var EVENT_ON_ENDEDITING               = 100;
var EVENT_ON_CANCELEDITING            = 101;

// ActiveX events;
var EVENT_ACTIVEX_EVENT               = 102;

// range derived;
var EVENT_ON_ADJUSTBOUNDS             = 103;
var EVENT_ON_CHANGEVALUE              = 104;
var EVENT_ON_MOVESLIDER               = 105;

// treeview columns again;
var EVENT_ON_HEADERCLICKED            = 106;
var EVENT_ON_COLUMNRESIZED            = 107;

var CLASS_FORM                        = 1;
var CLASS_BUTTON                      = 2;
var CLASS_HBOX                        = 3;
var CLASS_VBOX                        = 4;
var CLASS_HBUTTONBOX                  = 5;
var CLASS_VBUTTONBOX                  = 6;
var CLASS_EDIT                        = 7;
var CLASS_CHECKBUTTON                 = 8;
var CLASS_RADIOBUTTON                 = 9;
var CLASS_HSEPARATOR                  = 10;
var CLASS_VSEPARATOR                  = 11;
var CLASS_LABEL                       = 12;
var CLASS_FRAME                       = 13;
var CLASS_PROGRESSBAR                 = 14;
var CLASS_HPANED                      = 15;
var CLASS_VPANED                      = 16;
var CLASS_SCROLLEDWINDOW              = 17;
var CLASS_IMAGE                       = 18;
var CLASS_ADJUSTMENT                  = 19;
var CLASS_VSCALE                      = 20;
var CLASS_HSCALE                      = 21;
var CLASS_VSCROLLBAR                  = 22;
var CLASS_HSCROLLBAR                  = 23;
var CLASS_SPINBUTTON                  = 24;
var CLASS_TEXTVIEW                    = 25;
var CLASS_ALIGNMENT                   = 26;
var CLASS_NOTEBOOK                    = 27;

var CLASS_ASPECTFRAME                 = 28;
var CLASS_MENUITEM                    = 29;
var CLASS_MENU                        = 30;
var CLASS_MENUBAR                     = 31;
var CLASS_SEPARATORMENUITEM           = 32;
var CLASS_IMAGEMENUITEM               = 33;
var CLASS_TEAROFFMENUITEM             = 34;
var CLASS_CHECKMENUITEM               = 35;
var CLASS_EVENTBOX                    = 36;
var CLASS_EXPANDER                    = 37;
var CLASS_VIEWPORT                    = 38;
var CLASS_HANDLEBOX                   = 39;
var CLASS_TOOLBAR                     = 40;
var CLASS_TOOLBUTTON                  = 41;
var CLASS_TOOLSEPARATOR               = 42;
var CLASS_MENUTOOLBUTTON              = 43;
var CLASS_TOGGLETOOLBUTTON            = 44;
var CLASS_RADIOMENUITEM               = 45;
var CLASS_RADIOTOOLBUTTON             = 46;
var CLASS_TREEVIEW                    = 47;
var CLASS_COMBOBOX                    = 48;
var CLASS_COMBOBOXTEXT                = 49;
var CLASS_STATUSBAR                   = 50;
var CLASS_FIXED                       = 51;
var CLASS_CALENDAR                    = 52;
var CLASS_HTML                        = 53;
var CLASS_TEXTTAG                     = 54;
var CLASS_TABLE                       = 55;
var CLASS_PROPERTIESBOX               = 56;
var CLASS_ACTIVEX                     = 57;
var CLASS_PRINTER                     = 58;
var CLASS_STOCKMENUITEM               = 59;
var CLASS_ICONVIEW                    = 60;
var CLASS_MIRRORBIN                   = 61;
var CLASS_SEARCHBAR                   = 62;
var CLASS_CALENDARTIME                = 63;
var CLASS_TOGGLEBUTTON                = 64;
var CLASS_IMAGEBUTTON                 = 65;
var CLASS_CLIENTCHART                 = 66;
var CLASS_HTMLSNAP                    = 67;
var CLASS_HTMLAPP                     = 68;

// general (window) messages
var P_CAPTION                         = 100;
var P_RESIZABLE                       = 101;
var P_HEIGHT                          = 102;
var P_WIDTH                           = 103;
var P_LEFT                            = 104;
var P_TOP                             = 105;
var P_MODAL                           = 106;
var P_MAXIMIZED                       = 107;
var P_MINIMIZED                       = 108;
var P_FULLSCREEN                      = 109;

var P_NORMAL_BG_COLOR                 = 110;
var P_INACTIVE_BG_COLOR               = 111;
var P_SELECTED_BG_COLOR               = 112;

var P_NORMAL_FG_COLOR                 = 113;
var P_INACTIVE_FG_COLOR               = 114;
var P_SELECTED_FG_COLOR               = 115;

var P_TITLEBAR                        = 116;
var P_DEFAULTCONTROL                  = 117;
var P_BORDERWIDTH                     = 118;

var P_VISIBLE                         = 119;
var P_TOOLTIP                         = 120;
var P_POPUPMENU                       = 121;

var P_ENABLED                         = 122;
var P_DECORATIONS                     = 123;
var P_SKIPTASKBAR                     = 124;
var P_KEEPABOVE                       = 125;
var P_KEEPBELOW                       = 126;
var P_SKIPPAGER                       = 127;
var P_PACKTYPE                        = 128;
var P_FOCUS                           = 129;
// special message:
var P_FIXED                           = 130;
var P_DISPOSE                         = 131;
var P_PACKING                         = 132;
var P_PARENT                          = 133;
var P_MOUSECURSOR                     = 134;
var P_MOUSECURSORIMAGE                = 135;
// window
var P_CLOSEABLE                       = 136;
var P_URGENT                          = 137;
var P_TYPE                            = 138;
var P_OPACITY                         = 139;

var P_TOOLTIPMARKUP                   = 140;
var P_TOOLTIPWINDOW                   = 141;
var P_SHOWTOOLTIP                     = 142;
var P_FORMMASK                        = 143;

var P_PRESENTYOURSELF                 = 160;
var P_SCREENSHOT                      = 161;

var P_BACKGROUNDIMAGE                 = 162;
var P_CLIENTHANDLE                    = 163;

var P_SCREEN                          = 164;
var P_COOLSYSTEMWINDOW                = 165;

// button messages
var P_RELIEFSTYLE                     = 200;
var P_IMAGE                           = 201;
var P_LAYOUT                          = 202;
var P_USESTOCK                        = 203;
var P_IMAGEPOSITION                   = 204;

//box
var P_SPACING                         = 300;
var P_HOMOGENEOUS                     = 301;
var P_XPADDING                        = 302;
var P_YPADDING                        = 303;
var P_MINHEIGHT                       = 304;
var P_MINWIDTH                        = 305;
var P_STYLE                           = 306;

//edit
var P_MASKED                          = 400;
var P_MASKEDCHAR                      = 401;
var P_MAXLEN                          = 402;
var P_READONLY                        = 403;
var P_ACTIVATEDEFAULT                 = 404;
var P_BORDER                          = 405;
var P_SUGESTION                       = 406;
var P_SUGESTIONMODEL                  = 407;
var P_CAPSWARNING                     = 408;
var P_PRIMARYICONNAME                 = 409;
var P_SECONDARYICONNAME               = 410;
var P_PRIMARYICON                     = 411;
var P_SECONDARYICON                   = 412;
var P_PRIMARYACTIVABLE                = 413;
var P_SECONDARYACTIVABLE              = 414;
var P_PRIMARYTOOLTIP                  = 415;
var P_SECONDARYTOOLTIP                = 416;
// same ID
var P_ABSOLUTE                        = 417;
var P_FORMAT                          = 418;


// radio & check
var P_CHECKED              = 500;
var P_INCONSISTENT         = 501;
var P_GROUP                = 502;

// label
var P_ANGLE                = 600;
var P_SELECTABLE           = 601;
var P_WRAP                 = 602;
var P_JUSTIFY              = 603;
var P_PATTERN              = 604;
var P_MARKUP               = 605;
var P_USEUNDERLINE         = 606;

// frame
var P_SHADOWTYPE           = 700;
var P_VTEXTALIGN           = 701;
var P_HTEXTALIGN           = 702;

// progress bar
var P_FRACTION             = 800;
var P_ORIENTATION          = 801;
var P_STEP                 = 802;

// paned
var P_POSITION             = 803;

// Adjustment
var P_LOWER                = 900;
var P_UPPER                = 901;
var P_VALUE                = 902;
var P_INCREMENT            = 903;
var P_PAGEINCREMENT        = 904;
var P_PAGESIZE             = 905;

// v/h scale
var P_DIGITS               = 1001;
var P_DRAWVALUE            = 1002;
var P_VALUEPOS             = 1003;

//scrollbar
var P_INVERTED             = 1100;

//spinbutton
var P_SNAPTOTICKS          = 1200;
var P_NUMERIC              = 1201;

//textview
var P_ACCEPTTAB            = 1300;
var P_CURSORVISIBLE        = 1301;
var P_INDENT               = 1302;
var P_LEFTMARGIN           = 1303;
var P_OVERWRITE            = 1304;
var P_PIXELSABOVE          = 1305;
var P_PIXELSBELOW          = 1306;
var P_PIXELSINSIDE         = 1307;
var P_RIGHTMARGIN          = 1308;
var P_ADDTEXT              = 1309;
var P_PRINT                = 1310;
var P_SELECTIONSTART       = 1311;
var P_SELECTIONLENGTH      = 1312;

//alignment
var P_XSCALE               = 1401;
var P_YSCALE               = 1402;
var P_XALIGN               = 1403;
var P_YALIGN               = 1404;
var P_TOPPADDING           = 1405;
var P_LEFTPADDING          = 1406;
var P_RIGHTPADDING         = 1407;
var P_BOTTOMPADDING        = 1408;

//notebook
var P_PAGE                 = 1500;
var P_SCROLLABLE           = 1501;
var P_SHOWBORDER           = 1502;
var P_SHOWTABS             = 1503;
var P_TABBORDER            = 1504;
var P_TABPOS               = 1505;
var P_VERTICALBORDER       = 1506;
var P_HORIZONTALBORDER     = 1507;
var P_ACTIONWIDGETSTART    = 1508;
var P_ACTIONWIDGETEND      = 1509;

//scrolled window;
var P_HADJUSTMENT          = 1600;
var P_VADJUSTMENT          = 1601;
var P_PLACEMENT            = 1602;
var P_HSCROLLBARPOLICY     = 1603;
var P_VSCROLLBARPOLICY     = 1604;

//aspect frame
var P_OBEYCHILD            = 1700;
var P_RATIO                = 1701;

//drag & drop
var P_DRAGOBJECT           = 1801;
var P_DROPSITE             = 1802;
var P_DRAGDATA             = 1803;
var P_DRAGICON             = 1804;

// item properties
var P_SELECTED             = 1900;
var P_TOGGLE               = 1901;

// menuitem
var P_SUBMENU              = 2000;
var P_RIGHTJUSTIFIED       = 2001;
var P_ACCELKEY             = 2002;
var P_MENUOBJECT           = 2003;

// checked menuitem
var P_DRAWASRADIO          = 2100;

// expander
var P_EXPANDED             = 2200;

// handlebox
var P_INDRAG               = 2300;
var P_ISDETACHED           = 2301;
var P_HANDLEPOSITION       = 2302;
var P_SNAPEDGE             = 2303;

// toolbar;
var P_ICONSIZE             = 2400;
var P_SHOWARROW            = 2401;
var P_TOOLBARSTYLE         = 2402;
var P_TOOLTIPS             = 2403;

// toobutton
var P_ISIMPORTANT          = 2500;
var P_VISIBLEVERTICAL      = 2501;
var P_VISIBLEHORIZONTAL    = 2502;
var P_ICON                 = 2503;
var P_EXPAND               = 2504;

// menutoolbar
var P_MENU                 = 2600;

// treeview !!!
var P_ENABLESEARCH         = 2701;
var P_EXPANDERCOLUMN       = 2702;
var P_FIXEDHEIGHTMODE      = 2703;
var P_HEADERSCLICKABLE     = 2704;
var P_HEADERSVISIBLE       = 2705;
var P_HOVEREXPAND          = 2706;
var P_HOVERSELECTION       = 2707;
var P_MODEL                = 2708;
var P_REORDABLE            = 2709;
var P_RULESHINT            = 2710;
var P_SEARCHCOLUMN         = 2711;
var P_SEARCHENTRY          = 2712;
var P_CURSOR               = 2713;
var P_GRIDLINES            = 2714;
var P_TREELINES            = 2715;
var P_LEVELIDENTATION      = 2716;
var P_SHOWEXPANDERS        = 2717;
var P_MULTIPLESELECTION    = 2718;
var P_SELECTEDCOLUMN       = 2719;

var P_SCROLLTOCOLUMN       = 2720;
var P_SCROLLTOROW          = 2721;
var P_SELECT               = 2722;
var P_UNSELECT             = 2733;

// combobox
//P_CURSOR;
var P_ADDTEAROFFS      = 2800;
var P_FOCUSONCLICK     = 2801;
var P_WRAPWIDTH        = 2802;
var P_ROWSPANCOLUMN    = 2803;
//P_HASFRAME;

// comboboxentry
var P_TEXTCOLUMN           = 2900;

// font
var P_FONTFAMILY           = 3000;
var P_FONTSTYLE            = 3001;
var P_FONTVARIANT          = 3002;
var P_FONTWEIGHT           = 3003;
var P_FONTSTRETCH          = 3004;
var P_FONTSIZE             = 3005;
var P_FONTNAME             = 3006;

// calendar
var P_DAY                  = 3100;
var P_MONTH                = 3101;
var P_YEAR                 = 3102;

// text tag
var P_PRIORITY             = 3201;
var P_UNDERLINE            = 3202;
var P_STRIKEOUT            = 3203;
var P_STRETCH              = 3204;
var P_RISE                 = 3205;
var P_LANGUAGE             = 3206;
var P_INVISIBLE            = 3207;
var P_BACKGROUNDFULL       = 3208;
var P_SCALE                = 3209;
var P_FORECOLOR            = 3210;
var P_BACKCOLOR            = 3211;
var P_DIRECTION            = 3212;

// tree column
var P_ALIGNMENT            = 3301;
var P_CLICKABLE            = 3302;
var P_FIXEDWIDTH           = 3303;
var P_MAXWIDTH             = 3304;
var P_REORDERABLE          = 3305;
var P_SIZING               = 3306;
var P_SORTINDICATOR        = 3307;
var P_SORTINDICATORTYPE    = 3308;
var P_WIDGET               = 3309;
var P_SENSITIVE            = 3310;

// table
var P_COLUMNS              = 3400;
var P_ROWS                 = 3401;
var P_ROWSPACING           = 3402;
var P_COLUMNSPACING        = 3403;
var P_ATTLEFT              = 3404;
var P_ATTRIGHT             = 3405;
var P_ATTTOP               = 3406;
var P_ATTBOTTOM            = 3407;
var P_ATTHOPT              = 3408;
var P_ATTVOPT              = 3409;
var P_ATTHSPACING          = 3410;
var P_ATTVSPACING          = 3411;

// properties box
var P_PROPERTIES           = 3500;
var P_CHANGEDPROPERTIES    = 3501;

// IDE look properties !!!
var P_USERRESIZABLE        = 3600;
var P_DOTTED               = 3601;


// printer
var P_UNIT             = 3700;
var P_BOTTOMMARGIN     = 3701;
var P_TOPMARGIN        = 3702;
var P_PAGEHEIGHT       = 3703;
var P_PAGEWIDTH        = 3704;
var P_PAPERHEIGHT      = 3705;
var P_PAPERWIDTH       = 3706;
var P_PAPERSIZE        = 3707;

var P_COLLATE          = 3709;
var P_DITHER           = 3710;
var P_DUPLEX           = 3711;
var P_FINISHINGS       = 3712;
var P_MEDIATYPE        = 3713;
var P_COPIES           = 3714;
var P_NUMBERUP         = 3715;
var P_ORIENTATION_2    = 3716;
var P_OUTPUTBIN        = 3717;

var P_PAGERANGES       = 3718;
var P_PAGESET          = 3719;
var P_PAPERHEIGHT_2    = 3720;
var P_PAPERSIZE_2      = 3721;
var P_PAPERWIDTH_2     = 3722;
var P_PRINTPAGES       = 3723;
var P_PRINTER          = 3724;
var P_QUALITY          = 3725;
var P_RESOLUTION       = 3726;
var P_REVERSE          = 3727;
var P_USECOLOR         = 3729;

var P_CURRENTPAGE      = 3730;
var P_CAPABILITES      = 3731;

// IconView
var P_IMAGECOLUMN      = 3800;
var P_MARKUPCOLUMN     = 3801;
var P_ITEMWIDTH        = 3803;
var P_TOOLTIPCOLUMN    = 3804;

// mobile properties
var P_FILLVERTICALLY      = 8001;
var P_FILLHORIZONTICALLY  = 8002;
var P_WRAPVERTICALLY      = 8003;
var P_WRAPHORIZONTAICALLY = 8004;
var P_GRADIENTCOLOR       = 8005;

var P_SAMPLERATE         = 1000;
var P_NUMCHANNELS        = 1001;
var P_RECORD             = 1002;
var P_ADDBUFFER          = 1003;
var P_BEGINPLAYBACK      = 1004;
var P_QUALITY2           = 1005;
var P_USECOMPRESSION     = 1006;
var P_ADDBUFFER2         = 1007;
var P_CLEAR              = 1008;
var P_BANDWIDTH          = 1009;
var P_BITRATE            = 3725;

var __ICON_SIZE_INVALID          = 0;
var __ICON_SIZE_MENU             = 1;
var __ICON_SIZE_SMALL_TOOLBAR    = 2;
var __ICON_SIZE_LARGE_TOOLBAR    = 3;
var __ICON_SIZE_BUTTON           = 4;
var __ICON_SIZE_DND              = 5;
var __ICON_SIZE_DIALOG           = 6;

var __ABOUT                      = 1;
var __ADD                        = 2;
var __APPLY                      = 3;
var __BOLD                       = 4;
var __CANCEL                     = 5;
var __CDROM                      = 6;
var __CLEAR                      = 7;
var __CLOSE                      = 8;
var __COLOR_PICKER               = 9;
var __CONNECT                    = 10;
var __CONVERT                    = 11;
var __COPY                       = 12;
var __CUT                        = 13;
var __DELETE                     = 14;
var __DIALOG_AUTHENTICATION      = 15;
var __DIALOG_ERROR               = 16;
var __DIALOG_INFO                = 17;
var __DIALOG_QUESTION            = 18;
var __DIALOG_WARNING             = 19;
var __DIRECTORY                  = 20;
var __DISCONNECT                 = 21;
var __DND                        = 22;
var __DND_MULTIPLE               = 23;
var __EDIT                       = 24;
var __EXECUTE                    = 25;
var __FILE                       = 26;
var __FIND                       = 27;
var __FIND_AND_REPLACE           = 28;
var __FLOPPY                     = 29;
var __FULLSCREEN                 = 30;
var __GO_BACK                    = 31;
var __GO_DOWN                    = 32;
var __GO_FORWARD                 = 33;
var __GO_UP                      = 34;
var __GOTO_BOTTOM                = 35;
var __GOTO_FIRST                 = 36;
var __GOTO_LAST                  = 37;
var __GOTO_TOP                   = 38;
var __HARDDISK                   = 39;
var __HELP                       = 40;
var __HOME                       = 41;
var __INDENT                     = 42;
var __INDEX                      = 43;
var __INFO                       = 44;
var __ITALIC                     = 45;
var __JUMP_TO                    = 46;
var __JUSTIFY_CENTER             = 47;
var __JUSTIFY_FILL               = 48;
var __JUSTIFY_LEFT               = 49;
var __JUSTIFY_RIGHT              = 50;
var __LEAVE_FULLSCREEN           = 51;
var __MEDIA_FORWARD              = 52;
var __MEDIA_NEXT                 = 53;
var __MEDIA_PAUSE                = 54;
var __MEDIA_PLAY                 = 55;
var __MEDIA_PREVIOUS             = 56;
var __MEDIA_RECORD               = 57;
var __MEDIA_REWIND               = 58;
var __MEDIA_STOP                 = 59;
var __MISSING_IMAGE              = 60;
var __NETWORK                    = 61;
var __NEW                        = 62;
var __NO                         = 63;
var __OK                         = 64;
var __OPEN                       = 65;
var __PASTE                      = 66;
var __PREFERENCES                = 67;
var __PRINT                      = 68;
var __PRINT_PREVIEW              = 69;
var __PROPERTIES                 = 70;
var __QUIT                       = 71;
var __REDO                       = 72;
var __REFRESH                    = 73;
var __REMOVE                     = 74;
var __REVERT_TO_SAVED            = 75;
var __SAVE                       = 76;
var __SAVE_AS                    = 77;
var __SELECT_COLOR               = 78;
var __SELECT_FONT                = 79;
var __SORT_ASCENDING             = 80;
var __SORT_DESCENDING            = 81;
var __SPELL_CHECK                = 82;
var __STOP                       = 83;
var __STRIKETHROUGH              = 84;
var __UNDELETE                   = 85;
var __UNDERLINE                  = 86;
var __UNDO                       = 87;
var __UNINDENT                   = 88;
var __YES                        = 89;
var __ZOOM_100                   = 90;
var __ZOOM_FIT                   = 91;
var __ZOOM_IN                    = 92;
var __ZOOM_OUT                   = 93;


var RESPONSE_NONE         = -1;
var RESPONSE_REJECT       = -2;
var RESPONSE_ACCEPT       = -3;
var RESPONSE_DELETE_EVENT = -4;
var RESPONSE_OK           = -5;
var RESPONSE_CANCEL       = -6;
var RESPONSE_CLOSE        = -7;
var RESPONSE_YES          = -8;
var RESPONSE_NO           = -9;
var RESPONSE_APPLY        = -10;
var RESPONSE_HELP         = -11;
var RESPONSE_SIGNUP       = -12

var MESSAGE_INFO          = 0;
var MESSAGE_WARNING       = 1;
var MESSAGE_QUESTION      = 2;
var MESSAGE_ERROR         = 3;
var MESSAGE_OTHER         = 4;

var BUTTONS_NONE                = 0;
var BUTTONS_OK                  = 1;
var BUTTONS_CLOSE               = 2;
var BUTTONS_CANCEL              = 3;
var BUTTONS_YES_NO              = 4;
var BUTTONS_OK_CANCEL           = 5;
var BUTTONS_YES_NO_CANCEL       = 6;
var BUTTONS_SAVE_DISCARD_CANCEL = 7;

var HIDDEN_COLUMN   = 0;
var NORMAL_COLUMN   = 1;
var PERCENT_COLUMN  = 2;
var CHECK_COLUMN    = 3;
var RADIO_COLUMN    = 4;
var IMAGE_COLUMN    = 5;
var COMBO_COLUMN    = 6;
var MARKUP_COLUMN   = 7;

var EDITABLE_COLUMN = 0x80;

var ORIENTATION_HORIZONTAL = 0;
var ORIENTATION_VERTICAL   = 1;

var TOOLBAR_ICONS          = 0;
var TOOLBAR_TEXT           = 1;
var TOOLBAR_BOTH           = 2;
var TOOLBAR_BOTH_HORIZ     = 3;

var POS_LEFT   = 0;
var POS_RIGHT  = 1;
var POS_TOP    = 2;
var POS_BOTTOM = 3;

var BUTTONBOX_DEFAULT_STYLE = 0;
var BUTTONBOX_SPREAD        = 1;
var BUTTONBOX_EDGE          = 2;
var BUTTONBOX_START         = 3;
var BUTTONBOX_END           = 4;

var PROP_INSTANT    = 0x01;
var PROP_OPTION     = 0x02;
var PROP_OPTIONEDIT = 0x03;
var PROP_COLOR      = 0x04;
var PROP_FONT       = 0x05;
var PROP_FILE       = 0x06;
var PROP_STATIC     = 0x07;
var PROP_LONGTEXT   = 0x08;
var PROP_BOOLEAN    = 0x09;
var PROP_DATE       = 0x0A;
var PROP_TIME       = 0x0B;
var PROP_DATETIME   = 0x0C;
var PROP_INTEGER    = 0x0D;
var PROP_DECIMAL    = 0x0E;

var res_prefix = "/@";
var js_flags = { };
var mimeTypes	=	{
				"ez" : "application/andrew-inset",
				"aw" : "application/applixware",
				"atom" : "application/atom+xml",
				"atomcat" : "application/atomcat+xml",
				"atomsvc" : "application/atomsvc+xml",
				"ccxml" : "application/ccxml+xml",
				"cdmia" : "application/cdmi-capability",
				"cdmic" : "application/cdmi-container",
				"cdmid" : "application/cdmi-domain",
				"cdmio" : "application/cdmi-object",
				"cdmiq" : "application/cdmi-queue",
				"cu" : "application/cu-seeme",
				"davmount" : "application/davmount+xml",
				"dbk" : "application/docbook+xml",
				"dssc" : "application/dssc+der",
				"xdssc" : "application/dssc+xml",
				"ecma" : "application/ecmascript",
				"emma" : "application/emma+xml",
				"epub" : "application/epub+zip",
				"exi" : "application/exi",
				"pfr" : "application/font-tdpfr",
				"gml" : "application/gml+xml",
				"gpx" : "application/gpx+xml",
				"gxf" : "application/gxf",
				"stk" : "application/hyperstudio",
				"ink" : "application/inkml+xml",
				"inkml" : "application/inkml+xml",
				"ipfix" : "application/ipfix",
				"jar" : "application/java-archive",
				"ser" : "application/java-serialized-object",
				"class" : "application/java-vm",
				"js" : "application/javascript",
				"json" : "application/json",
				"jsonml" : "application/jsonml+json",
				"lostxml" : "application/lost+xml",
				"hqx" : "application/mac-binhex40",
				"cpt" : "application/mac-compactpro",
				"mads" : "application/mads+xml",
				"mrc" : "application/marc",
				"mrcx" : "application/marcxml+xml",
				"ma" : "application/mathematica",
				"nb" : "application/mathematica",
				"mb" : "application/mathematica",
				"mathml" : "application/mathml+xml",
				"mbox" : "application/mbox",
				"mscml" : "application/mediaservercontrol+xml",
				"metalink" : "application/metalink+xml",
				"meta4" : "application/metalink4+xml",
				"mets" : "application/mets+xml",
				"mods" : "application/mods+xml",
				"m21" : "application/mp21",
				"mp21" : "application/mp21",
				"mp4s" : "application/mp4",
				"doc" : "application/msword",
				"dot" : "application/msword",
				"mxf" : "application/mxf",
				"bin" : "application/octet-stream",
				"dms" : "application/octet-stream",
				"lrf" : "application/octet-stream",
				"mar" : "application/octet-stream",
				"so" : "application/octet-stream",
				"dist" : "application/octet-stream",
				"distz" : "application/octet-stream",
				"pkg" : "application/octet-stream",
				"bpk" : "application/octet-stream",
				"dump" : "application/octet-stream",
				"elc" : "application/octet-stream",
				"deploy" : "application/octet-stream",
				"oda" : "application/oda",
				"opf" : "application/oebps-package+xml",
				"ogx" : "application/ogg",
				"omdoc" : "application/omdoc+xml",
				"onetoc" : "application/onenote",
				"onetoc2" : "application/onenote",
				"onetmp" : "application/onenote",
				"onepkg" : "application/onenote",
				"oxps" : "application/oxps",
				"xer" : "application/patch-ops-error+xml",
				"pdf" : "application/pdf",
				"pgp" : "application/pgp-encrypted",
				"asc" : "application/pgp-signature",
				"sig" : "application/pgp-signature",
				"prf" : "application/pics-rules",
				"p10" : "application/pkcs10",
				"p7m" : "application/pkcs7-mime",
				"p7c" : "application/pkcs7-mime",
				"p7s" : "application/pkcs7-signature",
				"p8" : "application/pkcs8",
				"ac" : "application/pkix-attr-cert",
				"cer" : "application/pkix-cert",
				"crl" : "application/pkix-crl",
				"pkipath" : "application/pkix-pkipath",
				"pki" : "application/pkixcmp",
				"pls" : "application/pls+xml",
				"ai" : "application/postscript",
				"eps" : "application/postscript",
				"ps" : "application/postscript",
				"cww" : "application/prs.cww",
				"pskcxml" : "application/pskc+xml",
				"rdf" : "application/rdf+xml",
				"rif" : "application/reginfo+xml",
				"rnc" : "application/relax-ng-compact-syntax",
				"rl" : "application/resource-lists+xml",
				"rld" : "application/resource-lists-diff+xml",
				"rs" : "application/rls-services+xml",
				"gbr" : "application/rpki-ghostbusters",
				"mft" : "application/rpki-manifest",
				"roa" : "application/rpki-roa",
				"rsd" : "application/rsd+xml",
				"rss" : "application/rss+xml",
				"rtf" : "application/rtf",
				"sbml" : "application/sbml+xml",
				"scq" : "application/scvp-cv-request",
				"scs" : "application/scvp-cv-response",
				"spq" : "application/scvp-vp-request",
				"spp" : "application/scvp-vp-response",
				"sdp" : "application/sdp",
				"setpay" : "application/set-payment-initiation",
				"setreg" : "application/set-registration-initiation",
				"shf" : "application/shf+xml",
				"smi" : "application/smil+xml",
				"smil" : "application/smil+xml",
				"rq" : "application/sparql-query",
				"srx" : "application/sparql-results+xml",
				"gram" : "application/srgs",
				"grxml" : "application/srgs+xml",
				"sru" : "application/sru+xml",
				"ssdl" : "application/ssdl+xml",
				"ssml" : "application/ssml+xml",
				"tei" : "application/tei+xml",
				"teicorpus" : "application/tei+xml",
				"tfi" : "application/thraud+xml",
				"tsd" : "application/timestamped-data",
				"plb" : "application/vnd.3gpp.pic-bw-large",
				"psb" : "application/vnd.3gpp.pic-bw-small",
				"pvb" : "application/vnd.3gpp.pic-bw-var",
				"tcap" : "application/vnd.3gpp2.tcap",
				"pwn" : "application/vnd.3m.post-it-notes",
				"aso" : "application/vnd.accpac.simply.aso",
				"imp" : "application/vnd.accpac.simply.imp",
				"acu" : "application/vnd.acucobol",
				"atc" : "application/vnd.acucorp",
				"acutc" : "application/vnd.acucorp",
				"air" : "application/vnd.adobe.air-application-installer-package+zip",
				"fcdt" : "application/vnd.adobe.formscentral.fcdt",
				"fxp" : "application/vnd.adobe.fxp",
				"fxpl" : "application/vnd.adobe.fxp",
				"xdp" : "application/vnd.adobe.xdp+xml",
				"xfdf" : "application/vnd.adobe.xfdf",
				"ahead" : "application/vnd.ahead.space",
				"azf" : "application/vnd.airzip.filesecure.azf",
				"azs" : "application/vnd.airzip.filesecure.azs",
				"azw" : "application/vnd.amazon.ebook",
				"acc" : "application/vnd.americandynamics.acc",
				"ami" : "application/vnd.amiga.ami",
				"apk" : "application/vnd.android.package-archive",
				"cii" : "application/vnd.anser-web-certificate-issue-initiation",
				"fti" : "application/vnd.anser-web-funds-transfer-initiation",
				"atx" : "application/vnd.antix.game-component",
				"mpkg" : "application/vnd.apple.installer+xml",
				"m3u8" : "application/vnd.apple.mpegurl",
				"swi" : "application/vnd.aristanetworks.swi",
				"iota" : "application/vnd.astraea-software.iota",
				"aep" : "application/vnd.audiograph",
				"mpm" : "application/vnd.blueice.multipass",
				"bmi" : "application/vnd.bmi",
				"rep" : "application/vnd.businessobjects",
				"cdxml" : "application/vnd.chemdraw+xml",
				"mmd" : "application/vnd.chipnuts.karaoke-mmd",
				"cdy" : "application/vnd.cinderella",
				"cla" : "application/vnd.claymore",
				"rp9" : "application/vnd.cloanto.rp9",
				"c4g" : "application/vnd.clonk.c4group",
				"c4d" : "application/vnd.clonk.c4group",
				"c4f" : "application/vnd.clonk.c4group",
				"c4p" : "application/vnd.clonk.c4group",
				"c4u" : "application/vnd.clonk.c4group",
				"c11amc" : "application/vnd.cluetrust.cartomobile-config",
				"c11amz" : "application/vnd.cluetrust.cartomobile-config-pkg",
				"cdbcmsg" : "application/vnd.contact.cmsg",
				"cmc" : "application/vnd.cosmocaller",
				"clkx" : "application/vnd.crick.clicker",
				"clkk" : "application/vnd.crick.clicker.keyboard",
				"clkp" : "application/vnd.crick.clicker.palette",
				"clkt" : "application/vnd.crick.clicker.template",
				"clkw" : "application/vnd.crick.clicker.wordbank",
				"wbs" : "application/vnd.criticaltools.wbs+xml",
				"pml" : "application/vnd.ctc-posml",
				"ppd" : "application/vnd.cups-ppd",
				"car" : "application/vnd.curl.car",
				"pcurl" : "application/vnd.curl.pcurl",
				"dart" : "application/vnd.dart",
				"rdz" : "application/vnd.data-vision.rdz",
				"uvf" : "application/vnd.dece.data",
				"uvvf" : "application/vnd.dece.data",
				"uvd" : "application/vnd.dece.data",
				"uvvd" : "application/vnd.dece.data",
				"uvt" : "application/vnd.dece.ttml+xml",
				"uvvt" : "application/vnd.dece.ttml+xml",
				"uvx" : "application/vnd.dece.unspecified",
				"uvvx" : "application/vnd.dece.unspecified",
				"uvz" : "application/vnd.dece.zip",
				"uvvz" : "application/vnd.dece.zip",
				"fe_launch" : "application/vnd.denovo.fcselayout-link",
				"dna" : "application/vnd.dna",
				"mlp" : "application/vnd.dolby.mlp",
				"dpg" : "application/vnd.dpgraph",
				"dfac" : "application/vnd.dreamfactory",
				"kpxx" : "application/vnd.ds-keypoint",
				"ait" : "application/vnd.dvb.ait",
				"svc" : "application/vnd.dvb.service",
				"geo" : "application/vnd.dynageo",
				"mag" : "application/vnd.ecowin.chart",
				"nml" : "application/vnd.enliven",
				"esf" : "application/vnd.epson.esf",
				"msf" : "application/vnd.epson.msf",
				"qam" : "application/vnd.epson.quickanime",
				"slt" : "application/vnd.epson.salt",
				"ssf" : "application/vnd.epson.ssf",
				"es3" : "application/vnd.eszigno3+xml",
				"et3" : "application/vnd.eszigno3+xml",
				"ez2" : "application/vnd.ezpix-album",
				"ez3" : "application/vnd.ezpix-package",
				"fdf" : "application/vnd.fdf",
				"mseed" : "application/vnd.fdsn.mseed",
				"seed" : "application/vnd.fdsn.seed",
				"dataless" : "application/vnd.fdsn.seed",
				"gph" : "application/vnd.flographit",
				"ftc" : "application/vnd.fluxtime.clip",
				"fm" : "application/vnd.framemaker",
				"frame" : "application/vnd.framemaker",
				"maker" : "application/vnd.framemaker",
				"book" : "application/vnd.framemaker",
				"fnc" : "application/vnd.frogans.fnc",
				"ltf" : "application/vnd.frogans.ltf",
				"fsc" : "application/vnd.fsc.weblaunch",
				"oas" : "application/vnd.fujitsu.oasys",
				"oa2" : "application/vnd.fujitsu.oasys2",
				"oa3" : "application/vnd.fujitsu.oasys3",
				"fg5" : "application/vnd.fujitsu.oasysgp",
				"bh2" : "application/vnd.fujitsu.oasysprs",
				"ddd" : "application/vnd.fujixerox.ddd",
				"xdw" : "application/vnd.fujixerox.docuworks",
				"xbd" : "application/vnd.fujixerox.docuworks.binder",
				"fzs" : "application/vnd.fuzzysheet",
				"txd" : "application/vnd.genomatix.tuxedo",
				"ggb" : "application/vnd.geogebra.file",
				"ggt" : "application/vnd.geogebra.tool",
				"gex" : "application/vnd.geometry-explorer",
				"gre" : "application/vnd.geometry-explorer",
				"gxt" : "application/vnd.geonext",
				"g2w" : "application/vnd.geoplan",
				"g3w" : "application/vnd.geospace",
				"gmx" : "application/vnd.gmx",
				"kml" : "application/vnd.google-earth.kml+xml",
				"kmz" : "application/vnd.google-earth.kmz",
				"gqf" : "application/vnd.grafeq",
				"gqs" : "application/vnd.grafeq",
				"gac" : "application/vnd.groove-account",
				"ghf" : "application/vnd.groove-help",
				"gim" : "application/vnd.groove-identity-message",
				"grv" : "application/vnd.groove-injector",
				"gtm" : "application/vnd.groove-tool-message",
				"tpl" : "application/vnd.groove-tool-template",
				"vcg" : "application/vnd.groove-vcard",
				"hal" : "application/vnd.hal+xml",
				"zmm" : "application/vnd.handheld-entertainment+xml",
				"hbci" : "application/vnd.hbci",
				"les" : "application/vnd.hhe.lesson-player",
				"hpgl" : "application/vnd.hp-hpgl",
				"hpid" : "application/vnd.hp-hpid",
				"hps" : "application/vnd.hp-hps",
				"jlt" : "application/vnd.hp-jlyt",
				"pcl" : "application/vnd.hp-pcl",
				"pclxl" : "application/vnd.hp-pclxl",
				"sfd-hdstx" : "application/vnd.hydrostatix.sof-data",
				"mpy" : "application/vnd.ibm.minipay",
				"afp" : "application/vnd.ibm.modcap",
				"listafp" : "application/vnd.ibm.modcap",
				"list3820" : "application/vnd.ibm.modcap",
				"irm" : "application/vnd.ibm.rights-management",
				"sc" : "application/vnd.ibm.secure-container",
				"icc" : "application/vnd.iccprofile",
				"icm" : "application/vnd.iccprofile",
				"igl" : "application/vnd.igloader",
				"ivp" : "application/vnd.immervision-ivp",
				"ivu" : "application/vnd.immervision-ivu",
				"igm" : "application/vnd.insors.igm",
				"xpw" : "application/vnd.intercon.formnet",
				"xpx" : "application/vnd.intercon.formnet",
				"i2g" : "application/vnd.intergeo",
				"qbo" : "application/vnd.intu.qbo",
				"qfx" : "application/vnd.intu.qfx",
				"rcprofile" : "application/vnd.ipunplugged.rcprofile",
				"irp" : "application/vnd.irepository.package+xml",
				"xpr" : "application/vnd.is-xpr",
				"fcs" : "application/vnd.isac.fcs",
				"jam" : "application/vnd.jam",
				"rms" : "application/vnd.jcp.javame.midlet-rms",
				"jisp" : "application/vnd.jisp",
				"joda" : "application/vnd.joost.joda-archive",
				"ktz" : "application/vnd.kahootz",
				"ktr" : "application/vnd.kahootz",
				"karbon" : "application/vnd.kde.karbon",
				"chrt" : "application/vnd.kde.kchart",
				"kfo" : "application/vnd.kde.kformula",
				"flw" : "application/vnd.kde.kivio",
				"kon" : "application/vnd.kde.kontour",
				"kpr" : "application/vnd.kde.kpresenter",
				"kpt" : "application/vnd.kde.kpresenter",
				"ksp" : "application/vnd.kde.kspread",
				"kwd" : "application/vnd.kde.kword",
				"kwt" : "application/vnd.kde.kword",
				"htke" : "application/vnd.kenameaapp",
				"kia" : "application/vnd.kidspiration",
				"kne" : "application/vnd.kinar",
				"knp" : "application/vnd.kinar",
				"skp" : "application/vnd.koan",
				"skd" : "application/vnd.koan",
				"skt" : "application/vnd.koan",
				"skm" : "application/vnd.koan",
				"sse" : "application/vnd.kodak-descriptor",
				"lasxml" : "application/vnd.las.las+xml",
				"lbd" : "application/vnd.llamagraphics.life-balance.desktop",
				"lbe" : "application/vnd.llamagraphics.life-balance.exchange+xml",
				"123" : "application/vnd.lotus-1-2-3",
				"apr" : "application/vnd.lotus-approach",
				"pre" : "application/vnd.lotus-freelance",
				"nsf" : "application/vnd.lotus-notes",
				"org" : "application/vnd.lotus-organizer",
				"scm" : "application/vnd.lotus-screencam",
				"lwp" : "application/vnd.lotus-wordpro",
				"portpkg" : "application/vnd.macports.portpkg",
				"mcd" : "application/vnd.mcd",
				"mc1" : "application/vnd.medcalcdata",
				"cdkey" : "application/vnd.mediastation.cdkey",
				"mwf" : "application/vnd.mfer",
				"mfm" : "application/vnd.mfmp",
				"flo" : "application/vnd.micrografx.flo",
				"igx" : "application/vnd.micrografx.igx",
				"mif" : "application/vnd.mif",
				"daf" : "application/vnd.mobius.daf",
				"dis" : "application/vnd.mobius.dis",
				"mbk" : "application/vnd.mobius.mbk",
				"mqy" : "application/vnd.mobius.mqy",
				"msl" : "application/vnd.mobius.msl",
				"plc" : "application/vnd.mobius.plc",
				"txf" : "application/vnd.mobius.txf",
				"mpn" : "application/vnd.mophun.application",
				"mpc" : "application/vnd.mophun.certificate",
				"xul" : "application/vnd.mozilla.xul+xml",
				"cil" : "application/vnd.ms-artgalry",
				"cab" : "application/vnd.ms-cab-compressed",
				"xls" : "application/vnd.ms-excel",
				"xlm" : "application/vnd.ms-excel",
				"xla" : "application/vnd.ms-excel",
				"xlc" : "application/vnd.ms-excel",
				"xlt" : "application/vnd.ms-excel",
				"xlw" : "application/vnd.ms-excel",
				"xlam" : "application/vnd.ms-excel.addin.macroenabled.12",
				"xlsb" : "application/vnd.ms-excel.sheet.binary.macroenabled.12",
				"xlsm" : "application/vnd.ms-excel.sheet.macroenabled.12",
				"xltm" : "application/vnd.ms-excel.template.macroenabled.12",
				"eot" : "application/vnd.ms-fontobject",
				"chm" : "application/vnd.ms-htmlhelp",
				"ims" : "application/vnd.ms-ims",
				"lrm" : "application/vnd.ms-lrm",
				"thmx" : "application/vnd.ms-officetheme",
				"cat" : "application/vnd.ms-pki.seccat",
				"stl" : "application/vnd.ms-pki.stl",
				"ppt" : "application/vnd.ms-powerpoint",
				"pps" : "application/vnd.ms-powerpoint",
				"pot" : "application/vnd.ms-powerpoint",
				"ppam" : "application/vnd.ms-powerpoint.addin.macroenabled.12",
				"pptm" : "application/vnd.ms-powerpoint.presentation.macroenabled.12",
				"sldm" : "application/vnd.ms-powerpoint.slide.macroenabled.12",
				"ppsm" : "application/vnd.ms-powerpoint.slideshow.macroenabled.12",
				"potm" : "application/vnd.ms-powerpoint.template.macroenabled.12",
				"mpp" : "application/vnd.ms-project",
				"mpt" : "application/vnd.ms-project",
				"docm" : "application/vnd.ms-word.document.macroenabled.12",
				"dotm" : "application/vnd.ms-word.template.macroenabled.12",
				"wps" : "application/vnd.ms-works",
				"wks" : "application/vnd.ms-works",
				"wcm" : "application/vnd.ms-works",
				"wdb" : "application/vnd.ms-works",
				"wpl" : "application/vnd.ms-wpl",
				"xps" : "application/vnd.ms-xpsdocument",
				"mseq" : "application/vnd.mseq",
				"mus" : "application/vnd.musician",
				"msty" : "application/vnd.muvee.style",
				"taglet" : "application/vnd.mynfc",
				"nlu" : "application/vnd.neurolanguage.nlu",
				"ntf" : "application/vnd.nitf",
				"nitf" : "application/vnd.nitf",
				"nnd" : "application/vnd.noblenet-directory",
				"nns" : "application/vnd.noblenet-sealer",
				"nnw" : "application/vnd.noblenet-web",
				"ngdat" : "application/vnd.nokia.n-gage.data",
				"n-gage" : "application/vnd.nokia.n-gage.symbian.install",
				"rpst" : "application/vnd.nokia.radio-preset",
				"rpss" : "application/vnd.nokia.radio-presets",
				"edm" : "application/vnd.novadigm.edm",
				"edx" : "application/vnd.novadigm.edx",
				"ext" : "application/vnd.novadigm.ext",
				"odc" : "application/vnd.oasis.opendocument.chart",
				"otc" : "application/vnd.oasis.opendocument.chart-template",
				"odb" : "application/vnd.oasis.opendocument.database",
				"odf" : "application/vnd.oasis.opendocument.formula",
				"odft" : "application/vnd.oasis.opendocument.formula-template",
				"odg" : "application/vnd.oasis.opendocument.graphics",
				"otg" : "application/vnd.oasis.opendocument.graphics-template",
				"odi" : "application/vnd.oasis.opendocument.image",
				"oti" : "application/vnd.oasis.opendocument.image-template",
				"odp" : "application/vnd.oasis.opendocument.presentation",
				"otp" : "application/vnd.oasis.opendocument.presentation-template",
				"ods" : "application/vnd.oasis.opendocument.spreadsheet",
				"ots" : "application/vnd.oasis.opendocument.spreadsheet-template",
				"odt" : "application/vnd.oasis.opendocument.text",
				"odm" : "application/vnd.oasis.opendocument.text-master",
				"ott" : "application/vnd.oasis.opendocument.text-template",
				"oth" : "application/vnd.oasis.opendocument.text-web",
				"xo" : "application/vnd.olpc-sugar",
				"dd2" : "application/vnd.oma.dd2+xml",
				"oxt" : "application/vnd.openofficeorg.extension",
				"pptx" : "application/vnd.openxmlformats-officedocument.presentationml.presentation",
				"sldx" : "application/vnd.openxmlformats-officedocument.presentationml.slide",
				"ppsx" : "application/vnd.openxmlformats-officedocument.presentationml.slideshow",
				"potx" : "application/vnd.openxmlformats-officedocument.presentationml.template",
				"xlsx" : "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
				"xltx" : "application/vnd.openxmlformats-officedocument.spreadsheetml.template",
				"docx" : "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
				"dotx" : "application/vnd.openxmlformats-officedocument.wordprocessingml.template",
				"mgp" : "application/vnd.osgeo.mapguide.package",
				"dp" : "application/vnd.osgi.dp",
				"esa" : "application/vnd.osgi.subsystem",
				"pdb" : "application/vnd.palm",
				"pqa" : "application/vnd.palm",
				"oprc" : "application/vnd.palm",
				"paw" : "application/vnd.pawaafile",
				"str" : "application/vnd.pg.format",
				"ei6" : "application/vnd.pg.osasli",
				"efif" : "application/vnd.picsel",
				"wg" : "application/vnd.pmi.widget",
				"plf" : "application/vnd.pocketlearn",
				"pbd" : "application/vnd.powerbuilder6",
				"box" : "application/vnd.previewsystems.box",
				"mgz" : "application/vnd.proteus.magazine",
				"qps" : "application/vnd.publishare-delta-tree",
				"ptid" : "application/vnd.pvi.ptid1",
				"qxd" : "application/vnd.quark.quarkxpress",
				"qxt" : "application/vnd.quark.quarkxpress",
				"qwd" : "application/vnd.quark.quarkxpress",
				"qwt" : "application/vnd.quark.quarkxpress",
				"qxl" : "application/vnd.quark.quarkxpress",
				"qxb" : "application/vnd.quark.quarkxpress",
				"bed" : "application/vnd.realvnc.bed",
				"mxl" : "application/vnd.recordare.musicxml",
				"musicxml" : "application/vnd.recordare.musicxml+xml",
				"cryptonote" : "application/vnd.rig.cryptonote",
				"cod" : "application/vnd.rim.cod",
				"rm" : "application/vnd.rn-realmedia",
				"rmvb" : "application/vnd.rn-realmedia-vbr",
				"link66" : "application/vnd.route66.link66+xml",
				"st" : "application/vnd.sailingtracker.track",
				"see" : "application/vnd.seemail",
				"sema" : "application/vnd.sema",
				"semd" : "application/vnd.semd",
				"semf" : "application/vnd.semf",
				"ifm" : "application/vnd.shana.informed.formdata",
				"itp" : "application/vnd.shana.informed.formtemplate",
				"iif" : "application/vnd.shana.informed.interchange",
				"ipk" : "application/vnd.shana.informed.package",
				"twd" : "application/vnd.simtech-mindmapper",
				"twds" : "application/vnd.simtech-mindmapper",
				"mmf" : "application/vnd.smaf",
				"teacher" : "application/vnd.smart.teacher",
				"sdkm" : "application/vnd.solent.sdkm+xml",
				"sdkd" : "application/vnd.solent.sdkm+xml",
				"dxp" : "application/vnd.spotfire.dxp",
				"sfs" : "application/vnd.spotfire.sfs",
				"sdc" : "application/vnd.stardivision.calc",
				"sda" : "application/vnd.stardivision.draw",
				"sdd" : "application/vnd.stardivision.impress",
				"smf" : "application/vnd.stardivision.math",
				"sdw" : "application/vnd.stardivision.writer",
				"vor" : "application/vnd.stardivision.writer",
				"sgl" : "application/vnd.stardivision.writer-global",
				"smzip" : "application/vnd.stepmania.package",
				"sm" : "application/vnd.stepmania.stepchart",
				"sxc" : "application/vnd.sun.xml.calc",
				"stc" : "application/vnd.sun.xml.calc.template",
				"sxd" : "application/vnd.sun.xml.draw",
				"std" : "application/vnd.sun.xml.draw.template",
				"sxi" : "application/vnd.sun.xml.impress",
				"sti" : "application/vnd.sun.xml.impress.template",
				"sxm" : "application/vnd.sun.xml.math",
				"sxw" : "application/vnd.sun.xml.writer",
				"sxg" : "application/vnd.sun.xml.writer.global",
				"stw" : "application/vnd.sun.xml.writer.template",
				"sus" : "application/vnd.sus-calendar",
				"susp" : "application/vnd.sus-calendar",
				"svd" : "application/vnd.svd",
				"sis" : "application/vnd.symbian.install",
				"sisx" : "application/vnd.symbian.install",
				"xsm" : "application/vnd.syncml+xml",
				"bdm" : "application/vnd.syncml.dm+wbxml",
				"xdm" : "application/vnd.syncml.dm+xml",
				"tao" : "application/vnd.tao.intent-module-archive",
				"pcap" : "application/vnd.tcpdump.pcap",
				"cap" : "application/vnd.tcpdump.pcap",
				"dmp" : "application/vnd.tcpdump.pcap",
				"tmo" : "application/vnd.tmobile-livetv",
				"tpt" : "application/vnd.trid.tpt",
				"mxs" : "application/vnd.triscape.mxs",
				"tra" : "application/vnd.trueapp",
				"ufd" : "application/vnd.ufdl",
				"ufdl" : "application/vnd.ufdl",
				"utz" : "application/vnd.uiq.theme",
				"umj" : "application/vnd.umajin",
				"unityweb" : "application/vnd.unity",
				"uoml" : "application/vnd.uoml+xml",
				"vcx" : "application/vnd.vcx",
				"vsd" : "application/vnd.visio",
				"vst" : "application/vnd.visio",
				"vss" : "application/vnd.visio",
				"vsw" : "application/vnd.visio",
				"vis" : "application/vnd.visionary",
				"vsf" : "application/vnd.vsf",
				"wbxml" : "application/vnd.wap.wbxml",
				"wmlc" : "application/vnd.wap.wmlc",
				"wmlsc" : "application/vnd.wap.wmlscriptc",
				"wtb" : "application/vnd.webturbo",
				"nbp" : "application/vnd.wolfram.player",
				"wpd" : "application/vnd.wordperfect",
				"wqd" : "application/vnd.wqd",
				"stf" : "application/vnd.wt.stf",
				"xar" : "application/vnd.xara",
				"xfdl" : "application/vnd.xfdl",
				"hvd" : "application/vnd.yamaha.hv-dic",
				"hvs" : "application/vnd.yamaha.hv-script",
				"hvp" : "application/vnd.yamaha.hv-voice",
				"osf" : "application/vnd.yamaha.openscoreformat",
				"osfpvg" : "application/vnd.yamaha.openscoreformat.osfpvg+xml",
				"saf" : "application/vnd.yamaha.smaf-audio",
				"spf" : "application/vnd.yamaha.smaf-phrase",
				"cmp" : "application/vnd.yellowriver-custom-menu",
				"zir" : "application/vnd.zul",
				"zirz" : "application/vnd.zul",
				"zaz" : "application/vnd.zzazz.deck+xml",
				"vxml" : "application/voicexml+xml",
				"wgt" : "application/widget",
				"hlp" : "application/winhlp",
				"wsdl" : "application/wsdl+xml",
				"wspolicy" : "application/wspolicy+xml",
				"7z" : "application/x-7z-compressed",
				"abw" : "application/x-abiword",
				"ace" : "application/x-ace-compressed",
				"dmg" : "application/x-apple-diskimage",
				"aab" : "application/x-authorware-bin",
				"x32" : "application/x-authorware-bin",
				"u32" : "application/x-authorware-bin",
				"vox" : "application/x-authorware-bin",
				"aam" : "application/x-authorware-map",
				"aas" : "application/x-authorware-seg",
				"bcpio" : "application/x-bcpio",
				"torrent" : "application/x-bittorrent",
				"blb" : "application/x-blorb",
				"blorb" : "application/x-blorb",
				"bz" : "application/x-bzip",
				"bz2" : "application/x-bzip2",
				"boz" : "application/x-bzip2",
				"cbr" : "application/x-cbr",
				"cba" : "application/x-cbr",
				"cbt" : "application/x-cbr",
				"cbz" : "application/x-cbr",
				"cb7" : "application/x-cbr",
				"vcd" : "application/x-cdlink",
				"cfs" : "application/x-cfs-compressed",
				"chat" : "application/x-chat",
				"pgn" : "application/x-chess-pgn",
				"nsc" : "application/x-conference",
				"cpio" : "application/x-cpio",
				"csh" : "application/x-csh",
				"deb" : "application/x-debian-package",
				"udeb" : "application/x-debian-package",
				"dgc" : "application/x-dgc-compressed",
				"dir" : "application/x-director",
				"dcr" : "application/x-director",
				"dxr" : "application/x-director",
				"cst" : "application/x-director",
				"cct" : "application/x-director",
				"cxt" : "application/x-director",
				"w3d" : "application/x-director",
				"fgd" : "application/x-director",
				"swa" : "application/x-director",
				"wad" : "application/x-doom",
				"ncx" : "application/x-dtbncx+xml",
				"dtb" : "application/x-dtbook+xml",
				"res" : "application/x-dtbresource+xml",
				"dvi" : "application/x-dvi",
				"evy" : "application/x-envoy",
				"eva" : "application/x-eva",
				"bdf" : "application/x-font-bdf",
				"gsf" : "application/x-font-ghostscript",
				"psf" : "application/x-font-linux-psf",
				"otf" : "application/x-font-otf",
				"pcf" : "application/x-font-pcf",
				"snf" : "application/x-font-snf",
				"ttf" : "application/x-font-ttf",
				"ttc" : "application/x-font-ttf",
				"pfa" : "application/x-font-type1",
				"pfb" : "application/x-font-type1",
				"pfm" : "application/x-font-type1",
				"afm" : "application/x-font-type1",
				"woff" : "application/x-font-woff",
				"arc" : "application/x-freearc",
				"spl" : "application/x-futuresplash",
				"gca" : "application/x-gca-compressed",
				"ulx" : "application/x-glulx",
				"gnumeric" : "application/x-gnumeric",
				"gramps" : "application/x-gramps-xml",
				"gtar" : "application/x-gtar",
				"hdf" : "application/x-hdf",
				"install" : "application/x-install-instructions",
				"iso" : "application/x-iso9660-image",
				"jnlp" : "application/x-java-jnlp-file",
				"latex" : "application/x-latex",
				"lzh" : "application/x-lzh-compressed",
				"lha" : "application/x-lzh-compressed",
				"mie" : "application/x-mie",
				"prc" : "application/x-mobipocket-ebook",
				"mobi" : "application/x-mobipocket-ebook",
				"application" : "application/x-ms-application",
				"lnk" : "application/x-ms-shortcut",
				"wmd" : "application/x-ms-wmd",
				"wmz" : "application/x-ms-wmz",
				"xbap" : "application/x-ms-xbap",
				"mdb" : "application/x-msaccess",
				"obd" : "application/x-msbinder",
				"crd" : "application/x-mscardfile",
				"clp" : "application/x-msclip",
				"exe" : "application/x-msdownload",
				"dll" : "application/x-msdownload",
				"com" : "application/x-msdownload",
				"bat" : "application/x-msdownload",
				"msi" : "application/x-msdownload",
				"mvb" : "application/x-msmediaview",
				"m13" : "application/x-msmediaview",
				"m14" : "application/x-msmediaview",
				"wmf" : "application/x-msmetafile",
				"wmz" : "application/x-msmetafile",
				"emf" : "application/x-msmetafile",
				"emz" : "application/x-msmetafile",
				"mny" : "application/x-msmoney",
				"pub" : "application/x-mspublisher",
				"scd" : "application/x-msschedule",
				"trm" : "application/x-msterminal",
				"wri" : "application/x-mswrite",
				"nc" : "application/x-netcdf",
				"cdf" : "application/x-netcdf",
				"nzb" : "application/x-nzb",
				"p12" : "application/x-pkcs12",
				"pfx" : "application/x-pkcs12",
				"p7b" : "application/x-pkcs7-certificates",
				"spc" : "application/x-pkcs7-certificates",
				"p7r" : "application/x-pkcs7-certreqresp",
				"rar" : "application/x-rar-compressed",
				"ris" : "application/x-research-info-systems",
				"sh" : "application/x-sh",
				"shar" : "application/x-shar",
				"swf" : "application/x-shockwave-flash",
				"xap" : "application/x-silverlight-app",
				"sql" : "application/x-sql",
				"sit" : "application/x-stuffit",
				"sitx" : "application/x-stuffitx",
				"srt" : "application/x-subrip",
				"sv4cpio" : "application/x-sv4cpio",
				"sv4crc" : "application/x-sv4crc",
				"t3" : "application/x-t3vm-image",
				"gam" : "application/x-tads",
				"tar" : "application/x-tar",
				"tcl" : "application/x-tcl",
				"tex" : "application/x-tex",
				"tfm" : "application/x-tex-tfm",
				"texinfo" : "application/x-texinfo",
				"texi" : "application/x-texinfo",
				"obj" : "application/x-tgif",
				"ustar" : "application/x-ustar",
				"src" : "application/x-wais-source",
				"der" : "application/x-x509-ca-cert",
				"crt" : "application/x-x509-ca-cert",
				"fig" : "application/x-xfig",
				"xlf" : "application/x-xliff+xml",
				"xpi" : "application/x-xpinstall",
				"xz" : "application/x-xz",
				"z1" : "application/x-zmachine",
				"z2" : "application/x-zmachine",
				"z3" : "application/x-zmachine",
				"z4" : "application/x-zmachine",
				"z5" : "application/x-zmachine",
				"z6" : "application/x-zmachine",
				"z7" : "application/x-zmachine",
				"z8" : "application/x-zmachine",
				"xaml" : "application/xaml+xml",
				"xdf" : "application/xcap-diff+xml",
				"xenc" : "application/xenc+xml",
				"xhtml" : "application/xhtml+xml",
				"xht" : "application/xhtml+xml",
				"xml" : "application/xml",
				"xsl" : "application/xml",
				"dtd" : "application/xml-dtd",
				"xop" : "application/xop+xml",
				"xpl" : "application/xproc+xml",
				"xslt" : "application/xslt+xml",
				"xspf" : "application/xspf+xml",
				"mxml" : "application/xv+xml",
				"xhvml" : "application/xv+xml",
				"xvml" : "application/xv+xml",
				"xvm" : "application/xv+xml",
				"yang" : "application/yang",
				"yin" : "application/yin+xml",
				"zip" : "application/zip",
				"adp" : "audio/adpcm",
				"au" : "audio/basic",
				"snd" : "audio/basic",
				"mid" : "audio/midi",
				"midi" : "audio/midi",
				"kar" : "audio/midi",
				"rmi" : "audio/midi",
				"mp4a" : "audio/mp4",
				"mpga" : "audio/mpeg",
				"mp2" : "audio/mpeg",
				"mp2a" : "audio/mpeg",
				"mp3" : "audio/mpeg",
				"m2a" : "audio/mpeg",
				"m3a" : "audio/mpeg",
				"oga" : "audio/ogg",
				"ogg" : "audio/ogg",
				"spx" : "audio/ogg",
				"s3m" : "audio/s3m",
				"sil" : "audio/silk",
				"uva" : "audio/vnd.dece.audio",
				"uvva" : "audio/vnd.dece.audio",
				"eol" : "audio/vnd.digital-winds",
				"dra" : "audio/vnd.dra",
				"dts" : "audio/vnd.dts",
				"dtshd" : "audio/vnd.dts.hd",
				"lvp" : "audio/vnd.lucent.voice",
				"pya" : "audio/vnd.ms-playready.media.pya",
				"ecelp4800" : "audio/vnd.nuera.ecelp4800",
				"ecelp7470" : "audio/vnd.nuera.ecelp7470",
				"ecelp9600" : "audio/vnd.nuera.ecelp9600",
				"rip" : "audio/vnd.rip",
				"weba" : "audio/webm",
				"aac" : "audio/x-aac",
				"aif" : "audio/x-aiff",
				"aiff" : "audio/x-aiff",
				"aifc" : "audio/x-aiff",
				"caf" : "audio/x-caf",
				"flac" : "audio/x-flac",
				"mka" : "audio/x-matroska",
				"m3u" : "audio/x-mpegurl",
				"wax" : "audio/x-ms-wax",
				"wma" : "audio/x-ms-wma",
				"ram" : "audio/x-pn-realaudio",
				"ra" : "audio/x-pn-realaudio",
				"rmp" : "audio/x-pn-realaudio-plugin",
				"wav" : "audio/x-wav",
				"xm" : "audio/xm",
				"cdx" : "chemical/x-cdx",
				"cif" : "chemical/x-cif",
				"cmdf" : "chemical/x-cmdf",
				"cml" : "chemical/x-cml",
				"csml" : "chemical/x-csml",
				"xyz" : "chemical/x-xyz",
				"bmp" : "image/bmp",
				"cgm" : "image/cgm",
				"g3" : "image/g3fax",
				"gif" : "image/gif",
				"ief" : "image/ief",
				"jpeg" : "image/jpeg",
				"jpg" : "image/jpeg",
				"jpe" : "image/jpeg",
				"ktx" : "image/ktx",
				"png" : "image/png",
				"btif" : "image/prs.btif",
				"sgi" : "image/sgi",
				"svg" : "image/svg+xml",
				"svgz" : "image/svg+xml",
				"tiff" : "image/tiff",
				"tif" : "image/tiff",
				"psd" : "image/vnd.adobe.photoshop",
				"uvi" : "image/vnd.dece.graphic",
				"uvvi" : "image/vnd.dece.graphic",
				"uvg" : "image/vnd.dece.graphic",
				"uvvg" : "image/vnd.dece.graphic",
				"sub" : "image/vnd.dvb.subtitle",
				"djvu" : "image/vnd.djvu",
				"djv" : "image/vnd.djvu",
				"dwg" : "image/vnd.dwg",
				"dxf" : "image/vnd.dxf",
				"fbs" : "image/vnd.fastbidsheet",
				"fpx" : "image/vnd.fpx",
				"fst" : "image/vnd.fst",
				"mmr" : "image/vnd.fujixerox.edmics-mmr",
				"rlc" : "image/vnd.fujixerox.edmics-rlc",
				"mdi" : "image/vnd.ms-modi",
				"wdp" : "image/vnd.ms-photo",
				"npx" : "image/vnd.net-fpx",
				"wbmp" : "image/vnd.wap.wbmp",
				"xif" : "image/vnd.xiff",
				"webp" : "image/webp",
				"3ds" : "image/x-3ds",
				"ras" : "image/x-cmu-raster",
				"cmx" : "image/x-cmx",
				"fh" : "image/x-freehand",
				"fhc" : "image/x-freehand",
				"fh4" : "image/x-freehand",
				"fh5" : "image/x-freehand",
				"fh7" : "image/x-freehand",
				"ico" : "image/x-icon",
				"sid" : "image/x-mrsid-image",
				"pcx" : "image/x-pcx",
				"pic" : "image/x-pict",
				"pct" : "image/x-pict",
				"pnm" : "image/x-portable-anymap",
				"pbm" : "image/x-portable-bitmap",
				"pgm" : "image/x-portable-graymap",
				"ppm" : "image/x-portable-pixmap",
				"rgb" : "image/x-rgb",
				"tga" : "image/x-tga",
				"xbm" : "image/x-xbitmap",
				"xpm" : "image/x-xpixmap",
				"xwd" : "image/x-xwindowdump",
				"eml" : "message/rfc822",
				"mime" : "message/rfc822",
				"igs" : "model/iges",
				"iges" : "model/iges",
				"msh" : "model/mesh",
				"mesh" : "model/mesh",
				"silo" : "model/mesh",
				"dae" : "model/vnd.collada+xml",
				"dwf" : "model/vnd.dwf",
				"gdl" : "model/vnd.gdl",
				"gtw" : "model/vnd.gtw",
				"mts" : "model/vnd.mts",
				"vtu" : "model/vnd.vtu",
				"wrl" : "model/vrml",
				"vrml" : "model/vrml",
				"x3db" : "model/x3d+binary",
				"x3dbz" : "model/x3d+binary",
				"x3dv" : "model/x3d+vrml",
				"x3dvz" : "model/x3d+vrml",
				"x3d" : "model/x3d+xml",
				"x3dz" : "model/x3d+xml",
				"appcache" : "text/cache-manifest",
				"ics" : "text/calendar",
				"ifb" : "text/calendar",
				"css" : "text/css",
				"csv" : "text/csv",
				"html" : "text/html",
				"htm" : "text/html",
				"n3" : "text/n3",
				"txt" : "text/plain",
				"text" : "text/plain",
				"conf" : "text/plain",
				"def" : "text/plain",
				"list" : "text/plain",
				"log" : "text/plain",
				"in" : "text/plain",
				"dsc" : "text/prs.lines.tag",
				"rtx" : "text/richtext",
				"sgml" : "text/sgml",
				"sgm" : "text/sgml",
				"tsv" : "text/tab-separated-values",
				"t" : "text/troff",
				"tr" : "text/troff",
				"roff" : "text/troff",
				"man" : "text/troff",
				"me" : "text/troff",
				"ms" : "text/troff",
				"ttl" : "text/turtle",
				"uri" : "text/uri-list",
				"uris" : "text/uri-list",
				"urls" : "text/uri-list",
				"vcard" : "text/vcard",
				"curl" : "text/vnd.curl",
				"dcurl" : "text/vnd.curl.dcurl",
				"scurl" : "text/vnd.curl.scurl",
				"mcurl" : "text/vnd.curl.mcurl",
				"sub" : "text/vnd.dvb.subtitle",
				"fly" : "text/vnd.fly",
				"flx" : "text/vnd.fmi.flexstor",
				"gv" : "text/vnd.graphviz",
				"3dml" : "text/vnd.in3d.3dml",
				"spot" : "text/vnd.in3d.spot",
				"jad" : "text/vnd.sun.j2me.app-descriptor",
				"wml" : "text/vnd.wap.wml",
				"wmls" : "text/vnd.wap.wmlscript",
				"s" : "text/x-asm",
				"asm" : "text/x-asm",
				"c" : "text/x-c",
				"cc" : "text/x-c",
				"cxx" : "text/x-c",
				"cpp" : "text/x-c",
				"h" : "text/x-c",
				"hh" : "text/x-c",
				"dic" : "text/x-c",
				"f" : "text/x-fortran",
				"for" : "text/x-fortran",
				"f77" : "text/x-fortran",
				"f90" : "text/x-fortran",
				"java" : "text/x-java-source",
				"opml" : "text/x-opml",
				"p" : "text/x-pascal",
				"pas" : "text/x-pascal",
				"nfo" : "text/x-nfo",
				"etx" : "text/x-setext",
				"sfv" : "text/x-sfv",
				"uu" : "text/x-uuencode",
				"vcs" : "text/x-vcalendar",
				"vcf" : "text/x-vcard",
				"3gp" : "video/3gpp",
				"3g2" : "video/3gpp2",
				"h261" : "video/h261",
				"h263" : "video/h263",
				"h264" : "video/h264",
				"jpgv" : "video/jpeg",
				"jpm" : "video/jpm",
				"jpgm" : "video/jpm",
				"mj2" : "video/mj2",
				"mjp2" : "video/mj2",
				"mp4" : "video/mp4",
				"mp4v" : "video/mp4",
				"mpg4" : "video/mp4",
				"mpeg" : "video/mpeg",
				"mpg" : "video/mpeg",
				"mpe" : "video/mpeg",
				"m1v" : "video/mpeg",
				"m2v" : "video/mpeg",
				"ogv" : "video/ogg",
				"qt" : "video/quicktime",
				"mov" : "video/quicktime",
				"uvh" : "video/vnd.dece.hd",
				"uvvh" : "video/vnd.dece.hd",
				"uvm" : "video/vnd.dece.mobile",
				"uvvm" : "video/vnd.dece.mobile",
				"uvp" : "video/vnd.dece.pd",
				"uvvp" : "video/vnd.dece.pd",
				"uvs" : "video/vnd.dece.sd",
				"uvvs" : "video/vnd.dece.sd",
				"uvv" : "video/vnd.dece.video",
				"uvvv" : "video/vnd.dece.video",
				"dvb" : "video/vnd.dvb.file",
				"fvt" : "video/vnd.fvt",
				"mxu" : "video/vnd.mpegurl",
				"m4u" : "video/vnd.mpegurl",
				"pyv" : "video/vnd.ms-playready.media.pyv",
				"uvu" : "video/vnd.uvvu.mp4",
				"uvvu" : "video/vnd.uvvu.mp4",
				"viv" : "video/vnd.vivo",
				"webm" : "video/webm",
				"f4v" : "video/x-f4v",
				"fli" : "video/x-fli",
				"flv" : "video/x-flv",
				"m4v" : "video/x-m4v",
				"mkv" : "video/x-matroska",
				"mk3d" : "video/x-matroska",
				"mks" : "video/x-matroska",
				"mng" : "video/x-mng",
				"asf" : "video/x-ms-asf",
				"asx" : "video/x-ms-asf",
				"vob" : "video/x-ms-vob",
				"wm" : "video/x-ms-wm",
				"wmv" : "video/x-ms-wmv",
				"wmx" : "video/x-ms-wmx",
				"wvx" : "video/x-ms-wvx",
				"avi" : "video/x-msvideo",
				"movie" : "video/x-sgi-movie",
				"smv" : "video/x-smv",
				"ice" : "x-conference/x-cooltalk",
				"php" : "application/php",
				"con" : "application/concept",
				"csp" : "application/concept"
			};

function ConceptClient(url, container, loading, absolute_paths, debug) {
	this.Connected = false;

	if (absolute_paths)
		this.ResourceRoot = res_prefix + "/resources";
	else
		this.ResourceRoot = "resources";

	this.Container = document.getElementById(container);
	this.NoParentContainer = document.createElement("div");
	this.NoParentContainer.id = "NoParent_"+container;
	this.NoParentContainer.style.display = "none";
	this.Container.appendChild(this.NoParentContainer);
	this.URL = url;
	this.ChildApp = null;
	if (loading)
		this.LoadingContainer = document.getElementById(loading);

	this.Alerts = document.createElement("div");
	this.Alerts.className = "alert alert-warning alert-dismissible RNotify";
	this.Alerts.setAttribute("role", "alert");
	this.Alerts.innerHTML = '<button type="button" class="close" data-dismiss="alert"><span aria-hidden="true">&times;</span><span class="sr-only">Close</span></button>';
	this.Alerts.style.display = "none";
	this.AlertsLabel = document.createElement("div");
	this.Alerts.appendChild(this.AlertsLabel);

	this.Container.appendChild(this.Alerts);

	this.Controls = new Array();
	this.TreeData = new Array();
	this.POST_STRING = "";
	this.POST_TARGET = "";
	this.POST_STRING_BUF = 0;
	this.POST_OBJECT = 0;
	this.UPLOADED_FILE = [ ];
	this.MainForm = 0;
	this.HTMLUIID = 0;
	this.PendingCode = { };
	this.POST_FILENAME = "";
	this.Cookies = { };
	this.AudioObjects = { };
	this.MasterAudioContext = null;
	this.Ring = null;
	this.isIE = false;
	this.isFF = false;
	this.isEDGE = false;
	this.CompensatedMessages = [ ];
	this.MediaListeners = [ ];
	this.MediaAudio = false;
	this.MediaVideo = false;
	this.MediaObject = null;
	this.MediaWorkers = [ ];
	this.EmulatedMessage = { };
	this.StartTime = new Date().getTime();
	this.FPrefix = "D_" + container + "_";
	this.zIndexIndex = 100;
	this.InAudioContext = null;
	this.LastAudioLevel = 0;
	this.LastAudioLevel_tail = 0;
	this.Debug = debug;
	this.ReconnectCount = 0;
	this.CleanClose = true;

	if (navigator.userAgent.toLowerCase().indexOf("trident") != -1)
		this.isIE = true;
	else
	if (navigator.userAgent.toLowerCase().indexOf("firefox") != -1)
		this.isFF = true;
	else
	if (navigator.userAgent.toLowerCase().indexOf("edge") != -1)
		this.isEDGE = true;

	this.PendingUpdate = null;
	window[this.FPrefix + "IsClient"] = function() { return 1; };
	window.IsClient = window[this.FPrefix + "IsClient"];

	var self = this;

	if (url.substring(0, 8).toLowerCase() == "concept:")
		url = "ws:"+url.substring(8);
	else
	if (url.substring(0, 9).toLowerCase() == "concepts:")
		url = "wss:"+url.substring(9);

	this.URL = url;

	this.CreateSocket = function(url, debug, restore) {
		if (debug)
			this.ConceptSocket = new WebSocket(url, debug);
		else
		if (restore)
			this.ConceptSocket = new WebSocket(url, "restore");
		else
			this.ConceptSocket = new WebSocket(url);

		this.ConceptSocket.binaryType = "arraybuffer";
		this.ConceptSocket.onopen = function() {
			self.Connected = true;
			if (self.ReconnectCount > 0) {
				self.ReconnectCount = 0;
				console.log("Session restored");
				self.Container.disabled = false;
				self.Alerts.style.display = "none";
				if (self.UIRun)
					self.UIRun(2, "");
			}
		};

		this.ConceptSocket.onerror = function(evt) {
			if (!self.CleanClose)
				return;

			if (self.Container)
				self.Container.style.display = "none";

			if (self.Connected) {
				if (self.LoadingContainer)
					self.LoadingContainer.style.display = "block";

				self.NotifyLoading("[application closed]");
				if (self.UIRun)
					self.UIRun(0, "");
			} else {
				if ((evt) && (evt.data)) {
					self.NotifyLoading("error connecting to the server [" + evt.data + "]");
					if (self.UIRun)
						self.UIRun(-1, evt.data);
				} else {
					self.NotifyLoading("error connecting to the server");
					if (self.UIRun)
						self.UIRun(-1, "");
				}
			}
			self.Connected = false;
			self.MainForm = null;
			if (self.MediaObject) {
				self.MediaObject.stop();
				delete self.MediaObject;
			}
			self.TerminateWorkers();
		};

		this.ConceptSocket.onclose = function(evt) {
			if (!self.CleanClose) {
				if (self.ReconnectCount <= 10) {
					self.ReconnectWithTimeout(1000);
					return;
				}
			}
			if (self.Connected) {
				if (self.Container)
					self.Container.style.display = "none";
				if (self.LoadingContainer)
					self.LoadingContainer.style.display = "block";

				self.NotifyLoading("[application closed]");
				if (self.UIRun)
					self.UIRun(0, "");
			}
			self.MainForm = null;
		};

		this.ConceptSocket.onmessage = function(evt) {
			if (evt.data instanceof ArrayBuffer) {
				self.ParseResponse(evt.data);
			} else {
				var fileReader = new FileReader();
				fileReader.onload = function() {
					self.ParseResponse(this.result);
				};
				fileReader.readAsArrayBuffer(evt.data);
			}
		};
	};

	this.Reconnect = function() {
		if ((this.URL) && (!this.Debug)) {
			this.Container.disabled = true;
			this.ReconnectCount++;
			console.log("Reconnect attempt #" + this.ReconnectCount);
			this.AlertsLabel.innerHTML = "&#x1f4e1; Connection lost. Reconnecting (#" + this.ReconnectCount + ")";
			this.Alerts.style.display = "block";
			this.CreateSocket(url, this.Debug, true);
			if (this.UIRun)
				this.UIRun(3, this.ReconnectCount);
		}
	};

	this.ReconnectWithTimeout = function(timeout) {
		setTimeout(function() {
			self.Reconnect();
		}, timeout);
	};

	this.CreateSocket(url, debug, false);

	window.addEventListener("resize", function() {
		self.UpdateScrolledWindowSize(1);
	});

	window.addEventListener("beforeunload", function (e) {
		if ((self.MainForm) && (self.MainForm.ConceptDelete)) {
			self.SendMessage(self.MainForm.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_DELETEEVENT, "", 0);
			var confirmationMessage = "This application needs your attention before closing it.";
			(e || window.event).returnValue = confirmationMessage;
			return confirmationMessage;
		}
		if ((self.MainForm) && (self.MainForm.ConceptUnrealize))
			self.SendMessage(self.MainForm.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_UNREALIZE, "", 0);
	});

	this.TerminateWorkers = function() {
		for (var i = 0; i < this.MediaWorkers.length; i++) {
			var w = this.MediaWorkers[i];
			if (w) {
				try {
					w.terminate();
				} catch (e) {
					// nothing
				}
			}
		}
		this.MediaWorkers = [ ];
		if (this.MediaTimer) {
			clearTimeout(this.MediaTimer);
			delete this.MediaTimer;
		}
		for (var i = 0; i < this.Controls.length; i++) {
			var element = this.Controls[i];
			try {
				if ((element.ConceptProcessor) && (element.destination)) {
					console.log("Closing audio");
					element.ConceptProcessor.disconnect(element.destination);
					if (element.ConceptMicrophone)
						element.ConceptMicrophone.disconnect(element.ConceptProcessor);
				}
			} catch (e) {
			}
		}
	}

	this.FindLimit = function(control) {
		var parent = control.getAttribute("ConceptParentID");
		var next = null;
		var p_prev = control;
		if (parent) {
			var p_control = this.Controls[parseInt(parent)];
			while ((p_control) && (p_control.offsetParent)) {
				if ((p_control.ConceptClassID == CLASS_VBOX) || (p_control.ConceptClassID == CLASS_VBUTTONBOX) || (p_control.ConceptClassID == CLASS_VPANED)) {
					next = p_prev;
					do {
						next = next.nextSibling;
					} while ((next) && (!next.offsetParent));
					if (next)
						break;
				}
				parent = p_control.getAttribute("ConceptParentID");
				p_prev = p_control;
				if (parent)
					p_control = this.Controls[parseInt(parent)];
				else
					p_control = null;
			}
		}
		return next;
	}

	this.IsInChildForm = function(control) {
		var parent = control.getAttribute("ConceptParentID");
		if (parent) {
			var p_control = this.Controls[parseInt(parent)];
			while ((p_control) && (p_control.offsetParent)) {
				if (p_control.ConceptClassID == CLASS_FORM) {
					if (p_control.getAttribute("ConceptParentID"))
						return true;
					return false;
				}
				parent = p_control.getAttribute("ConceptParentID");
				if (parent)
					p_control = this.Controls[parseInt(parent)];
				else
					p_control = null;
			}
		}
		return false;
	}

	this.FindHorizontalLimit = function(control) {
		var parent = control.getAttribute("ConceptParentID");
		var next = null;
		var p_prev = control;
		if (parent) {
			var p_control = this.Controls[parseInt(parent)];
			while ((p_control) && (p_control.offsetParent)) {
				if ((p_control.ConceptClassID == CLASS_HBOX) || (p_control.ConceptClassID == CLASS_HBUTTONBOX) || (p_control.ConceptClassID == CLASS_HPANED)) {
					next = p_prev;
					do {
						next = next.nextSibling;
					} while ((next) && (!next.offsetParent));

					if (next)
						break;
				}
				parent = p_control.getAttribute("ConceptParentID");
				p_prev = p_control;
				if (parent)
					p_control = this.Controls[parseInt(parent)];
				else
					p_control = null;
			}
		}
		return next;
	}

	this.FindTabDelta = function(control) {
		var parent = control.getAttribute("ConceptParentID");
		var delta = 0;
		var p_prev = control;
		if (parent) {
			var p_control = this.Controls[parseInt(parent)];
			while ((p_control) && (p_control.offsetParent)) {
				if ((p_control.ConceptClassID == CLASS_NOTEBOOK) && (p_control.className.lastIndexOf('tabs-below') >= 0)) {
					var tabs = document.getElementById(p_control.id.replace("r", "t"));
					if ((tabs) && (tabs.offsetParent) && (tabs.offsetHeight))
						delta += tabs.offsetHeight;
				}
				parent = p_control.getAttribute("ConceptParentID");
				p_prev = p_control;
				if (parent)
					p_control = this.Controls[parseInt(parent)];
				else
					p_control = null;
			}
		}
		return delta;
	}

	this.UpdateScrolledWindowSize = function(timeout) {
		// application doesn't run yet
		if (!this.MainForm)
			return;
		if (!timeout)
			timeout = 1;

		if (this.PendingUpdate) {
			clearTimeout(this.PendingUpdate);
			this.PendingUpdate = null;
		}
		
		this.PendingUpdate = setTimeout(function () {
			self.UpdateScrolledWindowSizeCB();	
		}, timeout);
	}

	this.UpdateLabelMarkup = function(OwnerID, node, msg) {
		if (node) {
			if (node.nodeName.toLowerCase() == "a") {
				node.onclick = function() {
					self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + msg, this.href, 0);
					return false;
				};
			}
			for (var i = 0; i < node.children.length; i++) {
				var child = node.children[i];
				if (child)
					this.UpdateLabelMarkup(OwnerID, child, msg);
			}
		}
	}

	this.ParseUnderline = function(str) {
		var result = "";
		var len = str.length;
		var u_prec = false;
		for (var i = 0; i < len; i++) {
			var s = str.charAt(i);
			if ((s != "_") || (u_prec)) {
				result += s;
				u_prec = false;
			} else {
				u_prec = true;
			}	
		}
		return result;
	}

	this.UpdateIE = function(orig_control) {
		var child = orig_control.children[0];
		if ((child) && (child.ConceptClassID == CLASS_TREEVIEW)) {
			if (orig_control.IETimer)
				clearTimeout(orig_control.IETimer);

			var limit = self.FindHorizontalLimit(orig_control);
			if (limit) {
				var lid = limit.id;
				orig_control.IETimer = setTimeout(function () {
					orig_control.style.width = "0px";
				}, 100);
				setTimeout(function () {
					orig_control.style.width = "" + Math.floor(document.getElementById(lid).getBoundingClientRect().left - orig_control.getBoundingClientRect().left) + "px";
				}, 110);
			} else {
				orig_control.IETimer = setTimeout(function () {
					orig_control.style.width = "0px";
				}, 100);

				setTimeout(function () {
					orig_control.style.width = "" + Math.floor(self.Container.offsetWidth - orig_control.getBoundingClientRect().left) + "px";
				}, 110);
			}
		}
	}

	this.UpdateControl = function(control, AlreadyUpdated) {
		if ((control) && (control.offsetParent) && ((typeof control.ConceptHeight == "undefined") || (control.ConceptHeight < 0))) {
			var padding = 0;
			if ((control.ConceptClassID == CLASS_SCROLLEDWINDOW) || (control.ConceptClassID == 1000) || (control.ConceptClassID == 1012) || (control.ConceptClassID == CLASS_PROPERTIESBOX)) {
				var parent = control.getAttribute("ConceptParentID");
				var orig_control = control;
				if (parent) {
					var p_control = this.Controls[parseInt(parent)];
					var orig_p_control = p_control;
					// problem on IE
					while (p_control) {
						switch (p_control.ConceptClassID) {
							case CLASS_SCROLLEDWINDOW:
								p_control = null;
								break;
							case CLASS_FORM:
								if (AlreadyUpdated[parent]) {
									p_control = null
									break;
								}
								AlreadyUpdated[parent] = true;
								
								if (p_control.getAttribute("ConceptParentID")) {
									//var c_control = document.getElementById("c" + p_control.id.substring(1));
									//if (!c_control)
									//	c_control = p_control;
									if ((typeof control.ConceptHeight == "undefined") || (control.ConceptHeight < 0))
										control.style.height = "" + (p_control.offsetHeight - 200) + "px";
									else
										control.style.maxHeight = "" + (p_control.offsetHeight - 160) + "px";
								} else {
									control.style.height = "" + this.Container.offsetHeight + "px";
									if ((orig_control.style.flexGrow) || (orig_control.style.webkitFlexGrow)) {
										if ((orig_p_control) && (orig_p_control.ConceptClassID != CLASS_HBOX) && (orig_p_control.ConceptClassID != CLASS_HBUTTONBOX)) {
											orig_control.style.webkitFlexGrow = 0;
											orig_control.style.flexGrow = 0;
										}
									}
									orig_control.style.height = "" + (this.Container.offsetHeight - this.GetAbsolutePosition(orig_control).top) + "px";
								}
								p_control = null;
								break;
							case CLASS_NOTEBOOK:
								if ((p_control.offsetHeight) && ((typeof orig_control.ConceptHeight == "undefined") || (orig_control.ConceptHeight < 0))) {
									if (AlreadyUpdated[parent]) {
										p_control = null
										break;
									}

									AlreadyUpdated[parent] = true;
									var t_delta = 0;

									if ((p_control.className.lastIndexOf('tabs-left') == -1) && (p_control.className.lastIndexOf('tabs-right') == -1)) {
										var tabs = document.getElementById("t" + p_control.id.replace("r", ""));
										if ((tabs) && (tabs.offsetHeight) && (tabs.offsetParent))
											t_delta = tabs.offsetHeight;
									}
									var limit = this.FindLimit(p_control);
									var height;
									if (limit) {
										height = "" + (limit.getBoundingClientRect().top - p_control.getBoundingClientRect().top - t_delta - padding) + "px";
									} else {
										var delta = this.FindTabDelta(control);
										var pages = document.getElementById("c" + p_control.id.replace("r", ""));
										if (this.IsInChildForm(p_control))
											delta += 80;
										height = "" + (this.Container.offsetHeight - control.getBoundingClientRect().top - delta) + "px";
									}

									if (height != control.style.height) {
										if ((typeof control.ConceptHeight == "undefined") || (control.ConceptHeight < 0))
											control.style.height = height;
										else
											control.style.maxHeight = height;
									}

									if ((this.isIE) && (orig_control.children[0]) && ((typeof orig_control.ConceptWidth == "undefined") || (orig_control.ConceptWidth < 0)))
										this.UpdateIE(orig_control);
								}
								p_control = null;
								break;
							case CLASS_ALIGNMENT:
							case CLASS_TABLE:
							default:
								var cstyle = window.getComputedStyle(p_control, null);
								var p_top = cstyle.getPropertyValue('padding-top');
								var p_bottom = cstyle.getPropertyValue('padding-bottom');
								if (p_top)
									padding += parseInt(p_top.replace("px", ""));
								if (p_bottom)
									padding += parseInt(p_bottom.replace("px", ""));

								if (p_control.ConceptClassID)
									control = p_control;
								parent = p_control.getAttribute("ConceptParentID");
								if (parent)
									p_control = this.Controls[parseInt(parent)];
								else
									p_control = null;
						}
					}
				}
			} else
			if ((control.ConceptClassID == CLASS_COMBOBOXTEXT) || (control.ConceptClassID == CLASS_COMBOBOX)) {
				var parent = control.getAttribute("ConceptParentID");
				//var p_control = this.Controls[parseInt(parent)];
				var c_top = control.getBoundingClientRect().top;
				var p_limit = this.Container.getBoundingClientRect().height;
				if (c_top + 200 >= p_limit)
					control.classList.add("dropup");
				else
					control.classList.remove("dropup");
			}
		}
	}

	this.UpdateScrolledWindowSizeCB = function() {
		// application doesn't run yet
		if (!this.MainForm)
			return;

		//var delta_container = this.Container.offsetTop + this.Container.offsetHeight;
		var len = this.Controls.length;
		var AlreadyUpdated = [ ];

		for (var i = len - 1; i >= 0; i--) {
			var control = this.Controls[i];
			this.UpdateControl(control, AlreadyUpdated);
		}

	}

	this.ParseResponse = function(result) {
			var str = new Uint8Array(/*this.*/result);
			var sender_len = str[0];
			var index = 1;

			var sender = ToUTF8(str, sender_len, index);
			index += sender_len;

			var msg_id = str[index++] << 24 & 0xFF000000;
			msg_id += str[index++] << 16 & 0x00FF0000;
			msg_id += str[index++] << 8 & 0x0000FF00;
			msg_id += str[index++] & 0x000000FF;

			var target_len = str[index++] << 8 & 0xFF00;
			target_len += str[index++] & 0x00FF;

			var target = ToUTF8(str, target_len, index);
			index += target_len;
	
			var val_index = index;

			var val = "";
			var len = str.byteLength-index;
			switch (msg_id) {
				case MSG_SAVE_FILE:
				case MSG_SAVE_FILE2:
				case MSG_SEND_COOKIE:
				case MSG_SEND_ZCOOKIE:
					break;
				case MSG_CHUNK:
					if (sender.toLowerCase() == "send")
						break;
				default:
					val = ToUTF8(str, len, index);
			}
			if ((msg_id == MSG_SET_PROPERTY) && (!target_len))
				target = "" + P_ADDBUFFER

			if ((self.CompensatedMessages.length) && (msg_id == MSG_GET_PROPERTY)) {
				var obj = self.CompensatedMessages[0];
				if ((obj.MSG_ID == msg_id) && (obj.Sender == sender) && (obj.Target == target)) {
					self.CompensatedMessages.pop();
					self.SendMessage(obj.Sender, obj.MSG_ID, obj.Target, obj.Value, 0);
					return;
				}
			}
			self.CONCEPT_CALLBACK(sender, msg_id, target, val, /*this.*/result.slice(val_index), self.SendMessage);
	}

	this.SendMessage = function(sender, msg_id, target, val, is_buffer) {
		if (!self.Connected) {
			console.warn('ConceptClient is not connected');
			return;
		} else
		if (self.ReconnectCount > 0) {
			console.warn('ConceptClient is trying to reconnect ...');
			return;
		}
		var size = sender.length + target.length + 7;
		var max = 0;
		if (is_buffer >= 2) {
			for (var i = 0; i < val.length; i++) {
				var v = val[i];
				if (v.length > max)
					max = v.length;
			}
			if (is_buffer == 2)
				size += val.length * max * 2;
			else
				size += val.length * max;
		} else
			size += val.length;

		var arr = new ArrayBuffer(size);
		var str = new DataView(arr);
		var len = sender.length;
		str.setUint8(0, len);

		var index = 1;

		for (var i = 0; i < len; i++)
			str.setInt8(index++, sender.charCodeAt(i));

		str.setInt8(index++, msg_id >> 24 & 0xFF);
		str.setInt8(index++, msg_id >> 16 & 0xFF);
		str.setInt8(index++, msg_id >> 8 & 0xFF);
		str.setInt8(index++, msg_id & 0xFF);

		var t_len = target.length;
		str.setUint8(index++, t_len >> 8 & 0xFF);
		str.setUint8(index++, t_len & 0xFF);

		for (var i = 0;i < t_len;i++)
			str.setUint8(index++, target.charCodeAt(i));

		var v_len = val.length;
		if (is_buffer) {
			if (is_buffer == 1) {
				for (var i = 0; i<v_len; i++)
					str.setUint8(index++, val[i]);
			} else
			if (is_buffer == 2) {
				// convert to PCM short

				for (var j = 0; j < max; j++) {
					for (var i = 0; i < v_len; i++) {
						var val2 = val[i];

						var v = Math.floor(val2[j] * 32768);
						// signed big endian to little endian
						str.setUint16(index, ((v & 0xFF) << 8) | ((v >> 8) & 0xFF));

						index += 2;
					}
				}
			} else
			if (is_buffer == 3) {
				for (var j = 0; j < max; j++) {
					for (var i = 0; i < v_len; i++) {
						var val2 = val[i];
						str.setUint8(index++, val2[j]);
					}
				}	
			} else {
				for (var i = 0; i<v_len; i++)
					str.setUint8(index++, val.getUint8(i));
			}
		} else {
			for (var i = 0; i<v_len; i++)
				str.setUint8(index++, val.charCodeAt(i));
		}
		self.ConceptSocket.send(arr);
	}

	this.NotifyLoading = function(Value) {
		if ((this.LoadingContainer) && (this.LoadingContainer.offsetParent)) {
			this.LoadingContainer.innerHTML = "";
			this.LoadingContainer.appendChild(document.createTextNode(Value));
		}
	}

	this.CONCEPT_CALLBACK = function(Sender, MSG_ID, Target, Value, ValueBuffer, SendMessageFunction) {
		switch (MSG_ID) {
			case MSG_RAW_OUTPUT:
				console.log(Value);
				this.NotifyLoading(Value);
				break;
			case MSG_CREATE:
				this.CreateControl(Sender, parseInt(Target), Value);
				break;
			case MSG_SET_PROPERTY:
				this.SetProperty(parseInt(Sender), parseInt(Target), Value, ValueBuffer);
				break;
			case MSG_GET_PROPERTY:
				this.GetProperty(parseInt(Sender), parseInt(Target), Value, ValueBuffer, SendMessageFunction);
				break;
			case MSG_SET_EVENT:
				this.SetEvent(parseInt(Sender), parseInt(Target), Value);
				break;
			case MSG_POST_STRING:
				if (this.POST_FILENAME != "") {
					var re = /(?:\.([^.]+))?$/;
					var ext_p = re.exec(this.POST_FILENAME)[1];
					var blob_type = "application/binary";
					if (ext_p) {
						var ext = ext_p.toLowerCase(); 
						if ((ext) && (mimeTypes[ext]))
							blob_type = mimeTypes[ext];
					}
					saveAs(new Blob([ValueBuffer], {type: blob_type}), this.POST_FILENAME);
					this.POST_FILENAME = "";
				} else {
					this.POST_STRING = Value;
					this.POST_TARGET = Target;
					this.POST_STRING_BUF = ValueBuffer;
				}
				break;
			case MSG_POST_OBJECT:
				this.POST_OBJECT = parseInt(Sender);
				break;
			case MSG_PUT_STREAM:
				this.PutStream(parseInt(Sender), Target, Value, ValueBuffer);
				break;
			case MSG_PUT_SECONDARY_STREAM:
				this.PutSecondaryStream(parseInt(Sender), Target, Value, ValueBuffer);
				break;
			case MSG_CUSTOM_MESSAGE1:
			case MSG_CUSTOM_MESSAGE2:
			case MSG_CUSTOM_MESSAGE3:
			case MSG_CUSTOM_MESSAGE4:
			case MSG_CUSTOM_MESSAGE5:
			case MSG_CUSTOM_MESSAGE6:
			case MSG_CUSTOM_MESSAGE7:
			case MSG_CUSTOM_MESSAGE8:
			case MSG_CUSTOM_MESSAGE9:
			case MSG_CUSTOM_MESSAGE10:
			case MSG_CUSTOM_MESSAGE11:
			case MSG_CUSTOM_MESSAGE12:
			case MSG_CUSTOM_MESSAGE13:
			case MSG_CUSTOM_MESSAGE14:
			case MSG_CUSTOM_MESSAGE15:
			case MSG_CUSTOM_MESSAGE16:
			case MSG_REPAINT:
			case MSG_D_PRIMITIVE_LINE:
			case MSG_D_PRIMITIVE_ARC:
			case MSG_D_PRIMITIVE_FILL_ARC:
			case MSG_D_PRIMITIVE_RECT:
			case MSG_D_PRIMITIVE_FILL_RECT:
			case MSG_D_PRIMITIVE_POINT:
			case MSG_D_PRIMITIVE_COLOR:
				this.MSG_CUSTOM_MESSAGE(parseInt(Sender), MSG_ID, Target, Value, SendMessageFunction);
				break;
			case MSG_MESSAGE_BOX:
				this.ModalAlert(Target, Value, 0, BUTTONS_OK, 0);
				break;
			case MSG_MESSAGE_BOX_YESNO:
				this.ModalAlert(Target, Value, 0, BUTTONS_YES_NO, MSG_MESSAGE_BOX_YESNO);
				break;
			case MSG_MESSAGE_BOX_GENERIC:
				var param = parseInt(this.POST_STRING);
				this.ModalAlert(Target, Value, Math.floor(param / 0x100), param % 0x100, MSG_MESSAGE_BOX_GENERIC);
				break;
			case MSG_RAISE_ERROR:
				var title = Target;
				if (title[0] == "%")
					title = "Concept run-time error";

				this.ModalAlert(title, Value, MESSAGE_ERROR, BUTTONS_OK, 0);
				console.error(Value);
				this.NotifyLoading("Concept run-time error");
				break;
			case MSG_MESSAGE_REQUESTINPUT:
				this.ModalInput(this.POST_STRING, Target, parseInt(Value));
				break;
			case MSG_MESSAGE_LOGIN:
				this.NotifyLoading("login required");
				this.ModalLogin(Target, Value, this.POST_STRING, Sender.toUpperCase(), this.POST_TARGET);
				break;
			case MSG_APPLICATION_RUN:
				this.NotifyLoading("application initialized");
				if (this.LoadingContainer)
					this.LoadingContainer.style.display = "none";

				this.MainForm = this.Controls[parseInt(Target)];
				if (this.MainForm) {
					this.MainForm.style.display = "";//this.MainForm.getAttribute("OrigDisplay");
					this.MainForm.style.height = "100%";
					this.MainForm.style.width = "100%";
					this.CleanClose = false;
				}
				if (this.PendingCode) {
					setTimeout(function() {
						for (var k in self.PendingCode) {
							var f = self.PendingCode[k];
							f();
						}
						self.PendingCode = null;
					}, 1);
				}
				if (self.UIRun)
					self.UIRun(1, "");
				console.log("UI loading time: " + ((new Date().getTime() - this.StartTime)/1000) + " seconds");
				break;
			case MSG_SAVE_FILE:
				var re = /(?:\.([^.]+))?$/;
				var blob_type = "application/binary";
				var ext_p = re.exec(Sender)[1];
				if (ext_p) {
					var ext = ext_p.toLowerCase(); 
					if ((ext) && (mimeTypes[ext]))
						blob_type = mimeTypes[ext];
				}
				saveAs(new Blob([this.POST_STRING_BUF], {type: blob_type}), Sender);
				this.POST_STRING = "";
				this.POST_STRING_BUF = 0;
				SendMessageFunction("%CLIENT", MSG_SAVE_FILE, "", "1", 0);
				break;
			case MSG_SAVE_FILE2:
				SendMessageFunction("%CLIENT", MSG_SAVE_FILE2, "", "1", 0);
				this.POST_FILENAME = Sender;
				break;
			case MSG_SEND_COOKIE:
				this.Cookies[Target] = ValueBuffer;
				break;
			case MSG_SEND_ZCOOKIE:
				// to do
				break;
			case MSG_REQUEST_FOR_FILE:
				this.ModalUpload(Target, Value, MSG_REQUEST_FOR_FILE);
				break;
			case MSG_REQUEST_FOR_CHUNK:
				this.ModalUpload(Target, Value, MSG_REQUEST_FOR_CHUNK);
				break;
			case MSG_SEND_COOKIE_CHUNK:
				this.UPLOADED_FILE.push({ "file": new Uint8Array(0), "size": 0, "position": 0, "buffer": new ArrayBuffer(0), "cookie": true, "filename": Target});
				SendMessageFunction(Sender, MSG_ID, Target, "" + this.UPLOADED_FILE.length, 0);
				break;
			case MSG_SAVE_CHUNK:
				this.ModalSave(Sender, Target, Value);
				break;
			case MSG_CHUNK:
				var file_id = parseInt(Target);
				if (file_id > 0) {
					var file = this.UPLOADED_FILE[file_id - 1];
					if (file) {
						switch (Sender.toLowerCase()) {
							case "recv":
								var chunk_size = parseInt(Value);
								if (chunk_size < 0) {
									this.UPLOADED_FILE[file_id - 1] = null;
									SendMessageFunction(Sender, MSG_ID, Target, "1", 0);
									if (file.cookie)
										this.Cookies[file.filename] = file.buffer;
									if (file.upload) {
										var re = /(?:\.([^.]+))?$/;
										var ext_p = re.exec(file.filename)[1];
										var blob_type = "application/binary";
										if (ext_p) {
											var ext = ext_p.toLowerCase(); 
											if ((ext) && (mimeTypes[ext]))
												blob_type = mimeTypes[ext];
										}
										saveAs(new Blob([file.buffer], {type: blob_type}), file.filename);
									}
								} else
								if (file.position < file.size) {
									var remaining = file.size - file.position;
									if (remaining < chunk_size)
										chunk_size = remaining;
									var chunk_res = file.file.subarray(file.position, file.position + chunk_size);
									SendMessageFunction(Sender, MSG_ID, "" + chunk_res.length, chunk_res, 1);
									file.position += chunk_size;
								} else
									SendMessageFunction(Sender, MSG_ID, "0", "", 0);
								break;
							case "skip":
								file.position += parseInt(Value);
								SendMessageFunction(Sender, MSG_ID, Target, "1", 0);
								break;
							case "close":
								this.UPLOADED_FILE[file_id - 1] = null;
								SendMessageFunction(Sender, MSG_ID, Target, "1", 0);
								if (file.cookie)
									this.Cookies[file.filename] = file.buffer;
								if (file.upload) {
									var re = /(?:\.([^.]+))?$/;
									var ext_p = re.exec(file.filename)[1];
									var blob_type = "application/binary";
									if (ext_p) {
										var ext = ext_p.toLowerCase(); 
										if ((ext) && (mimeTypes[ext]))
											blob_type = mimeTypes[ext];
									}
									saveAs(new Blob([file.buffer], {type: blob_type}), file.filename);
								}
								break;
							case "send":
								file.position += ValueBuffer.byteLength;
								if (file.buffer.byteLength) {
									var tmp =  new Uint8Array(file.buffer.byteLength + ValueBuffer.byteLength);
									tmp.set(new Uint8Array(file.buffer), 0);
									tmp.set(new Uint8Array(ValueBuffer), file.buffer.byteLength);
									file.buffer = tmp.buffer;
								} else
									file.buffer = ValueBuffer;
								SendMessageFunction(Sender, MSG_ID, Target, "" + ValueBuffer.byteLength, 0);
								break;
							default:
								SendMessageFunction(Sender, MSG_ID, Target, Value, 0);
						}
					} else
						SendMessageFunction(Sender, MSG_ID, Target, "-1", 0);
				} else
					SendMessageFunction(Sender, MSG_ID, Target, "-1", 0);
				break;
			case MSG_OS_COMMAND:
				var cookie = this.GetCookie(Value, this.Cookies);
				if (cookie)
					window.open(cookie, "_blank");
				break;
			case MSG_NON_CRITICAL:
			case MSG_MESSAGING_SYSTEM:
				// not needed anymore
				break;
			case MSG_STATIC_QUERY:
				this.AudioQuery(Sender, Target, Value);
				break;
			case MSG_CLIENT_ENVIRONMENT:
				if ((Target) && (Target.length) && (Target.toLowerCase() != "simid"))
					setCookie(Target, Value, 36500);

				SendMessageFunction(Sender, MSG_CLIENT_ENVIRONMENT, "", document.cookie, 0);
				break;
			case MSG_CLIENT_QUERY:
				this.NotifyLoading("server queried for \"" + Target + "\"");
				switch (Target.toLowerCase()) {
					case "version":
						SendMessageFunction("%CLIENT", MSG_CLIENT_QUERY, "Concept Client v4.0 JS", "(c)2005-2015 Devronium Applications", 0);
						break;
					case "system":
						if (navigator.platform)
							SendMessageFunction("%CLIENT", MSG_CLIENT_QUERY, "" + navigator.platform, "browser", 0);
						else
							SendMessageFunction("%CLIENT", MSG_CLIENT_QUERY, "browser", "", 0);
						break;
					case "host":
						SendMessageFunction("%CLIENT", MSG_CLIENT_QUERY, Target, this.URL, 0);
						break;
					case "language":
						if (navigator.language)
							SendMessageFunction("%CLIENT", MSG_CLIENT_QUERY, Target, navigator.language, 0);
						else
						if (navigator.browserLanguage)
							SendMessageFunction("%CLIENT", MSG_CLIENT_QUERY, Target, navigator.browserLanguage, 0);
						else
						if (navigator.systemLanguage)
							SendMessageFunction("%CLIENT", MSG_CLIENT_QUERY, Target, navigator.systemLanguage, 0);
						else
						if (navigator.userLanguage)
							SendMessageFunction("%CLIENT", MSG_CLIENT_QUERY, Target, navigator.userLanguage, 0);
						else
							SendMessageFunction("%CLIENT", MSG_CLIENT_QUERY, Target, "en", 0);
						break;
					case "openlink":
						this.ChildApp = new ConceptClient(Value, this.Container, this.LoadingContainer);
						SendMessageFunction(Sender, MSG_ID, Target, "1", 0);
						break;
					case "openurl":
						window.open(Value, '_blank');
						SendMessageFunction(Sender, MSG_ID, Target, "1", 0);
						break;						
					case "defaultdisplay":
						SendMessageFunction(Sender, MSG_ID, Target, "0", 0);
						break;
					case "location":
						if (navigator.geolocation) {
							navigator.geolocation.getCurrentPosition(function(position) {
								if ((position) && (position.coords))
									SendMessageFunction(Sender, MSG_ID, Target, "" + position.coords.latitude + "," + position.coords.longitude, 0);
								else
									SendMessageFunction(Sender, MSG_ID, Target, "No location", 0);
							});
						} else {
							SendMessageFunction(Sender, MSG_ID, Target, "No location", 0);
						}
						break;
					case "orientation":
						SendMessageFunction(Sender, MSG_ID, Target, "No orientation", 0);
						break;
					case "accelerometer":
						SendMessageFunction(Sender, MSG_ID, Target, "No sensor", 0);
						break;
					case "notify":
						if ('Notification' in window) {
							Notification.requestPermission(function() {
								var notification = new Notification(Sender, { body: Value, icon: self.ResourceRoot + "/conceptclienticon.png" });
							});
						} else {
							if ((Sender) && (Sender.length))
								this.AlertsLabel.innerHTML = "<strong>" + Sender + "</strong><hr/>" + Value;
							else
								this.AlertsLabel.innerHTML = Value;
							this.Alerts.style.display = "block";
						}
						SendMessageFunction(Sender, MSG_ID, Target, "", 0);
						break;
					case "tts":
						if (window.SpeechSynthesisUtterance) {
							var utterance = new SpeechSynthesisUtterance();
							utterance.text = Value;
							if ((Sender) && (Sender.length))
								utterance.lang = Sender;
               						utterance.rate = 1;
							utterance.pitch = 1;
							window.speechSynthesis.speak(utterance)
							SendMessageFunction(Sender, MSG_ID, Target, "1", 0);
						} else
							SendMessageFunction(Sender, MSG_ID, Target, "0", 0);
						break;
					case "useraccount":
						SendMessageFunction(Sender, MSG_ID, Target, "none", 0);
						break;
					case "manufacturer":
						if (navigator.appName)
							SendMessageFunction(Sender, MSG_ID, Target, "" + navigator.appName, 0);
						else
							SendMessageFunction(Sender, MSG_ID, Target, "", 0);
						break;
					case "model":
						if (navigator.userAgent)
							SendMessageFunction(Sender, MSG_ID, Target, "" + navigator.userAgent, 0);
						else
							SendMessageFunction(Sender, MSG_ID, Target, "", 0);
						break;
					case "simid":
						var uuid = getCookie("simid");
						if ((!uuid) || (uuid.length < 16)) {
							uuid = guid();
							setCookie("simid", uuid, 36500);
						}
						SendMessageFunction(Sender, MSG_ID, Target, "" + uuid, 0);
						break;
					case "token":
						if ('serviceWorker' in navigator) {
							navigator.serviceWorker.register("/@ConceptServiceWorker.js", {scope: '/'}).then(function(registration) {
								navigator.serviceWorker.ready.then(function(serviceWorkerRegistration) {
									if (serviceWorkerRegistration.pushManager) {
										serviceWorkerRegistration.pushManager.subscribe({userVisibleOnly:true}).then(function(subscription) {
											if ((subscription.subscriptionId) && (subscription.endpoint.indexOf(subscription.subscriptionId) < 0))
												SendMessageFunction(Sender, MSG_ID, Target, subscription.endpoint + "/" + subscription.subscriptionId, 0);
											else
												SendMessageFunction(Sender, MSG_ID, Target, subscription.endpoint, 0);
									        	return true;
										}).catch(function(e) {
											SendMessageFunction(Sender, MSG_ID, Target, "-2", 0);
											if (Notification.permission === 'denied') {
												console.warn('Permission for Notifications was denied');
											} else {
												console.error('Unable to subscribe to push.', e);
											}
										});
									} else {
										SendMessageFunction(Sender, MSG_ID, Target, "-3", 0);
										console.error('Push notifications are not supported in this browser');
									}
								});

								// already registered ?
								if ((registration) && (registration.pushManager) && (registration.pushManager.getSubscription)) {
									registration.pushManager.getSubscription().then(function(subscription) {
										if (subscription) {
											if ((subscription.subscriptionId) && (subscription.endpoint.indexOf(subscription.subscriptionId) < 0))
												SendMessageFunction(Sender, MSG_ID, Target, subscription.endpoint + "/" + subscription.subscriptionId, 0);
											else
												SendMessageFunction(Sender, MSG_ID, Target, subscription.endpoint, 0);
										}
									});
								}

							}).catch(function(error) {
								console.error(error);
								SendMessageFunction(Sender, MSG_ID, Target, error.toString(), 0);
							});
						} else {
							console.warn("Service workers are not supported in this browser");
							SendMessageFunction(Sender, MSG_ID, Target, "-1", 0);
						}
						break;
					case "style":
						this.Container.style.cssText += Value;
						SendMessageFunction(Sender, MSG_ID, Target, Value, 0);
						break;
					case "displays":
						SendMessageFunction(Sender, MSG_ID, Target, "1", 0);
						break;
					case "codecs":
						SendMessageFunction(Sender, MSG_ID, Target, "Speex;Opus;H.264;", 0);
						break;
					case "ring":
						if (parseInt(Value)) {
							if (this.Ring)
								this.Ring.pause();
							this.Ring = new Audio(this.ResourceRoot + "/audio/ringtone.mp3");
							//this.Ring.autoplay = true;
							this.Ring.play();
						} else {
							if (this.Ring)
								this.Ring.pause();
							this.Ring = null;
						}
						SendMessageFunction(Sender, MSG_ID, Target, Value, 0);
						break;
					case "network":
						SendMessageFunction(Sender, MSG_ID, Target, "1.0", 0);
						break;
					case "html":
						var data = JSON.parse(Value);
						if (data) {
							if (data.content) {
								var html = document.createElement('html');
								html.innerHTML = data.content;
								var elements = html.childNodes;
								for (var  i = 0; i < elements.length; i++) {
									var node = elements[i];
									var node_name = node.nodeName.toLowerCase();
									if (node_name == "head")
										data.header = node.innerHTML;
									else
									if (node_name == "body")
										data.html = node.innerHTML;
								}
							}
							if (data.header) {
								var head = document.getElementsByTagName('head')[0];
								if (head) {
									var div = document.createElement('div');
									div.innerHTML = data.header;
									var elements = div.childNodes;

									for (var  i = 0; i < elements.length; i++) {
										var node = elements[i];
										var node_name = node.nodeName.toLowerCase();
										switch (node_name) {
											case "script":
											case "link":
												var script = document.createElement(node_name);
												if (node.src)
													script.src = node.src;
												if (node.href)
													script.href = node.href;
												if (node.rel)
													script.rel = node.rel;
												script.innerHTML = node.innerHTML;
												if (node.type)
													script.type = node.type;
												if (node.onload)
													script.onload = node.onload;
												head.appendChild(script);
												break;
											default:
												head.appendChild(node.cloneNode(true));
										}
									}
								} else
									console.warn("Error appending to head element");
							}
							js_flags["snap." + Sender] = data.html;
						}
						SendMessageFunction(Sender, MSG_ID, Target, "1", 0);
						break;
					case "ping":
					default:
						SendMessageFunction(Sender, MSG_ID, Target, Value, 0);
				}
				break;
			case MSG_APPLICATION_QUIT:
				if (self.UIRun) {
					self.UIRun(0, "");
				} else {
					if (self.Container)
						self.Container.style.display = "none";
					if (self.LoadingContainer)
						self.Container.style.display = "block";
				}
				self.MainForm = null;
				self.NotifyLoading("Done");
				self.CleanClose = true;
				self.ConceptSocket.close();
				if (this.MediaObject) {
					this.MediaObject.stop();
					delete this.MediaObject;
				}
				this.TerminateWorkers();
				break;
			case MSG_RUN_APPLICATION:
				self.RunApplication(Target, parseInt(Value), false);
				break;
			case MSG_DEBUG_APPLICATION:
				self.RunApplication(Target, parseInt(Value), true);
				break;
			case MSG_SET_PROPERTY_BY_NAME:
				var element = self.Controls[parseInt(Sender)];
				if (element)
					element[Target] = Value;
				break;
			case MSG_CHOOSE_COLOR:
				self.ChooseColor(Target);
				break;
			case MSG_CHOOSE_FONT:
				self.ChooseFont(Target);
				break;
			case MSG_SET_COOKIE:
				if ("localStorage" in window && window["localStorage"] !== null)
					localStorage.setItem(Target, Value);
				break;
			case MSG_GET_COOKIE:
				if ("localStorage" in window && window["localStorage"] !== null) {
					var cookie = localStorage.getItem(Target);
					if (cookie)
						SendMessageFunction(Sender, MSG_ID, Target, "" + cookie, 0);
					else
						SendMessageFunction(Sender, MSG_ID, Target, "", 0);
				} else
					SendMessageFunction(Sender, MSG_ID, Target, "", 0);
				break;
			case MSG_CONFIRM_EVENT:
				// ignore this message
				break;
			case MSG_SET_CLIPBOARD:
				if (parseInt(Target) == 2) {
					var code = function() {
						var input = document.createElement("input");
						input.setAttribute("type", "text");
						input.style.position = "absolute";
						input.style.top = "0px";
						document.body.appendChild(input);
						input.setAttribute("value", "test");
							input.value = Value;
						input.setSelectionRange(0, input.value.length);
						input.select();
						document.execCommand("copy");
						document.body.removeChild(input);
					}

					if (self.MainForm)
						this.MainForm.addEventListener('click', code);
					else {
						var clipboard_function = function() {
							code();
							self.MainForm.removeEventListener('click', clipboard_function);
						};

						self.PendingCode.Clipboard = function() {
							self.MainForm.addEventListener('click', clipboard_function);
						}
					}
				}
				break;
			default:
				SendMessageFunction(Sender, MSG_ID, Target, Value, 0);
				if (Value.length > 0xFF) {
					Sender = parseInt("" + Sender);
					Value = "(value omitted)";
				}
				if (Target.length > 0xFF) {
					Sender = parseInt("" + Sender);
					Target = "(target omitted)";
				}
				console.warn("Unknown message (" + Sender + "\n, " + MSG_ID + ", \n" + Target + ", \n" + Value + ")\n");
		}
	}

	this.RunApplication = function(AppName, ParentAPID, debug) {
		var url = "";
		var parser = document.createElement('a');
		parser.href = document.URL;
 
		url = parser.protocol + "//" + parser.hostname;
		if ((parser.port) && (parser.port != 0))
		 	url += ":" + parser.port;
		url += "/" + AppName;
		url += "?parent=" + ParentAPID;
		if (debug)
			url += "&debug=1";
		window.open(url, "_blank");
	}

	this.GetCookie = function(Value, CookieList) {
		var cookie = CookieList[Value];
		if ((!cookie) && (Value.length > 2)) {
			var c = Value.charAt(0);
			if ((c == '"') || (c == "'")) {
				if (Value.charAt(Value.length - 1) == c) {
					Value = Value.substr(1, Value.length - 2);
					cookie = CookieList[Value];
				}
			}
		}
		if (cookie) {
			var re = /(?:\.([^.]+))?$/;
			var ext = re.exec(Value)[1].toLowerCase(); 
			var blob_type = "application/binary";
			if ((ext) && (mimeTypes[ext]))
				blob_type = mimeTypes[ext];
			return "data:" + blob_type + ";base64," + ToBase64(cookie);
		}
		return "";
	}

	this.AudioQuery = function(Sender, Target, Value) {
		var obj = this.AudioObjects[Target];
		if (!obj) {
			obj = new Audio("");
			obj.id = Target;

			this.AudioObjects[Target] = obj;
		}
		var query = Value.split(":");
		if (query) {
			var command = query[0].toLowerCase();
			switch (command) {
				case "open":
					obj.src = this.GetCookie(query[1], this.Cookies);
					break;
				case "play":
					obj.play();
					break;
				case "pause":
					obj.pause();
					break;
				case "stop":
					obj.pause();
					break;
				case "repeat":
					if (parseInt(query[1])) {
						obj.addEventListener('ended', function() {
							this.currentTime = 0;
							this.play();
						}, false);
					}
					break;
				case "vibrate":
					window.navigator = window.navigator || {};
					if (navigator.vibrate)
						navigator.vibrate(500);
					break;
				default:
					obj.src = this.ResourceRoot + "/audio/" + command + ".mp3";
					obj.play();
					break;
			}
		}	
	}

	this.SetResponse = function(button, container, Response, ResponseValue) {
		if (Response) {
			button.onclick = function() {
				container.style.display = "none";
				if (Response == MSG_MESSAGE_REQUESTINPUT)
					self.SendMessage("%CLIENT", Response, document.getElementById("inputInput").value, "" + ResponseValue, 0);
				else
					self.SendMessage("%CLIENT", Response, "", "" + ResponseValue, 0);
			}
		} else {
			button.onclick = function() {
				container.style.display = "none";
			}
		}
	}

	this.ModalAlert = function(Title, Content, Type, Buttons, Response) {
		var container = document.getElementById("alertDialog");
		if (container)
			this.Container.removeChild(container);

		container = document.createElement("div");
		container.className = "RMessageBox modal";
		container.id = "alertDialog";
		type = "btn-success";

		var type_img = "";
		switch (Type) {
			case MESSAGE_INFO:
				type_img = "<i class='fa fa-info' aria-hidden='true'></i> ";
				type = "btn-primary";
				break;
			case MESSAGE_WARNING:
				type_img = "<i class='fa fa-warning' aria-hidden='true'></i> ";
				type = "btn-warning";
				break;
			case MESSAGE_QUESTION:
				type_img = "<i class='fa fa-question-circle' aria-hidden='true'></i> ";
				type = "btn-success";
				break;
			case MESSAGE_ERROR:
				type_img = "<i class='fa fa-exclamation-circle' aria-hidden='true'></i> ";
				type = "btn-danger";
				break;
		}

		container.innerHTML = '<div class="RMessageBox modal-dialog"><div class="modal-content"><div class="RMessageBoxHeader modal-header ' + type + '"><button type="button" class="close" data-dismiss="modal" id="alertClose"><span aria-hidden="true">&times;</span><span class="sr-only">Close</span></button><h4 class="modal-title" id="alertTitle"></h4></div><div class="RMessageBoxContent modal-body"><p id="alertMessage"></p></div><div class="RMessageBoxButtons modal-footer" id="alertButtons"></div></div></div>';
			
		this.Container.appendChild(container);

		container.style.display = "block";

		var title = document.getElementById("alertTitle");
		if (title)
			title.innerHTML = type_img + Title;

		var msg = document.getElementById("alertMessage");
		if (msg)
			msg.innerHTML = Content.replace(/(?:\r\n|\r|\n)/g, '<br />');

		var bcontainer = document.getElementById("alertButtons");
		var button;

		if (bcontainer) {
			bcontainer.innerHTML = "";

			switch (Buttons) {
				case BUTTONS_NONE:
					break;
				case BUTTONS_CLOSE:
					button = document.createElement("button");
					button.className = "RButton btn btn-primary";
					button.innerHTML = "Close";
					button.focus();
					button.setAttribute("data-dismiss", "modal");
					this.SetResponse(button, container, Response, RESPONSE_CLOSE);
					bcontainer.appendChild(button);
					break;
				case BUTTONS_CANCEL:
					button = document.createElement("button");
					button.className = "RButton btn btn-primary";
					button.innerHTML = "Cancel";
					button.focus();
					button.setAttribute("data-dismiss", "modal");
					this.SetResponse(button, container, Response, RESPONSE_CANCEL);
					bcontainer.appendChild(button);
					break;
				case BUTTONS_YES_NO:
					button = document.createElement("button");
					button.className = "RButton btn btn-primary";
					button.innerHTML = "Yes";
					button.focus();
					button.setAttribute("data-dismiss", "modal");
					this.SetResponse(button, container, Response, RESPONSE_YES);
					bcontainer.appendChild(button);

					button = document.createElement("button");
					button.className = "RButton btn btn-default";
					button.innerHTML = "No";
					button.setAttribute("data-dismiss", "modal");
					this.SetResponse(button, container, Response, RESPONSE_NO);
					bcontainer.appendChild(button);
					break;
				case BUTTONS_OK_CANCEL:
					button = document.createElement("button");
					button.className = "RButton btn btn-default";
					button.innerHTML = "Cancel";
					button.setAttribute("data-dismiss", "modal");
					this.SetResponse(button, container, Response, RESPONSE_CANCEL);
					bcontainer.appendChild(button);

					button = document.createElement("button");
					button.className = "RButton btn btn-primary";
					button.innerHTML = "Ok";
					button.setAttribute("data-dismiss", "modal");
					button.focus();
					this.SetResponse(button, container, Response, RESPONSE_OK);
					bcontainer.appendChild(button);
					break;
				case BUTTONS_YES_NO_CANCEL:
					button = document.createElement("button");
					button.className = "RButton btn btn-default";
					button.innerHTML = "Cancel";
					button.setAttribute("data-dismiss", "modal");
					this.SetResponse(button, container, Response, RESPONSE_CANCEL);
					bcontainer.appendChild(button);

					button = document.createElement("button");
					button.className = "RButton btn btn-primary";
					button.innerHTML = "Yes";
					button.setAttribute("data-dismiss", "modal");
					this.SetResponse(button, container, Response, RESPONSE_YES);
					bcontainer.appendChild(button);

					button = document.createElement("button");
					button.className = "RButton btn btn-warning";
					button.innerHTML = "No";
					button.setAttribute("data-dismiss", "modal");
					this.SetResponse(button, container, Response, RESPONSE_NO);
					bcontainer.appendChild(button);
					break;
				case BUTTONS_SAVE_DISCARD_CANCEL:
					button = document.createElement("button");
					button.className = "RButton btn btn-default";
					button.innerHTML = "Cancel";
					button.setAttribute("data-dismiss", "modal");
					this.SetResponse(button, container, Response, RESPONSE_CANCEL);
					bcontainer.appendChild(button);

					button = document.createElement("button");
					button.className = "RButton btn btn-danger";
					button.innerHTML = "Discard";
					button.setAttribute("data-dismiss", "modal");
					this.SetResponse(button, container, Response, RESPONSE_NO);
					bcontainer.appendChild(button);

					button = document.createElement("button");
					button.className = "RButton btn btn-primary";
					button.innerHTML = "Save";
					button.focus();
					button.setAttribute("data-dismiss", "modal");
					this.SetResponse(button, container, Response, RESPONSE_YES);
					bcontainer.appendChild(button);
					break;
				case BUTTONS_OK:
				default:
					button = document.createElement("button");
					button.className = "RButton btn btn-primary";
					button.innerHTML = "Ok";
					button.setAttribute("data-dismiss", "modal");
					button.focus();
					this.SetResponse(button, container, Response, RESPONSE_OK);
					bcontainer.appendChild(button);
					break;
			}
		}

		var close = document.getElementById("alertClose");
		if (close)
			this.SetResponse(close, container, Response, RESPONSE_DELETE_EVENT);

	}

	this.ModalInput = function(Title, Content, Masked) {
		var container = document.getElementById("inputDialog");
		if (container)
			this.Container.removeChild(container);

		container = document.createElement("div");
		container.className = "RMessageBox modal";
		container.id = "inputDialog";
		container.innerHTML = '<div class="RMessageBox login-dialog modal-dialog"><div class="modal-content"><div class="RMessageBoxHeader modal-header btn-primary"><button type="button" class="close" data-dismiss="modal" id="inputClose"><span aria-hidden="true">&times;</span><span class="sr-only">Close</span></button><h4 class="modal-title" id="inputTitle"></h4></div><div class="RMessageBoxContent modal-body"><input type="text" id="inputInput" class="RMessageBoxInput form-control"/></div><div class="RMessageBoxButtons modal-footer" id="inputButtons"></div></div></div>';
			
		this.Container.appendChild(container);
		
		container.style.display = "block";

		var title = document.getElementById("inputTitle");
		if (title)
			title.innerHTML = Title;

		var msg = document.getElementById("inputInput");
		if (msg) {
			if (Masked)
				msg.type = "password";
			else
				msg.type = "text";

			msg.onkeypress = function(evt) {
				if (evt.keyCode == 13) {
					container.style.display = "none";
					self.SendMessage("%CLIENT", MSG_MESSAGE_REQUESTINPUT, msg.value, "" + RESPONSE_OK, 0);
				}
			}

			msg.value = Content;
			msg.focus();
			msg.select();
		}

		var bcontainer = document.getElementById("inputButtons");
		var button;

		if (bcontainer) {
			bcontainer.innerHTML = "";
			button = document.createElement("button");
			button.className = "RButton btn btn-default";
			button.innerHTML = "Cancel";
			button.setAttribute("data-dismiss", "modal");
			this.SetResponse(button, container, MSG_MESSAGE_REQUESTINPUT, RESPONSE_CANCEL);
			bcontainer.appendChild(button);

			button = document.createElement("button");
			button.className = "RButton btn btn-primary";
			button.innerHTML = "Ok";
			button.setAttribute("data-dismiss", "modal");
			this.SetResponse(button, container, MSG_MESSAGE_REQUESTINPUT, RESPONSE_OK);
			bcontainer.appendChild(button);
		}

		var close = document.getElementById("inputClose");
		if (close)
			this.SetResponse(close, container, MSG_MESSAGE_REQUESTINPUT, RESPONSE_DELETE_EVENT);
	}

	this.ChooseColor = function(Title) {
		var container = document.getElementById("colorDialog");
		if (container)
			this.Container.removeChild(container);

		container = document.createElement("div");
		container.className = "RMessageBox modal";
		container.id = "colorDialog";
		container.innerHTML = '<div class="RMessageBox login-dialog modal-dialog"><div class="modal-content"><div class="RMessageBoxHeader modal-header btn-info"><button type="button" class="close" data-dismiss="modal" id="colorClose"><span aria-hidden="true">&times;</span><span class="sr-only">Close</span></button><h4 class="modal-title" id="colorTitle"></h4></div><div class="RMessageBoxContent modal-body"><div class="input-group colorpicker-component"><span class="input-group-addon"><i></i></span><input type="text" id="colorInput" value="" class="form-control" /></div></div><div class="RMessageBoxButtons modal-footer" id="colorButtons"></div></div></div>';
			
		this.Container.appendChild(container);
		
		$(".colorpicker-component").colorpicker();

		container.style.display = "block";

		var title = document.getElementById("colorTitle");
		if (title)
			title.innerHTML = Title;

		var color = document.getElementById("colorInput");
		if (color) {
			color.value = "";
			color.focus();
			color.select();
		}

		var bcontainer = document.getElementById("colorButtons");
		var button;

		if (bcontainer) {
			bcontainer.innerHTML = "";
			button = document.createElement("button");
			button.className = "RButton btn btn-default";
			button.innerHTML = "Cancel";
			button.setAttribute("data-dismiss", "modal");
			button.onclick = function() {
				container.style.display="none";
				self.SendMessage("%CLIENT", MSG_CHOOSE_COLOR, "0", "-1", 0);
			}
			bcontainer.appendChild(button);

			button = document.createElement("button");
			button.className = "RButton btn btn-primary";
			button.innerHTML = "Ok";
			button.setAttribute("data-dismiss", "modal");
			button.onclick = function() {
				container.style.display="none";
				var colorText = color.value;
				var colorvalue = 0;
				if ((colorText) && (colorText.length))
					colorvalue = parseInt(colorText.substring(1), 16);
				self.SendMessage("%CLIENT", MSG_CHOOSE_COLOR, "1", "" + colorvalue, 0);
			}
			bcontainer.appendChild(button);
		}

		var close = document.getElementById("colorClose");
		if (close) {
			close.onclick = function() {
				container.style.display="none";
				self.SendMessage("%CLIENT", MSG_CHOOSE_COLOR, "0", "-1", 0);
			}
		}
	}

	this.ChooseFont = function(Title) {
		var container = document.getElementById("fontDialog");
		if (container)
			this.Container.removeChild(container);

		container = document.createElement("div");
		container.className = "RMessageBox modal";
		container.id = "fontDialog";
		container.innerHTML = '<div class="RMessageBox login-dialog modal-dialog"><div class="modal-content"><div class="RMessageBoxHeader modal-header btn-info"><button type="button" class="close" data-dismiss="modal" id="fontClose"><span aria-hidden="true">&times;</span><span class="sr-only">Close</span></button><h4 class="modal-title" id="fontTitle"></h4></div><div class="RMessageBoxContent modal-body"><label for="fontInput" id="fontLabel">The quick brown fox jumps over the lazy dog</label><select class="form-control" id="fontInput" onchange="document.getElementById(\'fontLabel\').style.fontFamily = this.value;"><optgroup label="Web safe fonts"><option selected="selected"> </option><option>Arial</option><option>Arial Black</option><option>Charcoal</option><option>Book Antiqua</option><option>Comic Sans MS</option><option>Courier New</option><option>Gadget</option><option>Garamond</option><option>Geneva</option><option>Georgia</option><option>Helvetica</option><option>Impact</option><option>Lucida Console</option><option>Lucida Grande</option><option>Lucida Sans Unicode</option><option>Palatino</option><option>Palatino Linotype</option><option>Serif</option><option>Tahoma</option><option>Trebuchet MS</option></optgroup></select></div><div class="RMessageBoxButtons modal-footer" id="fontButtons"></div></div></div>';
		this.Container.appendChild(container);
		
		container.style.display = "block";

		var title = document.getElementById("fontTitle");
		if (title)
			title.innerHTML = Title;

		var font = document.getElementById("fontInput");
		if (font) {
			font.value = "";
			font.focus();
		}

		var bcontainer = document.getElementById("fontButtons");
		var button;

		if (bcontainer) {
			bcontainer.innerHTML = "";
			button = document.createElement("button");
			button.className = "RButton btn btn-default";
			button.innerHTML = "Cancel";
			button.setAttribute("data-dismiss", "modal");
			button.onclick = function() {
				container.style.display="none";
				self.SendMessage("%CLIENT", MSG_CHOOSE_FONT, "0", "-1", 0);
			}
			bcontainer.appendChild(button);

			button = document.createElement("button");
			button.className = "RButton btn btn-primary";
			button.innerHTML = "Ok";
			button.setAttribute("data-dismiss", "modal");
			button.onclick = function() {
				container.style.display="none";
				var val = font.value;
				if (val.indexOf(" ") > 0)
					val = '"' + val + '"';
				self.SendMessage("%CLIENT", MSG_CHOOSE_FONT, "1", "" + val, 0);
			}
			bcontainer.appendChild(button);
		}

		var close = document.getElementById("fontClose");
		if (close) {
			close.onclick = function() {
				container.style.display="none";
				self.SendMessage("%CLIENT", MSG_CHOOSE_FONT, "0", "-1", 0);
			}
		}
	}

	this.ModalUpload = function(Title, FileType, MESSAGE_ID) {
		var container = document.getElementById("fileDialog");
		if (container)
			this.Container.removeChild(container);

		container = document.createElement("div");
		container.className = "RMessageBox modal";
		container.id = "fileDialog";
		container.innerHTML = '<div class="RMessageBox login-dialog modal-dialog"><div class="modal-content"><div class="RMessageBoxHeader modal-header btn-info"><button type="button" class="close" data-dismiss="modal" id="fileClose"><span aria-hidden="true">&times;</span><span class="sr-only">Close</span></button><h4 class="modal-title" id="fileTitle"></h4></div><div class="RMessageBoxContent modal-body"><span class="btn btn-primary btn-file">Open file <input type="file" id="fileInput"/></span></div><div class="RMessageBoxButtons modal-footer" id="fileButtons"></div></div></div>';
		this.Container.appendChild(container);
		
		container.style.display = "block";

		var title = document.getElementById("fileTitle");
		if (title)
			title.innerHTML = Title;

		var close = document.getElementById("fileClose");
		if (close) {
			close.onclick = function() {
				container.style.display = "none";
				if (MESSAGE_ID == MSG_REQUEST_FOR_FILE)
					self.SendMessage("%CLIENT", MSG_REQUEST_FOR_FILE, "", "", 0);
				else
					self.SendMessage("%CLIENT", MESSAGE_ID, "-1", "0", 0);
			}
		}

		var msg = document.getElementById("fileInput");
		if (msg) {
			msg.onchange = function() {
				container.style.display = "none";
				var file = msg.files[0];
				if (file) {
					var reader = new FileReader();
					reader.readAsArrayBuffer(file);
					reader.onload = function (evt) {
						var data = new Uint8Array(evt.target.result);
						if (MESSAGE_ID == MSG_REQUEST_FOR_FILE) {
							self.SendMessage("%CLIENT", MESSAGE_ID, msg.value, data, 1);
						} else {
							self.UPLOADED_FILE.push({ "file": data, "size": data.length, "position": 0, "buffer": new ArrayBuffer(0)});
							self.SendMessage(msg.value, MESSAGE_ID, "" + data.length, "" + self.UPLOADED_FILE.length, 0);
						}
					}
					reader.onerror = function (evt) {
						if (MSG_REQUEST_FOR_FILE)
							self.SendMessage("%CLIENT", MESSAGE_ID, "", "", 0);
						else
							self.SendMessage("%CLIENT", MESSAGE_ID, "", 0, 0);
						alert("Error reading file");
					}
				} else {
					if (MSG_REQUEST_FOR_FILE)
						self.SendMessage("%CLIENT", MESSAGE_ID, "", "", 0);
					else
						self.SendMessage("%CLIENT", MESSAGE_ID, "", 0, 0);
				}
			}
		}
	}

	this.ModalSave = function(Sender, Title, FileType) {
		var container = document.getElementById("fileDialog");
		if (container)
			this.Container.removeChild(container);

		container = document.createElement("div");
		container.className = "RMessageBox modal";
		container.id = "fileDialog";
		container.innerHTML = '<div class="RMessageBox modal-dialog"><div class="modal-content"><div class="RMessageBoxHeader modal-header btn-primary"><button type="button" class="close" data-dismiss="modal" id="fileClose"><span aria-hidden="true">&times;</span><span class="sr-only">Close</span></button><h4 class="modal-title" id="fileTitle"></h4></div><div class="RMessageBoxContent modal-body" id="fileDialogContent"></div><div class="RMessageBoxButtons modal-footer" id="fileButtons"></div></div></div>';
		this.Container.appendChild(container);

		container.style.display = "block";

		var title = document.getElementById("fileTitle");
		if (title)
			title.innerHTML = Title;

		var close = document.getElementById("fileClose");
		if (close)
			this.SetResponse(close, container, MSG_SAVE_CHUNK, 0);

		var msg = document.getElementById("fileDialogContent");
		var button = document.createElement("button");
		button.className = "RButton btn btn-warning";
		button.style.width = "100%";
		button.innerHTML = "Save&nbsp";
		button.appendChild(document.createTextNode(Sender));
		button.setAttribute("data-dismiss", "modal");
		msg.appendChild(button);

		button.onclick = function() {
			self.UPLOADED_FILE.push({ "file": new Uint8Array(0), "size": 0, "position": 0, "buffer": new ArrayBuffer(0), "filename": Sender, "upload": true });
			self.SendMessage(Sender, MSG_SAVE_CHUNK, "", "" + self.UPLOADED_FILE.length, 0);
			container.style.display="none";
		}
	}

	this.NotifyLogin = function(user, pass, response, method, challenge) {
		self.SendMessage("%CLIENT", MSG_MESSAGE_LOGIN, "", "" + response, 0);

		if (response == RESPONSE_OK) {
			switch (method) {
				case "MD5":
					pass = "" + CryptoJS.MD5(challenge + CryptoJS.MD5(pass).toString()).toString();
					break;
				case "SHA1":
					pass = "" + CryptoJS.SHA1(challenge + CryptoJS.SHA1(pass).toString()).toString();
					break;
				case "SHA256":
					pass = "" + CryptoJS.SHA256(challenge + CryptoJS.SHA256(pass).toString()).toString();
					break;
				case "PLAIN":
					break;
				default:
					// fall back to SHA1
					pass = "" + CryptoJS.SHA1(challenge + CryptoJS.SHA1(pass).toString()).toString();
					break;
			}
			self.SendMessage("%CLIENT", MSG_MESSAGE_LOGIN, user, pass, 0);
		} else
			self.SendMessage("%CLIENT", MSG_MESSAGE_LOGIN, "", "", 0);
	}

	this.ModalLogin = function(Username, Password, Title, Method, Challenge) {
		var container = document.getElementById("loginDialog");
		if (container)
			this.Container.removeChild(container);

		container = document.createElement("div");
		container.className = "RMessageBox modal";
		container.id = "loginDialog";
		container.innerHTML = '<div class="RMessageBox RMessageBoxPassword dialog modal-dialog"><div class="modal-content"><div class="RMessageBoxHeader modal-header btn-primary"><button type="button" class="close" data-dismiss="modal" id="loginClose"><span aria-hidden="true">&times;</span><span class="sr-only">Close</span></button><h4 class="modal-title" id="loginTitle"></h4></div><div class="RMessageBoxContent modal-body"><div class="form-group"><input type="text" id="usernameLogin" placeholder="Username" class="RMessageBoxInput form-control"/></div><div class="form-group"><input type="password" id="passwordLogin" placeholder="Password" class="RMessageBoxInput form-control"/></div><div class="RMessageBoxButtons modal-footer" id="loginButtons"></div></div></div></div>';
		this.Container.appendChild(container);

		container.style.display = "block";

		var title = document.getElementById("loginTitle");
		if (title)
			title.innerHTML = Title;

		var user = document.getElementById("usernameLogin");
		var pass = document.getElementById("passwordLogin");
		if ((user) && (pass)) {
			user.onkeypress = function(evt) {
				if (evt.keyCode == 13) {
					pass.select();
					pass.focus();
				}
			}

			user.value = Username;
			user.focus();
			user.select();

			pass.onkeypress = function(evt) {
				if (evt.keyCode == 13) {
					container.style.display = "none";
					self.NotifyLogin(user.value, pass.value, RESPONSE_OK, Method, Challenge);
				}
			}

			pass.value = Password;
		}

		var bcontainer = document.getElementById("loginButtons");
		var button;

		if (bcontainer) {
			bcontainer.innerHTML = "";

			if (!self.MainForm) {
				button = document.createElement("button");
				button.className = "RButton btn-link pull-left";
				button.innerHTML = "Sign-up";
				button.setAttribute("data-dismiss", "modal");
				button.onclick = function() {
					container.style.display = "none";
					self.NotifyLogin("", "", RESPONSE_SIGNUP, Method, "");
				}
				bcontainer.appendChild(button);
			}

			button = document.createElement("button");
			button.className = "RButton btn btn-default";
			button.innerHTML = "Cancel";
			button.setAttribute("data-dismiss", "modal");
			button.onclick = function() {
				container.style.display = "none";
				self.NotifyLogin("", "", RESPONSE_CANCEL, Method, "");
			}
			bcontainer.appendChild(button);

			button = document.createElement("button");
			button.className = "RButton btn btn-primary";
			button.innerHTML = "Sign-in";
			button.setAttribute("data-dismiss", "modal");
			button.onclick = function() {
				container.style.display = "none";
				self.NotifyLogin(user.value, pass.value, RESPONSE_OK, Method, Challenge);
			}
			bcontainer.appendChild(button);
		}

		var close = document.getElementById("loginClose");
		if (close) {
			close.onclick = function() {
				container.style.display = "none";
				self.NotifyLogin("", "", RESPONSE_DELETE_EVENT, Method, "");
			}
		}
	}

	this.PutStream = function(OwnerID, Target, Value, ValueBuffer) {
		var element = this.Controls[OwnerID];

		if (!element) {
			console.warn("MSG_PUT_STREAM: Unknown object RID: " + OwnerID);
			return;
		}
		var cls_id = element.ConceptClassID;
		if (!cls_id)
			cls_id = parseInt(element.getAttribute("ConceptClassID"));

		if (cls_id == CLASS_IMAGE) {
			var ext = Target.substr(Target.lastIndexOf('.') + 1).toLowerCase();
			if (!ext)
				ext = "png";

			var src = "data:image/" + ext + ";base64," + ToBase64(ValueBuffer);
			element.src = src;
			if (!element.ConceptHeight)
				element.style.height = "auto";
			if (!element.ConceptWidth)
				element.style.width = "auto";
		} else
		if ((cls_id == 1012) || (cls_id == 1001)) {
			switch (Target.toLowerCase()) {
				case "editable":
					if (parseInt(Value))
						element.contentEditable = "true";
					else
						element.contentEditable = "";
					if ((element.contentWindow.document) && (element.contentWindow.document.body))
						element.contentWindow.document.body.contentEditable = element.contentEditable;
					break;
				case "transparent":
					if (parseInt(Value))
						element.backgroundColor = "transparent";
					else
						element.backgroundColor = "";
					break;
			}
		}
	}

	this.GetStock = function(id) {
		switch (id) {
			case __ABOUT:
				return "gtk-about";

			case __ADD:
				return "gtk-add";

			case __APPLY:
				return "gtk-apply";

			case __BOLD:
				return "gtk-bold";

			case __CANCEL:
				return "gtk-cancel";

			case __CDROM:
				return "gtk-cdrom";

			case __CLEAR:
				return "gtk-clear";

			case __CLOSE:
				return "gtk-close";

			case __COLOR_PICKER:
				return "gtk-color-picker";

			case __CONNECT:
				return "gtk-connect";

			case __CONVERT:
				return "gtk-convert";

			case __COPY:
				return "gtk-copy";

			case __CUT:
				return "gtk-cut";

			case __DELETE:
				return "gtk-delete";

			case __DIALOG_AUTHENTICATION:
				return "gtk-dialog-authentication";

			case __DIALOG_ERROR:
				return "gtk-dialog-error";

			case __DIALOG_INFO:
				return "gtk-dialog-info";

			case __DIALOG_QUESTION:
				return "gtk-dialog-question";

			case __DIALOG_WARNING:
				return "gtk-dialog-warning";

			case __DIRECTORY:
				return "gtk-directory";

			case __DISCONNECT:
				return "gtk-disconnect";

			case __DND:
				return "gtk-dnd";

			case __DND_MULTIPLE:
				return "gtk-dnd-multiple";

			case __EDIT:
				return "gtk-edit";

			case __EXECUTE:
				return "gtk-execute";

			case __FILE:
				return "gtk-file";

			case __FIND:
				return "gtk-find";

			case __FIND_AND_REPLACE:
				return "gtk-find-and-replace";

			case __FLOPPY:
				return "gtk-floppy";

			case __FULLSCREEN:
				return "gtk-fullscreen";

			case __GO_BACK:
				return "gtk-go-back";

			case __GO_DOWN:
				return "gtk-go-down";

			case __GO_FORWARD:
				return "gtk-go-forward";

			case __GO_UP:
				return "gto-go-up";

			case __GOTO_BOTTOM:
				return "gtk-goto-bottom";

			case __GOTO_FIRST:
				return "gtk-goto-first";

			case __GOTO_LAST:
				return "gtk-goto-last";

			case __GOTO_TOP:
				return "gtk-goto-top";

			case __HARDDISK:
				return "gtk-harddisk";

			case __HELP:
				return "gtk-help";

			case __HOME:
				return "gtk-home";

			case __INDENT:
				return "gtk-indent";

			case __INDEX:
				return "gtk-index";

			case __INFO:
				return "gtk-info";

			case __ITALIC:
				return "gtk-italic";

			case __JUMP_TO:
				return "gtk-jump-to";

			case __JUSTIFY_CENTER:
				return "gtk-justify-center";

			case __JUSTIFY_FILL:
				return "gtk-justify-fill";

			case __JUSTIFY_LEFT:
				return "gtk-justify-left";

			case __JUSTIFY_RIGHT:
				return "gtk-justify-right";

			case __LEAVE_FULLSCREEN:
				return "gtk-leave-fullscreen";

			case __MEDIA_FORWARD:
				return "gtk-media-forward";

			case __MEDIA_NEXT:
				return "gtk-media-next";

			case __MEDIA_PAUSE:
				return "gtk-media-pause";

			case __MEDIA_PLAY:
				return "gtk-media-play";

			case __MEDIA_PREVIOUS:
				return "gtk-media-previous";

			case __MEDIA_RECORD:
				return "gtk-media-record";

			case __MEDIA_REWIND:
				return "gtk-media-rewind";

			case __MEDIA_STOP:
				return "gtk-media-stop";

			case __MISSING_IMAGE:
				return "gtk-missing-image";

			case __NETWORK:
				return "gtk-network";

			case __NEW:
				return "gtk-new";

			case __NO:
				return "gtk-no";

			case __OK:
				return "gtk-ok";

			case __OPEN:
				return "gtk-open";

			case __PASTE:
				return "gtk-paste";

			case __PREFERENCES:
				return "gtk-preferences";

			case __PRINT:
				return "gtk-print";

			case __PRINT_PREVIEW:
				return "gtk-print-preview";

			case __PROPERTIES:
				return "gtk-properties";

			case __QUIT:
				return "gtk-quit";

			case __REDO:
				return "gtk-redo";

			case __REFRESH:
				return "gtk-refresh";

			case __REMOVE:
				return "gtk-remove";

			case __REVERT_TO_SAVED:
				return "gtk-revert-to-saved";

			case __SAVE:
				return "gtk-save";

			case __SAVE_AS:
				return "gtk-save-as";

			case __SELECT_COLOR:
				return "gtk-select-color";

			case __SELECT_FONT:
				return "gtk-select-font";

			case __SORT_ASCENDING:
				return "gtk-sort-ascending";

			case __SORT_DESCENDING:
				return "gtk-sort-descending";

			case __SPELL_CHECK:
				return "gtk-spell-check";

			case __STOP:
				return "gtk-stop";

			case __STRIKETHROUGH:
				return "gtk-strikethrough";

			case __UNDELETE:
				return "gtk-undelete";

			case __UNDERLINE:
				return "gtk-underline";

			case __UNDO:
				return "gtk-undo";

			case __UNINDENT:
				return "gtk-unident";

			case __YES:
				return "gtk-yes";

			case __ZOOM_100:
				return "gtk-zoom-100";

			case __ZOOM_FIT:
				return "gtk-zoom-fit";

			case __ZOOM_IN:
				return "gtk-zoom-in";

			case __ZOOM_OUT:
				return "gtk-zoom-out";
		}
		return "";
	}

	this.PutSecondaryStream = function(OwnerID, Target, Value, ValueBuffer) {
		var element = this.Controls[OwnerID];

		if (!element) {
			console.warn("PutSecondaryStream: Unknown object RID: " + OwnerID);
			return;
		}
		var cls_id = element.ConceptClassID;
		if (!cls_id)
			cls_id = parseInt(element.getAttribute("ConceptClassID"));

		if (cls_id == CLASS_IMAGE) {
			element.src = this.ResourceRoot + "/" + this.AdjustStock(Target) + ".png";
			var size = parseInt(Value);
			switch (size) {
				case 0:
				case 1:
				case 2:
					element.style.width = "16px";
					element.style.height = "16px";
					break;
				case 3:
					element.style.width = "24px";
					element.style.height = "24px";
					break;
				case 6:
					element.style.width = "32px";
					element.style.height = "32px";
					break;
				default:
					if (size > 7) {
						element.style.width = "" + size + "px";
						element.style.height = "" + size + "px";
					} else {
						element.style.width = "32px";
						element.style.height = "32px";
					}
					break;
			}
		} else
		if (cls_id == 1012) {
			if (!element.ConceptCache)
				element.ConceptCache = [ ];
			element.ConceptCache[Target] = ValueBuffer;
		}
	}

	this.GetComboIndex = function(node) {
		if (node)
			node = node.parentNode;
		
		if (node) {
			var parent = node.parentNode;
			if (parent)
				return Array.prototype.indexOf.call(parent.childNodes, node);
		}
		return -1;
	}

	this.SelectComboItem = function(OwnerID, element) {
		var label = document.getElementById("l" + OwnerID);
		var control = this.Controls[OwnerID];//document.getElementById("r" + OwnerID);
		if ((label) && (control)) {
			if (element) {
				label.innerHTML = element.innerHTML + '&nbsp;<span class="caret RCaret"></span>';
			} else {
				label.innerHTML = '&nbsp;<span class="caret RCaret"></span>';
				control.setAttribute("ConceptRowIndex", "-1");
				return;
			}

			var index = self.GetComboIndex(element);
			var prev_index_attr = control.getAttribute("ConceptRowIndex");
			var prev_index = -1;
			if (prev_index_attr)
				prev_index = parseInt(prev_index_attr);

			control.setAttribute("ConceptRowIndex", index);
			if ((control.ConceptChanged) && (index != prev_index))
				self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_CHANGED, "" + index, 0);
		}
	}

	this.SelectComboItemText = function(OwnerID, element) {
		var label = document.getElementById("l" + OwnerID);
		var control = this.Controls[OwnerID];//document.getElementById("r" + OwnerID);
		if ((label) && (control)) {
			var prec_value = label.value;
			if (element) {
				label.value = element.getAttribute("ConceptText");
			} else {
				label.value = "";
				control.setAttribute("ConceptRowIndex", "-1");
				return;
			}

			var index = self.GetComboIndex(element);
			control.setAttribute("ConceptRowIndex", index);

			if ((control.ConceptChanged) && (prec_value != label.value))
				self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_CHANGED, label.value, 0);
		}
	}

	this.GetComboElement = function(OwnerID, index) {
		if (index >= 0) {
			var container = document.getElementById("c" + OwnerID);
			if (container) {
				var li = container.children[index];
				if (li)
					return li.children[0];
			}
		}
		return null;
	}

	this.SetXAlign = function(element, f) {
		if (f < 0.1)
			element.style.textAlign = "left";
		else
		if (f > 0.9)
			element.style.textAlign = "right";
		else
		if (f < 0.0)
			element.style.textAlign = "justify";
		else
			element.style.textAlign = "center";
	}

	this.SetYAlign = function(element, f) {
		if (f < 0.1)
			element.style.verticalAlign = "top";
		else
		if (f > 0.9)
			element.style.verticalAlign = "bottom";
		else
			element.style.verticalAlign = "middle";
	}

	this.ParseFont = function(element, val) {
		var val_lower = val.toLowerCase();
		if (val_lower.indexOf("bold") >= 0) {
			element.style.fontWeight = "bold";
			val = val.replace("bold", "");
		}
		if (val_lower.indexOf("italic") >= 0) {
			element.style.fontStyle = "italic";
			val = val.replace("italic", "");
		}
		if (val_lower.indexOf("underline") >= 0) {
			element.style.textDecoration = "underline";
			val = val.replace("underline", "");
		}
		val = val.trim();
		if ((val.length) && (val.toLowerCase() != "normal") && (val.toLowerCase() != "regular")) {
			if (val.indexOf(" ") < 0) {
				element.style.fontFamily = val;
				return;
			} else
				element.style.font = val;
		}
	}

	this.UpdateCombo = function(element, e, val, treeview, tr, column, OwnerID) {
		if (e) {
			if (e.id)
				e.id = "";
			var is_ul = (e.nodeName.toLowerCase() == "ul");
			for (var i = 0; i < e.children.length; i++) {
				var child = e.children[i];
				this.UpdateCombo(element, child, val, treeview, tr, column, OwnerID);
				if (is_ul) {
					if (child.firstChild) {
						var content = child.firstChild.firstChild;
						if (content.innerHTML == val)
							element.firstChild.innerHTML = child.firstChild.innerHTML + '&nbsp;<span class="caret RCaret"></span>';
						child.onclick = function() {
							element.firstChild.innerHTML = this.firstChild.innerHTML + '&nbsp;<span class="caret RCaret"></span>';
							if (treeview.ConceptStartEditing) {
								self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_STARTEDITING, "" + self.GetPath(tr, "tr") + "/" + column, 0);
							}
							if (treeview.ConceptEndEditing)
								self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_ENDEDITING, "" + this.firstChild.firstChild.innerHTML, 0);
							return false;
						}
					}
				}
			}
		}
	}

	this.CreateTreeItem = function(OwnerID, body, element, treeData, cls_id, parent_level) {
		var td, tr, li, imageColumn = -1, textColumn = -1, markupColumn = -1, tooltipColumn = -1, itemWidth = -1, itemOrientation = -1, itemColumns = -1;
		if (cls_id == CLASS_TREEVIEW) {
			tr = document.createElement("tr");
			tr.setAttribute("ConceptTreeLevel", parent_level);
			tr.setAttribute("ConceptToggled", "1");
			tr.className = "RTreeViewItem";
			tr.onmousedown = function() {
				element.setAttribute("ConceptRowIndex", self.GetPath(tr, "tr"));
			};
		} else
		if (cls_id == CLASS_ICONVIEW) {
			tr = document.createElement("li");
			tr.onmousedown = function() {
				element.setAttribute("ConceptRowIndex", Array.prototype.indexOf.call(body.childNodes, tr));
			};
			tr.className = "RIconViewItem";
			var attr = element.getAttribute("ConceptImageColumn");
			if (attr)
				imageColumn = parseInt(attr);
			attr = element.getAttribute("ConceptTextColumn");
			if (attr)
				textColumn = parseInt(attr);
			attr = element.getAttribute("ConceptMarkupColumn");
			if (attr)
				markupColumn = parseInt(attr);
			attr = element.getAttribute("ConceptTooltipColumn");
			if (attr)
				tooltipColumn = parseInt(attr);
			attr = element.getAttribute("ConceptItemWidth");
			if (attr)
				itemWidth = parseInt(attr);
			attr = element.getAttribute("ConceptVertical");
			if (attr)
				itemOrientation = parseInt(attr);
			attr = element.getAttribute("ConceptColumns");
			if (attr)
				itemColumns = parseInt(attr);
		} else {
			var li = document.createElement("li");

			tr = document.createElement("a");
			tr.href = "#";
			tr.className = "RComboBoxItem";

			if (cls_id == CLASS_COMBOBOXTEXT) {
				var attr = body.getAttribute("ConceptTextColumn");
				if (attr)
					textColumn = parseInt(attr);
				else
					textColumn = 0;

				tr.onclick = function() {
					self.SelectComboItemText(OwnerID, this);
					return false;
				}
			} else {
				tr.onclick = function() {
					self.SelectComboItem(OwnerID, this);
					return false;
				}
			}
			li.appendChild(tr);
		}
		var control;
		var nval;
		var len  = treeData.Columns.length;
		var first = true;
		var cells = [];
		var idx = 0;
		for (var i = 0; i < len; i++) {
			var val = treeData.Post[idx++];
			var column = treeData.Columns[i];
			var is_markup = false;
			if ((column) && (column.Property)) {
				var cell;
				if (column.Column == -1)
					cell = tr;
				else
					cell = cells[column.Column];

				if (cell) {
					switch (column.Name.toLowerCase()) {
						case "background":
						case "cell-background":
							cell.style.backgroundColor = val;
							break;
						case "foreground":
						case "cell-foreground":
							cell.style.color = val;
							break;
						case "font":
							this.ParseFont(cell, val);
							break;
						case "fontsize":
							cell.style.fontSize = "" + parseInt(val) + "px";
							break;
						case "family":
							cell.style.fontFamily = val;
							break;
						case "variant":
							cell.style.fontVariant = val;
							break;
						case "visible":
							if (parseInt(val))
								cell.style.display = "none";
							else
								cell.style.display = "";
							break;
						case "strikethrough":
							if (parseInt(val))
								cell.style.textDecoration = "line-through";
							else
								cell.style.textDecoration = "";
							break;
						case "underline":
							if (parseInt(val))
								cell.style.textDecoration = "underline";
							else
								cell.style.textDecoration = "";
							break;
						case "xalign":
						case "text-xalign":
							this.SetXAlign(cell, parseFloat(val));
							break;
						case "yalign":
						case "text-yalign":
							this.SetYAlign(cell, parseFloat(val));
							break;
						case "style":
							cell.style.cssText += val;
							break;
						case "width":
							cell.style.width = "" + val + "px";
							break;
						case "height":
							cell.style.height = "" + val + "px";
							break;
						case "xpad":
							cell.style.paddingLeft = val;
							cell.style.paddingRight = val;
							break;
						case "ypad":
							cell.style.paddingTop = val;
							cell.style.paddingBottom = val;
							break;
						default:
							console.warn("Unsupported property: " + column.Name + " (value \"" + val + "\")");
					}
				}
			} else
			if ((column) && ((column.Type != 0) || (i == textColumn) || (i == imageColumn) || (i == markupColumn))) {
				if (!val)
					val = "";
				if (cls_id == CLASS_ICONVIEW) {
					if ((i != imageColumn) && (i != textColumn) && (i != markupColumn))
						continue;
					td = document.createElement("span");
					if (i == imageColumn) {
						td.className = "RIconViewData RIconViewDataIcon";
						if (itemOrientation == ORIENTATION_HORIZONTAL)
							td.className += " RIconViewItemIconHorizontal";
						else
							td.className += " RIconViewItemIconVertical";
					} else {
						td.className = "RIconViewData";
						if (itemOrientation == ORIENTATION_HORIZONTAL)
							td.className += " RIconViewItemHorizontal";
						else
							td.className += " RIconViewItemVertical";
						if (i == markupColumn)
							is_markup = true;
					}

				} else
				if (cls_id == CLASS_TREEVIEW) {
					td = document.createElement("td");
					if ((first) && (parent_level > 0))
						td.style.paddingLeft = "" + (parent_level * 30) + "px";
					first = false;
				} else {
					td = document.createElement("div");
					td.className = "RComboBoxColumn";
				}
				if (val) {
					var disabled = "";
					switch (column.Type) {
						case PERCENT_COLUMN:
							// progress
							nval = parseInt(val);
							td.innerHTML += '<div class="progress RProgressBar"><div class="progress-bar RProgressBarValue" role="progressbar" aria-valuenow="' + nval + '" aria-valuemin="0" aria-valuemax="100" style="width: ' + nval + '%;">' + nval + '%</div></div>';
							break;
						case CHECK_COLUMN:
							if (parseInt(val))
								td.innerHTML += '<input type="checkbox" checked="checked" disabled="disabled"/>';
							else
								td.innerHTML += '<input type="checkbox" disabled="disabled"/>';
							td.style.width = "0px";
							break;
						case RADIO_COLUMN | EDITABLE_COLUMN:
						case CHECK_COLUMN | EDITABLE_COLUMN:
							// check
							var inp = document.createElement("input");
							if (column.Type == (RADIO_COLUMN | EDITABLE_COLUMN))
								inp.setAttribute("type", "radio");
							else
								inp.setAttribute("type", "checkbox");
							if (parseInt(val))
								inp.checked = true;
							else
								inp.checked = false;
							td.appendChild(inp);
							td.style.width = "0px";
							inp.onchange = function() {
								if (element.ConceptStartEditing)
									self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_STARTEDITING, "" + self.GetPath(tr, "tr") + "/" + Array.prototype.indexOf.call(this.parentNode.parentNode.childNodes, this.parentNode), 0);
								if (element.ConceptEndEditing)
									self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_ENDEDITING, this.checked ? "1" : "0", 0);
							};
							break;
						case RADIO_COLUMN:
							// radio
							if (parseInt(val))
								td.innerHTML += '<input type="radio" checked="checked" disabled="disabled"/>';
							else
								td.innerHTML += '<input type="radio" disabled="disabled"/>';
							td.style.width = "0px";
							break;
						case IMAGE_COLUMN:
							var source = this.Controls[val];//document.getElementById("r" + parseInt(val));
							if (source) {
								var image = document.createElement("img");
								image.src = source.src;
								image.style.width = source.style.width;
								image.style.height = source.style.height;
								td.appendChild(image);
							}
							break;
						case COMBO_COLUMN:
							td.appendChild(document.createTextNode(val));
							break;
						case COMBO_COLUMN | EDITABLE_COLUMN:
							var combo = self.Controls[parseInt(treeData.Post[idx++])];
							if (combo) {
								var e = combo.cloneNode(true);
								self.UpdateCombo(e, e, val, element, tr, tr.children.length, OwnerID);
								e.style.display = "";
								e.style.width = "100%";
								td.appendChild(e);
							}
							break;
						case MARKUP_COLUMN:
						case MARKUP_COLUMN | EDITABLE_COLUMN:
							// markup
							// do not escape here
							td.innerHTML += val.replace("\n", "<br/>");
							break;
						case NORMAL_COLUMN:
							if (is_markup)
								td.innerHTML += val.replace("\n", "<br/>");
							else
								td.appendChild(document.createTextNode(val));	
							break;
						case NORMAL_COLUMN | EDITABLE_COLUMN:
							var edit = document.createElement("input");
							edit.setAttribute("type", "text");
							edit.value = val;
							edit.className = "RTreeViewEdit";
							td.appendChild(edit);
							edit.oninput = function() {
								if (element.ConceptStartEditing)
									self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_STARTEDITING, "" + self.GetPath(tr, "tr") + "/" + Array.prototype.indexOf.call(this.parentNode.parentNode.childNodes, this.parentNode), 0);
								if (element.ConceptEndEditing)
									self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_ENDEDITING, "" + this.value, 0);
							};
							break;
						default:
							td.appendChild(document.createTextNode(val));
							break;
					}
					if ((cls_id == CLASS_COMBOBOXTEXT) && (i == textColumn))
						tr.setAttribute("ConceptText", val);
				}

				if ((cls_id == CLASS_ICONVIEW) && (column.Type == IMAGE_COLUMN) && (tr.children.length)) {
					tr.insertBefore(td, tr.children[0]);
				} else
					tr.appendChild(td);

				if (tooltipColumn >= 0) {
					var tooltip_data = treeData.Post[tooltipColumn];
					if (tooltip_data) {
						tr.setAttribute("title", tooltip_data);
						tr.setAttribute("data-toggle", "tooltip");
						$(tr).tooltip();
					}
				}

				if (itemWidth > 0)
					tr.style.width = "" + itemWidth + "px";
				if (itemColumns > 0) {
					tr.style.width = "100%";
					if (tr.children.length >= itemColumns)
						tr.style.clear = "both";
				}

				var properties = treeData.ColProperties;
				if ((properties) && (properties[cells.length])) {
					var props = properties[cells.length];
					for (var j = 0; j < props.length; j++) {
						switch (props[j].Property) {
							case P_VISIBLE:
								if (parseInt(props[j].Value) == 0)
									td.style.display = "none";
								break;
							case P_ALIGNMENT:
							case P_REORDERABLE:
							case P_RESIZABLE:
							case P_SORTINDICATOR:
							case P_SORTINDICATORTYPE:
							case P_SENSITIVE:
							case P_SIZING:
							case P_CLICKABLE:
							case P_EXPAND:
							case P_WRAP:
								// not supported yet
								break;
							case P_FIXEDWIDTH:
								td.style.width = "" + props[j].Value + "px";
								break;
							case P_MINWIDTH:
								td.style.minWidth = "" + props[j].Value + "px";
								break;
							case P_MAXWIDTH:
								td.style.maxWidth = "" + props[j].Value + "px";
								break;
							case P_BACKCOLOR:
								td.style.backgroundColor = this.doColorString(parseInt(props[j].Value));
								break;
							case P_FORECOLOR:
								td.style.color = this.doColorString(parseInt(props[j].Value));
								break;
							case P_XALIGN:
								this.SetXAlign(td, parseFloat(props[j].Value));
								break;
							case P_YALIGN:
								this.SetYAlign(td, parseFloat(props[j].Value));
								break;
							case P_XPADDING:
								td.style.paddingLeft = "" + props[j].Value + "px";
								td.style.paddingRight = "" + props[j].Value + "px";
								break;
							case P_YPADDING:
								td.style.paddingTop = "" + props[j].Value;
								td.style.paddingBottom = "" + props[j].Value;
								break;
							case P_FONTNAME:
								this.ParseFont(td, props[j].Value);
								break;
						}
					}
				}
				cells.push(td);
			} else
				cells.push({"Placeholder": 1});
		}
		treeData.Post = [];
		if ((cls_id == CLASS_TREEVIEW)  || (cls_id == CLASS_ICONVIEW))
			return tr;
		return li;
	}

	this.ExpandAll = function(table, expanded) {
		var children = table.children[1].children;
		for (var i = 0; i < children.length; i++) {
			var child = children[i];
			if ((child.getAttribute("ConceptCollapsible") == 1) || (child.getAttribute("data-row-parent-id"))) {
				if (expanded) {
					if (child.getAttribute("ConceptCollapsible") == 1)
						child.setAttribute("ConceptToggled", "0");
					if (child.getAttribute("data-row-parent-id"))
						child.style.display = "";
					var first_td = child.children[0];
					if (first_td) {
						var href = first_td.firstChild;
						if ((href) && (href.nodeName.toLowerCase() == "a"))
							href.className = "RTreeViewCollapse";
					}
				} else {
					if (child.getAttribute("ConceptCollapsible") == 1)
						child.setAttribute("ConceptToggled", "1");
					if (child.getAttribute("data-row-parent-id"))
						child.style.display = "none";
					var first_td = child.children[0];
					if (first_td) {
						var href = first_td.firstChild;
						if ((href) && (href.nodeName.toLowerCase() == "a"))
							href.className = "RTreeViewExpand";
					}
				}
			}
		}
	}

	this.ExpandChildren = function(children, parent, expanded) {
		for (var i = 0; i < children.length; i++) {
			var child = children[i];
			if (child.getAttribute("data-row-parent-id") == parent) {
				if (expanded) {
					if (child.getAttribute("ConceptCollapsible") == 1)
						child.setAttribute("ConceptToggled", "0");
					if (child.getAttribute("data-row-parent-id"))
						child.style.display = "";
				} else {
					if (child.getAttribute("ConceptCollapsible") == 1)
						child.setAttribute("ConceptToggled", "1");
					if (child.getAttribute("data-row-parent-id"))
						child.style.display = "none";
				}
			}
		}
	}

	this.GetRow = function(table, parent, index, expanded) {
		var tbody = table.children[1];
		var children = tbody.children;
		for (var i = 0; i < children.length; i++) {
			var child = children[i];
			if (((!parent) && (!child.getAttribute("data-row-parent-id"))) || (child.getAttribute("data-row-parent-id") == parent)) {
				if (expanded) {
					if (child.getAttribute("ConceptCollapsible") == 1)
						child.setAttribute("ConceptToggled", "0");
					if (child.getAttribute("data-row-parent-id"))
						child.style.display = "";
					var first_td = child.children[0];
					if (first_td) {
						var href = first_td.firstChild;
						if ((href) && (href.nodeName.toLowerCase() == "a"))
							href.className = "RTreeViewCollapse";
					}
				} else {
					if (child.getAttribute("ConceptCollapsible") == 1)
						child.setAttribute("ConceptToggled", "1");
					if (child.getAttribute("data-row-parent-id"))
						child.style.display = "none";
					var first_td = child.children[0];
					if (first_td) {
						var href = first_td.firstChild;
						if ((href) && (href.nodeName.toLowerCase() == "a"))
							href.className = "RTreeViewExpand";
					}
				}
				this.HideRecursive(tbody, child.getAttribute("data-row-id"), expanded);
				if (!index)
					return child.getAttribute("data-row-id");
				index--;
			}
		}
		return -1;
	}

	this.Expand = function(table, path, expanded) {
		if ((path) && (path.length)) {
			var parent = "";
			for (var i = 0; i < path.length; i++) {
				parent = this.GetRow(table, parent, parseInt(path[i]), expanded);
				if (parent < 0)
					break;
			}
		}
	}

	this.TreeSearch = function(element, str) {
		if (!element.ConceptSearchColumn)
			element.ConceptSearchColumn = 0;
		if (element.ConceptSearchColumn >= 0) {
			var children = element.firstChild.children[1].children;
			str = str.toLowerCase();
			for (var i = 0; i < children.length; i++) {
				var child = children[i];
				child.classList.remove("RTreeViewSelectedItem");
				if (str) {
					var target = child.children[element.ConceptSearchColumn];
					var text = target.textContent || target.innerText;
					if (text.toLowerCase().indexOf(str) >= 0)
						child.classList.add("RTreeViewSelectedItem");
				}
			}
		}
	}

	this.MSG_CUSTOM_MESSAGE = function(OwnerID, MSG_ID, Target, Value, SendMessageFunction) {
		var t, len, val, treeData, body, tr, td, index, child, type, count;
		var element = this.Controls[OwnerID];

		if (!element) {
			console.warn("MSG_CUSTOM_MESSAGE (" + MSG_ID + "): Unknown object RID: " + OwnerID);
			return;
		}
		var cls_id = element.ConceptClassID;
		if (!cls_id)
			cls_id = parseInt(element.getAttribute("ConceptClassID"));

		switch (MSG_ID) {
			case MSG_CUSTOM_MESSAGE1:
				switch (cls_id) {
					case CLASS_TREEVIEW:
						type = parseInt(Value);
						if (type) {
							var header = document.getElementById("l" + OwnerID);
							if (header) {
								td = document.createElement("th");
								if ((type == (CHECK_COLUMN | EDITABLE_COLUMN)) || (type == (RADIO_COLUMN | EDITABLE_COLUMN)) || (type == CHECK_COLUMN) || (type == RADIO_COLUMN))
									td.style.width = "0px";

								var ahref = document.createElement("a");
								ahref.appendChild(document.createTextNode(Target));
								td.appendChild(ahref);
								header.appendChild(td);
								ahref.href = "#";
								ahref.onclick = function() {
									var index = Array.prototype.indexOf.call(td.parentNode.childNodes, td);
									if (element.ConceptHeaderClicked)
										self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_HEADERCLICKED, "" + index, 0);
									element.ConceptColumnIndex = index;
									return false;
								};
							}
						}

						treeData = this.TreeData[OwnerID];
						if (treeData)
							treeData.Columns.push({ "Name": Target, "Type": type });
						break;
					case CLASS_COMBOBOX:
					case CLASS_COMBOBOXTEXT:
					case CLASS_ICONVIEW:
						treeData = this.TreeData[OwnerID];
						if (treeData) {
							type = parseInt(Target);
							count = parseInt(Value);
							for (var i = 0; i < count; i++)
								treeData.Columns.push({ "Name": "", "Type": type });
						}
						break;
					case CLASS_PROGRESSBAR:
						var label = document.getElementById("l" + OwnerID);
						if (label) {
							label.style.width = "100%";
							label.setAttribute("aria-valuenow", "100");
						}
						break;
					case CLASS_FIXED:
						var element2 = this.Controls[parseInt(Target)];
						if (element2) {
							var coords = parseInt(Value);
							element2.style.position = "absolute";
							element2.style.left = "" + Math.floor(coords / 0x10000);
							element2.style.top = "" + (coords % 0x10000);
						}
						break;
					case CLASS_TEXTVIEW:
						var tag = this.Controls[Target];
						if ((tag) && (tag.ConceptClassID == CLASS_TEXTTAG)) {
							var p = document.createElement(p);
							p.className = "RTextViewTag";
							p.style.cssText = tag.cssText;
							p.appendChild(document.createTextNode(Value));
							element.appendChild(p);
						} else
							element.appendChild(document.createTextNode(Value));
						break;
					case CLASS_IMAGE:
						var v = parseInt(Value);
						var w = v/0xFFFF;
						var h = v%0xFFFF;
						element.style.width = "" + w + "px";
						element.style.height = "" + h + "px";
						break;
					case CLASS_MENU:
						setTimeout(function (e) {
							e = e || window.event;
							element.parentNode.style.position = "absolute";
							if (e) {
								element.parentNode.style.top = "" + e.clientY + "px";
								element.parentNode.style.left = "" + e.clientX + "px";
							} else
							if (!element.parentNode.style.top) {
								element.parentNode.style.top = "100px";
								element.parentNode.style.left = "100px";
							}
							element.parentNode.className = "open";
						}, 300);
						break;
				}
				break;
			case MSG_CUSTOM_MESSAGE2:
				t = parseInt(Target);
				switch (cls_id) {
					case CLASS_TREEVIEW:
					case CLASS_COMBOBOX:
					case CLASS_COMBOBOXTEXT:
					case CLASS_ICONVIEW:
						//if (cls_id == CLASS_ICONVIEW)
						//	body = element;
						//else
						body = document.getElementById("c" + OwnerID);
						treeData = this.TreeData[OwnerID];
						if (t == 0) {
							if ((body) && (treeData)) {
								tr = this.CreateTreeItem(OwnerID, body, element, treeData, cls_id, 0);
								treeData.Post = [];
								index = parseInt(Value);
								child = 0;
								if (index >= 0) {
									if (cls_id == CLASS_TREEVIEW)
										child = this.GetNodeByPath(body, Value);
									else
										child = body.children[parseInt(Value)];
								}

								if (child)
									body.insertBefore(tr, child);
								else
									body.appendChild(tr);

								if (cls_id == CLASS_TREEVIEW) {
									var next_id = body.ConceptRowID;
									if (!body.ConceptRowID)
										body.ConceptRowID = 1;
									element.ConceptColumnIndex = -1;
									tr.setAttribute("data-row-id", body.ConceptRowID++);
								}
								if (element.ConceptDraggable)
									$(tr).draggable({ 
										revert: "invalid", 
										cancel:false, 
										helper: "clone",
										zIndex: 10000000,
										start: function(event, ui) {
											if (element.ConceptDragBegin)
												self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_DRAGBEGIN, "", 0);
										},
										stop: function(event, ui) {
											if (element.ConceptDragEnd)
												self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_DRAGEND, "", 0);
										}
									});
							}
						} else
						if (t == 1)
							body.innerHTML = "";
						else
						if (t == 2) {
							// delete item (value)
							if (cls_id == CLASS_TREEVIEW) {
								child = this.GetNodeByPath(body, Value);
								this.DeleteNode(child);
							} else {
								child = body.children[parseInt(Value)];
								if (child)
									body.removeChild(child);
							}
						} else
						if (t == 3) {
							// clear columns
							if (treeData)
								treeData.Columns = [];
							body.innerHTML = "";
						} else
						if (t == 4) {
							// remove column
							treeData.Columns[parseInt(Value)] = null;
							//treeData.Columns.splice(parseInt(Value), 1);
						}
						break;
					case CLASS_NOTEBOOK:
						// set caption
						var href = document.getElementById("a_tab" + Target);
						if (href) {
							href.innerHTML = "";
							href.appendChild(document.createTextNode(Value));
						}
						break;
					case CLASS_TEXTVIEW:
						var tag = this.Controls[Target];
						var position = parseInt(this.POST_STRING);
						var text = element.textContent || element.innerText || "";
						var p;

						if ((tag) && (tag.ConceptClassID == CLASS_TEXTTAG)) {
							p = document.createElement(p);
							p.className = "RTextViewTag";
							p.style.cssText = tag.cssText;
							p.appendChild(document.createTextNode(Value));
						} else
							p = document.createTextNode(Value);

						if (position <= 0)
							element.insertBefore(p, element.firstNode);
						else
						if (position >= text.length)
							element.appendChild(p);
						else {
							var p1 = text.substring(0, position);
							var p2 = text.substring(position);
							element.innerHTML = "";
							element.appendChild(document.createTextNode(p1));
							element.appendChild(p);
							element.appendChild(document.createTextNode(p2));
						}
						break;
					case 1000:
						if (element.ConceptEditor) {
							var msg = parseInt(this.POST_STRING)
							switch (msg) {
								// 2014 = SCI_SETSAVEPOINT
								case 2014:
									element.ConceptHash = "" + CryptoJS.MD5(element.ConceptEditor.getSession().getValue());
									break;
								// 2024 = SCI_GOTOLINE
								case 2024:
									// to do
									var line = parseInt(Target)
									//element.ConceptEditor.resize(true);
									element.ConceptEditor.scrollToLine(line, true, true, function () {});
									element.ConceptEditor.gotoLine(line, 0, true);
									break;
								// 2043 = SCI_MARKERADD
								case 2043:
									var line = parseInt(Target)
									var type = parseInt(Value);
									var Range = ace.require('ace/range').Range;
									var marker;
									if (type == 0)
										marker = element.ConceptEditor.getSession().addMarker(new Range(line, 0, line + 1, 0), "CodeError", "line")
									else {
										element.ConceptEditor.getSession().setBreakpoint(line, "CodeBreakpoint");
										marker = { };
									}
									marker.ConceptLine = line;
									marker.ConceptType = type;
									element.ConceptMarkers.push(marker);
									break;
								// 2044 = SCI_MARKERDELETE
								case 2044:
									var line = parseInt(Target)
									var type = parseInt(Value);
									for (var i = 0; i < element.ConceptMarkers.length; i++) {
										var marker = element.ConceptMarkers[i];
										if ((marker) && (marker.ConceptLine == line) && (marker.ConceptType == type)) {
											if (type == 0)
												element.ConceptEditor.getSession().removeMarker(marker);
											else
												element.ConceptEditor.getSession().clearBreakpoint(line);
											element.ConceptMarkers[i] = null;
										}
									}
									break;
								// 2045 = SCI_MARKERDELETEALL
								case 2045:
									var type = parseInt(Target);
									for (var i = 0; i < element.ConceptMarkers.length; i++) {
										var marker = element.ConceptMarkers[i];
										if ((marker) && (marker.ConceptType == type)) {
											if (type == 0)
												element.ConceptEditor.getSession().removeMarker(marker);
											else
												element.ConceptEditor.getSession().clearBreakpoint(marker.ConceptLine);
											element.ConceptMarkers[i] = null;
										}
									}
									//element.ConceptMarkers = [ ];
									break;
								// 2176 = SCI_UNDO
								case 2176:
									element.ConceptEditor.undo();
									break;
								// 2011 = SCI_REDO
								case 2011:
									element.ConceptEditor.redo();
									break;
								// 2178 = SCI_COPY
								case 2178:
									var text = element.ConceptEditor.getCopyText();
									if (text.length)
										window.prompt("Copy to clipboard: Ctrl+C on PC/Cmd+C on Mac, Enter", text);
									break;
							}
						}
						break;
				}
				break;
			case MSG_CUSTOM_MESSAGE3:
				t = parseInt(Target);
				switch (cls_id) {
					case CLASS_ICONVIEW:
					case CLASS_TREEVIEW:
					case CLASS_COMBOBOX:
					case CLASS_COMBOBOXTEXT:
						if (t == 0) {
							var treeData = this.TreeData[OwnerID];
							if (treeData)
								treeData.Post.push(Value);
						}
						break;
					case CLASS_TEXTVIEW:
						var obj = this.Controls[Value];
						if (obj) {
							var t = parseInt(Target);
							if (t == -1)
								element.appendChild(obj.cloneNode(true));
							else
							if (t == -2)
								element.appendChild(obj);
							else {
								var text = element.textContent || element.innerText || "";
								if (t == 0)
									element.insertBefore(obj, element.firstNode);
								else
								if (t >= text.length)
									element.appendChild(obj);
								else {
									var p1 = text.substring(0, position);
									var p2 = text.substring(position);
									element.innerHTML = "";
									element.appendChild(document.createTextNode(p1));
									element.appendChild(obj);
									element.appendChild(document.createTextNode(p2));
								}
							}
						}
						break;
					case CLASS_CLIENTCHART:
						var set_data_function = function() {
							var data = google.visualization.arrayToDataTable(JSON.parse(Value));
							var chart;
							switch (Target.toLowerCase()) {
								case "pie":
									chart = new google.visualization.PieChart(element);
									break;
								case "bar":
									chart = new google.visualization.BarChart(element);
									break;
								case "area":
									chart = new google.visualization.AreaChart(element);
									break;
								case "bubble":
									chart = new google.visualization.BubbleChart(element);
									break;
								case "candlestick":
									chart = new google.visualization.CandlestickChart(element);
									break;
								case "column":
									chart = new google.visualization.ColumnChart(element);
									break;
								case "combo":
									chart = new google.visualization.ComboChart(element);
									break;
								case "donut":
									chart = new google.visualization.DonutChart(element);
									break;
								case "scatter":
									chart = new google.visualization.ScatterChart(element);
									break;
								case "steppedarea":
									chart = new google.visualization.SteppedChart(element);
									break;
								case "gauge":
									chart = new google.visualization.Gauge(element);
									break;
								case "gantt":
									chart = new google.visualization.Gantt(element);
									break;
								case "geo":
									chart = new google.visualization.GeoChart(element);
									break;
								case "histogram":
									chart = new google.visualization.Histogram(element);
									break;
								case "line":
									chart = new google.visualization.LineChart(element);
									break;
								case "map":
									chart = new google.visualization.Map(element);
									break;
								case "org":
									chart = new google.visualization.OrgChart(element);
									break;
								case "table":
									chart = new google.visualization.Table(element);
									break;
								case "timeline":
									chart = new google.visualization.Timeline(element);
									break;
								case "treemap":
									chart = new google.visualization.TreeMap(element);
									break;
								case "wordtree":
									chart = new google.visualization.WordTree(element);
									break;
							}
							if (chart)
								chart.draw(data, element.ConceptOptions);
							else
								console.warn("Unknown chart type: " + Target);
						}
						if (js_flags["charts"] == 2)
							set_data_function();
						else {
							if (js_flags["charts_pending"])
								js_flags["charts_pending"].push(set_data_function);
							else
								js_flags["charts_pending"] = [ set_data_function ];
						}
						break;
					case CLASS_HTMLSNAP:
						if (element.ConceptHTMLClass) {
							var set_name = element.ConceptHTMLClass + "Set";
							var fn = window[set_name];
							if (fn) {
								try {

									fn({ "Client": this, "RID": "" + OwnerID, "Object": element }, Target, JSON.parse(Value));
								} catch (e) {
									console.error("Error in " + set_name);
									console.error(e);
								}
							} else {
								console.error("Function " + set_name + " is not defined.");
							}
						} else {
							console.error("Cannot use Set/Get for unregistered RSnap objects");
						}
						break;
					case CLASS_HTMLAPP:
						if (this.UISet) {
							try {
								this.UISet({ "Client": this, "event": this.UIEvent, "RID": "" + OwnerID, "Object": element }, Target, JSON.parse(Value));
							} catch (e) {
								console.error("Error in UISet");
								console.error(e);
							}							
						} else
							console.log("client.UISet is not set");
						break;
					case 1000:
						if (element.ConceptEditor) {
							var msg = parseInt(this.POST_STRING)
							switch (msg) {
								case 2170:
									element.ConceptEditor.replace(Value);
									break;
								
							}
						}
						SendMessageFunction("" + OwnerID, MSG_ID, Target, Value, 0);
						break;
				}
				break;
			case MSG_CUSTOM_MESSAGE4:
				switch (cls_id) {
					case CLASS_TREEVIEW:
						body = document.getElementById("c" + OwnerID);
						var target_index = parseInt(Target);
						var parent = this.GetNodeByPath(body, Value);

						var parent2 = null;
						if (target_index >= 0)
							parent2 = this.GetNodeByPath(body, Value + ":" + target_index);

						if (parent) {
							treeData = this.TreeData[OwnerID];
							if ((body) && (treeData)) {
								tr = this.CreateTreeItem(OwnerID, body, element, treeData, cls_id, Value.split(":").length);
								if (element.ConceptDraggable)
									$(tr).draggable({ 
										revert: "invalid", 
										cancel:false, 
										helper: "clone",
										zIndex: 10000000,
										start: function(event, ui) {
											if (element.ConceptDragBegin)
												self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_DRAGBEGIN, "", 0);
										},
										stop: function(event, ui) {
											if (element.ConceptDragEnd)
												self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_DRAGEND, "", 0);
										}
									});
								treeData.Post = [];
								if (parent2)
									body.insertBefore(tr, parent2);
								else
									body.insertBefore(tr, this.GetLastNode(parent));

								if (parent.getAttribute("ConceptToggled") === "1")
									tr.style.display = "none";
								else
									tr.style.display = parent.style.display;
								if (cls_id == CLASS_TREEVIEW) {
									var next_id = body.ConceptRowID;
									if (!body.ConceptRowID)
										body.ConceptRowID = 1;

									tr.setAttribute("data-row-id", body.ConceptRowID++);
									tr.setAttribute("data-row-parent-id", parent.getAttribute("data-row-id"));

									this.UpdateCollapsible(element, parent, true);
								}
							}
						}
						break;
					case CLASS_NOTEBOOK:
						var href = document.getElementById("a_tab" + Target);
						var header = this.Controls[Value];
						if ((href) && (header)) {
							href.innerHTML = "";
							header.style.display = "";
							href.appendChild(header);
						}
						break;
					case CLASS_HTMLSNAP:
					case CLASS_HTMLAPP:
						if ((element.ConceptHTMLClass) || (this.UISet)) {
							var fn;
							var get_name;
							var eval_result = "";
							if (cls_id == CLASS_HTMLAPP) {
								get_name = "this.UIGet";
								fn = this.UIGet;
							} else {
								get_name = element.ConceptHTMLClass + "Get";
								fn = window[get_name];
							}
							if (fn) {
								try {
									eval_result = fn({ "Client": this, "event": this.UIEvent, "RID": "" + OwnerID, "Object": element}, Target, Value);
								} catch (e) {
									console.error("Error in " + get_name);
									console.error(e);
								}
							} else {
								console.error("Function " + get_name + " is not defined.");
							}

							switch (typeof eval_result) {
								case "string":
									self.SendMessage("" + OwnerID, MSG_ID, "0", eval_result, 0);
									break;
								case "number":
									self.SendMessage("" + OwnerID, MSG_ID, "0", "" + eval_result, 0);
									break;
								default:
									if (eval_result)
										self.SendMessage("" + OwnerID, MSG_ID, "1", JSON.stringify(eval_result), 0);
									else
										self.SendMessage("" + OwnerID, MSG_ID, "0", "", 0);
							}
						} else {
							self.SendMessage("" + OwnerID, MSG_ID, "0", "", 0);
							console.error("Cannot use Set/Get for unregistered RSnap/HTMLUI objects");
						}
						break;
				}
				break;
			case MSG_CUSTOM_MESSAGE5:
				if (cls_id == CLASS_TREEVIEW) {
					switch (parseInt(Target)) {
						case 0:
							// expand all
							this.ExpandAll(element.firstChild, true);
							break;
						case 1:
							// collapse all
							this.ExpandAll(element.firstChild, false);
							break;
						case 2:
						case 4:
							// expand to path
							this.Expand(element.firstChild, Value.split(":"), true);
							break;
						case 3:
							// collapse path
							this.Expand(element.firstChild, Value.split(":"), false);
							break;
					}
				} else
				if (cls_id == CLASS_HTMLSNAP) {
					if (element.ConceptHTMLClass) {
						var set_name = element.ConceptHTMLClass + "Set";
						var fn = window[set_name];
						if (fn) {
							try {
								fn({ "Client": this, "RID": "" + OwnerID, "Object": element }, Target, Value);
							} catch (e) {
								console.error("Error in " + set_name);
								console.error(e);
							}
						} else {
							console.error("Function " + set_name + " is not defined.");
						}
					} else {
						console.error("Cannot use Set/Get for unregistered RSnap objects");
					}
				} else
				if (cls_id == CLASS_HTMLAPP) {
					if (this.UISet) {
						try {
							this.UISet({ "Client": this, "event": this.UIEvent, "RID": "" + OwnerID, "Object": element }, Target, Value);
						} catch (e) {
							console.error("Error in UISet");
							console.error(e);
						}							
					} else
						console.log("client.UISet is not set");
				}				
				break;
			case MSG_CUSTOM_MESSAGE6:
				switch (cls_id) {
					case CLASS_TREEVIEW:
					case CLASS_ICONVIEW:
					case CLASS_COMBOBOX:
					case CLASS_COMBOBOXTEXT:
						// update item (value = position)
						body = document.getElementById("c" + OwnerID);
						treeData = this.TreeData[OwnerID];
						if ((body) && (treeData)) {
							var node;
							if (cls_id == CLASS_TREEVIEW)
							 	node = this.GetNodeByPath(body, Value);
							else
								node = body.children[parseInt(Value)];
							if (node) {
								tr = this.CreateTreeItem(OwnerID, body, element, treeData, cls_id, Value.split(":").length - 1);
								if (element.ConceptDraggable)
									$(tr).draggable({ 
										revert: "invalid", 
										cancel:false, 
										helper: "clone",
										zIndex: 10000000,
										start: function(event, ui) {
											if (element.ConceptDragBegin)
												self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_DRAGBEGIN, "", 0);
										},
										stop: function(event, ui) {
											if (element.ConceptDragEnd)
												self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_DRAGEND, "", 0);
										}
									});
								if (cls_id == CLASS_TREEVIEW) {
									tr.setAttribute("data-row-id", node.getAttribute("data-row-id"));
									var parent_id = node.getAttribute("data-row-parent-id");
									if (parent_id)
										tr.setAttribute("data-row-parent-id", parent_id);
									tr.setAttribute("ConceptTreeLevel", node.getAttribute("ConceptTreeLevel"));

									var val = node.getAttribute("ConceptToggled");
									if ((!val) || (val!="0"))
										tr.setAttribute("ConceptToggled", "1");
									else
										tr.setAttribute("ConceptToggled", "0");

									if (node.getAttribute("ConceptCollapsible") == 1)
										this.UpdateCollapsible(element, tr, true);

									tr.style.display = node.style.display;
								}
								body.replaceChild(tr, node);
							}
						}
						treeData.Post = [];
						break;
					case 1000:
						SendMessageFunction("" + OwnerID, MSG_ID, Target, Value, 0);
						break;
				}
				break;
			case MSG_CUSTOM_MESSAGE7:
				switch (cls_id) {
					case CLASS_TREEVIEW:
						var column = parseInt(this.POST_STRING);
						var prop = parseInt(Target);
						switch (prop) {
							case P_WIDGET:
								var header = this.Controls[parseInt(Value)];
								var treeHeader = document.getElementById("l" + OwnerID);
								if ((header) && (column >= 0) && (treeHeader)) {
									var child = treeHeader.children[column];
									if (child) {
										child.firstChild.innerHTML = "";
										child.firstChild.appendChild(header);
									}
								}
								break;
							case P_CAPTION:
								var treeHeader = document.getElementById("l" + OwnerID);
								if ((column > 0) && (treeHeader)) {
									var child = treeHeader.children[column];
									if (child) {
										child.innerHTML = "";
										child.firstChild.appendChild(document.createTextNode(Value));
									}
								}
								break;
							case P_SORTINDICATOR:
								var treeHeader = document.getElementById("l" + OwnerID);
								if ((column > 0) && (treeHeader)) {
									var child = treeHeader.children[column];
									if (child) {
										if (parseInt(Value))
											child.firstChild.classList.add("SortableColumn");
										else
											child.firstChild.classList.remove("SortableColumn");
									}
								}
								break;
							case P_SORTINDICATORTYPE:
								var treeHeader = document.getElementById("l" + OwnerID);
								if ((column > 0) && (treeHeader)) {
									var child = treeHeader.children[column];
									if (child) {
										if (parseInt(Value)) {
											child.firstChild.classList.remove("SortableAscending");
											child.firstChild.classList.add("SortableDescending");
										} else {
											child.firstChild.classList.remove("SortableDescending");
											child.firstChild.classList.add("SortableAscending");
										}
									}
								}
								break;
							default:
								var treeData = this.TreeData[OwnerID];
								if ((treeData) && (column >= 0)) {
									if (treeData.ColProperties[column])
										 treeData.ColProperties[column].push({ "Property": prop, "Value": Value, "Column": column });
									else
										 treeData.ColProperties[column]= [{ "Property": prop, "Value": Value, "Column": column }];
								}
								break;
						}
						break;
					case 1000:
						SendMessageFunction("" + OwnerID, MSG_ID, Target, Value, 0);
						break;
				}
				break;
			case MSG_CUSTOM_MESSAGE8:
				switch (cls_id) {
					case 1000:
						SendMessageFunction("" + OwnerID, MSG_ID, Target, Value, 0);
						break;
				}
				break;
			case MSG_CUSTOM_MESSAGE9:
				switch (cls_id) {
					case 1000:
						SendMessageFunction("" + OwnerID, MSG_ID, Target, Value, 0);
						break;
				}
				break;
			case MSG_CUSTOM_MESSAGE10:
				switch (cls_id) {
					case CLASS_TREEVIEW:
						var treeData = this.TreeData[OwnerID];
						if (parseInt(Target) < 0) {
							// row property
							treeData.Columns.push({ "Name": Value, "Type": 0, "Property": true, "Column" : -1});
						} else {
							// column property
							treeData.Columns.push({ "Name": Value, "Type": 0, "Property": true, "Column": parseInt(this.POST_STRING)});
						}
						break;
					case 1000:
						if (element.ConceptEditor) {
							var msg = parseInt(this.POST_STRING)
							switch (msg) {
								// 2159 = SCI_GETMODIFY
								case 2159:
									var text = "" + CryptoJS.MD5(element.ConceptEditor.getSession().getValue());
									if (element.ConceptHash == text)
										SendMessageFunction("" + OwnerID, MSG_ID, Target, "0", 0);
									else
										SendMessageFunction("" + OwnerID, MSG_ID, Target, "1", 0);
									break;
								// 2143 = SCI_GETSELECTIONSTART
								case 2143:
									var pos = element.ConceptEditor.selection.getCursor();
									if (pos)
										SendMessageFunction("" + OwnerID, MSG_ID, Target, "" + pos.row + ":" + pos.column, 0);
									else
										SendMessageFunction("" + OwnerID, MSG_ID, Target, "-1", 0);
									break;
								// 2166 = SCI_LINEFROMPOSITION
								case 2166:
									var pos = parseInt(Target);
									SendMessageFunction("" + OwnerID, MSG_ID, Target, "" + pos, 0);
									break;
								default:
									SendMessageFunction("" + OwnerID, MSG_ID, Target, Value, 0);
							}
						} else
							SendMessageFunction("" + OwnerID, MSG_ID, Target, Value, 0);
						break;
				}
				break;
			case MSG_CUSTOM_MESSAGE11:
				switch (cls_id) {
					case 1000:
						if (element.ConceptEditor) {
							var msg = parseInt(this.POST_STRING)
							switch (msg) {
								case 2367:
								case 2368:
									var flags = parseInt(Target);
									var range = element.ConceptEditor.find(Value, {
										backwards: (msg == 2368),
										wrap: false,
										caseSensitive: ((flags & 4) != 0),
										wholeWord: ((flags & 2) != 0),
										regExp: ((flags & 0x00200000) != 0)
									});
									var where = "-1";
									if ((range) && (range.start))
										where = "" + range.start.row + ":" + range.start.column;
									SendMessageFunction("" + OwnerID, MSG_ID, Target, where, 0);
									break;
								
							}
						}
						SendMessageFunction("" + OwnerID, MSG_ID, Target, Value, 0);
						break;
				}
				break;
			case MSG_CUSTOM_MESSAGE12:
				switch (cls_id) {
					case 1000:
						SendMessageFunction("" + OwnerID, MSG_ID, Target, Value, 0);
						break;
				}
				break;
			case MSG_CUSTOM_MESSAGE13:
				switch (cls_id) {
					case 1000:
						SendMessageFunction("" + OwnerID, MSG_ID, Target, Value, 0);
						break;
				}
				break;
			case MSG_CUSTOM_MESSAGE14:
				switch (cls_id) {
					case 1000:
						if (!element.ConceptCompleter) {
							var langTools = ace.require("ace/ext/language_tools");
							element.ConceptEditor.setOptions({enableBasicAutocompletion: true, enableSnippets: false, enableLiveAutocompletion: true});
							// hints: Target = min chars, Value = suggest list
							element.ConceptCompleter = {
								getCompletions: function(editor, session, pos, prefix, callback) {
									if (prefix.length === 0) {
										callback(null, []);
										return;
									}
									if (editor.ConceptHints) {
										var res = [];
										for (var i = 0; i < editor.ConceptHints.length; i++) {
											var h = editor.ConceptHints[i];
											if ((h) && (h.indexOf(prefix) == 0))
												res.push({value: h, caption: h, meta: "framework", score: 1000});
										}
										callback(null, res);
									} else
										callback(null, []);
								}
							}
							langTools.addCompleter(element.ConceptCompleter);
						}
						element.ConceptEditor.ConceptHints = Value.split(" ");
						element.ConceptEditor.ConceptLen = parseInt(Target);
						break;
				}
			case MSG_D_PRIMITIVE_LINE:
				var ctx = this.GetCanvas(element);
				var xy1 = parseInt(Target);
				var x1  = xy1 / 0xFFFF;
				var y1  = xy1 % 0xFFFF;

				var xy2 = parseInt(Value);
				var x2  = xy2 / 0xFFFF;
				var y2  = xy2 % 0xFFFF;

				ctx.moveTo(x1, y1);
				ctx.lineTo(x2, y2);
				ctx.stroke();
				break;
			case MSG_D_PRIMITIVE_ARC:
			case MSG_D_PRIMITIVE_FILL_ARC:
				var ctx = this.GetCanvas(element);

				var xy = parseInt(Target);
				var x = xy / 0xFFFF;
				var y = xy % 0xFFFF;

				var wh = parseInt(Value);
				var w  = wh / 0xFFFF;
				var h  = wh % 0xFFFF;

				var angle  = parseInt(this.POST_STRING);
				var angle1 = 0;//angle / 0xFFFF;
				var angle2 = angle % 0xFFFF;

				if (angle2 == 360*64)
					angle2 = 2 * Math.PI;

				ctx.arc(x, y, w/2, angle1, angle2);
				if (MSG_ID == MSG_D_PRIMITIVE_FILL_ARC)
					ctx.fill();
				ctx.stroke();
				break;
			case MSG_D_PRIMITIVE_RECT:
			case MSG_D_PRIMITIVE_FILL_RECT:
				var ctx = this.GetCanvas(element);
				var xy1 = parseInt(Target);
				var x1  = xy1 / 0xFFFF;
				var y1  = xy1 % 0xFFFF;
				var xy2 = parseInt(Value);
				var x2  = xy2 / 0xFFFF;
				var y2  = xy2 % 0xFFFF;
				if (MSG_ID == MSG_D_PRIMITIVE_FILL_RECT)
					ctx.fillRect(x1, y1, x2, y2);
				else
					ctx.rect(x1, y1, x2, y2);
				ctx.stroke();
				break;
			case MSG_D_PRIMITIVE_POINT:
				break;
			case MSG_D_PRIMITIVE_COLOR:
				var ctx = this.GetCanvas(element);
				var color = this.doColorString(parseInt(Value));
				ctx.fillStyle = color;
				ctx.strokeStyle = color;
				break;
		}
	}

	this.GetCanvas = function(element) {
		var child = element.ConceptCanvas;
		if (!child) {
			element.ConceptCanvas = document.createElement("canvas");
			element.ConceptCanvas.style.width = "100%";
			element.ConceptCanvas.style.height = "100%";
			element.innerHTML = "";
			element.appendChild(element.ConceptCanvas);
		}
		var ctx = element.ConceptCanvas.getContext("2d");
		return ctx;
	}

	this.AddFunction = function(func_name, variables, code, parameters) {
		if ((parameters) && (parameters.length))
			parameters = parameters + ", thi$";
		else
			parameters = "thi$";
		var body = "function (" + parameters + ") {\n";

		var parameters2 = parameters + ",";
		for(var key in variables) {
			var v = variables[key];
			if ((key != "this") && (parameters2.indexOf(key +",") == -1)) {
				body += "\tvar " + key +" = " + v + ";\n";
			}
		}
		body += code;
		body += "}\n";
		try {
			eval("window." + self.FPrefix + func_name + " = " + body);
		} catch (e) {
			console.error("Error in compensation function:\n" + body + "ERROR:" + e.toString() + " on line " + e.lineNumber);
		}
	}

	this.Level = function(level) {
		var result = "\t";
		for (var i = 0; i < level; i++)
			result += "\t";
		return result;
	}

	this.GetParam = function(line, sep) {
		var pindex = line.indexOf(sep);
		var get_func = "";
		if (pindex >= 0) {
			pindex += sep.length;
			get_func = line.substring(pindex);
			pindex = get_func.indexOf(" ");
			if (pindex > 0)
				get_func = get_func.substring(0, pindex).trim();
			get_func = get_func.replace(".", "__");
		}
		return get_func;
	}

	this.SendMessageLocal = function(sender, msg_id, target, val, code) {
		self.EmulatedMessage = { "Sender": sender, "MSG_ID": msg_id, "Target": target, "Value": val, "Code": code};
		if (msg_id == MSG_GET_PROPERTY)
			self.CompensatedMessages.push({ "Sender": sender, "MSG_ID": msg_id, "Target": target, "Value": val, "Code": code});
	}

	// used by latency compensation
	window[this.FPrefix + "send_message"] = function(sender, msg_id, target, val) {
		if (sender)
			sender = sender.substring(1);
		else
			sender = "-1";
		self.CONCEPT_CALLBACK(sender, msg_id, target, val, val, self.SendMessageLocal);
	
		return true;
	}

	this.ExtractVariables = function(line, variables) {
		if ((line.indexOf(".") != -1) || (line.indexOf("(") != -1)) {
			if ((line[0] != "(") && (line.indexOf(" = (") == -1))
				return;
		}

		line = line.split("(").join(" ");
		line = line.split(")").join(" ");
		line = line.split("[").join(" ");
		line = line.split("]").join(" ");
		line = line.split(",").join(" ");

		var arr = line.split(" ");
		var len = arr.length;
		for (var i = 0; i < len; i++) {
			var v = arr[i];

			if ((v) && (v.length) && (v.match(/^\w+$/))) {
				if (typeof variables[v] == "undefined") {
					variables[v] = 0;

				}
			}
		}
	}

	this.LatencyCompensationCode = function(code) {
		var lines = code.split("\n");
		var len = lines.length;
		var in_func = false;
		var func = "";
		var parameters = "";
		var variables = { };
		var code_block = "";
		var entry_point = "";
		var level = 0;
		var func_line = 0;
		var end_brackets = [ ];
		var follow_else = false;
		var in_if = 0;
		var properties = { };

		for (var i = 0; i < len; i++) {
			var level_str = this.Level(level);
			var line = lines[i].trim();
			if ((line) && (line.length > 0)) {
				if (line[0] == "#") {
					entry_point = line.substring(1).trim().replace(".","__");
					continue;
				}
				if (in_func) {
					switch (line[0]) {
						case '$':
							line = line.substring(1).trim();
							var pos = line.indexOf(" ");
							if (pos > -1) {
								var varname = line.substring(0, pos);
								var varvalue = line.substring(pos + 1);
								if ((varvalue) && (varvalue.length >= 2) && (varvalue[0] == varvalue[varvalue.length - 1]) && (varvalue[0] == '"'))
									variables[varname] = varvalue;
								else
									variables[varname] = parseFloat(varvalue);
							}
							break;
						case '@':
							// new function
							if (in_func)
								this.AddFunction(func, variables, code_block, parameters);
							in_func = false;
							break;
						default:
							var arr = line.split(" ");
							var len2 = arr.length;

							if ((len2 == 2) && (arr[0] == "this")) {
								code_block += level_str + "if (typeof thi$ == \"undefined\")\n";
								code_block += level_str + "\tthi$ = self.Controls[" + parseInt(arr[1]) + "];\n";
								break;
							}
							var if_closed = false;
							var extra_block = "";
							func_line++;
							if (end_brackets[func_line]) {
								if (in_if) {
									if_closed = true;
									in_if--;
								}
								var count = end_brackets[func_line];

								for (var j = 0; j < count; j++)
									extra_block += this.Level(--level) + "}\n";
							}

							var v1 = "";
							var v2 = "";
							var v_local = "";
							if (len > 0) {
								v1 = arr[0];
								switch (v1) {
									case "if":
										v_local = arr[1];
										if (v_local[0]=="!")
											v_local = v_local.substring(1);
										if (typeof variables[v_local] == "undefined")
											variables[v_local] = 0;

										if (len2 >= 3) {
											if (arr[2] == "return") {
												code_block += level_str + "if (" + arr[1] +")\n" + level_str + "\treturn;\n";
											} else
											if (arr[2] == "goto") {
												var pos = parseInt(arr[3]);
												if (pos >= func_line) {
													var cond = arr[1];
													if (cond[0] == "!")
														cond = cond.substring(1);
													else
														cond = "!" + cond;

													code_block += level_str + "if (" + cond +") {\n";
													level++;
													if (end_brackets[pos])
														end_brackets[pos]++;
													else
														end_brackets[pos] = 1;
													in_if ++;
												} else
													code_block += level_str + line +";\n";
											}
										} else
											code_block += level_str + line +" {\n";
										break;
									case "echo":
										if (len > 1) {
											v2 = arr[1];
											if (typeof variables[v2] == "undefined")
												variables[v2] = 0;
											code_block += level_str + "console.warn(" + v2 + ");\n";
										}
										break;
									case "goto":
										if ((len2 == 2) && (if_closed)) {
											var pos = parseInt(arr[1]);
											if (pos >= func_line) {
												code_block += extra_block.substring(0,  extra_block.length - 1) + " else {\n";
												extra_block = "";
												level++;
												if (end_brackets[pos])
													end_brackets[pos]++;
												else
													end_brackets[pos] = 1;
											} else
												code_block += level_str + line + ";\n";
										} else
											code_block += level_str + line + ";\n";
										break;
									case "return":
										v_local = arr[1];
										if (typeof variables[v_local] == "undefined")
											variables[v_local] = 0;

										code_block += level_str + line + ";\n";
										break;
									case "new":
										code_block += level_str + line + ";\n";
										break;
									default:
										line = line.replace(" this ", " thi$ ");
										var obj_pos = line.indexOf("#");
										if ((obj_pos > 0) && (line.indexOf("=") > 0)) {
											var rid = line.substring(obj_pos + 1).trim();
											if (rid) {
												if (typeof variables[v1] == "undefined")
													variables[v1] = 0;
												code_block += level_str + v1 + " = self.Controls[" + parseInt(rid) + "];\n";
											}
										} else
										if (line.indexOf(" value ") != -1) {	
											line = line.replace(" value ", " parseFloat(\"\" + ") + ")";
											code_block += level_str + line + ";\n";
										} else
										if (line.indexOf("(") == -1) {
											if (typeof variables[v1] == "undefined")
												variables[v1] = 0;

											line += "@";
											line = line.replace(" REMOTE_ID_STR@", " id");
											line = line.replace(" REMOTE_ID@", " id");
											line = line.replace(" RID@", "id");
											line = line.replace("@", "");

											var orig = this.GetParam(line, " . ");
											var set_func = this.GetParam(line, " -> ");
											var get_func = this.GetParam(line, " <- ");

											if ((get_func.length) || (set_func.length)) {
												var pre_line = "";
												var param;
												var var_res = "";
												if (arr[1] == "=") {
													pre_line = v1 + " = ";
													param = arr[2];
													var_res = v1;
												} else
													param = arr[0];

												if (get_func.length)
													code_block += level_str + pre_line + self.FPrefix + get_func + "(" + param + ");\n";
												else
													code_block += level_str + pre_line + param + " . " + orig + ";\n";

												if ((set_func.length) && (var_res.length))
													properties[var_res] = self.FPrefix + set_func + "(" + var_res + ", " +  param + ");"
											} else {
												code_block += level_str + line + ";\n";
												this.ExtractVariables(line, variables);
												if ((len2 >= 3) && (properties[v1])) {
													switch (arr[1]) {
														case "=":
														case "+=":
														case "-=":
														case "*=":
														case "/=":
														case "%=":
														case "|=":
														case "&=":
														case "^=":
														case ">>=":
														case "<<=":
															code_block += level_str + properties[v1] + "\n";
															properties[v1] = null;
															break;
													}
												}
											}
										} else {
											if ((len2 > 2) && (arr[1] == "=")) {
												if (typeof variables[v1] == "undefined")
													variables[v1] = 0;
											}

											if ((line.indexOf("send_message(") == 0) || (line.indexOf(" send_message_ID(") > 0))
												line = line.replace("send_message(", self.FPrefix + "send_message(");

											var p_start = line.indexOf("(");
											var p_end = line.indexOf(")");
											var params = "";
											var is_wait_message = false;

											if ((line.indexOf("wait_message_ID(") == 0) || (line.indexOf(" wait_message_ID(") > 0))
												is_wait_message =true;

											if ((len2 > 2) && (arr[2][0] != "(")) {
												if ((p_start > 0) && (p_end > 0) && (p_end - p_start > 1)) {
													params = line.substring(p_start + 1, p_end);
													var p_arr = params.split(",");
													var len3 = p_arr.length;

													for (var k = 0; k < len3; k++) {
														var v = p_arr[k].trim();
														if (typeof variables[v] == "undefined")
															variables[v] = 0;
														if (is_wait_message) {
															switch (k) {
																case 0:
																	code_block += level_str + v +" = self.EmulatedMessage.Sender;\n";
																	break;
																case 1:
																	code_block += level_str + v +" = self.EmulatedMessage.MSG_ID;\n";
																	break;
																case 2:
																	code_block += level_str + v +" = self.EmulatedMessage.Target;\n";
																	break;
																case 3:
																	code_block += level_str + v +" = self.EmulatedMessage.Value;\n";
																	break;
															}
														}
													}
												}
											}
											if (is_wait_message)
												break;

											if (line.indexOf("::") > 0) {
												// static functions not supported yet
												code_block += level_str + "return;\n";
												break;
											} else
											if (line.indexOf(" . ") > 0) {
												var orig = this.GetParam(line, " . ");
												var get_func = this.GetParam(line, " <- ");

												if (get_func.length) {
													var pre_line = "";
													var param;
													if (arr[1] == "=") {
														pre_line = arr[0] + " = ";
														param = arr[2];
													} else
														param = arr[0];

													if (get_func.length) {
														if (params)
															params += ", ";
														params += param;
														code_block += level_str + pre_line + self.FPrefix + get_func + "(" + params + ");\n";
													} else
														code_block += level_str + pre_line + " . " + orig + ";\n";
													break;
												}
											} else
											if (line.indexOf(".") > 0) {
												line = line.replace(".", "__");
												line = line.replace("()", "(thi$)");
												line = line.replace(")", ", thi$)");
												if (line.indexOf(" = ") > 0)
													line = line.replace(" = ", " = " + self.FPrefix);
												else
													line = self.FPrefix + line;
											} else
												this.ExtractVariables(line, variables);
											code_block += level_str + line + ";\n";
										}
										break;
								}
							}
							code_block += extra_block;
							break;
					}
				}
				if ((!in_func) && (line[0] == "@")) {
					line = line.substring(1).trim().replace(".","__");
					var pos = line.indexOf("(");
					if (pos >= 0) {
						func = line.substring(0, pos);
						parameters = line.substring(pos + 1);
						var pos2 = parameters.lastIndexOf(")");
						if (pos2 >= 0)
							parameters = parameters.substring(0, pos2);

						if (func) {
							while (level)
								code_block += this.Level(--level) + "}\n";
							in_func = true;
							variables = { };
							code_block = "";
							func_line = 0;
							end_brackets = [ ];
							properties = { };
							level = 0;
							in_if = 0;
						}
					}
				}
			}
		}
		if (in_func) {
			while (level)
				code_block += this.Level(--level) + "}\n";
			this.AddFunction(func, variables, code_block, parameters);
		}
		return entry_point;
	}

	this.LatencyCompensation = function(latency_code, sender, event_data) {
		self.CompensatedMessages = [ ];
		var func_id = self.FPrefix + latency_code;
		if (window[func_id]) {
			try {
				window[func_id](sender, "");
				return true;
			} catch (e) {
				console.error("ERROR:" + e + " on line " + e.lineNumber);
				console.error("Stack:\n" + e.stack);
			}
		}
		return false;
	}

	this.SetEvent = function(OwnerID, EVENT_ID, Value) {
		var element = this.Controls[OwnerID];
		var label;
		if (!element) {
			console.warn("SetEvent: Unknown object RID: " + OwnerID);
			return;
		}
		var cls_id = element.ConceptClassID;
		if (!cls_id)
			cls_id = parseInt(element.getAttribute("ConceptClassID"));

		switch (EVENT_ID) {
			case EVENT_ON_CLICKED:
				switch (cls_id) {
					case CLASS_BUTTON:
					case CLASS_TOOLBUTTON:
					case CLASS_RADIOTOOLBUTTON:
					case CLASS_TOGGLETOOLBUTTON:
					case CLASS_MENUTOOLBUTTON:
						var latency_code = null;
						if (Value.length > 3)
							latency_code = this.LatencyCompensationCode(window.atob(Value.replace(/\s/g, '')));

						element.onclick = function(e) {
							if (latency_code)
								self.LatencyCompensation(latency_code, element, "");

							self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_CLICKED, "", 0);
						}
						break;
				}
				break;
			case EVENT_ON_BUTTONPRESS:
				element.onmousedown = function(e) {
					e = e || window.event;
					var button = "0";

					if ((e.which && e.which == 3) || (e.button && e.button == 2))
						button = "1";
					self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_BUTTONPRESS, button, 0);
					if (self.isIE)
						e.cancelBubble = true;
					else
						e.stopPropagation();
				}
				break;
			case EVENT_ON_BUTTONRELEASE:
				element.onmouseup = function(e) {
					e = e || window.event;
					var button = "0";

					if ((e.which && e.which == 3) || (e.button && e.button == 2))
						button = "1";
					self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_BUTTONRELEASE, button, 0);
					if (self.isIE)
						e.cancelBubble = true;
					else
						e.stopPropagation();
				}
				break;
			case EVENT_ON_HIDE:
				element.ConceptHide = this;
				break;
			case EVENT_ON_SHOW:
				element.ConceptShow = true;
				break;
			case EVENT_ON_VISIBILITY:
				element.ConceptVisibility = this;
				break;				
			case EVENT_ON_TOGGLED:
				switch (cls_id) {
					case CLASS_CHECKBUTTON:
					case CLASS_RADIOBUTTON:
						element.onchange = function() {
							if (element.checked)
								self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_TOGGLED, "1", 0);
							else
								self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_TOGGLED, "0", 0);
						}
						break;
					default:
						element.ConceptToggled = true;
				}
				break;
			case EVENT_ON_ACTIVATE:
				switch (cls_id) {
					case CLASS_IMAGEMENUITEM:
					case CLASS_RADIOMENUITEM:
					case CLASS_CHECKMENUITEM:
					case CLASS_STOCKMENUITEM:
					case CLASS_MENUITEM:
					case CLASS_TEAROFFMENUITEM:
						label = document.getElementById("l"+OwnerID);
						if (label) {
							label.onclick = function() {
								self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_ACTIVATE, "", 0);
								return false;
							};
						}
						break;
					case CLASS_EDIT:
					case CLASS_COMBOBOXTEXT:
						var label = element.firstChild.children[1];
						if (label) {
							label.addEventListener("keydown", function(e) {
								if (!e)
									e = window.event;

								if ((e.keyCode || e.which) == 0x0D)
									self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_ACTIVATE, "", 0);
							});
						}
						break;

				}
				break;
			case EVENT_ON_ROWACTIVATED:
				if (cls_id == CLASS_TREEVIEW) {
					element.addEventListener("click", function(e) {
						if (e.target) {
							var ename = e.target.nodeName.toLowerCase();
							if ((ename == "tr") || (ename == "td"))
								self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_ROWACTIVATED, self.GetPath(e.target, ename), 0);
						}
					});
				} else
				if (cls_id == CLASS_ICONVIEW) {
					element.addEventListener("click", function(e) {
						var target = e.target;
						if (target) {
							while ((target) && (target.nodeName.toLowerCase() != "li"))
								target = target.parentNode

							if (target)
								self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_ROWACTIVATED, "" + Array.prototype.indexOf.call(target.parentNode.childNodes, target), 0);
						}
					});
				}
				break;
			case EVENT_ON_CHANGED:
				switch (cls_id) {
					case CLASS_ICONVIEW:
						element.addEventListener("click", function(e) {
							var target = e.target;
							if (target) {
								while ((target) && (target.nodeName.toLowerCase() != "li"))
									target = target.parentNode

								if (target)
									self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_CHANGED, "" + Array.prototype.indexOf.call(target.parentNode.childNodes, target), 0);
							}
						});
						break;
					case CLASS_EDIT:
					case CLASS_COMBOBOXTEXT:
						var label = element.firstChild.children[1];
						if (label) {
							label.addEventListener("input", function() {
								self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_CHANGED, "" + label.value, 0);
							});
						}
						if (cls_id != CLASS_COMBOBOXTEXT)
							break;
					case CLASS_COMBOBOX:
					case CLASS_PROPERTIESBOX:
						element.ConceptChanged = true;
						break;
				}
				break;
			case EVENT_ON_CURSORCHANGED:
				if (cls_id == CLASS_TREEVIEW) {
					element.addEventListener("click", function(e) {
						if (e.target) {
							var ename = e.target.nodeName.toLowerCase();
							if ((ename == "tr") || (ename == "td"))
								self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_CURSORCHANGED, self.GetPath(e.target, ename), 0);
						}
					});
				}
				break;
			case EVENT_ON_HEADERCLICKED:
				if (cls_id == CLASS_TREEVIEW)
					element.ConceptHeaderClicked = true;
				break;
			case EVENT_ON_TIMER:
				var timeout = parseInt(Value);
				if (element.ConceptTimer) {
					clearTimeout(element.ConceptTimer);
					element.ConceptTimer = null;
				}
				if (timeout > 0) {
					element.ConceptTimer = setTimeout(function () {
						self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_TIMER, "", 0);
					}, timeout);
				}
				break;
			case EVENT_ON_REALIZE:
				self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_REALIZE, "", 0);
				break;
			case EVENT_ON_GRABFOCUS:
				element.onfocus = function(e) {
					self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_GRABFOCUS, "", 0);
					var event = e || window.event;
					if (self.isIE)
						event.cancelBubble = true;
					else
						event.stopPropagation();
				}
				break;
			case EVENT_ON_FOCUS:
				var ev_func = function(e) {
					self.SendMessage(this.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_FOCUS, "", 0);
					var event = e || window.event;
					if (self.isIE)
						event.cancelBubble = true;
					else
						event.stopPropagation();
				}
				switch (element.nodeName.toLowerCase()) {
					case "div":
					case "p":
					case "table":
						element.addEventListener('mousedown', ev_func, false);
						break;
					default:
						element.onfocusin = ev_func;
				}
				break;
			case EVENT_ON_FOCUSIN:
				var ef_func_in = function(e) {
					self.SendMessage(this.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_FOCUSIN, "", 0);
					var event = e || window.event;
					if (self.isIE)
						event.cancelBubble = true;
					else
						event.stopPropagation();
				}
				switch (element.nodeName.toLowerCase()) {
					case "div":
					case "p":
					case "table":
						element.addEventListener('mousedown', ef_func_in, false);
						break;
					default:
						element.onfocusin = ef_func_in;
				}
				break;
			case EVENT_ON_SETFOCUS:
				var ef_func_in2 = function(e) {
					self.SendMessage(this.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_SETFOCUS, "", 0);
					var event = e || window.event;
					if (self.isIE)
						event.cancelBubble = true;
					else
						event.stopPropagation();
				}
				switch (element.nodeName.toLowerCase()) {
					case "div":
					case "p":
					case "table":
						element.addEventListener('mousedown', ef_func_in2, false);
						break;
					default:
						element.addEventListener('focusin', ef_func_in2, false);
				}
				break;
			case EVENT_ON_FOCUSOUT:
				if (cls_id == CLASS_FORM) {
					var e2 = document.getElementById("d" + OwnerID);
					if (e2) {
						// not e2!
						element.onclick = function(e) {
							self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_FOCUSOUT, "", 0);
						};
						e2.onclick = function(e) {
							var event = e || window.event;
							if (self.isIE)
								event.cancelBubble = true;
							else
								event.stopPropagation();
						};
					}
				} else
					element.onfocusout = function() {
						self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_FOCUSOUT, "", 0);
					}
				break;
			case EVENT_ON_ENTER:
				element.addEventListener('mouseenter', function(e) {
					var event = e || window.event;
					var position = e.pageX + "," + e.pageY;
					self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_ENTER, position, 0);
				}, false);
				break;
			case EVENT_ON_LEAVE:
				element.addEventListener('mouseleave', function(e) {
					var event = e || window.event;
					var position = e.pageX + "," + e.pageY;
					self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_LEAVE, position, 0);
				}, false);
				break;
			case EVENT_ON_MOTIONNOTIFY:
				element.addEventListener('mouseleave', function(e) {
					var event = e || window.event;
					var position = e.pageX + "," + e.pageY;
					self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_MOTIONNOTIFY, position, 0);
				}, false);
				break;
			case EVENT_ON_PRESSED:
				if ((cls_id == CLASS_EDIT) || (cls_id == CLASS_COMBOBOXTEXT))
					element.ConceptIconPress = true;
				break;
			case EVENT_ON_RELEASED:
				if ((cls_id == CLASS_EDIT) || (cls_id == CLASS_COMBOBOXTEXT))
					element.ConceptIconRelease = true;
				break;
			case EVENT_ON_DELETEEVENT:
				element.ConceptDelete = this;
				break;
			case EVENT_ON_UNREALIZE:
				element.ConceptUnrealize = this;
				break;
			case EVENT_ON_DRAGDATARECEIVED:
				element.ConceptDragDataReceived = true;
				break;
			case EVENT_ON_DRAGDROP:
				element.ConceptDragDrop = true;
				break;
			case EVENT_ON_DRAGBEGIN:
				element.ConceptDragBegin = true;
				break;
			case EVENT_ON_DRAGEND:
				element.ConceptDragBegin = true;
				break;
			case EVENT_ON_DRAGDATAGET:
				element.ConceptDragDataGet = true;
				break;
			case EVENT_ON_VALUECHANGED:
				element.ConceptValueChanged = true;
				break;
			case EVENT_ON_DAYSELECTED:
				if (cls_id == CLASS_CALENDAR) {
					$(element).on("changeDate", function(e){
						if (e.date)
							self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_DAYSELECTED, "", 0);
					});
				}
				break;
			case EVENT_ON_DAYSELECTEDDBCLICK:
				if (cls_id == CLASS_CALENDAR) {
					$(element).on("changeDate", function(e){
						if (e.date) {
							var year = "" + e.date.getFullYear();
							var month = "" + (e.date.getMonth() + 1);
							if (month.length < 2)
								month = "0" + month;
							var day = "" + e.date.getDate();
							if (day.length < 2)
								day = "0" + day;
							self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_DAYSELECTEDDBCLICK, year + "-" + month + "-" + day, 0);
						}
					});
				}
				break;
			case EVENT_ON_MONTHCHANGED:
				if (cls_id == CLASS_CALENDAR) {
					$(element).on("changeMonth", function(e) {
						if (e.date) {
							var date = "" + e.date.getFullYear() + "-" + e.date.getMonth();
							self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_MONTHCHANGED, date, 0);
						}
					});
				}
				break;
			case EVENT_ON_NEXTMONTH:
				if (cls_id == CLASS_CALENDAR) {
					$(element).on("changeMonth", function(e) {
						if (e.date) {
							var date = "" + e.date.getFullYear() + "-" + e.date.getMonth();
							self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_NEXTMONTH, date, 0);
						}
					});
				}
				break;
			case EVENT_ON_NEXTYEAR:
				if (cls_id == CLASS_CALENDAR) {
					$(element).on("changeYear", function(e) {
						self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_NEXTYEAR, e.date ? "" + e.date.getFullYear() : "", 0);
					});
				}
				break;
			case EVENT_ON_PREVMONTH:
				if (cls_id == CLASS_CALENDAR) {
					$(element).on("changeMonth", function(e) {
						if (e.date) {
							var date = "" + e.date.getFullYear() + "-" + e.date.getMonth();
							self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_PREVMONTH, date, 0);
						}
					});
				}
				break;
			case EVENT_ON_PREVYEAR:
				if (cls_id == CLASS_CALENDAR) {
					$(element).on("changeYear", function(e){
						self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_PREVYEAR, e.date ? "" + e.date.getFullYear() : "", 0);
					});
				}
				break;
			case EVENT_ON_STARTEDITING:
				element.ConceptStartEditing = true;
				break;
			case EVENT_ON_ENDEDITING:
				element.ConceptEndEditing = true;
				break;
			case EVENT_ON_SWITCHPAGE:
				element.ConceptSwitchPage = true;
				break;
			case EVENT_ON_MOVESLIDER:
				element.ConceptMoveSlider = true;
				break;
			case EVENT_ON_CHANGEVALUE:
				if ((cls_id == CLASS_VSCALE) || (cls_id == CLASS_HSCALE)) {
					$(element.ConceptEdit).slider().on('slideStop', function(ev) {
						self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_CHANGEVALUE, "" + ev.value, 0);
					});
				}
				break;
			case EVENT_ON_INSERTATCURSOR:
				if ((cls_id == CLASS_EDIT) || (cls_id == CLASS_COMBOBOXTEXT)) {
					label = element.firstChild.children[1];
					if (label) {
						label.addEventListener('input', function() {
							self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_INSERTATCURSOR, "" + label.value, 0);
						}, false);
					}
				} else
				if (cls_id == CLASS_TEXTVIEW) {
					element.addEventListener('input', function() {
						self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_INSERTATCURSOR, "", 0);
					}, false);
				}
				break;
			case EVENT_ON_SETANCHOR:
				if ((cls_id == CLASS_EDIT) || (cls_id == CLASS_COMBOBOXTEXT)) {
					label = element.firstChild.children[1];
					if (label) {
						label.addEventListener('select', function(e) {
							self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_SETANCHOR, "", 0);
						}, false);
					}
				} else
				if (cls_id == CLASS_TEXTVIEW) {
					element.addEventListener('select', function(e) {
						self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_SETANCHOR, "", 0);
					}, false);
				}
				break;
			case EVENT_ON_ROWCOLLAPSED:
				element.ConceptRowCollapsed = true;
				break;
			case EVENT_ON_ROWEXPANDED:
				element.ConceptRowExpanded = true;
				break;
			case EVENT_ON_SIZEREQUEST:
				element.addEventListener("resize", function(e) {
					self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_SIZEREQUEST, "", 0);
				}, false);
				break;
			case EVENT_ON_SIZEALLOCATE:
				element.addEventListener("resize", function(e) {
					self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_SIZEALLOCATE, "", 0);
				}, false);
				break;
			case EVENT_ON_CONFIGURE:
				element.addEventListener("resize", function(e) {
					self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_CONFIGURE, "", 0);
				}, false);
				break;
			case EVENT_ON_KEYPRESS:
				if (cls_id == CLASS_TEXTVIEW)
					element.setAttribute("tabindex", "10000");
				element.onkeydown = function(e) {
					var code = (e.keyCode || e.which);
					self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_KEYPRESS, "" + code, 0);
				}
				break;
			case EVENT_ON_KEYRELEASE:
				if (cls_id == CLASS_TEXTVIEW)
					element.setAttribute("tabindex", "10000");
				else
				if ((cls_id == CLASS_EDIT) || (cls_id == CLASS_COMBOBOXTEXT))
					element = element.firstChild.children[1];

				element.onkeyup = function(e) {
					var code = (e.keyCode || e.which);
					self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_KEYRELEASE, "" + code, 0);
				}
				break;
			case EVENT_ON_EVENT:
				if (cls_id == CLASS_HTMLSNAP) {
					if (element.ConceptHTMLClass) {
						var event_name = element.ConceptHTMLClass + "Event";
						var fn = window[event_name];
						if (fn) {
							try {

								fn({ "Client": this, "RID": "" + OwnerID, "Object": element });
							} catch (e) {
								console.error("Error in " + event_name);
								console.error(e);
							}
						} else {
							console.error("Function " + event_name + " is not defined.");
						}
					} else {
						console.error("Cannot use events for unregistered RSnap objects");
					}
				}
				break;
			case 350:
				// first custom event
				switch (cls_id) {
					case 1016:
						element.ConceptBufferFull = function(data, type) {
							if (element.ConceptCompression) {
								switch (element.ConceptCompression) {	
									case 1:
										// speex
										data = self.SpeexCompress(element, data);
										if (data)
											self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "350", data, 3);
										return;
									case 2:
										// opus
										var data_out = element.OpusEncoder.encode_float(data[0]);
										if (data_out) {
											var size = 0;
											for (var i = 0; i < data_out.length; i++)
												size += data_out[i].length;
											if (size > 0) {
												var ref_out = new Uint8Array(size);
												var offset = 0;
												for (var i = 0; i < data_out.length; i++) {
													var buf = data_out[i];
													ref_out.set(buf, offset);
													offset += buf.length;
												}
												self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "350", ref_out, 1);
											}
										}
										break;
								}	
							} else
								self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "350", data, type);
						}
						break;
					case 1018:
						element.onerror = function(event) {
							event = event || window.event;
							self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "350", event.error, 0);
						}
						break;
				}
				break;
			case 351:
				if (cls_id == 1018) {
					element.onstart = function() {
						self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "351", "", 0);
					}
				}
				break;
			case 352:
				if (cls_id == 1018) {
					element.onend = function() {
						self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "352", "", 0);
					}
				}
				break;
			case 353:
				if (cls_id == 1018) {
					element.onresult = function(event) {
						event = event || window.event;
						var interim_transcript = "";
						var final_transcript = "";
						for (var i = event.resultIndex; i < event.results.length; ++i) {
							if (event.results[i].isFinal) {
								final_transcript += event.results[i][0].transcript;
							} else {
								interim_transcript += event.results[i][0].transcript;
							}
						}
						if (final_transcript.length)
							self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "353", final_transcript, 0);
						else
							self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "353", "*" + interim_transcript, 0);
					};
				}
				break;
		}
	}

	this.Fire = function(RID, parameter) {
		var data = "";
		switch (typeof parameter) {
			case "string":
			case "number":
				data = "" +parameter;
				break;
			default:
				if (parameter)
					data = JSON.stringify(parameter);
		}
		this.SendMessage("" + RID, MSG_EVENT_FIRED, "21", data, 0);
	}

	this.UIEvent = function(parameter) {
		self.Fire(this.HTMLUIID, parameter);
	}

	this.SpeexCompress = function(element, data) {
		if (data) {
			var arr2 = data[0];
			var len2 = arr2.length;

			var remainder = 0;
			if (element.Speex.Remainder)
				remainder = element.Speex.Remainder.length;
			var len3 = len2 + remainder;
			var limit = Math.floor(len3 / 160) * 160;
			var remIndex = 0;
			var outIndex = 0;

			if (len3 < 160) {
				var rem2 = new Int16Array(len3);
				for (var i = 0; i < remainder; i++)
					rem2[remIndex++] = element.Speex.Remainder[i];
				for (var i = 0; i < len2; i++)
					rem2[remIndex++] = arr2[i];

				element.Speex.Remainder = rem2;
				return;
			}

			var size = limit;
			var out = new Int16Array(size);
			for (var i = 0; i < remainder; i++)
				out[outIndex++] = element.Speex.Remainder[i];

			if (limit < len3) {
				element.Speex.Remainder = new Int16Array(len3 - limit);
			} else {
				element.Speex.Remainder = null;
			}

			for (var i = 0; i < len2; i++) {
				if (outIndex < limit)
					out[outIndex++] = Math.floor(arr2[i] * 32768);
				else
					element.Speex.Remainder[remIndex++] = Math.floor(arr2[i] * 32768);
			}

			if (outIndex) {
				//var res = element.Speex.encode(out);
				var res = element.Speex.encoder.process(out);
				if (res) {
					var arr = res[0];
					if (arr) {
						var frames = res[1].length;
						var nb = res[1][0];
						var len = arr.length;

						var res2 = new Uint8Array(len + 2);
						res2[0] = frames;
						res2[1] = nb;
						for (var i = 0; i < len; i++)
							res2[i + 2] = arr[i];

						return [res2];
					}
				}
			}
		}
		return null;
	}

	this.Decompress = function(element, data, big_buffer) {
		if (data) {
			if (element.ConceptCompression) {
				switch (element.ConceptCompression) {	
					case 1:
						// speex
						if (element.Speex) {
							var str = new DataView(data);
							var index = 0;
							var frames;
							if (big_buffer) {
								var v = str.getUint32(index);
								//frames = ((v & 0xFF) << 24) | ((v & 0xFF00) << 8) | ((v >> 8) & 0xFF00) | ((v >> 24) & 0xFF);
								frames = v;
								index += 4;
							} else
								frames = str.getUint8(index++);
							
							var nb = str.getUint8(index++);
							if (frames) {
								var size = frames * nb;
								var out = new Uint8Array(size);
								var segments = new Uint8Array(frames);

								for (var i = 0; i < size; i++)
									out[i] = str.getUint8(index++);

								for (var i = 0; i < frames; i++)
									segments[i] = nb;

								var res = element.Speex.decode(out, segments);

								return [res];
							}
						}
						break;
					case 2:
						// opus
						if (element.OpusDecoder) {
							var output = [ ];
							var d2 = new Uint8Array(data);
							var offset = 0;
							var total_size = 0;
							while (offset < d2.length) {
								var size = d2[offset++];
								if (size > 0xC0) {
									size -= 0xC0;
									size *= 0x3F;
									size += d2[offset++];
								}
								var res = element.OpusDecoder.decode_float(d2.subarray(offset, offset + size));
								if ((res) && (res.length)) {
									output.push(res);
									total_size += res.length;
								}
								offset += size;
							}
							offset = 0;
							var output2 = new Float32Array(total_size);
							for (var i = 0; i < output.length; i++) {
								output2.set(output[i], offset)
								offset += output[i].length;
							}
							return [output2];
						}
						break;
				}
			}
		}
		return null;
	}

	this.GetAbsolutePosition = function(element) {
		var top = 0;
		left = 0;
		do {
			top += (element.offsetTop  || 0) - (element.scrollTop || 0);
			left += (element.offsetLeft || 0) - (element.scrollLeft || 0);
			element = element.offsetParent;
		} while (element);

		return { top: top, left: left };
	}

	this.GetProperty = function(OwnerID, PROP_ID, Value, ValueBuffer, SendMessageFunction) {
		var element = this.Controls[OwnerID];
		var label;

		if (!element) {
			console.warn("GetProperty: Unknown object RID: " + OwnerID);
			return;
		}

		var cls_id = element.ConceptClassID;
		if (!cls_id)
			cls_id = parseInt(element.getAttribute("ConceptClassID"));

		var result = "";

		switch (PROP_ID) {
			case P_CAPTION:
				switch (cls_id) {
					case CLASS_LABEL:
					case CLASS_HTMLSNAP:
						result = element.innerHTML;
						break;
					case CLASS_TEXTVIEW:
						result = element.textContent || element.innerText || "";
						break;
					case CLASS_EDIT:
					case CLASS_COMBOBOXTEXT:
						label = element.firstChild.children[1];
						if (label)
							result = label.value;
						break;
					case CLASS_BUTTON:
					case CLASS_CHECKBUTTON:
					case CLASS_RADIOBUTTON:
					case CLASS_PROGRESSBAR:
					case CLASS_SPINBUTTON:
						label = document.getElementById("l" + OwnerID);
						if (label)
							result = label.innerHTML;
						break;
					case 1000:
						if (element.ConceptEditor)
							result = element.ConceptEditor.getSession().getValue();
						break;
					case 1012:
						if ((element.contentWindow) && (element.contentWindow.document))
							result = element.contentWindow.document.title;
				}
				break;
			case P_CHECKED:
				switch (cls_id) {
					case CLASS_CHECKBUTTON:
					case CLASS_RADIOBUTTON:
						var input = document.getElementById("i" + OwnerID);
						if (input) {
							if (input.checked)
								result = "1";
							else
								result = "0";
						}
						break;
					case CLASS_TOGGLETOOLBUTTON:
					case CLASS_CHECKMENUITEM:
					case CLASS_RADIOTOOLBUTTON:
					case CLASS_RADIOMENUITEM:
						if (element.ConceptChecked)
							result = "1";
						else
							result = "0";
						break;
				}
				break;
			case P_CURSOR:
				switch (cls_id) {
					case CLASS_TREEVIEW:
					case CLASS_COMBOBOX:
					case CLASS_COMBOBOXTEXT:
					case CLASS_ICONVIEW:
						var index = element.getAttribute("ConceptRowIndex");
						if (index)
							result = "" + index;
						else
							result = "-1";
						break;
				}
				break;
			case P_SELECTEDCOLUMN:
				if (cls_id == CLASS_TREEVIEW) {
					if (typeof element.ConceptColumnIndex == "undefined")
						result = "-1";
					else
						result = "" + element.ConceptColumnIndex;
				}
				break;
			case 1002:
				if (cls_id == 1015) {
					var canvas = element.ConceptCanvasFrame;
					if (canvas) {
						try {
							var ctx = canvas.getContext('2d');
							if (ctx) {
								ctx.drawImage(element, 0, 0, canvas.width, canvas.height);
								var data;
								if (element.ConceptFormat)
									data = canvas.toDataURL(element.ConceptFormat);
								else
									data = canvas.toDataURL("image/jpeg", 0.5);
								if (data) {
									SendMessageFunction("" + OwnerID, MSG_GET_PROPERTY, "" + PROP_ID, FromBase64(data), 0);
									return;
								}
							}
						} catch (e) {
							// nothing
						}
					}
				}
				break;
			case 1033:
				if (cls_id == 1015) {
					var canvas = element.ConceptCanvasSmallFrame;
					if (canvas) {
						try {
							canvas.width = element.videoWidth;
							canvas.height = element.videoHeight;
							var ctx = canvas.getContext('2d');
							if (ctx) {
								ctx.drawImage(element, 0, 0, canvas.width, canvas.height);
								var data;
								if (element.ConceptFormat)
									data = canvas.toDataURL(element.ConceptFormat);
								else
									data = canvas.toDataURL("image/jpeg", 0.5);
								if (data) {
									SendMessageFunction("" + OwnerID, MSG_GET_PROPERTY, "" + PROP_ID, FromBase64(data), 0);
									return;
								}
							}
						} catch (e) {
							// nothing
						}
					}
				}
				break;
			case P_SCREENSHOT:
				$(element).html2canvas({
					onrendered: function (canvas) {
						var data = canvas.toDataURL("image/png");
						if (data)
							self.SendMessage("" + OwnerID, MSG_GET_PROPERTY, "" + PROP_ID, FromBase64(data), 0);
						else
							self.SendMessage("" + OwnerID, MSG_GET_PROPERTY, "" + PROP_ID, "", 0);
						return;
					}
				});
				return;
				break;
			case P_LEFT:
				var rect = element.getBoundingClientRect();
				if (rect)
					result = "" + rect.left;
				break;
			case P_TOP:
				var rect = element.getBoundingClientRect();
				if (rect)
					result = "" + rect.top;
				break;
			case P_ABSOLUTE:
				var rect = this.GetAbsolutePosition(element);
				if (rect)
					result = "" + rect.left + "," + rect.top;
				break;
			case P_WIDTH:
				if (cls_id == CLASS_FORM) {
					var e2 = document.getElementById("d" + OwnerID);
					if (e2)
						element = e2;
				}
				result = "" + element.offsetWidth;
				break;
			case P_HEIGHT:
				if (cls_id == CLASS_FORM) {
					var e2 = document.getElementById("d" + OwnerID);
					if (e2)
						element = e2;
				}
				result = "" + element.offsetHeight;
				break;
			case P_PAGE:
				if (cls_id == CLASS_NOTEBOOK) {
					var tabs = document.getElementById("t" + OwnerID);
					if (tabs)
						result = "" + this.GetCurrentPageIndex(tabs);
				}
				break;
			case P_DAY:
				if (cls_id == CLASS_CALENDAR) {
					var date = $(element).datepicker("getDate");
					result = "" + date.getDate();
				}
				break;
			case P_MONTH:
				if (cls_id == CLASS_CALENDAR) {
					var date = $(element).datepicker("getDate");
					result = "" + (date.getMonth() + 1);
				}
				break;
			case P_YEAR:
				if (cls_id == CLASS_CALENDAR) {
					var date = $(element).datepicker("getDate");
					result = "" + date.getFullYear();
				}
				break;
			case P_VALUE:
				if (cls_id == CLASS_ADJUSTMENT)
					result = "" + element.ConceptValue;
				break;
			case P_LOWER:
				if (cls_id == CLASS_ADJUSTMENT)
					result = "" + element.ConceptLower;
				break;
			case P_UPPER:
				if (cls_id == CLASS_ADJUSTMENT)
					result = "" + element.ConceptUpper;
				break;
			case P_INCREMENT:
				if (cls_id == CLASS_ADJUSTMENT)
					result = "" + element.ConceptIncrement;
				break;
			case P_PAGEINCREMENT:
				if (cls_id == CLASS_ADJUSTMENT)
					result = "" + element.ConceptPageIncrement;
				break;
			case P_PAGESIZE:
				if (cls_id == CLASS_ADJUSTMENT)
					result = "" + element.ConceptPageSize;
				break;
			case P_POSITION:
				if ((cls_id == CLASS_HPANED) || (cls_id == CLASS_VPANED)) {
					var child = element.children[0];
					if (child) {
						if (cls_id == CLASS_HPANED)
							result = "" + child.offsetWidth;
						else
							result = "" + child.offsetHeight;
					}
				}
				break;
			case P_EXPANDED:
				if (cls_id == CLASS_EXPANDER) {
					var content = document.getElementById("c" + OwnerID);
					if (!content)
						content = element.ConceptContainer;

					if (content) {
						if (content.style.display == "none")
							result = "0";
						else
							result = "1";
					}
				}
				break;
			case 10011:
				if (cls_id == 1012)
					result = element.contentWindow.document.body.innerHTML;
				break;
			case 10007:
				if (cls_id == 1012) {
					if (Value.indexOf("document.execCommand('ForeColor', 1") == 0)
						Value = Value.replace("document.execCommand('ForeColor', 1", "document.execCommand('foreColor', 0");
					if (Value.indexOf("document.execCommand('BackColor'") == 0)
						Value = Value.replace("document.execCommand('BackColor'", "document.execCommand('backColor'");
					if (Value.indexOf("document.execCommand('CreateLink'") == 0)
						Value = Value.replace("document.execCommand('CreateLink', 1", "document.execCommand('createLink', 0");
					element.contentWindow.ConceptFoo = undefined;
					element.contentWindow.eval("function ConceptFoo(){" + Value + "}");
					result = element.contentWindow.ConceptFoo();
				}
				break;
			case P_PRESENTYOURSELF:
				result = "";
				for (prop in element) {
					result += prop;
					result += ",,,,,";
					result += element[prop];
					result += "\n";
				}
				break;
			case 10001:
				if (cls_id == 1012)
					result = element.contentWindow.location.href;
				break;
		}
		SendMessageFunction("" + OwnerID, MSG_GET_PROPERTY, "" + PROP_ID, result, 0);
	}

	this.SetChildClass = function(element, classname) {
		var children = element.children;
		var len = children.length;
		for (var i = 0; i < len; i++)
			children[i].className = classname;
	}

	this.GetCurrentPageIndex = function(element) {
		var children = element.children;
		var len = children.length;
		for (var i = 2; i < len; i++) {
			var child = children[i];
			if (child.className.lastIndexOf('active') != -1)
				return i - 2;
		}
		return -1;
	}

	this.GetPrev = function(element, prev) {
		var children = element.children;
		var len = children.length;
		var last = -1;
		for (var i = 2; i < len; i++) {
			var child = children[i];
			if (child.className.lastIndexOf('active') == -1) {
				if (child.style.display != "none")
					last = i;
			} else {
				if (last != -1)
					return last;
			}
		}
		if (last == -1) {
			for (var i = len - 1; i >= 2; i--) {
				child = children[i];
				if (child.style.display != "none") {
					last = i;
					break;
				}
			}
		}
		return last;
	}

	this.GetIndex = function(tabs, tab) {
		var children = tabs.children;
		var len = children.length;
		var last = -1;

		for (var i = 0; i < len; i++) {
			var child = children[i];
			if (child.id == tab.id)
				return i;
		}
		return -1;
	}

	this.GetRowParent = function(children, id) {
		for (var i = 0; i < children.length; i++) {
			var child = children[i];
			if (child.getAttribute("data-row-id") === id)
				return child;
		}
		return null;
	}

	this.GetIndexInParent = function(children, row_id, parent_id) {
		var idx = "";
		var index = -1;
		for (var i = 0; i < children.length; i++) {
			var child = children[i];
			var child_id = child.getAttribute("data-row-id");
			var child_parent_id = child.getAttribute("data-row-parent-id");
			if (parent_id === child_parent_id)
				index++;
			if (child_id === row_id) {
				if (child_parent_id) {
					var parent = this.GetRowParent(children, child_parent_id);
					if (parent)
						return this.GetPath(parent, "tr") + ":" + index;
				}
				return "" + index;
			}
		}
		return "-1";
	}

	this.GetPath = function(node, ename) {
		if (ename == "td")
			node = node.parentNode;

		var parent_id = node.getAttribute("data-row-parent-id");
		var row_id = node.getAttribute("data-row-id");

		var prefix = "";
		var idx = this.GetIndexInParent(node.parentNode.children, row_id, parent_id);
		return idx;//prefix + (node.rowIndex - 1);
	}

	this.UpdateDraggable = function(body, enabled) {
		var children = body.children;
		for (var i = 0; i < children.length; i++) {
			var child = children[i];
			if (enabled)
				$(child).draggable({ 
					revert: "invalid", 
					cancel:false, 
					helper: "clone",
					zIndex: 10000000,
					start: function(event, ui) {
						if (element.ConceptDragBegin)
							self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_DRAGBEGIN, "", 0);
					},
					stop: function(event, ui) {
						if (element.ConceptDragEnd)
							self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_DRAGEND, "", 0);
					}
				});
			else
				$(child).draggable("disable");
		}
	}

	this.GetNodeByParent = function(children, parent, index) {
		if (parent) {
			var pid = parent.getAttribute("data-row-id");
			if (pid) {
				for (var i = 0; i < children.length; i++) {
					var child = children[i];
					var parent_id = child.getAttribute("data-row-parent-id");
					if ((parent_id) && (parent_id == pid)) {
						if (!index)
							return child;
						index--;
					}
				}
			}
		}
		return null;
	}

	this.GetSkip = function(tr, start) {
		var children = tr.parentNode.children;
		var last = tr;
		var last_index = -1;
		var parent_id = tr.getAttribute("data-row-id");
		for (var i = start; i < children.length; i++) {
			var child = children[i];
			if (child.getAttribute("data-row-parent-id") === parent_id) {
				last = child;
				last_index = i;
			}	
		}
		if (last_index >= 0)
			last = this.GetSkip(last, last_index);
		return last;
	}

	this.GetLastNode = function(tr) {
		var children = tr.parentNode.children;
		var last = tr
		var parent_id = tr.getAttribute("data-row-id");
		var last_index = -1;
		for (var i = 0; i < children.length; i++) {
			var child = children[i];
			if (child.getAttribute("data-row-parent-id") === parent_id) {
				last = child;
				last_index = i;
			}
		}
		if (last_index >= 0)
			last = this.GetSkip(last, last_index);

		return last.nextSibling;
	}

	this.DeleteNode = function(tr) {
		var children = tr.parentNode.children;
		var last = tr;
		var parent_id = tr.getAttribute("data-row-id");
		for (var i = 0; i < children.length; i++) {
			var child = children[i];
			if (child.getAttribute("data-row-parent-id") === parent_id) {
				tr.parentNode.removeChild(child);
				i--;
			}
		}
		tr.parentNode.removeChild(tr);
	}

	this.GetNodeByPath = function(body, path) {
		var arr=path.split(":");
		if (arr.length) {
			var index = parseInt(arr[0]);
			var child = null;
			for (var i = 0; i < body.children.length; i++) {
				var child_p = body.children[i];
				var parent_id = child_p.getAttribute("data-row-parent-id");

				if ((typeof parent_id == "undefined") || (parent_id === null)) {
					if (!index) {
						child = child_p;
						break;
					}
					index--;
				}
			}
			if ((child) && (arr.length>1)) {
				for (var i = 1; i < arr.length; i++) {
					child = this.GetNodeByParent(body.children, child, parseInt(arr[i]));
					if (!child)
						return child;
				}
			}
			return child;
		}
		return null;
	}

	this.HideRecursive = function(node, rowid, visible) {
		var children = node.children;
		for (var i = 0; i < children.length; i++) {
			var child = children[i];
			if ((child) && (child.getAttribute("data-row-parent-id") == rowid)) {
				if (visible) {
					//child.style.display = "";
					$(child).show("fade");
				} else {
					//child.style.display = "none";
					$(child).hide("fade");
				}
				if (child.getAttribute("ConceptToggled")!="1")
					this.HideRecursive(node, child.getAttribute("data-row-id"), visible);
			}
		}
	}

	this.ToggleItem = function(element, node, toggled) {
		var row_id = node.getAttribute("data-row-id");
		var val = node.getAttribute("ConceptToggled");
		if (!val)
			val = "0";
		var toggled = parseInt(val);

		this.HideRecursive(node.parentNode, row_id, toggled);
		if (toggled) {
			toggled = "0";
			if (element.ConceptRowExpanded)
				self.SendMessage(element.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_ROWEXPANDED, self.GetPath(node, "tr"), 0);
		} else {
			toggled = "1";
			if (element.ConceptRowCollapsed)
				self.SendMessage(element.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_ROWCOLLAPSED, self.GetPath(node, "tr"), 0);
		}

		node.setAttribute("ConceptToggled", toggled);
	}

	this.UpdateCollapsible = function(element, node, collapsible) {
		if (collapsible)
			collapsible = 1;
		else
			collapsible = 0;

		var is_collapsible = node.getAttribute("ConceptCollapsible");
		if (is_collapsible != collapsible) {
			node.setAttribute("ConceptCollapsible", collapsible);
			var first_td = node.children[0];
			if (first_td) {
				if (collapsible) {
					var a = document.createElement("a");
					if (node.getAttribute("ConceptToggled") == "1")
						a.className = "RTreeViewExpand";
					else
						a.className = "RTreeViewCollapse";
					a.style.textDecoration = "none";
					a.href="#";
					a.onclick = function() {
						if (this.className == "RTreeViewCollapse") {
							this.className = "RTreeViewExpand";
							self.ToggleItem(element, node, "0");
						} else {
							this.className = "RTreeViewCollapse";
							self.ToggleItem(element, node, "1");
						}
						return false;
					}
					first_td.insertBefore(a, first_td.firstChild);
				} else {
					var ref = first_td.child[0];
					if ((ref) && (ref.nodeName.toLowerCase() == "a"))
						first_td.removeChild(ref);
				}
			}
		}
	}

	this.interpolateArray = function(data, fitCount) {
		var linearInterpolate = function (before, after, atPoint) {
			return before + (after - before) * atPoint;
		};

		var newData = new Float32Array(fitCount);
		var springFactor = new Number((data.length - 1) / (fitCount - 1));
		newData[0] = data[0];
		for ( var i = 1; i < fitCount - 1; i++) {
			var tmp = i * springFactor;
			var before = new Number(Math.floor(tmp)).toFixed();
			var after = new Number(Math.ceil(tmp)).toFixed();
			var atPoint = tmp - before;
			newData[i] = linearInterpolate(data[before], data[after], atPoint);
		}
		newData[fitCount - 1] = data[data.length - 1];
		return newData;
	}

	this.AdjustSampleRate = function(arrin, in_rate, out_rate, element, agc, is_in) {
		var avg = 0;
		if ((in_rate) && (out_rate) && (in_rate != out_rate)) {
			var arrout = [];
			var channels = arrin.length;
			for (var k = 0; k < channels; k++) {
				var floatin = arrin[k];

				var len = floatin.length;
				if (!len) {
					arrout[k] = floatin;
					continue;
				}

				var index = 0;
				var out;
				var out_len;
				if (in_rate > out_rate) {
					var ratio = in_rate / out_rate;

					var r = len / ratio;
					if (element.SampleReminder)
						r += element.SampleReminder;

					out_len = Math.floor(r);
					element.SampleReminder = r - out_len;
					out = new Float32Array(out_len);
				} else
				if (in_rate < out_rate) {
					var ratio = out_rate / in_rate;

					var r = len * ratio;
					if (element.SampleReminder)
						r += element.SampleReminder;

					out_len = Math.floor(r);
					element.SampleReminder = r - out_len;

					out = new Float32Array(out_len);
				}
				// ugly but fast
				var coef = len / out_len;
				var ref = 0.01584893192; // 10 ^ (-18/10)
				var m_Gain = element.ConceptGain;
				if (!m_Gain)
					m_Gain = 1;
				var limit = out_len - 1;
				out[0] = floatin[0] * m_Gain;

				var delta = 1 - this.LastAudioLevel * 4;
				if (delta < 0)
					delta = 0.05;

				for (var i = 1; i < limit; i++) {
					var level = floatin[Math.floor(i * coef)];
					if (!is_in) {
						//if (delta > 0.4)
						//	delta = 0.95;

						level *= delta;
					}
					/*if (agc) {
						var level_g = level * m_Gain;
						if (level_g > 1) {
							m_Gain = 1/level;
							level_g = 1;
						}
						out[i] = level_g;

						m_Gain += ref - level_g * level_g;
						if (m_Gain > 3)
							m_Gain = 3;
					} else*/
						out[i] = level;
					avg += Math.abs(level/limit);
				}
				out[limit] = floatin[floatin.length - 1] * m_Gain;
				arrout[k] = out;
				element.ConceptGain = m_Gain;

				// slower
				/*var coef = len / out_len;
				var springFactor = (floatin.length - 1) / (out_len - 1);
				var limit = out_len - 1;
				var ref = 0.01584893192; // 10 ^ (-18/10)
				var m_Gain = element.ConceptGain;
				if (!m_Gain)
					m_Gain = 1;

				out[0] = floatin[0] * m_Gain;
				for (var i = 1; i < limit; i++) {
					var tmp = i * springFactor;
					var before = Math.floor(tmp);
					var after = Math.ceil(tmp);
					var atPoint = tmp - before;

					var level = floatin[before] + (floatin[after] - floatin[before]) * atPoint;
					if (agc) {
						level *= m_Gain;
						out[i] = level;

						m_Gain += ref - level * level;
						if (m_Gain > 8)
							m_Gain = 8;
					} else
						out[i] = level;
				}
				element.ConceptGain = m_Gain;
				out[limit] = floatin[floatin.length - 1] * m_Gain;

				arrout[k] = out;*/

				// slowest
				// arrout[k] = this.interpolateArray(floatin, out_len);
			}
			if (is_in) {
				if ((avg > this.LastAudioLevel) || (this.LastAudioLevel_tail <= 0)) {
					this.LastAudioLevel_tail = 5;
					this.LastAudioLevel = avg;
				}
			} else {
				this.LastAudioLevel_tail--;
				this.LastAudioOutLevel = avg;
			}
			return arrout;
		}
		var channels = arrin.length;
		for (var k = 0; k < channels; k++) {
			var floatin = arrin[k];

			var len = floatin.length;
			for (var i = 0; i < len; i++)
				avg += Math.abs(floatin[i]/len);
		}
		if (is_in)
			this.LastAudioLevel = avg;
		else
			this.LastAudioOutLevel = avg;
		return arrin;
	}

	this.Record = function(audioContext) {
		var hasMicrophoneInput = false;
		try {
			navigator.getUserMedia	=	navigator.getUserMedia ||
							navigator.webkitGetUserMedia ||
							navigator.mozGetUserMedia ||
							navigator.msGetUserMedia;

			var hasMicrophoneInput = (navigator.getUserMedia || navigator.webkitGetUserMedia || navigator.mozGetUserMedia || navigator.msGetUserMedia);
		} catch (e) {
			alert("Audio recording is not supported in your browser");
		}

		if (hasMicrophoneInput) {
			if (!audioContext.ConceptChannels)
				audioContext.ConceptChannels = 1;
			if (!audioContext.ConceptSampleRate)
				audioContext.ConceptSampleRate = 44100;
			if (audioContext.ConceptCompression == 1) {
				var vquality = audioContext.ConceptQuality;
				if (!vquality)
					vquality = 8;

				var framesize = audioContext.ConceptFrameSize;
				if (!framesize)
					framesize = 160;

				audioContext.Speex = new Speex({floating_point: false, sample_rate: audioContext.ConceptSampleRate, quality: vquality, frame_size: framesize});
			} else
			if (audioContext.ConceptCompression == 2) {
				var vquality = audioContext.ConceptQuality;
				if (!vquality)
					vquality = 8;

				var framesize = audioContext.ConceptFrameSize;
				// 2048 = VoIP
				audioContext.OpusEncoder = new OpusEncoder(audioContext.ConceptSampleRate, audioContext.ConceptChannels, 2048, framesize);

				try {
					var i32ptr = libspeex.allocate(4, 'i32', ALLOC_STACK);
					if (!this.SetBandWidth(audioContext)) {
						if (audioContext.ConceptBitRate<=16000) {
							libspeex.setValue(i32ptr, 3001, 'i32');
							audioContext.OpusEncoder.ctl(4024, i32ptr);
							if (audioContext.ConceptBitRate<=8000) {
								libspeex.setValue(i32ptr, 1101, 'i32');
								audioContext.OpusEncoder.ctl(4008, i32ptr);
							}
						} else {
							libspeex.setValue(i32ptr, -1000, 'i32');
							audioContext.OpusEncoder.ctl(4024, i32ptr);
							if (audioContext.ConceptBitRate>8000) {
								libspeex.setValue(i32ptr, -1000, 'i32');
								audioContext.OpusEncoder.ctl(4008, i32ptr);
							}
						}
					}

					if (audioContext.ConceptBitRate) {
						libspeex.setValue(i32ptr, audioContext.ConceptBitRate, 'i32');
						audioContext.OpusEncoder.ctl(4002, i32ptr);
					}
					if (audioContext.ConceptQuality) {
						libspeex.setValue(i32ptr, audioContext.ConceptQuality, 'i32');
						audioContext.OpusEncoder.ctl(4010, i32ptr);
					}
				} catch (e) {
					console.log(e);
				}
			}
			this.MediaAudio = true;
			var script = audioContext.ref.createScriptProcessor(this.GetBufferSize(audioContext), audioContext.ConceptChannels, audioContext.ConceptChannels);
			audioContext.ConceptProcessor = script;
			if (audioContext.ConceptCompression)
				this.InAudioContext = audioContext;
			script.onaudioprocess = function(audioProcessingEvent) {
				if (audioContext.ConceptBufferFull) {
					var channels = audioProcessingEvent.inputBuffer.numberOfChannels;
					var output = [];

					for (var i = 0; i < channels; i++)
						output.push(audioProcessingEvent.inputBuffer.getChannelData(i));

					audioContext.ConceptBufferFull(self.AdjustSampleRate(output, audioContext.ref.sampleRate, audioContext.ConceptSampleRate, audioContext, 0, false), 2);
				}
				if ((self.InAudioContext) && (self.InAudioContext.ConceptAudioPair))
					self.InAudioContext.ConceptAudioPair.ConceptProcess(audioProcessingEvent);
			}

			this.MediaListeners.push(function(stream) {
				var microphone = audioContext.ref.createMediaStreamSource(stream);
				microphone.connect(script);
				audioContext.ConceptMicrophone = microphone;
				script.connect(audioContext.ref.destination);
			});

			if (this.MediaTimer)
				clearTimeout(this.MediaTimer);
			this.MediaTimer = setTimeout(function () {
				self.getUserMediaDo();
			}, 1000);


		}
		return hasMicrophoneInput;
	}

	this.ProcessBuffer = function(buf, channels) {
		var output = [];

		var str = new DataView(buf);//new Int16Array(buf);
		var len_p = str.byteLength;

		var samplesPerChannel = Math.floor(len_p / 2 / channels);
		if (!samplesPerChannel)
			return output;

		for (var j = 0; j < channels; j++)
			output[j] = new Float32Array(samplesPerChannel);

		var index = 0;

		var i = 0;
		while (i < len_p) {
			for (var j = 0; j < channels; j++) {
				var v = str.getUint16(i);
				v = ((v & 0xFF) << 8) | ((v >> 8) & 0xFF)
				if (v > 32768)
					v = v - 65536;

				output[j][index] =  v / 32768; 
				i+=2;
			}
			index++;
		}
		return output;
	}

	this.GetValues = function(output, buffers, count) {
		var index = 0;
		var channels = output.length;
		do {
			if (!buffers.length)
				return;
			var buf = buffers[0];

			if ((buf) && (!buf.length)) {
				buffers.shift();
				continue;
			}

			if ((!buf) || (!buf[0]))
				return;

			var len = buf[0].length;
			var has_remainder;

			var remCount = count - index;
			if (len <= remCount) {
				buffers.shift();
				len_p = len;
				has_remainder = false;
			} else {
				len_p = remCount;
				has_remainder = true;
			}

			for (var i = 0; i < len_p; i++) {
				for (var j = 0; j < channels; j++)
					output[j][index] = buf[j][i];

				index++;
			}

			if (has_remainder) {
				var remSize = len - len_p;
				var rem = [];
				for (var j = 0; j < channels; j++)
					rem[j] = new Float32Array(remSize);

				var remIndex = 0;
				for (var i = len_p; i < len; i++) {
					for (var j = 0; j < channels; j++)
						rem[j][remIndex] = buf[j][i];
					remIndex++;
				}
				buffers[0] = rem;
			}
		} while (index < count);
	}

	this.GetBufferSize = function(audioContext) {
		if (this.ConceptSampleRate < 11025)
			return 256;
		else
		if (this.ConceptSampleRate < 22050)
			return 512;
		else
		if (this.ConceptSampleRate < 44100)
			return 1024;

		return 4096;
	}

	this.Play = function(audioContext) {
		if (!audioContext.ConceptChannels)
			audioContext.ConceptChannels = 1;

		if (audioContext.ConceptCompression == 1) {
			if (!audioContext.ConceptSampleRate)
				audioContext.ConceptSampleRate = 44100;

			var framesize = audioContext.ConceptFrameSize;
			if (!framesize)
				framesize = 160;
			audioContext.Speex = new Speex({floating_point: false, sample_rate: audioContext.ConceptSampleRate, frame_size: framesize});
		} else
		if (audioContext.ConceptCompression == 2) {
			if (!audioContext.ConceptSampleRate)
				audioContext.ConceptSampleRate = 48000;
			audioContext.OpusDecoder = new OpusDecoder(audioContext.ConceptSampleRate, audioContext.ConceptChannels);
		}

		var processFunction= function(audioProcessingEvent) {
			var buf = audioContext.ConceptBuffers;
			var channels = audioProcessingEvent.outputBuffer.numberOfChannels;
			if ((buf) && (buf.length > 0)) {
				var output = [];
				var samples = 0;

				for (var i = 0; i < channels; i++) {
					var data = audioProcessingEvent.outputBuffer.getChannelData(i);
					output.push(data);
					samples = data.length;
				}
				if (channels)
					self.GetValues(output, buf, samples);
			} else {
				for (var i = 0; i < channels; i++) {
					var data = audioProcessingEvent.outputBuffer.getChannelData(i);
					for (var sample = 0; sample < data.length; sample++)
	      					data[sample] = 0;//((Math.random() * 2) - 1) * 0.002;
				}
			}
		}

		var script = audioContext.ref.createScriptProcessor(this.GetBufferSize(audioContext), 0, audioContext.ConceptChannels);
		audioContext.ConceptProcessor = script;
		script.onaudioprocess = processFunction;
		script.connect(audioContext.ref.destination);
	}

	this.UpdateYAlign = function(element) {
		if ((element.ConceptYAlign) && (element.offsetHeight))
			element.style.lineHeight = "" + (element.offsetHeight) + "px";
	}

	this.CloneMenu = function(node) {
		var res = node.cloneNode(false);
		if (node.onclick)
			res.onclick = node.onclick;
		if (res.id)
			res.id += "_c";

		for (var i = 0; i < node.childNodes.length; i++) {
			var child = node.childNodes[i];
			if (child)
				res.appendChild(this.CloneMenu(child));
		}
		return res;
	}

	this.GetSuggestionModel = function(element) {
		var result = [ ];
		var ul = document.getElementById("c" + element.id.substring(1));
		if (ul) {
			for (var i = 0; i < ul.children.length; i++) {
				var child = ul.children[i];
				if (child) {
					var target = child.firstChild.firstChild;
					var text = target.textContent || target.innerText || "";
					if ((text) && (text.length))
						result.push(text);
				}
			}
		}
		return result;
	}

	this.UpdateTextView = function(element, arr2, tag) {
		var nodes = [ ];
		for (var j = 0; j < element.childNodes.length; j++) {
			var child = element.childNodes[j];
			if (child.nodeValue)
				nodes.push(child);
			else
				this.UpdateTextView(child, arr2, tag);
		}
		var child = element.firstChild;
		while (child) {
			if (child.nodeValue != null) {
				var el = child.nodeValue;
				for (var i = 0; i < arr2.length; i++) {
					var exp = "\\b" + arr2[i].replace(/[\-\[\]\/\{\}\(\)\*\+\?\.\\\^\$\|]/g, "\\$&") + "\\b";
					if (tag)
						el = el.replace(new RegExp(exp, 'g'), "<p class='RTextViewTag' style='" + tag.cssText + "'>" + arr2[i] + "</p>");
					else
						el = el.replace(new RegExp(exp, 'g'), "<p class='RTextViewHighlight'>" + arr2[i] + "</p>");				
				}
				if (el != child.nodeValue) {
					var p = document.createElement("p");
					p.className = 'RTextViewTag';
					p.innerHTML = el;
					element.replaceChild(p, child);
					child = p;
				}

			}
			child = child.nextSibling;
		}
	}

	this.SetBandWidth = function(audioContext) {
		if (audioContext.ConceptBandWidth) {
			var i32ptr = libspeex.allocate(4, 'i32', ALLOC_STACK);
			switch (audioContext.ConceptBandWidth) {
				case 1:
					libspeex.setValue(i32ptr, 1101, 'i32');
					audioContext.OpusEncoder.ctl(4008, i32ptr);
					return true;
				case 2:
					libspeex.setValue(i32ptr, 1102, 'i32');
					audioContext.OpusEncoder.ctl(4008, i32ptr);
					return true;
				case 3:
					libspeex.setValue(i32ptr, 1103, 'i32');
					audioContext.OpusEncoder.ctl(4008, i32ptr);
					return true;
				case 4:
					libspeex.setValue(i32ptr, 1105, 'i32');
					audioContext.OpusEncoder.ctl(4008, i32ptr);
					return true;
				case 5:
					libspeex.setValue(i32ptr, 1105, 'i32');
					audioContext.OpusEncoder.ctl(4008, i32ptr);
					return true;
			}
		}
		return false;
	}

	this.UpdateTableHeader = function(element) {
		if (element.scrollTop > 0) {
			element.firstChild.ConceptHeader.style.position = "relative";
			element.firstChild.ConceptHeader.style.top = "" + element.scrollTop + "px";
			if ((!self.isFF) && (element.firstChild.offsetParent) /*&& (element.firstChild.ConceptHeader.style.display != "table-caption")*/) {
				var children = element.firstChild.ConceptHeaderRow.children;
				var children2 = element.firstChild.ConceptTableRows.children[0];
				if (children2){
					var len = children.length;
					for (var i = 0; i < len; i++) {
						var child = children[i];			
						var child2 = children2.children[i];

						if ((child.style.width) && (!child.ConceptPushWidth) && (!child.ConceptSetWidth))
							child.ConceptPushWidth = child.style.width;
						child.ConceptSetWidth = true;
						if ((child2) && (child2.clientWidth > child.clientWidth))
							child.style.width = "" + child2.clientWidth + "px";
						else
							child.style.width = "" + child.clientWidth + "px";
						if (child2)
							child2.style.width = child.style.width;
					}
					element.firstChild.ConceptHeader.style.display = "table-caption";
				}
			}
		} else {
			element.firstChild.ConceptHeader.style.position = "";
			element.firstChild.ConceptHeader.style.top = "";
			if ((!self.isFF) && (element.firstChild.ConceptHeader.style.display == "table-caption")) {
				element.firstChild.ConceptHeader.style.display = "";
				var children = element.firstChild.ConceptHeaderRow.children;
				var len = children.length;
				for (var i = 0; i < len; i++) {
					var child = children[i];
					if (child.ConceptSetWidth) {
						if (child.ConceptPushWidth) {
							child.style.width = child.ConceptPushWidth;
							delete child.ConceptPushWidth;
						} else
							delete child.style.width;
						delete child.ConceptSetWidth;
					}
				}
			}
		}
	}

	this.getUserMediaDo = function() {
		delete this.MediaTimer;
		navigator.getUserMedia = navigator.getUserMedia || navigator.webkitGetUserMedia || navigator.mozGetUserMedia || navigator.msGetUserMedia;
		if (!navigator.getUserMedia) {
			console.error("getUserMedia not supported on this browser");
			this.MediaListeners = [ ];
			return;
		}
		var params = {"audio": this.MediaAudio, "googEchoCancellation": true, "googNoiseSupression": true, "googAutoGainControl": true};
		if ((this.MediaVideo) && (!this.isEDGE))
			params["video"] = this.MediaVideo;
		this.MediaObject = navigator.getUserMedia(params, function(stream) {
				for (var i = 0; i < self.MediaListeners.length; i++) {
					var listener = self.MediaListeners[i];
					listener(stream);
				}
				this.MediaListeners = [ ];
			},
			function(error) {
				console.error("getUserMedia error: " + error.name ? error.name : error.code); 
				this.MediaListeners = [ ];
			}
		);
		this.MediaVideo = false;
		this.MediaAudio = false;
	}

	this.SetProperty = function(OwnerID, PROP_ID, Value, ValueBuffer) {
		var element = this.Controls[OwnerID];
		var label;

		if (!element) {
			console.warn("SetProperty: Unknown object RID: " + OwnerID);
			return;
		}

		var cls_id = element.ConceptClassID;
		if (!cls_id)
			cls_id = parseInt(element.getAttribute("ConceptClassID"));

		switch (PROP_ID) {
			case P_FOCUS:
				if (parseInt(Value)) {
					if ((cls_id == CLASS_EDIT) || (cls_id == CLASS_COMBOBOXTEXT))
						element.firstChild.children[1].focus();
					else
						element.focus();
				}
				break;
			case P_ADDTEXT:
				if (cls_id == CLASS_TEXTVIEW)
					element.appendChild(document.createTextNode(Value));
				break;
			case P_VISIBLE:
				var v = parseInt(Value);
				if (cls_id == CLASS_TEXTTAG) {
					if (v)
						element.cssText = "display: inline;";
					else
						element.cssText = "display: none;";
					break;
				}
				var tab = document.getElementById("l_tab" + OwnerID);
				if (v == 0) {
					if (tab) {
						tab.style.display = "none";
						if (tab.className.lastIndexOf('active') >= 0) {
							var tabs = document.getElementById("t" + OwnerID)
							var prev = this.GetPrev(tab.parentNode, tab);
							$('#r' + tab.getAttribute("ConceptParentID") + ' li:eq(' + prev + ') a').tab('show');

							var parent_id = element.getAttribute("ConceptParentID");
							var parent = this.Controls[parseInt(parent_id)];
							if (parent)
								this.PageChangedNotify(parent, prev - 2);
						}
					}
					if (element.style.dispaly != "none") {
						if (element.ConceptVisibility) 
							self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_VISIBILITY, "0", 0);
						if (element.ConceptHide) 
							self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_HIDE, "", 0);
					}
					element.style.display = "none";
					if ((cls_id == CLASS_FORM) && (this.MainForm) && (this.MainForm.id == element.id)) {
						if (self.Container)
							self.Container.style.display = "none";
						if (self.LoadingContainer)
							self.Container.style.display = "block";
						self.MainForm = null;
						self.CleanClose = true;
						self.NotifyLoading("Done");
						self.ConceptSocket.close();
					}
				} else {
					if (element.style.dispaly == "none") {
						if (element.ConceptVisibility) 
							self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_VISIBILITY, "1", 0);
						if (element.ConceptShow)
							self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_SHOW, "", 0);
					}

					if (v == -1)
						element.style.visibility = "hidden";
					else
						element.style.visibility = "";

					if (cls_id == CLASS_FORM) {
						element.style.display = "block";
						element.style.zIndex = this.zIndexIndex++;
					} else
						element.style.display = "";//element.getAttribute("OrigDisplay");
					if (element.ConceptYAlign) {
						setTimeout(function() {
							self.UpdateYAlign(element);
						}, 20);
					}
					if (tab) {
						tab.style.display = "";

						var parent_id = element.getAttribute("ConceptParentID");
						var parent = this.Controls[parseInt(parent_id)];
						if ((parent) && (!parent.ConceptPageSet)) {
							parent.ConceptPageSet = 1;
							var key = "t" + parent_id + "_" + OwnerID;
							var page = document.getElementById(key);
							$('#r' + parent_id + ' a[href="#' + key + '"]').tab('show');
						}
					}
					if (cls_id == 1000) {
						// some problem with ace code editor
						setTimeout(function () {
							window.dispatchEvent(new Event('resize'));
						}, 1000);
						break;
					}
				}
				this.UpdateScrolledWindowSize(100);
				break;
			case P_MARKUP:
				element.ConceptMarkup = parseInt(Value);
				if (element.ConceptText) {
					var label = element;
					if (cls_id == CLASS_EXPANDER)
						label = document.getElementById("b" + OwnerID);
					else
					if (cls_id == CLASS_FRAME)
						label = document.getElementById("l" + OwnerID);

					if (label) {
						if (element.ConceptMarkup) {
							label.innerHTML = element.ConceptText.replace("\n", "<br/>");
							this.UpdateLabelMarkup(OwnerID, element, EVENT_ON_CLICKED);
						} else {
							label.innerHTML = "";
							label.appendChild(document.createTextNode(element.ConceptText));
						}
					}
				}
				break;
			case P_CAPTION:
				switch (cls_id) {
					case CLASS_LABEL:
					case CLASS_STATUSBAR:
					case CLASS_TEXTVIEW:
						element.ConceptText = Value;
						if (element.ConceptMarkup) {
							element.innerHTML = Value.replace("\n", "<br/>");
							this.UpdateLabelMarkup(OwnerID, element, EVENT_ON_CLICKED);
						} else {
							element.innerHTML = "";
							element.appendChild(document.createTextNode(Value));
						}
						break;
					case CLASS_EDIT:
					case CLASS_COMBOBOXTEXT:
						label = element.firstChild.children[1];
						if (label)
							label.value = Value;
						break;
					case CLASS_BUTTON:
						label = element.children[1];//document.getElementById("l" + OwnerID);
						if (label) {
							if (element.getAttribute("ConceptStock") == "1")
								this.UpdateButton(element, OwnerID, Value);
							else {
								Value = this.ParseUnderline(Value);
								if (element.ConceptMarkup) {
									label.innerHTML = Value.replace("\n", "<br/>");
								} else {
									label.innerHTML = "";
									label.appendChild(document.createTextNode(Value));
								}
							}
						}
						break;
					case CLASS_CHECKBUTTON:
					case CLASS_RADIOBUTTON:
					case CLASS_PROGRESSBAR:
					case CLASS_TOOLBUTTON:
					case CLASS_RADIOTOOLBUTTON:
					case CLASS_TOGGLETOOLBUTTON:
					case CLASS_MENUTOOLBUTTON:
						Value = this.ParseUnderline(Value);
					case CLASS_FRAME:
						label = document.getElementById("l" + OwnerID);
						if (label) {
							element.ConceptText = Value;
							if (element.ConceptMarkup) {
								label.innerHTML = Value;
							} else {
								label.innerHTML = "";
								label.appendChild(document.createTextNode(Value));
							}
							if ((cls_id == CLASS_CHECKBUTTON) || (cls_id == CLASS_RADIOBUTTON)) {
								if (Value.length)
									label.style.display = "";
								else
									label.style.display = "none";
							}
						}
						break;
					case CLASS_SPINBUTTON:
						label = document.getElementById("l" + OwnerID);
						if (label)
							label.value = parseInt(Value);
						break;
					case CLASS_EXPANDER:
						label = document.getElementById("b" + OwnerID);
						if (label) {
							element.ConceptText = Value;
							if (element.ConceptMarkup) {
								label.innerHTML = Value.replace("\n", "<br/>");
							} else {
								label.innerHTML = "";
								label.appendChild(document.createTextNode(Value));
							}
						}
						break;
					case CLASS_FORM:
						label = document.getElementById("t" + OwnerID);
						if (label) {
							label.innerHTML = "";
							label.appendChild(document.createTextNode(Value));
						} else
							document.title = Value;
						break;
					case CLASS_MENU:
					case CLASS_IMAGEMENUITEM:
					case CLASS_STOCKMENUITEM:
					case CLASS_CHECKMENUITEM:
					case CLASS_MENUITEM:
					case CLASS_TEAROFFMENUITEM:
					case CLASS_RADIOMENUITEM:
					case CLASS_SEPARATORMENUITEM:
						label = document.getElementById("t" + OwnerID);
						Value = this.ParseUnderline(Value);
						if (label) {
							if (element.ConceptMarkup) {
								label.innerHTML = Value.replace("\n", "<br/>");
							} else {
								label.innerHTML = "";
								label.appendChild(document.createTextNode(Value));
							}
						}
						break;
					case CLASS_HTMLSNAP:
						element.innerHTML = Value;
						break;
					case 1001:
					case 1012:
						if (element.ConceptCache) {
							for(var key in element.ConceptCache)
								Value = Value.replace(key, this.GetCookie(key, element.ConceptCache));
						}
						if ((element.contentWindow) && (element.contentWindow.document.documentElement))
							element.contentWindow.document.removeChild(element.contentWindow.document.documentElement);
						if ((element.contentWindow) && ((this.isIE) || (Value.indexOf("internal://") > 0))) {
							element.contentWindow.document.close();
							element.contentWindow.document.open();
							element.contentWindow.document.write(Value);
							self.UpdateLabelMarkup(OwnerID, element.contentWindow.document.documentElement, 360);
							element.contentWindow.document.close();
						} else {
							element.src = "data:text/html;charset=utf-8," + encodeURIComponent(Value);
						}
						element.onload = function() {
							self.UpdateLabelMarkup(OwnerID, this.contentWindow.document.documentElement, 360);
							if ((element.contentEditable == "true") && (this.contentWindow.document.body))
								this.contentWindow.document.body.contentEditable = true;
						}
						break;
					case 1000:
						if (element.ConceptEditor)
							element.ConceptEditor.getSession().setValue(Value);
						self.UpdateScrolledWindowSize(1);
						break;
				}
				break;
			case P_KEEPABOVE:
				if (parseInt(Value))
					element.style.zIndex = "10000";
				else
					element.style.zIndex = "";
				break;
			case P_KEEPBELOW:
				if (parseInt(Value))
					element.style.zIndex = "-10000";
				else
					element.style.zIndex = "";
				break;
			case P_TITLEBAR:
				if (cls_id == CLASS_FORM) {
					var header = document.getElementById("h" + OwnerID);
					if (header) {
						if (parseInt(Value)) {
							header.style.display = "";
						} else {
							header.style.display = "none";
						}
					}
				}
				break;
			case P_DECORATIONS:
				if (cls_id == CLASS_FORM) {
					var header = document.getElementById("h" + OwnerID);
					if (header) {
						var flags = parseInt(Value);
						if (flags & (1<<1)) {
							header.style.display = "none";
						} else
						if (flags & (1<<8)) {
							header.style.display = "none";
						} else
							header.style.display = "";
					}
				}
				break;
			case P_TYPE:
				if (cls_id == CLASS_FORM) {
					var header = document.getElementById("h" + OwnerID);
					if (header) {
						var type = parseInt(Value);
						if (type == 4) {
							header.style.display = "";
						} else {
							header.style.display = "none";
						}
					}
				}
				break;
			case P_MAXLEN:
				if ((cls_id == CLASS_EDIT) || (cls_id == CLASS_COMBOBOXTEXT)) {
					var label = element.firstChild.children[1];
					if (label)
						label.maxLength = parseInt(Value);
				} else
				if ((cls_id == 1016) || (cls_id == 1017))
					element.ConceptMaxBuffers = parseInt(Value);
				break;
			case P_MASKED:
				if ((cls_id == CLASS_EDIT) || (cls_id == CLASS_COMBOBOXTEXT)) {
					var label = element.firstChild.children[1];
					if (label) {
						if (parseInt(Value))
							label.type = "password";
						else
							label.type = "text";
					}
				}
				break;
			case P_GROUP:
				if (cls_id == CLASS_RADIOBUTTON) {
					var input = document.getElementById("i" + OwnerID);
					if (input)
						input.name = "r"+Value;
				} else
				if ((cls_id == CLASS_RADIOTOOLBUTTON) || (cls_id == CLASS_RADIOMENUITEM)) {
					if (element.ConceptGroupWith)
						element.ConceptGroupWith = null;

					var gwith = this.Controls[Value];
					if (gwith) {
						if (!gwith.ConceptGroupWith) {
							gwith.ConceptGroupWith = [ ];
							gwith.ConceptGroupWith.push(gwith);
						}
						element.ConceptGroupWith = gwith.ConceptGroupWith;
						gwith.ConceptGroupWith.push(element);
					}
				}
				break;
			case P_CHECKED:
				switch (cls_id) {
					case CLASS_CHECKBUTTON:
					case CLASS_RADIOBUTTON:
						var input = document.getElementById("i" + OwnerID);
						if (input) {
							if (parseInt(Value))
								input.checked = true;
							else
								input.checked = false;
						}
						break;
					case CLASS_TOGGLETOOLBUTTON:
					case CLASS_RADIOTOOLBUTTON:
						var t = parseInt(Value);
						if (element.ConceptChecked != t) {
							element.ConceptChecked = t;
							if (element.ConceptChecked) {
								element.classList.add("btn-primary");
								if (this.ConceptGroupWith) {
									for (var i = 0; i < this.ConceptGroupWith.length; i++) {
										var w = this.ConceptGroupWith[i];
										if ((w) && (w != this)) {
											w.ConceptChecked = 0;
											w.classList.remove("btn-primary");
										}
									}
								}
							} else
								element.classList.remove("btn-primary");

							if (element.ConceptToggled)
								this.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_TOGGLED, "" + element.ConceptChecked, 0);
						}
						break;
					case CLASS_CHECKMENUITEM:
					case CLASS_RADIOMENUITEM:
						var t = parseInt(Value);
						if (element.ConceptChecked != t) {
							element.ConceptChecked = t;
							if (element.ConceptChecked) {
								element.classList.add("RMenuItemChecked");
								if (this.ConceptGroupWith) {
									for (var i = 0; i < this.ConceptGroupWith.length; i++) {
										var w = this.ConceptGroupWith[i];
										if ((w) && (w != this)) {
											w.ConceptChecked = 0;
											w.classList.remove("RMenuItemChecked");
										}
									}
								}
							} else
								element.classList.remove("RMenuItemChecked");

							if (element.ConceptToggled)
								this.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_TOGGLED, "" + element.ConceptChecked, 0);
						}
						break;
				}
				break;
			case P_ANGLE:
				var angle = "rotate(" + parseInt(Value) + "deg)";
				element.style.webkitTransform = angle;
				element.style.MozTransform = angle;
				element.style.msTransform = angle;
				element.style.OTransform = angle;
				element.style.transform = angle;
				break;
			case P_ENABLED:
				switch (cls_id) {
					case CLASS_EDIT:
					case CLASS_COMBOBOXTEXT:
						if (parseInt(Value)) {
							for (var i = 0; i < element.firstChild.children.length; i++)
								element.firstChild.children[i].removeAttribute("disabled");
						} else {
							for (var i = 0; i < element.firstChild.children.length; i++)
								element.firstChild.children[i].setAttribute("disabled", "disabled");
						}
						break;
					case CLASS_TEXTVIEW:
						if (parseInt(Value))
							element.contentEditable = true;
						else
							element.contentEditable = false;
						break;
					case CLASS_CHECKBUTTON:
					case CLASS_RADIOBUTTON:
					case CLASS_SPINBUTTON:
						var label = element.firstChild;
						if (label) {
							if (parseInt(Value))
								label.removeAttribute("disabled");
							else
								label.setAttribute("disabled", "disabled");
						}
						break;
					default:
						if (parseInt(Value))
							element.removeAttribute("disabled");
						else
							element.setAttribute("disabled", "disabled");
				}
				break;
			case P_FRACTION:
				switch (cls_id) {
					case CLASS_PROGRESSBAR:
						label = document.getElementById("l" + OwnerID);
						if (label) {
							var val = parseFloat(Value);
							label.style.width = "" + val + "%";
							label.setAttribute("aria-valuenow", "" + val);
						}
						break;
					case CLASS_EDIT:
					case CLASS_COMBOBOXTEXT:
						element = element.firstChild.children[1];
						var step = parseFloat(Value) * 100;
						if (step >= 100)
							element.style.background = "linear-gradient(to right, #d3d3d3 " + step + "%, transparent " + step + "%)";
						else
						if (step)
							element.style.background = "linear-gradient(to right, #dedede " + step + "%, transparent " + step + "%)";
						else
							element.style.background = "";
						break;
				}
				break;
			case P_PACKING:
				this.Packing(element, parseInt(Value));
				break;
			case P_PACKTYPE:
				element.ConceptPackType = parseInt(Value);
				break;
			case P_IMAGE:
			case P_ICON:
				var iconsize = -1;
				var iconlabel = null;
				switch (cls_id) {
					case CLASS_TOOLBUTTON:
					case CLASS_RADIOTOOLBUTTON:
					case CLASS_TOGGLETOOLBUTTON:
					case CLASS_MENUTOOLBUTTON:
						iconsize = 2;
						var parent_id = element.getAttribute("ConceptParentID");
						if (parent_id) {
							var parent = this.Controls[parent_id];
							var i_size_ref = parent.getAttribute("ConceptIconSize");
							if (i_size_ref)
								iconsize = parseInt(i_size_ref);
						}
						iconlabel = document.getElementById("l" + OwnerID);
					case CLASS_BUTTON:
					case CLASS_IMAGEMENUITEM:
					case CLASS_RADIOMENUITEM:
					case CLASS_CHECKMENUITEM:
					case CLASS_MENUITEM:
					case CLASS_TEAROFFMENUITEM:
					case CLASS_STOCKMENUITEM:
						var image = document.getElementById("i" + OwnerID);
						if (image) {
							var source = this.Controls[Value];//document.getElementById("r" + parseInt(Value));
							if (source) {
								switch (iconsize) {
									case -1:
										image.style.width = source.style.width;
										image.style.height = source.style.height;
										break;
									case 0:
									case 1:
										image.style.width = "16px";
										image.style.height = "16px";
										break;
									case 2:
										image.style.width = "24px";
										image.style.height = "24px";
										break;
									case 3:
									case 4:
										image.style.width = "32px";
										image.style.height = "32px";
										break;
									default:
										image.style.width = "" + iconsize + "px";
										image.style.height = image.style.width;
										break;
								}
								image.src = source.src;
								image.style.display = "";
								if (iconlabel)
									iconlabel.style.lineHeight = image.style.height;
							} else
								image.style.display = "none";
						}
						break;
					case CLASS_FORM:
						var link = document.getElementById("favicon");
						if (!link) {
							link = document.createElement('link');
							link.type = 'image/x-icon';
							link.id = "favicon";
							link.rel = 'shortcut icon';
							document.getElementsByTagName('head')[0].appendChild(link);
						}

						var source = this.Controls[Value];//document.getElementById("r" + parseInt(Value));
						if (source)
							link.href = source.src;
						else
							link.href = "";
						break;
					case CLASS_IMAGE:
						var source = this.Controls[Value];
						if ((source) && (source.ConceptClassID == CLASS_IMAGE))
							element.src = source.src;
						else
							element.src = "";
						break;
				}
				break;
			case P_BACKGROUNDIMAGE:
				var source = this.Controls[Value];//document.getElementById("r" + parseInt(Value));
				if (source) {
					if (source.src)
						element.style.backgroundImage = "url('" + source.src +"')";
					else {
						element.style.backgroundImage = "url('" + source.src +"')";
						source.addEventListener('load', function() {
							element.style.backgroundImage = "url('" + this.src +"')";
						});
					}
				} else
					element.style.backgroundImage = "";
				break;
			case P_ORIENTATION:
				if (cls_id == CLASS_TOOLBAR) {
					if (parseInt(Value) == ORIENTATION_VERTICAL) {
						element.className = "RToolbarVertical";
						element.setAttribute("ConceptVertical", "1");
						this.SetChildClass(element, "btn-group-vertical");
					} else {
						element.className = "btn-toolbar RToolbar";
						element.setAttribute("ConceptVertical", "0");
						this.SetChildClass(element, "btn-group");
					}
				} else
				if (cls_id == CLASS_ICONVIEW) {
					if (parseInt(Value) == ORIENTATION_VERTICAL)
						element.setAttribute("ConceptVertical", "1");
					else
						element.setAttribute("ConceptVertical", "0");
				}
				break;
			case P_PAGE:
				if (cls_id == CLASS_NOTEBOOK) {
					element.PageIndexSet = 1;
					var tabs = document.getElementById("t" + OwnerID);
					if (tabs) {
						var index = parseInt(Value) + 2;
						var tab = tabs.children[index];
						if ((tab) && (tab.firstChild)) {
							$(tab.firstChild).tab('show');
							this.PageChangedNotify(element, index - 2);
							//$('#r' + OwnerID + ' li:eq(' + (parseInt(Value) + 2) + ') a').tab('show');
						}
					}
				}
				break;
			case P_SCROLLABLE:
				if (cls_id == CLASS_NOTEBOOK) {
					if (parseInt(Value)) {
						//var tabs = document.getElementById("t" + OwnerID);
						//if (tabs)
						//	$(tabs).tabdrop();
					}
				}				
				break;
			case P_USESTOCK:
				switch (cls_id) {
					case CLASS_BUTTON:
						var use = parseInt(Value);
						element.setAttribute("ConceptStock", use);
						label = document.getElementById("l" + OwnerID);
						if (label) {
							var text = label.innerHTML;
							if (text)
								this.UpdateButton(element, OwnerID, text);
						}
						break;
					case CLASS_TOOLBUTTON:
					case CLASS_RADIOTOOLBUTTON:
					case CLASS_TOGGLETOOLBUTTON:
					case CLASS_MENUTOOLBUTTON:
						var image = document.getElementById("i" + OwnerID);
						if (image) {
							var iconsize = 2;
							var parent_id = element.getAttribute("ConceptParentID");
							if (parent_id) {
								var parent = this.Controls[parent_id];//document.getElementById("r" + parent_id);
								var i_size_ref = parent.getAttribute("ConceptIconSize");
								if (i_size_ref)
									iconsize = parseInt(i_size_ref);
							}

							switch (iconsize) {
								case 0:
								case 1:
									image.style.width = "16px";
									image.style.height = "16px";
									break;
								case 2:
									image.style.width = "24px";
									image.style.height = "24px";
									break;
								case 3:
								case 4:
									image.style.width = "32px";
									image.style.height = "32px";
									break;
								default:
									image.style.width = "" + iconsize + "px";
									image.style.height = image.style.width;
									break;
							}
							image.src = this.ResourceRoot + "/" + this.AdjustStock(Value) + ".png";
							image.style.display = "";
						}
						break;
				}
				break;
			case P_ICONSIZE:
				element.setAttribute("ConceptIconSize", parseInt(Value));
				break;
			case P_NORMAL_BG_COLOR:
				if (cls_id == CLASS_FORM) {
					var e2 = document.getElementById("f" + OwnerID);
					if (e2)
						element = e2;
				} else
				if ((cls_id == CLASS_EDIT) || (cls_id == CLASS_COMBOBOXTEXT))
					element = element.firstChild.children[1];
				else
				if (cls_id == CLASS_SPINBUTTON)
					element = element.firstChild;

				if (Value != "")
					element.style.backgroundColor = this.doColorString(parseInt(Value));
				else
					element.style.backgroundColor = "";
				break;
			case P_INACTIVE_BG_COLOR:
			case P_SELECTED_BG_COLOR:
				break;
			case P_NORMAL_FG_COLOR:
				if ((cls_id == CLASS_EDIT) || (cls_id == CLASS_COMBOBOXTEXT))
					element = element.firstChild.children[1];
				else
				if (cls_id == CLASS_SPINBUTTON)
					element = element.firstChild;
				if (cls_id == CLASS_BUTTON)
					element = element.children[1];
				if (Value != "")
					element.style.color = this.doColorString(parseInt(Value));
				else
					element.style.color = "";
				break;
			case P_INACTIVE_FG_COLOR:
			case P_SELECTED_FG_COLOR:
				break;
			case P_COLUMNS:
				if ((cls_id == CLASS_TABLE) || (cls_id == CLASS_ICONVIEW)) {
					var v = parseInt(Value);
					element.setAttribute("ConceptColumns", v);
				}
				break;
			case P_ROWS:
				if (cls_id == CLASS_TABLE) {
					var v = parseInt(Value);
					element.setAttribute("ConceptRows", v);
					if (v <= 1)
						element.style.height = "100%";
					else
						element.style.height = "0px";
				}
				break;
			case P_ATTLEFT:
				if (cls_id == CLASS_TABLE)
					element.setAttribute("ConceptAttLeft", parseInt(Value));
				break;
			case P_ATTRIGHT:
				if (cls_id == CLASS_TABLE)
					element.setAttribute("ConceptAttRight", parseInt(Value));
				break;
			case P_ATTTOP:
				if (cls_id == CLASS_TABLE)
					element.setAttribute("ConceptAttTop", parseInt(Value));
				break;
			case P_ATTBOTTOM:
				if (cls_id == CLASS_TABLE)
					element.setAttribute("ConceptAttBottom", parseInt(Value));
				break;
			case P_ATTHOPT:
				if (cls_id == CLASS_TABLE)
					element.setAttribute("ConceptAttHOpt", parseInt(Value));
				break;
			case P_ATTVOPT:
				if (cls_id == CLASS_TABLE)
					element.setAttribute("ConceptAttVOpt", parseInt(Value));
				break;
			case P_ATTHSPACING:
				if (cls_id == CLASS_TABLE)
					element.setAttribute("ConceptAttHSpacing", parseInt(Value));
				break;
			case P_ATTVSPACING:
				if (cls_id == CLASS_TABLE)
					element.setAttribute("ConceptAttVSpacing", parseInt(Value));
				break;
			case P_RELIEFSTYLE:
				if (cls_id == CLASS_BUTTON) {
					if (parseInt(Value) == 2)
						element.className = "RNoRelief";
					else
						element.className += " RButton";
				}
				break;
			case P_MINWIDTH:
				element.style.minWidth = "" + parseInt(Value) + "px";
				break;
			case P_MINHEIGHT:
				element.style.minHeight = "" + parseInt(Value) + "px";
				break;
			case P_FIXED:
				element.ConceptFixed = true;
				break;
			case P_LEFT:
				if (cls_id != CLASS_FORM) {
					var parent = element.getAttribute("ConceptParentID");
					var p_control = this.Controls[parseInt(parent)];
					if ((element.ConceptFixed) || ((p_control) && (p_control.ConceptClassID == CLASS_FIXED))) {
						element.style.position = "absolute";
						element.style.left = "" + parseInt(Value);
					}
				}
				break;
			case P_TOP:
				if (cls_id != CLASS_FORM) {
					var parent = element.getAttribute("ConceptParentID");
					var p_control = this.Controls[parseInt(parent)];
					if ((element.ConceptFixed) || ((p_control) && (p_control.ConceptClassID == CLASS_FIXED))) {
						element.style.position = "absolute";
						element.style.top = "" + parseInt(Value);
					}
				}
				break;
			case P_WIDTH:
				element.ConceptWidth = parseInt(Value);
				if (cls_id == CLASS_FORM) {
					var val = element.ConceptWidth;

					var e2 = document.getElementById("d" + OwnerID);
					// set width only for child forms
					if (e2) {
						element = e2;
						element.ConceptWidth = val;
					} else
						break;
					element.style.width = "0px";
					if (val > screen.width - 50)
						val = screen.width - 50;

					element.style.minWidth = "" + val + "px";
				} else
				if (cls_id != CLASS_TOOLBAR) {
					var parent = element.getAttribute("ConceptParentID");
					if (parent) {
						var p_control = this.Controls[parseInt(parent)];
						if ((p_control) && (p_control.ConceptClassID == CLASS_NOTEBOOK)) {
							element.style.minWidth = "" + element.ConceptHeight + "px";
							break;
						}
					}
					element.style.width = "" + element.ConceptWidth + "px";
				}
				break;
			case P_HEIGHT:
				element.ConceptHeight = parseInt(Value);
				if (cls_id == CLASS_FORM) {
					var val = element.ConceptHeight;
					var e2 = document.getElementById("d" + OwnerID);
					if (e2) {
						element = e2;
						element.ConceptHeight = val;
					}
					/*element.style.height = "0px";

					if (val > screen.height - 100)
						val = screen.height - 100;

					element.style.minHeight = "" + val + "px";*/
				} else
				if (cls_id != CLASS_TOOLBAR) {
					var parent = element.getAttribute("ConceptParentID");
					if (parent) {
						var p_control = this.Controls[parseInt(parent)];
						if ((p_control) && (p_control.ConceptClassID == CLASS_NOTEBOOK)) {
							element.style.minHeight = "" + element.ConceptHeight + "px";
							break;
						}
					}
					element.style.height = "" + element.ConceptHeight + "px";
				}
				break;
			case P_SHOWTABS:
				if (cls_id == CLASS_NOTEBOOK) {
					var tabs = document.getElementById("t" + OwnerID);
					if (tabs) {
						if (parseInt(Value))
							tabs.style.display = "";
						else
							tabs.style.display = "none";
					}
				}
				break;
			case P_TABBORDER:
			case P_SHOWBORDER:
				if (cls_id == CLASS_NOTEBOOK)
					element.style.border = "" + parseInt(Value) + "px";
				break;
			case P_TABPOS:
				if (cls_id == CLASS_NOTEBOOK) {
					var tabs = element.ConceptTabs;
					if (!tabs)
						tabs = document.getElementById("t" + OwnerID);
					var pages = element.ConceptPages;
					if (!pages)
						pages = document.getElementById("c" + OwnerID);

					if ((tabs) && (pages)) {
						switch (parseInt(Value)) {
							case POS_LEFT:
								element.className = "RNotebook tabbable tabs-left";
								element.innerHTML = "";
								element.appendChild(tabs);
								element.appendChild(pages);
								break;
							case POS_RIGHT:
								element.className = "RNotebook tabbable tabs-right";
								element.innerHTML = "";
								element.appendChild(pages);
								element.appendChild(tabs);
								break;
							case POS_TOP:
								element.className = "RNotebook tabbable";
								element.innerHTML = "";
								element.appendChild(tabs);
								element.appendChild(pages);
								break;
							case POS_BOTTOM:
								element.className = "RNotebook tabbable tabs-below";
								element.innerHTML = "";
								element.appendChild(pages);
								element.appendChild(tabs);
								break;
						}
					}
				}
				break;
			case P_MAXIMIZED:
				if ((cls_id == CLASS_FORM) && (parseInt(Value))) {
					var form = document.getElementById("d" + OwnerID);
					if (form) {
						form.style.width = "100%";
						form.style.height = "100%";
					} else {
						element.style.width = "auto";
						element.style.height = "auto";
					}
				}
				break;
			case P_DRAGOBJECT:
				if (parseInt(Value)) {
					if (cls_id == CLASS_TREEVIEW) {
						// to do
						body = document.getElementById("c" + OwnerID);
						if (body)
							this.UpdateDraggable(body, true);
					} else
						$(element).draggable({
							revert: "invalid",
							cancel:false,
							start: function(event, ui) {
								if (element.ConceptDragBegin)
									self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_DRAGBEGIN, "", 0);
							},
							stop: function(event, ui) {
								if (element.ConceptDragEnd)
									self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_DRAGEND, "", 0);
							}
						});
					element.ConceptDraggable = true;
				} else {
					if (clsid == CLASS_TREEVIEW) {
						body = document.getElementById("c" + OwnerID);
						if (body)
							this.UpdateDraggable(body, false);
					} else
						$(element).draggable("disable");
					element.ConceptDraggable = false;
				}
				break;
			case P_DRAGICON:
				break;
			case P_DRAGDATA:
				element.setAttribute("ConceptDragData", Value);
				break;
			case P_DROPSITE:
				switch (parseInt(Value)) {
					case 0:
						// unset
						$(element).droppable("disable");
						break;
					/*case 2:
						// files
						$(element).droppable();
						break;
					case 3:
						// files + strings
						$(element).droppable();
						break;*/
					case 1:
					default:
						// strings
						$(element).droppable({
							tolerance: "touch",
							greedy: true,
							drop: function(event, ui) {
								var dragdata = "";
								var position = "-1";
								var node = ui.draggable.context;
								var index= -1;
								//if ((cls_id == CLASS_TREEVIEW) || (cls_id == CLASS_ICONVIEW))
								//	index = Array.prototype.indexOf.call(node.parentNode.childNodes, node);
								while (node) {
									if ((node.id) && (node.id[0] == "r")) {
										var drag = node.getAttribute("ConceptDragData");
										if (drag)
											dragdata = drag;
										node = self.Controls[parseInt(node.id.substring(1))];
										break;
									}
									node = node.parentNode;
								}
								if (ui.draggable.context) {
									if (ui.draggable.context.nodeName.toLowerCase() == "tr") {
										index = self.GetPath(ui.draggable.context, "tr");
										if (!dragdata.length)
											dragdata = "" + index + "%" + dragdata;
									} else
									if (ui.draggable.context.nodeName.toLowerCase() == "li") {
										if (node.parentNode) {
											index = Array.prototype.indexOf.call(node.parentNode.childNodes, node);
											if (!dragdata.length)
												dragdata = "" + index + "%" + dragdata;
										}
									}
								}
								if (element.ConceptDragDataReceived)
									self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_DRAGDATARECEIVED, dragdata, 0);
								if (element.ConceptDragDrop) {
									if (index != -1)
										position ="" + index;
									else
										position = "" + Math.floor(ui.position.left - element.offsetLeft) + "," + Math.floor(ui.position.top - element.offsetTop);
									self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_DRAGDROP, position, 0);
								}
								if (element.ConceptDragDataGet)
									self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_DRAGDATAGET, "", 0);
							}
						});
				}
				break;
			case P_TOOLBARSTYLE:
				if (cls_id == CLASS_TOOLBAR) {
					element.className = element.className.replace(" RToolbarIconsOnly").replace(" RToolbarLabelsOnly")
					switch (parseInt(Value)) {
						case TOOLBAR_ICONS:
							element.className += " RToolbarIconsOnly";
							break;
						case TOOLBAR_TEXT:
							element.className += " RToolbarLabelsOnly";
							break;
						case TOOLBAR_BOTH:
							break;
						case TOOLBAR_BOTH_HORIZ:
							break;
						default:
							break;
					}
				}
				break;
			case P_ISIMPORTANT:
				label = document.getElementById("l" + OwnerID);
				if (label) {
					if (parseInt(Value))
						label.style.display = "block";
					else
						label.style.display = "none";
				}
				break;
			case P_ACTIONWIDGETSTART:
				if (cls_id == CLASS_NOTEBOOK) {
					var primary = document.getElementById("pr" + OwnerID);
					if (primary) {
						var obj = this.Controls[parseInt(Value)];
						if (primary.firstChild)
							this.NoParentContainer.appendChild(primary.firstChild);
						if (obj)
							primary.appendChild(obj);
					}
				}
				break;
			case P_ACTIONWIDGETEND:
				if (cls_id == CLASS_NOTEBOOK) {
					var secondary = document.getElementById("se" + OwnerID);
					if (secondary) {
						var obj = this.Controls[parseInt(Value)];
						if (secondary.firstChild)
							this.NoParentContainer.appendChild(secondary.firstChild);
						if (obj)
							secondary.appendChild(obj);
					}
				}
				break;
			case P_EXPANDED:
				if (cls_id == CLASS_EXPANDER) {
					var content = document.getElementById("c" + OwnerID);
					var button = document.getElementById("b" + OwnerID);
					if (!content)
						content = element.ConceptContainer;

					if (content) {
						if (parseInt(Value)) {
							button.className = "RExpanderButton RExpanderButtonExpanded";
							if (content.offsetParent)
								$(content).show("fade");
							else
								content.style.display = "";
						} else {
							button.className = "RExpanderButton RExpanderButtonCollapsed";
							if (content.offsetParent)
								content.style.display = "none";
							else
								$(content).hide("fade");
						}
					}
				}
				break;
			case P_HEADERSVISIBLE:
				if (cls_id == CLASS_TREEVIEW) {
					var header = document.getElementById("h" + OwnerID);
					if (header) {
						if (parseInt(Value)) {
							header.style.display = "";
							//if (typeof element.ConceptWidth == "undefined")
							//	element.style.width = "0px";
						} else {
							header.style.display = "none";
							if (typeof element.ConceptWidth == "undefined")
								element.style.width = "100%";
						}
					}
				}
				break;
			case P_GRIDLINES:
				if (cls_id == CLASS_TREEVIEW) {
					switch (parseInt(Value)) {
						case 1:
							element.classList.remove("RTreeViewBorderHorizontal");
							element.classList.remove("RTreeViewBorder");
							element.classList.add("RTreeViewBorderVertical");
							break;
						case 2:
							element.classList.remove("RTreeViewBorder");
							element.classList.remove("RTreeViewBorderVertical");
							element.classList.add("RTreeViewBorderHorizontal");
							break;
						case 3:
							element.classList.remove("RTreeViewBorderHorizontal");
							element.classList.remove("RTreeViewBorderVertical");
							element.classList.add("RTreeViewBorder");
							break;
						default:
							element.classList.remove("RTreeViewBorderHorizontal");
							element.classList.remove("RTreeViewBorder");
							element.classList.remvoe("RTreeViewBorderVertical");
							break;
					}
				}
				break;
			case P_CURSOR:
				switch (cls_id) {
					case CLASS_TREEVIEW:
					case CLASS_ICONVIEW:
						element.setAttribute("ConceptRowIndex", "" + Value);
						break;
					case CLASS_COMBOBOX:
					case CLASS_COMBOBOXTEXT:
						this.SelectComboItem(OwnerID, this.GetComboElement(OwnerID, parseInt(Value)));
						break;
					case CLASS_SCROLLEDWINDOW:
						element.scrollTop = element.scrollHeight;
						break;
				}
				break;
			case P_SELECTEDCOLUMN:
				if (cls_id == CLASS_TREEVIEW)
					element.ConceptColumnIndex = parseInt(Value);
				break;
			case P_POSITION:
				if ((cls_id == CLASS_HPANED) || (cls_id == CLASS_VPANED)) {
					var val = parseInt(Value);
					var child;
					var child2;
					if (val > 0) {
						child = element.children[0];
						child2 = element.children[2];
					} else {
						child = element.children[2];
						val *= -1;
						child2 = element.children[0];
					}
					if (child) {
						if (cls_id == CLASS_HPANED) {
							child.style.width = "" + val + "px";
							child2.style.width = "calc(100% - " + val + "px - 8px)";
						} else {
							child.style.height = "" + val + "px";
							child2.style.maxHeight = "calc(100% - " + val + "px - 8px)";
						}
						child.style.webkitFlexShrink = 0;
						child.style.flexShrink = 0;
						child.style.webkitFlexGrow = 1;
						child.style.flexGrow = 1;
						child2.style.webkitFlexShrink = 0;
						child2.style.flexShrink = 0;
						child2.style.webkitFlexGrow = 1;
						child2.style.flexGrow = 1;
					}
				}
				break;
			case P_RECORD:
				if (cls_id == 1016) {
					this.LastAudioLevel = 0;
					this.LastAudioLevel_tail = 0;
					if (parseInt(Value) != -2)
						this.Record(element);
					else
					if ((element) && (element.ConceptProcessor)) {
						if (element.destination)
							element.ConceptProcessor.disconnect(element.destination);
						element.ConceptProcessor.onaudioprocess = null;
						if (element.ConceptMicrophone)
							element.ConceptMicrophone = null;
						element.ConceptProcessor = null;
						element.ConceptBuffers = [];
						if (element.ConceptWorker) {
							element.ConceptWorker.terminate();
							element.ConceptWorker = null;
						}
						this.InAudioContext = null;
						if (this.MediaStream) {
							this.MediaStream.stop();
							delete this.MediaStream;
						}
					}
				} else
				if (cls_id == 1017) {
					if (parseInt(Value) == -2) {
						if (this.MediaStream) {
							this.MediaStream.stop();
							delete this.MediaStream;
							if (element.ConceptEncoder) {
								element.ConceptEncoder.terminate();
								delete element.ConceptEncoder;
							}
						}
						if (element.ConceptFrameTimer) {
							clearTimeout(element.ConceptFrameTimer);
							delete element.ConceptFrameTimer;
							delete this.MediaTimer;
						}
					} else {
						this.MediaVideo = true;
						navigator.getUserMedia	= navigator.getUserMedia || navigator.webkitGetUserMedia || navigator.mozGetUserMedia || navigator.msGetUserMedia;

						if (navigator.getUserMedia) {
							this.MediaVideo = true;
							var control = element.ConceptPreviewControl;
							this.MediaListeners.push(function(stream) {
									if (window.URL)
										control.src = window.URL.createObjectURL(stream);
									else
									if (navigator.webkitGetUserMedia)
										control.src = window.webkitURL.createObjectURL(stream);
									else
									if (navigator.mozGetUserMedia)
										control.src = window.URL.createObjectURL(stream);
									else
										control.src = stream;
									control.play();

									element.ConceptEncoder = new Worker(res_prefix + "/js/openh264/openh264_encoder.js");
									self.MediaWorkers.push(element.ConceptEncoder);
									element.ConceptEncoder.Initialized = false;
									element.ConceptEncoder.onmessage = function(ev) {
										if (ev.data.length > 1)
											self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "350", ev.data, 1);
									}
									// ~ 7 fps
									var captureFrame = function() {
										var canvas = element.ConceptCanvasSmallFrame;
										if (canvas) {
											var ctx = canvas.getContext('2d');
											if ((ctx) && (canvas.width > 0) && (canvas.height > 0)) {
												if (element.ConceptEncoder.Initialized) {
													ctx.drawImage(control, 0, 0, canvas.width, canvas.height);
													var img = ctx.getImageData(0, 0, canvas.width, canvas.height);
													element.ConceptEncoder.postMessage(img.data, [img.data.buffer]); 
												} else {

													var width = 640;
													if (element.ConceptQuality <= 6)
														width = 320;
													else
													if (element.ConceptQuality <= 7)
														width = 320;
													var height = width * 3 / 4;

													canvas.width = width;
													canvas.height = height;

													console.log("Video initialized " + width + "x" + height);
													element.ConceptEncoder.Initialized = true;
													element.ConceptEncoder.postMessage({
														width: width,
														height: height,
														rgb: true,
														openh264: {"bitrate": element.ConceptBitRate}
													});
												}
											}
										}
										if ((element.ConceptFPS) && (element.ConceptFPS > 0))
											element.ConceptFrameTimer = setTimeout(captureFrame, 1000/element.ConceptFPS);
										else
											element.ConceptFrameTimer = setTimeout(captureFrame, 140);
										self.MediaTimer = element.ConceptFrameTimer;
									}
									element.ConceptFrameTimer = setTimeout(captureFrame, 1);
									self.MediaTimer = element.ConceptFrameTimer;
							});

							if (this.MediaTimer)
								clearTimeout(this.MediaTimer);
							this.MediaTimer = setTimeout(function () {
								self.getUserMediaDo();
							}, 1000);
						} else {
							console.error('Webcam API is not supported in this browser');
						}
					}
				}
				break;
			case P_BEGINPLAYBACK:
				if (cls_id == 1016) {
					if (parseInt(Value))
						this.Play(element);
				}
				break;
			case P_ADDBUFFER2:
			case P_ADDBUFFER:
				if (cls_id == 1016) {
					if ((element.ConceptMaxBuffers) && (element.ConceptMaxBuffers > 0)) {
						if (element.ConceptBuffers.length >= element.ConceptMaxBuffers) {
							/*var total = element.ConceptMaxBuffers / 2;

							while (element.ConceptBuffers.length >= total)
								element.ConceptBuffers.pop();*/
							console.warn("Jitter: " + element.ConceptBuffers.length + " (max buffers: " + element.ConceptMaxBuffers + ")");
							element.ConceptBuffers = [];
						}
					}
					if (!element.ConceptSampleRate)
						element.ConceptSampleRate = element.ref.sampleRate;
					if (!element.ConceptChannels)
						element.ConceptChannels = 1;

					var buf = ValueBuffer;
					if ((element.ConceptCompression == 2) && (Worker)) {
						if (!element.ConceptWorker) {
							element.ConceptWorker = new Worker(res_prefix + "/ConceptWorker.js");
							self.MediaWorkers.push(element.ConceptWorker);
							element.ConceptWorker.postMessage({"IsInit": 1, "SampleRate": element.ConceptSampleRate, "Channels": element.ConceptChannels});
							element.ConceptWorker.onmessage = function(e) {
								if (e.data) {
									if ((element.ConceptMaxBuffers) && (element.ConceptMaxBuffers > 0) && (element.ConceptMaxBuffers.length > element.ConceptMaxBuffers))
										element.ConceptBuffers = [];

									element.ConceptBuffers.push(self.AdjustSampleRate(e.data, element.ConceptSampleRate, element.ref.sampleRate, element, element.ConceptCompression, true));
								}
							}
						}
						element.ConceptWorker.postMessage({"IsInit": 0, "Data": ValueBuffer});
						break;
					}

					var buf2 = this.Decompress(element, ValueBuffer, PROP_ID == P_ADDBUFFER2);
					if (buf2)
						buf = buf2;
					else
						buf = this.ProcessBuffer(buf, element.ConceptChannels)

					if (buf) {
						if ((element.ConceptMaxBuffers) && (element.ConceptMaxBuffers > 0) && (element.ConceptMaxBuffers.length > element.ConceptMaxBuffers))
							element.ConceptBuffers = [];

						element.ConceptBuffers.push(this.AdjustSampleRate(buf, element.ConceptSampleRate, element.ref.sampleRate, element, element.ConceptCompression, true));
					}
				} else
				if (cls_id == 1017)
					element.decode(new Uint8Array(ValueBuffer));
				break;
			case P_SAMPLERATE:
				if (cls_id == 1016)
					element.ConceptSampleRate = parseInt(Value);
				else
				if (cls_id == 1017)
					element.ConceptFPS = parseInt(Value);
				else
				if (cls_id == 1015)
					element.ConceptLinkedImage = this.Controls[parseInt(Value)];
				else
				if (cls_id == 1018) {
					if (parseInt(Value))
						element.start();
					else
						element.stop();
				}
				break;
			case P_BITRATE:
				if (cls_id == 1016) {
					element.ConceptBitRate = parseInt(Value);
					if (element.OpusEncoder)
						element.OpusEncoder.ctl(4002, element.ConceptBitRate);
				} else
				if (cls_id == 1017)
					element.ConceptBitRate = parseInt(Value);
				break;
			case P_BANDWIDTH:
				if (cls_id == 1016) {
					element.ConceptBandWidth = parseInt(Value);
					if (element.OpusEncoder)
						this.SetBandWidth(element);
				}
				break;
			case P_CLEAR:
				if (cls_id == 1016) {
					if (parseInt(Value))
						element.ConceptBuffers = [];
				}
				break;
			case 1031:
				if (cls_id == 1015)
					element.ConceptCanvasFrame.width = parseInt(Value);
				break;
			case 1032:
				if (cls_id == 1015)
					element.ConceptCanvasFrame.height = parseInt(Value);
				break;
			case P_DIGITS:
			case P_NUMCHANNELS:
				if (cls_id == 1016)
					element.ConceptChannels = parseInt(Value);
				else
				if (cls_id == 1015) {
					if ((element.ConceptLinkedImage) && (parseInt(Value))) {
						var canvas = element.ConceptCanvas;
						if (canvas) {
							try {
								canvas.width = element.videoWidth;
								canvas.height = element.videoHeight;
								var ctx = canvas.getContext('2d');
								if (ctx) {
									ctx.drawImage(element, 0, 0, canvas.width, canvas.height);
									var data = canvas.toDataURL("image/bmp");
									if (data)
										element.ConceptLinkedImage.src = data;
								}
							} catch (e) {
								// nothing
							}
						}
					}
				} else
				if (cls_id == 1018)
					element.lang = Value;
				break;
			case P_DRAWVALUE:
				if (cls_id == 1018) {
					if (parseInt(Value))
						element.continous = true;
					else
						element.continous = false;
				}
				break;
			case P_VALUEPOS:
				if (cls_id == 1018) {
					if (parseInt(Value))
						element.interimResults = true;
					else
						element.interimResults = false;
				}
				break;
			case P_QUALITY2:
				if (cls_id == 1016) {
					element.ConceptQuality = parseInt(Value);
					if (element.Speex) {
						var i32ptr = libspeex.allocate(1, 'i32', libspeex.ALLOC_STACK);
						libspeex.setValue(i32ptr, element.ConceptQuality, 'i32');
						libspeex._speex_encoder_ctl(element.Speex.encoder.state, Speex.SPEEX_SET_QUALITY, i32ptr);
						element.Speex.encoder.set("quality", element.ConceptQuality);
					} else
					if (element.OpusEncoder)
						element.OpusEncoder.ctl(4010, element.ConceptQuality);
				} else
				if (cls_id == 1015) {
					element.ConceptFormat = mimeTypes[Value];
					if (!element.ConceptFormat)
						element.ConceptFormat = "image/" + Value;
				} else
				if (cls_id == 1017)
					element.ConceptQuality = parseInt(Value);
				break
			case P_USECOMPRESSION:
				if (cls_id == 1016)
					element.ConceptCompression = parseInt(Value);
				break;
			case P_MAXLEN:
				if (cls_id == 1016)
					element.ConceptMaxBuffers = parseInt(Value);
				break;
			case P_STEP:
				if (cls_id == 1016)
					element.ConceptFrameSize = parseInt(Value);
				break;
			case P_MASKED:
				if (cls_id == 1016)
					element.ConceptDRM = parseInt(Value);
				break;
			case P_VALUE:
				if (cls_id == 1016)
					element.ConceptMicLevel = parseInt(Value);
				else
				if (cls_id == CLASS_ADJUSTMENT) {
					element.ConceptValue = parseFloat(Value);
					var target = element.Target;
					if (target.ConceptClassID == CLASS_SPINBUTTON) {
						if (element.ConceptValue > element.ConceptUpper)
							element.ConceptValue = element.ConceptUpper;
						if (element.ConceptValue < element.ConceptLower)
							element.ConceptValue = element.ConceptLower;

						target.children[0].value = element.ConceptValue;
					} else
					if ((target.ConceptClassID == CLASS_SCROLLEDWINDOW) || (target.ConceptClassID == CLASS_TREEVIEW) || 
						(target.ConceptClassID == CLASS_TEXTVIEW) || (target.ConceptClassID == CLASS_HSCROLLBAR) ||
						(target.ConceptClassID == CLASS_ICONVIEW) || (target.ConceptClassID == CLASS_VSCROLLBAR)) {
						if ((target.ConceptClassID == CLASS_VSCROLLBAR) || (element.Vertical))
							target.scrollTop = element.ConceptValue;
						else
							target.scrollLeft = element.ConceptValue;
					} else
					if ((target.ConceptClassID == CLASS_VSCALE) || (target.ConceptClassID == CLASS_HSCALE)) {
						if (element.ConceptValue > element.ConceptUpper)
							element.ConceptValue = element.ConceptUpper;
						if (element.ConceptValue < element.ConceptLower)
							element.ConceptValue = element.ConceptLower;
						$(target.ConceptEdit).slider('setValue', element.ConceptValue);
					}
				}
				break;
			case P_LOWER:
				if (cls_id == CLASS_ADJUSTMENT) {
					element.ConceptLower = parseFloat(Value);
					var target = element.Target;
					if ((target) && ((target.ConceptClassID == CLASS_VSCALE) || (target.ConceptClassID == CLASS_HSCALE)))
						$(target.ConceptEdit).slider('setMin', element.ConceptLower);
				}
				break;
			case P_UPPER:
				if (cls_id == CLASS_ADJUSTMENT) {
					element.ConceptUpper = parseFloat(Value);
					var target = element.Target;
					if ((target) && ((target.ConceptClassID == CLASS_VSCALE) || (target.ConceptClassID == CLASS_HSCALE)))
						$(target.ConceptEdit).slider('setMax', element.ConceptUpper);
				}
				break;
			case P_INCREMENT:
				if (cls_id == CLASS_ADJUSTMENT) {
					element.ConceptIncrement = parseFloat(Value);
					var target = element.Target;
					if ((target) && ((target.ConceptClassID == CLASS_VSCALE) || (target.ConceptClassID == CLASS_HSCALE)))
						$(target.ConceptEdit).slider('setStep', element.ConceptIncrement);
				}
				break;
			case P_PAGEINCREMENT:
				if (cls_id == CLASS_ADJUSTMENT)
					element.ConceptPageIncrement = parseFloat(Value);
				break;
			case P_PAGESIZE:
				if (cls_id == CLASS_ADJUSTMENT)
					element.ConceptPageSize = parseFloat(Value);
				break;
			case 10011:
				if (cls_id == 1012) {
					if (element.ConceptCache) {
						for(var key in element.ConceptCache)
							Value = Value.replace(key, this.GetCookie(key, element.ConceptCache));
					}
					if ((element.contentWindow) && ((this.isIE) || (Value.indexOf("internal://") > 0))) {
						element.contentWindow.document.close();
						element.contentWindow.document.open();
						element.contentWindow.document.write(Value);
						self.UpdateLabelMarkup(OwnerID, element.contentWindow.document.documentElement, 360);
						element.contentWindow.document.close();
					} else {
						element.src = "data:text/html;charset=utf-8," + encodeURIComponent(Value);
					}
					element.onload = function() {							
						self.UpdateLabelMarkup(OwnerID, this.contentWindow.document.documentElement, 360);
						if ((element.contentEditable == "true") && (this.contentWindow.document.body))
							this.contentWindow.document.body.contentEditable = true;
					}
				}
				break;
			case 10007:
				if (cls_id == 1012) {
					if (Value.indexOf("document.execCommand('ForeColor', 1") == 0)
						Value = Value.replace("document.execCommand('ForeColor', 1", "document.execCommand('foreColor', 0");
					if (Value.indexOf("document.execCommand('BackColor'") == 0)
						Value = Value.replace("document.execCommand('BackColor'", "document.execCommand('backColor'");
					if (Value.indexOf("document.execCommand('CreateLink'") == 0)
						Value = Value.replace("document.execCommand('CreateLink', 1", "document.execCommand('createLink', 0");
					element.contentWindow.ConceptFoo = undefined;
					element.contentWindow.eval("function ConceptFoo(){" + Value + "}");
					element.contentWindow.ConceptFoo();
				}
				break;
			case P_LAYOUT:
				if ((cls_id == CLASS_VBUTTONBOX) || (cls_id == CLASS_HBUTTONBOX)) {
					switch (parseInt(Value)) {
						case BUTTONBOX_DEFAULT_STYLE:
						case BUTTONBOX_END:
							element.style.webkitJustifyContent = "flex-end";
							element.style.justifyContent = "flex-end";
							break;
						case BUTTONBOX_SPREAD:
							element.style.webkitJustifyContent = "flex-center";
							element.style.justifyContent = "flex-center";
							break;
						case BUTTONBOX_EDGE:
							element.style.webkitJustifyContent = "space-around";
							element.style.justifyContent = "space-around";
							break;
						case BUTTONBOX_START:
							element.style.webkitJustifyContent = "flex-start";
							element.style.justifyContent = "flex-start";
							break;
					}
				}
				break;
			case P_IMAGECOLUMN:
				if (cls_id == CLASS_ICONVIEW)
					element.setAttribute("ConceptImageColumn", parseInt(Value));
				break;
			case P_MARKUPCOLUMN:
				if (cls_id == CLASS_ICONVIEW)
					element.setAttribute("ConceptMarkupColumn", parseInt(Value));
				break;
			case P_TEXTCOLUMN:
				if ((cls_id == CLASS_ICONVIEW) || (cls_id == CLASS_COMBOBOXTEXT))
					element.setAttribute("ConceptTextColumn", parseInt(Value));
				break;
			case P_TOOLTIPCOLUMN:
				if ((cls_id == CLASS_ICONVIEW) || (cls_id == CLASS_TREEVIEW))
					element.setAttribute("ConceptTooltipColumn", parseInt(Value));
				break;
			case P_ITEMWIDTH:
				if (cls_id == CLASS_ICONVIEW)
					element.setAttribute("ConceptItemWidth", parseInt(Value));
				break;
			case P_TOOLTIP:
				if ((cls_id == CLASS_EDIT) || (cls_id == CLASS_COMBOBOXTEXT)) {
					var label = element.firstChild.children[1];
					if (label)
						label.setAttribute("placeholder", Value);
					label.setAttribute("title", Value);
				} else {
					if ((cls_id == CLASS_TEXTVIEW) || (cls_id == CLASS_COMBOBOXTEXT))
						element.setAttribute("placeholder", Value);
					element.setAttribute("title", Value);
				}
				break;
			case P_TOOLTIPMARKUP:
				if ((cls_id == CLASS_EDIT) || (cls_id == CLASS_COMBOBOXTEXT)) {
					var label = element.firstChild.children[1];
					if (label)
						label.setAttribute("placeholder", Value);
					label.setAttribute("title", strip(Value));
				} else {
					if ((cls_id == CLASS_TEXTVIEW) || (cls_id == CLASS_COMBOBOXTEXT))
						element.setAttribute("placeholder", Value);
					element.setAttribute("title", strip(Value));
				}
				break;
			case P_XALIGN:
				switch (cls_id) {
					case CLASS_EDIT:
					case CLASS_COMBOBOXTEXT:
						element = element.firstChild.children[1];
					case CLASS_LABEL:
						var f = parseFloat(Value);
						if (f < 0.1)
							element.style.textAlign = "left";
						else
						if (f > 0.9)
							element.style.textAlign = "right";
						else
						if (f < 0.0)
							element.style.textAlign = "justify";
						else
							element.style.textAlign = "center";
						break;
				}
				break;
			case P_YALIGN:
				switch (cls_id) {
					case CLASS_LABEL:
						element.ConceptYAlign = parseFloat(Value);
						this.UpdateYAlign(element);
						var parent = element.getAttribute("ConceptParentID");
						if (parent) {
							var obj_p = this.Controls[parseInt(parent)];
							if ((obj_p) && ((obj_p.ConceptClassID == CLASS_HBOX) || (obj_p.ConceptClassID == CLASS_HBUTTONBOX))) {
								element.style.alignSelf = "center";
								if (f < 0.1)
									element.style.alignSelf = "flex-start";
								else
								if (f > 0.9)
									element.style.alignSelf = "flex-end";
								else
								if (f < 0.0)
									element.style.alignSelf = "stretch";
								else
									element.style.alignSelf = "center";
								element.style.webkitAlignSelf = element.style.alignSelf;
							}
						}
						break;
				}
				break;
			case P_POPUPMENU:
				if (element.ConceptClassID == CLASS_TREEVIEW) {
					var parent = element.getAttribute("ConceptParentID");
					if (parent) {
						var obj_p = this.Controls[parseInt(parent)];
						if ((obj_p) && (obj_p.ConceptClassID == CLASS_SCROLLEDWINDOW))
							element = obj_p;
					}
				}
				var obj = this.Controls[parseInt(Value)];
				if (obj) {
					element.setAttribute("data-toggle", "context");
					element.setAttribute("data-target", "#context" + Value);
					var container = document.createElement("div");
					container.id = "context" + Value;
					if (obj.parentNode) {
						var obj_cloned = this.CloneMenu(obj);
						obj.parentNode.replaceChild(obj_cloned, obj);
					}
					container.appendChild(obj);
					this.Container.appendChild(container);
				} else {
					element.setAttribute("data-toggle", "");
					element.setAttribute("data-target", "");
				}
				break;
			case P_MENU:
				if (cls_id == CLASS_MENUTOOLBUTTON) {
					var obj = this.Controls[parseInt(Value)];
					if (obj) {
						if (element.parentNode) {
							var obj_cloned = this.CloneMenu(obj);
							if (obj.parentNode) {
								var obj_cloned = this.CloneMenu(obj);
								obj.parentNode.replaceChild(obj_cloned, obj);
							}
							element.parentNode.insertBefore(obj, element.nextSibling);
						}
					}
				}
				break;
			case P_ACCELKEY:
				switch (cls_id) {
					case CLASS_IMAGEMENUITEM:
					case CLASS_RADIOMENUITEM:
					case CLASS_CHECKMENUITEM:
					case CLASS_MENUITEM:
					case CLASS_TEAROFFMENUITEM:
					case CLASS_STOCKMENUITEM:
						var first_child = element.children[0].firstChild;
						if ((!first_child) || (first_child.className != "RMenuItemAccelKey")) {
							element.children[0].children[1].style.paddingRight = "80px";
							var new_child = document.createElement("div");
							new_child.className = "RMenuItemAccelKey";
							element.children[0].insertBefore(new_child, first_child);
							first_child = new_child;
						}
						new_child.innerHTML = "";
						Value = Value.replace("control", "ctrl").replace("<", "").replace(">", "+");
						var val = Value.charAt(0).toUpperCase() + Value.slice(1);
						val = val.replace("+", " + ");
						new_child.appendChild(document.createTextNode(val));
						shortcut.add(Value, function() {
							self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_ACTIVATE, "", 0);
						}, {'active_element': element, 'ignore_visibility': true});
						break;
					case CLASS_BUTTON:
					case CLASS_TOOLBUTTON:
					case CLASS_RADIOTOOLBUTTON:
					case CLASS_TOGGLETOOLBUTTON:
					case CLASS_MENUTOOLBUTTON:
						shortcut.add(Value.replace("control", "ctrl").replace("<", "").replace(">", "+"), function() {
							self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_CLICKED, "", 0);
						}, {'active_element': element});
						break;
				}
				break;
			case P_FULLSCREEN:
				if ((parseInt(Value)) && (!document.fullscreenElement && !document.mozFullScreenElement && !document.webkitFullscreenElement && !document.msFullscreenElement)) {
					if (element.requestFullscreen) {
						element.requestFullscreen();
					} else
					if (element.msRequestFullscreen) {
						element.msRequestFullscreen();
					} else
					if (element.mozRequestFullScreen) {
						element.mozRequestFullScreen();
					} else
					if (element.webkitRequestFullscreen) {
						element.webkitRequestFullscreen(Element.ALLOW_KEYBOARD_INPUT);
					}
					if (element.getAttribute("ConceptParentID")) {
						element.firstChild.style.width = "100%";
						element.firstChild.style.height = "100%";
					}
				} else {
					if (document.exitFullscreen) {
						document.exitFullscreen();
					} else
					if (document.msExitFullscreen) {
						document.msExitFullscreen();
					} else
					if (document.mozCancelFullScreen) {
						document.mozCancelFullScreen();
					} else
					if (document.webkitExitFullscreen) {
						document.webkitExitFullscreen();
					}
				}
				break;
			case P_STYLE:
				Value = Value.replace("#rid", "#r");
				if (Value.indexOf("{") != -1) {
					var head = document.head || document.getElementsByTagName("head")[0];
					if (head) {
						var style = element.ConceptStyle;
						if (!style) {
							style = document.createElement("style");
							style.type = "text/css";
							element.ConceptStyle = style;
						}
						if (style.styleSheet)
							style.styleSheet.cssText = Value;
						else {
							style.innerHTML = "";
							style.appendChild(document.createTextNode(Value));
						}
						head.appendChild(style);
					}
				} else
					element.style.cssText += Value;
				break;
			case P_PRIMARYICONNAME:
			case P_SECONDARYICONNAME:
				if ((cls_id == CLASS_EDIT) || (cls_id == CLASS_COMBOBOXTEXT)) {
					var icon_container;
					if (PROP_ID == P_PRIMARYICONNAME)
						icon_container = element.firstChild.children[0];
					else
						icon_container = element.firstChild.children[2];
					if (icon_container) {
						icon_container.firstChild.innerHTML = "";
						if (Value.length) {
							icon_container.style.display = "";
							var href = document.createElement("a");
							
							var img = document.createElement("img");
							img.src = this.ResourceRoot + "/" + this.AdjustStock(Value) + ".png";
							img.style.width = "24px";
							img.style.height = "24px"
							href.appendChild(img);
							href.href = "#";
							href.onclick = function() {
								return false;
							}
							var icon_id;
							if (PROP_ID == P_PRIMARYICONNAME)
								icon_id = "0";
							else
								icon_id = "1";
							href.onmouseup = function() {
								if (element.ConceptIconPress)
									self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_PRESSED, icon_id, 0);
							}
							href.onmousedown = function() {
								if (element.ConceptIconRelease)
									self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_RELEASED, icon_id, 0);
							}
							icon_container.firstChild.appendChild(href);
						} else
							icon_container.style.display ="none";
					}
				}
				break;
			case P_PRIMARYICON:
			case P_SECONDARYICON:
				if ((cls_id == CLASS_EDIT) || (cls_id == CLASS_COMBOBOXTEXT)) {
					var icon_container;
					if (PROP_ID == P_PRIMARYICON)
						icon_container = element.firstChild.children[0];
					else
						icon_container = element.firstChild.children[2];
					if (icon_container) {
						icon_container.firstChild.innerHTML = "";
						var source = this.Controls[Value];
						if ((source) && (source.ConceptClassID == CLASS_IMAGE)) {
							icon_container.style.display = "";
							var href = document.createElement("a");
							
							var img = document.createElement("img");
							img.src = source.src;
							img.style.width = "24px";
							img.style.height = "24px"
							href.appendChild(img);
							href.href = "#";
							href.onclick = function() {
								return false;
							}
							var icon_id;
							if (PROP_ID == P_PRIMARYICONNAME)
								icon_id = "0";
							else
								icon_id = "1";
							href.onmouseup = function() {
								if (element.ConceptIconPress)
									self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_PRESSED, icon_id, 0);
							}
							href.onmousedown = function() {
								if (element.ConceptIconRelease)
									self.SendMessage("" + OwnerID, MSG_EVENT_FIRED, "" + EVENT_ON_RELEASED, icon_id, 0);
							}
							icon_container.firstChild.appendChild(href);
						} else
							icon_container.style.display ="none";
					}
				}
				break;
			case P_PRIMARYTOOLTIP:
			case P_SECONDARYTOOLTIP:
				if ((cls_id == CLASS_EDIT) || (cls_id == CLASS_COMBOBOXTEXT)) {
					var icon_container;
					if (PROP_ID == P_PRIMARYTOOLTIP)
						icon_container = element.firstChild.children[0];
					else
						icon_container = element.firstChild.children[2];
					if (icon_container) {
						icon_container.setAttribute("title", Value);
						$(icon_container).tooltip({ html : true });
					}
				}
				break;
			case P_PRIMARYACTIVABLE:
			case P_SECONDARYACTIVABLE:
				if ((cls_id == CLASS_EDIT) || (cls_id == CLASS_COMBOBOXTEXT)) {
					var icon_container;
					if (PROP_ID == P_PRIMARYACTIVABLE)
						icon_container = element.firstChild.children[0];
					else
						icon_container = element.firstChild.children[2];
					if (icon_container) {
						if (parseInt(Value))
							icon_container.disabled = false;
						else
							icon_container.disabled = true;
					}
				}
				break;
			case P_HADJUSTMENT:
			case P_VADJUSTMENT:
				switch (cls_id) {
					case CLASS_SCROLLEDWINDOW:
					case CLASS_TREEVIEW:
					case CLASS_HSCROLLBAR:
					case CLASS_VSCROLLBAR:
						var adjustment = this.Controls[parseInt(Value)];
						if ((adjustment) && (adjustment.ConceptClassID == CLASS_ADJUSTMENT)) {
							adjustment.Target = element;
							if (PROP_ID == P_VADJUSTMENT) {
								adjustment.Vertical = true;
								element.ConceptVAdjustment = adjustment;
							} else {
								adjustment.Vertical = false;
								element.ConceptHAdjustment = adjustment;
							}
							if (!element.onscroll) {
								element.onscroll = function(event) {
									var max;
									if (this.ConceptVAdjustment) {
										// vertical scroll
										max = this.scrollTop + this.clientHeight;
										if (max < this.scrollHeight)
											max = this.scrollHeight;
										this.ConceptVAdjustment.ConceptUpper = max;
										this.ConceptVAdjustment.ConceptLower = 0;
										this.ConceptVAdjustment.ConceptValue = this.scrollTop;
										this.ConceptVAdjustment.ConceptPageSize = this.clientHeight;
										if (this.ConceptVAdjustment.ConceptValueChanged)
											self.SendMessage(this.ConceptVAdjustment.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_VALUECHANGED, "" + this.scrollTop + ":" + this.clientHeight + ":" +  max, 0);
									}
									if (this.ConceptHAdjustment) {
										// horizontal scroll
										max = this.scrollLeft + this.clientWidth;
										if (max < this.scrollWidth)
											max = this.scrollWidth;
										this.ConceptHAdjustment.ConceptUpper = max;
										this.ConceptHAdjustment.ConceptLower = 0;
										this.ConceptHAdjustment.ConceptValue = this.scrollLeft;
										this.ConceptHAdjustment.ConceptPageSize = this.clientWidth;
										if (this.ConceptHAdjustment.ConceptValueChanged) 
											self.SendMessage(this.ConceptVAdjustment.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_VALUECHANGED, "" + this.scrollLeft + ":" + this.clientWidth + ":" + max, 0);
									}
									if (element.firstChild.ConceptClassID == CLASS_TREEVIEW) {
										if (self.isIE) {
											element.firstChild.ConceptHeader.style.position = "";
											element.firstChild.ConceptHeader.style.top = "";
											if (element.PendingTimer)
												clearTimeout(element.PendingTimer);
											element.PendingTimer = setTimeout(function () {
												self.UpdateTableHeader(element);
											}, 100);
										} else
											self.UpdateTableHeader(element);
									}
								}
							}
						} else {
							if (PROP_ID == P_VADJUSTMENT)
								element.ConceptVAdjustment = null;
							else
								element.ConceptHAdjustment = null;
						}
						break;
				}
				break;
			case P_HSCROLLBARPOLICY:
				switch (parseInt(Value)) {
					case 0:
						element.style.overflowY = "scroll";
						break;
					case 2:
						element.style.overflowY = "hidden";
						break;
					default:
						element.style.overflowY = "auto";
						break;
				}
				break;
			case P_VSCROLLBARPOLICY:
				switch (parseInt(Value)) {
					case 0:
						element.style.overflowX = "scroll";
						break;
					case 2:
						element.style.overflowX = "hidden";
						break;
					default:
						element.style.overflowX = "auto";
						break;
				}
				break;
			case P_SEARCHCOLUMN:
				element.ConceptSearchColumn = parseInt(Value);
				break;
			case P_SEARCHENTRY:
				if (cls_id == CLASS_TREEVIEW) {
					var entry = this.Controls[Value];
					if ((entry) && ((entry.ConceptClassID == CLASS_EDIT) || (entry.ConceptClassID == CLASS_COMBOBOXTEXT))) {
						var label = entry.firstChild.children[1];
						if (label) {
							label.addEventListener('keypress', function(e) {
								if (label.PendingTimer)
									clearTimeout(label.PendingTimer);
								label.PendingTimer = setTimeout(function () {
									self.TreeSearch(element, label.value);
								}, 100);
							}, false);
						}
					}
				} else
				if (cls_id == CLASS_TEXTVIEW) {
					var arr = Value.split(";");
					if (arr.length) {
						var str = arr[arr.length - 1];
						if (str.length) {
							var tag = null;
							if (arr.length > 1) {
								var rtag = this.Controls[arr[0]];
								if ((rtag) && (rtag.ConceptClassID == CLASS_TEXTTAG))
									tag = rtag;
							}
							var arr2 = str.split(/\W+/);
							this.UpdateTextView(element, arr2, tag);
						}
					}
				}
				break;
			case P_SUGESTION:
			case P_SUGESTIONMODEL:
				if ((cls_id == CLASS_EDIT) || (cls_id == CLASS_COMBOBOXTEXT)) {
					var label = element.firstChild.children[1];
					if (label) {
						if (PROP_ID == P_SUGESTIONMODEL) {
							var obj = this.Controls[parseInt(Value)];
							if ((obj) && ((obj.ConceptClassID == CLASS_COMBOBOX) || (obj.ConceptClassID == CLASS_COMBOBOXTEXT)))
								element.ConceptSuggestModel = obj;
							else
								element.ConceptSuggestModel = null;
							element.ConceptSuggest = [ ];
						} else
							element.ConceptSuggest = Value.split(";");

						if (!element.ConceptSList) {
							var container = document.createElement("div");
							container.className = "dropdown";
							container.style.display = "none";

							var list = document.createElement("ul");
							list.className = "dropdown-menu REditSuggest";
							list.setAttribute("role", "menu");
							container.appendChild(list);
							element.appendChild(container);
							element.ConceptSList = list;

							list.addEventListener("blur", function(e) {
								container.style.display = "none";
							});

							label.addEventListener("blur", function(e) {
								if (!container.PreventFocusOut)
									container.style.display = "none";
							});

							var keylistener = function(e) {
								if (!e)
									e = window.event;
								var code = (e.keyCode || e.which);
								if ((container.offsetParent) && (list.children.length)) {
									switch (code) {
										case 40:
											container.PreventFocusOut = true;
											if (list.selChild)
												list.selChild = list.selChild.nextSibling;
											if (!list.selChild)
												list.selChild = list.firstChild;
											list.selChild.firstChild.focus();
											label.value = list.selChild.firstChild.innerHTML;
											break;
										case 38:
											container.PreventFocusOut = true;
											if (list.selChild)
												list.selChild = list.selChild.previousSibling;
											if (!list.selChild)
												list.selChild = list.lastChild;
											list.selChild.firstChild.focus();
											label.value = list.selChild.firstChild.innerHTML;
											break;
									}
								
								}
							};

							label.addEventListener("keydown", keylistener);

							label.addEventListener("keyup", function(e) {
								var code = (e.keyCode || e.which);
								var text = label.value;
								if ((!text.length) || ((code < 0x20) && (code != 0x08))) {
									container.style.display = "none";
									return;
								}
								container.PreventFocusOut = false;
								list.innerHTML = "";
								var r = new RegExp(text.replace(/[\-\[\]\/\{\}\(\)\*\+\?\.\\\^\$\|]/g, "\\$&"), 'i');
								var count = 0;
								var suggest;
								if (element.ConceptSuggestModel)
									suggest = self.GetSuggestionModel(element.ConceptSuggestModel);
								else
									suggest = element.ConceptSuggest;

								for (var i = 0; i < suggest.length; i++) {
									var e = suggest[i];
									if (r.test(e)) {
										var li = document.createElement("li");
										var a = document.createElement("a");
										a.setAttribute("role", "menuitem");
										li.addEventListener("keydown", keylistener);
										a.tabIndex = -1;
										a.href = "#";
										a.onclick = function() {
											label.value = this.innerHTML;
											container.style.display = "none";
											label.focus();
											return false;
										};
										a.appendChild(document.createTextNode(e));
										li.appendChild(a);
										list.appendChild(li);
										count++;
									}
								}
								container.style.display = count ? "block" : "none";
							});
						}
					}
				}
				break;
			case P_DAY:
				if (cls_id == CLASS_CALENDAR) {
					var date = $(element).datepicker("getDate");
					date.setDate(parseInt(Value));
					$(element).datepicker("setDate", date);
				}
				break;
			case P_MONTH:
				if (cls_id == CLASS_CALENDAR) {
					var date = $(element).datepicker("getDate");
					date.setMonth(parseInt(Value) - 1);
					$(element).datepicker("setDate", date);
				}
				break;
			case P_YEAR:
				if (cls_id == CLASS_CALENDAR) {
					var date = $(element).datepicker("getDate");
					date.setFullYear(parseInt(Value));
					$(element).datepicker("setDate", date);
				}
				break;
			case P_HOMOGENEOUS:
				if (cls_id == CLASS_TABLE) {
					if (parseInt(Value))
						element.style.tableLayout = "fixed";
					else
						element.style.tableLayout = "auto";
				}
				break;
			case P_TOPPADDING:
				element.style.paddingTop = "" + parseInt(Value) + "px";
				break;
			case P_LEFTPADDING:
				element.style.paddingLeft = "" + parseInt(Value) + "px";
				break;
			case P_BOTTOMPADDING:
				element.style.paddingBottom = "" + parseInt(Value) + "px";
				break;
			case P_RIGHTPADDING:
				element.style.paddingRight = "" + parseInt(Value) + "px";
				break;
			case P_XPADDING:
				var val = parseInt(Value);
				switch (cls_id) {
					case CLASS_LABEL:
					case CLASS_IMAGE:
					default:
						element.style.paddingLeft = "" + val + "px";
						element.style.paddingRight = "" + val + "px";
						break;
				}
				break;
			case P_YPADDING:
				var val = parseInt(Value);
				switch (cls_id) {
					case CLASS_LABEL:
					case CLASS_IMAGE:
					default:
						element.style.paddingTop = "" + val + "px";
						element.style.paddingBottom = "" + val + "px";
						break;
				}
				break;
			case P_ROWSPACING:
			case P_COLUMNSPACING:
				if (cls_id == CLASS_TABLE) {
					var table = element.children[0];
					if (table) {
						table.style.borderCollapse = "separate";
						table.style.borderSpacing = "" + parseInt(Value) + "px";
					}
				}
				break;
			case P_BORDERWIDTH:
				element.style.margin = "" + parseInt(Value) + "px";
				break;
			case P_FORMAT:
				if ((cls_id == CLASS_EDIT) || (cls_id == CLASS_COMBOBOXTEXT)) {
					var label = element.firstChild.children[1];
					var arr = Value.split(";");
					var str = arr[0];
					str =  str.replace(/0/g,"9");
					if (arr.length > 1)
						$(label).mask(str, {placeholder: arr[1]});
					else
						$(label).mask(str);
				} else
				if (cls_id == CLASS_CLIENTCHART)
					element.ConceptOptions = JSON.parse(Value);
				break;
			case P_READONLY:
				switch (cls_id) {
					case CLASS_EDIT:
					case CLASS_COMBOBOXTEXT:
						var label = element.firstChild.children[1];
						if (label) {
							if (parseInt(Value)) {
								label.setAttribute("readonly", "true");
								element.firstChild.children[0].setAttribute("disabled", "disabled");
								element.firstChild.children[2].setAttribute("disabled", "disabled");
							} else {
								label.removeAttribute("readonly");
								element.firstChild.children[0].removeAttribute("disabled");
								element.firstChild.children[2].removeAttribute("disabled");
							}
						}
						break;
					case CLASS_TEXTVIEW:
						if (parseInt(Value))
							element.contentEditable = false;
						else
							element.contentEditable = true;
						break;
				}
				break;
			case P_BORDER:
				element.style.padding = "" + parseInt(Value) + "px";
				break;
			case P_SELECTABLE:
				if (parseInt(Value)) {
					element.style.userSelect = "text";
					element.style.webkitUserSelect = "text";
					element.style.MozUserSelect = "text";
					element.style.msUserSelect = "text";
					element.style.cursor = "";
				} else {
					element.style.userSelect = "none";
					element.style.webkitUserSelect = "none";
					element.style.MozUserSelect = "none";
					element.style.msUserSelect = "none";
					element.style.cursor = "default";
				}
				break;
			case P_WRAP:
				if (cls_id == CLASS_TEXTTAG) {
					switch (parseInt(Value)) {
						case 1:
						case 2:
						case 3:
							element.cssText += "word-wrap: normal; white-space: initial;";
							break;
						default:
							element.cssText += "word-wrap: break-word; white-space: nowrap;";
					}
				} else {
					switch (parseInt(Value)) {
						case 1:
						case 2:
						case 3:
							element.style.wordWrap = "normal";
							element.style.whiteSpace = "";
							break;
						default:
							element.style.wordWrap = "break-word";
							element.style.whiteSpace = "nowrap";
					}
				}
				break;
			case P_JUSTIFY:
				switch (cls_id) {
					case CLASS_EDIT:
					case CLASS_COMBOBOXTEXT:
					case CLASS_TEXTVIEW:
						var label = element.firstChild.children[1];
						if (label) {
							switch (parseInt(Value)) {
								case 1:
									element.style.textAlign = "right";
									break;
								case 2:
									element.style.textAlign = "center";
									break;
								case 3:
									element.style.textAlign = "justify";
									break;
								default:
									element.style.textAlign = "left";
							}
						}				
						break;
					case CLASS_TEXTTAG:
						switch (parseInt(Value)) {
							case 1:
								element.cssText += "text-align: right;";
								break;
							case 2:
								element.cssText += "text-align: center;";
								break;
							case 3:
								element.cssText += "text-align: justify;";
								break;
							default:
								element.cssText += "text-align: left;";
						}
						break;
					default:
						switch (parseInt(Value)) {
							case 1:
								element.style.textAlign = "right";
								break;
							case 2:
								element.style.textAlign = "center";
								break;
							case 3:
								element.style.textAlign = "justify";
								break;
							default:
								element.style.textAlign = "left";
						}
						break;
				}
				break;
			case P_OPACITY:
				element.style.opacity = parseFloat(Value);
				break;
			case P_ACCEPTTAB:
				if (cls_id == CLASS_TEXTVIEW) {
					if (parseInt(Value)) {
						element.onkeydown = function(e) {
							e = e || window.event;
							if (e.keyCode == 9)
								return false;
						}
					} else {
						element.onkeydown = function(e) {
							// nothing
						}
					}
				}
				break;
			case P_SELECTIONSTART:
				if (cls_id == CLASS_TEXTVIEW)
					element.selectionStart = parseInt(Value);
				break;
			case P_SELECTIONLENGTH:
				if (cls_id == CLASS_TEXTVIEW)
					element.selectionEnd = element.selectionStart + parseInt(Value);
				break;
			case P_INDENT:
				if (cls_id == CLASS_TEXTTAG)
					element.cssText = "text-indent: " + parseInt(Value) + "px;";
				else
					element.style.textIndent = "" + parseInt(Value) + "px";
				break;
			case P_LEFTMARGIN:
				if (cls_id == CLASS_TEXTTAG)
					element.cssText = "padding-left: " + parseInt(Value) + "px;";
				else
					element.style.paddingLeft = "" + parseInt(Value) + "px";
				break;
			case P_RIGHTMARGIN:
				if (cls_id == CLASS_TEXTTAG)
					element.cssText = "padding-right: " + parseInt(Value) + "px;";
				else
					element.style.paddingRight = "" + parseInt(Value) + "px";
				break;
			case P_PIXELSABOVE:
				if (cls_id == CLASS_TEXTTAG)
					element.cssText = "padding-top: " + parseInt(Value) + "px;";
				else
					element.style.paddingTop = "" + parseInt(Value) + "px";
				break;
			case P_PIXELSBELOW:
				if (cls_id == CLASS_TEXTTAG)
					element.cssText = "padding-bottom: " + parseInt(Value) + "px;";
				else
					element.style.paddingBottom = "" + parseInt(Value) + "px";
				break;
			case P_PIXELSINSIDE:
				if (cls_id == CLASS_TEXTTAG)
					element.cssText = "padding: " + parseInt(Value) + "px;";
				else
					element.style.padding = "" + parseInt(Value) + "px";
				break;
			case P_FORECOLOR:
            			if (cls_id == CLASS_TEXTTAG)
					element.cssText += "color: " + this.doColorString(parseInt(Value)) + ";";
				break;
			case P_BACKCOLOR:
            			if (cls_id == CLASS_TEXTTAG)
					element.cssText += "background-color: " + this.doColorString(parseInt(Value)) + ";";
				break;
			case P_DIRECTION:
            			if (cls_id == CLASS_TEXTTAG) {
					switch (parseInt(Value)) {	
						case 1:
							element.cssText += "direction: ltr;";
							break;
						case 2:
							element.cssText += "direction: rtl;";
							break;
						case 3:
						default:
							element.cssText += "direction: initial;";
							break;
					}
				}
				break;
			case P_FONTWEIGHT:
            			if (cls_id == CLASS_TEXTTAG)
					element.cssText += "font-weight: " + Value + ";";
				else
					element.style.fontWeight = Value;
				break;
			case P_UNDERLINE:
            			if (cls_id == CLASS_TEXTTAG) {
					var val = parseInt(Value);
					if (val) {
						if (val == 4)
							element.cssText += "border-bottom: 1px dashed red;";
						else
							element.cssText += "text-decoration: underline;";
					} else
						element.cssText += "text-decoration: none;";
				} else {
					if (parseInt(Value))
						element.style.textDecoration = "underline";
					else
						element.style.textDecoration = "";
				}
				break;
			case P_STRIKEOUT:
            			if (cls_id == CLASS_TEXTTAG) {
					if (parseInt(Value))
						element.cssText += "text-decoration: line-through;";
					else
						element.cssText += "text-decoration: none;";
				} else {
					if (parseInt(Value))
						element.style.textDecoration = "line-through";
					else
						element.style.textDecoration = "";
				}
				break;
			case P_FONTSIZE:
            			if (cls_id == CLASS_TEXTTAG)
					element.cssText += "font-size: " + parseInt(Value) + "px;";
				else
					element.style.fontSize = "" + parseInt(Value) + "px";
				break;
			case P_SCALE:
            			if (cls_id == CLASS_TEXTTAG)
					element.cssText += "font-size: " + (parseFloat(Value) * 100) + "%;";
				else
					element.style.fontSize = "" + (parseFloat(Value) * 100) + "%";
				break;
			case P_FONTFAMILY:
				if (cls_id == CLASS_TEXTTAG)
					element.cssText += "font-family: " + Value + ";";
				else
					element.style.fontFamily = Value;
				break;
			case P_FONTSTYLE:
				if (cls_id == CLASS_TEXTTAG)
					element.cssText += "font-style: " + Value + ";";
				else
					element.style.fontStyle = Value;
				break;
			case P_FONTNAME:
				if (cls_id == CLASS_TEXTTAG)
					element.cssText += "font: " + Value + ";";
				else
					this.ParseFont(element, Value);
				break;
			case P_STRETCH:
				if (cls_id == CLASS_TEXTTAG)
					element.cssText += "font-stretch: " + Value + ";";
				else
					element.style.fontStretch = Value;
				break;
			case P_PARENT:
				var new_parent = this.Controls[Value];
				if (new_parent)
					new_parent.appendChild(element);
				else
					this.NoParentContainer.appendChild(element);
				break;
			case P_DISPOSE:
				if (parseInt(Value)) {
					element.parentNode.removeChild(element);
					this.Controls[OwnerID] = null;
				}
				break;
			case P_MODEL:
				if (cls_id == CLASS_PROPERTIESBOX)
					element.ConceptReturnPropertyIndex = parseInt(Value);
				break;
			case 10001:
				switch (cls_id) {
					case 1001:
					case 1012:
						if (element.ConceptCache) {
							for(var key in element.ConceptCache)
								Value = Value.replace(key, this.GetCookie(key, element.ConceptCache));
						}
						if ((element.contentWindow) && (element.contentWindow.document.documentElement))
							element.contentWindow.document.removeChild(element.contentWindow.document.documentElement);
						element.src = Value;
						element.onload = function(e) {
							if (element.contentEditable == "true")
								this.contentWindow.document.body.contentEditable = true;

							self.SendMessage(element.id.substring(1), MSG_EVENT_FIRED, "360", this.contentWindow.location.href, 0);
						}
						break;
				}
			case P_PROPERTIES:
				if (cls_id == CLASS_PROPERTIESBOX) {
					element.style.visibility = "gone";
					element.innerHTML = "";
					var xmlDoc = null;
					if (window.DOMParser) {
						parser = new DOMParser();
						xmlDoc = parser.parseFromString(Value, "text/xml");
					} else {
						xmlDoc = new ActiveXObject("Microsoft.XMLDOM");
						xmlDoc.async = false;
						xmlDoc.loadXML(Value);
					} 
					if (xmlDoc) {
						var root = xmlDoc.documentElement;
						if (root.nodeName.toLowerCase() == "pr") {
							var children = root.childNodes;
							var len = children.length;
							var prec_cat = "";
							for (var i = 0; i < len; i++) {
								var child = children[i];
								if (child.nodeName.toLowerCase() == "pp") {
									var name = child.getAttribute("n");
									var type = parseInt(child.getAttribute("t"));
									var val = child.getAttribute("v");
									var cat = child.getAttribute("c");
									if (name) {
										if ((cat) && (prec_cat != cat)) {
											var c = document.createElement("p");
											c.className = "RPropertiesBoxCategory";
											c.appendChild(document.createTextNode(cat));
											element.appendChild(c);
											prec_cat = cat;
										}

										var e = document.createElement("label");
										if (type < 0) {
											e.className = "RPropertiesBoxImportant";
											type *= -1;
										}

										e.appendChild(document.createTextNode(name));
										element.appendChild(e);

										var e2;
										switch (type) {
											case PROP_BOOLEAN:
												e2 = document.createElement("input");
												e2.setAttribute("type", "checkbox");
												if (parseInt(val))
													e2.checked = true;
												else
													e2.checked = false;
												e2.onchange = function() {
													if (element.ConceptChanged === true) {
														if (element.ConceptReturnPropertyIndex)
															self.SendMessage(element.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_CHANGED, "" + this.ConceptIndex + "=" + (this.checked ? "1" : "0"), 0);
														else
															self.SendMessage(element.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_CHANGED, "" + this.ConceptName + "=" + (this.checked ? "1" : "0"), 0);
													}
												}
												break;
											case PROP_STATIC:
												e2 = document.createElement("input");
												e2.setAttribute("type", "text");
												e2.disabled = true;
												if (val)
													e2.value = val;
												break;
											case PROP_LONGTEXT:
												e2 = document.createElement("textarea");
												if (val)
													e2.value = val;
												e2.oninput = function() {
													if (element.ConceptChanged === true) {
														if (element.ConceptReturnPropertyIndex)
															self.SendMessage(element.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_CHANGED, "" + this.ConceptIndex + "=" + this.value, 0);
														else
															self.SendMessage(element.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_CHANGED, "" + this.ConceptName + "=" + this.value, 0);
													}
												}
												break;
											case PROP_OPTION:
											case PROP_OPTIONEDIT:
												e2 = document.createElement("select");
												var options = child.childNodes;
												var len2 = options.length;
												for (var j = 0; j < len2; j++) {
													var option = options[j];
													var o = document.createElement("option");
													o.appendChild(document.createTextNode(option.innerHTML));
													if (option.innerHTML == val)
														o.selected = true;
													e2.appendChild(o);
												}
												e2.onchange = function() {
													if (element.ConceptChanged === true) {
														if (element.ConceptReturnPropertyIndex)
															self.SendMessage(element.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_CHANGED, "" + this.ConceptIndex + "=" + this.value, 0);
														else
															self.SendMessage(element.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_CHANGED, "" + this.ConceptName + "=" + this.value, 0);
													}
												}
												break;
											case PROP_INSTANT:
											case PROP_COLOR:
											default:
												e2 = document.createElement("input");
												e2.setAttribute("type", "text");
												if (val) {
													if (type == PROP_COLOR)
														val = "#" + parseInt(val).toString(16);
													e2.value = val;
												}
												e2.ConceptName = name;
												e2.ConceptIndex = i;
												if (type == PROP_COLOR) {
													e2.oninput = function() {
														if (element.ConceptChanged === true) {
															var color_val = this.value;
															if ((color_val) && (color_val[0] == "#"))
																color_val = color_val.substring(1);
															var n_val = parseInt(color_val, 16);
															if (element.ConceptReturnPropertyIndex)
																self.SendMessage(element.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_CHANGED, "" + this.ConceptIndex + "=" + n_val, 0);
															else
																self.SendMessage(element.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_CHANGED, "" + this.ConceptName + "=" + n_val, 0);
														}
													}
												} else {
													e2.oninput = function() {
														if (element.ConceptChanged === true) {
															if (element.ConceptReturnPropertyIndex)
																self.SendMessage(element.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_CHANGED, "" + this.ConceptIndex + "=" + this.value, 0);
															else
																self.SendMessage(element.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_CHANGED, "" + this.ConceptName + "=" + this.value, 0);
														}
													}
												}
												break;
										}
										e2.appendChild(document.createTextNode(val));
										e2.id = "r" + OwnerID + "prop_" + i;
										e2.ConceptPropertyName = name;
										e2.ConceptPropertyIndex = i;
										e2.ConceptPropertyType = type;
										if (cat)
											e2.ConceptCategory = cat;
										element.appendChild(e2);

										e.setAttribute("for", e2.id);
									}
								}
							}
						}
					}
					element.style.visibility = "visible";
				}
				break;
			case P_SIZING:
				if (cls_id == 1017) {
					var arr = Value.split(",");
					element.ConceptPreviewControl.style.position = "absolute";
					element.ConceptPreviewControl.style.left = "" + arr[0] + "pt";
					element.ConceptPreviewControl.style.top = "" + arr[1] + "pt";
					element.ConceptPreviewControl.style.width = "" + arr[2] + "pt";
					/*if (element.ConceptQuality <= 6)
						element.ConceptPreviewControl.style.width = "192px";
					else
					if (element.ConceptQuality <= 7)
						element.ConceptPreviewControl.style.width = "320px";
					else
						element.ConceptPreviewControl.style.width = "640px";*/
				}
				break;
			case P_WIDGET:
				if (cls_id == 1017) {
					element.ConceptWidget = this.Controls[Value];
					if (element.ConceptWidget) {
						element.ConceptWidget.appendChild(element.canvas);
						element.ConceptWidget.appendChild(element.ConceptPreviewControl);
						element.ConceptPreviewControl.style.zIndex = 10000;
					}
				}
				break;
		}
	}

	this.doColorString = function(color) {
		if (color < 0)
			return "";
		var result = "";
		var alpha = -1;
		if (color >= 0x1000000) {
			alpha = Math.floor(color / 0x1000000) / 0x100;
			color %= 0x1000000;
		}
		var red   = Math.floor(color / 0x10000);
		color %= 0x10000;
		var green = Math.floor(color / 0x100);
		color %= 0x100;
		var blue = color;

		if (alpha>=0)
			result = "rgba(";
		else
			result = "rgb(";
		result += red + ", " + green + ", " + blue;
		if (alpha >= 0)
			result += ", " + alpha;
		result += ")";
		return result;
	}

	this.StockName = function(str) {
		switch (str) {
			case "gtk-goto-first":
			case "gtk-go-back":
			case "gtk-go-forward":
			case "gtk-goto-last":
			case "gtk-media-play":
			case "gtk-media-stop":
			case "gtk-media-pause":
			case "gtk-media-record":
				return "";
		}
		str = str.replace("gtk-", "");
		str = str.replace("-", " ");
		str = str.charAt(0).toUpperCase() + str.slice(1);
		return str;
	}

	this.UpdateButton = function(element, OwnerID, text) {
		var label = document.getElementById("l" + OwnerID);
		var image = document.getElementById("i" + OwnerID);
		if ((label) && (image)) {
			image.src = this.ResourceRoot + "/" + this.AdjustStock(text) + ".png";
			image.style.width = "16px";
			image.style.height = "16px";
			image.style.display = "";

			text = this.StockName(text);
			label.innerHTML = text;
			switch (text.toLowerCase()) {
				case "ok":
				case "yes":
				case "save":
					element.className = "RButton btn btn-primary";
					break;
				case "edit":
				case "call start":
					element.className = "RButton btn btn-success";
					break;
				case "no":
				case "discard":
				case "call stop":
					element.className = "RButton btn btn-danger";
					break;
				case "cancel":
					element.className = "RButton btn btn-default";
					break;
			}
		}
	}

	this.AdjustStock = function(Value) {
		var res = this.GetStock(parseInt(Value));
		if (!res)
			res = Value;
		return res;
	}

	this.Packing = function(element, pack) {
		var cls_id = element.ConceptClassID;
		if (!cls_id)
			cls_id = parseInt(element.getAttribute("ConceptClassID"));

		switch (pack) {
			case PACK_SHRINK:
				element.style.webkitFlexGrow = 0;
				element.style.flexGrow = 0;
				if ((cls_id == CLASS_EDIT) || (cls_id == CLASS_COMBOBOXTEXT) || (cls_id == CLASS_COMBOBOX) || (cls_id == CLASS_BUTTON) || (cls_id == CLASS_VPANED)) {
					var parent = element.getAttribute("ConceptParentID");
					var p_control = null;
					if (parent)
						p_control = this.Controls[parseInt(parent)];

					if ((p_control) && ((p_control.ConceptClassID == CLASS_VBOX) || (p_control.ConceptClassID == CLASS_VBUTTONBOX) || (p_control.ConceptClassID == CLASS_HPANED)) && ((cls_id == CLASS_EDIT) || (cls_id == CLASS_COMBOBOXTEXT) || (cls_id == CLASS_COMBOBOX) || (cls_id == CLASS_BUTTON) || (cls_id == CLASS_VPANED))) {
						element.style.webkitFlexShrink = 0;
						element.style.flexShrink = 0;
					} else {
						element.style.webkitFlexShrink = 1;
						element.style.flexShrink = 1;
					}
				} else {
					element.style.webkitFlexShrink = 0;
					element.style.flexShrink = 0;
				}
				break;
			case PACK_EXPAND_PADDING:
			case PACK_EXPAND_WIDGET:
				element.style.webkitFlexShrink = 1;
				element.style.webkitFlexGrow = 1;
				element.style.flexShrink = 1;
				element.style.flexGrow = 1;
				if (cls_id == CLASS_VBOX) {
					var parent = element.getAttribute("ConceptParentID");
					var p_control = null;
					if (parent)
						p_control = this.Controls[parseInt(parent)];
					// firefox & ie
					//if ((p_control) && (p_control.ConceptClassID == CLASS_VBOX))
					//	element.style.height = "0px";
				}
				break;
		}
	}

	this.PageChanged = function(control, tabs, tab) {
		if (tab.className.lastIndexOf('active') == -1) {
			var index = this.GetIndex(tabs, tab);
			if (index > 0)
				index -= 2;
			this.PageChangedNotify(control, index);
		}
	}

	this.PageChangedNotify = function(element, index) {
		self.UpdateScrolledWindowSize(1);
		if (element.ConceptSwitchPage)
			self.SendMessage(element.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_SWITCHPAGE, "" + index, 0);
	}

	this.CreateControl = function(Parent, CLASS_ID, RID) {
		var parent = this.Controls[Parent];
		var packing = PACK_SHRINK;
		var control;
		var label;
		var ptype = -1;
		if (parent) {
			ptype = parent.ConceptClassID;
			if (!ptype)
				ptype = parseInt(parent.getAttribute("ConceptClassID"));
			if ((ptype == CLASS_HTMLAPP) && (this.UICreate))
				parent = this.UICreate();
		}

		switch (CLASS_ID) {
			case CLASS_FORM:
				if (parent) {
					control = document.createElement("div");
					control.className = "modal RForm";
					control.innerHTML = '<div class="modal-dialog RChildForm" id="d' + RID + '"><div id="f' + RID + '" class="modal-content"><div id="h' + RID + '" class="RFormHeader modal-header btn-primary"><button id="close' + RID + '" type="button" class="close" data-dismiss="modal" onclick="if (document.getElementById(\'r' + RID + '\').ConceptDelete) { document.getElementById(\'r' + RID + '\').ConceptDelete.SendMessage(\'\' + ' + RID + ', MSG_EVENT_FIRED, \'\' + EVENT_ON_DELETEEVENT, \'\', 0); } else { document.getElementById(\'r' + RID + '\').style.display=\'none\'; } if (document.getElementById(\'r' + RID + '\').ConceptHide) { document.getElementById(\'r' + RID + '\').ConceptHide.SendMessage(\'\' + ' + RID + ', MSG_EVENT_FIRED, \'\' + EVENT_ON_HIDE, \'\', 0); } if (document.getElementById(\'r' + RID + '\').ConceptVisibility) { document.getElementById(\'r' + RID + '\').ConceptVisibility.SendMessage(\'\' + ' + RID + ', MSG_EVENT_FIRED, \'\' + EVENT_ON_VISIBILITY, \'0\', 0); } if (document.getElementById(\'r' + RID + '\').ConceptUnrealize) { document.getElementById(\'r' + RID + '\').ConceptUnrealize.SendMessage(\'\' + ' + RID + ', MSG_EVENT_FIRED, \'\' + EVENT_ON_UNREALIZE, \'\', 0); }"><span aria-hidden="true">&times;</span><span class="sr-only">Close</span></button><h4 class="modal-title" id="t' + RID + '">&nbsp;</h4></div><div class="RFormContent modal-body navbar-form" id="c' + RID + '"></div></div></div>';
					$(control).draggable({ handle: "#h" + RID });
					$(control).resizable();
					control.style.position = "absolute";
				} else {
					control = document.createElement("div");
					control.className = "RForm";
				}
				break;
			case CLASS_BUTTON:
				control = document.createElement("button");
				control.className = "RButton btn btn-default";

				var image = document.createElement("img");
				image.className = "RButtonImage";
				image.id = "i" + RID;
				image.style.display = "none";

				var blabel = document.createElement("span");
				blabel.className = "RButtonLabel";
				blabel.id = "l" + RID;

				control.appendChild(image);
				control.appendChild(blabel);
				packing = PACK_SHRINK;
				break;
			case CLASS_TOOLBUTTON:
			case CLASS_RADIOTOOLBUTTON:
			case CLASS_TOGGLETOOLBUTTON:
			case CLASS_MENUTOOLBUTTON:
				control = document.createElement("button");
				control.className = "RToolButton btn btn-sm btn-default";

				var image = document.createElement("img");
				image.className = "RToolButtonImage";
				image.id = "i" + RID;
				image.style.display = "none";

				var blabel = document.createElement("div");
				blabel.className = "RToolButtonLabel";
				blabel.id = "l" + RID;

				control.appendChild(image);
				control.appendChild(blabel);
				if (CLASS_ID == CLASS_MENUTOOLBUTTON) {
					blabel.className = "RToolButtonLabel RToolMenuButtonLabel";
					var href = document.createElement("a");
					href.id = "d" + RID;
					href.className = "RMenuToolButtonCaret";
					href.innerHTML = '<span class="caret RCaret"></span>';
					href.href = "#";
					href.onmousedown = function() {
						return false;
					};
					control.appendChild(href);
					control.setAttribute("data-toggle", "dropdown");
					control.setAttribute("aria-expanded", "false");
					control.className += " dropdown-toggle";
				} else
				if ((CLASS_ID == CLASS_TOGGLETOOLBUTTON) || (CLASS_ID == CLASS_RADIOTOOLBUTTON)) {
					control.ConceptChecked = 0;
					control.addEventListener("click", function(e) {
						if (this.ConceptChecked) {
							if (CLASS_ID == CLASS_RADIOTOOLBUTTON) 
								return;
							this.classList.remove("btn-primary");
							this.ConceptChecked = 0;
						} else {
							this.classList.add("btn-primary");
							this.ConceptChecked = 1;
						}
						if ((this.ConceptChecked) && (this.ConceptGroupWith)) {
							for (var i = 0; i < this.ConceptGroupWith.length; i++) {	
								var w = this.ConceptGroupWith[i];
								if ((w) && (w != this)) {
									w.ConceptChecked = 0;
									w.classList.remove("btn-primary");
								}
							}
						}
						if (this.ConceptToggled)
							self.SendMessage(this.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_TOGGLED, "" + this.ConceptChecked, 0);
					});
				}
				break;
			case CLASS_TOOLSEPARATOR:
				if (ptype == CLASS_TOOLBAR) {
					var group1 = document.getElementById("g" + Parent);
					if (group1) {
						group1.id = "r" + RID;
						group1.setAttribute("ConceptClassID", CLASS_TOOLSEPARATOR);
					}

					var group2 = document.createElement("div");
					group2.className = "btn-group";

					var vert = parent.getAttribute("ConceptVertical");
					if ((vert) && (vert == "1"))
						group2.className = "btn-group-vertical";

					group2.id = "g" + Parent;

					parent.appendChild(group2);
				}
				// dummy control
				control = document.createElement("div");
				control.className = "RToolSeparator";
				control.style.display = "hidden";
				control.ConceptClassID = CLASS_ID;
				this.NoParentContainer.appendChild(control);
				this.Controls[RID] = control;
				return;
				break;
			case CLASS_LABEL:
				control = document.createElement("p");
				control.className = "RLabel";
				control.ConceptYAlign = 0.5;
				packing = PACK_SHRINK;
				break;
			case CLASS_IMAGE:
				control = document.createElement("img");
				control.className = "RImage";
				break;
			case CLASS_EDIT:
				control = document.createElement("div");
				control.className = "REditPrimaryContainer";

				var inner = document.createElement("div");
				inner.className = "input-group REditContainer";

				var primary_icon_span = document.createElement("span");
				primary_icon_span.className = "input-group-addon REditIcon";
				primary_icon_span.style.display = "none";

				var primary_i = document.createElement("i")
				primary_i.className = "fa";
				primary_i.id = "p" + RID;
				primary_icon_span.appendChild(primary_i);
				
				var edit = document.createElement("input");
				edit.setAttribute("type", "text");
				edit.id = "c" + RID;
				edit.className = "form-control REdit";
				var secondary_icon_span = document.createElement("span");
				secondary_icon_span.className = "input-group-addon REditIcon";

				var secondary_i = document.createElement("i")
				secondary_i.className = "fa";
				secondary_i.id = "s" + RID;
				secondary_icon_span.appendChild(secondary_i);
				secondary_icon_span.style.display = "none";

				inner.appendChild(primary_icon_span);
				inner.appendChild(edit);
				inner.appendChild(secondary_icon_span);
				control.appendChild(inner);
				packing = PACK_EXPAND_WIDGET;
				break;
			case CLASS_RADIOBUTTON:
				control = document.createElement("div");
				control.className = "RRadioButton";

				var input = document.createElement("input");
				input.setAttribute("type", "radio");
				input.checked = false;
				input.id = "i" + RID;
				input.name = "r" + RID;

				label = document.createElement("label");
				label.className = control.className;
				label.setAttribute("for", input.id);
				label.id = "l" + RID;
				label.style.display = "none";

				control.appendChild(input);
				control.appendChild(label);
				break;
			case CLASS_CHECKBUTTON:
				control = document.createElement("div");
				control.className = "RCheckButton";

				input = document.createElement("input");
				input.setAttribute("type", "checkbox");
				input.checked = false;
				input.id = "i" + RID;

				label = document.createElement("label");
				label.className = control.className;
				label.setAttribute("for", input.id);
				label.id = "l" + RID;
				label.style.display = "none";

				control.appendChild(input);
				control.appendChild(label);
				break;
			case CLASS_TEXTVIEW:
				control = document.createElement("div");
				control.contentEditable = true;
				control.className = "form-control RTextView";
				packing = PACK_EXPAND_WIDGET;
				break;
			case CLASS_TEXTTAG:
				control = { "id": "r" + RID, "ConceptClassID": CLASS_TEXTTAG, "cssText": ""};
				this.Controls[RID] = control;
				return;
				break;
			case CLASS_VBOX:
				control = document.createElement("div");
				control.className = "RVBox";
				packing = PACK_EXPAND_WIDGET;
				break;
			case CLASS_HBOX:
				control = document.createElement("div");
				control.className = "RHBox";
				packing = PACK_EXPAND_WIDGET;
				break;
			case CLASS_VBUTTONBOX:
				control = document.createElement("div");
				control.className = "RVButtonBox";
				break;
			case CLASS_HBUTTONBOX:
				control = document.createElement("div");
				control.className = "RHButtonBox";
				if (parent)
					control.classNaem += "RCentered";
				break;
			case CLASS_TOOLBAR:
				control = document.createElement("div");
				control.className = "btn-toolbar RToolbar";
				control.setAttribute("role", "toolbar");

				var group = document.createElement("div");
				group.className = "btn-group";
				group.id = "g"+RID;

				control.appendChild(group);
				packing = PACK_SHRINK;
				break;
			case CLASS_MENUBAR:
				control = document.createElement("div");
				control.className = "navbar navbar-default RMenubar";
				control.setAttribute("role", "navigation");

				var fluid = document.createElement("div");
				fluid.className = "container-fluid";
				fluid.id = "f" + RID;
				fluid.innerHTML = '<div class="navbar-header"><button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target=".navbar-collapse"><span class="sr-only">Toggle navigation</span><span class="icon-bar"></span><span class="icon-bar"></span><span class="icon-bar"></span></button></div>';

				var container = document.createElement("div");
				container.className = "navbar-collapse collapse";
				container.id = "c" + RID;
				var items = document.createElement("ul");
				items.className = "nav navbar-nav";
				items.id = "i"+RID;

				container.appendChild(items);
				fluid.appendChild(container);

				control.appendChild(fluid);
				packing = PACK_SHRINK;
				break;
			case CLASS_MENU:
				control = document.createElement("ul");
				control.className = "dropdown-menu RMenu";
				control.setAttribute("role", "menu");
				switch (ptype) {
					case CLASS_IMAGEMENUITEM:
					case CLASS_RADIOMENUITEM:
					case CLASS_CHECKMENUITEM:
					case CLASS_MENUITEM:
					case CLASS_TEAROFFMENUITEM:
					case CLASS_STOCKMENUITEM:
						if (!parent.getAttribute("ConceptTopLevel"))
							parent.className += " dropdown-submenu";
						break;
				}
				break;
			case CLASS_SEPARATORMENUITEM:
				control = document.createElement("li");
				control.className = "divider RSeparatorMenuItem";
				break;
			case CLASS_IMAGEMENUITEM:
			case CLASS_RADIOMENUITEM:
			case CLASS_CHECKMENUITEM:
			case CLASS_MENUITEM:
			case CLASS_TEAROFFMENUITEM:
			case CLASS_STOCKMENUITEM:
				control = document.createElement("li");
				control.className = "RMenuItem";
				if (ptype == CLASS_MENUBAR)
					control.setAttribute("ConceptTopLevel", 1);

				label = document.createElement("a");
				label.href = "#";
				label.onclick = function() {
					return false;
				}
				label.className = "dropdown-toggle";
				label.setAttribute("data-toggle", "dropdown");
				label.id = "l" + RID;

				var image = document.createElement("img");
				image.className = "RMenuImage";
				image.id = "i" + RID;
				image.style.display = "none";
				label.appendChild(image);

				var l2 = document.createElement("div");
				l2.className = "dropdown-toggle RMenuLabel";
				l2.id = "t" + RID;
				label.appendChild(l2);

				if (CLASS_ID == CLASS_STOCKMENUITEM) {
					image.src = this.ResourceRoot + "/" + this.AdjustStock(this.POST_STRING) + ".png";
					image.style.width = "16px";
					image.style.height = "16px";
					image.style.display = "";
					l2.appendChild(document.createTextNode(this.StockName(this.POST_STRING)));
				} else
					l2.appendChild(document.createTextNode(this.ParseUnderline(this.POST_STRING)));

				control.appendChild(label);

				if ((CLASS_ID == CLASS_CHECKMENUITEM) || (CLASS_ID == CLASS_RADIOMENUITEM)) {
					control.ConceptChecked = 0;
					control.addEventListener("click", function(e) {
						if (this.ConceptChecked) {
							if (CLASS_ID == CLASS_RADIOTOOLBUTTON) 
								return;
							this.classList.remove("RMenuItemChecked");
							this.ConceptChecked = 0;
						} else {
							this.classList.add("RMenuItemChecked");
							this.ConceptChecked = 1;
						}
						if ((this.ConceptChecked) && (this.ConceptGroupWith)) {
							for (var i = 0; i < this.ConceptGroupWith.length; i++) {	
								var w = this.ConceptGroupWith[i];
								if ((w) && (w != this)) {
									w.ConceptChecked = 0;
									w.classList.remove("RMenuItemChecked");
								}
							}
						}
						if (this.ConceptToggled)
							self.SendMessage(this.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_TOGGLED, "" + this.ConceptChecked, 0);
					});
				}
				break;
			case CLASS_NOTEBOOK:
				control = document.createElement("div");
				control.className = "RNotebook tabbable";

				var tabs = document.createElement("ul");
				tabs.className = "nav nav-tabs small RNotebookTabs";
				tabs.setAttribute("role", "tablist");
				tabs.id = "t" + RID;

				var primary = document.createElement("li");
				primary.id = "pr" + RID;

				var secondary = document.createElement("li");
				secondary.className = "pull-right";
				secondary.id = "se" + RID;

				tabs.appendChild(primary);
				tabs.appendChild(secondary);

				var pages = document.createElement("div");
				pages.className = "tab-content RNotebookPages";
				pages.id = "c" + RID;

				control.appendChild(tabs);
				control.appendChild(pages);
				control.ConceptTabs = tabs;
				control.ConceptPages = pages;

				packing = PACK_EXPAND_WIDGET;
				break;
			case CLASS_EXPANDER:
				control = document.createElement("div");
				control.className = "RExpander";

				var button = document.createElement("button");
				button.className = "RExpanderButton RExpanderButtonCollapsed";
				button.innerHTML = "Expander";
				button.id = "b" + RID;

				var content = document.createElement("div");
				content.className = "RExpanderContent";
				content.style.display = "none";
				content.id = "c" + RID;

				control.appendChild(button);
				control.appendChild(content);

				control.ConceptContainer = content;
				control.ConceptButton = button;

				button.onclick = function() {
					if (content.style.display === "none") {
						$(content).show("fade");
						button.className = "RExpanderButton RExpanderButtonExpanded";
					} else {
						$(content).hide("fade");
						button.className = "RExpanderButton RExpanderButtonCollapsed";
					}
				}
				packing = PACK_EXPAND_WIDGET;
				break;
			case CLASS_PROGRESSBAR:
				control = document.createElement("div");
				control.className = "RProgressBar progress progress-striped active";

				var bar = document.createElement("div");
				bar.className = "RProgressBarValue progress-bar";
				bar.setAttribute("role", "progressbar");
				bar.setAttribute("aria-valuenow", "0");
				bar.style.width = "auto";
				bar.setAttribute("aria-valuemin", "0");
				bar.setAttribute("aria-valuemax", "100");
				bar.id = "l" + RID;

				control.appendChild(bar);
				packing = PACK_SHRINK;
				break;
			case CLASS_FRAME:
				control = document.createElement("div");
				control.className = "panel panel-primary RFrame";

				var heading = document.createElement("div");
				heading.className = "panel-heading RFrameHeader";
				heading.id = "h" + RID;

				label = document.createElement("h3");
				label.className = "panel-title";
				label.id = "l" + RID;

				heading.appendChild(label);

				var container = document.createElement("div");
				container.className = "panel-body RFrameContainer";
				container.id = "c" + RID;

				control.appendChild(heading);
				control.appendChild(container);

				control.ConceptContainer = content;
				control.ConceptLabel = button;
				break;
			case CLASS_TREEVIEW:
				control = document.createElement("div");
				control.className = "RTreeViewContainer";
				if (!this.isIE)
					control.style.height = "100%";
				var control_child = document.createElement("table");
				control_child.className = "table table-condensed table-striped table-hover RTreeView";
				control.appendChild(control_child);

				var header = document.createElement("thead");
				header.className = "RTreeViewHeader";
				header.id = "h" + RID;
				control.ConceptHeader = header;

				var headerRow = document.createElement("tr");
				headerRow.className = "RTreeViewHeaderRow";
				headerRow.id = "l" + RID;

				header.appendChild(headerRow);
				control.ConceptHeaderRow = headerRow;

				var body = document.createElement("tbody");
				body.id = "c" + RID;
				body.className = "RTreeviewBody";

				control_child.appendChild(header);
				control_child.appendChild(body);
				control.ConceptTableRows = body;

				this.TreeData[RID] = { "Columns": [], "Post": [], "ColProperties": [] };
				
				control_child.setAttribute("ConceptRowIndex", "-1");
				control_child.addEventListener("click", function(e) {
					if (e.target) {
						var ename = e.target.nodeName.toLowerCase();
						if ((ename == "tr") || (ename == "td"))
							control.setAttribute("ConceptRowIndex", self.GetPath(e.target, ename));
					}
				});
				break;
			case CLASS_TABLE:
				// using table until grid will be supported in all major browsers
				control = document.createElement("table");
				control.className = "RTable";
				packing = PACK_SHRINK;
				break;
			case CLASS_ICONVIEW:
				control = document.createElement("div");
				control.className = "RIconView";

				var container = document.createElement("ul");
				container.className = "RIconViewContainer";
				container.id = "c" + RID;

				control.appendChild(container);

				this.TreeData[RID] = { "Columns": [], "Post": [] };
				packing = PACK_SHRINK;
				control.setAttribute("ConceptRowIndex", "-1");
				break;
			case CLASS_SCROLLEDWINDOW:
				control = document.createElement("div");
				control.className = "RScrolledWindow";
				control.scrollTop = 0;
				control.scrollLeft = 0;
				control.addEventListener("OverflowEvent" in window ? "overflowchanged" : "overflow", function(e) {
					var child = control.firstChild;
					if (e.type == ("overflow") || ((e.orient == 0 && e.horizontalOverflow) /*|| (e.orient == 1 && e.verticalOverflow)*/ || (e.orient == 2 && e.horizontalOverflow && e.verticalOverflow))) {
						if (child) {
							if (typeof child.ConceptWidth == "undefined")	{
								var hasScroll = false;

								if (control.scrollWidth > control.offsetWidth)
									hasScroll = true;
								else
								if ((control.scrollWidth == control.offsetWidth) && (control.clientWidth) && (control.clientWidth < control.offsetWidth))
									hasScroll = true;

								if ((control.offsetWidth) && (hasScroll)) {
									if (child.ConceptClassID != CLASS_TREEVIEW)
										child.style.width = "" + control.offsetWidth + "px";
									else
										child.style.width = "0px";
								} else
									child.style.width = "100%";
							}
							/*if (typeof child.ConceptHeight == "undefined")	{
								if ((control.offsetHeight) && (control.scrollHeight > control.offsetHeight))
									child.style.height = "" + control.offsetHeight + "px";
								else {
									if (child.ConceptClassID != CLASS_TREEVIEW)
										child.style.height = "" + control.offsetHeight + "px";
									else
										child.style.height = "0px";
								}
							}*/
						}
					} else {
						child.style.width = "100%";
						//child.style.height = "100%";//"" + (control.offsetHeight - 50) + "px";
					}
				});
				packing = PACK_EXPAND_WIDGET;
				break;
			case CLASS_VIEWPORT:
				control = document.createElement("div");
				control.className = "RViewPort";
				CLASS_ID = CLASS_SCROLLEDWINDOW;
				packing = PACK_EXPAND_WIDGET;
				break;
			case CLASS_STATUSBAR:
				control = document.createElement("div");
				control.className = "RStatusBar";
				if (ptype == -1)
					control.className = "RStatusBarNoParent";
				packing = PACK_SHRINK;
				break;
			case CLASS_EVENTBOX:
				control = document.createElement("div");
				control.className = "REventBox";
				break;
			case CLASS_ALIGNMENT:
				control = document.createElement("div");
				control.className = "RAlignment";
				packing = PACK_EXPAND_WIDGET;
				break;
			case CLASS_COMBOBOX:
				control = document.createElement("div");
				control.className = "btn-group RComboBox";

				var items = document.createElement("ul");
				items.className = "dropdown-menu RComboBoxItems";
				items.id = "c" + RID;

				var label = document.createElement("a");
				label.className = "btn btn-default dropdown-toggle btn-select RComboBoxButton";
				label.setAttribute("data-toggle", "dropdown");
				label.href = "#";
				label.id = "l" + RID;
				label.innerHTML = '&nbsp;<span class="caret RCaret"></span>';
				label.onclick = function() {
					return false;
				}

				this.TreeData[RID] = { "Columns": [], "Post": [] };
				control.setAttribute("ConceptRowIndex", "-1");

				control.appendChild(label);
				control.appendChild(items);
				packing = PACK_SHRINK;
				break;
			case CLASS_COMBOBOXTEXT:
				control = document.createElement("div");
				control.className = "REditPrimaryContainer";

				var inner = document.createElement("div");
				inner.className = "input-group RComboBox RComboBoxEditContainer";

				var primary_icon_span = document.createElement("span");
				primary_icon_span.className = "input-group-addon REditIcon";
				primary_icon_span.style.display = "none";

				var primary_i = document.createElement("i")
				primary_i.className = "fa";
				primary_i.id = "p" + RID;
				primary_icon_span.appendChild(primary_i);

				var edit = document.createElement("input");
				edit.setAttribute("type", "text");
				edit.id = "l" + RID;
				edit.className = "form-control REdit RComboEdit";

				var secondary_icon_span = document.createElement("span");
				secondary_icon_span.className = "input-group-addon REditIcon";

				var secondary_i = document.createElement("i")
				secondary_i.className = "fa";
				secondary_i.id = "s" + RID;
				secondary_icon_span.appendChild(secondary_i);
				secondary_icon_span.style.display = "none";

				inner.appendChild(primary_icon_span);
				inner.appendChild(edit);
				inner.appendChild(secondary_icon_span);

				var group = document.createElement("div");
				group.className = "input-group-btn REditComboBoxButtonContainer";
				group.style.height = "100%"

				var button = document.createElement("button");
				button.className = "btn btn-default dropdown-toggle REditComboBoxButton";
				button.setAttribute("data-toggle", "dropdown");
				button.innerHTML = '<span class="caret"></span>';
				group.appendChild(button);

				var items = document.createElement("ul");
				items.className = "dropdown-menu dropdown-menu-right RComboBoxItems";
				items.setAttribute("role", "menu");
				items.id = "c" + RID;

				this.TreeData[RID] = { "Columns": [], "Post": [] };
				control.setAttribute("ConceptRowIndex", "-1");

				group.appendChild(items);
				inner.appendChild(group);
				control.appendChild(inner);
				break;
			case CLASS_FIXED:
				control = document.createElement("div");
				control.className = "RFixed";
				break;
			case CLASS_VPANED:
				control = document.createElement("div");
				control.className = "RVBox";
				packing = PACK_EXPAND_WIDGET;
				break;
			case CLASS_HPANED:
				control = document.createElement("div");
				control.className = "RHBox";
				packing = PACK_EXPAND_WIDGET;
				break;
			case CLASS_ADJUSTMENT:
				// virtual control
				control = { "id": "r" + RID, "ConceptClassID": CLASS_ADJUSTMENT, "ConceptValue": 0, "ConceptLower": 0, "ConceptUpper": 100, "ConceptIncrement": 1, "ConceptPageIncrement": 10, "ConceptPageSize": 10};
				this.Controls[RID] = control;
				return;
				break;
			case CLASS_CALENDAR:
				control = document.createElement("div");
				$(control).datepicker({todayHighlight: true,  calendarWeeks: true, format: "yyyy-mm-dd"});
				control.className += "RCalendar";
				break;
			case CLASS_SPINBUTTON:
				control = document.createElement("div");
				var input = document.createElement("input");
				input.setAttribute("type", "text");
				input.id = "l" + RID;
				input.value = "0";
				input.className = "form-control REdit";
				control.ConceptVAdjustment = this.Controls[this.POST_OBJECT];

				if (control.ConceptVAdjustment)
					control.ConceptVAdjustment.Target = control;

				input.addEventListener('keypress', function(e) {
						e = e || window.event;
						var code = "";
						if (e.keyCode)
							code = e.keyCode;
						else 
						if (e.which)
							code = e.which;
						if ((code < 13) || (code == 109) || ((code >= 33) && (code <= 40)))
							return;

						var character = String.fromCharCode(code).toLowerCase();
						switch (character) {
							case "0":
							case "1":
							case "2":
							case "3":
							case "4":
							case "5":
							case "6":
							case "7":
							case "8":
							case "9":
							case ".":
							case ",":
							case "-":
								setTimeout(function () {
									if (input.value == "-")
										return;

									var val = parseFloat(input.value);
									if ((isNaN(val)) && (control.ConceptVAdjustment))
										input.value = control.ConceptVAdjustment.ConceptValue;
									else
									if (control.ConceptVAdjustment) {
										if (val > control.ConceptVAdjustment.ConceptUpper) {
											val = control.ConceptVAdjustment.ConceptUpper;
											input.value = val;
										}
										if (val < control.ConceptVAdjustment.ConceptLower) {
											val = control.ConceptVAdjustment.ConceptLower;
											input.value = val;
										}
										control.ConceptVAdjustment.ConceptValue = val;
									}
								}, 1);
								break;
							default:
								e.preventDefault();
						}
				}, false);

				var container = document.createElement("div");
				container.id = "c" + RID;
				container.className = "input-group-btn-vertical RSpinButtonContainer";
				var button = document.createElement("button");
				button.className = "btn btn-default RSpinButtonButton";
				button.innerHTML = '<i class="fa fa-caret-up"></i>';
				container.appendChild(button);

				var button2 = document.createElement("button");
				button2.className = "btn btn-default RSpinButtonButton";
				button2.innerHTML = '<i class="fa fa-caret-down"></i>'
				container.appendChild(button2);

				button.addEventListener('click', function(e) {
					var val = parseFloat(input.value);
					if ((isNaN(val)) && (control.ConceptVAdjustment))
						input.value = control.ConceptVAdjustment.ConceptValue;
					else
					if (control.ConceptVAdjustment) {
						val += control.ConceptVAdjustment.ConceptIncrement;
						if (val <= control.ConceptVAdjustment.ConceptUpper) {
							input.value = val;
							control.ConceptVAdjustment.ConceptValue = val;
						}
					}
				}, false);

				button2.addEventListener('click', function(e) {
					var val = parseFloat(input.value);
					if ((isNaN(val)) && (control.ConceptVAdjustment))
						input.value = control.ConceptVAdjustment.ConceptValue;
					else
					if (control.ConceptVAdjustment) {
						val -= control.ConceptVAdjustment.ConceptIncrement;
						if (val >= control.ConceptVAdjustment.ConceptLower) {
							input.value = val;
							control.ConceptVAdjustment.ConceptValue = val;
						}
					}
				}, false);

				control.appendChild(input);
				control.appendChild(container);
				control.className = "input-group spinner RSpinButton";
				break;
			case CLASS_PROPERTIESBOX:
				control = document.createElement("div");
				control.className = "RPropertiesBox";
				if (ptype != CLASS_SCROLLEDWINDOW)
					control.style.overflow = "auto";
				break;
			case CLASS_VSCROLLBAR:

				control = document.createElement("div");
				control.className = "RVScrollBar";
				var child = document.createElement("div");
				child.style.width="0.1px";
				child.style.height="200%";
				control.appendChild(child);
				control.ConceptVAdjustment = this.Controls[this.POST_OBJECT];
				if (control.ConceptVAdjustment) {
					control.ConceptVAdjustment.Target = control;
					control.ConceptVAdjustment.Vertical = true;
				}
				break;
			case CLASS_HSCROLLBAR:
				control = document.createElement("div");
				control.className = "RHScrollBar";
				var child = document.createElement("div");
				child.style.width="200%";
				child.style.height="0.1px";
				control.appendChild(child);
				control.ConceptHAdjustment = this.Controls[this.POST_OBJECT];
				if (control.ConceptHAdjustment) {
					control.ConceptHAdjustment.Target = control;
					control.ConceptHAdjustment.Vertical = false;
				}
				break;
			case CLASS_VSCALE:
				control = document.createElement("div");
				control.className = "RVScale";

				var edit = document.createElement("input");
				edit.id = "c" + RID;
				edit.setAttribute("type", "text");
				edit.setAttribute("data-slider-id", "s" + RID);
				edit.setAttribute("data-slider-min", "0");
				edit.setAttribute("data-slider-max", "100");
				edit.setAttribute("data-slider-step", "1");
				edit.setAttribute("data-slider-value", "0");
				control.appendChild(edit);
				$(edit).slider({orientation: "vertical"});
				$(edit).slider('setValue', 0);
				control.ConceptVAdjustment = this.Controls[this.POST_OBJECT];
				control.ConceptEdit = edit;
				if (control.ConceptVAdjustment) {
					control.ConceptVAdjustment.Target = control;
					control.ConceptVAdjustment.Vertical = true;
				}
				$(edit).slider().on('slide', function(ev) {
					control.ConceptVAdjustment.ConceptValue = ev.value;
					if (control.ConceptMoveSlider)
						self.SendMessage(control.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_MOVESLIDER, "" + control.ConceptVAdjustment.ConceptValue, 0);
				});
				break;
			case CLASS_HSCALE:
				control = document.createElement("div");
				control.className = "RHScale";

				var edit = document.createElement("input");
				edit.id = "c" + RID;
				edit.setAttribute("type", "text");
				edit.setAttribute("data-slider-id", "s" + RID);
				edit.setAttribute("data-slider-min", "0");
				edit.setAttribute("data-slider-max", "100");
				edit.setAttribute("data-slider-step", "1");
				edit.setAttribute("data-slider-value", "0");
				control.appendChild(edit);
				$(edit).slider({ });
				control.ConceptHAdjustment = this.Controls[this.POST_OBJECT];
				control.ConceptEdit = edit;	
				if (control.ConceptHAdjustment) {
					control.ConceptHAdjustment.Target = control;
					control.ConceptHAdjustment.Vertical = false;
				}

				$(edit).slider().on('slide', function(ev) {
					control.ConceptHAdjustment.ConceptValue = ev.value;
					if (control.ConceptMoveSlider)
						self.SendMessage(control.id.substring(1), MSG_EVENT_FIRED, "" + EVENT_ON_MOVESLIDER, "" + control.ConceptHAdjustment.ConceptValue, 0);
				});
				break;
			case CLASS_CLIENTCHART:
				if (!("charts" in js_flags)) {
					js_flags["charts"] = 1;
					google.charts.load('current', {'packages':['corechart', 'gauge', 'gantt', 'geochart', 'orgchart', 'table', 'timeline', 'treemap', 'wordtree', 'map']});
					google.charts.setOnLoadCallback(function() {
						js_flags["charts"] = 2;
						if ("charts_pending" in js_flags) {
							var pending = js_flags["charts_pending"];
							for (var i = 0; i < pending.length; i++)
								pending[i]();

							delete js_flags["charts_pending"];
						}
					});
				}
				control = document.createElement("div");
				control.className = "RChart";
				break;
			case CLASS_HTMLSNAP:
				control = document.createElement("div");
				control.className = "RSnap";
				var key = "snap." + this.POST_STRING;
				control.ConceptHTMLClass = this.POST_STRING;
				if (key in js_flags)
					control.innerHTML = js_flags[key];
				break;
			case CLASS_HTMLAPP:
				if (this.UIContainer)
					control = document.getElementById(this.UIContainer);
				else
					control = this.Container;
				control.ConceptClassID = CLASS_HTMLAPP;
				this.Controls[RID] = control;
				this.HTMLUIID = RID;
				// not an UI object !
				return;
			case 1000:
				control = document.createElement("pre");
				control.className = "RScintilla";
				control.ConceptMarkers = [ ];
				break;
			case 1012:
				control = document.createElement("iframe");
				//control.setAttribute("sandbox", "allow-same-origin");
				control.className = "RWebView";
				break;
			case 1015:
				control = document.createElement("video");
				var canvas = document.createElement("canvas");
				var canvasframe = document.createElement("canvas");
				control.ConceptCanvas = canvas;
				canvasframe.width = 640;
				canvasframe.height = 480;
				control.ConceptCanvasFrame = canvasframe;
				control.ConceptCanvasSmallFrame = document.createElement("canvas");
				var videoObj = { "video": true, "googEchoCancellation": true };

				navigator.getUserMedia	=	navigator.getUserMedia ||
								navigator.webkitGetUserMedia ||
								navigator.mozGetUserMedia ||
								navigator.msGetUserMedia;

				if (navigator.getUserMedia) { 
					navigator.getUserMedia(videoObj, function(stream) {
							if (window.URL)
								control.src = window.URL.createObjectURL(stream);
							if (navigator.webkitGetUserMedia)
								control.src = window.webkitURL.createObjectURL(stream);
							else
							if (navigator.mozGetUserMedia)
								control.src = window.URL.createObjectURL(stream);
							else
								control.src = stream;
							control.play();
						},
						function(error) {
							console.error("Video capture error: " + error.code); 
						}
					);
				} else {
					console.error('Webcam API is not supported in this browser');
				}
				break;
			case 1016:
				window.AudioContext = window.AudioContext || window.webkitAudioContext;
				try {
					if (!this.MasterAudioContext)
						this.MasterAudioContext = new AudioContext();
					control = { ref: this.MasterAudioContext };
					control.ConceptChannels = 2;
					control.ConceptClassID = 1016;
					control.ConceptBuffers = [];
					this.Controls[RID] = control;
				} catch (e) {
					console.error("Audio API is not supported in this browser");
					console.error(e);
				}
				// not an UI object !
				return;
			case 1017:
				control = new Player({webgl: false, useWorker: true, workerFile: res_prefix + "/js/avc.js"});
				control.ConceptClassID = 1017;
				control.canvas.className = "RVideoCanvas";

				var previewcontrol = document.createElement("video");
				previewcontrol.muted = true;
				control.ConceptPreviewControl = previewcontrol;
				control.ConceptQuality = 5;
				control.ConceptBitRate = 120000;
				control.ConceptCanvasSmallFrame = document.createElement("canvas");

				control.onPictureDecoded = function(frame, width, height, info) {
					if ((this.ConceptWidget) && (this.ConceptWidget.offsetWidth) && (width > 0) && (height > 0)) {
						var w_ratio = this.ConceptWidget.offsetWidth / width;
						var h_ratio = this.ConceptWidget.offsetHeight / height;
						var ratio = Math.min(w_ratio, h_ratio);
						this.canvas.style.width = "" + (width * ratio) + "px";
						this.canvas.style.height = "" + (height * ratio) + "px";						
					}
				}
				this.Controls[RID] = control;
				return;
			case 1018:
				window.SpeechRecognition = window.SpeechRecognition || window.webkitSpeechRecognition;
				try {
					control = new SpeechRecognition();
					control.ConceptClassID = 1018;
					control.continuous = true;
					control.interimResults = true;
					this.Controls[RID] = control;
				} catch (e) {
					console.error("Speech recognition is not supported in this browser");
				}
				// not an UI object !
				return;
			default:
				control = document.createElement("div");
				control.className = "RemoteObject";
				break;
		}

		if (control != null) {
			control.style.display = "none";
			control.id = "r" + RID;
			control.ConceptClassID = CLASS_ID;
			control.setAttribute("ConceptClassID", CLASS_ID);

			if (parent) {
				control.setAttribute("ConceptParentID", Parent);

				if (ptype == CLASS_BUTTON)
					parent.innerHTML = "";

				if (ptype == CLASS_NOTEBOOK) {
					var tabs = parent.ConceptTabs;
					if (!tabs)
						tabs = document.getElementById("t" + Parent);
					var pages = parent.ConceptPages;
					if (!pages)
						pages = document.getElementById("c" + Parent);

					if ((tabs) && (pages)) {
						var tab = document.createElement("li");
						tab.id = "l_tab" + RID;
						tab.setAttribute("ConceptParentID", Parent);

						tab.style.display = "none";
						tab.className = "RNotebookPageItem";

						var key = "t" + Parent + "_" + RID;

						var href = document.createElement("a");
						href.className = "RNotebookPageLink";
						href.href = "#"+key;

						href.innerHTML = "Page";
						href.id = "a_tab" + RID;
						href.setAttribute("role", "tab");
						href.setAttribute("data-toggle", "tab");

						href.onclick = function() {
							self.PageChanged(parent, tabs, tab);
							return false;
						}

						tab.appendChild(href);
						tabs.appendChild(tab);

						var page = document.createElement("div");
						page.id = key;
						page.className = "tab-pane RNotebookPage"
						page.appendChild(control);

						control.style.width = "100%";
						pages.appendChild(page);
					} else
						console.warn("Invalid notebook control encountered (#r" + Parent +")");
				} else
				if ((ptype == CLASS_EXPANDER) || (ptype == CLASS_FRAME)) {
					var content = parent.ConceptContainer;
					if (!content)
						content = document.getElementById("c" + Parent);
					control.style.width = "100%";
					if (content)
						content.appendChild(control);
				} else
				if (ptype == CLASS_TOOLBAR) {
					var group = document.getElementById("g" + Parent);
					if (group)
						group.appendChild(control);
				} else
				if (ptype == CLASS_MENUBAR) {
					var mitems = document.getElementById("i" + Parent);
					if (mitems)
						mitems.appendChild(control);
				} else
				if (CLASS_ID == CLASS_FORM) {
					this.Container.appendChild(control);
				} else
				if (ptype == CLASS_FORM) {
					var content = document.getElementById("c" + Parent);
					if (content) {
						content.appendChild(control);
					} else {
						control.style.height = "100%";
						parent.appendChild(control);
					}
				} else
				if ((ptype == CLASS_VPANED) || (ptype == CLASS_HPANED)) {
					if (parent.children.length == 1) {
						var divider = document.createElement("div");
						if (ptype == CLASS_VPANED)
							divider.className = "btn-primary RVDivider";
						else
							divider.className = "btn-primary RHDivider";
						divider.id = "d" + Parent;
						this.ConceptPrec = -1;
						this.ConceptPressed = false;

						divider.onmousedown = function(e) {
							this.ConceptPressed = true;
						}

						divider.onmouseup = function(e) {
							this.ConceptPressed = false;
							self.UpdateScrolledWindowSize(10);
						}

						parent.onmousemove = function(e) {
							if (divider.ConceptPressed) {
								var btn = e.buttons;
								if (typeof btn == "undefined")
									btn = e.which;
								if (btn) {
									if (parent.children.length >= 3) {
										var child = parent.children[0];
										var child2 = parent.children[2];
										if (ptype == CLASS_HPANED) {
											var w1 = e.clientX - divider.offsetWidth / 2 - child.offsetLeft;
											var w2 = parent.offsetWidth - e.clientX + divider.offsetWidth / 2 + child.offsetLeft;

											child.style.width = "" + w1 + "px";
											child2.style.width = "" + (parent.offsetWidth - e.clientX + divider.offsetWidth / 2 + child.offsetLeft) + "px";
										} else {
											var h1 = e.clientY - divider.offsetHeight / 2 - child.offsetTop;
											var h2 = parent.offsetHeight - e.clientY + divider.offsetHeight / 2 + child.offsetTop;

											if (child2.ConceptClassID == CLASS_NOTEBOOK) {
												if (child2.className.lastIndexOf('tabs-below') >= 0) {
													var tabs = document.getElementById(child2.id.replace("r", "t"));
													if ((tabs) && (tabs.offsetParent) && (tabs.offsetHeight))
														h2 -= tabs.offsetHeight;
												}
											}

											child.style.height = "" + h1 + "px";
											child2.style.height = "" + h2 + "px";
											if (!parent.AceBugTimer) {
												parent.AceBugTimer = setTimeout(function () {
													window.dispatchEvent(new Event('resize'));
													parent.AceBugTimer = null;
												}, 100);
											}

										}
									}
								} else {
									self.UpdateScrolledWindowSize(1);
									divider.ConceptPressed = false;
								}
							}
						}
						parent.appendChild(divider);
					}
					parent.appendChild(control);
					this.Packing(control, PACK_EXPAND_WIDGET);
					if (ptype == CLASS_VPANED) {
						control.style.width = "100%";
						control.style.height = "50%";
					} else
						control.style.width = "50%";
				} else
				if (ptype == CLASS_TABLE) {
					var left = parent.getAttribute("ConceptAttLeft");
					var right = parent.getAttribute("ConceptAttRight");
					var top = parent.getAttribute("ConceptAttTop");
					var bottom = parent.getAttribute("ConceptAttBottom");
					var vopt = parent.getAttribute("ConceptAttVOpt");
					var hopt = parent.getAttribute("ConceptAttHOpt");
					var vspacing = parent.getAttribute("ConceptAttVSpacing");
					var hspacing = parent.getAttribute("ConceptAttHSpacing");
					if (!left)
						left = 0;
					else
						left = parseInt(left);

					if (!top)
						top = 0;
					else
						top = parseInt(top);

					if (!right)
						right = left + 1;
					else
						right = parseInt(right);

					if (!bottom)
						bottom = top + 1;
					else
						bottom = parseInt(bottom);

					if (!hopt)
						hopt = 2;
					else
						hopt = parseInt(hopt);

					if (!vopt)
						vopt = 2;
					else
						vopt = parseInt(vopt);

					var colSpan = right - left;
					var rowSpan = bottom - top;
					var row = document.getElementById("row_" + Parent + "_" + top);
					if (!row) {
						row = document.createElement("tr");
						row.className = "RTableRow";
						row.id = "row_" + Parent + "_" + top;
						parent.appendChild(row);
					}
					if (rowSpan > 1) {
						if (!parent.InvisibleTD)
							parent.InvisibleTD = [ ];

						for (var ii = left; ii < right; ii++)
							parent.InvisibleTD [ii] = true;
					}
					if (row) {
						var cell = document.createElement("td");
						cell.setAttribute("ConceptOrder", "" + left);
						cell.className = "RTableCell";
						if (hopt != 2)
							control.style.width = "100%";
						//if ((CLASS_ID == CLASS_SCROLLEDWINDOW) || (CLASS_ID == CLASS_VPANED) || (CLASS_ID == CLASS_VBOX) || (CLASS_ID == CLASS_VBUTTONBOX)) {
						//	parent.style.height = "100%";
						//}
						if (vopt & 1) {
							control.style.height = "100%";
							if (CLASS_ID == CLASS_SCROLLEDWINDOW) {
								cell.style.height = "150px";
								//if (typeof control.ConceptHeight == "undefined")
								//	parent.style.height = "100%";
							}
						}

						var children = row.children;
						var leftCells = 0;
						var replace_node = null;
						if (children) {
							var len = children.length;
							for (var i = 0; i < len; i++) {
								var child = children[i];
								if (child) {
									var prevOrder = parseInt(child.getAttribute("ConceptOrder"));
									if (prevOrder >= left) {
										if (prevOrder == left)
											replace_node = child;
										break;
									}

									if (child.colSpan)
										leftCells += parseInt(child.colSpan);
									else
										leftCells ++;
								}
							}

							for (var i = leftCells; i < left; i++) {
								var node = document.createElement("td");
								if ((parent.InvisibleTD) && (parent.InvisibleTD [i]))
									node.style.display = "none";
								node.setAttribute("ConceptOrder", "" + i);
								row.appendChild(node);
							}
							
						}
						cell.appendChild(control);
						if (replace_node)
							row.replaceChild(cell, replace_node);
						else
							row.appendChild(cell);

						if (colSpan > 1) {
							cell.colSpan = "" + colSpan;
							if (replace_node) {
								children = row.children;
								if (children) {
									var consumed_cells = 1;
									var i = 0;
									while (i < children.length) {
										var child = children[i];
										if (child) {
											var order = parseInt(child.getAttribute("ConceptOrder"));
											if ((order > left) && (order < right))
												row.removeChild(child);
											else
												i++;
										} else
											break;
									}
								}
							}
							//cell.style.columnSpan = colSpan;
							//cell.style.webkitColumnSpan = colSpan;
							//cell.style.MozColumnSpan = colSpan;
							//cell.style.width = "100%";
						}
						if (rowSpan > 1)
							cell.rowSpan = "" + rowSpan;
					}
				} else
					parent.appendChild(control);
			} else
			if (CLASS_ID == CLASS_FORM) {
				control.style.width = "100%";
				this.Container.appendChild(control);
			} else
				this.NoParentContainer.appendChild(control);

			this.Controls[RID] = control;
			if ((ptype == CLASS_VBOX) || (ptype == CLASS_HBOX) ||
				(ptype == CLASS_VBUTTONBOX) || (ptype == CLASS_HBUTTONBOX) ||
				(ptype == CLASS_TOOLBAR) || (ptype == CLASS_MENUBAR)) {
				if (typeof parent.ConceptPackType != "undefined")
					packing = parent.ConceptPackType;
	
				if ((ptype != CLASS_VBUTTONBOX) && (ptype != CLASS_HBUTTONBOX))
					this.Packing(control, packing);
				else
					this.Packing(control, PACK_SHRINK);

				if ((ptype == CLASS_VBOX) || (ptype == CLASS_VBUTTONBOX)) {
					if (CLASS_ID == CLASS_TOOLBAR)
						control.style.width = "auto";
					else
						control.style.width = "100%";
					control.style.clear = "both";
				}
			} else
			if ((CLASS_ID == CLASS_TREEVIEW) && (ptype == CLASS_SCROLLEDWINDOW)) {
			//	control.style.width = "100%";
				if (parent.style.height != "100%")
					control.style.height = "0px";
			} else
			if ((ptype == CLASS_FORM) || (ptype == CLASS_HANDLEBOX) || (ptype == CLASS_FRAME) || (ptype == CLASS_ALIGNMENT) || 
				(ptype == CLASS_EVENTBOX) || (ptype == CLASS_BUTTON) || (ptype == CLASS_SCROLLEDWINDOW)) {
				if (CLASS_ID != CLASS_FORM)
					control.style.width = "100%";

				if ((ptype == CLASS_SCROLLEDWINDOW) && (CLASS_ID == CLASS_TEXTVIEW))
					control.style.height = "100%";
				if ((ptype == CLASS_SCROLLEDWINDOW) && (CLASS_ID == CLASS_TABLE)) {
					if (!parent.ConceptHeight)
						parent.style.height = "0px";
					//control.style.height = "0px";
				}
			} else
			if (ptype == CLASS_EXPANDER)
				control.style.height = "100%";
			if (CLASS_ID == 1000) {
				try {
					var editor = ace.edit(control.id);
					editor.setTheme("ace/theme/twilight");
					editor.getSession().setMode("ace/mode/concept");
					control.ConceptEditor = editor;
				} catch (exc) {
					console.warn(exc);
				}
			}
		}
	}
}

function HTMLUI(url, container, settings) {
	var loading = "";
	if ((settings) && (settings.loading))
		loading = settings.loading;
	var client = new ConceptClient(url, container, loading, true);
	if (settings) {
		if (settings.get)
			client.UIGet = settings.get;
		if (settings.set)
			client.UISet = settings.set;
		if (settings.run)
			client.UIRun = settings.run;
		if (settings.runcontainer)
			client.UIContainer = settings.runcontainer;
		if (settings.create)
			client.UICreate = settings.create;
	}
	return client;
}
//========================================================================================//
function ToBase64(arrayBuffer) {
	var base64    = '';
	var encodings = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';

	var bytes         = new Uint8Array(arrayBuffer);
	var byteLength    = bytes.byteLength;
	var byteRemainder = byteLength % 3;
	var mainLength    = byteLength - byteRemainder;

	var a, b, c, d;
	var chunk;

	for (var i = 0; i < mainLength; i = i + 3) {
		chunk = (bytes[i] << 16) | (bytes[i + 1] << 8) | bytes[i + 2];
		a = (chunk & 16515072) >> 18;
		b = (chunk & 258048)   >> 12;
		c = (chunk & 4032)     >>  6;
		d = chunk & 63;
		base64 += encodings[a] + encodings[b] + encodings[c] + encodings[d];
	}

	if (byteRemainder == 1) {
		chunk = bytes[mainLength];
		a = (chunk & 252) >> 2 // 252 = (2^6 - 1) << 2;
		b = (chunk & 3)   << 4 // 3   = 2^2 - 1;
		base64 += encodings[a] + encodings[b] + '==';
	} else
	if (byteRemainder == 2) {
		chunk = (bytes[mainLength] << 8) | bytes[mainLength + 1];
		a = (chunk & 64512) >> 10 // 64512 = (2^6 - 1) << 10;
		b = (chunk & 1008)  >>  4 // 1008  = (2^6 - 1) << 4;
		c = (chunk & 15)    <<  2 // 15    = 2^4 - 1;
		base64 += encodings[a] + encodings[b] + encodings[c] + '=';
	}
	return base64;
}
//========================================================================================//
function FromBase64(dataURI) {
	var BASE64_MARKER = ";base64,";
	var base64Index = dataURI.indexOf(BASE64_MARKER) + BASE64_MARKER.length;
	var base64 = dataURI.substring(base64Index);
	return window.atob(base64);
}
//========================================================================================//
function ToUTF8(str, len, index) {
	var val = "";
	var c, c2, c3;
	len += index;
	while (index < len) {
		c = str[index++];
		switch (c >> 4) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				val += String.fromCharCode(c);
				break;
			case 12:
			case 13:
				if (index < len) {
					c2 = str[index++];
					val += String.fromCharCode(((c & 0x1F) << 6) | (c2 & 0x3F));
				}
				break;
			case 14:
				if (index < len -1) {
					c2 = str[index++];
					c3 = str[index++];
					val += String.fromCharCode(((c & 0x0F) << 12) | ((c2 & 0x3F) << 6) | ((c3 & 0x3F) << 0));
				}
				break;
		}
	}
	return val;
}
//========================================================================================//
if (!ArrayBuffer.prototype.slice) {
	ArrayBuffer.prototype.slice = function (begin, end) {
		var len = this.byteLength;
		begin = (begin|0) || 0;
		end = end === (void 0) ? len : (end|0);

		if (begin < 0)
			begin = Math.max(begin + len, 0);
		if (end < 0)
			end = Math.max(end + len, 0);

	    	if (len === 0 || begin >= len || begin >= end)
      			return new ArrayBuffer(0);

		var length = Math.min(len - begin, end - begin);
		var target = new ArrayBuffer(length);
		var targetArray = new Uint8Array(target);
		targetArray.set(new Uint8Array(this, begin, length));
		return target;
	};
}
//========================================================================================//
var guid = (function() {
  function s4() {
    return Math.floor((1 + Math.random()) * 0x10000)
               .toString(16)
               .substring(1);
  }
  return function() {
    return s4() + s4() + '-' + s4() + '-' + s4() + '-' +
           s4() + '-' + s4() + s4() + s4();
  };
})();
//========================================================================================//
function getCookie(c_name) {
    var i,x,y,ARRcookies=document.cookie.split(";");

    for (i=0;i<ARRcookies.length;i++)
    {
        x=ARRcookies[i].substr(0,ARRcookies[i].indexOf("="));
        y=ARRcookies[i].substr(ARRcookies[i].indexOf("=")+1);
        x=x.replace(/^\s+|\s+$/g,"");
        if (x==c_name)
        {
            return unescape(y);
        }
     }
}
//========================================================================================//
function setCookie(cname, cvalue, exdays) {
    var d = new Date();
    d.setTime(d.getTime() + (exdays*24*60*60*1000));
    var expires = "expires="+d.toUTCString();
    document.cookie = cname + "=" + cvalue + "; " + expires;
}
//========================================================================================//
/**
 * http://www.openjs.com/scripts/events/keyboard_shortcuts/
 * Version : 2.01.B
 * By Binny V A
 * License : BSD
 */
shortcut = {
	'all_shortcuts':{},//All the shortcuts are stored in this array
	'add': function(shortcut_combination,callback,opt) {
		//Provide a set of default options
		var default_options = {
			'type':'keydown',
			'propagate':false,
			'disable_in_input':false,
			'target':document,
			'keycode':false
		}
		if(!opt) opt = default_options;
		else {
			for(var dfo in default_options) {
				if(typeof opt[dfo] == 'undefined') opt[dfo] = default_options[dfo];
			}
		}

		var ele = opt.target;
		if(typeof opt.target == 'string') ele = document.getElementById(opt.target);
		var ths = this;
		shortcut_combination = shortcut_combination.toLowerCase();

		//The function to be called at keypress
		var func = function(e) {
			e = e || window.event;
			var ae = opt['active_element'];

			if (!opt['ignore_visibility']) {
				if ((ae) && (ae.offsetParent == null))
					return;
			}

			if(opt['disable_in_input']) { //Don't enable shortcut keys in Input, Textarea fields
				var element;
				if(e.target) element=e.target;
				else if(e.srcElement) element=e.srcElement;
				if(element.nodeType==3) element=element.parentNode;

				if(element.tagName == 'INPUT' || element.tagName == 'TEXTAREA') return;
			}
	
			//Find Which key is pressed
			if (e.keyCode) code = e.keyCode;
			else if (e.which) code = e.which;
			var character = String.fromCharCode(code).toLowerCase();
			
			if(code == 188) character=","; //If the user presses , when the type is onkeydown
			if(code == 190) character="."; //If the user presses , when the type is onkeydown

			var keys = shortcut_combination.split("+");
			//Key Pressed - counts the number of valid keypresses - if it is same as the number of keys, the shortcut function is invoked
			var kp = 0;
			
			//Work around for stupid Shift key bug created by using lowercase - as a result the shift+num combination was broken
			var shift_nums = {
				"`":"~",
				"1":"!",
				"2":"@",
				"3":"#",
				"4":"$",
				"5":"%",
				"6":"^",
				"7":"&",
				"8":"*",
				"9":"(",
				"0":")",
				"-":"_",
				"=":"+",
				";":":",
				"'":"\"",
				",":"<",
				".":">",
				"/":"?",
				"\\":"|"
			}
			//Special Keys - and their codes
			var special_keys = {
				'esc':27,
				'escape':27,
				'tab':9,
				'space':32,
				'return':13,
				'enter':13,
				'backspace':8,
	
				'scrolllock':145,
				'scroll_lock':145,
				'scroll':145,
				'capslock':20,
				'caps_lock':20,
				'caps':20,
				'numlock':144,
				'num_lock':144,
				'num':144,
				
				'pause':19,
				'break':19,
				
				'insert':45,
				'home':36,
				'delete':46,
				'end':35,
				
				'pageup':33,
				'page_up':33,
				'pu':33,
	
				'pagedown':34,
				'page_down':34,
				'pd':34,
	
				'left':37,
				'up':38,
				'right':39,
				'down':40,
	
				'f1':112,
				'f2':113,
				'f3':114,
				'f4':115,
				'f5':116,
				'f6':117,
				'f7':118,
				'f8':119,
				'f9':120,
				'f10':121,
				'f11':122,
				'f12':123
			}
	
			var modifiers = { 
				shift: { wanted:false, pressed:false},
				ctrl : { wanted:false, pressed:false},
				alt  : { wanted:false, pressed:false},
				meta : { wanted:false, pressed:false}	//Meta is Mac specific
			};
                        
			if(e.ctrlKey)	modifiers.ctrl.pressed = true;
			if(e.shiftKey)	modifiers.shift.pressed = true;
			if(e.altKey)	modifiers.alt.pressed = true;
			if(e.metaKey)   modifiers.meta.pressed = true;
                        
			for(var i=0; k=keys[i],i<keys.length; i++) {
				//Modifiers
				if(k == 'ctrl' || k == 'control') {
					kp++;
					modifiers.ctrl.wanted = true;

				} else if(k == 'shift') {
					kp++;
					modifiers.shift.wanted = true;

				} else if(k == 'alt') {
					kp++;
					modifiers.alt.wanted = true;
				} else if(k == 'meta') {
					kp++;
					modifiers.meta.wanted = true;
				} else if(k.length > 1) { //If it is a special key
					if(special_keys[k] == code) kp++;
					
				} else if(opt['keycode']) {
					if(opt['keycode'] == code) kp++;

				} else { //The special keys did not match
					if(character == k) kp++;
					else {
						if(shift_nums[character] && e.shiftKey) { //Stupid Shift key bug created by using lowercase
							character = shift_nums[character]; 
							if(character == k) kp++;
						}
					}
				}
			}
			
			if(kp == keys.length && 
						modifiers.ctrl.pressed == modifiers.ctrl.wanted &&
						modifiers.shift.pressed == modifiers.shift.wanted &&
						modifiers.alt.pressed == modifiers.alt.wanted &&
						modifiers.meta.pressed == modifiers.meta.wanted) {
				callback(e);
	
				if(!opt['propagate']) { //Stop the event
					//e.cancelBubble is supported by IE - this will kill the bubbling process.
					e.cancelBubble = true;
					e.returnValue = false;
	
					//e.stopPropagation works in Firefox.
					if (e.stopPropagation) {
						e.stopPropagation();
						e.preventDefault();
					}
					return false;
				}
			}
		}
		this.all_shortcuts[shortcut_combination] = {
			'callback':func, 
			'target':ele, 
			'event': opt['type']
		};
		//Attach the function with the event
		if(ele.addEventListener) ele.addEventListener(opt['type'], func, false);
		else if(ele.attachEvent) ele.attachEvent('on'+opt['type'], func);
		else ele['on'+opt['type']] = func;
	},

	//Remove the shortcut - just specify the shortcut and I will remove the binding
	'remove':function(shortcut_combination) {
		shortcut_combination = shortcut_combination.toLowerCase();
		var binding = this.all_shortcuts[shortcut_combination];
		delete(this.all_shortcuts[shortcut_combination])
		if(!binding) return;
		var type = binding['event'];
		var ele = binding['target'];
		var callback = binding['callback'];

		if(ele.detachEvent) ele.detachEvent('on'+type, callback);
		else if(ele.removeEventListener) ele.removeEventListener(type, callback, false);
		else ele['on'+type] = false;
	}
}

function strip(html) {
   var tmp = document.createElement("DIV");
   tmp.innerHTML = html;
   return tmp.textContent || tmp.innerText || "";
}
//========================================================================================//
